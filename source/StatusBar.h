#pragma once
#include <JuceHeader.h>

class StatusBar : public juce::Component
{
public:
    StatusBar() = default;

    void paint (juce::Graphics& g) override;
    void resized() override {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatusBar)
};