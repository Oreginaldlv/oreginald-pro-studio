#pragma once
#include <JuceHeader.h>
#include "TrackEngine.h"
#include "ClipComponent.h"
#include "TimelineGrid.h"
#include "TransportState.h"

class ArrangementView : public juce::Component,
                        public juce::FileDragAndDropTarget
{
public:
    ArrangementView(TrackEngine& engine, TransportState& state);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    std::function<void()> onClipDataChanged;
    std::function<void()> onPlayheadMoved;
    std::function<void()> onSaveRequested;
    std::function<void()> onLoadRequested;
    std::function<void(const juce::File&, int trackIndex, double startBeat)> onAudioFileDropped;

    void refreshFromModel();
    void resetSelection();

private:
    int getTrackHeight() const noexcept { return 60; }
    int getTimelineTop() const noexcept { return grid.getRulerHeight(); }
    int getTimelineStartX() const noexcept { return 130; }
    int getTrackIndexForY(int y) const noexcept;
    double getBeatForX(int x) const noexcept;

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
