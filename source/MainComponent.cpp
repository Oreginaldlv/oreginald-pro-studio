#include "MainComponent.h"

namespace
{
    constexpr int transportHeight = 64;
    constexpr int statusHeight = 28;
    constexpr int leftPanelWidth = 240;
    constexpr int rightPanelWidth = 260;
    constexpr int timerHz = 30;
}

MainComponent::MainComponent()
    : transportBar (transportState),
      statusBar (transportState)
{
    addAndMakeVisible (transportBar);
    addAndMakeVisible (browserPanel);
    addAndMakeVisible (inspectorPanel);
    addAndMakeVisible (arrangementView);
    addAndMakeVisible (statusBar);

    arrangementView.setPlayheadBeats (transportState.getPlayheadBeats());
    arrangementView.setLoopState (transportState.isLoopEnabled(),
                                  transportState.getLoopStartBeat(),
                                  transportState.getLoopEndBeat());

    setSize (1400, 860);
    startTimerHz (timerHz);
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (16, 17, 22));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    auto top = area.removeFromTop (transportHeight);
    transportBar.setBounds (top);

    auto bottom = area.removeFromBottom (statusHeight);
    statusBar.setBounds (bottom);

    auto left = area.removeFromLeft (leftPanelWidth);
    browserPanel.setBounds (left);

    auto right = area.removeFromRight (rightPanelWidth);
    inspectorPanel.setBounds (right);

    arrangementView.setBounds (area);
}

void MainComponent::timerCallback()
{
    constexpr double deltaSeconds = 1.0 / (double) timerHz;

    transportState.advance (deltaSeconds);

    arrangementView.setPlayheadBeats (transportState.getPlayheadBeats());
    arrangementView.setLoopState (transportState.isLoopEnabled(),
                                  transportState.getLoopStartBeat(),
                                  transportState.getLoopEndBeat());
}