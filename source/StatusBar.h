#pragma once
#include <JuceHeader.h>

class StatusBar : public juce::Component
{
public:
    StatusBar();

    void paint (juce::Graphics& g) override;
    void resized() override;

    void setStatusText (const juce::String& text);

private:
    juce::Label statusLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatusBar)
};
