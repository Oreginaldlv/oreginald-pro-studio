#include "ArrangementView.h"

void ArrangementView::setPlayheadBeats (double newPlayheadBeats)
{
    newPlayheadBeats = juce::jmax (0.0, newPlayheadBeats);

    if (! juce::approximatelyEqual (playheadBeats, newPlayheadBeats))
    {
        playheadBeats = newPlayheadBeats;
        repaint();
    }
}

void ArrangementView::setLoopState (bool shouldShowLoop, double newLoopStartBeat, double newLoopEndBeat)
{
    newLoopStartBeat = juce::jmax (0.0, newLoopStartBeat);
    newLoopEndBeat = juce::jmax (newLoopStartBeat + 1.0, newLoopEndBeat);

    const bool changed = (loopEnabled != shouldShowLoop)
                      || ! juce::approximatelyEqual (loopStartBeat, newLoopStartBeat)
                      || ! juce::approximatelyEqual (loopEndBeat, newLoopEndBeat);

    loopEnabled = shouldShowLoop;
    loopStartBeat = newLoopStartBeat;
    loopEndBeat = newLoopEndBeat;

    if (changed)
        repaint();
}

void ArrangementView::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    g.fillAll (juce::Colour::fromRGB (18, 20, 25));

    auto topTabs = bounds.removeFromTop (36);
    g.setColour (juce::Colour::fromRGB (28, 31, 38));
    g.fillRect (topTabs);

    auto arrangementTab = topTabs.removeFromLeft (140).reduced (8, 6);
    auto sessionTab = topTabs.removeFromLeft (110).reduced (8, 6);

    g.setColour (juce::Colour::fromRGB (60, 72, 110));
    g.fillRoundedRectangle (arrangementTab.toFloat(), 6.0f);
    g.setColour (juce::Colours::white);
    g.drawText ("Arrangement", arrangementTab, juce::Justification::centred);

    g.setColour (juce::Colour::fromRGB (40, 43, 50));
    g.fillRoundedRectangle (sessionTab.toFloat(), 6.0f);
    g.setColour (juce::Colours::white.withAlpha (0.82f));
    g.drawText ("Session", sessionTab, juce::Justification::centred);

    auto timeline = bounds.removeFromTop (42);
    g.setColour (juce::Colour::fromRGB (24, 26, 31));
    g.fillRect (timeline);

    constexpr int beatWidth = 80;
    constexpr int timelineOffsetX = 80;

    if (loopEnabled)
    {
        const int loopX = timelineOffsetX + (int) std::round (loopStartBeat * (double) beatWidth);
        const int loopW = (int) std::round ((loopEndBeat - loopStartBeat) * (double) beatWidth);

        g.setColour (juce::Colour::fromRGBA (90, 130, 220, 90));
        g.fillRect (loopX, timeline.getY() + 4, loopW, timeline.getHeight() - 8);

        g.setColour (juce::Colour::fromRGB (140, 180, 255));
        g.drawRect (loopX, timeline.getY() + 4, loopW, timeline.getHeight() - 8, 2);
    }

    g.setColour (juce::Colours::white.withAlpha (0.65f));
    for (int i = 0; i < 16; ++i)
    {
        const int x = timelineOffsetX + i * beatWidth;
        g.drawText (juce::String (i + 1), x - 8, 8, 30, 20, juce::Justification::centred);
    }

    auto trackArea = bounds;
    g.setColour (juce::Colour::fromRGB (20, 22, 27));
    g.fillRect (trackArea);

    const int headerWidth = 160;
    auto headers = trackArea.removeFromLeft (headerWidth);

    juce::StringArray tracks { "Drums", "Bass", "Keys", "Lead", "Vox" };

    for (int i = 0; i < tracks.size(); ++i)
    {
        auto row = juce::Rectangle<int> (headers.getX(), headers.getY() + i * 88, headers.getWidth(), 88);
        g.setColour ((i % 2 == 0) ? juce::Colour::fromRGB (29, 31, 37) : juce::Colour::fromRGB (26, 28, 34));
        g.fillRect (row);

        g.setColour (juce::Colours::white.withAlpha (0.85f));
        g.drawText (tracks[i], row.reduced (14, 0), juce::Justification::centredLeft);
    }

    for (int i = 0; i < 5; ++i)
    {
        const int y = trackArea.getY() + i * 88;
        g.setColour (juce::Colour::fromRGB (38, 42, 48));
        g.drawHorizontalLine (y, (float) trackArea.getX(), (float) trackArea.getRight());
    }

    for (int x = trackArea.getX(); x < trackArea.getRight(); x += beatWidth)
    {
        g.setColour (juce::Colour::fromRGB (34, 37, 42));
        g.drawVerticalLine (x, (float) trackArea.getY(), (float) trackArea.getBottom());
    }

    g.setColour (juce::Colour::fromRGB (92, 124, 210));
    g.fillRoundedRectangle ((float) trackArea.getX() + 30.0f, (float) trackArea.getY() + 18.0f, 220.0f, 44.0f, 8.0f);

    g.setColour (juce::Colour::fromRGB (110, 190, 150));
    g.fillRoundedRectangle ((float) trackArea.getX() + 180.0f, (float) trackArea.getY() + 106.0f, 180.0f, 44.0f, 8.0f);

    g.setColour (juce::Colour::fromRGB (210, 130, 92));
    g.fillRoundedRectangle ((float) trackArea.getX() + 360.0f, (float) trackArea.getY() + 194.0f, 260.0f, 44.0f, 8.0f);

    g.setColour (juce::Colour::fromRGB (255, 255, 255).withAlpha (0.25f));
    const int playheadX = trackArea.getX() + (int) std::round (playheadBeats * (double) beatWidth);
    g.drawLine ((float) playheadX, (float) trackArea.getY(), (float) playheadX, (float) trackArea.getBottom(), 2.0f);
}