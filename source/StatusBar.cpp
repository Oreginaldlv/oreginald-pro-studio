#include "StatusBar.h"

StatusBar::StatusBar()
{
    statusLabel.setText ("Ready", juce::dontSendNotification);
    statusLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (statusLabel);
}

void StatusBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff151515));

    g.setColour (juce::Colour (0xff2a2a2a));
    g.drawLine (0.0f, 0.0f, (float) getWidth(), 0.0f, 1.0f);
}

void StatusBar::resized()
{
    statusLabel.setBounds (getLocalBounds().reduced (10, 2));
}

void StatusBar::setStatusText (const juce::String& text)
{
    statusLabel.setText (text, juce::dontSendNotification);
}
