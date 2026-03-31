#include "TransportBar.h"

TransportBar::TransportBar()
{
    addAndMakeVisible (playButton);
    addAndMakeVisible (stopButton);
    addAndMakeVisible (recordButton);
    addAndMakeVisible (loopToggle);
    addAndMakeVisible (metroToggle);
    addAndMakeVisible (titleLabel);
    addAndMakeVisible (bpmLabel);
    addAndMakeVisible (timeLabel);

    titleLabel.setText ("Oreginald Pro Studio", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setFont (juce::FontOptions (20.0f, juce::Font::bold));

    bpmLabel.setText ("BPM 120", juce::dontSendNotification);
    bpmLabel.setJustificationType (juce::Justification::centred);

    timeLabel.setText ("1.1.1", juce::dontSendNotification);
    timeLabel.setJustificationType (juce::Justification::centred);
}

void TransportBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (26, 28, 34));

    g.setColour (juce::Colour::fromRGB (50, 54, 62));
    g.drawLine (0.0f, (float) getHeight() - 1.0f, (float) getWidth(), (float) getHeight() - 1.0f, 1.0f);
}

void TransportBar::resized()
{
    auto area = getLocalBounds().reduced (10, 8);

    auto left = area.removeFromLeft (250);
    titleLabel.setBounds (left);

    auto controls = area.removeFromLeft (320);
    playButton.setBounds (controls.removeFromLeft (70).reduced (4, 0));
    stopButton.setBounds (controls.removeFromLeft (70).reduced (4, 0));
    recordButton.setBounds (controls.removeFromLeft (70).reduced (4, 0));
    loopToggle.setBounds (controls.removeFromLeft (55));
    metroToggle.setBounds (controls.removeFromLeft (55));

    auto right = area.removeFromRight (180);
    timeLabel.setBounds (right.removeFromRight (90));
    bpmLabel.setBounds (right.removeFromRight (90));
}