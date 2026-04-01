#pragma once
#include <JuceHeader.h>

class TimelineGrid
{
public:
    void setTempo(double bpm)
    {
        tempo = bpm;
    }

    int getPixelsPerBeat() const noexcept
    {
        return pixelsPerBeat;
    }

    int getPixelsPerBar() const noexcept
    {
        return pixelsPerBeat * beatsPerBar;
    }

    int getRulerHeight() const noexcept
    {
        return rulerHeight;
    }

    int snapX(int x) const noexcept
    {
        const int relativeX = juce::jmax(0, x - timelineStartX);
        const int beat = relativeX / pixelsPerBeat;
        return timelineStartX + beat * pixelsPerBeat;
    }

    void drawRuler(juce::Graphics& g, juce::Rectangle<int> area) const
    {
        g.setColour(juce::Colour(0xff181818));
        g.fillRect(area);

        for (int x = area.getX(); x < area.getRight(); x += pixelsPerBeat)
        {
            const int relative = x - area.getX();
            const bool isBarLine = (relative % getPixelsPerBar()) == 0;

            g.setColour(isBarLine ? juce::Colour(0xff505050)
                                  : juce::Colour(0xff2d2d2d));

            g.drawVerticalLine(x, (float)area.getY(), (float)area.getBottom());

            if (isBarLine)
            {
                const int barNumber = (relative / getPixelsPerBar()) + 1;
                g.setColour(juce::Colours::lightgrey);
                g.drawText(juce::String(barNumber),
                           x + 4,
                           area.getY() + 2,
                           40,
                           area.getHeight() - 4,
                           juce::Justification::centredLeft);
            }
        }

        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawLine((float)area.getX(), (float)area.getBottom(),
                   (float)area.getRight(), (float)area.getBottom(), 1.0f);
    }

    void drawLanes(juce::Graphics& g, juce::Rectangle<int> area) const
    {
        g.setColour(juce::Colour(0xff262626));

        for (int x = area.getX(); x < area.getRight(); x += pixelsPerBeat)
            g.drawVerticalLine(x, (float)area.getY(), (float)area.getBottom());

        g.setColour(juce::Colour(0xff333333));

        for (int x = area.getX(); x < area.getRight(); x += getPixelsPerBar())
            g.drawVerticalLine(x, (float)area.getY(), (float)area.getBottom());
    }

private:
    double tempo { 120.0 };
    int pixelsPerBeat { 60 };
    int beatsPerBar { 4 };
    int rulerHeight { 24 };
    int timelineStartX { 130 };
};
