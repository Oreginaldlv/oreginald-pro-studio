#pragma once
#include <cmath>
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
        playheadBeats = wrapToLoop(playheadBeats);
    }

    double getPlayheadBeats() const noexcept
    {
        return playheadBeats;
    }

    void setPlayheadBeats (double newBeats)
    {
        playheadBeats = juce::jmax (0.0, newBeats);
        playheadBeats = wrapToLoop(playheadBeats);
        playheadAnchorBeats = playheadBeats;

        if (playing)
            lastStartTimeMs = juce::Time::getMillisecondCounterHiRes();
    }

    void setPlayheadBeatsDirect(double newBeats)
    {
        playheadBeats = juce::jmax(0.0, newBeats);
        playheadBeats = wrapToLoop(playheadBeats);
        playheadAnchorBeats = playheadBeats;
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

    bool isLoopEnabled() const noexcept { return loopEnabled; }
    double getLoopStartBeats() const noexcept { return loopStartBeats; }
    double getLoopEndBeats() const noexcept { return loopEndBeats; }
    int getBeatsPerBar() const noexcept { return beatsPerBar; }
    bool isMetronomeEnabled() const noexcept { return metronomeEnabled; }

    void setLoopEnabled(bool enabled)
    {
        loopEnabled = enabled;
    }

    void setMetronomeEnabled(bool enabled)
    {
        metronomeEnabled = enabled;
    }

    void setLoopStartBeats(double beats)
    {
        loopStartBeats = juce::jmax(0.0, beats);
        ensureLoopOrder();
    }

    void setLoopEndBeats(double beats)
    {
        loopEndBeats = juce::jmax(loopStartBeats + minLoopLength, beats);
        ensureLoopOrder();
    }

    void setLoopRange(double startBeats, double endBeats)
    {
        loopStartBeats = juce::jmax(0.0, startBeats);
        loopEndBeats = juce::jmax(loopStartBeats + minLoopLength, endBeats);
        ensureLoopOrder();
    }

    double wrapToLoop(double beats) const noexcept
    {
        if (! loopEnabled)
            return beats;

        const double range = loopEndBeats - loopStartBeats;
        if (range <= 0.0)
            return loopStartBeats;

        if (beats < loopStartBeats)
            beats = loopStartBeats;

        if (beats >= loopEndBeats)
        {
            double offset = std::fmod(beats - loopStartBeats, range);
            if (offset < 0.0)
                offset += range;
            beats = loopStartBeats + offset;
        }

        return beats;
    }

private:

    void ensureLoopOrder()
    {
        if (loopEndBeats <= loopStartBeats)
            loopEndBeats = loopStartBeats + minLoopLength;
    }

    bool playing { false };
    bool recording { false };

    float tempo { 120.0f };

    double lastStartTimeMs { 0.0 };
    double playheadBeats { 0.0 };
    double playheadAnchorBeats { 0.0 };

    bool loopEnabled { false };
    double loopStartBeats { 0.0 };
    double loopEndBeats { 4.0 };
    bool metronomeEnabled { false };

    int beatsPerBar { 4 };
    static constexpr double minLoopLength { 1.0 };
};
