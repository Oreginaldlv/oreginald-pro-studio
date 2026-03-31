#include "StatusBar.h"

void StatusBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (24, 26, 31));

    g.setColour (juce::Colour::fromRGB (48, 52, 60));
    g.drawLine (0.0f, 0.0f, (float) getWidth(), 0.0f, 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.72f));
    g.drawText ("Audio: Ready   |   Engine: Idle   |   Project: Untitled.ops", getLocalBounds().reduced (10, 0),
                juce::Justification::centredLeft);
}