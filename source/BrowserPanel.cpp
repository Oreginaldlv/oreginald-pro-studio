#include "BrowserPanel.h"

BrowserPanel::BrowserPanel()
    : fileListModel(*this)
{
    headerLabel.setText ("BROWSER", juce::dontSendNotification);
    headerLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (headerLabel);

    importButton.onClick = [this]
    {
        importAudioFiles();
    };
    addAndMakeVisible (importButton);

    fileList.setModel (&fileListModel);
    addAndMakeVisible (fileList);

    refreshFileList();
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
    importButton.setBounds (area.removeFromTop (28));
    area.removeFromTop (8);

    fileList.setBounds (area);
}

int BrowserPanel::FileListModel::getNumRows()
{
    return owner.audioFiles.size();
}

void BrowserPanel::FileListModel::paintListBoxItem (int rowNumber,
                                                    juce::Graphics& g,
                                                    int width,
                                                    int height,
                                                    bool rowIsSelected)
{
    if (! juce::isPositiveAndBelow (rowNumber, owner.audioFiles.size()))
        return;

    if (rowIsSelected)
        g.fillAll (juce::Colour (0xff2d2d2d));

    g.setColour (juce::Colours::white);
    g.drawText (owner.audioFiles[rowNumber].getFileName(),
                8,
                0,
                width - 16,
                height,
                juce::Justification::centredLeft);
}

void BrowserPanel::FileListModel::listBoxItemDoubleClicked (int row, const juce::MouseEvent&)
{
    if (! juce::isPositiveAndBelow (row, owner.audioFiles.size()))
        return;

    if (owner.onAudioFileChosen)
        owner.onAudioFileChosen (owner.audioFiles[row]);
}

void BrowserPanel::refreshFileList()
{
    audioFiles.clear();

    const auto assetDirectory = juce::File::getCurrentWorkingDirectory().getChildFile ("assets");
    const auto files = assetDirectory.findChildFiles (juce::File::findFiles, true, "*.wav");

    for (const auto& file : files)
        audioFiles.add (file);

    fileList.updateContent();
    fileList.repaint();
}

void BrowserPanel::importAudioFiles()
{
    fileChooser = std::make_unique<juce::FileChooser>("Import WAV file",
                                                      juce::File::getCurrentWorkingDirectory(),
                                                      "*.wav");

    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                             [this](const juce::FileChooser& chooser)
                             {
                                 const auto file = chooser.getResult();

                                 if (file == juce::File{})
                                     return;

                                 audioFiles.addIfNotAlreadyThere(file);
                                 fileList.updateContent();
                                 fileList.repaint();
                             });
}
