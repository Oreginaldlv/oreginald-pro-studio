#include "MainComponent.h"

MainComponent::MainComponent()
    : transportBar(transportState),
      inspectorPanel(trackEngine),
      arrangementView(trackEngine, transportState)
{
    addAndMakeVisible(transportBar);
    addAndMakeVisible(browserPanel);
    addAndMakeVisible(inspectorPanel);
    addAndMakeVisible(arrangementView);
    addAndMakeVisible(statusBar);

    startTimerHz(30);
    setSize(1400, 900);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111111));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    constexpr int transportHeight = 60;
    constexpr int statusHeight = 22;
    constexpr int browserWidth = 220;
    constexpr int inspectorWidth = 300;

    transportBar.setBounds(area.removeFromTop(transportHeight));
    statusBar.setBounds(area.removeFromBottom(statusHeight));

    browserPanel.setBounds(area.removeFromLeft(browserWidth));
    inspectorPanel.setBounds(area.removeFromRight(inspectorWidth));

    arrangementView.setBounds(area);
}

void MainComponent::timerCallback()
{
    transportState.updatePlayhead();
    inspectorPanel.syncFromTrackEngine();
    transportBar.syncFromTransportState();

    arrangementView.repaint();
    inspectorPanel.repaint();

    const int selectedIndex = trackEngine.getSelectedTrackIndex();
    const auto& track = trackEngine.getTrack(selectedIndex);

    juce::String status;

    status << "Position: "
           << transportState.getBarBeatString()

           << " | Track: "
           << track.name

           << " | Vol: "
           << juce::String(track.volume, 2)

           << " | Pan: "
           << juce::String(track.pan, 2)

           << " | Tempo: "
           << juce::String(transportState.getTempo(), 1)

           << " | Playing: "
           << (transportState.isPlaying() ? "Yes" : "No")

           << " | Recording: "
           << (transportState.isRecording() ? "Yes" : "No");

    statusBar.setStatusText(status);
}
