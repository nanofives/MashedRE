// RaceSession — standalone in-race subsystem scaffold (2026-06-15).
//
// This is the SCAFFOLD for the race layer: a RaceSession that owns one stub per
// remaining game subsystem and drives them in dependency order each frame. Every
// subsystem here is an interface + a no-op/log stub today; each is filled in
// demand-driven by porting its owning original functions (RVAs cited per stub
// from re/analysis/subsystem_map.md). The point is that the standalone's race
// architecture compiles and runs end-to-end now, so individual subsystems can be
// dropped in without re-plumbing.
//
// Execution order (RaceSession::Tick): Input -> Ai -> Vehicle -> Collision ->
// Camera -> Powerups -> Particles -> Audio -> (Render: Track + Vehicles + HUD).
//
// Existing real pieces this ties together: Track/TrackWorld, Track/DffModel,
// D3d9Render/TrackRenderer, Race/RaceCamera (already in the exe build).
#pragma once
#include <cstdint>

struct IDirect3DDevice9;   // COM fwd-decl (pointer only; no d3d9.h here)

namespace mashed_re {
namespace D3d9Render { class TrackRenderer; }   // the integrated race engine
namespace Race {

// One race entrant (player or AI). Mirrors the per-car slot state the original
// keeps in the DAT_007f1a14.. / entity-pool arrays.
struct RaceCar {
    int   carIndex   = 0;        // livery/vehicle id (FUN_0042fab0 index)
    int   colour     = 0;        // colour selection (Player Colour Select)
    bool  isAi       = true;     // FUN_0040e480(slot,2) marks AI
    bool  isPlayer   = false;
    int   device     = 0;        // 1=joypad 2=keyboard (DAT_007e96fc)
    float pos[3]     = {0, 0, 0};
    float vel[3]     = {0, 0, 0};
    float yaw        = 0.f;
    int   lap        = 0;
    int   racePos    = 0;
    bool  eliminated = false;
};

// Parameters chosen in the frontend setup flow (game-mode / colour / ability /
// challenge screens) and consumed when a race begins.
struct RaceConfig {
    int  gameMode    = 0;        // DAT_0067e9fc (2/3/4/5/6/10..)
    int  trackId     = 0;        // DAT_0067f17c
    int  numCars     = 1;
    int  difficulty  = 0;        // DAT_0067ea7c
    int  powerUps    = 0;        // DAT_0067ea80
    int  laps        = 3;
    int  raceMode    = 0;        // race objective: 0 = elimination, 1 = laps
    RaceCar cars[8];
};

// ---- Subsystem scaffold interfaces (stubs today) ------------------------------
// Each has the lifecycle the game loop needs. Methods are no-ops/logs until the
// owning original code is ported.

struct ITrackRuntime {            // wraps Track/TrackWorld + collision + spawns
    void Load(int trackId);       // TODO-RE: FUN_004b6940 track load chain
    void Spawns(RaceConfig&);     // place cars at start grid (LED/track data)
    void Render();                // -> D3d9Render/TrackRenderer (real)
    bool ready = false;
};

struct IVehicleSim {              // per-car physics/handling integrator
    void Update(RaceConfig&, float dt);  // TODO-RE: vehicle update cluster
    bool ready = false;
};

struct IAiController {            // opponent driver controllers
    void Update(RaceConfig&, float dt);  // TODO-RE: AI/ dir (FUN_0040e480 family)
    bool ready = false;
};

struct ICollisionWorld {         // car-track + car-car
    void Resolve(RaceConfig&, float dt); // TODO-RE: RW-Physics / collision cluster
    bool ready = false;
};

struct IPowerupSystem {          // in-race power-ups (icons RE'd for game-mode)
    void Update(RaceConfig&, float dt);  // TODO-RE: FUN_00430670 + pickup logic
    void Render();
    bool ready = false;
};

struct IRaceCameraDrv {          // race camera (Race/RaceCamera real port exists)
    void Update(RaceConfig&, float dt);  // LED.piz angle format cracked
    bool ready = false;
};

struct IParticleSystem {         // smoke/sparks/explosions
    void Update(float dt);
    void Render();
    bool ready = false;
};

struct IRaceAudio {              // RWS engine/FX/music playback
    void Update(RaceConfig&, float dt);  // TODO-RE: Rws/ + audio pool
    bool ready = false;
};

struct IRaceHud {                // in-race HUD (position/lap/timer/powerup)
    void Render(RaceConfig&);    // -> HUD/ dir
    bool ready = false;
};

// ---- The session --------------------------------------------------------------
class RaceSession {
public:
    // Enter a race (called by GameFlow). `track` is the already-world-loaded
    // TrackRenderer (the integrated R6 sim/render engine) and `dev` its device;
    // Begin uses them to spawn the player car + AI liveries and start the match,
    // which activates vehicle physics / collision / chase camera / AI / race
    // rules / HUD in the in-race render path. Either may be null (logs-only).
    void Begin(const RaceConfig& cfg, D3d9Render::TrackRenderer* track,
               IDirect3DDevice9* dev);
    void Tick(float dt);                 // one simulation step
    void Render();                       // draw the race frame
    void End();                          // leave the race
    bool active() const { return m_active; }
    RaceConfig& config() { return m_cfg; }

private:
    bool          m_active = false;
    RaceConfig    m_cfg;
    D3d9Render::TrackRenderer* m_engine = nullptr;  // integrated sim/render engine
    IDirect3DDevice9*         m_dev    = nullptr;
    int                       m_ambient = -1;       // ambience audio voice id
    int                       m_ticks   = 0;        // for the cursor-advance proof
    ITrackRuntime   m_track;
    IVehicleSim     m_vehicles;
    IAiController    m_ai;
    ICollisionWorld m_collision;
    IPowerupSystem  m_powerups;
    IRaceCameraDrv  m_camera;
    IParticleSystem m_particles;
    IRaceAudio      m_audio;
    IRaceHud        m_hud;
};

}  // namespace Race
}  // namespace mashed_re
