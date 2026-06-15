// GameFlow scaffold impl (2026-06-15). See GameFlow.h.
#include "GameFlow.h"
#include <cstdio>

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

// Challenge Cup 1 — the first cup's tracks (names OBSERVED from the original
// Challenge Select capture verify/parity/orig_s6.bmp). Fresh-save default: only
// the first challenge unlocked, no trophies. TODO-RE: real unlock/trophy state
// from the gamesave + the cup table.
const Cup kCup0 = {
    L"Challenge Cup 1", 8,
    {
        { 0, L"Angel Peak",     true,  0 },
        { 1, L"Mirage Tunnels", false, 0 },
        { 2, L"Roosters",       false, 0 },
        { 3, L"Fairgrounds",    false, 0 },
        { 4, L"Ventura Blvd",   false, 0 },
        { 5, L"Rooster Bay",    false, 0 },
        { 6, L"Polar Wharf",    false, 0 },
        { 7, L"Angel Peak",     false, 0 },
    }
};

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

const Cup& Campaign_CurrentCup() { return kCup0; }
int  Campaign_SelectedTrack() { return g_selTrack; }
void Campaign_SetSelectedTrack(int t) {
    if (t < 0) t = 0;
    if (t >= kCup0.trackCount) t = kCup0.trackCount - 1;
    g_selTrack = t;
}

}  // namespace Race
}  // namespace mashed_re
