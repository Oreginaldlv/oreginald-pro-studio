#include <cassert>
#include "../source/TrackEngine.h"

int main()
{
    TrackEngine engine;

    // Startup state
    assert(engine.getNumTracks() == 5);
    assert(engine.getSelectedTrackIndex() == 0);

    // Names
    assert(engine.getTrack(0).name == "Drums");
    assert(engine.getTrack(1).name == "Bass");
    assert(engine.getTrack(2).name == "Keys");
    assert(engine.getTrack(3).name == "Lead");
    assert(engine.getTrack(4).name == "Vox");

    // Selection
    engine.setSelectedTrackIndex(3);
    assert(engine.getSelectedTrackIndex() == 3);

    // Arm
    engine.setTrackArmed(3, true);
    assert(engine.getTrack(3).armed == true);
    engine.toggleTrackArmed(3);
    assert(engine.getTrack(3).armed == false);

    // Mute
    engine.setTrackMuted(1, true);
    assert(engine.getTrack(1).muted == true);
    engine.toggleTrackMuted(1);
    assert(engine.getTrack(1).muted == false);

    // Solo
    engine.setTrackSolo(2, true);
    assert(engine.getTrack(2).solo == true);
    assert(engine.anyTrackSoloed() == true);
    engine.toggleTrackSolo(2);
    assert(engine.getTrack(2).solo == false);

    // Volume clamp
    engine.setTrackVolume(0, 2.0f);
    assert(engine.getTrack(0).volume == 1.5f);

    engine.setTrackVolume(0, -1.0f);
    assert(engine.getTrack(0).volume == 0.0f);

    // Pan clamp
    engine.setTrackPan(0, 2.0f);
    assert(engine.getTrack(0).pan == 1.0f);

    engine.setTrackPan(0, -2.0f);
    assert(engine.getTrack(0).pan == -1.0f);

    // Audibility logic
    engine.setTrackMuted(0, true);
    assert(engine.isTrackAudible(0) == false);
    engine.setTrackMuted(0, false);
    assert(engine.isTrackAudible(0) == true);

    engine.setTrackSolo(4, true);
    assert(engine.isTrackAudible(4) == true);
    assert(engine.isTrackAudible(0) == false);

    return 0;
}