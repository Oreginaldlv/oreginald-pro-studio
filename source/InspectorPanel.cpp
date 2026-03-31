#include "InspectorPanel.h"

InspectorPanel::InspectorPanel()
{
    addAndMakeVisible (headerLabel);
    headerLabel.setText ("Inspector", juce::dontSendNotification);
    headerLabel.setFont (juce::FontOptions (16.0f, juce::Font::bold));
    headerLabel.setJustificationType (juce::Justification::centredLeft);
}

void InspectorPanel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (22, 24, 29));

    g.setColour (juce::Colour::fromRGB (48, 52, 60));
    g.drawLine (0.0f, 0.0f, 0.0f, (float) getHeight(), 1.0f);

    auto area = getLocalBounds().reduced (12);
    area.removeFromTop (42);

    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawText ("Clip", area.removeFromTop (24), juce::Justification::centredLeft);

    g.setColour (juce::Colour::fromRGB (34, 37, 44));
    g.fillRoundedRectangle (area.removeFromTop (120).toFloat(), 8.0f);

    area.removeFromTop (14);
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawText ("Device Chain", area.removeFromTop (24), juce::Justification::centredLeft);

    g.setColour (juce::Colour::fromRGB (34, 37, 44));
    g.fillRoundedRectangle (area.removeFromTop (180).toFloat(), 8.0f);
}

void InspectorPanel::resized()
{
    headerLabel.setBounds (12, 12, getWidth() - 24, 24);
}