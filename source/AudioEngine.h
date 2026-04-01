#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "TrackEngine.h"
#include "TransportState.h"
#include "TrackAudioChannel.h"

class AudioEngine : private juce::AudioIODeviceCallback
{
public:
    AudioEngine(TransportState& transportState, TrackEngine& trackEngine);
    ~AudioEngine() override;

    double getPlayheadBeats() const noexcept;
    juce::String getBarBeatString() const noexcept;
    void refreshClipSources();
    void jumpToBeat(double beats);

    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
                                           const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

    void mixToOutput(float* const* outputChannelData, int numOutputChannels, int numSamples);

private:
    TransportState& transportState;
    TrackEngine& trackEngine;

    juce::AudioDeviceManager deviceManager;
    juce::AudioSampleBuffer mixBuffer;
    std::vector<std::unique_ptr<TrackAudioChannel>> trackChannels;

    std::atomic<double> playheadBeats { 0.0 };
    double sampleRate { 44100.0 };
    int maxBlockSize { 512 };
    static constexpr int beatsPerBar { 4 };
};
