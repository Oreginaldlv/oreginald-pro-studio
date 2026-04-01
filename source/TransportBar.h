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

    std::function<void()> onSaveClicked;
    std::function<void()> onLoadClicked;

private:
    void updateButtonColours();

    TransportState& transportState;

    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton recordButton { "Record" };
    juce::TextButton saveButton { "Save" };
    juce::TextButton loadButton { "Load" };

    juce::Label tempoLabel;
    juce::Slider tempoSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBar)
};
