#include "AudioClipSource.h"
#include <cmath>

AudioClipSource::AudioClipSource(int clipTrackIndex, double clipStartBeat, double clipLengthBeats, const juce::String& clipName)
    : trackIndex(clipTrackIndex),
      startBeat(clipStartBeat),
      lengthBeats(clipLengthBeats),
      name(clipName)
{
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

    const double leftGain = pan <= 0.0f ? 1.0f : 1.0f - pan;
    const double rightGain = pan >= 0.0f ? 1.0f : 1.0f + pan;

    for (int sample = startSample; sample < endSample; ++sample)
    {
        const double beatAtSample = blockStartBeat + (double) sample / samplesPerBeat;
        const double clipOffsetBeats = beatAtSample - clipStartBeat;
        const double clipOffsetSamples = clipOffsetBeats * samplesPerBeat;
        const double phase = clipOffsetSamples * phaseIncrement;
        const float value = std::sin(phase);
        const float mixedValue = value * gain;

        buffer.addSample(0, sample, mixedValue * (float) leftGain);
        buffer.addSample(1, sample, mixedValue * (float) rightGain);
    }
}
