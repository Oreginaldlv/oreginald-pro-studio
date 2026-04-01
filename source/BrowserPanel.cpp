#include "BrowserPanel.h"

BrowserPanel::BrowserPanel()
{
    headerLabel.setText ("BROWSER", juce::dontSendNotification);
    headerLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (headerLabel);

    fileList.setModel (&fileListModel);
    addAndMakeVisible (fileList);
}

void BrowserPanel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff171717));

    g.setColour (juce::Colour (0xff2a2a2a));
    g.drawRect (getLocalBounds(), 1);
}

void BrowserPanel::resized()
{
    auto area = getLocalBounds().reduced (10);

    headerLabel.setBounds (area.removeFromTop (28));
    area.removeFromTop (8);

    fileList.setBounds (area);
}

int BrowserPanel::FileListModel::getNumRows()
{
    return 8;
}

void BrowserPanel::FileListModel::paintListBoxItem (int rowNumber,
                                                    juce::Graphics& g,
                                                    int width,
                                                    int height,
                                                    bool rowIsSelected)
{
    const char* items[] =
    {
        "Kick.wav",
        "Snare.wav",
        "HiHat.wav",
        "Bass.wav",
        "Keys.wav",
        "Lead.wav",
        "Vox.wav",
        "FX.wav"
    };

    if (rowIsSelected)
        g.fillAll (juce::Colour (0xff2d2d2d));

    g.setColour (juce::Colours::white);
    g.drawText (items[rowNumber],
                8,
                0,
                width - 16,
                height,
                juce::Justification::centredLeft);
}
