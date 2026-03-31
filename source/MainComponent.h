#pragma once
#include <JuceHeader.h>
#include "TransportState.h"
#include "TransportBar.h"
#include "BrowserPanel.h"
#include "InspectorPanel.h"
#include "ArrangementView.h"
#include "StatusBar.h"

class MainComponent : public juce::Component,
                      private juce::Timer
{
public:
    MainComponent();

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    TransportState transportState;
    TransportBar transportBar;
    BrowserPanel browserPanel;
    InspectorPanel inspectorPanel;
    ArrangementView arrangementView;
    StatusBar statusBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};