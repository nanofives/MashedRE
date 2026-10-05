// GameFlow scaffold impl (2026-06-15). See GameFlow.h.
#include "GameFlow.h"
#include "../Audio/AudioEngine.h"
#include "../Save/GameSaveFormat.h"        // [WS-G5] real gamesave.bin format
#include "../Frontend/MenuNavSM.h"          // [WS-G5] feed save image to menu state
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>                          // [D4 2026-10-05] getenv, for the
                                            // default-OFF save self-test

namespace mashed_re {
namespace Race {

namespace {
GameMode    g_mode = GameMode::Frontend;
bool        g_paused = false;          // in-race pause (mode 3 <-> 7); see GameFlow.h
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
// The display-NAME column was removed 2026-09-05. It held "Arctic", "Egypt",
// "Roundabout" and so on -- area names read out of each track's COURSE.LUA
// comments, which are internal asset names and NOT strings the game ever shows
// ("Arctic" occurs 0 times in MASHED.exe; flagged 2026-08-27). Its only
// consumer copied it into Cup::tracks[].name, which nothing read. The `piz`
// column below still carries the same identity and is a real on-disk filename,
// so nothing is lost. Row labels come from the message table (id 0x49 + row).
struct Area { int courseId; const char* piz; };
const Area kAreas[] = {
    {  0, "Arctic"   },
    {  3, "Egypt"    },
    { 26, "City"     },
    { 34, "Forest"   },
    { 38, "Highway"  },
    { 39, "Neustein" },
    { 36, "Storm"    },
    { 25, "SuperG"   },
    { 33, "Warzone"  },
    {  2, "rouabout" },   // Roundabout
    { 11, "sands"    },   // Sands
    { 37, "dump"     },   // Dump
    { 30, "training" },   // Training
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
// Challenge Cup 1 holds FOUR tracks, not 8. MEASURED behaviourally: an
// Interceptor on the string-draw entry 0x00427e00 (whose first cdecl arg is a
// message id) while screen 6 is displayed logs exactly four track msgids --
// 0x49, 0x4a, 0x4b, 0x4c -- at 72 calls each over 72 frames, i.e. one per row
// per frame. Locked rows are requested too, so 4 is the cup's TOTAL size and
// not the unlocked count. Names: Angel Peak / Kharga Temple / Neustein /
// Timgidski (ENGLISH.DAT in Font36.piz).
const int kCupTrackCount = 4;     // Challenge Cup 1: 4 tracks (msgid 0x49..0x4c)
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

// ── [D4 2026-10-05 PREREG_WIRE] the save image, and who does the file I/O ─────
//
// In the exe build these route through the PORTED pair instead of GameFlow's own
// fopen/fwrite/fread:
//   * the image lives in Save/GameSave.cpp's buffer — the ONE image, modelling the
//     original's single buffer at 0x00803358. Before this, GameFlow kept its own
//     file-static img[] and the ported writer had no caller at all.
//   * the write is Save::SaveWrite (0x00404f50) and the read is
//     Save::SaveFileExists (0x00404f80) then Save::SaveLoad (0x00404e50). That
//     ORDER is how the original's pair is meant to be used: SaveLoad discards its
//     I/O result and always returns 0, which is exactly why 0x00404f80 exists.
//   * SaveStatusClear(0) runs inside both, as in the original.
// The `n == kSaveSize` gate is PRESERVED via GameSave_LastReadBytes(); relaxing it
// to magic-only would let a truncated file with a valid magic parse a partly-zero
// span. Those accessors are exe-build-only, part of GameSave.cpp's declared
// deviation, and carry no C-level claim.
//
// The .asi arm below is the previous behaviour, unchanged.
#ifdef MASHED_STANDALONE
// Save/GameSave.cpp exports these with C linkage at global scope, not in a
// namespace (extern "C" __declspec(dllexport) ... __cdecl), so they are declared
// here exactly as defined.
extern "C" unsigned char* GameSave_BufferPtr();
extern "C" std::uint32_t  GameSave_LastReadBytes();
extern "C" int __cdecl SaveWrite();
extern "C" int __cdecl SaveLoad();
extern "C" int __cdecl SaveFileExists();

static unsigned char* SaveImageBuffer() { return GameSave_BufferPtr(); }
static void WriteSaveImage(unsigned char*) { SaveWrite(); }
static size_t ReadSaveImage(unsigned char* img) {
    std::memset(img, 0, mashed_re::Save::kSaveSize);   // a missing file must not
                                                       // read as stale bytes
    if (!SaveFileExists()) return 0;
    SaveLoad();
    return GameSave_LastReadBytes();
}
#else
static unsigned char* SaveImageBuffer() {
    static unsigned char img[mashed_re::Save::kSaveSize];
    return img;
}
static void WriteSaveImage(unsigned char* img) {
    if (std::FILE* f = std::fopen(kSavePath, "wb")) {
        std::fwrite(img, 1, mashed_re::Save::kSaveSize, f);
        std::fclose(f);
    }
}
static size_t ReadSaveImage(unsigned char* img) {
    if (std::FILE* f = std::fopen(kSavePath, "rb")) {
        const size_t n = std::fread(img, 1, mashed_re::Save::kSaveSize, f);
        std::fclose(f);
        return n;
    }
    return 0;
}
#endif

void SaveProgress() {
    using namespace mashed_re::Save;
    unsigned char span[kSpanBytes];
    BuildSpanFromProgress(span);
    unsigned char* img = SaveImageBuffer();
    BuildImage(span, ++g_saveCounter, img);            // bump the save-state counter
    WriteSaveImage(img);
}
}  // namespace

// Bind point for Save/GameSaveBuffer.cpp (standalone exe build only). Exposes the
// live save-state counter (models DAT_008A95AC) BY NAME, so the ported
// Serialize/DeserializeToBuffer neutralize their 0x008A95AC tunnel by binding to
// THIS real engine state (bumped by SaveProgress at ++g_saveCounter) rather than a
// private duplicate. Anon-namespace members are visible in the enclosing namespace,
// so this accessor (external linkage) can take the counter's address.
std::uint32_t* GameFlow_SaveCounterPtr() { return &g_saveCounter; }

GameMode GameFlow_Mode() { return g_mode; }
RaceSession& GameFlow_Session() { return g_session; }

// In-race pause toggle (mode 3 <-> 7). Only meaningful while InRace; a request in
// any other mode is ignored so the flag can never strand a non-race state frozen.
bool GameFlow_IsPaused() { return g_paused && g_mode == GameMode::InRace; }
void GameFlow_SetPaused(bool paused) {
    if (g_mode != GameMode::InRace) { g_paused = false; return; }
    g_paused = paused;
    // The original ducks nothing extra on pause here (mode 7 keeps the race
    // stream running; see re/analysis/audio_music_state_dispatch_20260711.md), so
    // audio state is left as-is.
}

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
    g_paused = false;                 // leaving the race clears the pause state
    g_session.End();
    g_mode = GameMode::Frontend;
    // 0x00466b50 mode dispatch: exit-to-menu-shaped modes (0/6) snap the
    // FUN_0045dbe0-driven envelope to 0 (dir=2) -- see
    // re/analysis/audio_music_state_dispatch_20260711.md.
    Audio::MusicSetState(Audio::MusicState::Menu);   // back to menu music
    log("RequestExit -> Frontend");
}

void GameFlow_RequestResults() {
    if (g_mode != GameMode::InRace) return;
    g_paused = false;                 // the match resolved; drop any pause
    g_mode = GameMode::Results;       // session stays active (scene + scores held)
    // 0x00466b50 mode 5 (stream-drain/post-race) snaps the FUN_0045dbe0
    // envelope to 0 once all 4 audio streams report state 3 for >=3 frames;
    // no win/lose distinction found in that branch (see U-9017).
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
                // 0x00466b50 modes 3/7 (CreateStream 0x00462dd0 + per-car
                // opponent-AI loop) are the heaviest per-frame audio body --
                // the strongest race-loop signal in the mode dispatch.
                Audio::MusicSetState(Audio::MusicState::Race);  // cdaudio race music
                log("LoadingRace -> InRace");
            }
            break;
        case GameMode::InRace:
            // Mode 7 (paused) skips the race tick in the original's FUN_00492d30
            // (0x00492d30). Freeze the session tick here too; exe_main gates its
            // own physics step on GameFlow_IsPaused().
            if (!g_paused) g_session.Tick(dt);
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
    g_cup.trackCount = kCupTrackCount;
    bool anyUnlocked = false;
    for (int i = 0; i < kCupTrackCount && i < 10; ++i) {
        g_cup.tracks[i].trackId  = i;             // index into kAreas
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
    unsigned char* img = SaveImageBuffer();
    const size_t nRead = ReadSaveImage(img);
    {
        unsigned char span[kSpanBytes];
        std::uint32_t counter = 0;
        // Magic gate (DEADBEEF) + size check; a blank/short file keeps defaults.
        if (nRead == kSaveSize && ParseImage(img, kSaveSize, span, &counter)) {
            ApplyProgressFromSpan(span);
            g_saveCounter = counter;
            // DISABLED 2026-08-28 — second of the two boot-time span restores
            // (the other was exe_main.cpp's original/gamesave.bin load). Same
            // wrong premise: the original leaves this span ZERO at the main menu
            // (measured live: DAT_007f0f2c = 0, DAT_007f0ad4 = 0), so driving
            // the grey-out state from a save image at boot makes screen 1's
            // Bonus Features render ENABLED where the original greys it.
            // Progression still loads — ApplyProgressFromSpan above populates
            // g_progUnlock/g_progTrophy, which is what Campaign_CurrentCup
            // actually consults; only the menu-gate span restore is dropped.
            // mashed_re::Frontend::Nav_GameStateLoadSave(img, kSaveSize);
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

// Dev/verification override — see the header. Same as above but clamped to the
// full kAreas[] table rather than the 8 cup tracks, so parity capture can reach
// Training/Sands/Dump/Roundabout. Not reachable from gameplay.
void Campaign_SetSelectedTrackDev(int t) {
    if (t < 0) t = 0;
    if (t >= kAreaCount) t = kAreaCount - 1;
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

#ifdef MASHED_STANDALONE
// ─────────────────────────────────────────────────────────────────────────────
// [D4 2026-10-05 PREREG_WIRE] G-BYTES / G-LIVE / G-ROUNDTRIP-LIVE.
//
// DEFAULT-OFF (MASHED_SAVE_SELFTEST). Exercises the LIVE GameFlow path, not the
// primitives — Save/GameSave.cpp's own self-test already covers those.
//
// G-LIVE is the gate that matters and it is why GameSave_LastReadBytes() is read
// here: that counter is written ONLY inside the exe-build gFileRead, so a value
// of 151456 is positive evidence the load genuinely went through SaveLoad. The
// session that produced this found a byte-faithful 0x00469df0 and a written
// BodyOrient_OmegaFromAngVel both sitting with ZERO call sites, so "linked" is
// not taken as "called".
namespace {

int GameFlowSaveSelfTest(char* out, int outSize) {
    char line[200];
    #define sayf(...) do { std::snprintf(line, sizeof line, __VA_ARGS__); \
                           if (out && outSize > 0) { \
                               const std::size_t u = std::strlen(out); \
                               if (u + 1 < static_cast<std::size_t>(outSize)) \
                                   std::strncpy(out + u, line, \
                                                static_cast<std::size_t>(outSize) - u - 1); \
                               out[outSize - 1] = '\0'; } } while (0)
    if (out && outSize > 0) out[0] = '\0';
    int pass = 0;

    // ---- G-BYTES: SaveWrite's bytes vs a direct fwrite of the same image ----
    unsigned char* img = SaveImageBuffer();
    for (std::uint32_t i = 0; i < mashed_re::Save::kSaveSize; ++i)
        img[i] = static_cast<unsigned char>((i * 17u + 3u) & 0xffu);
    WriteSaveImage(img);                       // through Save::SaveWrite
    const char* pathA = std::getenv("MASHED_SAVE_PATH");
    if (!pathA || !pathA[0]) pathA = "mashed_re_gamesave.bin";
    static unsigned char ref[mashed_re::Save::kSaveSize];
    std::memcpy(ref, img, mashed_re::Save::kSaveSize);
    char pathB[512];
    std::snprintf(pathB, sizeof pathB, "%s.fwriteref", pathA);
    if (std::FILE* f = std::fopen(pathB, "wb")) {
        std::fwrite(ref, 1, mashed_re::Save::kSaveSize, f);
        std::fclose(f);
    }
    std::uint32_t same = 0, szA = 0, szB = 0;
    {
        static unsigned char a[mashed_re::Save::kSaveSize];
        static unsigned char b[mashed_re::Save::kSaveSize];
        if (std::FILE* f = std::fopen(pathA, "rb")) {
            szA = static_cast<std::uint32_t>(std::fread(a, 1, sizeof a, f)); std::fclose(f); }
        if (std::FILE* f = std::fopen(pathB, "rb")) {
            szB = static_cast<std::uint32_t>(std::fread(b, 1, sizeof b, f)); std::fclose(f); }
        for (std::uint32_t i = 0; i < mashed_re::Save::kSaveSize; ++i)
            if (a[i] == b[i]) ++same;
    }
    const bool bytesOk = (same == mashed_re::Save::kSaveSize &&
                          szA == mashed_re::Save::kSaveSize &&
                          szB == mashed_re::Save::kSaveSize);
    if (bytesOk) ++pass;
    sayf("G-BYTES  SaveWrite vs fwrite: %u of %u matching, sizes %u / %u (want %u)\n",
         same, mashed_re::Save::kSaveSize, szA, szB, mashed_re::Save::kSaveSize);
    std::remove(pathB);

    // ---- G-ROUNDTRIP-LIVE: through Campaign_SaveNow / Campaign_ReloadFromSave --
    int wantUnlock[kAreaCount], wantTrophy[kAreaCount];
    for (int i = 0; i < kAreaCount; ++i) {
        wantUnlock[i] = (i % 3 == 0) ? 1 : 0;
        wantTrophy[i] = (i % 4);
        g_progUnlock[i] = wantUnlock[i];
        g_progTrophy[i] = wantTrophy[i];
    }
    g_progLoaded = true;                       // the state above IS the loaded state
    Campaign_SaveNow();

    for (int i = 0; i < kAreaCount; ++i) {     // clobber with a recognisable pattern
        g_progUnlock[i] = 1 - wantUnlock[i];
        g_progTrophy[i] = 3 - wantTrophy[i];
    }
    Campaign_ReloadFromSave();

    // ---- G-LIVE: only the exe-build gFileRead writes this counter ----
    const std::uint32_t lastRead = GameSave_LastReadBytes();
    const bool liveOk = (lastRead == mashed_re::Save::kSaveSize);
    if (liveOk) ++pass;
    sayf("G-LIVE   GameSave_LastReadBytes=%u (want %u) -> load %s through SaveLoad\n",
         lastRead, mashed_re::Save::kSaveSize, liveOk ? "WENT" : "did NOT go");

    int match = 0;
    for (int i = 0; i < kAreaCount; ++i)
        if (g_progUnlock[i] == wantUnlock[i] && g_progTrophy[i] == wantTrophy[i]) ++match;
    if (match == kAreaCount) ++pass;
    sayf("G-ROUNDTRIP-LIVE  %d of %d areas restored (unlock+trophy)\n", match, kAreaCount);

    sayf("RESULT %d of 3 gates passed\n", pass);
    #undef sayf
    return pass;
}

struct GameFlowSaveSelfTestRunner {
    GameFlowSaveSelfTestRunner() {
        const char* on = std::getenv("MASHED_SAVE_SELFTEST");
        if (!on || !on[0] || on[0] == '0') return;
        char buf[1024] = {};
        GameFlowSaveSelfTest(buf, static_cast<int>(sizeof buf));
        const char* p = std::getenv("MASHED_SAVE_SELFTEST_OUT2");
        if (!p || !p[0]) p = "gameflow_selftest.txt";
        if (std::FILE* f = std::fopen(p, "w")) { std::fputs(buf, f); std::fclose(f); }
    }
};
GameFlowSaveSelfTestRunner s_gameFlowSaveSelfTestRunner;

}  // namespace
#endif  // MASHED_STANDALONE

}  // namespace Race
}  // namespace mashed_re
