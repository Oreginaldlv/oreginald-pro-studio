#pragma once
#include <JuceHeader.h>

class TransportState
{
public:

    void setPlaying (bool shouldPlay)
    {
        if (shouldPlay && !playing)
            lastStartTimeMs = juce::Time::getMillisecondCounterHiRes();

        if (!shouldPlay)
            playheadBeats = 0.0;

        playing = shouldPlay;
    }

    bool isPlaying() const noexcept { return playing; }

    void setRecording (bool shouldRecord) { recording = shouldRecord; }
    bool isRecording() const noexcept { return recording; }

    void setTempo (float newTempo)
    {
        tempo = juce::jlimit (40.0f, 240.0f, newTempo);
    }

    float getTempo() const noexcept { return tempo; }

    void updatePlayhead()
    {
        if (!playing)
            return;

        const auto nowMs = juce::Time::getMillisecondCounterHiRes();
        const auto elapsedMs = nowMs - lastStartTimeMs;

        playheadBeats = playheadAnchorBeats
            + (elapsedMs / 60000.0) * tempo;
    }

    double getPlayheadBeats() const noexcept
    {
        return playheadBeats;
    }

    void setPlayheadBeats (double newBeats)
    {
        playheadBeats = juce::jmax (0.0, newBeats);
        playheadAnchorBeats = playheadBeats;

        if (playing)
            lastStartTimeMs = juce::Time::getMillisecondCounterHiRes();
    }

    int getCurrentBar() const noexcept
    {
        return (int)(playheadBeats / beatsPerBar) + 1;
    }

    int getCurrentBeat() const noexcept
    {
        return ((int)playheadBeats % beatsPerBar) + 1;
    }

    juce::String getBarBeatString() const
    {
        return juce::String(getCurrentBar())
            + ":"
            + juce::String(getCurrentBeat());
    }

private:

    bool playing { false };
    bool recording { false };

    float tempo { 120.0f };

    double lastStartTimeMs { 0.0 };
    double playheadBeats { 0.0 };
    double playheadAnchorBeats { 0.0 };

    int beatsPerBar { 4 };
};