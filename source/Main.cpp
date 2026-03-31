#include <JuceHeader.h>
#include "MainComponent.h"

class OreginaldProStudioApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override       { return "Oreginald Pro Studio"; }
    const juce::String getApplicationVersion() override    { return "0.1.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise (const juce::String&) override
    {
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String&) override
    {
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        explicit MainWindow (juce::String name)
            : DocumentWindow (name,
                              juce::Colour::fromRGB (24, 24, 28),
                              juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setResizable (true, true);
            setResizeLimits (1100, 700, 3840, 2160);
            setContentOwned (new MainComponent(), true);
            centreWithSize (1400, 860);
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (OreginaldProStudioApplication)