// GameFlow scaffold impl (2026-06-15). See GameFlow.h.
#include "GameFlow.h"
#include "../Audio/AudioEngine.h"
#include "../Save/GameSaveFormat.h"        // [WS-G5] real gamesave.bin format
#include "../Frontend/MenuNavSM.h"          // [WS-G5] feed save image to menu state
#include <cstdio>
#include <cstdint>
#include <cstring>

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

// ---- progression persistence (REAL gamesave.bin format; WS-G5) ---------------
// The standalone persists campaign progression (per-area unlock + trophy) in the
// REAL 0x24FA0-byte gamesave.bin format (Save/GameSaveFormat.h = buffer port of
// Save::SerializeToBuffer 0x00404ee0 / DeserializeFromBuffer 0x00404e80), written
// to a STANDALONE-COPY file — NEVER original/gamesave.bin. Replaces the old MRP1
// sidecar. Progression maps onto the championship span (row = area index): the
// verified track-unlock column (col 4) + a standalone-private trophy column.
const char* kSavePath = "mashed_re_gamesave.bin";
int g_progUnlock[kAreaCount] = {0};
int g_progTrophy[kAreaCount] = {0};
std::uint32_t g_saveCounter = 0;
bool g_progLoaded = false;
bool g_autosave   = true;     // [WS-G4] Autosave option gate (Sound/Options screen 32)

// progression -> championship span (the SerializeToBuffer source layout, 0x7f0a40).
void BuildSpanFromProgress(unsigned char span[mashed_re::Save::kSpanBytes]) {
    using namespace mashed_re::Save;
    std::memset(span, 0, kSpanBytes);
    bool any = false;
    for (int r = 0; r < kAreaCount && r < kSpanRows; ++r) {
        std::uint32_t u = g_progUnlock[r] ? 2u : 0u;   // real "track available" = 2
        std::uint32_t t = static_cast<std::uint32_t>(g_progTrophy[r]);
        std::memcpy(span + r * kRowStride + kColTrackUnlock, &u, 4);
        std::memcpy(span + r * kRowStride + kColTrophyPriv,  &t, 4);
        if (g_progUnlock[r] || g_progTrophy[r]) any = true;
    }
    std::uint32_t gate = any ? 1u : 0u;                // DAT_007f0f2c savedata gate
    std::memcpy(span + kSpanSavedataGate, &gate, 4);
}

// championship span -> progression (the Deserialize consumer side).
void ApplyProgressFromSpan(const unsigned char span[mashed_re::Save::kSpanBytes]) {
    using namespace mashed_re::Save;
    for (int r = 0; r < kAreaCount && r < kSpanRows; ++r) {
        std::uint32_t u, t;
        std::memcpy(&u, span + r * kRowStride + kColTrackUnlock, 4);
        std::memcpy(&t, span + r * kRowStride + kColTrophyPriv,  4);
        g_progUnlock[r] = (u != 0) ? 1 : 0;
        g_progTrophy[r] = static_cast<int>(t);
    }
}

void SaveProgress() {
    using namespace mashed_re::Save;
    unsigned char span[kSpanBytes];
    BuildSpanFromProgress(span);
    static unsigned char img[kSaveSize];
    BuildImage(span, ++g_saveCounter, img);            // bump the save-state counter
    if (std::FILE* f = std::fopen(kSavePath, "wb")) {
        std::fwrite(img, 1, kSaveSize, f);
        std::fclose(f);
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
    Audio::MusicSetState(Audio::MusicState::Menu);   // back to menu music
    log("RequestExit -> Frontend");
}

void GameFlow_RequestResults() {
    if (g_mode != GameMode::InRace) return;
    g_mode = GameMode::Results;       // session stays active (scene + scores held)
    Audio::MusicSetState(Audio::MusicState::Results);  // duck the race music
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
                Audio::MusicSetState(Audio::MusicState::Race);  // cdaudio race music
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
        // unlocked if the save table says so OR our own progress store does.
        bool unlocked = (unlockTbl[i * 12 + 4] != 0) ||
                        (i < kAreaCount && g_progUnlock[i] != 0);
        g_cup.tracks[i].unlocked = unlocked;
        g_cup.tracks[i].trophy   = (i < kAreaCount) ? g_progTrophy[i] : 0;
        if (unlocked) anyUnlocked = true;
    }
    if (!anyUnlocked) g_cup.tracks[0].unlocked = true;   // fresh-save default
    return g_cup;
}

// [D-11054] cup-tier launch gate — see GameFlow.h. Reads the same live table
// the gamesave loader populates (fresh/blank save: all zero -> tiers locked,
// which is the correct fresh-game state; mode 3 launches are gated by the
// existing Campaign_CurrentCup unlock logic, not this).
bool Campaign_TierUnlocked(int trackIdx, int col) {
    if (trackIdx < 0 || trackIdx >= kCupTrackCount) return false;
    if (col < 0 || col >= 12) return false;
    const std::int32_t* unlockTbl = reinterpret_cast<const std::int32_t*>(0x007f0a40);
    return unlockTbl[trackIdx * 12 + col] != 0;
}

void Campaign_LoadProgress() {
    using namespace mashed_re::Save;
    if (g_progLoaded) return;
    g_progLoaded = true;
    g_progUnlock[0] = 1;                          // track 0 always available
    static unsigned char img[kSaveSize];
    if (std::FILE* f = std::fopen(kSavePath, "rb")) {
        size_t n = std::fread(img, 1, kSaveSize, f);
        std::fclose(f);
        unsigned char span[kSpanBytes];
        std::uint32_t counter = 0;
        // Magic gate (DEADBEEF) + size check; a blank/short file keeps defaults.
        if (n == kSaveSize && ParseImage(img, kSaveSize, span, &counter)) {
            ApplyProgressFromSpan(span);
            g_saveCounter = counter;
            // Drive the menu grey-out / unlock state from the SAME real image (the
            // real reader Nav_GameStateLoadSave consumes the full gamesave.bin).
            mashed_re::Frontend::Nav_GameStateLoadSave(img, kSaveSize);
        }
    }
    char m[112];
    int n = 0; for (int i = 0; i < kAreaCount; ++i) n += (g_progUnlock[i] != 0);
    std::snprintf(m, sizeof(m), "progress loaded: %d/%d tracks unlocked (save counter %u)",
                  n, kAreaCount, g_saveCounter);
    log(m);
}

// Manual Save Game (Options screen 8 row 3): write the current progression to the
// real-format standalone gamesave. SaveProgress is the file-static writer above.
void Campaign_SaveNow() {
    SaveProgress();
    log("manual save: gamesave written");
}

// Manual Load Game (Options screen 8 row 2): re-read + re-apply the gamesave.
void Campaign_ReloadFromSave() {
    g_progLoaded = false;          // drop the load-once guard
    Campaign_LoadProgress();       // re-reads mashed_re_gamesave.bin and applies
}

void Campaign_OnRaceResult(int trackIdx, int winnerSlot, int position) {
    Campaign_LoadProgress();
    char m[128];
    std::snprintf(m, sizeof(m), "race result: track=%d winner=%d pos=%d",
                  trackIdx, winnerSlot, position);
    log(m);
    if (winnerSlot != 0) return;                  // only the player's win unlocks
    if (trackIdx >= 0 && trackIdx < kAreaCount) {
        // trophy by finishing position (1st=gold..); win => at least bronze.
        int tr = (position <= 1) ? 3 : (position == 2) ? 2 : (position == 3) ? 1 : 1;
        if (tr > g_progTrophy[trackIdx]) g_progTrophy[trackIdx] = tr;
    }
    const int next = trackIdx + 1;                // unlock the next track
    if (next >= 0 && next < kAreaCount) g_progUnlock[next] = 1;
    // [WS-G4] persist only when Autosave is on; the in-memory unlock/trophy still
    // updated either way (manual Save Game always writes).
    if (g_autosave) {
        SaveProgress();
        log("progress autosaved (next track unlocked)");
    } else {
        log("autosave off: progression updated in memory, not written");
    }
}

// [WS-G4] Autosave option (screen 32). When off, race results update progression
// in memory but are not auto-written (manual Save Game still persists).
void Campaign_SetAutosave(bool on) { g_autosave = on; }
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
