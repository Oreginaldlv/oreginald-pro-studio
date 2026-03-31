#include "StatusBar.h"

StatusBar::StatusBar (TransportState& state)
    : transportState (state)
{
    transportState.addListener (this);
    rebuildText();
}

StatusBar::~StatusBar()
{
    transportState.removeListener (this);
}

void StatusBar::transportStateChanged()
{
    rebuildText();
    repaint();
}

void StatusBar::rebuildText()
{
    const auto modeText = transportState.isPlaying() ? "Playing" : "Stopped";
    const auto recText = transportState.isRecordArmed() ? "Rec: Armed" : "Rec: Off";
    const auto loopText = transportState.isLoopEnabled() ? "Loop: On" : "Loop: Off";
    const auto metText = transportState.isMetronomeEnabled() ? "Met: On" : "Met: Off";
    const auto beatText = "Beat: " + juce::String (transportState.getPlayheadBeats(), 2);

    statusText = "Audio: Ready   |   Engine: " + modeText
               + "   |   " + recText
               + "   |   " + loopText
               + "   |   " + metText
               + "   |   " + beatText;
}

void StatusBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (24, 26, 31));

    g.setColour (juce::Colour::fromRGB (48, 52, 60));
    g.drawLine (0.0f, 0.0f, (float) getWidth(), 0.0f, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.72f));
    g.drawText (statusText, getLocalBounds().reduced (10, 0), juce::Justification::centredLeft);
}