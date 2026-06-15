// ParticleSystem — billboarded D3D9 particles for the in-race scene (2026-06-15).
//
// Replaces the IParticleSystem stub: a fixed-size pool of camera-facing quads
// drawn with a procedural radial-alpha texture. Two emitters:
//   * AMBIENT track weather — Snow (Arctic) / Dust drifting around the camera.
//   * Car DUST — kicked up behind the car, spawn rate scaled by speed.
// Owned by TrackRenderer (which has the device + camera + car state) and drawn
// at the end of its 3D pass with depth-test ON / depth-write OFF so particles
// are occluded by geometry but don't occlude each other.
//
// [SCAFFOLD] The emitter shapes/rates are invented presentation, not RE'd from
// the original's particle system (Particle/ dir, still un-ported). The DATA path
// (D3D9 billboards) is real; tune/replace rates when the original is reversed.
#pragma once

#include <d3d9.h>
#include <cstdint>
#include <vector>

namespace mashed_re {
namespace D3d9Render {

class ParticleSystem {
public:
    enum Ambient { None, Snow, Dust };

    bool EnsureTexture(IDirect3DDevice9* dev);  // lazy radial-alpha texture
    void SetAmbient(Ambient a, float worldRadius);
    void Reset();
    Ambient ambient() const { return amb_; }

    // Advance the pool + spawn ambient (around camEye) and car dust (behind the
    // car when moving). camFwd biases ambient spawning into view.
    void Update(float dt, const float camEye[3], const float camFwd[3],
                const float carPos[3], float carSpeed);

    // Draw alive particles as camera-facing billboards. Assumes a scene is in
    // progress and view/proj are set; saves/restores the states it changes.
    void Render(IDirect3DDevice9* dev, const float camEye[3], const float camAt[3]);

    // One-off FX (power-up trails / explosions). Burst = n particles flung out
    // from `pos`; Trail = a single drifting particle (call each frame on a
    // moving projectile). Sizes are absolute world units.
    void SpawnBurst(const float pos[3], int n, std::uint32_t col,
                    float speed, float size, float life);
    void SpawnTrail(const float pos[3], std::uint32_t col, float size, float life);

    int alive() const;

private:
    struct P {
        float        pos[3] = {0, 0, 0};
        float        vel[3] = {0, 0, 0};
        float        life = 0.f, maxlife = 1.f, size = 1.f;
        std::uint32_t col = 0xffffffffu;
    };
    struct PV { float x, y, z; D3DCOLOR c; float u, v; };

    P*    Spawn();
    float Frand();                 // 0..1 deterministic LCG/xorshift

    std::vector<P>       pool_;
    int                  next_ = 0;
    IDirect3DTexture9*   tex_  = nullptr;
    Ambient              amb_  = None;
    float                worldR_ = 100.f;
    float                snowAccum_ = 0.f;
    float                dustAccum_ = 0.f;
    std::uint32_t        rng_ = 0x9e3779b9u;
    std::vector<PV>      verts_;   // scratch billboard buffer
};

}  // namespace D3d9Render
}  // namespace mashed_re
