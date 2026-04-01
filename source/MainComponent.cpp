#include <memory>
#include "MainComponent.h"

namespace
{
    juce::String formatLoopBeat(double beats, int beatsPerBar)
    {
        const int bar = (int)(beats / beatsPerBar) + 1;
        const int beatNumber = ((int)beats % beatsPerBar) + 1;
        return juce::String(bar) + ":" + juce::String(beatNumber);
    }
}

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

    transportBar.onLoopToggled = [this](bool enabled)
    {
        transportState.setLoopEnabled(enabled);
        arrangementView.repaint();
        setTransientStatus(enabled ? "Loop enabled" : "Loop disabled");
    };

    transportBar.onMetronomeToggled = [this](bool enabled)
    {
        transportState.setMetronomeEnabled(enabled);
        setTransientStatus(enabled ? "Metronome on" : "Metronome off");
    };

    arrangementView.onSaveRequested = [this]()
    {
        saveProject();
    };

    arrangementView.onLoadRequested = [this]()
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
        loadProjectFromFile(currentProjectFile, false);

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
    const bool wasRecordingSession = recordingSessionActive;

    transportState.setPlayheadBeatsDirect(audioEngine.getPlayheadBeats());
    inspectorPanel.syncFromTrackEngine();
    transportBar.syncFromTransportState();

    const bool shouldRecord = transportState.isPlaying()
                              && transportState.isRecording()
                              && trackEngine.anyTrackArmed();

    if (shouldRecord && !recordingSessionActive)
    {
        const int armedIndex = trackEngine.getFirstArmedTrackIndex();

        if (armedIndex >= 0)
        {
            recordingTrackIndex = armedIndex;
            recordingStartBeat = transportState.getPlayheadBeats();
            recordingTempoBpm = transportState.getTempo();
            const auto trackName = trackEngine.getTrack(armedIndex).name;
            const auto file = createRecordingFile(trackName);

            if (audioEngine.startRecording(file, 1))
            {
                recordingFile = file;
                recordingSessionActive = true;
                setTransientStatus("Recording: " + trackName);
            }
            else
            {
                transportState.setRecording(false);
                setTransientStatus("Recording failed");
            }
        }
        else
        {
            transportState.setRecording(false);
            setTransientStatus("No armed track to record");
        }
    }

    if (recordingSessionActive && (!shouldRecord || !transportState.isPlaying()))
    {
        const auto samples = audioEngine.stopRecording();
        const bool hadAudio = samples > 0;
        recordingSessionActive = false;

        if (hadAudio && recordingFile.existsAsFile())
        {
            const double durationSeconds = (double) samples / juce::jmax(1.0, audioEngine.getSampleRate());
            double lengthBeats = durationSeconds * ((double) recordingTempoBpm / 60.0);
            lengthBeats = juce::jmax(1.0, lengthBeats);

            const juce::String clipName = recordingFile.getFileNameWithoutExtension();

            trackEngine.addClip(recordingTrackIndex,
                                recordingStartBeat,
                                lengthBeats,
                                clipName,
                                recordingFile.getFullPathName());

            refreshProjectState();
            setTransientStatus("Recorded: " + clipName);
        }
        else
        {
            if (recordingFile.existsAsFile())
                recordingFile.deleteFile();

            setTransientStatus("Recording canceled");
        }

        transportState.setRecording(false);
    }

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

           << " | Recording: ";

    if (recordingSessionActive && recordingTrackIndex >= 0)
        status << "Yes (" << trackEngine.getTrack(recordingTrackIndex).name << ")";
    else
        status << (transportState.isRecording() ? "Armed" : "No");

    const int beatsPerBar = transportState.getBeatsPerBar();

    status << " | Loop: ";
    if (transportState.isLoopEnabled())
        status << formatLoopBeat(transportState.getLoopStartBeats(), beatsPerBar)
               << " - "
               << formatLoopBeat(transportState.getLoopEndBeats(), beatsPerBar);
    else
        status << "Off";

    status << " | Metronome: "
           << (transportState.isMetronomeEnabled() ? "On" : "Off");

    const double nowMs = juce::Time::getMillisecondCounterHiRes();

    if (nowMs >= transientStatusExpiryMs)
        transientStatus.clear();

    if (! transientStatus.isEmpty())
        status << " | " << transientStatus;

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
    auto chooser = std::make_shared<juce::FileChooser>("Save project file",
                                                        currentProjectFile.existsAsFile()
                                                            ? currentProjectFile
                                                            : juce::File::getCurrentWorkingDirectory(),
                                                        "*.ops.xml");

    chooser->launchAsync(juce::FileBrowserComponent::saveMode,
                         [this, chooser](const juce::FileChooser& dialog)
    {
        auto file = dialog.getResult();

        if (file == juce::File{})
        {
            setTransientStatus("Save canceled");
            return;
        }

        if (! file.hasFileExtension(".ops.xml"))
            file = file.withFileExtension(".ops.xml");

        if (projectFileService.saveProject(file, trackEngine, transportState))
        {
            currentProjectFile = file;
            setTransientStatus("Project saved: " + file.getFileName());
        }
        else
        {
            setTransientStatus("Save failed");
        }
    });
}

void MainComponent::loadProject()
{
    auto chooser = std::make_shared<juce::FileChooser>("Open project",
                                                        currentProjectFile.existsAsFile()
                                                            ? currentProjectFile
                                                            : juce::File::getCurrentWorkingDirectory(),
                                                        "*.ops.xml");

    chooser->launchAsync(juce::FileBrowserComponent::openMode,
                         [this, chooser](const juce::FileChooser& dialog)
    {
        const auto file = dialog.getResult();

        if (file == juce::File{})
        {
            setTransientStatus("Load canceled");
            return;
        }

        loadProjectFromFile(file);
    });
}

bool MainComponent::loadProjectFromFile(const juce::File& file, bool showStatus)
{
    if (! file.existsAsFile())
    {
        if (showStatus)
            setTransientStatus("Project file not found");

        return false;
    }

    if (! projectFileService.loadProject(file, trackEngine, transportState))
    {
        if (showStatus)
            setTransientStatus("Load failed");

        return false;
    }

    currentProjectFile = file;
    audioEngine.jumpToBeat(transportState.getPlayheadBeats());
    arrangementView.resetSelection();
    refreshProjectState();

    if (showStatus)
        setTransientStatus("Project loaded: " + file.getFileName());

    return true;
}

void MainComponent::setTransientStatus(const juce::String& message)
{
    transientStatus = message;
    transientStatusExpiryMs = juce::Time::getMillisecondCounterHiRes() + 4000.0;
}

juce::File MainComponent::createRecordingFile(const juce::String& trackName) const
{
    auto recordingsDir = juce::File::getCurrentWorkingDirectory().getChildFile("recordings");
    recordingsDir.createDirectory();

    const auto now = juce::Time::getCurrentTime();
    const auto sanitizedTrack = trackName.retainCharacters("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-");
    const auto safeTrack = sanitizedTrack.isEmpty() ? juce::String("Track") : sanitizedTrack;
    const auto filename = juce::String::formatted("%s_%04d%02d%02d_%02d%02d%02d.wav",
                                                  safeTrack.toRawUTF8(),
                                                  now.getYear(),
                                                  now.getMonth() + 1,
                                                  now.getDayOfMonth(),
                                                  now.getHours(),
                                                  now.getMinutes(),
                                                  now.getSeconds());

    return recordingsDir.getChildFile(filename);
}
