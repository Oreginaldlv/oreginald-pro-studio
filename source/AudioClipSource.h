#pragma once
#include <JuceHeader.h>

class AudioClipSource
{
public:
    AudioClipSource(int clipTrackIndex,
                    double clipStartBeat,
                    double clipLengthBeats,
                    const juce::String& clipName,
                    const juce::String& clipFilePath);

    void prepare(double newSampleRate);
    void render(juce::AudioSampleBuffer& buffer,
                int numSamples,
                double blockStartBeat,
                double samplesPerBeat,
                float gain,
                float pan);

    double getStartBeat() const noexcept { return startBeat; }
    double getEndBeat() const noexcept { return startBeat + lengthBeats; }

private:
    void loadAudioFile();

    int trackIndex { 0 };
    double startBeat { 0.0 };
    double lengthBeats { 0.0 };
    juce::String name;
    juce::String filePath;
    juce::AudioBuffer<float> audioData;
    bool hasAudioFile { false };
    double sampleRate { 44100.0 };
    float frequency { 220.0f };
    double phaseIncrement { 0.0 };
};
