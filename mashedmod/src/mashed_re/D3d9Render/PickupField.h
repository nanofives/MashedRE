// PickupField — in-race power-up pickups (2026-06-15). Replaces the
// IPowerupSystem stub with a real, interactive minimal pickup loop:
//   * orbs placed along the AI gate ribbon, bobbing + spinning billboards;
//   * collected when the player car drives through one;
//   * respawn after a cooldown;
//   * a held-power-up slot + collected counter the HUD reads.
//
// [SCAFFOLD] The power-up EFFECTS (missiles/mines/shock — FUN_00430670 family)
// are NOT simulated; this is the pickup/economy layer only. The orb placement
// and effect roster are invented presentation pending RE of the power-up cluster.
#pragma once

#include <d3d9.h>
#include <cstdint>
#include <vector>
#include <array>

namespace mashed_re {
namespace D3d9Render {

class PickupField {
public:
    // Power-up roster (names only; effects un-ported). -1 = none held.
    enum Kind { Missile = 0, Mine, Shock, Boost, Shield, kKindCount };
    static const char* KindName(int k);

    bool EnsureTexture(IDirect3DDevice9* dev);
    // Place orbs at a subset of the supplied world positions (e.g. gate centers).
    void Init(const std::vector<std::array<float, 3>>& spots, float worldRadius);
    void Reset();

    // Advance bob/spin + respawn timers; collect orbs the player drives through.
    // Returns true on the frame an orb is collected (host may play an SFX).
    bool Update(float dt, const float carPos[3]);
    void Render(IDirect3DDevice9* dev, const float camEye[3], const float camAt[3]);

    int  collected() const { return collected_; }
    int  held() const { return held_; }
    // Use the held power-up: returns its kind (or -1 if none) and clears it.
    int  ConsumeHeld() { int h = held_; held_ = -1; return h; }
    bool enabled() const { return !orbs_.empty(); }

private:
    struct Orb { float pos[3]; bool active; float respawn; std::uint32_t col; };
    struct PV  { float x, y, z; D3DCOLOR c; float u, v; };

    std::vector<Orb>   orbs_;
    IDirect3DTexture9* tex_   = nullptr;
    float              worldR_ = 100.f;
    float              phase_  = 0.f;     // shared bob/spin phase
    int                collected_ = 0;
    int                held_   = -1;
    std::uint32_t      rng_    = 0x51ed270bu;
    std::vector<PV>    verts_;
    float Frand();
};

}  // namespace D3d9Render
}  // namespace mashed_re
