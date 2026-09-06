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
// In-race PAUSE (faithful port of the original's mode 3 <-> mode 7 toggle). The
// pause key freezes the sim while the frozen 3D scene keeps rendering; it does
// NOT exit to the frontend (exit is an OPTION inside the pause menu). RVA map:
//   - master mode machine FUN_004929d0 (0x004929d0): case 3 (driving) sets mode 7
//     when FUN_0042c1c0 (0x0042c1c0) != 0; case 7 (paused) sets mode 3 on resume.
//   - pause-arm: FUN_0042c220 (0x0042c220) samples the pause button and pushes the
//     pause-menu event via FUN_0042c280 (0x0042c280) -> FUN_0042bf30 (0x0042bf30,
//     event 0xff210000), which raises DAT_0067eab0 (read by FUN_0042c1c0).
//   - sim freeze: game-logic tick FUN_00492d30 (0x00492d30) runs the race tick
//     FUN_004111c0 (0x004111c0) only in case 3; mode 7 skips it (sim frozen) while
//     the render tick FUN_00492e90 (0x00492e90) draws case 7 identically to case 3.
//   - pause-menu quit-to-frontend: action -0xce0000 in the overlay FUN_0043d7c0
//     (0x0043d7c0) -> FUN_0043d2a0(1,0). Restart: -0xe00000 -> FUN_0040de10.
// The standalone steps the sim in exe_main's RenderFrame, so the caller consults
// GameFlow_IsPaused() to gate the physics step and to draw a minimal overlay; this
// state object is the single source of truth. Paused only ever holds in InRace.
bool     GameFlow_IsPaused();
void     GameFlow_SetPaused(bool paused);
// Driven once per frame from exe_main's main loop (after the frontend update).
void     GameFlow_Update(float dt);
// Draw the race frame when in race; no-op in Frontend (the menu draws itself).
void     GameFlow_Render();
RaceSession& GameFlow_Session();

// ---- Campaign scaffold (Challenge Cup data the Challenge Select reads) --------
// trophy: 0=none/locked-incomplete, 1=bronze, 2=silver, 3=gold.
//
// No display NAME here, by design (2026-09-05). Both this struct and Cup used
// to carry a `const wchar_t* name`, filled with strings that exist nowhere in
// the game: the cup's "Challenge Cup 1", and per track the kAreas[] area names
// ("Arctic", "Egypt", ... — cracked from COURSE.LUA, and flagged 2026-08-27 as
// a defect because "Arctic" occurs 0 times in MASHED.exe). Neither field was
// ever read: the Challenge Select list draws its row labels from the real
// message table by id 0x49 + row (exe_main.cpp, measured behaviourally), and
// reads only trackCount / unlocked / trophy off this struct. They were invented
// strings one consumer away from being drawn, so they are gone rather than
// re-sourced. Pairing the original's cup place-names to areas needs the binary
// cup table (FUN_0040b6c0 + DAT_007f0a40) reversed; until then a row's label
// and the track it launches are still not proven to agree.
struct CupTrack {
    int            trackId;
    bool           unlocked;
    int            trophy;
};
struct Cup {
    int            trackCount;
    CupTrack       tracks[10];
};

// Returns the currently-selected challenge cup (default: cup 0, only the first
// challenge unlocked = the fresh-save state). TODO-RE: load from gamesave +
// FUN_0040b6c0 track-name table + the cup/unlock arrays (DAT_007f0a50..).
const Cup& Campaign_CurrentCup();
int        Campaign_SelectedTrack();      // cursor within the cup
void       Campaign_SetSelectedTrack(int);
// DEV/VERIFICATION ONLY. Campaign_SetSelectedTrack clamps to the 8 cup tracks,
// which is correct for gameplay but makes the other 5 areas in kAreas[]
// (Roundabout, Sands, Dump, Training, ...) unreachable. Same-track parity capture
// needs them: the original's Quick Battle always races TRAINING (kAreas[12]),
// measured 2026-08-16 via race_draw_burst.py's track detector, so with the cup
// clamp the standalone can never be put on the same track as that capture.
// Clamps to kAreaCount instead. Reached only from the MASHED_TRACK_SEL override.
void       Campaign_SetSelectedTrackDev(int);
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
// Autosave option (Options screen 32): when off, race results update progression
// in memory but are not auto-written; manual Save Game still persists. (WS-G4)
void       Campaign_SetAutosave(bool on);
// Track index (into the cup / real area table) -> area .piz path + engine
// Course_Id (cracked from each track's COURSE.LUA).
void       Campaign_TrackPizPath(int trackIdx, char* buf, int cap);
int        Campaign_TrackCourseId(int trackIdx);
// [D-11054] cup-tier launch gate: column `col` of the live cup/unlock table
// row for `trackIdx` ((&DAT_007f0a40)[col + track*0xc] — the check the
// original race-launch runs at FUN_0043dfd0 with col = FUN_004309b0()'s
// mode->column map: mode 3->1, 4->2, 5->5, 6..9->3, 10->0xb, 2->0).
bool       Campaign_TierUnlocked(int trackIdx, int col);

}  // namespace Race
}  // namespace mashed_re
