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
// is FAITHFUL — format-cracked and validated. Everything BEHAVIORAL in this
// class is [SCAFFOLD] — invented stand-ins to be REPLACED by RE'd verbatim
// ports, NOT refined: the orbit/chase/centroid cameras, the kinematic+
// harvested-rates handling, AI lane/braking driving, the ">7 gates"
// elimination rule, first-to-N scoring, spawn scorer, drive/round demos.
// ARCHITECTURE NOTE: this is the R4 *opening spike* — a minimal D3D9 path
// consuming the cracked RW data so the renderer-architecture decision
// (librw vs RW-subset port vs custom D3D9) is made against something
// concrete. It deliberately prejudges nothing: the parsed Track::World is
// renderer-agnostic.
#pragma once

#include <d3d9.h>
#include <cstdint>
#include <vector>

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

private:
    static constexpr DWORD kFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    std::vector<std::vector<V>>      batches_;   // per material, tri lists
    std::vector<IDirect3DTexture9*>  textures_;  // per material (may be null)
    float  center_[3] = {};
    float  radius_    = 1.f;
    bool   ready_     = false;

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
    struct Gate { float center[3]; };
    std::vector<Gate> gates_;

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
    };
    std::vector<AiCar> ai_cars_;

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
    // match / round structure
    int   wins_[kRaceCars] = {};  // round wins this match
    int   match_target_ = 3;      // first-to-N rounds
    int   match_winner_ = -1;
    int   round_no_ = 0;
    float countdown_ = 0.f;       // >0 = pre-go freeze (seconds remaining)
    void  StartRound();           // grid all 4 cars at the start line
    void  StartMatch(int first_to);  // reset wins, start round 1
    void  NextRoundOrEnd();          // tally the win, start next or finish
    int   match_winner() const { return match_winner_; }
    int   match_target() const { return match_target_; }
    int   round_no() const { return round_no_; }
    int   wins(int car) const { return (car >= 0 && car < kRaceCars) ? wins_[car] : 0; }
    float countdown() const { return countdown_; }
    // round step (also advances player bookkeeping outside round mode)
    void  UpdateRace(float dt);
    int   round_winner() const { return round_winner_; }
    int   round_alive() const { return round_alive_; }

private:
    float car_vel_[3] = {};       // +0x9b0-shape velocity vector (world)
};

}  // namespace D3d9Render
}  // namespace mashed_re
