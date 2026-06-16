// GameFlow scaffold impl (2026-06-15). See GameFlow.h.
#include "GameFlow.h"
#include <cstdio>
#include <cstdint>

namespace mashed_re {
namespace Race {

namespace {
GameMode    g_mode = GameMode::Frontend;
RaceSession g_session;
RaceConfig  g_pending;
int         g_loadFrames = 0;
int         g_selTrack = 0;
D3d9Render::TrackRenderer* g_pendingTrack = nullptr;   // engine for the pending race
IDirect3DDevice9*          g_pendingDev   = nullptr;

// REAL track areas — Course_Id + area .piz + area name, cracked from each
// track's COURSE.LUA (Course_Id(N) + the area comment; see re/tools/
// piz_extract.py + the COURSE.LUA dumps). The id is the engine's Course_Id
// (also indexes Common/LED.piz LE<id>.LED for the race camera). Loading a
// track by its area .piz makes TrackRenderer derive the matching course_id_
// from the area's own COURSE.LUA, so the camera/LED data lines up.
struct Area { int courseId; const wchar_t* name; const char* piz; };
const Area kAreas[] = {
    {  0, L"Arctic",     "Arctic"   },
    {  3, L"Egypt",      "Egypt"    },
    { 26, L"City",       "City"     },
    { 34, L"Forest",     "Forest"   },
    { 38, L"Highway",    "Highway"  },
    { 39, L"Neustein",   "Neustein" },
    { 36, L"Storm",      "Storm"    },
    { 25, L"SuperG",     "SuperG"   },
    { 33, L"Warzone",    "Warzone"  },
    {  2, L"Roundabout", "rouabout" },
    { 11, L"Sands",      "sands"    },
    { 37, L"Dump",       "dump"     },
    { 30, L"Training",   "training" },
};
const int kAreaCount = static_cast<int>(sizeof(kAreas) / sizeof(kAreas[0]));

// The Challenge Select cup is a VIEW over the real areas (the 8 main race
// areas, in Course-table order). Display names are the REAL area names so the
// label matches the track that loads. Fresh-save default: only the first
// track unlocked, no trophies. [SCAFFOLD] The original groups areas into named
// cups (Challenge Cup 1 = "Angel Peak"/"Mirage Tunnels"/...) and the
// place-name<->area pairing + per-cup membership live in the binary cup table
// (FUN_0040b6c0 track-name table + DAT_007f0a40 13x12 cup/unlock table). Pairing
// those place-names to areas needs that table RE'd — not guessed here.
const int kCupTrackCount = 8;     // the 8 main race areas
Cup       g_cup;                  // rebuilt by Campaign_CurrentCup()

void log(const char* m) {
    if (std::FILE* f = std::fopen("log/mashed_re.log", "a")) {
        std::fprintf(f, "[gameflow] %s\n", m); std::fclose(f);
    }
}
}  // namespace

GameMode GameFlow_Mode() { return g_mode; }
RaceSession& GameFlow_Session() { return g_session; }

void GameFlow_RequestRace(const RaceConfig& cfg,
                          D3d9Render::TrackRenderer* track,
                          IDirect3DDevice9* dev) {
    g_pending = cfg;
    g_pendingTrack = track;
    g_pendingDev   = dev;
    g_mode = GameMode::LoadingRace;
    g_loadFrames = 0;
    log("RequestRace -> LoadingRace");
}

void GameFlow_RequestExit() {
    if (g_mode == GameMode::Frontend) return;
    g_session.End();
    g_mode = GameMode::Frontend;
    log("RequestExit -> Frontend");
}

void GameFlow_RequestResults() {
    if (g_mode != GameMode::InRace) return;
    g_mode = GameMode::Results;       // session stays active (scene + scores held)
    log("RequestResults -> Results");
}

void GameFlow_Update(float dt) {
    switch (g_mode) {
        case GameMode::Frontend:
            break;                               // the menu drives itself
        case GameMode::LoadingRace:
            // Brief load gate (the real loader streams the track here).
            if (++g_loadFrames > 30) {
                g_session.Begin(g_pending, g_pendingTrack, g_pendingDev);
                g_mode = GameMode::InRace;
                log("LoadingRace -> InRace");
            }
            break;
        case GameMode::InRace:
            g_session.Tick(dt);
            break;
        case GameMode::Results:
            break;
    }
}

void GameFlow_Render() {
    if (g_mode == GameMode::InRace) g_session.Render();
}

const Cup& Campaign_CurrentCup() {
    // Build the cup view over the real areas, reading unlock flags from the
    // live cup/unlock table the gamesave loader populated at 0x007f0a40 (13
    // rows x 12 int32; the track-unlock column is 0x007f0a50 = row*12 + 4).
    // A blank/absent save leaves the table zero -> we fall back to "first
    // track unlocked" (the correct fresh-game state).
    const std::int32_t* unlockTbl = reinterpret_cast<const std::int32_t*>(0x007f0a40);
    g_cup.name = L"Challenge Cup 1";
    g_cup.trackCount = kCupTrackCount;
    bool anyUnlocked = false;
    for (int i = 0; i < kCupTrackCount && i < 10; ++i) {
        g_cup.tracks[i].trackId  = i;             // index into kAreas
        g_cup.tracks[i].name     = kAreas[i].name;
        bool unlocked = false;
        // guard the read (image-padded span); col 4 = track-unlock flag.
        unlocked = (unlockTbl[i * 12 + 4] != 0);
        g_cup.tracks[i].unlocked = unlocked;
        g_cup.tracks[i].trophy   = 0;             // TODO-RE trophy column
        if (unlocked) anyUnlocked = true;
    }
    if (!anyUnlocked) g_cup.tracks[0].unlocked = true;   // fresh-save default
    return g_cup;
}
int  Campaign_SelectedTrack() { return g_selTrack; }
void Campaign_SetSelectedTrack(int t) {
    if (t < 0) t = 0;
    if (t >= kCupTrackCount) t = kCupTrackCount - 1;
    g_selTrack = t;
}

// Track index (into the cup / kAreas) -> area .piz path + engine Course_Id.
void Campaign_TrackPizPath(int trackIdx, char* buf, int cap) {
    if (trackIdx < 0 || trackIdx >= kAreaCount) trackIdx = 0;
    std::snprintf(buf, cap, "original/TOASTART/TRACKS/%s.piz", kAreas[trackIdx].piz);
}
int Campaign_TrackCourseId(int trackIdx) {
    if (trackIdx < 0 || trackIdx >= kAreaCount) trackIdx = 0;
    return kAreas[trackIdx].courseId;
}

}  // namespace Race
}  // namespace mashed_re
