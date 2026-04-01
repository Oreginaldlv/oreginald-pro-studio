#include "AudioEngine.h"
#include <cstring>

AudioEngine::AudioEngine(TransportState& transportStateRef, TrackEngine& trackEngineRef)
    : transportState(transportStateRef), trackEngine(trackEngineRef)
{
    for (int i = 0; i < trackEngine.getNumTracks(); ++i)
        trackChannels.emplace_back(std::make_unique<TrackAudioChannel>(trackEngine, i));

    refreshClipSources();
    deviceManager.initialiseWithDefaultDevices(0, 2);
    deviceManager.addAudioCallback(this);
}

AudioEngine::~AudioEngine()
{
    deviceManager.removeAudioCallback(this);
}

double AudioEngine::getPlayheadBeats() const noexcept
{
    return playheadBeats.load();
}

juce::String AudioEngine::getBarBeatString() const noexcept
{
    const double beats = playheadBeats.load();
    const int barNumber = (int)(beats / beatsPerBar) + 1;
    const int beatNumber = ((int) beats % beatsPerBar) + 1;
    return juce::String(barNumber) + ":" + juce::String(beatNumber);
}

void AudioEngine::refreshClipSources()
{
    for (auto& channel : trackChannels)
    {
        channel->refreshClips();
        channel->prepare(sampleRate);
    }
}

void AudioEngine::jumpToBeat(double beats)
{
    playheadBeats.store(beats);
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    sampleRate = device->getCurrentSampleRate();
    maxBlockSize = device->getCurrentBufferSizeSamples();
    mixBuffer.setSize(2, maxBlockSize);
    refreshClipSources();
}

void AudioEngine::audioDeviceStopped()
{
    mixBuffer.setSize(0, 0);
}

void AudioEngine::audioDeviceIOCallback(const float* const* inputChannelData,
                                        int numInputChannels,
                                        float** outputChannelData,
                                        int numOutputChannels,
                                        int numSamples)
{
    juce::ignoreUnused(inputChannelData, numInputChannels);

    if (numSamples <= 0)
        return;

    mixBuffer.clear();

    if (!transportState.isPlaying())
    {
        mixToOutput(outputChannelData, numOutputChannels, numSamples);
        return;
    }

    const double tempo = juce::jmax(1.0, transportState.getTempo());
    const double samplesPerBeat = this->sampleRate * 60.0 / tempo;

    const double blockStartBeat = playheadBeats.load();
    const double blockDurationBeats = numSamples / samplesPerBeat;

    for (auto& channel : trackChannels)
        channel->render(mixBuffer, numSamples, blockStartBeat, samplesPerBeat);

    mixToOutput(outputChannelData, numOutputChannels, numSamples);

    playheadBeats.store(blockStartBeat + blockDurationBeats);
}

void AudioEngine::mixToOutput(float** outputChannelData, int numOutputChannels, int numSamples)
{
    if (numOutputChannels <= 0 || numSamples <= 0)
        return;

    const int mixChannels = mixBuffer.getNumChannels();

    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        auto* dest = outputChannelData[channel];

        if (dest == nullptr)
            continue;

        const int sourceChannel = channel < mixChannels ? channel : mixChannels - 1;
        const float* source = mixBuffer.getReadPointer(sourceChannel);

        std::memcpy(dest, source, sizeof(float) * (size_t) numSamples);
    }
}
