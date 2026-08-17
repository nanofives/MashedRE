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
//
// 2026-08-16: the FX class (kind==2 — SpawnBurst/SpawnTrail, i.e. explosions and
// power-up trails) is CUT FROM THE DEFAULT BUILD. Measured, it alone produced the
// saturated-orange in-race frames that blocked the D1 renderer inversion
// (verify/d1_fxbloom/RESULT.md). Set MASHED_PARTS_KINDS=7 to restore it. The
// ambient and car-spray classes are unaffected and still draw by default.
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

    // Vertical FOV of the projection the billboards will be drawn under, in
    // radians. Render() needs it to convert a world-space billboard size into
    // the fraction of the viewport it covers (the FX lens-wall guard). Defaults
    // to the TrackRenderer value (60 deg) if never set.
    void SetFovY(float fovy);

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
        int          kind = 0;  // particle class: 0=ambient 1=car-spray 2=fx
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
    float                tanHalfFov_ = 0.57735027f;   // tan(60deg/2), see SetFovY
    std::vector<PV>      verts_;   // scratch billboard buffer
};

}  // namespace D3d9Render
}  // namespace mashed_re
