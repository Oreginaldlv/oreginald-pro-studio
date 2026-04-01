#pragma once
#include <JuceHeader.h>

class TransportState
{
public:
    void setPlaying (bool shouldPlay)
    {
        playing = shouldPlay;
    }

    bool isPlaying() const noexcept
    {
        return playing;
    }

    void setRecording (bool shouldRecord)
    {
        recording = shouldRecord;
    }

    bool isRecording() const noexcept
    {
        return recording;
    }

    void setTempo (float newTempo)
    {
        tempo = juce::jlimit (40.0f, 240.0f, newTempo);
    }

    float getTempo() const noexcept
    {
        return tempo;
    }

private:
    bool playing { false };
    bool recording { false };
    float tempo { 120.0f };
};