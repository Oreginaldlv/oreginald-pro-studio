#pragma once
#include <JuceHeader.h>
#include "TransportState.h"

class TransportBar : public juce::Component,
                     private TransportState::Listener
{
public:
    explicit TransportBar (TransportState& state);
    ~TransportBar() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void transportStateChanged() override;
    void refreshFromState();

    TransportState& transportState;

    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton recordButton { "Rec" };
    juce::TextButton bpmDownButton { "-" };
    juce::TextButton bpmUpButton { "+" };

    juce::ToggleButton loopToggle { "Loop" };
    juce::ToggleButton metroToggle { "Met" };

    juce::Label titleLabel;
    juce::Label bpmLabel;
    juce::Label timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBar)
};