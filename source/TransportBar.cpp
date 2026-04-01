#include "TransportBar.h"

TransportBar::TransportBar (TransportState& state)
    : transportState (state)
{
    playButton.onClick = [this]
    {
        transportState.setPlaying (true);
    };
    addAndMakeVisible (playButton);

    stopButton.onClick = [this]
    {
        transportState.setPlaying (false);
    };
    addAndMakeVisible (stopButton);

    recordButton.onClick = [this]
    {
        transportState.setRecording (! transportState.isRecording());
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