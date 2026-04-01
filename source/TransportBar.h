#pragma once
#include <JuceHeader.h>
#include "TransportState.h"

class TransportBar : public juce::Component
{
public:
    explicit TransportBar (TransportState& state);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void syncFromTransportState();

private:
    void updateButtonColours();

    TransportState& transportState;

    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton recordButton { "Record" };

    juce::Label tempoLabel;
    juce::Slider tempoSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBar)
};
