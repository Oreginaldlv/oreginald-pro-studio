#include "ArrangementView.h"

void ArrangementView::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (28, 29, 33));
    g.setColour (juce::Colour::fromRGB (44, 46, 52));
    for (int x = 0; x < getWidth(); x += 80)
        g.drawVerticalLine (x, 0.0f, (float) getHeight());
    for (int y = 0; y < getHeight(); y += 48)
        g.drawHorizontalLine (y, 0.0f, (float) getWidth());
}
