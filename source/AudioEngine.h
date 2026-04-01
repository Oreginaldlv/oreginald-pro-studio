#pragma once
#include <atomic>
#include <vector>
#include <memory>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
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
    double getSampleRate() const noexcept { return sampleRate; }

    bool startRecording(const juce::File& file, int numChannels = 1);
    juce::int64 stopRecording();
    bool isRecordingActive() const noexcept { return activeWriter.load() != nullptr; }

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
    void renderMetronome(juce::AudioSampleBuffer& buffer,
                         int numSamples,
                         double blockStartBeat,
                         double samplesPerBeat);

    void writeRecordingBlock(const float* const* inputChannelData,
                             int numInputChannels,
                             int numSamples);

    TransportState& transportState;
    TrackEngine& trackEngine;

    juce::AudioDeviceManager deviceManager;
    juce::AudioSampleBuffer mixBuffer;
    std::vector<std::unique_ptr<TrackAudioChannel>> trackChannels;

    std::atomic<double> playheadBeats { 0.0 };
    double sampleRate { 44100.0 };
    int maxBlockSize { 512 };

    // Recording
    juce::TimeSliceThread recordingThread { "Recording Writer" };
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;
    std::atomic<juce::AudioFormatWriter::ThreadedWriter*> activeWriter { nullptr };
    juce::AudioBuffer<float> recordingBuffer;
    std::atomic<juce::int64> recordedSamples { 0 };
    int recordingChannels { 1 };

    static constexpr int beatsPerBar { 4 };
    static constexpr int metronomeLengthSamples { 1200 };
    static constexpr float metronomeAccentGain { 0.35f };
    static constexpr float metronomeBeatGain { 0.2f };
    static constexpr float metronomeAccentFreq { 1600.0f };
    static constexpr float metronomeBeatFreq { 1200.0f };
};
