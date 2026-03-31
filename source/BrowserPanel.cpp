#include "BrowserPanel.h"

BrowserPanel::BrowserPanel()
{
    addAndMakeVisible (headerLabel);
    addAndMakeVisible (searchBox);

    headerLabel.setText ("Browser", juce::dontSendNotification);
    headerLabel.setFont (juce::FontOptions (16.0f, juce::Font::bold));
    headerLabel.setJustificationType (juce::Justification::centredLeft);

    searchBox.setTextToShowWhenEmpty ("Search samples, presets, projects...", juce::Colours::grey);

    items.add ("All Sounds");
    items.add ("Drums");
    items.add ("Instruments");
    items.add ("Audio FX");
    items.add ("MIDI FX");
    items.add ("Samples");
    items.add ("Projects");
    items.add ("Collections");
}

void BrowserPanel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (22, 24, 29));

    g.setColour (juce::Colour::fromRGB (48, 52, 60));
    g.drawLine ((float) getWidth() - 1.0f, 0.0f, (float) getWidth() - 1.0f, (float) getHeight(), 1.0f);

    g.setColour (juce::Colours::white.withAlpha (0.90f));

    int y = 72;
    for (int i = 0; i < items.size(); ++i)
    {
        auto row = juce::Rectangle<int> (12, y, getWidth() - 24, 28);

        if (i == 0)
        {
            g.setColour (juce::Colour::fromRGB (55, 65, 96));
            g.fillRoundedRectangle (row.toFloat(), 6.0f);
            g.setColour (juce::Colours::white);
        }
        else
        {
            g.setColour (juce::Colours::white.withAlpha (0.78f));
        }

        g.drawText (items[i], row.reduced (10, 0), juce::Justification::centredLeft);
        y += 34;
    }
}

void BrowserPanel::resized()
{
    auto area = getLocalBounds().reduced (12);

    headerLabel.setBounds (area.removeFromTop (28));
    area.removeFromTop (8);
    searchBox.setBounds (area.removeFromTop (28));
}