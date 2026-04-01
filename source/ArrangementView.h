#pragma once
#include <JuceHeader.h>
#include "TrackEngine.h"
#include "ClipComponent.h"
#include "TimelineGrid.h"
#include "TransportState.h"

class ArrangementView : public juce::Component
{
public:
    ArrangementView(TrackEngine& engine, TransportState& state);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    std::function<void()> onClipDataChanged;
    std::function<void()> onPlayheadMoved;

private:
    int getTrackHeight() const noexcept { return 60; }
    int getTimelineTop() const noexcept { return grid.getRulerHeight(); }
    int getTimelineStartX() const noexcept { return 130; }

    void createDemoClips();
    void layoutClips();
    void handleClipDragged(ClipComponent*, int newX);
    void handleClipResized(ClipComponent*, int newRightX);
    void handleClipSelected(ClipComponent*);
    void setPlayheadFromX(int x);
    void ensureClipComponents();
    void refreshClipComponents();

    TrackEngine& trackEngine;
    TransportState& transportState;
    TimelineGrid grid;

    juce::OwnedArray<ClipComponent> clipComponents;

    int selectedClipIndex { -1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArrangementView)
};
