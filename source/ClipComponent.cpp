#include "ClipComponent.h"

ClipComponent::ClipComponent(const juce::String& clipName)
    : name(clipName)
{
}

void ClipComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(selected ? juce::Colour(0xff4c6fff)
                         : juce::Colour(0xff3a8f66));

    g.fillRoundedRectangle(bounds.reduced(1.0f), 6.0f);

    g.setColour(juce::Colour(0xff101010));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 6.0f, 1.0f);

    g.setColour(juce::Colours::white);
    g.setFont(13.0f);

    g.drawText(
        name,
        getLocalBounds().reduced(8,2),
        juce::Justification::centredLeft,
        true
    );
}

void ClipComponent::resized()
{
}

void ClipComponent::mouseDown(const juce::MouseEvent& e)
{
    dragStart = e.getPosition();

    if (onSelected)
        onSelected(this);
}

void ClipComponent::mouseDrag(const juce::MouseEvent& e)
{
    auto delta = e.getPosition() - dragStart;

    if (onDragged)
        onDragged(this, delta);
}

void ClipComponent::setSelected(bool shouldBeSelected)
{
    if (selected != shouldBeSelected)
    {
        selected = shouldBeSelected;
        repaint();
    }
}

bool ClipComponent::isSelected() const noexcept
{
    return selected;
}

void ClipComponent::setClipName(const juce::String& newName)
{
    name = newName;
    repaint();
}

const juce::String& ClipComponent::getClipName() const noexcept
{
    return name;
}