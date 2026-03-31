#pragma once
#include <JuceHeader.h>

class ArrangementView : public juce::Component
{
public:
    ArrangementView() = default;

    void paint (juce::Graphics& g) override;
    void resized() override {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArrangementView)
};