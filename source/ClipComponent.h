#pragma once
#include <JuceHeader.h>

class ClipComponent : public juce::Component
{
public:
    explicit ClipComponent(const juce::String& clipName);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;

    std::function<void(ClipComponent*, juce::Point<int>)> onDragged;
    std::function<void(ClipComponent*)> onSelected;

    void setSelected(bool shouldBeSelected);
    bool isSelected() const noexcept;

    void setClipName(const juce::String& newName);
    const juce::String& getClipName() const noexcept;

private:
    juce::String name;
    bool selected { false };

    juce::Point<int> dragStart;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipComponent)
};