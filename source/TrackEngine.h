#pragma once
#include <JuceHeader.h>

class TrackEngine
{
public:
    struct Track
    {
        juce::String name;
        bool armed { false };
        bool muted { false };
        bool solo  { false };
        float volume { 0.8f };
        float pan { 0.0f };
    };

    struct Clip
    {
        int trackIndex { 0 };
        double startBeat { 0.0 };
        double lengthBeats { 1.0 };
        juce::String name;
        juce::String filePath;
    };

    TrackEngine()
    {
        tracks.add ({ "Drums", false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Bass",  false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Keys",  false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Lead",  false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Vox",   false, false, false, 0.8f, 0.0f });

        clips.add({ 0, 0.5, 2.0, "Drum Loop A", {} });
        clips.add({ 1, 1.5, 2.0, "Bass Verse", {} });
        clips.add({ 2, 2.0, 1.5, "Keys Pad", {} });
        clips.add({ 3, 2.5, 1.2, "Lead Hook", {} });
        clips.add({ 4, 3.0, 1.6, "Vox Main", {} });
    }

    int getNumTracks() const noexcept
    {
        return tracks.size();
    }

    const Track& getTrack (int index) const
    {
        jassert (juce::isPositiveAndBelow (index, tracks.size()));
        return tracks.getReference (index);
    }

    Track& getTrack (int index)
    {
        jassert (juce::isPositiveAndBelow (index, tracks.size()));
        return tracks.getReference (index);
    }

    int getSelectedTrackIndex() const noexcept
    {
        return selectedTrackIndex;
    }

    void setSelectedTrackIndex (int index)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            selectedTrackIndex = index;
    }

    void setTrackArmed (int index, bool shouldBeArmed)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).armed = shouldBeArmed;
    }

    void toggleTrackArmed (int index)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).armed = ! tracks.getReference (index).armed;
    }

    void setTrackMuted (int index, bool shouldBeMuted)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).muted = shouldBeMuted;
    }

    void toggleTrackMuted (int index)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).muted = ! tracks.getReference (index).muted;
    }

    void setTrackSolo (int index, bool shouldBeSolo)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).solo = shouldBeSolo;
    }

    void toggleTrackSolo (int index)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).solo = ! tracks.getReference (index).solo;
    }

    void setTrackVolume (int index, float newVolume)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).volume = juce::jlimit (0.0f, 1.5f, newVolume);
    }

    void setTrackPan (int index, float newPan)
    {
        if (juce::isPositiveAndBelow (index, tracks.size()))
            tracks.getReference (index).pan = juce::jlimit (-1.0f, 1.0f, newPan);
    }

    bool anyTrackSoloed() const
    {
        for (const auto& track : tracks)
            if (track.solo)
                return true;

        return false;
    }

    bool isTrackAudible (int index) const
    {
        if (! juce::isPositiveAndBelow (index, tracks.size()))
            return false;

        const auto& track = tracks.getReference (index);

        if (track.muted)
            return false;

        if (anyTrackSoloed())
            return track.solo;

        return true;
    }

    float getLeftGain (int index) const
    {
        const auto& track = getTrack (index);
        const float p = juce::jlimit (-1.0f, 1.0f, track.pan);
        return track.volume * (p <= 0.0f ? 1.0f : 1.0f - p);
    }

    float getRightGain (int index) const
    {
        const auto& track = getTrack (index);
        const float p = juce::jlimit (-1.0f, 1.0f, track.pan);
        return track.volume * (p >= 0.0f ? 1.0f : 1.0f + p);
    }

    int getClipCount() const noexcept
    {
        return clips.size();
    }

    const Clip& getClip (int index) const
    {
        jassert (juce::isPositiveAndBelow (index, clips.size()));
        return clips.getReference (index);
    }

    Clip& getClip (int index)
    {
        jassert (juce::isPositiveAndBelow (index, clips.size()));
        return clips.getReference (index);
    }

    const juce::Array<Clip>& getClips() const noexcept
    {
        return clips;
    }

    void setClipStart (int index, double startBeat)
    {
        if (! juce::isPositiveAndBelow (index, clips.size()))
            return;

        clips.getReference(index).startBeat = juce::jmax (0.0, startBeat);
    }

    void setClipLength (int index, double lengthBeats)
    {
        if (! juce::isPositiveAndBelow (index, clips.size()))
            return;

        clips.getReference(index).lengthBeats = juce::jmax (minClipLength, lengthBeats);
    }

    int addClip (int trackIndex, double startBeat, double lengthBeats, const juce::String& name, const juce::String& filePath = {})
    {
        if (! juce::isPositiveAndBelow (trackIndex, tracks.size()))
            return -1;

        clips.add ({ trackIndex, juce::jmax (0.0, startBeat), juce::jmax (minClipLength, lengthBeats), name, filePath });
        return clips.size() - 1;
    }

    bool duplicateClip (int index)
    {
        if (! juce::isPositiveAndBelow (index, clips.size()))
            return false;

        auto copy = clips.getReference (index);
        copy.startBeat += copy.lengthBeats;
        copy.name = copy.name + " Copy";
        clips.insert (index + 1, copy);
        return true;
    }

    bool removeClip (int index)
    {
        if (! juce::isPositiveAndBelow (index, clips.size()))
            return false;

        clips.remove (index);
        return true;
    }

    void clearClips()
    {
        clips.clear();
    }

    void clearTracks()
    {
        tracks.clear();
        selectedTrackIndex = 0;
    }

    void addTrack (const juce::String& name,
                   bool armed = false,
                   bool muted = false,
                   bool solo = false,
                   float volume = 0.8f,
                   float pan = 0.0f)
    {
        tracks.add ({ name, armed, muted, solo, juce::jlimit (0.0f, 1.5f, volume), juce::jlimit (-1.0f, 1.0f, pan) });
    }

    void replaceProjectData (const juce::Array<Track>& newTracks,
                             const juce::Array<Clip>& newClips,
                             int newSelectedTrackIndex)
    {
        tracks = newTracks;
        clips = newClips;

        if (tracks.isEmpty())
            tracks.add ({ "Track 1", false, false, false, 0.8f, 0.0f });

        selectedTrackIndex = juce::jlimit (0, tracks.size() - 1, newSelectedTrackIndex);
    }

private:
    juce::Array<Track> tracks;
    int selectedTrackIndex { 0 };
    juce::Array<Clip> clips;
    static constexpr double minClipLength { 1.0 };
};
