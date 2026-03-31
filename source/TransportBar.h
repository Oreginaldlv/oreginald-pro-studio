#pragma once
#include <JuceHeader.h>

class TransportBar : public juce::Component
{
public:
    TransportBar();

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::TextButton recordButton { "Rec" };

    juce::ToggleButton loopToggle { "Loop" };
    juce::ToggleButton metroToggle { "Met" };

    juce::Label titleLabel;
    juce::Label bpmLabel;
    juce::Label timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBar)
};