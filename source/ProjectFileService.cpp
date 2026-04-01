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
            auto clipNode = clipsNode.getChild(i);
            clips.add({
                (int) clipNode["trackIndex"],
                (double) clipNode["startBeat"],
                (double) clipNode["lengthBeats"],
                clipNode["name"].toString(),
                clipNode["filePath"].toString()
            });
        }
    }

    trackEngine.replaceProjectData(tracks, clips, (int) projectTree["selectedTrackIndex"]);
    transportState.setTempo((float) (double) projectTree["tempo"]);
    transportState.setPlaying(false);
    transportState.setRecording(false);
    transportState.setPlayheadBeats((double) projectTree["playheadBeats"]);
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

    for (int i = 0; i < trackEngine.getClipCount(); ++i)
    {
        const auto& clip = trackEngine.getClip(i);
        juce::ValueTree clipNode("Clip");
        clipNode.setProperty("trackIndex", clip.trackIndex, nullptr);
        clipNode.setProperty("startBeat", clip.startBeat, nullptr);
        clipNode.setProperty("lengthBeats", clip.lengthBeats, nullptr);
        clipNode.setProperty("name", clip.name, nullptr);
        clipNode.setProperty("filePath", clip.filePath, nullptr);
        clipsNode.appendChild(clipNode, nullptr);
    }

    project.appendChild(tracksNode, nullptr);
    project.appendChild(clipsNode, nullptr);
    return project;
}
