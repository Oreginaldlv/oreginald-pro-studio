#include "ArrangementView.h"

ArrangementView::ArrangementView(TrackEngine& engine, TransportState& state)
    : trackEngine(engine), transportState(state)
{
    createDemoClips();
}

void ArrangementView::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff101010));
    grid.setTempo(transportState.getTempo());

    const int numTracks = trackEngine.getNumTracks();
    const int trackHeight = getTrackHeight();
    const int rulerHeight = grid.getRulerHeight();
    const int timelineStartX = getTimelineStartX();

    auto rulerArea = juce::Rectangle<int>(timelineStartX, 0, getWidth() - timelineStartX, rulerHeight);
    grid.drawRuler(g, rulerArea);

    for (int i = 0; i < numTracks; ++i)
    {
        const int y = rulerHeight + i * trackHeight;
        const auto& track = trackEngine.getTrack(i);
        const bool isSelected = (i == trackEngine.getSelectedTrackIndex());

        g.setColour(isSelected ? juce::Colour(0xff2a2f36)
                               : juce::Colour(0xff1a1a1a));
        g.fillRect(0, y, getWidth(), trackHeight - 1);

        auto laneGridArea = juce::Rectangle<int>(timelineStartX, y, getWidth() - timelineStartX, trackHeight - 1);
        grid.drawLanes(g, laneGridArea);

        g.setColour(juce::Colour(0xff303030));
        g.drawLine(0.0f, (float)y, (float)getWidth(), (float)y, 1.0f);

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
        g.drawText(trackFlags, left, juce::Justification::centredLeft);

        auto meterArea = juce::Rectangle<int>(timelineStartX, y + 14, getWidth() - 260, 14);

        g.setColour(juce::Colour(0xff222222));
        g.fillRect(meterArea);

        const float audible = trackEngine.isTrackAudible(i) ? 1.0f : 0.25f;
        const int meterWidth = (int)((float) meterArea.getWidth()
                            * juce::jlimit(0.0f, 1.0f, track.volume / 1.5f)
                            * audible);

        auto filledMeter = meterArea;
        filledMeter.setWidth(meterWidth);

        g.setColour(track.muted ? juce::Colours::darkgrey
                                : track.solo ? juce::Colours::yellow
                                             : juce::Colours::green);
        g.fillRect(filledMeter);

        g.setColour(juce::Colour(0xff404040));
        g.drawRect(juce::Rectangle<int>(timelineStartX, y + 14, getWidth() - 260, 14));

        g.setColour(juce::Colours::lightgrey);
        g.drawText("Pan: " + juce::String(track.pan, 2),
                   getWidth() - 120, y + 10, 100, 20,
                   juce::Justification::centredRight);
    }

    const int playheadX = timelineStartX
                        + (int) std::round(transportState.getPlayheadBeats() * grid.getPixelsPerBeat());

    if (playheadX >= timelineStartX && playheadX < getWidth())
    {
        g.setColour(juce::Colours::white);
        g.drawLine((float) playheadX, 0.0f, (float) playheadX, (float) getHeight(), 2.0f);
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
    const int timelineStartX = getTimelineStartX();

    if (e.y < getTimelineTop() && e.x >= timelineStartX)
    {
        setPlayheadFromX(e.x);
        repaint();
        return;
    }

    if (e.x >= timelineStartX)
    {
        setPlayheadFromX(e.x);
    }

    const int clickedTrack = (e.y - getTimelineTop()) / getTrackHeight();

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
    clipData.add({ 1, 240, 140, "Bass Verse" });
    clipData.add({ 2, 300, 150, "Keys Pad" });
    clipData.add({ 3, 360, 130, "Lead Hook" });
    clipData.add({ 4, 420, 160, "Vox Main" });

    for (int i = 0; i < clipData.size(); ++i)
    {
        auto* comp = clipComponents.add(new ClipComponent(clipData[i].name));

        comp->onSelected = [this](ClipComponent* c)
        {
            handleClipSelected(c);
        };

        comp->onDragged = [this](ClipComponent* c, int newX)
        {
            handleClipDragged(c, newX);
        };

        comp->onResized = [this](ClipComponent* c, int newRightX)
        {
            handleClipResized(c, newRightX);
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
    const int rulerHeight = grid.getRulerHeight();

    for (int i = 0; i < clipComponents.size(); ++i)
    {
        const auto& clip = clipData.getReference(i);
        const int clipY = rulerHeight + clip.trackIndex * trackHeight + clipYInset;

        clipComponents[i]->setBounds(clip.x, clipY, clip.width, clipHeight);
        clipComponents[i]->setSelected(i == selectedClipIndex);
    }
}

void ArrangementView::handleClipDragged(ClipComponent* clip, int newX)
{
    const int index = clipComponents.indexOf(clip);

    if (!juce::isPositiveAndBelow(index, clipData.size()))
        return;

    auto& data = clipData.getReference(index);
    data.x = grid.snapX(juce::jmax(getTimelineStartX(), newX));

    layoutClips();
    repaint();
}

void ArrangementView::handleClipResized(ClipComponent* clip, int newRightX)
{
    const int index = clipComponents.indexOf(clip);

    if (!juce::isPositiveAndBelow(index, clipData.size()))
        return;

    auto& data = clipData.getReference(index);

    const int minWidth = grid.getPixelsPerBeat();
    const int snappedRight = grid.snapX(juce::jmax(data.x + minWidth, newRightX));

    data.width = juce::jmax(minWidth, snappedRight - data.x);

    layoutClips();
    repaint();
}

void ArrangementView::handleClipSelected(ClipComponent* clip)
{
    selectedClipIndex = clipComponents.indexOf(clip);

    if (!juce::isPositiveAndBelow(selectedClipIndex, clipData.size()))
        return;

    trackEngine.setSelectedTrackIndex(clipData[selectedClipIndex].trackIndex);
    layoutClips();
    repaint();
}

void ArrangementView::setPlayheadFromX(int x)
{
    const int timelineStartX = getTimelineStartX();
    const int snappedX = grid.snapX(juce::jmax(timelineStartX, x));
    const double beats = (double) (snappedX - timelineStartX) / (double) grid.getPixelsPerBeat();

    transportState.setPlayheadBeats(beats);
}
