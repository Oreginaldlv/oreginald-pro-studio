#include "InspectorPanel.h"

InspectorPanel::InspectorPanel (TrackEngine& engine)
    : trackEngine (engine)
{
    headerLabel.setText ("INSPECTOR", juce::dontSendNotification);
    addAndMakeVisible (headerLabel);

    trackSelector.onChange = [this]
    {
        trackEngine.setSelectedTrackIndex(trackSelector.getSelectedItemIndex());
        syncFromTrackEngine();
    };

    addAndMakeVisible(trackSelector);

    volumeLabel.setText("Volume", juce::dontSendNotification);
    addAndMakeVisible(volumeLabel);

    volumeSlider.setRange(0.0, 1.5, 0.01);
    volumeSlider.onValueChange = [this]
    {
        auto index = trackEngine.getSelectedTrackIndex();
        trackEngine.setTrackVolume(index, (float)volumeSlider.getValue());
    };

    addAndMakeVisible(volumeSlider);

    panLabel.setText("Pan", juce::dontSendNotification);
    addAndMakeVisible(panLabel);

    panSlider.setRange(-1.0, 1.0, 0.01);
    panSlider.onValueChange = [this]
    {
        auto index = trackEngine.getSelectedTrackIndex();
        trackEngine.setTrackPan(index, (float)panSlider.getValue());
    };

    addAndMakeVisible(panSlider);

    armToggle.onClick = [this]
    {
        auto index = trackEngine.getSelectedTrackIndex();
        trackEngine.setTrackArmed(index, armToggle.getToggleState());
    };

    addAndMakeVisible(armToggle);

    muteToggle.onClick = [this]
    {
        auto index = trackEngine.getSelectedTrackIndex();
        trackEngine.setTrackMuted(index, muteToggle.getToggleState());
    };

    addAndMakeVisible(muteToggle);

    soloToggle.onClick = [this]
    {
        auto index = trackEngine.getSelectedTrackIndex();
        trackEngine.setTrackSolo(index, soloToggle.getToggleState());
    };

    addAndMakeVisible(soloToggle);

    populateTrackSelector();
    syncFromTrackEngine();
}

void InspectorPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));

    g.setColour(juce::Colour(0xff2a2a2a));
    g.drawRect(getLocalBounds(), 1);
}

void InspectorPanel::resized()
{
    auto area = getLocalBounds().reduced(8);

    headerLabel.setBounds(area.removeFromTop(24));

    trackSelector.setBounds(area.removeFromTop(24));
    area.removeFromTop(10);

    volumeLabel.setBounds(area.removeFromTop(20));
    volumeSlider.setBounds(area.removeFromTop(24));

    panLabel.setBounds(area.removeFromTop(20));
    panSlider.setBounds(area.removeFromTop(24));

    armToggle.setBounds(area.removeFromTop(22));
    muteToggle.setBounds(area.removeFromTop(22));
    soloToggle.setBounds(area.removeFromTop(22));
}

void InspectorPanel::syncFromTrackEngine()
{
    if (trackSelector.getNumItems() != trackEngine.getNumTracks())
        populateTrackSelector();

    const int selected = trackEngine.getSelectedTrackIndex();
    trackSelector.setSelectedItemIndex(selected, juce::dontSendNotification);

    const auto& track = trackEngine.getTrack(selected);

    volumeSlider.setValue(track.volume, juce::dontSendNotification);
    panSlider.setValue(track.pan, juce::dontSendNotification);

    armToggle.setToggleState(track.armed, juce::dontSendNotification);
    muteToggle.setToggleState(track.muted, juce::dontSendNotification);
    soloToggle.setToggleState(track.solo, juce::dontSendNotification);
}

void InspectorPanel::populateTrackSelector()
{
    trackSelector.clear();

    for (int i = 0; i < trackEngine.getNumTracks(); ++i)
        trackSelector.addItem(trackEngine.getTrack(i).name, i + 1);
}
