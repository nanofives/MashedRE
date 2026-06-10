// Mashed RE — R4 opener: track world renderer + fly-through camera.
//
// Renders a parsed Track::World (GRAPH*.BSP) through D3D9 fixed-function:
// one triangle-list batch per material, textures resolved by material name
// from the track's TXD (TEXTURES.TXD or <TRACK>.TXD; same chunk-0x23
// container as the menu TXDs), vertex diffuse = the world's baked prelight.
//
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
    struct V { float x, y, z; D3DCOLOR c; float u, v; };
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

    // car model + state
    std::vector<std::vector<V>>     car_batches_;
    std::vector<IDirect3DTexture9*> car_textures_;
    bool  car_ready_   = false;
    float car_pos_[3]  = {};
    float car_yaw_     = 0.f;
    float car_speed_   = 0.f;
    float car_ground_off_ = 0.f;   // model bbox min-Y -> wheels on ground
};

}  // namespace D3d9Render
}  // namespace mashed_re
