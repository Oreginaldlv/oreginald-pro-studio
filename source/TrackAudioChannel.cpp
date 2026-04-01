#include "TrackAudioChannel.h"

TrackAudioChannel::TrackAudioChannel(TrackEngine& engine, int index)
    : trackEngine(engine), trackIndex(index)
{
}

void TrackAudioChannel::prepare(double newSampleRate)
{
    sampleRate = newSampleRate;
    for (auto& clip : clipSources)
        clip->prepare(sampleRate);
}

void TrackAudioChannel::refreshClips()
{
    clipSources.clear();
    const auto& clips = trackEngine.getClips();

    for (int i = 0; i < clips.size(); ++i)
    {
        const auto& clip = clips.getReference(i);

        if (clip.trackIndex == trackIndex)
        {
            clipSources.emplace_back(std::make_unique<AudioClipSource>(clip.trackIndex,
                                                                        clip.startBeat,
                                                                        clip.lengthBeats,
                                                                        clip.name));
        }
    }

    prepare(sampleRate);
}

void TrackAudioChannel::render(juce::AudioSampleBuffer& mixBuffer,
                               int numSamples,
                               double blockStartBeat,
                               double samplesPerBeat)
{
    if (!trackEngine.isTrackAudible(trackIndex))
        return;

    const auto& track = trackEngine.getTrack(trackIndex);
    const float gain = track.volume;
    const float pan = track.pan;

    for (auto& clip : clipSources)
        clip->render(mixBuffer, numSamples, blockStartBeat, samplesPerBeat, gain, pan);
}
