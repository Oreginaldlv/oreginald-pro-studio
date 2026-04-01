#include "TransportBar.h"

TransportBar::TransportBar (TransportState& state)
    : transportState (state)
{
    playButton.onClick = [this]
    {
        transportState.setPlaying (true);
        syncFromTransportState();
    };
    addAndMakeVisible (playButton);

    stopButton.onClick = [this]
    {
        transportState.setPlaying (false);
        syncFromTransportState();
    };
    addAndMakeVisible (stopButton);

    recordButton.onClick = [this]
    {
        transportState.setRecording (! transportState.isRecording());
        syncFromTransportState();
    };
    addAndMakeVisible (recordButton);

    tempoLabel.setText ("Tempo", juce::dontSendNotification);
    tempoLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (tempoLabel);

    tempoSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    tempoSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
    tempoSlider.setRange (40.0, 240.0, 0.1);

    tempoSlider.onValueChange = [this]
    {
        transportState.setTempo ((float) tempoSlider.getValue());
    };

    tempoSlider.setValue (transportState.getTempo(),
                          juce::dontSendNotification);

    addAndMakeVisible (tempoSlider);

    syncFromTransportState();
}

void TransportBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff202020));

    g.setColour (juce::Colour (0xff2a2a2a));
    g.drawLine (0.0f,
                (float) getHeight(),
                (float) getWidth(),
                (float) getHeight(),
                1.0f);
}

void TransportBar::resized()
{
    auto area = getLocalBounds().reduced (10);

    auto buttonArea = area.removeFromLeft (260);

    playButton.setBounds (buttonArea.removeFromLeft (70));
    buttonArea.removeFromLeft (6);

    stopButton.setBounds (buttonArea.removeFromLeft (70));
    buttonArea.removeFromLeft (6);

    recordButton.setBounds (buttonArea.removeFromLeft (90));

    area.removeFromLeft (20);

    tempoLabel.setBounds (area.removeFromLeft (60));
    tempoSlider.setBounds (area.removeFromLeft (220));
}

void TransportBar::syncFromTransportState()
{
    tempoSlider.setValue(transportState.getTempo(), juce::dontSendNotification);
    updateButtonColours();
    repaint();
}

void TransportBar::updateButtonColours()
{
    playButton.setColour(juce::TextButton::buttonColourId,
                         transportState.isPlaying() ? juce::Colour(0xff2f7d4c)
                                                    : juce::Colour(0xff404040));

    stopButton.setColour(juce::TextButton::buttonColourId,
                         transportState.isPlaying() ? juce::Colour(0xff404040)
                                                    : juce::Colour(0xff5a5a5a));

    recordButton.setColour(juce::TextButton::buttonColourId,
                           transportState.isRecording() ? juce::Colour(0xff9d3131)
                                                        : juce::Colour(0xff404040));
}
