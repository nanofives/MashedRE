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
};

}  // namespace D3d9Render
}  // namespace mashed_re
