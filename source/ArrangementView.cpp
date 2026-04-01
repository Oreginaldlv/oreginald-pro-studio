#include "ArrangementView.h"

namespace
{
    static constexpr double minClipBeats = 1.0;
}

ArrangementView::ArrangementView(TrackEngine& engine, TransportState& state)
    : trackEngine(engine), transportState(state)
{
    setWantsKeyboardFocus(true);
    ensureClipComponents();
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
    grabKeyboardFocus();
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

bool ArrangementView::keyPressed(const juce::KeyPress& key)
{
    if (selectedClipIndex < 0)
        return false;

    if (key == juce::KeyPress::deleteKey || key == juce::KeyPress::backspaceKey)
    {
        if (trackEngine.removeClip(selectedClipIndex))
        {
            selectedClipIndex = -1;
            refreshFromModel();

            if (onClipDataChanged)
                onClipDataChanged();

            return true;
        }
    }

    if (key.getModifiers().isCommandDown() && key.getTextCharacter() == 'd')
    {
        const int duplicatedIndex = trackEngine.duplicateClip(selectedClipIndex);

        if (duplicatedIndex >= 0)
        {
            selectedClipIndex = duplicatedIndex;
            const auto& duplicatedClip = trackEngine.getClip(selectedClipIndex);
            trackEngine.setSelectedTrackIndex(duplicatedClip.trackIndex);

            refreshFromModel();

            if (onClipDataChanged)
                onClipDataChanged();

            return true;
        }
    }

    const auto textChar = key.getTextCharacter();
    const auto isCommand = key.getModifiers().isCommandDown();

    if (isCommand && (textChar == 's' || textChar == 'S'))
    {
        if (onSaveRequested)
            onSaveRequested();

        return true;
    }

    if (isCommand && (textChar == 'o' || textChar == 'O'))
    {
        if (onLoadRequested)
            onLoadRequested();

        return true;
    }

    return false;
}

bool ArrangementView::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& file : files)
        if (file.endsWithIgnoreCase(".wav"))
            return true;

    return false;
}

void ArrangementView::filesDropped(const juce::StringArray& files, int x, int y)
{
    if (onAudioFileDropped == nullptr)
        return;

    const int trackIndex = getTrackIndexForY(y);

    if (! juce::isPositiveAndBelow(trackIndex, trackEngine.getNumTracks()))
        return;

    const double startBeat = getBeatForX(x);

    for (const auto& path : files)
    {
        const juce::File file(path);

        if (file.existsAsFile() && file.hasFileExtension("wav"))
            onAudioFileDropped(file, trackIndex, startBeat);
    }
}

void ArrangementView::refreshFromModel()
{
    ensureClipComponents();
    layoutClips();
    repaint();
}

void ArrangementView::resetSelection()
{
    selectedClipIndex = -1;

    for (int i = 0; i < clipComponents.size(); ++i)
        clipComponents[i]->setSelected(false);
}

void ArrangementView::ensureClipComponents()
{
    const int clipCount = trackEngine.getClipCount();

    if (clipComponents.size() != clipCount)
    {
        clipComponents.clear();

        for (int i = 0; i < clipCount; ++i)
        {
            auto* comp = clipComponents.add(new ClipComponent(trackEngine.getClip(i).name));

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

        if (selectedClipIndex >= clipCount)
            selectedClipIndex = -1;
    }

    refreshClipComponents();
}

void ArrangementView::refreshClipComponents()
{
    const int clipCount = trackEngine.getClipCount();

    for (int i = 0; i < clipComponents.size() && i < clipCount; ++i)
    {
        clipComponents[i]->setClipName(trackEngine.getClip(i).name);
        clipComponents[i]->setSelected(i == selectedClipIndex);
    }
}

void ArrangementView::layoutClips()
{
    ensureClipComponents();

    const int trackHeight = getTrackHeight();
    const int clipYInset = 20;
    const int clipHeight = 28;
    const int rulerHeight = grid.getRulerHeight();
    const int timelineStartX = getTimelineStartX();
    const int pixelsPerBeat = grid.getPixelsPerBeat();

    for (int i = 0; i < clipComponents.size(); ++i)
    {
        const auto& clip = trackEngine.getClip(i);
        const int clipX = timelineStartX + (int) std::round(clip.startBeat * pixelsPerBeat);
        const int clipWidth = juce::jmax(pixelsPerBeat, (int) std::round(clip.lengthBeats * pixelsPerBeat));
        const int clipY = rulerHeight + clip.trackIndex * trackHeight + clipYInset;

        clipComponents[i]->setBounds(clipX, clipY, clipWidth, clipHeight);
        clipComponents[i]->setSelected(i == selectedClipIndex);
    }
}

void ArrangementView::handleClipDragged(ClipComponent* clip, int newX)
{
    const int index = clipComponents.indexOf(clip);

    if (!juce::isPositiveAndBelow(index, trackEngine.getClipCount()))
        return;

    const double beat = juce::jmax(0.0,
                                    (double) (newX - getTimelineStartX())
                                        / (double) grid.getPixelsPerBeat());

    trackEngine.setClipStart(index, beat);

    layoutClips();
    refreshClipComponents();

    if (onClipDataChanged)
        onClipDataChanged();
}

void ArrangementView::handleClipResized(ClipComponent* clip, int newRightX)
{
    const int index = clipComponents.indexOf(clip);

    if (!juce::isPositiveAndBelow(index, trackEngine.getClipCount()))
        return;

    const auto& clipData = trackEngine.getClip(index);
    const double targetLengthBeats = (double) (newRightX - getTimelineStartX()) / (double) grid.getPixelsPerBeat()
                                     - clipData.startBeat;

    trackEngine.setClipLength(index, juce::jmax(minClipBeats, targetLengthBeats));

    layoutClips();
    refreshClipComponents();

    if (onClipDataChanged)
        onClipDataChanged();
}

void ArrangementView::handleClipSelected(ClipComponent* clip)
{
    selectedClipIndex = clipComponents.indexOf(clip);

    if (!juce::isPositiveAndBelow(selectedClipIndex, trackEngine.getClipCount()))
        return;

    trackEngine.setSelectedTrackIndex(trackEngine.getClip(selectedClipIndex).trackIndex);
    layoutClips();
    refreshClipComponents();
    repaint();
}

void ArrangementView::setPlayheadFromX(int x)
{
    const int timelineStartX = getTimelineStartX();
    const int snappedX = grid.snapX(juce::jmax(timelineStartX, x));
    const double beats = (snappedX - timelineStartX) / (double) grid.getPixelsPerBeat();

    transportState.setPlayheadBeats(beats);

    if (onPlayheadMoved)
        onPlayheadMoved();
}

int ArrangementView::getTrackIndexForY(int y) const noexcept
{
    return (y - getTimelineTop()) / getTrackHeight();
}

double ArrangementView::getBeatForX(int x) const noexcept
{
    const int timelineStartX = getTimelineStartX();
    const int snappedX = grid.snapX(juce::jmax(timelineStartX, x));
    return (snappedX - timelineStartX) / (double) grid.getPixelsPerBeat();
}
