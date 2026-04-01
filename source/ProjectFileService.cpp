#include <algorithm>
#include <cmath>
#include <limits>

#include "ProjectFileService.h"

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
            auto trackNode = tracksNode.getChild(i);
            tracks.add({
                trackNode["name"].toString(),
                (bool) trackNode["armed"],
                (bool) trackNode["muted"],
                (bool) trackNode["solo"],
                (float) (double) trackNode["volume"],
                (float) (double) trackNode["pan"]
            });
        }
    }

    if (auto clipsNode = projectTree.getChildWithName("Clips"); clipsNode.isValid())
    {
        for (int i = 0; i < clipsNode.getNumChildren(); ++i)
        {
            const auto clipNode = clipsNode.getChild(i);
            const int trackIndex = clipNode.hasProperty("trackIndex")
                ? (int) (double) clipNode["trackIndex"]
                : 0;

            const double rawStartBeat = clipNode.hasProperty("startBeat")
                ? (double) clipNode["startBeat"]
                : 0.0;
            const double startBeat = std::isfinite(rawStartBeat) ? rawStartBeat : 0.0;

            double lengthBeats = clipNode.hasProperty("lengthBeats")
                ? (double) clipNode["lengthBeats"]
                : std::numeric_limits<double>::quiet_NaN();
            const double endBeat = clipNode.hasProperty("endBeat")
                ? (double) clipNode["endBeat"]
                : std::numeric_limits<double>::quiet_NaN();

            if (! std::isfinite(lengthBeats) && std::isfinite(endBeat))
                lengthBeats = endBeat - startBeat;

            if (! std::isfinite(lengthBeats))
                lengthBeats = 1.0;

            clips.add({
                trackIndex,
                startBeat,
                lengthBeats,
                clipNode["name"].toString(),
                clipNode["filePath"].toString()
            });
        }
    }

    const double tempoRaw = (double) projectTree.getProperty("tempo", 120.0);
    const double tempoValue = std::isfinite(tempoRaw) ? tempoRaw : 120.0;

    const double playheadRaw = (double) projectTree.getProperty("playheadBeats", 0.0);
    const double playheadBeats = std::isfinite(playheadRaw) ? playheadRaw : 0.0;
    const int requestedTrackIndex = (int) projectTree.getProperty("selectedTrackIndex", 0);

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
    transportState.setTempo((float) tempoValue);
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
        clipNode.setProperty("name", clip.name, nullptr);
        clipNode.setProperty("filePath", clip.filePath, nullptr);
        clipNode.setProperty("endBeat", clip.startBeat + clip.lengthBeats, nullptr);
        clipsNode.appendChild(clipNode, nullptr);
    }

    project.appendChild(tracksNode, nullptr);
    project.appendChild(clipsNode, nullptr);
    return project;
}
