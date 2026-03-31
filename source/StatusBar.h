#pragma once
#include <JuceHeader.h>
#include "TransportState.h"

class StatusBar : public juce::Component,
                  private TransportState::Listener
{
public:
    explicit StatusBar (TransportState& state);
    ~StatusBar() override;

    void paint (juce::Graphics& g) override;
    void resized() override {}

private:
    void transportStateChanged() override;
    void rebuildText();

    TransportState& transportState;
    juce::String statusText;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StatusBar)
};