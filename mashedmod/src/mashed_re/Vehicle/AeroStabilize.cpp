// Mashed RE — WS-A6b: aerodynamic stabilization FUN_00468980.
//
// [dcphys2 2026-10-22] ONE body, shared by mashed_re.exe and mashed_re_dev.asi (this TU is in both rsp files). The
// .asi's naked verbatim copy (PhysicsChainHooks.cpp A6b_BodyAsm) was deleted once this body read against the
// original in that file's in-process A/B (verify/dcphys2_20261022). Only the leaf thunks below differ by target.
//
// Listing 0x00468980..0x00468b34 (MASHED.exe.unpatched, re/tools/disasm_fn.py 2026-10-22):
//   self = ECX; orient = ESI; dt = the one stack arg. Airborne only: FCOMP +0x9e0 with 0.0 (0x00468989), JP out.
//   motion state +0x9f0 == 0 (0x004689a4):
//     dts = dt * _DAT_005cc558 (0.001f), FSTP float                                      0x004689aa..0x004689b4
//     c1  = (at.y*Y.y + at.x*Y.x) + at.z*Y.z, Y = _DAT_006146fc (0,1,0), clamp [-1,1]   0x004689b8..0x004689fd
//     ang1 = -(acos(c1) * _DAT_005ccae0 - _DAT_005ccad8) * dts   (double 180/pi, double 90, FCHS)  0x00468a09..0x00468a22
//     FUN_004c4d20(orient, _DAT_006146f0 (1,0,0), ang1, 1)                              0x00468a31
//     c2  = (right.y*Y.y + Y.x*right.x) + right.z*Y.z, clamp                            0x00468a36..0x00468a7d
//     ang2 = (acos(c2) * _DAT_005ccae0 - _DAT_005ccad0) * dts    (float 90, no FCHS)    0x00468a89..0x00468aa0
//     FUN_004c4d20(orient, _DAT_00614708 (0,0,1), ang2, 1)                              0x00468aaf
//   motion state != 0: +0x9c4 / +0x9c0 / +0x9bc = 0 (0x00468abb..0x00468ac7); dir = +0x9b0..+0x9b8 copied,
//     FUN_004c39b0(dir, dir) (LUT normalise, 0x00468af1); FUN_004c4d20(orient, dir, dt * _DAT_005cc9a0 (0.05f), 1)
//     (0x00468b29).
// acos = FUN_004a3384 (CRT). Under the race loop's PC_24 its result leaves rounded to double (Ai/AiLeafTargets.cpp
// AL_Acos header), and every FMUL / FSUB of the angle chain rounds to 24 bits, once per operation, before the single
// FSTP float. orient = A4's +0x928 matrix (record + [+0x9a8]*0x40 + 0x928, 0x00470699 / 0x0047093b, U-9149); the
// standalone passes its own body basis there (U-9152), an input of this body, not part of it.
//
// [dcphys2] MASHED_A6B_ACOS_CRT (default OFF in the exe): the angle chain above as the original runs it (CRT acos,
// one PC_24 rounding per operation). OFF keeps the exe's former form: std::acos and the chain in double, rounded to
// float once. MASHED_A6B_NORM_LUT (default OFF in the exe): the state != 0 normalise through the RW LUT (FUN_004c39b0,
// Math::LutNormalize) instead of the exact Vec3Norm3. The .asi seeds both ON (PhysicsChainHooks.cpp A6bAsiDefaults),
// which is what its deleted verbatim copy computed.
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
#include "ForceIntegrator.h"
#include "../Math/RwLutTables.h"
#include <cstdint>
#include <cstring>
#include <cmath>
#include <cstdio>    // [U-9151] MASHED_A6BTEST witness
#include <cstdlib>
#include <float.h>   // _controlfp_s (MASHED_A6B_ACOS_CRT PC_24 scope)

#ifdef MASHED_STANDALONE
extern "C" void AL_Acos();   // Ai/AiLeafTargets.cpp: CRT acos 0x004a3384, naked, result in ST0 (exe only)
#endif

namespace mashed_re {
namespace Vehicle {

extern "C" void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode);

// ── target thunks: the leaves with no both-target port ───────────────────────
#ifdef MASHED_STANDALONE
static inline void* A6bAcosFn() { return reinterpret_cast<void*>(&AL_Acos); }
static inline void  A6bRotate(float* m, const float* axis, float angle) { RwMatrixRotate(m, axis, angle, 1); }
#else
static inline void* A6bAcosFn() { return reinterpret_cast<void*>(0x004a3384u); }                       // FUN_004a3384
static inline void  A6bRotate(float* m, const float* axis, float angle) {                               // FUN_004c4d20
    reinterpret_cast<void (__cdecl*)(void*, const float*, float, int)>(0x004c4d20u)(m, axis, angle, 1);
}
#endif

static inline float Rf(void* b, int off) { float v; std::memcpy(&v, (char*)b + off, 4); return v; }
static inline void  Wf(void* b, int off, float v) { std::memcpy((char*)b + off, &v, 4); }
static inline int   Ri(void* b, int off) { int v; std::memcpy(&v, (char*)b + off, 4); return v; }

// exact-bit constructors (memory_read pool6 2026-06-17; the decimal literals for
// 180/pi mis-round). Cf=float, Cd=double from the .rdata qword.
static inline float  Cf(std::uint32_t bits) { float  f; std::memcpy(&f, &bits, 4); return f; }
static inline double Cd(std::uint64_t bits) { double d; std::memcpy(&d, &bits, 8); return d; }

namespace a6b {
constexpr float kDtScale = 0.001f;   // _DAT_005cc558 = 0x3a83126f
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
constexpr float kVelAng  = 0.05f;    // _DAT_005cc9a0 = 0x3d4ccccd
// axis vectors (image bits 2026-10-22): X=(1,0,0) @0x006146f0, Y=(0,1,0) @0x006146fc, Z=(0,0,1) @0x00614708
const float kAxisX[3] = { 1.0f, 0.0f, 0.0f };   // _DAT_006146f0
const float kAxisY[3] = { 0.0f, 1.0f, 0.0f };   // _DAT_006146fc
const float kAxisZ[3] = { 0.0f, 0.0f, 1.0f };   // _DAT_00614708
} // namespace a6b

static inline float clamp11(float d) {
    using namespace a6b;
    if (d < kNegOne) return kNegOne;
    return (kOne < d) ? kOne : d;
}

static bool A6bKnob(const char* name) { const char* e = std::getenv(name); return e && e[0] && e[0] != '0'; }
static bool A6bAcosCrt() { static const bool s = A6bKnob("MASHED_A6B_ACOS_CRT"); return s; }
static bool A6bNormLut() { static const bool s = A6bKnob("MASHED_A6B_NORM_LUT"); return s; }

// [dcphys2] the angle chain as the original runs it: acos (FUN_004a3384, result left in ST0) then FMUL qword
// 180/pi, FSUB 90 (qword for pitch, dword for roll), FCHS (pitch only), FMUL dword dts, FSTP dword, all under PC_24
// (0x00468a09..0x00468a22 / 0x00468a89..0x00468aa0). The .asi runs inside the race thread's PC_24 already; the
// scope makes the standalone match it.
__declspec(noinline) static float A6bAngleCrt(float c, float dts, bool pitch) {   // noinline: the __asm CALL is invisible to the register allocator
    unsigned int oldcw = 0, t = 0;
    _controlfp_s(&oldcw, 0, 0);
    _controlfp_s(&t, _PC_24, _MCW_PC);
    double cc = c;
    void* fn = A6bAcosFn();
    const double r2d = a6b::kRad2Deg, b90d = a6b::kBias90d;
    const float  b90f = a6b::kBias90f;
    float d = dts, res;
    if (pitch) {
        __asm {
            sub   esp, 8
            fld   qword ptr cc
            fstp  qword ptr [esp]          // 0x00468a03..0x00468a06
            call  dword ptr fn             // 0x00468a09
            add   esp, 8
            fmul  qword ptr r2d            // 0x00468a0e
            fsub  qword ptr b90d           // 0x00468a16
            fchs                           // 0x00468a1c
            fmul  dword ptr d              // 0x00468a1e
            fstp  dword ptr res            // 0x00468a22
        }
    } else {
        __asm {
            sub   esp, 8
            fld   qword ptr cc
            fstp  qword ptr [esp]          // 0x00468a83..0x00468a86
            call  dword ptr fn             // 0x00468a89
            add   esp, 8
            fmul  qword ptr r2d            // 0x00468a8e
            fsub  dword ptr b90f           // 0x00468a96
            fmul  dword ptr d              // 0x00468a9c
            fstp  dword ptr res            // 0x00468aa0
        }
    }
    _controlfp_s(&t, oldcw, _MCW_PC);
    return res;
}

// 0x00468980 — Vehicle_AeroStabilize(self, orient, dt). orient = the +0x928 RwMatrix (16 floats).
void Vehicle_AeroStabilize(int* self, float* orient, float dt)
{
    using namespace a6b;
    void* v = self;
    if (Rf(v, 0x9e0) != 0.0f) return;                 // grounded -> not airborne (0x00468989 / 0x00468994)

#ifdef MASHED_STANDALONE
    // [U-9151 2026-09-29] exe-copy witness, DIAG ONLY, default-OFF, writes nothing when
    // unset. MASHED_A6BTEST=<path> logs one line per call that gets past the airborne
    // gate: whether `orient` is bound at all, whether it is the SAME pointer A5 was
    // handed (the original's invariant — A4 computes the matrix once at 0x00470699 and
    // gives the identical pointer to A5 at 0x00470918 and to A6b at 0x0047093b), the
    // motion state that picks the branch, and the 16 matrix floats BEFORE and AFTER so
    // `ndiff` says the rotation actually landed. Exe only (g_a6bA5Matrix lives in the
    // exe-only ForceIntegratorStubs.cpp); the .asi's A/B is its own witness.
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
#endif

    if (Ri(v, 0x9f0) == 0) {                           // motion state 0: auto-level (0x004689a4)
        const float dts = dt * kDtScale;               // 0x004689aa..0x004689b4
        if (orient) {
            if (A6bAcosCrt()) {
                // the dots as the original adds them (0x004689b8..0x004689ca, 0x00468a36..0x00468a4a)
                const float c1 = clamp11((orient[9] * kAxisY[1] + orient[8] * kAxisY[0]) + orient[10] * kAxisY[2]);
                A6bRotate(orient, kAxisX, A6bAngleCrt(c1, dts, true));                 // 0x00468a31
                const float c2 = clamp11((orient[1] * kAxisY[1] + kAxisY[0] * orient[0]) + orient[2] * kAxisY[2]);
                A6bRotate(orient, kAxisZ, A6bAngleCrt(c2, dts, false));                // 0x00468aaf
            } else {
                // pitch: -(acos(c1)*180/pi - 90.0_double) * dts   (FCHS @0x00468a1c)
                const float c1 = clamp11(orient[9]);       // at.y  (pitch)
                const float ang1 = (float)(-((double)std::acos((double)c1) * kRad2Deg - kBias90d) * (double)dts);
                A6bRotate(orient, kAxisX, ang1);
                // roll: (acos(c2)*180/pi - 90.0_float) * dts      (no FCHS)
                const float c2 = clamp11(orient[1]);       // right.y (roll)
                const float ang2 = (float)(((double)std::acos((double)c2) * kRad2Deg - (double)kBias90f) * (double)dts);
                A6bRotate(orient, kAxisZ, ang2);
            }
        }
        return;
    }

    // motion state != 0: zero angular velocity, align orientation to velocity direction
    Wf(v, 0x9c4, 0.0f); Wf(v, 0x9c0, 0.0f); Wf(v, 0x9bc, 0.0f);   // 0x00468abb..0x00468ac7
    float dir[3] = { Rf(v, 0x9b0), Rf(v, 0x9b4), Rf(v, 0x9b8) };
    if (A6bNormLut()) mashed_re::Math::LutNormalize(dir, dir);       // FUN_004c39b0 (0x00468af1)
    else              Vec3Norm3(dir, dir);
    if (orient) A6bRotate(orient, dir, dt * kVelAng);               // 0x00468b29
}

} // namespace Vehicle
} // namespace mashed_re
