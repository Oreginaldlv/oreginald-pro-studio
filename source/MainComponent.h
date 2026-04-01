#pragma once
#include <JuceHeader.h>

#include "TransportState.h"
#include "TrackEngine.h"

#include "TransportBar.h"
#include "BrowserPanel.h"
#include "InspectorPanel.h"
#include "ArrangementView.h"
#include "StatusBar.h"
#include "AudioEngine.h"
#include "ProjectFileService.h"

class MainComponent : public juce::Component,
                      private juce::Timer
{
public:
    MainComponent();

    void paint (juce::Graphics&) override;
    void resized() override;
    bool keyPressed (const juce::KeyPress& key) override;

private:
    void timerCallback() override;
    void addAudioClipToSelectedTrack (const juce::File& file);
    void addAudioClipAtPosition (const juce::File& file, int trackIndex, double startBeat);
    double getAudioLengthBeats (const juce::File& file) const;
    void refreshProjectState();
    void saveProject();
    void loadProject();

    // core state
    TransportState transportState;
    TrackEngine trackEngine;
    AudioEngine audioEngine;
    ProjectFileService projectFileService;
    juce::File currentProjectFile;

    // UI
    TransportBar transportBar;
    BrowserPanel browserPanel;
    InspectorPanel inspectorPanel;
    ArrangementView arrangementView;
    StatusBar statusBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
