// Mashed RE — R4 opener: track world renderer + fly-through camera.
//
// Renders a parsed Track::World (GRAPH*.BSP) through D3D9 fixed-function:
// one triangle-list batch per material, textures resolved by material name
// from the track's TXD (TEXTURES.TXD or <TRACK>.TXD; same chunk-0x23
// container as the menu TXDs), vertex diffuse = the world's baked prelight.
//
// ============================ SCAFFOLD NOTICE ===========================
// (reconciliation 2026-06-10, see re/analysis/DIVERGENCE_LEDGER_3D.md)
// The DATA parsing consumed here (TrackWorld/DffModel/MTS/gates/fog params)
// is FAITHFUL — format-cracked and validated. PORTED (no longer scaffold,
// 2026-06-10): the shared race camera + zoom-saturation elimination rule
// (Race/RaceCamera.{h,cpp}, verbatim from 0x00446520/0x00410d10). Still
// [SCAFFOLD] — invented stand-ins to be REPLACED by RE'd verbatim ports,
// NOT refined: the orbit/free dev cameras and single-car chase cam, the
// kinematic+harvested-rates handling, AI lane/braking driving, first-to-N
// scoring, spawn scorer, drive/round demos.
// ARCHITECTURE NOTE: this is the R4 *opening spike* — a minimal D3D9 path
// consuming the cracked RW data so the renderer-architecture decision
// (librw vs RW-subset port vs custom D3D9) is made against something
// concrete. It deliberately prejudges nothing: the parsed Track::World is
// renderer-agnostic.
#pragma once

#include <d3d9.h>
#include <cstdint>
#include <vector>

#include "../Race/RaceCamera.h"
#include "ParticleSystem.h"
#include "PickupField.h"

namespace mashed_re {
namespace D3d9Render {

class TrackRenderer {
public:
    // Load + parse <piz_path>'s GRAPH*.BSP and its TXD, build batches and
    // textures on `dev`. Appends a load report to `log_path` (may be null).
    bool Load(IDirect3DDevice9* dev, const char* piz_path, const char* log_path);

    // Per-frame camera input (assembled by the host from DirectInput keys +
    // Win32 mouse). Any nonzero movement switches from auto-orbit to free
    // mode; `reset_orbit` switches back.
    struct CamInput {
        float move_fwd    = 0.f;   // +1 W / -1 S
        float move_strafe = 0.f;   // +1 D / -1 A
        float move_up     = 0.f;   // +1 E / -1 Q
        float yaw_delta   = 0.f;   // radians (mouse-look / arrow keys)
        float pitch_delta = 0.f;
        bool  reset_orbit = false; // R: back to auto-orbit
        float dt          = 0.f;   // seconds since last frame
    };

    // Draw one frame. Default camera = auto-orbit fly-through around the
    // world bbox (yaw = t * 0.3 rad); `in` (optional) drives the free camera.
    // Assumes BeginScene is active.
    void Render(IDirect3DDevice9* dev, float t, const CamInput* in = nullptr);

    bool ready() const { return ready_; }
    // Vertex layout shared by world/car/prop batches (public so file-scope
    // builder helpers can use it).
    struct V { float x, y, z; D3DCOLOR c; float u, v; };
    // Current camera eye/target (for HUD/debug and the car-mode chase cam).
    void camera(float eye[3], float at[3]) const;
    float world_center(int axis) const { return center_[axis]; }
    float world_radius() const { return radius_; }

    // ---- R5: car on track -------------------------------------------------
    // Load a vehicle DFF (+ its TXD) from `piz_path` and spawn it on the
    // collision ground (the track's COLLI*/COLL*.BSP is parsed during Load()).
    bool LoadCar(IDirect3DDevice9* dev, const char* piz_path,
                 const char* dff_entry, const char* log_path);
    bool car_ready() const { return car_ready_; }

    // PORTED car selection (FUN_0040d110, 0x0040d110): one model piz is
    // shared by every player — vehicle id snaps to its 6-livery group
    // ((id/6)*6) and each player's car = base + livery (DFF "<Base><n>").
    // Loads liveries 1..3 for the AI cars (mode-0xb of the original assigns
    // livery = player index; per-player DAT_007f1a1c defaults pending RE).
    bool LoadCarLiveries(IDirect3DDevice9* dev, const char* piz_path,
                         const char* dff_base, const char* log_path);

    struct DriveInput {
        float accel = 0.f;   // +1 up-arrow / -1 down-arrow (reverse/brake)
        float steer = 0.f;   // +1 right / -1 left
        float dt    = 0.f;
    };
    // Kinematic drive step: speed/yaw integration, ground-height snap via
    // collision raycast. When a car is loaded the camera becomes a chase cam.
    void UpdateCar(const DriveInput& in);
    // Downward raycast on the collision world. Returns ground Y at (x,z); ok
    // set false when no triangle is under the point.
    float GroundHeight(float x, float z, bool* ok) const;
    void  car_pos(float out[3]) const {
        out[0] = car_pos_[0]; out[1] = car_pos_[1]; out[2] = car_pos_[2];
    }
    float car_speed() const { return car_speed_; }

    // Enable track-weather particles (0=none, 1=snow, 2=dust). Called when a
    // race begins; the field is drawn at the end of the 3D pass.
    void SetAmbientParticles(int type) {
        parts_.SetAmbient(type == 1 ? ParticleSystem::Snow
                         : type == 2 ? ParticleSystem::Dust
                                     : ParticleSystem::None,
                          track_radius_ > 1.f ? track_radius_ : radius_);
        parts_.Reset();
    }

private:
    static constexpr DWORD kFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    std::vector<std::vector<V>>      batches_;   // per material, tri lists
    std::vector<IDirect3DTexture9*>  textures_;  // per material (may be null)
    float  center_[3] = {};
    float  radius_    = 1.f;
    // Orbit-camera focus derived from the AI gate ribbon (the raceable track),
    // not the raw world bbox — the bbox is skewed by skybox/backdrop geometry,
    // so orbiting its midpoint frames mostly empty sky. Falls back to the bbox
    // center/radius when there are too few gates. Set in Load().
    float  track_center_[3] = {};
    float  track_radius_    = 1.f;
    bool   ready_     = false;
    ParticleSystem parts_;          // in-race weather/dust billboards
    PickupField    pickups_;        // in-race power-up orbs
    float  last_t_    = -1.f;        // for per-frame dt (particles)

public:
    // Enable + place power-up pickups along the gate ribbon (called when a race
    // starts). The HUD reads collected()/held() back. No-op if no gates.
    void InitPickups();
    int  pickups_collected() const { return pickups_.collected(); }
    int  pickup_held() const { return pickups_.held(); }
    const char* pickup_held_name() const {
        return PickupField::KindName(pickups_.held());
    }
private:

    // free-camera state (orbit when !free_)
    bool   free_      = false;
    float  eye_[3]    = {};
    float  yaw_       = 0.f;
    float  pitch_     = -0.4f;
    float  last_eye_[3] = {};
    float  last_at_[3]  = {};

    // collision world (flat triangle soup for the ground raycast) + the
    // render world's soup (spawn validation: ground must be VISIBLE — the
    // frozen-bay ice has collision but its SEA.DFF visual is an unrendered
    // prop, so collision-only scoring spawns the car on invisible ice)
    std::vector<float>          col_verts_;  // x,y,z per vertex
    std::vector<std::uint32_t>  col_tris_;   // v0,v1,v2 triples
    std::vector<float>          rend_verts_;
    std::vector<std::uint32_t>  rend_tris_;

    // AI path gates (AI*.BSP: 94-ish vertical quads; material RED byte = the
    // gate index LAPDATA's Lap_Line numbers refer to). gate 0 = start line.
    // c0/c3 = first/fourth vertex in stream order (the original's node
    // corners j=0/j=3, FUN_00426d00); dir = unit race direction (the
    // original's node +0x00, FUN_00426cc0) — feeds the RaceCamera port.
    struct Gate {
        float center[3];
        float c0[3], c3[3];
        float dir[3];
    };
    std::vector<Gate> gates_;

    // VERBATIM-PORTED race camera + elimination (Race/RaceCamera.{h,cpp};
    // replaces the invented centroid camera + ">7 gates" rule).
    Race::RaceCamera race_cam_;
    std::vector<Race::RaceCamNode> cam_nodes_;
    int    course_id_ = -1;       // COURSE.LUA Course_Id(N) -> LE<N>.LED
    char   gate_bsp_[64] = {};    // COURSE.LUA AI_Bsp_Filename (gate BSP, per-track)
    double cam_ticks_ = 0.0;      // DAT_007f1030 equivalent (~3.0 MHz live)
public:
    float cam_required_zoom() const { return race_cam_.required_zoom(); }
private:

    // track props: RWP_Object DFF+MTS instanced sets + Clump_Filename DFFs
    // at identity (their frames carry placement; COURSE.LUA wiring).
    struct Prop {
        std::vector<std::vector<V>>     batches;   // per material
        std::vector<IDirect3DTexture9*> textures;
        std::vector<D3DMATRIX>          instances;
    };
    std::vector<Prop> props_;

    // renderer-gap closures (reconciliation 2026-06-10): fog from COURSE.LUA
    // Setup_Fog(start_frac, end, r, g, b); sky.dff drawn first, z-write off,
    // unfogged.
    bool     fog_on_   = false;
    float    fog_start_ = 0.f, fog_end_ = 100.f;
    D3DCOLOR fog_color_ = D3DCOLOR_XRGB(24, 28, 40);
    Prop     sky_;
public:
    D3DCOLOR fog_color() const { return fog_color_; }
private:

    // car model + state
    std::vector<std::vector<V>>     car_batches_;
    std::vector<IDirect3DTexture9*> car_textures_;
    // AI livery variants (index 0 = livery 1 = AI car 0, etc.); full models
    // with wheels baked in — the spin overlay only applies to the player.
    struct CarVariant {
        std::vector<std::vector<V>>     batches;
        std::vector<IDirect3DTexture9*> textures;
    };
    std::vector<CarVariant> car_variants_;
    bool  car_ready_   = false;
    float car_pos_[3]  = {};
    float car_yaw_     = 0.f;
    float car_speed_   = 0.f;
    float car_ground_off_ = 0.f;   // model bbox min-Y -> wheels on ground

    // visual wheels: split from the body by per-atomic bbox heuristic
    // (disc-shaped, lateral-thin, at the 4 ground corners). Verts stored
    // pivot-relative; spun around the lateral axle, front pair steered.
    struct CarWheel {
        std::vector<std::pair<std::uint32_t, std::vector<V>>> parts;
        float pivot[3] = {};
        float radius   = 0.3f;
        bool  front    = false;
        bool  lateral_is_x = true;   // axle along model X (else Z)
    };
    std::vector<CarWheel> wheels_;
    float wheel_spin_ = 0.f;
    float steer_vis_  = 0.f;

    // stretch: AI cars following the gate loop at fixed speed (share the
    // player's model batches; placeholder until the real AI port)
    // AI v2: follows the gate ribbon with a per-car lateral lane offset
    // (so cars spread out / overtake), brakes for sharp upcoming corners,
    // and uses a velocity-shaped speed instead of teleport-to-gate.
    struct AiCar {
        float pos[3]; float yaw; int target; float speed;
        float cur_speed; float lane;   // lane = signed lateral offset
        float spin = 0.f;              // >0: spun out (missile/mine hit)
        float slow = 0.f;              // >0: shocked (capped speed)
    };
    std::vector<AiCar> ai_cars_;

    // ---- power-up EFFECTS (scaffold over the race state; FUN_00430670 family
    // not yet RE'd). The held power-up (PickupField) is fired with FireHeldPowerup
    // and acts on the player car / AI cars: Missile -> projectile -> spin-out;
    // Mine -> dropped hazard -> spin-out on contact (Shield blocks); Shock ->
    // slow nearby cars; Boost -> player top-speed burst; Shield -> block 1 hit.
    float boost_timer_  = 0.f;     // player +top speed while >0
    float shield_timer_ = 0.f;     // player blocks the next spin-out while >0
    struct Missile { float pos[3]; float vel[3]; int target; float life; };
    std::vector<Missile> missiles_;
    struct Mine { float pos[3]; float life; };
    std::vector<Mine> mines_;
    void UpdatePowerups(float dt);
    void SpinOut(int carSlot);     // slot 0 = player, 1..3 = ai_cars_[slot-1]
    void ApplyPowerup(int kind);   // spawn the effect for a PickupField::Kind
public:
    // Use the held power-up (from the pickup field) — called on the fire key.
    bool FireHeldPowerup();
    // Fire a specific power-up kind regardless of inventory (demo/testing).
    void FirePowerupKind(int kind) { if (car_ready_) ApplyPowerup(kind); }
    bool boost_active()  const { return boost_timer_  > 0.f; }
    bool shield_active() const { return shield_timer_ > 0.f; }

public:
    // ---- R6: handling v2 + race rules + elimination round -----------------
    // Velocity-vector handling (struct shape adopted from VehicleControlUpdate
    // 0x00470670 — see re/analysis/standalone_menu_sm/HANDLING_V2_2026-06-10.md).
    // Ground probe also returns the hit triangle's normal for slope gravity.
    float GroundProbe(float x, float z, bool* ok, float normal[3]) const;

    struct RaceCar {           // race bookkeeping per car (0 = player)
        int   gate = 1;        // next gate to cross
        int   laps = 0;
        float progress = 0.f;  // gate + fraction (ranking metric)
        bool  alive = true;
    };
    static constexpr int kRaceCars = 4;
    RaceCar race_[kRaceCars];
    bool  round_mode_  = false;   // MASHED_ROUND: 4-car exhibition round
    int   round_alive_ = kRaceCars;
    int   round_winner_ = -1;     // set when the round ends
    // PORTED points system (FUN_0040eee0 + FUN_0040b290 + Race::
    // EvaluateResult 0x00410510): per-elimination awards with 4
    // participants: 1st out -2, 2nd out -1, runner-up +1 (zeroed if its
    // score >10), winner +2; scores floor at 0; signed delta + 6000 ms
    // display timer (DAT_008a9520/DAT_008a9510); elimination order kept
    // (DAT_008a94c0). Match win at score > 11 (0x00410510, DAT_008a94d0==4).
    int   scores_[kRaceCars] = {};       // DAT_008a94e0
    int   score_prev_[kRaceCars] = {};   // DAT_008a9570
    int   score_delta_[kRaceCars] = {};  // DAT_008a9520
    float delta_timer_[kRaceCars] = {};  // DAT_008a9510 (ms)
    int   elim_order_[kRaceCars] = {-1, -1, -1, -1};   // DAT_008a94c0
    int   elim_count_ = 0;
    int   match_winner_ = -1;
    int   round_no_ = 0;
    int   race_mode_ = 0;         // 0 = elimination, 1 = laps
    int   lap_target_ = 3;        // laps mode: laps to finish
    float countdown_ = 0.f;       // >0 = pre-go freeze (seconds remaining)
    void  ScoreAward(int car, int delta);          // FUN_0040b290 mode-0 path
    void  ScoreOnElimination(int victim);          // FUN_0040eee0 4-player path
    void  StartRound();           // grid all 4 cars at the start line
    void  StartMatch(int first_to);  // reset scores, start round 1
    void  NextRoundOrEnd();          // check match win, start next round
    int   match_winner() const { return match_winner_; }
    int   round_no() const { return round_no_; }
    int   score(int car) const { return (car >= 0 && car < kRaceCars) ? scores_[car] : 0; }
    int   score_delta(int car) const { return (car >= 0 && car < kRaceCars) ? score_delta_[car] : 0; }
    float delta_timer(int car) const { return (car >= 0 && car < kRaceCars) ? delta_timer_[car] : 0.f; }
    float countdown() const { return countdown_; }
    // round step (also advances player bookkeeping outside round mode)
    void  UpdateRace(float dt);
    int   round_winner() const { return round_winner_; }
    int   round_alive() const { return round_alive_; }
    // Dev/verification: end the match now, winner = current points leader.
    void  ForceMatchEnd() {
        int best = 0;
        for (int i = 1; i < kRaceCars; ++i) if (scores_[i] > scores_[best]) best = i;
        match_winner_ = best;
    }
    // Final standings: car slots ranked (desc) by score (elimination mode) or by
    // race progress (laps mode). out[] gets car indices best-first.
    void  Standings(int out[kRaceCars], bool byProgress = false) const {
        for (int i = 0; i < kRaceCars; ++i) out[i] = i;
        for (int i = 0; i < kRaceCars; ++i)
            for (int j = i + 1; j < kRaceCars; ++j) {
                const bool swap = byProgress
                    ? (race_[out[j]].progress > race_[out[i]].progress)
                    : (scores_[out[j]] > scores_[out[i]]);
                if (swap) { int t = out[i]; out[i] = out[j]; out[j] = t; }
            }
    }

    // ---- race objective mode: 0 = Elimination (rounds + score, the default),
    // 1 = Laps (single race to lap_target_; positions by progress). The frontend
    // game-mode selection maps to this; SetRaceMode is called when a race begins.
    void  SetRaceMode(int mode, int laps) {
        race_mode_ = (mode == 1) ? 1 : 0;
        lap_target_ = laps > 0 ? laps : 3;
    }
    int   race_mode()  const { return race_mode_; }
    int   lap_target() const { return lap_target_; }
    int   car_lap(int slot) const {
        return (slot >= 0 && slot < kRaceCars) ? race_[slot].laps : 0;
    }
    int   car_position(int slot) const {       // 1-based rank by progress (desc)
        if (slot < 0 || slot >= kRaceCars) return kRaceCars;
        int rank = 1;
        for (int i = 0; i < kRaceCars; ++i)
            if (i != slot && race_[i].progress > race_[slot].progress) ++rank;
        return rank;
    }

private:
    float car_vel_[3] = {};       // +0x9b0-shape velocity vector (world)
};

}  // namespace D3d9Render
}  // namespace mashed_re
