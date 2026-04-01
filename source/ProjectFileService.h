#pragma once
#include <JuceHeader.h>
#include "TrackEngine.h"
#include "TransportState.h"

class ProjectFileService
{
public:
    bool saveProject(const juce::File& projectFile,
                     const TrackEngine& trackEngine,
                     const TransportState& transportState) const;

    bool loadProject(const juce::File& projectFile,
                     TrackEngine& trackEngine,
                     TransportState& transportState) const;

private:
    juce::ValueTree createProjectTree(const TrackEngine& trackEngine,
                                      const TransportState& transportState) const;
};
