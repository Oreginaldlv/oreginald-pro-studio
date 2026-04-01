#pragma once
#include <JuceHeader.h>
#include "TrackEngine.h"
#include "ClipComponent.h"
#include "TimelineGrid.h"

class ArrangementView : public juce::Component
{
public:
    explicit ArrangementView(TrackEngine& engine);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;

private:

    struct ClipData
    {
        int trackIndex { 0 };
        int x { 0 };
        int width { 120 };
        juce::String name;
    };

    int getTrackHeight() const noexcept { return 60; }

    void createDemoClips();
    void layoutClips();

    void handleClipDragged(ClipComponent*, juce::Point<int>);
    void handleClipSelected(ClipComponent*);

    TrackEngine& trackEngine;
    TimelineGrid grid;
    
    juce::OwnedArray<ClipComponent> clipComponents;
    juce::Array<ClipData> clipData;

    int selectedClipIndex { -1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArrangementView)
};