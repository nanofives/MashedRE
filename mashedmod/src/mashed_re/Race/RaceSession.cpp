// RaceSession scaffold impl (2026-06-15). Stubs log once via the standalone log
// so a race "runs" (advances state, calls every subsystem in order) without a
// real simulation yet. Replace each stub body with the ported subsystem.
#include "RaceSession.h"
#include "../D3d9Render/TrackRenderer.h"
#include "../Audio/AudioEngine.h"
#include <cstdio>
#include <cstdarg>

namespace mashed_re {
namespace Race {

namespace {
const char* kLog = "log/mashed_re.log";
void once(const char* msg) {
    static const char* seen[64]; static int n = 0;
    for (int i = 0; i < n; ++i) if (seen[i] == msg) return;
    if (n < 64) seen[n++] = msg;
    if (std::FILE* f = std::fopen(kLog, "a")) {
        std::fprintf(f, "[race-scaffold] %s\n", msg); std::fclose(f);
    }
}
void logf(const char* fmt, ...) {
    if (std::FILE* f = std::fopen(kLog, "a")) {
        std::va_list ap; va_start(ap, fmt);
        std::fprintf(f, "[race] "); std::vfprintf(f, fmt, ap); std::fprintf(f, "\n");
        va_end(ap); std::fclose(f);
    }
}

// Vehicle catalogue: maps a car selection index -> {model piz, DFF base}. One
// model piz holds a 6-livery group; LoadCarLiveries spawns the AI cars from the
// same base (FUN_0040d110 selection shape). Index 0 = Advantage (the MASHED_CAR
// default). TODO-RE: the real frontend car-select index -> vehicle table
// (Player Colour/Car Select writes DAT_007f1a1c..); for now Begin uses the
// config's car-0 carIndex into this list, defaulting to Advantage.
struct VehicleEntry { const char* piz; const char* dffBase; };
const VehicleEntry kVehicles[] = {
    { "original/TOASTART/VEHICLES/Advantag.piz", "ADVANTAGE" },
    { "original/TOASTART/VEHICLES/Atmos.piz",    "ATMOS"     },
    { "original/TOASTART/VEHICLES/BamBam.piz",   "BAMBAM"    },
    { "original/TOASTART/VEHICLES/Bullet.piz",   "BULLET"    },
    { "original/TOASTART/VEHICLES/Cooler.piz",   "COOLER"    },
    { "original/TOASTART/VEHICLES/Creeper.piz",  "CREEPER"   },
    { "original/TOASTART/VEHICLES/Crusader.piz", "CRUSADER"  },
    { "original/TOASTART/VEHICLES/Formula.piz",  "FORMULA"   },
    { "original/TOASTART/VEHICLES/Kustom.piz",   "KUSTOM"    },
    { "original/TOASTART/VEHICLES/Shorty.piz",   "SHORTY"    },
};
const int kVehicleCount = static_cast<int>(sizeof(kVehicles) / sizeof(kVehicles[0]));

// trackId -> per-track ambience bank (toastaudio/pc/audio/pcdics/<area>.rws).
// Mirrors exe_main's kRaceTrackPiz order; the .rws bank names are the lowercase
// area name. (A track without a bank just gets no ambience — graceful.)
const char* kTrackBank[] = {
    "arctic", "egypt", "city", "forest", "highway", "neustein", "storm", "superg",
};
const int kTrackBankCount = static_cast<int>(sizeof(kTrackBank) / sizeof(kTrackBank[0]));

// Ambient bed wave names to try, in priority order — different tracks name
// their environmental loops differently; play the first that resolves.
const char* kAmbientCandidates[] = { "windy", "wind", "sea", "heavyrainloop",
                                     "ambient", "rain", "loop" };
}  // namespace

// --- subsystem stubs ----------------------------------------------------------
// Track/Vehicle/AI/Collision/Camera/Rules/HUD are now REAL — provided by the
// integrated TrackRenderer engine (spawned in Begin, driven per-frame by the
// host's in-race render block: UpdateCar->UpdateRace + the verbatim race camera
// + the HUD overlay). These Update bodies are therefore no-ops (the engine owns
// the per-frame work); the once() line records that the subsystem is engine-
// backed. Audio + Particles are real HERE (see RaceAudio/ParticleSystem). Only
// Powerups remains a genuine TODO stub.
void ITrackRuntime::Load(int trackId)        { (void)trackId; once("TrackRuntime: REAL via TrackRenderer (GRAPH*.BSP world + collision + gates)"); ready = true; }
void ITrackRuntime::Spawns(RaceConfig& c)    { (void)c; once("TrackRuntime::Spawns: REAL via StartRound grid (gate 0 start line)"); }
void ITrackRuntime::Render()                 { once("TrackRuntime::Render: REAL via D3d9Render/TrackRenderer"); }

void IVehicleSim::Update(RaceConfig& c, float dt)   { (void)c; (void)dt; once("VehicleSim: REAL via TrackRenderer::UpdateCar (handling integrator)"); ready = true; }
void IAiController::Update(RaceConfig& c, float dt)  { (void)c; (void)dt; once("AiController: REAL via TrackRenderer::UpdateRace (gate-ribbon AI)"); ready = true; }
void ICollisionWorld::Resolve(RaceConfig& c, float dt){ (void)c; (void)dt; once("CollisionWorld: REAL via TrackRenderer::GroundHeight (ground snap)"); ready = true; }
void IPowerupSystem::Update(RaceConfig& c, float dt) { (void)c; (void)dt; once("PowerupSystem: REAL via TrackRenderer::pickups (orb collect/respawn; effects TODO)"); ready = true; }
void IPowerupSystem::Render()                        { once("PowerupSystem::Render: REAL via PickupField billboards"); }
void IRaceCameraDrv::Update(RaceConfig& c, float dt) { (void)c; (void)dt; once("RaceCamera: REAL via Race/RaceCamera verbatim port (0x00446520)"); ready = true; }
void IParticleSystem::Update(float dt)               { (void)dt; once("ParticleSystem: REAL via D3d9Render/ParticleSystem (snow/dust billboards)"); ready = true; }
void IParticleSystem::Render()                       { once("ParticleSystem::Render: REAL via TrackRenderer 3D pass"); }
void IRaceAudio::Update(RaceConfig& c, float dt)     { (void)c; (void)dt; once("RaceAudio: REAL via Audio/AudioEngine DirectSound (RWS ambience; engine-RPM TODO)"); ready = true; }
void IRaceHud::Render(RaceConfig& c)                 { (void)c; once("RaceHud: REAL via in-race HUD overlay (scoreboard/countdown/standings)"); }

// --- session ------------------------------------------------------------------
void RaceSession::Begin(const RaceConfig& cfg, D3d9Render::TrackRenderer* track,
                        IDirect3DDevice9* dev) {
    m_cfg = cfg;
    m_active = true;
    m_engine = track;
    m_dev    = dev;
    once("RaceSession::Begin");
    // The track WORLD is already loaded into `track` by the frontend launch
    // (GRAPH*.BSP + collision + gates). Here we spawn the entrants and start the
    // match, which flips the integrated engine into round_mode + car_ready — that
    // activates vehicle physics, ground collision, the chase/race camera, AI
    // opponents, race rules and the HUD in the in-race render path.
    if (track && dev && track->ready()) {
        int vi = m_cfg.cars[0].carIndex;
        if (vi < 0 || vi >= kVehicleCount) vi = 0;
        const VehicleEntry& ve = kVehicles[vi];
        char dff0[64];
        std::snprintf(dff0, sizeof(dff0), "%s0.DFF", ve.dffBase);
        const bool car = track->LoadCar(dev, ve.piz, dff0, kLog);
        const bool liv = track->LoadCarLiveries(dev, ve.piz, ve.dffBase, kLog);
        // First-to-3-rounds match (the ported elimination/scoring rules). This
        // grids all 4 cars at the start line and runs the countdown.
        track->StartMatch(3);
        // Race objective from the chosen game mode (0 = elimination, 1 = laps).
        track->SetRaceMode(m_cfg.raceMode, m_cfg.laps);
        // Track-weather particles: snow on the frozen tracks, dust elsewhere
        // (1=snow, 2=dust). Arctic=0, Storm=6 are the snowy/stormy areas.
        const int tid = m_cfg.trackId;
        const int ptype = (tid == 0 || tid == 6) ? 1 : 2;
        track->SetAmbientParticles(ptype);
        track->InitPickups();        // place power-up orbs along the gate ribbon
        m_powerups.ready = true;
        logf("Begin: track ready, vehicle=%s player_car=%d liveries=%d "
             "-> StartMatch(3) particles=%d pickups=on mode=%s laps=%d [real sim active]",
             ve.dffBase, (int)car, (int)liv, ptype,
             m_cfg.raceMode == 1 ? "laps" : "elim", m_cfg.laps);
    } else {
        logf("Begin: no track/device (logs-only race) ready=%d",
             track ? (int)track->ready() : -1);
    }
    m_track.ready = true;   // ITrackRuntime now backed by the engine

    // Real in-race audio: play the track's environmental ambience (the per-track
    // <area>.rws bank decoded by Audio/RwsBank) as a looping DirectSound voice.
    if (Audio::Available()) {
        const int t = (m_cfg.trackId >= 0 && m_cfg.trackId < kTrackBankCount)
                          ? m_cfg.trackId : 0;
        char bank[200];
        std::snprintf(bank, sizeof(bank),
                      "original/toastaudio/pc/audio/pcdics/%s.rws", kTrackBank[t]);
        int voice = -1;
        for (const char* cand : kAmbientCandidates) {
            voice = Audio::PlayLoop(bank, cand, 0.75f);
            if (voice >= 0) { logf("ambience '%s' from %s (voice %d)", cand, bank, voice); break; }
        }
        if (voice < 0) logf("ambience: no loopable wave found in %s", bank);
        m_ambient = voice;
        m_ticks = 0;
        m_audio.ready = (voice >= 0);
        // Real engine: the permdict `eng1` loop, pitched by RPM (the engN engine
        // classes live in permdict, NOT the english/*.rws banks — see
        // re/analysis/audio_character_banks_REmap_20260616.md §4).
        Audio::EngineStart(0.5f);
        // Character voice bank: follows the racer's Player Colour index 0..5
        // (0=RED 1=BLUEJAY 2=MELON 3=GOLD 4=PINK 5=SHADOW), NOT the vehicle
        // (REmap note §1-§3; cited from DAT_006041f0 / FUN_004625b0 / FUN_00462dd0).
        // Resolve + log the player's bank (cars[0].colour); per-clip taunt
        // playback awaits the 0x80E clip-directory parse (deferred).
        {
            const int colour = m_cfg.cars[0].colour;
            char vbank[200];
            if (Audio::CharacterBankPath(colour, 0, vbank, sizeof(vbank)))
                logf("player voice bank: colour=%d -> %s", colour, vbank);
            else
                logf("player voice bank: colour=%d out of range (0..5)", colour);
        }
        // Music (cdaudio race track) is owned by GameFlow's music-state
        // controller, switched on the Frontend/Race/Results state edges.
    } else {
        logf("ambience: audio engine unavailable");
    }
}

void RaceSession::Tick(float dt) {
    if (!m_active) return;
    // Dependency-ordered subsystem update (see RaceSession.h header note).
    m_ai.Update(m_cfg, dt);
    m_vehicles.Update(m_cfg, dt);
    m_collision.Resolve(m_cfg, dt);
    m_camera.Update(m_cfg, dt);
    m_powerups.Update(m_cfg, dt);
    m_particles.Update(dt);
    m_audio.Update(m_cfg, dt);
    // Cursor-advance proof (headless runs can't capture sound): log the ambient
    // voice's play cursor at two well-separated ticks — a moved cursor proves
    // DirectSound is actually rendering the decoded RWS PCM, not just holding a
    // created-but-silent buffer.
    if (m_ambient >= 0) {
        ++m_ticks;
        if (m_ticks == 12 || m_ticks == 180)
            logf("ambience cursor @tick%d: %u (playing=%d)", m_ticks,
                 Audio::PlayCursor(m_ambient), (int)Audio::IsPlaying(m_ambient));
    }
}

void RaceSession::Render() {
    if (!m_active) return;
    m_track.Render();
    // (vehicle meshes render here once the renderer/vehicle scaffolds land)
    m_particles.Render();
    m_powerups.Render();
    m_hud.Render(m_cfg);
}

void RaceSession::End() {
    if (!m_active) return;
    once("RaceSession::End");
    Audio::EngineStop();
    Audio::MusicStop();
    Audio::StopAll();          // silence the ambience when the race ends
    m_active = false;
}

}  // namespace Race
}  // namespace mashed_re
