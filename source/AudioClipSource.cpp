#include "AudioClipSource.h"
#include <cmath>

AudioClipSource::AudioClipSource(int clipTrackIndex,
                                 double clipStartBeat,
                                 double clipLengthBeats,
                                 const juce::String& clipName,
                                 const juce::String& clipFilePath)
    : trackIndex(clipTrackIndex),
      startBeat(clipStartBeat),
      lengthBeats(clipLengthBeats),
      name(clipName),
      filePath(clipFilePath)
{
    loadAudioFile();
}

void AudioClipSource::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    frequency = 220.0f + (float) trackIndex * 40.0f;
    phaseIncrement = juce::MathConstants<double>::twoPi * (double) frequency / sampleRate;
}

void AudioClipSource::render(juce::AudioSampleBuffer& buffer,
                             int numSamples,
                             double blockStartBeat,
                             double samplesPerBeat,
                             float gain,
                             float pan)
{
    if (numSamples <= 0 || buffer.getNumChannels() < 2)
        return;

    const double clipStartBeat = startBeat;
    const double clipEndBeat = startBeat + lengthBeats;
    const double blockEndBeat = blockStartBeat + (double) numSamples / samplesPerBeat;

    if (blockEndBeat <= clipStartBeat || blockStartBeat >= clipEndBeat)
        return;

    int startSample = juce::jmax(0, (int) std::floor((clipStartBeat - blockStartBeat) * samplesPerBeat));
    int endSample = juce::jmin(numSamples, (int) std::ceil((clipEndBeat - blockStartBeat) * samplesPerBeat));

    startSample = juce::jlimit(0, numSamples, startSample);
    endSample = juce::jlimit(0, numSamples, endSample);

    if (endSample <= startSample)
        return;

    const float leftGain = gain * (pan <= 0.0f ? 1.0f : 1.0f - pan);
    const float rightGain = gain * (pan >= 0.0f ? 1.0f : 1.0f + pan);

    if (hasAudioFile && audioData.getNumSamples() > 0)
    {
        const int sourceLength = audioData.getNumSamples();
        const int sourceChannels = juce::jmax(1, audioData.getNumChannels());

        for (int sample = startSample; sample < endSample; ++sample)
        {
            const double beatAtSample = blockStartBeat + (double) sample / samplesPerBeat;
            const double beatOffset = beatAtSample - clipStartBeat;
            const double sourcePosition = (beatOffset / lengthBeats) * (double) sourceLength;
            const int sourceIndex = juce::jlimit(0, sourceLength - 1, (int) sourcePosition);
            const float left = audioData.getSample(0, sourceIndex);
            const float right = audioData.getSample(sourceChannels > 1 ? 1 : 0, sourceIndex);

            buffer.addSample(0, sample, left * leftGain);
            buffer.addSample(1, sample, right * rightGain);
        }

        return;
    }

    for (int sample = startSample; sample < endSample; ++sample)
    {
        const double beatAtSample = blockStartBeat + (double) sample / samplesPerBeat;
        const double clipOffsetBeats = beatAtSample - clipStartBeat;
        const double clipOffsetSamples = clipOffsetBeats * samplesPerBeat;
        const double phase = clipOffsetSamples * phaseIncrement;
        const float value = (float) std::sin(phase);

        buffer.addSample(0, sample, value * leftGain);
        buffer.addSample(1, sample, value * rightGain);
    }
}

void AudioClipSource::loadAudioFile()
{
    audioData.setSize(0, 0);
    hasAudioFile = false;

    if (filePath.isEmpty())
        return;

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    juce::File sourceFile(filePath);

    if (! sourceFile.existsAsFile())
        return;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(sourceFile));

    if (reader == nullptr)
        return;

    const int64 sourceSamples = reader->lengthInSamples;
    const int channelCount = (int) reader->numChannels;

    if (sourceSamples <= 0 || channelCount <= 0)
        return;

    audioData.setSize(juce::jmin(2, channelCount), (int) sourceSamples);
    reader->read(&audioData, 0, (int) sourceSamples, 0, true, channelCount > 1);
    hasAudioFile = true;
}
