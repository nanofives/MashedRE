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
// WHICH MATRIX `orient` IS — DECODED 2026-09-29 (U-9149). The old caveat here read
// "`orient==nullptr` guards the standalone-inert case until A8 binds the vehicle transform
// matrix". A8 landed and nothing ever bound it, so BOTH `if (orient)` legs below have been
// dead in mashed_re.exe for the whole of D2. The object is now known: the original loads ESI
// at 0x0047093b from A4's stack slot E+0x0c, which A4 filled at 0x004706a2 with
// LEA [EAX+EDI+0x928] (EAX = [EDI+0x9a8]<<6) — the SAME pointer it hands A5 as A5's second
// argument. The .asi forwarder is fixed accordingly (PhysicsChainHooks.cpp Call_A6b) and
// verified live: 144 samples, 64 of them airborne and bit-identical.
//
// THE EXE SIDE IS STILL nullptr, AND IT IS BLOCKED, NOT FORGOTTEN [UNCERTAIN U-9151].
// Binding g_bodyBasis[slot] here crashes mashed_re.exe with 0xC0000005 (3 runs of 3,
// 2026-09-29): both legs call RwMatrixRotate with mode 1 (rwCOMBINEPRECONCAT), and
// Math/RwMatrixRotateInner.cpp:169-176 resolves mode 1 through the RW DEVICE table at the
// absolute addresses 0x007d4028 / 0x007d3ff8, which are mapped in the injected .asi and not
// in the standalone. Only mode 0 is self-contained there. Unblocker: a standalone CPU port
// of RwMatrixMultiply 0x004c4600 (today only a function pointer into the original image,
// HUD/FontCtx.cpp:42, C1). Full write-up: re/analysis/D2_REOPEN_2026-09-29.md §4.6.
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "ForceIntegrator.h"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdio>    // [U-9151] MASHED_A6BTEST witness
#include <cstdlib>

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

    // [U-9151 2026-09-29] exe-copy witness, DIAG ONLY, default-OFF, writes nothing when
    // unset. MASHED_A6BTEST=<path> logs one line per call that gets past the airborne
    // gate: whether `orient` is bound at all, whether it is the SAME pointer A5 was
    // handed (the original's invariant — A4 computes the matrix once at 0x00470699 and
    // gives the identical pointer to A5 at 0x00470918 and to A6b at 0x0047093b), the
    // motion state that picks the branch, and the 16 matrix floats BEFORE and AFTER so
    // `ndiff` says the rotation actually landed. This is the exe analogue of the .asi's
    // `xf=%08x exp=%08x xfok=%d` self-test: `exp` cannot be the original's
    // record+[record+0x9a8]*0x40+0x928 here, because the port keeps the basis in
    // g_bodyBasis (U-9152), so the checkable invariant is A5-pointer equality instead.
    // That difference is the point of the CONFIDENCE.md copy clause: this is the exe
    // copy's OWN evidence, not the .asi's.
    static const char* s_a6bt = std::getenv("MASHED_A6BTEST");
    float before[16];
    if (s_a6bt && orient) std::memcpy(before, orient, sizeof(before));
    struct Witness {
        const char* path; int* self; float* orient; float* bef; bool on; int state;
        ~Witness() {
            if (!on) return;
            std::FILE* f = std::fopen(path, "a");
            if (!f) return;
            std::fprintf(f, "A6B slot_rec=%p orient=%p a5=%p xfok=%d state=%d bound=%d",
                         (void*)self, (void*)orient, (void*)g_a6bA5Matrix,
                         (orient && orient == g_a6bA5Matrix) ? 1 : 0,
                         state, orient ? 1 : 0);
            int nd = 0;
            if (orient) {
                for (int i = 0; i < 16; ++i) {
                    std::uint32_t a, b;
                    std::memcpy(&a, &bef[i], 4); std::memcpy(&b, &orient[i], 4);
                    if (a != b) ++nd;
                }
                std::fprintf(f, " ndiff=%d after=[", nd);
                for (int i = 0; i < 16; ++i) std::fprintf(f, "%s%.9g", i ? "," : "", orient[i]);
                std::fprintf(f, "]");
            }
            std::fprintf(f, "\n");
            std::fclose(f);
        }
    } w{ s_a6bt, self, orient, before, s_a6bt != nullptr, Ri(v, 0x9f0) };

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
