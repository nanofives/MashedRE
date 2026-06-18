// Mashed RE — WS-A6b: aerodynamic stabilization FUN_00468980.
//
// STATUS: verbatim port, PENDING diff-original C4. The matrix-apply gap [U-A6B-MTX] is
// now RESOLVED from the asm (listing 0x00468980..0x00468b34, Ghidra pool11 2026-06-17):
//   self = ECX (this); orient = ESI = the vehicle world-transform RwMatrix (right@0,
//   up@0x10, at@0x20, pos@0x30); dt = first stack arg. Airborne only (grounded +0x9e0==0).
//   motion-state +0x9f0 == 0:
//     c1 = clamp(at.y = orient[9], -1, 1);   angle1 = -(acos(c1)*-2 - 0)*dts   (pitch)
//        RwMatrixRotate(orient, X=(1,0,0)@0x6146f0, angle1, 1)
//     c2 = clamp(right.y = orient[1], -1, 1); angle2 = (acos(c2)*-2 - 90)*dts   (roll)
//        RwMatrixRotate(orient, Z=(0,0,1)@0x614708, angle2, 1)
//   motion-state != 0: zero ang-vel, normalize linear vel -> dir,
//        RwMatrixRotate(orient, dir, dt*0.05, 1)
// dts = dt*0.001 (005cc558). acos = FUN_004a3384 = std::acos. Consts memory_read 2026-06-17.
//
// CAVEAT: RwMatrixRotate mode 1 (PRECONCAT) dispatches the RW DEVICE matrix-mult — it is
// standalone-correct only once RW device init lands (WS-E); in the dev .asi (A-VERIFY-2) it
// hits the real device. `orient==nullptr` guards the standalone-inert case until A8 binds
// the vehicle transform matrix.
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "ForceIntegrator.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace mashed_re {
namespace Vehicle {

extern "C" void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode);

static inline float Rf(void* b, int off) { float v; std::memcpy(&v, (char*)b + off, 4); return v; }
static inline void  Wf(void* b, int off, float v) { std::memcpy((char*)b + off, &v, 4); }
static inline int   Ri(void* b, int off) { int v; std::memcpy(&v, (char*)b + off, 4); return v; }

// exact-bit constructors (memory_read pool6 2026-06-17; the decimal literals for
// 180/pi mis-round). Cf=float, Cd=double from the .rdata qword.
static inline float  Cf(std::uint32_t bits) { float  f; std::memcpy(&f, &bits, 4); return f; }
static inline double Cd(std::uint64_t bits) { double d; std::memcpy(&d, &bits, 8); return d; }

namespace a6b {
constexpr float kDtScale = 0.001f;   // _DAT_005cc558
constexpr float kNegOne  = -1.0f;    // _DAT_005cc33c
constexpr float kOne     =  1.0f;    // _DAT_005cc320
// FUN_00468980 disasm (0x00468a0e..0x00468a9c, pool6 2026-06-17): BOTH the pitch
// and roll angles are acos(c) * RAD2DEG - 90 (degrees), then * dts. The original
// FMULs _DAT_005ccae0 = double 180/pi (NOT -2.0 — prior labeling was WRONG) and
// FSUBs 90: pitch uses the DOUBLE 90.0 @_DAT_005ccad8 (then FCHS), roll uses the
// FLOAT 90.0f @_DAT_005ccad0 (no FCHS).
const     double kRad2Deg   = Cd(0x404ca5dcc0000000);  // _DAT_005ccae0 = 180/pi (57.295799)
const     double kBias90d   = Cd(0x4056800000000000);  // _DAT_005ccad8 = 90.0 (double, pitch)
const     float  kBias90f   = Cf(0x42b40000);          // _DAT_005ccad0 = 90.0f (float, roll)
constexpr float kVelAng  = 0.05f;    // _DAT_005cc9a0
// axis vectors (memory_read 0x006146f0..): X=(1,0,0), Z=(0,0,1)
const float kAxisX[3] = { 1.0f, 0.0f, 0.0f };   // _DAT_006146f0
const float kAxisZ[3] = { 0.0f, 0.0f, 1.0f };   // _DAT_00614708
} // namespace a6b

static inline float clamp11(float d) {
    using namespace a6b;
    if (d < kNegOne) return kNegOne;
    return (kOne < d) ? kOne : d;
}

// 0x00468980 — Vehicle_AeroStabilize(self, orient, dt). orient = vehicle world-transform
// RwMatrix (16 floats); nullptr -> rotation-apply skipped (standalone-inert until A8).
void Vehicle_AeroStabilize(int* self, float* orient, float dt)
{
    using namespace a6b;
    void* v = self;
    if (Rf(v, 0x9e0) != 0.0f) return;                 // grounded -> not airborne

    if (Ri(v, 0x9f0) == 0) {                           // motion state 0: auto-level
        const float dts = dt * kDtScale;
        if (orient) {
            // pitch: -(acos(c1)*180/pi - 90.0_double) * dts   (FCHS @0x00468a1c)
            const float c1 = clamp11(orient[9]);       // at.y  (pitch)
            const float ang1 = (float)(-((double)std::acos((double)c1) * kRad2Deg - kBias90d) * (double)dts);
            RwMatrixRotate(orient, kAxisX, ang1, 1);
            // roll: (acos(c2)*180/pi - 90.0_float) * dts      (no FCHS)
            const float c2 = clamp11(orient[1]);       // right.y (roll)
            const float ang2 = (float)(((double)std::acos((double)c2) * kRad2Deg - (double)kBias90f) * (double)dts);
            RwMatrixRotate(orient, kAxisZ, ang2, 1);
        }
        return;
    }

    // motion state != 0: zero angular velocity, align orientation to velocity direction
    Wf(v, 0x9c4, 0.0f); Wf(v, 0x9c0, 0.0f); Wf(v, 0x9bc, 0.0f);
    float dir[3] = { Rf(v, 0x9b0), Rf(v, 0x9b4), Rf(v, 0x9b8) };
    Vec3Norm3(dir, dir);
    if (orient) RwMatrixRotate(orient, dir, dt * kVelAng, 1);
}

} // namespace Vehicle
} // namespace mashed_re
