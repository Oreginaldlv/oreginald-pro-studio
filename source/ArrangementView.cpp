#include "ArrangementView.h"

ArrangementView::ArrangementView(TrackEngine& engine)
    : trackEngine(engine)
{
    createDemoClips();
}

void ArrangementView::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101010));

    const int numTracks = trackEngine.getNumTracks();
    const int trackHeight = getTrackHeight();

    for (int i = 0; i < numTracks; ++i)
    {
        const int y = i * trackHeight;
        const auto& track = trackEngine.getTrack(i);
        const bool isSelected = (i == trackEngine.getSelectedTrackIndex());

        g.setColour(isSelected ? juce::Colour(0xff2a2f36)
                               : juce::Colour(0xff1a1a1a));
        g.fillRect(0, y, getWidth(), trackHeight - 1);

        g.setColour(juce::Colour(0xff303030));
        g.drawLine(0.0f, (float) y, (float) getWidth(), (float) y, 1.0f);

        auto left = juce::Rectangle<int>(8, y + 8, 110, trackHeight - 16);

        g.setColour(juce::Colours::white);
        g.drawText(track.name,
                   left.removeFromTop(22),
                   juce::Justification::centredLeft);

        juce::String trackFlags;
        trackFlags << (track.armed ? "[ARM] " : "")
                   << (track.muted ? "[MUTE] " : "")
                   << (track.solo  ? "[SOLO]" : "");

        g.setColour(track.armed ? juce::Colours::red : juce::Colours::lightgrey);
        g.drawText(trackFlags,
                   left,
                   juce::Justification::centredLeft);

        auto meterArea = juce::Rectangle<int>(130, y + 14, getWidth() - 260, 14);

        g.setColour(juce::Colour(0xff222222));
        g.fillRect(meterArea);

        const float audible = trackEngine.isTrackAudible(i) ? 1.0f : 0.25f;
        const int meterWidth = (int) ((float) meterArea.getWidth()
                            * juce::jlimit(0.0f, 1.0f, track.volume / 1.5f)
                            * audible);

        auto filledMeter = meterArea;
        filledMeter.setWidth(meterWidth);

        g.setColour(track.muted ? juce::Colours::darkgrey
                                : track.solo ? juce::Colours::yellow
                                             : juce::Colours::green);
        g.fillRect(filledMeter);

        g.setColour(juce::Colour(0xff404040));
        g.drawRect(juce::Rectangle<int>(130, y + 14, getWidth() - 260, 14));

        g.setColour(juce::Colours::lightgrey);
        g.drawText("Pan: " + juce::String(track.pan, 2),
                   getWidth() - 120, y + 10, 100, 20,
                   juce::Justification::centredRight);
    }

    g.setColour(juce::Colour(0xff303030));
    g.drawRect(getLocalBounds(), 1);
}

void ArrangementView::resized()
{
    layoutClips();
}

void ArrangementView::mouseDown(const juce::MouseEvent& e)
{
    const int clickedTrack = e.y / getTrackHeight();

    if (juce::isPositiveAndBelow(clickedTrack, trackEngine.getNumTracks()))
    {
        trackEngine.setSelectedTrackIndex(clickedTrack);
        selectedClipIndex = -1;

        for (int i = 0; i < clipComponents.size(); ++i)
            clipComponents[i]->setSelected(false);

        repaint();
    }
}

void ArrangementView::createDemoClips()
{
    clipData.clear();
    clipComponents.clear();

    clipData.add({ 0, 180, 120, "Drum Loop A" });
    clipData.add({ 1, 220, 140, "Bass Verse" });
    clipData.add({ 2, 320, 150, "Keys Pad" });
    clipData.add({ 3, 420, 130, "Lead Hook" });
    clipData.add({ 4, 520, 160, "Vox Main" });

    for (int i = 0; i < clipData.size(); ++i)
    {
        auto* comp = clipComponents.add(new ClipComponent(clipData[i].name));

        comp->onSelected = [this](ClipComponent* c)
        {
            handleClipSelected(c);
        };

        comp->onDragged = [this](ClipComponent* c, juce::Point<int> delta)
        {
            handleClipDragged(c, delta);
        };

        addAndMakeVisible(comp);
    }

    layoutClips();
}

void ArrangementView::layoutClips()
{
    const int trackHeight = getTrackHeight();
    const int clipYInset = 20;
    const int clipHeight = 28;

    for (int i = 0; i < clipComponents.size(); ++i)
    {
        const auto& clip = clipData.getReference(i);
        const int clipY = clip.trackIndex * trackHeight + clipYInset;

        clipComponents[i]->setBounds(clip.x, clipY, clip.width, clipHeight);
        clipComponents[i]->setSelected(i == selectedClipIndex);
    }
}

void ArrangementView::handleClipDragged(ClipComponent* clip, juce::Point<int> delta)
{
    const int index = clipComponents.indexOf(clip);

    if (! juce::isPositiveAndBelow(index, clipData.size()))
        return;

    auto& data = clipData.getReference(index);
    data.x = juce::jmax(130, data.x + delta.x);

    layoutClips();
    repaint();
}

void ArrangementView::handleClipSelected(ClipComponent* clip)
{
    selectedClipIndex = clipComponents.indexOf(clip);

    if (! juce::isPositiveAndBelow(selectedClipIndex, clipData.size()))
        return;

    trackEngine.setSelectedTrackIndex(clipData[selectedClipIndex].trackIndex);
    layoutClips();
    repaint();
}