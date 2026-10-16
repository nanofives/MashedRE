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
    // Vertex layout of the D3D9-drawn batches: the sky dome (copters: librw since librwrest) (public
    // so file-scope builder helpers can use it).
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

    // AI-car liveries are NOT loaded: the default build draws every AI car with the
    // player body (the per-livery D3D9 variants, LoadCarLiveries, were deleted in
    // D1-residue R3, 2026-10-16). The original's selection is FUN_0040d110
    // (0x0040d110): the vehicle id snaps to its 6-livery group ((id/6)*6) and each
    // player's car = base + livery (DFF "<Base><n>"); mode 0xb assigns livery =
    // player index.

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

    bool   ready_     = false;
    ParticleSystem parts_;          // in-race weather/dust billboards
    // Collision/slide FX state, per car slot (0=player, 1..3 = ai_cars_):
    // skid-smoke rate accumulator, prev-frame speed (sudden drop = impact),
    // prev-frame yaw (yaw-rate drives the cornering skid trigger).
    float  fx_skid_accum_[4] = {};
    float  fx_prev_speed_[4] = {};
    float  fx_prev_yaw_[4]   = {};
    float  fx_prev_pos_[4][3] = {};     // world position last EmitCarFx call (world-unit speed, see EmitCarFx)
    bool   fx_have_prev_[4]  = {};
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
    std::vector<PickupField::Spawn> powerup_spawns_;  // POWERUPS_GOLD.DFF markers

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
    int   cam_replay_row() const { return cam_replay_row_; }   // d1r2 MASHED_CAM_REPLAY
private:

    // track props: RWP_Object DFF+MTS instanced sets + Clump_Filename DFFs
    // at identity (their frames carry placement; COURSE.LUA wiring).
    struct Prop {
        // D3D9 bake: filled ONLY for the sky dome (sky_). Track props have no D3D9
        // copy; they are drawn as librw instances.
        std::vector<std::vector<V>>     batches;   // per material
        std::vector<IDirect3DTexture9*> textures;
        std::vector<D3DMATRIX>          instances;
        // E2'b step 3: librw clump handle for this model, or -1 = not drawn
        // (RaceSubmit_RegisterModel failed; logged to log/librw_race.txt).
        int                             rw_model = -1;
        // F3: per-material UV-scroll rate (units/sec) from the DFF material's
        // RW UVAnim extension -> the track .UVA dict (sea/sky props scroll).
        std::vector<MatScroll>          mat_scroll;
        // [rstates] LibRw::kInst* kind the instances are queued with: in the RpWorld
        // (RWP_Object, non-excluded Clump_Filename) or drawn after it (excluded
        // clumps, the Arctic sea grid). See PREREG_rstates.md F2.
        int                             rw_kind = 0;
        // [rstates] RW vertex-alpha need of the mesh RpClumpRender draws LAST for this
        // model (DFF atomic 0, highest material): material alpha != 255 or a prelit
        // alpha != 255 in that batch (FUN_00543500 / default callback rule).
        bool                            last_mesh_va = false;
        // [rstates F6] COURSE.LUA Clump_Filename index (the course clump slot), -1 = not a
        // Clump_Filename prop (RWP_Object).
        int                             clump_idx = -1;
        // [rstates F8] instances are a Clump_Mts_Pair's copies, one per Mts record.
        bool                            mts_pair = false;
    };
    std::vector<Prop> props_;
    // [rstates F6] the order props_ are queued in each frame (BuildPropOrder).
    std::vector<std::size_t> prop_order_;
    void BuildPropOrder(std::FILE* log);

    // [rprops U-9695] Props whose placement the original re-poses every frame from its per-track
    // hook-table helpers (Arctic freighters 0x00448cb0, fishing boats 0x00448e20). Each entry is an
    // index into props_ whose `instances` UpdatePropAnims() rewrites once per Render. prop_acc_ are
    // the float accumulators 0x00683258 / 0x0068325c / 0x00683260 (BSS, zero at start).
    struct PropAnim { int kind; std::size_t prop; };
    std::vector<PropAnim> prop_anims_;
    float prop_acc_[3] = {};
    // [rfx2] the lensflare light FUN_00448ef0 @ 0x00448ef0 bobs (Arctic): its index, the float
    // accumulator 0x00683250, and the Render count that times the batches' one build
    int   lens_bob_ = -1;
    float lens_acc_ = 0.f;
    int   lens_frames_ = 0;
    int   prop_tick_frames_ = 0;
    void UpdatePropAnims();

    // ---- F2 .ANM animated copters (COURSE.LUA General_Anim_Filename + SetCopter;
    // KTCSCRIPT.LUA KTC_NewCopter). The flight paths are the track's .ANM
    // (Track::HAnim, rwID_HANIMANIMATION); the models are COPTER/JETRANGER/
    // KTC_APACHE.DFF (+ COPTERS.TXD) from Common/Perm.piz COPTERS/. Each copter
    // flies its bound path, looped on the race clock — drawn under the per-frame
    // HAnim transform (props otherwise draw under a static matrix). See
    // re/analysis/formats/track_anim_data.md F2.
    // [librwrest] drawn through librw (D1-residue follow-up): the model is registered with
    // its frame hierarchy. FUN_00425b90 @ 0x00425b90 hands RpClumpForAllAtomics' atomics, in
    // the clump's order (the reverse of the stream order: RW adds each streamed atomic at the
    // list head), to FUN_004516d0 @ 0x004516d0, which stores the 1st at slot+4 (body), the 2nd
    // at slot+8 and the 3rd at slot+0xc (the two rotors FUN_00425d50 spins).
    struct CopterModel {
        int   rw_model = -1;
        int   body_frame = -1, rotA_frame = -1, rotB_frame = -1;   // slot+4 / +8 / +0xc frames
        float rotA_bind[12] = {}, rotB_bind[12] = {};               // their streamed modelling
    };
    std::vector<CopterModel> copter_models_;
    // gameplay=true: bound via KTCSCRIPT.LUA KTC_NewCopter (rule-5 collectible
    // feed, D-11056/U-8997); done=true once it has completed one traversal of
    // its flight path this round (UpdateRace -> OnCollect()).
    // [librwrest] elapsed = slot+0x40 (FUN_00425d50's anim clock), rotA / rotB = the rotor
    // angles FUN_00425ca0 has added to slot+8 / slot+0xc's frames, degrees.
    struct AnimCopter { int model = -1; Track::HAnim anim; bool gameplay = false; bool done = false;
                        float elapsed = 0.f, rotA = 0.f, rotB = 0.f; };
    std::vector<AnimCopter>  copters_;
    void CopterTick(int units);
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

    // car model + state. The car has no D3D9 copy: the player body and the AI cars
    // are placed instances of librw clumps (RwRaceSubmit.h), registered by LoadCar.
    // -1 = registration failed, no car is drawn.
    int rw_car_model_ = -1;   // slot 0's livery as LoadCar was asked for it

    // [librwrest] AI car liveries. FUN_0040d110 @ 0x0040d110 gives car i the vehicle id
    // DAT_007f1a1c[i*4] + (id/6)*6 (mode 0xb: i + (id/6)*6), and FUN_00420420 @ 0x00420420
    // loads that id's DFF from the table at 0x005f37a8 (+0x10). Every table entry's DFF is
    // its name plus id % 6 (78 / 78, verify/librwrest_20261016), so car i draws
    // "<base><livery>.DFF" from the same vehicle piz. One registered model per DFF used.
    // Each is built with its frame hierarchy so the wheel / body frames can be posed.
public:
    struct CarModel {
        int   handle = -1;
        char  dff[32] = {};
        int   wheel_frame[8];     // atomic frames of parts 0..3 then 0x21..0x24 (-1 = none)
        int   hub_frame[8];       // their parent frames
        float hub_pos[8][3];      // the parents' streamed positions (FUN_0041f3d0 keeps them)
        int   body_frame = -1;    // part 0x13's atomic frame (FUN_0041f4f0)
        float body_pos[3] = {};
        bool  env_ok = false;
        // diagnostic replay only (MASHED_CAR_REPLAY): the streamed chains around the body
        // frame, X = part 0xe's frame up to the body frame (exclusive), Y = the body frame's
        // parent up to the root (exclusive); RW row-vector rows right/up/at/pos.
        bool  body_chain_ok = false;
        float body_X[12] = {}, body_Y[12] = {};
    };
    std::vector<CarModel> car_models_;
    int  car_slot_model_[4] = {0, 0, 0, 0};   // index into car_models_ per car slot
    char car_piz_[260] = {};
    char car_dff_base_[32] = {};
    bool livery_dirty_ = false;
    int  RegisterCarModel(IDirect3DDevice9* dev, const char* dff_entry, std::FILE* log);
    void ResolveCarLiveries();
    // [librwrest] FUN_00420870 @ 0x00420870's part inputs per car slot, refreshed by
    // CarParts_FrameTick where FUN_0040fc00 calls FUN_0040da50 (0x0040fc75): the wheel spin
    // angles (rec+0x220, FUN_0041f3d0), the per-wheel steer means (FUN_0046bda0) and the
    // two body vectors (FUN_0046bce0 / FUN_0046bd20) FUN_0041f4f0 rotates part 0x13 by.
    struct CarPartState { float ang[4] = {}; float steer[4] = {}; float va[3] = {}; float vb[3] = {};
                          bool ok = false; };
private:
    CarPartState car_parts_[4];
    void CarParts_FrameTick(int units);
    // [librwrest_checks] MASHED_CARPARTS_LAW=<in> MASHED_CARPARTS_LAW_OUT=<out>, DIAGNOSTIC
    // ONLY, default OFF, one shot: replays the original's recorded per-row part inputs
    // (camera_probe.py --carparts) through the same spin step and BuildCarPoses the race
    // uses, and writes the resulting frame poses for the W1 law check.
    void CarPartsLawReplay();

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
    // [D3 2026-09-14] AI bridge: fill g_aib (pos/vel/alive) from live scene state,
    // then dump the ctrl blocks the ported tick wrote (env MASHED_AI_STEPDUMP).
    // See re/analysis/D3_AUDIT_2026-09-14.md section 1.
    void  AiBridgeSnapshot();
    void  AiStepDump();
    // [U-9186 leg G1] MEASUREMENT ONLY, default-OFF (MASHED_U9186_GATES=<path>).
    // Dumps every input FUN_00442a60 reads so its gate chain can be evaluated
    // offline without porting it. All reads; writes nothing the game uses.
    // PREREG: verify/d3_u9186_20261008/PREREG_G1.md section 1.
    void  U9186GateDump();
    void  AiOptionBStep(AiCar& a, int v, float dt, int ng);   // MASHED_REAL_PHYSICS=0 revert only
    void  EnsurePowerupBackend();                     // lazy Init(this)
    void  SyncHostCar();                              // fill pu_player_/pu_ai_
    // [D3 WS-D G-D1/G-D2] the per-frame dispatcher tick (FUN_0045bba0), 4 slots.
    void  TickPowerupDispatch(float dt);
    int   MissileTargetAhead() const;                 // nearest AI ahead, or -1
public:
    // The fire key's HELD state for this frame (slot 0). The dispatcher derives
    // press/held/release (fire_mode 2/3/1) from it and last frame's value.
    void SetPowerupFire(bool held) { pu_fire_held_ = held; }
    // Demo: hold fire on the armed slot-0 power-up for ~0.5 s. False if none armed.
    bool FireHeldPowerup();
    // Demo/testing: arm a specific power-up TYPE CODE on slot 0 (replacing any
    // held one) and hold fire for ~0.5 s.
    void FirePowerupKind(int code);
    // [mode 7 M3] FUN_00458920's grant FUN_0045c010(v, type): the ported FUN_0045bfa0 (Activate) for car v.
    void PickupGrant(int v, int type);
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
    // FUN_0040eee0's TEAM arms (the DAT_0067ea64 != 0 side of every
    // FUN_0042f500 test). Team ids come from the original's own per-slot field
    // DAT_007f1a18 + slot*0x10, which MenuTeamBalance (0x0042bb60) fills:
    // -1 = slot has no assigned player, 0 / 1 = the two teams. A car with -1
    // is skipped exactly as the original's `piVar8[-1] != -1` guard skips a
    // slot with no profile.
    void  SetTeamPlay(bool on, const int team_of[4]);
    void  ScoreOnEliminationTeams(int victim);     // FUN_0040eee0 team arms
    void  KillCar(int car);                        // FUN_00422fd0 equivalent
    void  AwardByTeam(int winTeam, int delta);
    float RacePct(int car) const;                  // 0x00408ad0 equivalent
    bool  ProgBehind(int A, int B) const;          // 80/20/100 wrap compare
    bool  team_play() const { return team_play_; }
    int   team_of(int car) const {
        return (car >= 0 && car < kRaceCars) ? team_of_[car] : -1;
    }
    void  StartRound();           // grid all 4 cars at the start line
    // Reset + seed scores (FUN_0040b180), start round 1. The match length is
    // not a parameter: the original derives it from the seed (6, or 4) and
    // FUN_00410510's per-rule target (score > 0xb, or == 8). D3 2026-09-26.
    void  StartMatch();
    // FUN_0040b180 (0x0040b180): initial score from rule / participants /
    // teams. Re-run by SetRaceRule and SetTeamPlay (setup-only calls, before
    // any scoring) so the seed sees the final inputs, as the original's single
    // call at race setup (FUN_004111c0 @0x0040ffb8) does.
    void  SeedMatchScores();
    // DAT_008a94d0 equivalent: count of occupied car slots (the original's
    // 0x0040ff40..0x0040ffb6 loop over *(PTR_005f2770+0x34+i*4)); the same
    // predicate UpdateRace uses for RuleEngine::Cars::active.
    int   ParticipantCount() const;
    void  NextRoundOrEnd();          // check match win, start next round
    int   match_winner() const { return match_winner_; }
    int   round_no() const { return round_no_; }
    int   score(int car) const { return (car >= 0 && car < kRaceCars) ? scores_[car] : 0; }
    // Per-car Player Colour 0..5 (DAT_007f1a1c mirror, Finding 16). Drives the
    // standings car badge. Default 0 (RED).
    int   car_colour(int car) const { return (car >= 0 && car < kRaceCars) ? colour_[car] : 0; }
    void  SetCarColour(int car, int c) {
        if (car >= 0 && car < kRaceCars) { colour_[car] = c; livery_dirty_ = true; }   // [librwrest]
    }
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
    // [D-11073 leg 2] RaceCamCar adapter shared by UpdateRace and the MASHED_SUBSTATE pre-race
    // camera chain (FUN_0040d470 -> FUN_00446520: entry init, entry dispatch, camera update, copy).
    void  FillRaceCamCars(Race::RaceCamCar* cc);
    // [d1r2] race_cam_.Update with the host inputs, or the MASHED_CAM_REPLAY row.
    void  RaceCamUpdate(const Race::RaceCamCar* cc, bool reset);
    void  SubStateCameraChain(bool reset);
    int   sub_p3_frames_ = 0;   // phase-3 frames this round (leg-2 trace / measurement knob)
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
