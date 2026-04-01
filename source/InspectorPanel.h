#pragma once
#include <JuceHeader.h>
#include "TrackEngine.h"

class InspectorPanel : public juce::Component
{
public:
    explicit InspectorPanel (TrackEngine& engine);

    void paint (juce::Graphics& g) override;
    void resized() override;
    void syncFromTrackEngine();

private:
    void populateTrackSelector();

    TrackEngine& trackEngine;

    juce::Label headerLabel;
    juce::ComboBox trackSelector;
    juce::Slider volumeSlider;
    juce::Slider panSlider;
    juce::ToggleButton armToggle { "Arm" };
    juce::ToggleButton muteToggle { "Mute" };
    juce::ToggleButton soloToggle { "Solo" };

    juce::Label volumeLabel;
    juce::Label panLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InspectorPanel)
};
