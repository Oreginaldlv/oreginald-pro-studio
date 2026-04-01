#include "AudioEngine.h"
#include <cstring>
#include <algorithm>
#include <cmath>

AudioEngine::AudioEngine(TransportState& transportStateRef, TrackEngine& trackEngineRef)
    : transportState(transportStateRef), trackEngine(trackEngineRef)
{
    for (int i = 0; i < trackEngine.getNumTracks(); ++i)
        trackChannels.emplace_back(std::make_unique<TrackAudioChannel>(trackEngine, i));

    refreshClipSources();
    deviceManager.initialiseWithDefaultDevices(0, 2);
    deviceManager.addAudioCallback(this);
    recordingThread.startThread();
}

AudioEngine::~AudioEngine()
{
    stopRecording();
    deviceManager.removeAudioCallback(this);
    recordingThread.stopThread(2000);
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
    playheadBeats.store(transportState.wrapToLoop(beats));
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
    stopRecording();
    mixBuffer.setSize(0, 0);
}

void AudioEngine::audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                                   int numInputChannels,
                                                   float* const* outputChannelData,
                                                   int numOutputChannels,
                                                   int numSamples,
                                                   const juce::AudioIODeviceCallbackContext&)
{
    if (numSamples <= 0)
        return;

    mixBuffer.clear();

    if (!transportState.isPlaying())
    {
        mixToOutput(outputChannelData, numOutputChannels, numSamples);
        return;
    }

    const double tempo = std::max(1.0, (double) transportState.getTempo());
    const double samplesPerBeat = this->sampleRate * 60.0 / tempo;

    double blockStartBeat = transportState.wrapToLoop(playheadBeats.load());
    const double blockDurationBeats = numSamples / samplesPerBeat;

    for (auto& channel : trackChannels)
        channel->render(mixBuffer, numSamples, blockStartBeat, samplesPerBeat);

    renderMetronome(mixBuffer, numSamples, blockStartBeat, samplesPerBeat);

    writeRecordingBlock(inputChannelData, numInputChannels, numSamples);

    mixToOutput(outputChannelData, numOutputChannels, numSamples);

    double nextBeat = blockStartBeat + blockDurationBeats;
    nextBeat = transportState.wrapToLoop(nextBeat);
    playheadBeats.store(nextBeat);
}

void AudioEngine::renderMetronome(juce::AudioSampleBuffer& buffer,
                                  int numSamples,
                                  double blockStartBeat,
                                  double samplesPerBeat)
{
    if (! transportState.isMetronomeEnabled())
        return;

    const int channels = buffer.getNumChannels();
    if (channels <= 0 || sampleRate <= 0.0)
        return;

    const int beatsPerBarLocal = transportState.getBeatsPerBar();
    const double blockEndBeat = blockStartBeat + (double) numSamples / samplesPerBeat;

    const int startBeatIndex = (int) std::floor(blockStartBeat);
    const int endBeatIndex = (int) std::floor(blockEndBeat);

    for (int beatIndex = startBeatIndex; beatIndex <= endBeatIndex; ++beatIndex)
    {
        const double beatTime = (double) beatIndex;
        if (beatTime < blockStartBeat || beatTime >= blockEndBeat)
            continue;

        const int sampleStart = (int) std::floor((beatTime - blockStartBeat) * samplesPerBeat);
        if (sampleStart >= numSamples)
            continue;

        const bool isAccent = (beatIndex % beatsPerBarLocal) == 0;
        const float frequency = isAccent ? metronomeAccentFreq : metronomeBeatFreq;
        const float gain = isAccent ? metronomeAccentGain : metronomeBeatGain;
        const double phaseIncrement = juce::MathConstants<double>::twoPi * frequency / sampleRate;

        for (int i = 0; i < metronomeLengthSamples; ++i)
        {
            const int sampleIndex = sampleStart + i;
            if (sampleIndex >= numSamples)
                break;

            const float envelope = 1.0f - (float) i / (float) metronomeLengthSamples;
            const float value = std::sin((double) i * phaseIncrement) * envelope * gain;

            for (int channel = 0; channel < channels; ++channel)
                buffer.addSample(channel, sampleIndex, value);
        }
    }
}

void AudioEngine::mixToOutput(float* const* outputChannelData, int numOutputChannels, int numSamples)
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

bool AudioEngine::startRecording(const juce::File& file, int numChannels)
{
    stopRecording();

    if (sampleRate <= 0.0 || numChannels <= 0)
        return false;

    std::unique_ptr<juce::FileOutputStream> fileStream(file.createOutputStream());

    if (fileStream == nullptr || ! fileStream->openedOk())
        return false;

    juce::WavAudioFormat format;
    auto* writer = format.createWriterFor(fileStream.release(),
                                          sampleRate,
                                          (unsigned int) numChannels,
                                          16,
                                          {},
                                          0);

    if (writer == nullptr)
        return false;

    threadedWriter.reset(new juce::AudioFormatWriter::ThreadedWriter(writer,
                                                                     recordingThread,
                                                                     32768));
    activeWriter.store(threadedWriter.get());
    recordingChannels = numChannels;
    recordedSamples = 0;
    recordingBuffer.setSize(recordingChannels, maxBlockSize);
    return true;
}

juce::int64 AudioEngine::stopRecording()
{
    activeWriter.store(nullptr);
    threadedWriter.reset();
    recordingBuffer.setSize(0, 0);
    return recordedSamples.exchange(0);
}

void AudioEngine::writeRecordingBlock(const float* const* inputChannelData,
                                      int numInputChannels,
                                      int numSamples)
{
    auto* writer = activeWriter.load();

    if (writer == nullptr)
        return;

    if (numSamples <= 0)
        return;

    if (recordingBuffer.getNumChannels() != recordingChannels
        || recordingBuffer.getNumSamples() < numSamples)
    {
        recordingBuffer.setSize(recordingChannels, numSamples, false, false, true);
    }

    recordingBuffer.clear();

    if (inputChannelData != nullptr && numInputChannels > 0)
    {
        const float* first = inputChannelData[0];
        const float* second = numInputChannels > 1 ? inputChannelData[1] : nullptr;

        auto* dest = recordingBuffer.getWritePointer(0);

        if (second != nullptr)
        {
            for (int i = 0; i < numSamples; ++i)
                dest[i] = 0.5f * (first[i] + second[i]);
        }
        else
        {
            std::memcpy(dest, first, sizeof(float) * (size_t) numSamples);
        }
    }

    writer->write(recordingBuffer.getArrayOfReadPointers(),
                  recordingChannels,
                  numSamples);

    recordedSamples += numSamples;
}
