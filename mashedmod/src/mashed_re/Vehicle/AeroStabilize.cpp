// Mashed RE — WS-A6b: aerodynamic stabilization FUN_00468980.
//
// STATUS: verbatim port, PENDING diff-original C4. The orientation-matrix source
// (original `unaff_ESI`, a float* in a register) and the two FUN_004c4d20 rotation-apply
// argument sets are IMPLICIT REGISTERS not recoverable from the C decompilation —
// marked [UNCERTAIN U-A6B-MTX]; the asm-level register dataflow + a diff-original run
// must resolve them. The determinable control flow + scalar math (airborne gate, up-axis
// dot clamp, acos torque, angular-vel zero + velocity normalize) is faithful.
//
// FUN_004a3384 = CRT acos (Ghidra C1 note "MSVC CRT acos") -> std::acos.
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "ForceIntegrator.h"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace mashed_re {
namespace Vehicle {

// Vec3Norm3 (ForceIntegrator.h inline) wraps the normalize; the matrix-apply path is the
// [U-A6B-MTX] gap (FUN_004c4d20 RwMatrixRotate args are register-implicit, deferred).
static inline float Rf(void* b, int off) { float v; std::memcpy(&v, (char*)b + off, 4); return v; }
static inline void  Wf(void* b, int off, float v) { std::memcpy((char*)b + off, &v, 4); }

constexpr float kDtScale = 0.001f;   // _DAT_005cc558 (0x3a83126f)
constexpr float kNegOne  = -1.0f;    // _DAT_005cc33c (0xbf800000)
constexpr float kOne     = 1.0f;     // _DAT_005cc320
constexpr float kAcosM   = -2.0f;    // _DAT_005ccae0 (0xc0000000)
constexpr float kAcosB   = 0.0f;     // _DAT_005ccad8 (0x00000000)
// up axis (_DAT_006146fc,_DAT_00614700,_DAT_00614704) = (0,1,0).

// 0x00468980 — Vehicle_AeroStabilize(self, dt). `orient` is the vehicle's 3x3 world
// orientation rows (original unaff_ESI) — [UNCERTAIN] which sub-object; pass self+offset
// once resolved. Here we accept it explicitly.
void Vehicle_AeroStabilize(int* self, float dt)
{
    void* v = self;
    if (Rf(v, 0x9e0) != 0.0f) return;        // grounded-wheel count != 0 -> not airborne

    if (*reinterpret_cast<int*>((char*)v + 0x9f0) == 0) {     // motion state 0
        float dts = dt * kDtScale;
        // up-axis dot of orientation row2 / row0 with (0,1,0) == that row's Y component.
        // [UNCERTAIN U-A6B-MTX] orient rows live in the register float*; using the record's
        // world-transform sub-object would require its resolved offset. Compute from the
        // forward/up fields we DO have is not equivalent — left faithful to the scalar form:
        float* orient = reinterpret_cast<float*>((char*)v + 0x9d4);  // [UNCERTAIN] fwd-vec stand-in for row source
        float d2 = orient[1];                                    // row2.y analogue
        float c2 = (kNegOne <= d2) ? ((kOne < d2) ? kOne : d2) : kNegOne;
        float ang2 = -((float)std::acos((double)c2) * kAcosM - kAcosB) * dts;   // = 2*acos(c2)*dts
        (void)ang2;
        // [UNCERTAIN U-A6B-MTX]: FUN_004c4d20(matrix, axis, ang2, mode) — axis/target/mode
        // are register-implicit in the decomp. Rotation-apply deferred to the diff-original
        // resolution; no faithful args available. (Original applies a stabilizing rotation.)
        float d0 = orient[0];
        float c0 = (kNegOne <= d0) ? ((kOne < d0) ? kOne : d0) : kNegOne;
        float ang0 = -((float)std::acos((double)c0) * kAcosM - kAcosB) * dts;
        (void)ang0;
        return;
    }

    // motion state != 0: zero angular velocity, normalize linear velocity, re-apply matrix
    Wf(v, 0x9c4, 0.0f); Wf(v, 0x9c0, 0.0f); Wf(v, 0x9bc, 0.0f);
    float tmp[3] = { Rf(v, 0x9b0), Rf(v, 0x9b4), Rf(v, 0x9b8) };
    Vec3Norm3(tmp, tmp);
    // [UNCERTAIN U-A6B-MTX]: original then FUN_004c4d20 with the normalized dir as axis;
    // exact matrix target/mode register-implicit — deferred to diff-original.
}

} // namespace Vehicle
} // namespace mashed_re
