#pragma once
#include <JuceHeader.h>

class BrowserPanel : public juce::Component
{
public:
    BrowserPanel();

    void paint (juce::Graphics& g) override;
    void resized() override;

    std::function<void(const juce::File&)> onAudioFileChosen;

private:
    void refreshFileList();
    void importAudioFiles();

    juce::Label headerLabel;
    juce::TextButton importButton { "Import audio" };
    juce::ListBox fileList;
    juce::Array<juce::File> audioFiles;
    std::unique_ptr<juce::FileChooser> fileChooser;

    class FileListModel : public juce::ListBoxModel
    {
    public:
        explicit FileListModel(BrowserPanel& ownerRef) : owner(ownerRef) {}

        int getNumRows() override;
        void paintListBoxItem (int rowNumber,
                               juce::Graphics& g,
                               int width,
                               int height,
                               bool rowIsSelected) override;
        void listBoxItemDoubleClicked (int row, const juce::MouseEvent&) override;

    private:
        BrowserPanel& owner;
    };

    FileListModel fileListModel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrowserPanel)
};
