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
    const juce::String modeText = transportState.isPlaying() ? "Playing" : "Stopped";
    const juce::String recText  = transportState.isRecordArmed() ? "Rec: Armed" : "Rec: Off";
    const juce::String loopText = transportState.isLoopEnabled() ? "Loop: On" : "Loop: Off";
    const juce::String metText  = transportState.isMetronomeEnabled() ? "Met: On" : "Met: Off";
    const juce::String bpmText  = "BPM: " + juce::String ((int) std::round (transportState.getBpm()));
    const juce::String beatText = "Beat: " + juce::String (transportState.getPlayheadBeats(), 2);
    const juce::String rangeText = "LoopRange: "
                                 + juce::String (transportState.getLoopStartBeat(), 2)
                                 + "-"
                                 + juce::String (transportState.getLoopEndBeat(), 2);

    statusText = juce::String ("Audio: Ready   |   Engine: ")
               + modeText
               + "   |   "
               + recText
               + "   |   "
               + loopText
               + "   |   "
               + metText
               + "   |   "
               + bpmText
               + "   |   "
               + beatText
               + "   |   "
               + rangeText;
}

void StatusBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (24, 26, 31));

    g.setColour (juce::Colour::fromRGB (48, 52, 60));
    g.drawLine (0.0f, 0.0f, (float) getWidth(), 0.0f, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.72f));
    g.drawText (statusText, getLocalBounds().reduced (10, 0), juce::Justification::centredLeft);
}