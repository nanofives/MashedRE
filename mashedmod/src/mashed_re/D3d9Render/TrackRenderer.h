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

    // Draw one frame: auto-orbit fly-through around the world bbox at time
    // `t` seconds (camera yaw = t * 0.3 rad). Assumes BeginScene is active.
    void Render(IDirect3DDevice9* dev, float t);

    bool ready() const { return ready_; }

private:
    struct V { float x, y, z; D3DCOLOR c; float u, v; };
    static constexpr DWORD kFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    std::vector<std::vector<V>>      batches_;   // per material, tri lists
    std::vector<IDirect3DTexture9*>  textures_;  // per material (may be null)
    float  center_[3] = {};
    float  radius_    = 1.f;
    bool   ready_     = false;
};

}  // namespace D3d9Render
}  // namespace mashed_re
