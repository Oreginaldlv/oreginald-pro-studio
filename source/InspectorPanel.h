#pragma once
#include <JuceHeader.h>

class InspectorPanel : public juce::Component
{
public:
    InspectorPanel();

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label headerLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InspectorPanel)
};