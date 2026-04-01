#pragma once
#include <JuceHeader.h>

class ClipComponent : public juce::Component
{
public:
    explicit ClipComponent(const juce::String& clipName);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

    std::function<void(ClipComponent*, int)> onDragged;
    std::function<void(ClipComponent*, int)> onResized;
    std::function<void(ClipComponent*)> onSelected;

    void setSelected(bool shouldBeSelected);
    bool isSelected() const noexcept;

    void setClipName(const juce::String& newName);
    const juce::String& getClipName() const noexcept;

private:
    bool isNearRightEdge(int x) const noexcept;

    juce::String name;
    bool selected { false };
    bool resizing { false };

    juce::Point<int> dragStart;
    juce::Rectangle<int> dragStartBounds;

    static constexpr int resizeHandleWidth = 10;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClipComponent)
};
