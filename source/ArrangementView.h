#pragma once
#include <JuceHeader.h>

class ArrangementView : public juce::Component
{
public:
    ArrangementView() = default;

    void paint (juce::Graphics& g) override;
    void resized() override {}

    void setPlayheadBeats (double newPlayheadBeats);

private:
    double playheadBeats { 0.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArrangementView)
};