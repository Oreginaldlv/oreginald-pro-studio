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

    TrackEngine()
    {
        tracks.add ({ "Drums", false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Bass",  false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Keys",  false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Lead",  false, false, false, 0.8f, 0.0f });
        tracks.add ({ "Vox",   false, false, false, 0.8f, 0.0f });
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

private:
    juce::Array<Track> tracks;
    int selectedTrackIndex { 0 };
};