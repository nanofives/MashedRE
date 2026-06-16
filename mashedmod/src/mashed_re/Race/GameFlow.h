// GameFlow — standalone top-level game state machine + campaign scaffold
// (2026-06-15). The frontend already runs in exe_main; this adds the Frontend ->
// LoadingRace -> InRace -> Results states and a RaceSession the frontend hands a
// RaceConfig to. Today nothing in the frontend requests a race yet, so the
// default state is Frontend and GameFlow_Update is a no-op there — wiring is in
// place without disturbing the menu. It also holds the CAMPAIGN scaffold (cups /
// tracks / unlock+trophy state) that the Challenge Select screen reads.
#pragma once
#include "RaceSession.h"

namespace mashed_re {
namespace Race {

enum class GameMode { Frontend, LoadingRace, InRace, Results };

GameMode GameFlow_Mode();
// Frontend calls this when the user launches a race (select on the colour/
// challenge/game-mode screen). Transitions Frontend -> LoadingRace. `track` is
// the TrackRenderer the frontend already loaded the world into and `dev` its
// device; they are handed to RaceSession::Begin after the load gate so the race
// spawns cars + starts the match (activating the integrated sim).
void     GameFlow_RequestRace(const RaceConfig& cfg,
                              D3d9Render::TrackRenderer* track,
                              IDirect3DDevice9* dev);
// Esc/back during a race: InRace/LoadingRace/Results -> Frontend (ends the
// session). The frontend menu resumes where it left off.
void     GameFlow_RequestExit();
// Match over: InRace -> Results (keeps the session/scene for the results
// overlay; the sim is frozen). Esc/timeout from Results -> Frontend.
void     GameFlow_RequestResults();
// Driven once per frame from exe_main's main loop (after the frontend update).
void     GameFlow_Update(float dt);
// Draw the race frame when in race; no-op in Frontend (the menu draws itself).
void     GameFlow_Render();
RaceSession& GameFlow_Session();

// ---- Campaign scaffold (Challenge Cup data the Challenge Select reads) --------
// trophy: 0=none/locked-incomplete, 1=bronze, 2=silver, 3=gold.
struct CupTrack {
    int            trackId;
    const wchar_t* name;
    bool           unlocked;
    int            trophy;
};
struct Cup {
    const wchar_t* name;
    int            trackCount;
    CupTrack       tracks[10];
};

// Returns the currently-selected challenge cup (default: cup 0, only the first
// challenge unlocked = the fresh-save state). TODO-RE: load from gamesave +
// FUN_0040b6c0 track-name table + the cup/unlock arrays (DAT_007f0a50..).
const Cup& Campaign_CurrentCup();
int        Campaign_SelectedTrack();      // cursor within the cup
void       Campaign_SetSelectedTrack(int);
// Progression persistence (sidecar mashed_re_progress.bin; NEVER original/).
// Load at boot; record a result (winnerSlot 0 = player won -> unlock next track
// + trophy, then save). Campaign_CurrentCup merges this with the save-table.
void       Campaign_LoadProgress();
void       Campaign_OnRaceResult(int trackIdx, int winnerSlot, int position);
// Manual Save Game / Load Game (Options screen 8 rows 2/3). SaveNow writes the
// current progression to the real-format standalone gamesave (mashed_re_gamesave
// .bin); ReloadFromSave re-reads it and re-applies (WS-G5/G4).
void       Campaign_SaveNow();
void       Campaign_ReloadFromSave();
// Track index (into the cup / real area table) -> area .piz path + engine
// Course_Id (cracked from each track's COURSE.LUA).
void       Campaign_TrackPizPath(int trackIdx, char* buf, int cap);
int        Campaign_TrackCourseId(int trackIdx);

}  // namespace Race
}  // namespace mashed_re
