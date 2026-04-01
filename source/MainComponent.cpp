#include "MainComponent.h"

MainComponent::MainComponent()
    : transportBar(transportState),
      inspectorPanel(trackEngine),
      audioEngine(transportState, trackEngine),
      arrangementView(trackEngine, transportState),
      currentProjectFile(juce::File::getCurrentWorkingDirectory().getChildFile("projects").getChildFile("default.ops.xml"))
{
    setWantsKeyboardFocus(true);

    arrangementView.onClipDataChanged = [this]
    {
        refreshProjectState();
    };

    arrangementView.onPlayheadMoved = [this]
    {
        audioEngine.jumpToBeat(transportState.getPlayheadBeats());
    };

    arrangementView.onAudioFileDropped = [this](const juce::File& file, int trackIndex, double startBeat)
    {
        addAudioClipAtPosition(file, trackIndex, startBeat);
    };

    browserPanel.onAudioFileChosen = [this](const juce::File& file)
    {
        addAudioClipToSelectedTrack(file);
    };

    transportBar.onSaveClicked = [this]
    {
        saveProject();
    };

    transportBar.onLoadClicked = [this]
    {
        loadProject();
    };

    addAndMakeVisible(transportBar);
    addAndMakeVisible(browserPanel);
    addAndMakeVisible(inspectorPanel);
    addAndMakeVisible(arrangementView);
    addAndMakeVisible(statusBar);

    startTimerHz(30);
    setSize(1400, 900);

    if (currentProjectFile.existsAsFile())
        loadProject();

    refreshProjectState();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff111111));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();

    constexpr int transportHeight = 60;
    constexpr int statusHeight = 22;
    constexpr int browserWidth = 220;
    constexpr int inspectorWidth = 300;

    transportBar.setBounds(area.removeFromTop(transportHeight));
    statusBar.setBounds(area.removeFromBottom(statusHeight));

    browserPanel.setBounds(area.removeFromLeft(browserWidth));
    inspectorPanel.setBounds(area.removeFromRight(inspectorWidth));

    arrangementView.setBounds(area);
}

bool MainComponent::keyPressed(const juce::KeyPress& key)
{
    if (key.getModifiers().isCommandDown() && key.getTextCharacter() == 's')
    {
        saveProject();
        return true;
    }

    if (key.getModifiers().isCommandDown() && key.getTextCharacter() == 'o')
    {
        loadProject();
        return true;
    }

    return arrangementView.keyPressed(key);
}

void MainComponent::timerCallback()
{
    transportState.setPlayheadBeatsDirect(audioEngine.getPlayheadBeats());
    inspectorPanel.syncFromTrackEngine();
    transportBar.syncFromTransportState();

    arrangementView.repaint();
    inspectorPanel.repaint();

    const int selectedIndex = trackEngine.getSelectedTrackIndex();
    const auto& track = trackEngine.getTrack(selectedIndex);

    juce::String status;

    status << "Position: "
           << audioEngine.getBarBeatString()

           << " | Track: "
           << track.name

           << " | Vol: "
           << juce::String(track.volume, 2)

           << " | Pan: "
           << juce::String(track.pan, 2)

           << " | Tempo: "
           << juce::String(transportState.getTempo(), 1)

           << " | Playing: "
           << (transportState.isPlaying() ? "Yes" : "No")

           << " | Recording: "
           << (transportState.isRecording() ? "Yes" : "No");

    statusBar.setStatusText(status);
}

void MainComponent::addAudioClipToSelectedTrack(const juce::File& file)
{
    addAudioClipAtPosition(file,
                           trackEngine.getSelectedTrackIndex(),
                           transportState.getPlayheadBeats());
}

void MainComponent::addAudioClipAtPosition(const juce::File& file, int trackIndex, double startBeat)
{
    if (! file.existsAsFile() || ! juce::isPositiveAndBelow(trackIndex, trackEngine.getNumTracks()))
        return;

    const double lengthBeats = getAudioLengthBeats(file);
    trackEngine.addClip(trackIndex,
                        startBeat,
                        lengthBeats,
                        file.getFileNameWithoutExtension(),
                        file.getFullPathName());
    trackEngine.setSelectedTrackIndex(trackIndex);
    refreshProjectState();
}

double MainComponent::getAudioLengthBeats(const juce::File& file) const
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader == nullptr || reader->sampleRate <= 0.0)
        return 1.0;

    const double durationSeconds = (double) reader->lengthInSamples / reader->sampleRate;
    const double beats = durationSeconds * ((double) transportState.getTempo() / 60.0);
    return juce::jmax(1.0, beats);
}

void MainComponent::refreshProjectState()
{
    audioEngine.refreshClipSources();
    arrangementView.refreshFromModel();
    inspectorPanel.syncFromTrackEngine();
    repaint();
}

void MainComponent::saveProject()
{
    projectFileService.saveProject(currentProjectFile, trackEngine, transportState);
}

void MainComponent::loadProject()
{
    if (! projectFileService.loadProject(currentProjectFile, trackEngine, transportState))
        return;

    audioEngine.jumpToBeat(transportState.getPlayheadBeats());
    refreshProjectState();
}
