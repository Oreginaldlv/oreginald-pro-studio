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

    int snapX(int x) const noexcept
    {
        int beat = x / pixelsPerBeat;
        return beat * pixelsPerBeat;
    }

    void draw(juce::Graphics& g, juce::Rectangle<int> area) const
    {
        g.setColour(juce::Colour(0xff262626));

        for (int x = area.getX(); x < area.getRight(); x += pixelsPerBeat)
        {
            g.drawVerticalLine(x, (float)area.getY(), (float)area.getBottom());
        }

        g.setColour(juce::Colour(0xff333333));

        for (int x = area.getX(); x < area.getRight(); x += pixelsPerBeat * 4)
        {
            g.drawVerticalLine(x, (float)area.getY(), (float)area.getBottom());
        }
    }

private:

    double tempo { 120.0 };
    int pixelsPerBeat { 60 };
};