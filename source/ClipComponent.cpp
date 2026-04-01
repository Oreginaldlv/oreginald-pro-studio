#include "ClipComponent.h"
#include <mutex>

ClipComponent::ClipComponent(const juce::String& clipName)
    : name(clipName),
      thumbnail(512, getSharedFormatManager(), getSharedThumbnailCache())
{
}

void ClipComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::Colour baseColour = selected ? juce::Colour(0xff4c6fff)
                                       : juce::Colour(0xff3a8f66);
    if (! audible)
        baseColour = baseColour.interpolatedWith(juce::Colours::darkgrey, 0.55f);

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds.reduced(1.0f), 6.0f);

    g.setColour(juce::Colour(0xff101010));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 6.0f, 1.0f);

    auto handleArea = getLocalBounds().removeFromRight(resizeHandleWidth);
    g.setColour(juce::Colour(0x55ffffff));
    g.fillRect(handleArea.reduced(2, 3));

    auto waveformArea = getLocalBounds().reduced(6, 6);
    waveformArea.removeFromRight(resizeHandleWidth + 4);

    if (hasFile && thumbnail.getTotalLength() > 0.0)
    {
        g.setColour(juce::Colours::black.withAlpha(0.25f));
        g.fillRect(waveformArea);

        g.setColour(selected ? juce::Colours::white : juce::Colours::lightgrey);
        thumbnail.drawChannels(g, waveformArea, 0.0, thumbnail.getTotalLength(), 1.0f);
    }

    g.setColour(juce::Colours::white);
    g.setFont(13.0f);
    g.drawText(name,
               getLocalBounds().reduced(8, 2).withTrimmedRight(resizeHandleWidth + 2),
               juce::Justification::centredLeft,
               true);
}

void ClipComponent::resized()
{
}

void ClipComponent::mouseMove(const juce::MouseEvent& e)
{
    setMouseCursor(isNearRightEdge(e.x)
                       ? juce::MouseCursor::LeftRightResizeCursor
                       : juce::MouseCursor::NormalCursor);
}

void ClipComponent::mouseExit(const juce::MouseEvent&)
{
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

void ClipComponent::mouseDown(const juce::MouseEvent& e)
{
    dragStart = e.getPosition();
    dragStartBounds = getBounds();
    resizing = isNearRightEdge(e.x);

    if (onSelected)
        onSelected(this);
}

void ClipComponent::mouseDrag(const juce::MouseEvent& e)
{
    auto delta = e.getPosition() - dragStart;

    if (resizing)
    {
        if (onResized)
            onResized(this, dragStartBounds.getRight() + delta.x);
    }
    else
    {
        if (onDragged)
            onDragged(this, dragStartBounds.getX() + delta.x);
    }
}

void ClipComponent::mouseUp(const juce::MouseEvent&)
{
    resizing = false;
    setMouseCursor(juce::MouseCursor::NormalCursor);
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

void ClipComponent::setClipFile(const juce::String& newPath)
{
    if (filePath == newPath && hasFile)
        return;

    filePath = newPath;
    hasFile = false;
    thumbnail.clear();

    if (filePath.isEmpty())
    {
        repaint();
        return;
    }

    const juce::File file(filePath);

    if (! file.existsAsFile())
    {
        repaint();
        return;
    }

    if (thumbnail.setSource(new juce::FileInputSource(file)))
    {
        hasFile = true;
    }
    else
    {
        thumbnail.setSource(nullptr);
    }

    repaint();
}

void ClipComponent::setAudible(bool isAudible)
{
    if (audible != isAudible)
    {
        audible = isAudible;
        repaint();
    }
}

bool ClipComponent::isNearRightEdge(int x) const noexcept
{
    return x >= getWidth() - resizeHandleWidth;
}

juce::AudioFormatManager& ClipComponent::getSharedFormatManager()
{
    static juce::AudioFormatManager manager;
    static std::once_flag initFlag;
    std::call_once(initFlag, [] { manager.registerBasicFormats(); });
    return manager;
}

juce::AudioThumbnailCache& ClipComponent::getSharedThumbnailCache()
{
    static juce::AudioThumbnailCache cache(64);
    return cache;
}
