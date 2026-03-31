#pragma once
#include <JuceHeader.h>

class BrowserPanel : public juce::Component
{
public:
    BrowserPanel();

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label headerLabel;
    juce::TextEditor searchBox;
    juce::StringArray items;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrowserPanel)
};