#include <algorithm>
#include <cmath>
#include <limits>

#include "ProjectFileService.h"

namespace
{
    juce::String getTrackName(const juce::ValueTree& node, int index)
    {
        auto name = node.getProperty("name", juce::String()).toString().trim();
        if (name.isEmpty())
            return "Track " + juce::String(index + 1);

        return name;
    }

    juce::String getClipName(const juce::ValueTree& node, int index)
    {
        auto name = node.getProperty("name", juce::String()).toString().trim();
        if (name.isEmpty())
            return "Clip " + juce::String(index + 1);

        return name;
    }

    template <typename T>
    T clamp(const T value, const T minimum, const T maximum)
    {
        return juce::jlimit(minimum, maximum, value);
    }
}

bool ProjectFileService::saveProject(const juce::File& projectFile,
                                     const TrackEngine& trackEngine,
                                     const TransportState& transportState) const
{
    auto projectTree = createProjectTree(trackEngine, transportState);
    std::unique_ptr<juce::XmlElement> xml(projectTree.createXml());

    if (xml == nullptr)
        return false;

    if (! projectFile.getParentDirectory().exists())
        projectFile.getParentDirectory().createDirectory();

    return xml->writeTo(projectFile);
}

bool ProjectFileService::loadProject(const juce::File& projectFile,
                                     TrackEngine& trackEngine,
                                     TransportState& transportState) const
{
    if (! projectFile.existsAsFile())
        return false;

    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(projectFile));

    if (xml == nullptr)
        return false;

    auto projectTree = juce::ValueTree::fromXml(*xml);

    if (! projectTree.isValid() || ! projectTree.hasType("OPSProject"))
        return false;

    juce::Array<TrackEngine::Track> tracks;
    juce::Array<TrackEngine::Clip> clips;

    if (auto tracksNode = projectTree.getChildWithName("Tracks"); tracksNode.isValid())
    {
        for (int i = 0; i < tracksNode.getNumChildren(); ++i)
        {
            const auto trackNode = tracksNode.getChild(i);
            TrackEngine::Track track;

            track.name = getTrackName(trackNode, i);
            track.armed = (bool) trackNode.getProperty("armed", false);
            track.muted = (bool) trackNode.getProperty("muted", false);
            track.solo = (bool) trackNode.getProperty("solo", false);
            track.volume = clamp((float) (double) trackNode.getProperty("volume", track.volume), 0.0f, 1.5f);
            track.pan = clamp((float) (double) trackNode.getProperty("pan", track.pan), -1.0f, 1.0f);

            tracks.add(track);
        }
    }

    if (auto clipsNode = projectTree.getChildWithName("Clips"); clipsNode.isValid())
    {
        for (int i = 0; i < clipsNode.getNumChildren(); ++i)
        {
            const auto clipNode = clipsNode.getChild(i);
            TrackEngine::Clip clip;

            clip.trackIndex = (int) (double) clipNode.getProperty("trackIndex", clip.trackIndex);
            const double startBeatRaw = (double) clipNode.getProperty("startBeat", 0.0);
            const double startBeat = std::isfinite(startBeatRaw) ? startBeatRaw : 0.0;

            double lengthBeats = (double) clipNode.getProperty("lengthBeats",
                                                               std::numeric_limits<double>::quiet_NaN());
            const double endBeat = (double) clipNode.getProperty("endBeat",
                                                                 std::numeric_limits<double>::quiet_NaN());

            if (! std::isfinite(lengthBeats) && std::isfinite(endBeat))
                lengthBeats = endBeat - startBeat;

            if (! std::isfinite(lengthBeats))
                lengthBeats = 1.0;

            clip.startBeat = juce::jmax(0.0, startBeat);
            clip.lengthBeats = juce::jmax(1.0, lengthBeats);
            clip.name = getClipName(clipNode, i);
            clip.filePath = clipNode.getProperty("filePath", juce::String()).toString();

            clips.add(clip);
        }
    }

    const double tempoRaw = (double) projectTree.getProperty("tempo", transportState.getTempo());
    const double tempo = std::isfinite(tempoRaw) ? tempoRaw : 120.0;

    const double playheadRaw = (double) projectTree.getProperty("playheadBeats", transportState.getPlayheadBeats());
    const double playheadBeats = std::isfinite(playheadRaw) ? playheadRaw : 0.0;

    const int requestedTrackIndex = (int) projectTree.getProperty("selectedTrackIndex", trackEngine.getSelectedTrackIndex());
    const int trackCount = juce::jmax(1, tracks.size());
    const int sanitizedTrackIndex = juce::jlimit(0, trackCount - 1, requestedTrackIndex);

    for (auto& clip : clips)
    {
        clip.trackIndex = juce::jlimit(0, trackCount - 1, clip.trackIndex);
        clip.startBeat = juce::jmax(0.0, clip.startBeat);
        clip.lengthBeats = juce::jmax(1.0, clip.lengthBeats);
    }

    std::sort(clips.begin(), clips.end(), [](const TrackEngine::Clip& lhs, const TrackEngine::Clip& rhs)
    {
        if (lhs.trackIndex != rhs.trackIndex)
            return lhs.trackIndex < rhs.trackIndex;

        return lhs.startBeat < rhs.startBeat;
    });

    trackEngine.replaceProjectData(tracks, clips, sanitizedTrackIndex);
    transportState.setTempo((float) tempo);
    transportState.setPlaying(false);
    transportState.setRecording(false);
    transportState.setPlayheadBeats(juce::jmax(0.0, playheadBeats));

    return true;
}

juce::ValueTree ProjectFileService::createProjectTree(const TrackEngine& trackEngine,
                                                      const TransportState& transportState) const
{
    juce::ValueTree project("OPSProject");
    project.setProperty("tempo", transportState.getTempo(), nullptr);
    project.setProperty("playheadBeats", transportState.getPlayheadBeats(), nullptr);
    project.setProperty("selectedTrackIndex", trackEngine.getSelectedTrackIndex(), nullptr);

    juce::ValueTree tracksNode("Tracks");

    for (int i = 0; i < trackEngine.getNumTracks(); ++i)
    {
        const auto& track = trackEngine.getTrack(i);
        juce::ValueTree trackNode("Track");
        trackNode.setProperty("name", track.name, nullptr);
        trackNode.setProperty("armed", track.armed, nullptr);
        trackNode.setProperty("muted", track.muted, nullptr);
        trackNode.setProperty("solo", track.solo, nullptr);
        trackNode.setProperty("volume", track.volume, nullptr);
        trackNode.setProperty("pan", track.pan, nullptr);
        tracksNode.appendChild(trackNode, nullptr);
    }

    juce::ValueTree clipsNode("Clips");
    auto sortedClips = trackEngine.getClips();

    std::sort(sortedClips.begin(), sortedClips.end(), [](const TrackEngine::Clip& lhs,
                                                         const TrackEngine::Clip& rhs)
    {
        if (lhs.trackIndex != rhs.trackIndex)
            return lhs.trackIndex < rhs.trackIndex;

        return lhs.startBeat < rhs.startBeat;
    });

    for (const auto& clip : sortedClips)
    {
        juce::ValueTree clipNode("Clip");
        clipNode.setProperty("trackIndex", clip.trackIndex, nullptr);
        clipNode.setProperty("startBeat", clip.startBeat, nullptr);
        clipNode.setProperty("lengthBeats", clip.lengthBeats, nullptr);
        clipNode.setProperty("endBeat", clip.startBeat + clip.lengthBeats, nullptr);
        clipNode.setProperty("name", clip.name, nullptr);
        clipNode.setProperty("filePath", clip.filePath, nullptr);
        clipsNode.appendChild(clipNode, nullptr);
    }

    project.appendChild(tracksNode, nullptr);
    project.appendChild(clipsNode, nullptr);
    return project;
}
