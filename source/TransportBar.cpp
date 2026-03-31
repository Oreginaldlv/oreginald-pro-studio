#include "TransportBar.h"

namespace
{
    juce::String formatBarsBeats (double beats)
    {
        const auto wholeBeats = juce::jmax (0, (int) std::floor (beats));
        const int bars = (wholeBeats / 4) + 1;
        const int beatInBar = (wholeBeats % 4) + 1;
        return juce::String (bars) + "." + juce::String (beatInBar) + ".1";
    }
}

TransportBar::TransportBar (TransportState& state)
    : transportState (state)
{
    transportState.addListener (this);

    addAndMakeVisible (playButton);
    addAndMakeVisible (stopButton);
    addAndMakeVisible (recordButton);
    addAndMakeVisible (bpmDownButton);
    addAndMakeVisible (bpmUpButton);
    addAndMakeVisible (loopToggle);
    addAndMakeVisible (metroToggle);
    addAndMakeVisible (titleLabel);
    addAndMakeVisible (bpmLabel);
    addAndMakeVisible (timeLabel);

    titleLabel.setText ("Oreginald Pro Studio", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setFont (juce::FontOptions (20.0f, juce::Font::bold));

    bpmLabel.setJustificationType (juce::Justification::centred);
    timeLabel.setJustificationType (juce::Justification::centred);

    playButton.onClick = [this] { transportState.play(); };
    stopButton.onClick = [this] { transportState.stop(); };
    recordButton.onClick = [this] { transportState.toggleRecordArmed(); };

    loopToggle.onClick = [this]
    {
        transportState.setLoopEnabled (loopToggle.getToggleState());
    };

    metroToggle.onClick = [this]
    {
        transportState.setMetronomeEnabled (metroToggle.getToggleState());
    };

    bpmDownButton.onClick = [this] { transportState.nudgeBpm (-5.0); };
    bpmUpButton.onClick   = [this] { transportState.nudgeBpm (5.0); };

    refreshFromState();
}

TransportBar::~TransportBar()
{
    transportState.removeListener (this);
}

void TransportBar::transportStateChanged()
{
    refreshFromState();
    repaint();
}

void TransportBar::refreshFromState()
{
    playButton.setEnabled (! transportState.isPlaying());
    stopButton.setEnabled (transportState.isPlaying());

    recordButton.setToggleState (transportState.isRecordArmed(), juce::dontSendNotification);
    loopToggle.setToggleState (transportState.isLoopEnabled(), juce::dontSendNotification);
    metroToggle.setToggleState (transportState.isMetronomeEnabled(), juce::dontSendNotification);

    bpmLabel.setText ("BPM " + juce::String ((int) std::round (transportState.getBpm())),
                      juce::dontSendNotification);

    timeLabel.setText (formatBarsBeats (transportState.getPlayheadBeats()),
                       juce::dontSendNotification);
}

void TransportBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (26, 28, 34));
    g.setColour (juce::Colour::fromRGB (50, 54, 62));
    g.drawLine (0.0f, (float) getHeight() - 1.0f, (float) getWidth(), (float) getHeight() - 1.0f, 1.0f);
}

void TransportBar::resized()
{
    auto area = getLocalBounds().reduced (10, 8);

    auto left = area.removeFromLeft (250);
    titleLabel.setBounds (left);

    auto controls = area.removeFromLeft (470);
    playButton.setBounds (controls.removeFromLeft (70).reduced (4, 0));
    stopButton.setBounds (controls.removeFromLeft (70).reduced (4, 0));
    recordButton.setBounds (controls.removeFromLeft (70).reduced (4, 0));
    loopToggle.setBounds (controls.removeFromLeft (55));
    metroToggle.setBounds (controls.removeFromLeft (55));
    bpmDownButton.setBounds (controls.removeFromLeft (36).reduced (2, 0));
    bpmUpButton.setBounds (controls.removeFromLeft (36).reduced (2, 0));

    auto right = area.removeFromRight (180);
    timeLabel.setBounds (right.removeFromRight (90));
    bpmLabel.setBounds (right.removeFromRight (90));
}