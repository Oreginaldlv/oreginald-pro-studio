#pragma once
#include <JuceHeader.h>

class TransportState
{
public:
    enum class Mode
    {
        stopped,
        playing
    };

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void transportStateChanged() = 0;
    };

    void addListener (Listener* listener)      { listeners.add (listener); }
    void removeListener (Listener* listener)   { listeners.remove (listener); }

    Mode getMode() const noexcept              { return mode; }
    bool isPlaying() const noexcept            { return mode == Mode::playing; }
    bool isRecordArmed() const noexcept        { return recordArmed; }
    bool isLoopEnabled() const noexcept        { return loopEnabled; }
    bool isMetronomeEnabled() const noexcept   { return metronomeEnabled; }

    double getBpm() const noexcept             { return bpm; }
    double getPlayheadBeats() const noexcept   { return playheadBeats; }

    void play()
    {
        if (mode != Mode::playing)
        {
            mode = Mode::playing;
            notifyListeners();
        }
    }

    void stop()
    {
        if (mode != Mode::stopped)
        {
            mode = Mode::stopped;
            notifyListeners();
        }
    }

    void togglePlay()
    {
        isPlaying() ? stop() : play();
    }

    void setRecordArmed (bool shouldBeArmed)
    {
        if (recordArmed != shouldBeArmed)
        {
            recordArmed = shouldBeArmed;
            notifyListeners();
        }
    }

    void toggleRecordArmed()
    {
        setRecordArmed (! recordArmed);
    }

    void setLoopEnabled (bool shouldBeEnabled)
    {
        if (loopEnabled != shouldBeEnabled)
        {
            loopEnabled = shouldBeEnabled;
            notifyListeners();
        }
    }

    void setMetronomeEnabled (bool shouldBeEnabled)
    {
        if (metronomeEnabled != shouldBeEnabled)
        {
            metronomeEnabled = shouldBeEnabled;
            notifyListeners();
        }
    }

    void setBpm (double newBpm)
    {
        newBpm = juce::jlimit (40.0, 240.0, newBpm);

        if (! juce::approximatelyEqual (bpm, newBpm))
        {
            bpm = newBpm;
            notifyListeners();
        }
    }

    void nudgeBpm (double delta)
    {
        setBpm (bpm + delta);
    }

    void setPlayheadBeats (double newPlayheadBeats)
    {
        newPlayheadBeats = juce::jmax (0.0, newPlayheadBeats);

        if (! juce::approximatelyEqual (playheadBeats, newPlayheadBeats))
        {
            playheadBeats = newPlayheadBeats;
            notifyListeners();
        }
    }

    void resetPlayhead()
    {
        setPlayheadBeats (0.0);
    }

    void advance (double deltaSeconds)
    {
        if (! isPlaying() || deltaSeconds <= 0.0)
            return;

        const auto beatsPerSecond = bpm / 60.0;
        setPlayheadBeats (playheadBeats + (deltaSeconds * beatsPerSecond));
    }

private:
    void notifyListeners()
    {
        listeners.call ([] (Listener& l) { l.transportStateChanged(); });
    }

    Mode mode { Mode::stopped };
    bool recordArmed { false };
    bool loopEnabled { false };
    bool metronomeEnabled { false };

    double bpm { 120.0 };
    double playheadBeats { 0.0 };

    juce::ListenerList<Listener> listeners;
};