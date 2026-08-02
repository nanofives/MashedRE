// Race/RaceSceneState.h — the renderer-neutral half of the race scene.
//
// Lane M3-E2'c step 1 (gate D2). Risk R1 in re/analysis/LIBRW_SIZING_2026-08.md:
// D3d9Render/TrackRenderer.cpp fused ~4100 LOC of D3D9 draw path with the race
// simulation, so swapping the renderer would have dragged gameplay along with it.
// This type is the simulation half, pulled out with NO behaviour change.
//
// WHY INHERITANCE, NOT COMPOSITION. TrackRenderer derives from this rather than
// holding one. Composition would have meant rewriting several hundred `foo_`
// references across TrackRenderer.cpp to `state_.foo_` — a large mechanical diff
// with real typo risk, in a commit whose entire value is being provably a no-op.
// Deriving moves only the declarations; every existing reference in the .cpp
// still resolves, unchanged, through the base. The architectural goal is met
// either way: the simulation now lives in a type with no D3D9 dependency, which
// a librw submitter can consume as `const RaceSceneState&`.
//
// This is STEP 1 OF 2. It does not yet break the coupling — TrackRenderer still
// *is-a* state holder. Flipping to composition is a follow-up, worth doing once
// E2'b has shown which members a submitter actually reads. Doing it now would be
// guessing at that interface.
//
// Members are public by design. This is a state bag consumed by renderers, not
// an encapsulated object; the accessors that enforce invariants stay on
// TrackRenderer. (It also keeps `friend class PowerupBackendImpl` working
// unchanged.)
//
// WHAT DELIBERATELY STAYED BEHIND in TrackRenderer:
//   - anything typed on D3D9: the V/RelitSrc vertex structs and every
//     std::vector<V> batch, IDirect3DTexture9*, and D3DMATRIX.
//   - ParticleSystem parts_ / PickupField pickups_ / powerup_spawns_, whose own
//     headers include <d3d9.h>.
//
// E2'b step 3 (2026-08-01) CLOSED the D3DCOLOR deferral noted here: fog_color_,
// amb_world_ and sun_color_ moved in and retyped to uint32_t, joined by the
// resolved-camera quartet (last_fov_/last_aspect_/last_near_/last_far_). That is
// what makes the state reachable from a renderer that is not D3D9 -- the librw
// submitter consumes this struct and never re-parses COURSE.LUA or LIGHTS.DFF.
#pragma once

#include <cstdint>
#include <vector>

#include "RaceCamera.h"
#include "RuleEngine.h"
#include "../Powerup/PowerupSystem.h"
#include "../Track/TrackData.h"

namespace mashed_re {
namespace Race {

struct RaceSceneState {
    // ---- world extents ----------------------------------------------------
    float center_[3] = {};
    float radius_    = 1.f;
    // Orbit-camera focus derived from the AI gate ribbon (the raceable track),
    // not the raw world bbox — the bbox is skewed by skybox/backdrop geometry,
    // so orbiting its midpoint frames mostly empty sky. Falls back to the bbox
    // center/radius when there are too few gates. Set in Load().
    float track_center_[3] = {};
    float track_radius_    = 1.f;
    float last_t_          = -1.f;   // for per-frame dt (particles)

    // ---- free/orbit dev camera --------------------------------------------
    bool  free_        = false;
    float eye_[3]      = {};
    float yaw_         = 0.f;
    float pitch_       = -0.4f;
    float last_eye_[3] = {};
    float last_at_[3]  = {};

    // ---- collision + render triangle soups ---------------------------------
    // Collision world (flat soup for the ground raycast) plus the render
    // world's soup (spawn validation: ground must be VISIBLE — the frozen-bay
    // ice has collision but its SEA.DFF visual is an unrendered prop, so
    // collision-only scoring spawns the car on invisible ice).
    std::vector<float>         col_verts_;   // x,y,z per vertex
    std::vector<std::uint32_t> col_tris_;    // v0,v1,v2 triples
    std::vector<float>         rend_verts_;
    std::vector<std::uint32_t> rend_tris_;

    // ---- AI path gates -----------------------------------------------------
    // AI*.BSP: 94-ish vertical quads; material RED byte = the gate index
    // LAPDATA's Lap_Line numbers refer to. gate 0 = start line. c0/c3 =
    // first/fourth vertex in stream order (the original's node corners j=0/j=3,
    // FUN_00426d00); dir = unit race direction (the original's node +0x00,
    // FUN_00426cc0) — feeds the RaceCamera port.
    struct Gate {
        float center[3];
        float c0[3], c3[3];
        float dir[3];
    };
    std::vector<Gate> gates_;

    // F4 LAPDATA.LUA: real lap lines, split sectors, safe-start ranges.
    Track::LapData lap_data_;

    // F3 .UVA UV-anim: per-material scrolling-UV rate (units/sec). Sized to the
    // renderer's batches_ (one entry per world material); zero = static.
    struct MatScroll { float du = 0.f, dv = 0.f; };
    std::vector<MatScroll> mat_scroll_;
    bool uv_anim_ = false;          // any world material scrolls

    // ---- ported race camera + course wiring --------------------------------
    Race::RaceCamera            race_cam_;
    std::vector<Race::RaceCamNode> cam_nodes_;
    int    course_id_   = -1;       // COURSE.LUA Course_Id(N) -> LE<N>.LED
    char   gate_bsp_[64] = {};      // COURSE.LUA AI_Bsp_Filename (per-track)
    double cam_ticks_   = 0.0;      // DAT_007f1030 equivalent (~3.0 MHz live)

    // ---- fog + lighting ----------------------------------------------------
    bool  fog_on_    = false;
    float fog_start_ = 0.f, fog_end_ = 100.f;
    // E2'b step 3 (2026-08-01): the three former D3DCOLOR members moved here
    // from TrackRenderer and retyped to uint32_t -- the retype §3.4 deferred.
    // D3DCOLOR is a DWORD typedef, so every existing use site (D3DCOLOR_XRGB
    // packing, SetRenderState(D3DRS_FOGCOLOR), Clear()) is source-compatible.
    // They live here so the librw submitter can READ the values TrackRenderer
    // already parsed instead of re-parsing COURSE.LUA / LIGHTS.DFF -- a second
    // copy of a parser that must agree with the first is the "wrong plate
    // propagates into ports" failure mode (LIBRW_SIZING_2026-08.md, step-3 block).
    //
    // fog: COURSE.LUA Setup_Fog(near, far, r, g, b), parsed TrackRenderer.cpp:1268.
    std::uint32_t fog_color_ = 0x00181C28u;  // = D3DCOLOR_XRGB(24, 28, 40)
    // WS-E lighting: track ambient RpLight term (LIGHTS.DFF, COURSE.LUA
    // Lights_Filename) as 0x00RRGGBB; added to world/prop baked prelight. The
    // dim baked prelight (Arctic mean ~55,78,78) is meant to be combined with
    // this ambient (Arctic 51,76,76) at render -- without it the world is a dark
    // void. 0 = no lights file. Parsed in Load() before the batches are built.
    std::uint32_t amb_world_ = 0;
    // WS-E s2 lighting: the track's DIRECTIONAL RpLight (LIGHTS.DFF type-1) --
    // sun colour as 0x00RRGGBB and its world-space direction (the light frame's
    // at-vector, i.e. the direction the light travels). Applied as N.L to ATOMIC
    // (prop/car) batches that carry vertex normals + rpGEOMETRYLIGHT; the static
    // world has no normals so it cannot receive it. Arctic LIGHTS.DFF
    // (asset-verified): colour (0.6,0.7,0.7)=(153,178,178), dir
    // (0.577,-0.577,-0.577), flags 0x3 (lights atomics+world). 0 = none.
    std::uint32_t sun_color_ = 0;

    std::uint32_t fog_color() const { return fog_color_; }

    // ---- resolved camera, written once per frame by the active renderer -----
    // E2'b step 3: last_eye_/last_at_ (above) gave eye+target but NOT the
    // projection, whose constants were function-local at TrackRenderer.cpp:3733.
    // The librw submitter must build the SAME frustum, so TrackRenderer now
    // publishes them here rather than a second copy being written by hand -- an
    // approximated camera would look authoritative while measuring nothing.
    float last_fov_    = 1.0472f;   // vertical FOV, radians (60 deg)
    float last_aspect_ = 800.f / 600.f;
    float last_near_   = 0.05f;
    float last_far_    = 8.f;       // TrackRenderer: radius_ * 8.f
    // WS-E vehicle lighting (RpLight subset, env MASHED_RPLIGHT). Faithful
    // FUN_00479330 light acquisition: float-precision colours, default lights
    // when COURSE.LUA has no Lights_Filename, DFF lights keyed on the subtype
    // byte alone. has_sun_dir_ gates the per-frame relight pass.
    bool  rp_light_on_ = false;
    bool  has_sun_dir_ = false;
    float amb_f_[3]    = {0.f, 0.f, 0.f};   // ambient RGB, float
    float sun_f_[3]    = {0.f, 0.f, 0.f};   // directional RGB, float
    float sun_L_[3]    = {0.f, 0.f, 0.f};   // unit world dir TO the light
    float sun_dir_[3]  = {0.f, 0.f, 0.f};   // world dir the light travels

    // ---- player car simulation state ---------------------------------------
    bool  car_ready_      = false;
    float car_pos_[3]     = {};
    float car_yaw_        = 0.f;
    float car_speed_      = 0.f;
    float car_vel_[3]     = {};     // +0x9b0-shape velocity vector (world)
    float car_ground_off_ = 0.f;    // model bbox min-Y -> wheels on ground
    float car_len_        = 1.f;    // long-axis extent (chase-cam scale)
    float car_height_     = 0.5f;   // height extent (chase-cam scale)
    bool  car_long_is_x_  = true;   // nose axis: true=+X, false=+Z
    float wheel_spin_     = 0.f;
    float steer_vis_      = 0.f;

    // ---- AI cars ------------------------------------------------------------
    struct AiCar {
        float pos[3]; float yaw; int target; float speed;
        float cur_speed; float lane;   // lane = signed lateral offset
        float spin = 0.f;              // >0: spun out (missile/mine hit)
        float slow = 0.f;              // >0: shocked (capped speed)
        float vel[3] = {0.f, 0.f, 0.f};// G3: persistent world velocity
        float stuck_t = 0.f;           // s since this car last advanced a gate
        int   prog_gate = -1;          // last observed race gate
    };
    std::vector<AiCar> ai_cars_;

    // ---- power-up effect state ----------------------------------------------
    float boost_timer_  = 0.f;     // no powerup sets it now (general hook)
    float shield_timer_ = 0.f;     // idem
    struct Missile { float pos[3]; float vel[3]; int target; float life; };
    std::vector<Missile> missiles_;
    struct Mine { float pos[3]; float life; };
    std::vector<Mine> mines_;

    Powerup::PowerupSystem        pw_;
    Powerup::IPowerupBackend*     pu_be_ = nullptr;   // lazily-created backend
    Powerup::HostCar              pu_player_;         // filled each fire
    std::vector<Powerup::HostCar> pu_ai_;
    float pu_oil_last_[3] = {0, 0, 0};                // OIL drop-distance trail
    bool  pu_oil_has_     = false;

    // ---- race bookkeeping ----------------------------------------------------
    struct RaceCar {           // per car (0 = player)
        int   gate = 1;        // next gate to cross
        int   laps = 0;
        float progress = 0.f;  // gate + fraction (ranking metric)
        bool  alive = true;
        // F4 (FUN_00408610): bitmask of LAPDATA Lap_Line gates crossed since
        // the last lap. A lap completes on the primary line once every declared
        // line is set (the multi-Lap_Line anti-shortcut).
        std::uint32_t lap_mask = 0;
    };
    static constexpr int kRaceCars = 4;
    RaceCar race_[kRaceCars];

    bool round_mode_   = false;   // MASHED_ROUND: 4-car exhibition round
    int  round_alive_  = kRaceCars;
    int  round_winner_ = -1;      // set when the round ends
    // PORTED points system (FUN_0040eee0 + FUN_0040b290 + Race::EvaluateResult
    // 0x00410510). Match win at score > 11 (DAT_008a94d0 == 4).
    int   scores_[kRaceCars]      = {};            // DAT_008a94e0
    int   score_prev_[kRaceCars]  = {};            // DAT_008a9570
    int   score_delta_[kRaceCars] = {};            // DAT_008a9520
    float delta_timer_[kRaceCars] = {};            // DAT_008a9510 (ms)
    int   elim_order_[kRaceCars]  = {-1, -1, -1, -1};  // DAT_008a94c0
    int   elim_count_   = 0;
    int   match_winner_ = -1;
    int   round_no_     = 0;
    int   race_mode_    = 0;      // 0 = elimination, 1 = laps
    int   lap_target_   = 3;      // laps mode: laps to finish

    // [D-11052] rule engine state (Race/RuleEngine). rule_ = DAT_007f0fd0.
    int   rule_                 = 0;
    bool  rule_engine_on_       = false;
    bool  rule_engine_race_on_  = true;
    bool  match_draw_           = false;
    float rule10_bonus_         = 0.f;   // per-checkpoint award (FUN_004046a0)
    std::uint32_t rule10_hit_mask_ = 0;  // lap lines awarded this lap
    int   rule10_lap_seen_      = -1;
    Race::RuleEngine::Persist rulep_;

    bool  human_drive_ = false;   // true: player car uses input
    float countdown_   = 0.f;     // >0 = pre-go freeze (seconds remaining)

    // F4: race clock (seconds since GO) + per-Split_Sector split times for the
    // player car (mirrors DAT_008a964c / FUN_00411600). Reset each lap.
    static constexpr int kMaxSplits = 8;
    float race_time_ = 0.f;
    float split_time_[kMaxSplits] = {};
    bool  split_done_[kMaxSplits] = {};
};

}  // namespace Race
}  // namespace mashed_re
