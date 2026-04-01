#pragma once
#include <vector>
#include <memory>
#include <juce_audio_basics/juce_audio_basics.h>
#include "TrackEngine.h"
#include "AudioClipSource.h"

class TrackAudioChannel
{
public:
    TrackAudioChannel(TrackEngine& engine, int index);

    void prepare(double sampleRate);
    void refreshClips();
    void render(juce::AudioSampleBuffer& mixBuffer,
                int numSamples,
                double blockStartBeat,
                double samplesPerBeat);

private:
    TrackEngine& trackEngine;
    int trackIndex;
    double sampleRate { 44100.0 };
    std::vector<std::unique_ptr<AudioClipSource>> clipSources;
};
