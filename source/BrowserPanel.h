#pragma once
#include <JuceHeader.h>

class BrowserPanel : public juce::Component
{
public:
    BrowserPanel();

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label headerLabel;
    juce::ListBox fileList;

    class FileListModel : public juce::ListBoxModel
    {
    public:
        int getNumRows() override;
        void paintListBoxItem (int rowNumber,
                               juce::Graphics& g,
                               int width,
                               int height,
                               bool rowIsSelected) override;
    };

    FileListModel fileListModel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrowserPanel)
};
