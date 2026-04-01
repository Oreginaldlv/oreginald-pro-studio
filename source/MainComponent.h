#pragma once
#include <JuceHeader.h>

#include "TransportState.h"
#include "TrackEngine.h"

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

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    // core state
    TransportState transportState;
    TrackEngine trackEngine;

    // UI
    TransportBar transportBar;
    BrowserPanel browserPanel;
    InspectorPanel inspectorPanel;
    ArrangementView arrangementView;
    StatusBar statusBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};