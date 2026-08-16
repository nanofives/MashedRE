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
#include <cstdio>
#include <vector>

#include "../Race/RaceCamera.h"
#include "../Race/RaceSceneState.h"
#include "../Race/RuleEngine.h"
#include "../Track/TrackData.h"
#include "ParticleSystem.h"
#include "PickupField.h"
#include "../Powerup/PowerupSystem.h"

namespace mashed_re { namespace Piz { class Archive; } }

namespace mashed_re { namespace D3d9Render { class PowerupBackendImpl; } }

namespace mashed_re {
namespace D3d9Render {

// Derives from the renderer-neutral simulation state (M3-E2'c step 1). Every
// gameplay member lives in the base now; this class is the D3D9 submitter that
// draws it. See Race/RaceSceneState.h for what moved and what deliberately did
// not. Pure move: no behaviour change.
class TrackRenderer : public mashed_re::Race::RaceSceneState {
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
    // WS-E vehicle lighting (RpLight subset): per-vertex relight source,
    // parallel to a batch's V array. Lit vertices (rpGEOMETRYLIGHT + normals)
    // get their diffuse recomputed per frame in world space:
    //   c = clamp(base + sunmul * max(0, N_model . L_model)),
    // where base = prelight + ambient (float, pre-clamp) and sunmul = sun
    // colour, both already multiplied by materialRGB/255 where the geometry
    // carries MODULATEMATERIALCOLOR. L_model = R * L_world per drawn instance
    // (exact: instance rotations are orthonormal). relit==0 vertices keep the
    // baked V colour. Public so file-scope builder helpers can fill it.
    struct RelitSrc { float n[3]; float base[3]; float sunmul[3];
                      std::uint32_t relit; };
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
    // Off-mesh recovery: scan a ring of `radius` around (px,pz) for an on-mesh
    // escape heading, preferring the one nearest curYaw. Returns false if the whole
    // ring is off-mesh. Used to steer a wedged car back onto the drivable surface
    // without depending on the gate ribbon. (TrackRenderer.cpp)
    bool FindOnMeshHeading(float px, float pz, float curYaw, float radius,
                           float& outYaw) const;
    // Gate-independent off-mesh recovery: car_pos_ is still the last on-mesh point
    // (the edge); steer toward an on-mesh heading and nudge the position inward so
    // the car never permanently freezes against an edge. Mutates car_pos_/yaw/vel.
    void  RecoverOffMesh();
    void  car_pos(float out[3]) const {
        out[0] = car_pos_[0]; out[1] = car_pos_[1]; out[2] = car_pos_[2];
    }
    float car_speed() const { return car_speed_; }
    float car_yaw()   const { return car_yaw_; }
    int   course_id() const { return course_id_; }   // COURSE.LUA Course_Id(N)

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
    bool   ready_     = false;
    ParticleSystem parts_;          // in-race weather/dust billboards
    // Collision/slide FX state, per car slot (0=player, 1..3 = ai_cars_):
    // skid-smoke rate accumulator, prev-frame speed (sudden drop = impact),
    // prev-frame yaw (yaw-rate drives the cornering skid trigger).
    float  fx_skid_accum_[4] = {};
    float  fx_prev_speed_[4] = {};
    float  fx_prev_yaw_[4]   = {};
    // [diag] PER-SLOT cumulative counts (MASHED_FX_DEBUG). Per-slot on purpose:
    // a single shared counter cannot be attributed to a car, which made the
    // 2026-08-14 "2164 skids/race" figure uninterpretable (the debug line logged
    // slot 1's inputs while the counter summed all four).
    int    fx_skids_[4] = {}, fx_sparks_[4] = {};
    // Emit skid smoke (cornering/lateral slip) + impact sparks (sudden decel) for
    // one car into parts_. Call once per car per frame BEFORE parts_.Update() --
    // emit is per-frame, render is per-view (the split-screen constraint in D-11063).
    void EmitCarFx(int slot, const float pos[3], const float vel[3],
                   float speed, float yaw, float dt);
    PickupField    pickups_;        // in-race power-up orbs
    std::vector<PickupField::Spawn> powerup_spawns_;  // POWERUPS_GOLD.LUA placement

public:
    // Enable + place power-up pickups along the gate ribbon (called when a race
    // starts). The HUD reads collected()/held() back. No-op if no gates.
    void InitPickups();
    int  pickups_collected() const { return pickups_.collected(); }
    int  pickup_held() const { return pickups_.held(); }
    const char* pickup_held_name() const {
        return PickupField::KindName(pickups_.held());
    }
    // Faithful MASHED name of the held power-up (real type code from the spawn data).
    const char* pickup_held_real_name() const {
        return PickupField::RealTypeName(pickups_.held_type());
    }
private:

public:
    float cam_required_zoom() const { return race_cam_.required_zoom(); }
private:

    // track props: RWP_Object DFF+MTS instanced sets + Clump_Filename DFFs
    // at identity (their frames carry placement; COURSE.LUA wiring).
    struct Prop {
        std::vector<std::vector<V>>     batches;   // per material
        std::vector<IDirect3DTexture9*> textures;
        std::vector<D3DMATRIX>          instances;
        // E2'b step 3: librw clump handle for this model, or -1 = draw via D3D9.
        int                             rw_model = -1;
        // F3: per-material UV-scroll rate (units/sec) from the DFF material's
        // RW UVAnim extension -> the track .UVA dict (sea/sky props scroll).
        std::vector<MatScroll>          mat_scroll;
    };
    std::vector<Prop> props_;

    // ---- F2 .ANM animated copters (COURSE.LUA General_Anim_Filename + SetCopter;
    // KTCSCRIPT.LUA KTC_NewCopter). The flight paths are the track's .ANM
    // (Track::HAnim, rwID_HANIMANIMATION); the models are COPTER/JETRANGER/
    // KTC_APACHE.DFF (+ COPTERS.TXD) from Common/Perm.piz COPTERS/. Each copter
    // flies its bound path, looped on the race clock — drawn under the per-frame
    // HAnim transform (props otherwise draw under a static matrix). See
    // re/analysis/formats/track_anim_data.md F2.
    struct CopterModel {
        std::vector<std::vector<V>>     batches;
        std::vector<IDirect3DTexture9*> textures;
    };
    std::vector<CopterModel> copter_models_;
    // gameplay=true: bound via KTCSCRIPT.LUA KTC_NewCopter (rule-5 collectible
    // feed, D-11056/U-8997); done=true once it has completed one traversal of
    // its flight path this round (UpdateRace -> OnCollect()).
    struct AnimCopter { int model = -1; Track::HAnim anim; bool gameplay = false; bool done = false; };
    std::vector<AnimCopter>  copters_;
    // Parse the copter wiring from the track piz's COURSE.LUA + KTCSCRIPT.LUA,
    // load the bound .ANM paths (track piz) and model DFFs (Common/Perm.piz).
    void LoadCopters(IDirect3DDevice9* dev, Piz::Archive& piz,
                     const char* piz_path, std::FILE* log);

    // renderer-gap closures (reconciliation 2026-06-10): sky.dff drawn first,
    // z-write off, unfogged. The fog COLOUR itself, plus amb_world_/sun_color_,
    // moved to Race::RaceSceneState in E2'b step 3 (2026-08-01) and retyped to
    // uint32_t so the librw submitter can read them; they are inherited, so every
    // use site in this file is unchanged.
    Prop     sky_;
    // WS-E vehicle lighting (RpLight subset, env MASHED_RPLIGHT, default ON;
    // =0 reverts to the legacy load-time model-space bake). Faithful
    // FUN_00479330 (0x00479330) light acquisition: float-precision colours
    // (unquantized), default lights when COURSE.LUA has no Lights_Filename
    // (DAT_006132dc ambient 0.25/0.25/0.25, DAT_006132ec directional
    // 0.75/0.75/0.75 rotated 60 deg about +X => at=(0,-sin60,cos60)), DFF
    // lights keyed on the subtype byte alone (no stream-flag filter,
    // last-wins), Ambient_RGB override (DFF branch only, any component >
    // DAT_005d757c = 0.0f). has_sun_dir_ gates the per-frame relight pass.

    // car model + state
    std::vector<std::vector<V>>     car_batches_;
    std::vector<IDirect3DTexture9*> car_textures_;
    // WS-E vehicle lighting: relight sources parallel to car_batches_
    // (empty inner vector = batch is not runtime-lit, draw static).
    std::vector<std::vector<RelitSrc>> car_relit_;
    // AI livery variants (index 0 = livery 1 = AI car 0, etc.); full models
    // with wheels baked in — the spin overlay only applies to the player.
    // E2'b step 3: librw clump handle for the player body, -1 = D3D9.
    int rw_car_model_ = -1;
    struct CarVariant {
        std::vector<std::vector<V>>     batches;
        std::vector<IDirect3DTexture9*> textures;
        std::vector<std::vector<RelitSrc>> relit;   // parallel to batches
    };
    std::vector<CarVariant> car_variants_;

    // visual wheels: split from the body by per-atomic bbox heuristic
    // (disc-shaped, lateral-thin, at the 4 ground corners). Verts stored
    // pivot-relative; spun around the lateral axle, front pair steered.
    struct CarWheel {
        std::vector<std::pair<std::uint32_t, std::vector<V>>> parts;
        // WS-E vehicle lighting: relight sources parallel to parts (empty
        // inner vector = part not runtime-lit).
        std::vector<std::vector<RelitSrc>> parts_relit;
        float pivot[3] = {};
        float radius   = 0.3f;
        bool  front    = false;
        bool  lateral_is_x = true;   // axle along model X (else Z)
    };
    std::vector<CarWheel> wheels_;

    // WS-E vehicle lighting: relit car/wheel/AI draw pass (replaces the
    // static car section of Render when MASHED_RPLIGHT is on and the track
    // has a directional light). Packs every relit batch of every instance
    // into one dynamic-VB upload, then draws in the exact legacy order.
    void RenderCarsRelit(IDirect3DDevice9* dev, const D3DMATRIX& worldm);

    // stretch: AI cars following the gate loop at fixed speed (share the
    // player's model batches; placeholder until the real AI port)
    // AI v2: follows the gate ribbon with a per-car lateral lane offset
    // (so cars spread out / overtake), brakes for sharp upcoming corners,
    // and uses a velocity-shaped speed instead of teleport-to-gate.

    // ---- power-up EFFECTS — D3 (2026-06-16): the ported dispatch replaces the
    // invented boost/shield/missile/mine/shock switch. PickupField::held_type()
    // (the real POWERUPS_GOLD.LUA code) now drives Powerup::PowerupSystem (the
    // verbatim FUN_0045bba0 dispatcher + 9-entry type table + slot lifecycle —
    // Powerup/PowerupSystem.{h,cpp}). The per-type effect LEAVES (projectile
    // spawn, hazard drop, hitscan, oil, flash, flame) are realised on the
    // visuals below via PowerupBackendImpl (IPowerupBackend) — the WS-B/WS-E
    // subsystem stand-in. boost_/shield_timer_ remain only as general handling
    // hooks; the real 9 codes do not set them (no boost/shield power-up exists).
    void UpdatePowerups(float dt);
    void SpinOut(int carSlot);     // slot 0 = player, 1..3 = ai_cars_[slot-1]

    // ported power-up dispatch + its host-visuals backend (WS-D2/D3).
    friend class PowerupBackendImpl;
    void  EnsurePowerupBackend();                     // lazy Init(this)
    void  SyncHostCar();                              // fill pu_player_/pu_ai_
    void  PowerupFireOnce(int realCode);              // drive the dispatch one-shot
    int   MissileTargetAhead() const;                 // nearest AI ahead, or -1
public:
    // Use the held power-up (from the pickup field) — called on the fire key.
    // Reads PickupField::held_type() (the real MASHED code) and runs the ported
    // per-type effect; falls back to MISSILE for the index-only (-1) orb.
    bool FireHeldPowerup();
    // Fire a specific power-up TYPE CODE regardless of inventory (demo/testing).
    void FirePowerupKind(int code);
    bool boost_active()  const { return boost_timer_  > 0.f; }
    bool shield_active() const { return shield_timer_ > 0.f; }

public:
    // ---- R6: handling v2 + race rules + elimination round -----------------
    // Velocity-vector handling (struct shape adopted from VehicleControlUpdate
    // 0x00470670 — see re/analysis/standalone_menu_sm/HANDLING_V2_2026-06-10.md).
    // Ground probe also returns the hit triangle's normal for slope gravity.
    float GroundProbe(float x, float z, bool* ok, float normal[3]) const;

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
    // F4 split timing (player car): race clock + the current lap's split times.
    float race_time() const { return race_time_; }
    float split_time(int id) const {
        return (id >= 0 && id < kMaxSplits) ? split_time_[id] : 0.f;
    }
    bool  split_done(int id) const {
        return (id >= 0 && id < kMaxSplits) && split_done_[id];
    }
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
    // ---- [WS-G rules debt, D-11052] full per-rule win-condition engine
    // (Race/RuleEngine — FUN_00410d10 + FUN_00410510 + the FUN_004177b0
    // metric/finish-order updater). SetRaceRule arms it with the real
    // DAT_007f0fd0 rule when a race begins; MASHED_RULE_ENGINE=0 reverts to
    // the two-objective collapse above. Rule 10 seeds its countdown from the
    // course id (FUN_004046a0).
    void  SetRaceRule(int rule);
    int   race_rule() const { return rule_; }
    float rule_timer() const { return rulep_.timer; }   // rule-10 HUD countdown
    bool  match_draw() const { return match_draw_; }    // EvaluateResult == -1
    // Rule-5 collectible feed (DAT_0063a5d0/DAT_0063a5d4 equivalents). U-8997
    // RESOLVED 2026-07-04 (Ghidra Mashed_pool14): the sole feeder is the
    // KTC_NewCopter track-script command (F2 gameplay copters in copters_,
    // AnimCopter::gameplay==true) — confirmed disjoint from KTC_AddPickUp,
    // which calls a different registrar (FUN_00405730) touching unrelated
    // globals. total = count of loaded KTC_NewCopter copters (SetRaceRule);
    // each reports done via OnCollect() on completing one flight-path
    // traversal (UpdateRace), mirroring FUN_004064c0's per-object completion
    // tick. See re/analysis/d11056_fable_ghidra_findings_2026-07-03.md.
    void  SetCollectibles(int total) { rulep_.collectTotal = total; rulep_.collectDone = 0; }
    void  OnCollect() { ++rulep_.collectDone; }
    // true: the player car is driven by input (a human races); false: the
    // exhibition auto-follow drives it. Set when a race begins.
    void  SetHumanDrive(bool h) { human_drive_ = h; }
    bool  human_drive() const { return human_drive_; }
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
};

}  // namespace D3d9Render
}  // namespace mashed_re
