// Mashed RE - RwMatrixRotate (axis-angle, degrees): builds/concats a rotation matrix.
//
// 0x004c4d20  FUN_004c4d20  WS-A2 (vehicle physics RW-math prereq)
//
// Verbatim from Ghidra (pool2, read-only, 2026-06-16). Body 0x004c4d20..0x004c4dba.
// Shape: RwMatrixRotate(RwMatrix* m, const RwV3d* axis, RwReal angle_degrees,
//                       RwOpCombineType mode).  29 callers across vehicle/camera/HUD/font.
//
// Decompiler:
//   angle_rad = angle_deg * π/180            (DAT_005cd7a8 = 0x3c8efa35)
//   invLen    = FastInvSqrt((axis[0]² + axis[1]²) + axis[2]²)   (FUN_004c3b90)
//   axis_n    = { invLen*axis[0], axis[1]*invLen, axis[2]*invLen }
//   s         = fsin(angle_rad)              (x87 FSIN)
//   c         = fcos(angle_rad)              (x87 FCOS)
//   FUN_004c4a50(m, axis_n, 1.0f - c, s, mode)        (1.0f = DAT_005cc320)
//   return m
//
// BIT-IDENTITY NOTE: the build uses MSVC x86 WITHOUT /arch:SSE2 -> x87 codegen, so the
// plain-float products keep extended-precision intermediates and round to f32 only on
// assignment, exactly matching the original FLD/FMUL/FSTP stream (same basis the existing
// Vec3Magnitude / FastInvSqrt leaves are bit-identical on). The ONE thing MSVC will NOT
// reproduce is the hardware transcendentals: its CRT sinf/cosf use argument-reduced
// polynomial code, not the raw FSIN/FCOS the original emits. So sin/cos/(1-cos) are done
// in an inline __asm block that mirrors asm 0x004c4d91..0x004c4da7 instruction-for-
// instruction (FLD; FSIN; FSTP / FLD; FCOS; FSUBR 1.0; FSTP).
//
// DELEGATION: the Rodrigues inner builder FUN_004c4a50 is now ported as the C++ symbol
// RwMatrixRotateInner (Math/RwMatrixRotateInner.cpp, also hooked at 0x004c4a50). This file
// calls that symbol directly (not the RVA), so RwMatrixRotate is real in BOTH the dev .asi
// and the standalone exe. In the .asi the inner is verified bit-identical (its own
// diff-original), so a diff of THIS function still isolates its preprocessing (deg->rad,
// axis normalize, sin/cos, 1-cos). NOTE: the inner's concat modes 1/2 dispatch the RW
// device matrix-mult, so those modes still need RW device init in the standalone (WS-E);
// mode 0 (a fresh rotation build) is fully self-contained.
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdio>     // D2 diagnostic
#include <cstdlib>    // D2 diagnostic

// Ported, bit-identical RW fast inverse-sqrt (Math/RwSqrt.cpp, 0x004c3b90).
extern "C" float __cdecl FastInvSqrt(float x);
// Ported Rodrigues inner builder (Math/RwMatrixRotateInner.cpp, 0x004c4a50). Calling the
// ported C++ symbol (not the original RVA) makes this work in the standalone exe too.
extern "C" float* __cdecl RwMatrixRotateInner(float* matrix, const float* axis_n,
                                              float one_minus_cos, float sin_a, int mode);

static constexpr std::uintptr_t kDegToRadAddr = 0x005cd7a8u;  // float π/180 = 0x3c8efa35
static constexpr std::uintptr_t kOneAddr      = 0x005cc320u;  // float 1.0f

// 0x004c4d20
extern "C" __declspec(dllexport)
void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode)
{
    // FIX 2026-08-21 (D2 root cause). These two constants used to be read from
    // the MASHED absolute addresses below. That is correct in the injected .asi,
    // where MASHED.exe's .rdata is mapped, but in the STANDALONE exe both
    // addresses read as **0** (measured: kDegToRad=0, kOne=0), which silently
    // turned every axis-angle rotation into a scale:
    //     angle_rad     = deg * 0 = 0
    //     s             = sin(0) = 0
    //     one_minus_cos = 0 - cos(0) = -1
    //     R = I + s*K + (1-c)*K^2 = I - K^2 = diag(2,1,2) about the up axis
    // which is exactly the observed steered-forward defect (a steered wheel got
    // 2x body-forward instead of a rotated forward, so no lateral force and the
    // car could not turn). Materialise the same bit patterns as literals — the
    // values are identical in both targets, so this is bit-identical in the .asi
    // and merely CORRECT in the exe. This is the D0.7 RVA-tunnel class of defect.
    static const std::uint32_t kDegToRadBits = 0x3c8efa35u;   // pi/180, was [0x005cd7a8]
    const float kDegToRad = *reinterpret_cast<const float*>(&kDegToRadBits);
    const float kOne      = 1.0f;                             // 0x3f800000, was [0x005cc320]
    (void)kDegToRadAddr; (void)kOneAddr;   // retained above for provenance

    // D2 DIAGNOSTIC 2026-08-21: both constants are read from MASHED ABSOLUTE
    // ADDRESSES, which are valid in the injected .asi but not in the standalone
    // exe. Predicted consequence if they read 0 there: angle_rad = 0, s = 0,
    // one_minus_cos = 0 - 1 = -1, so Rodrigues yields R = I - K^2 = diag(2,1,2)
    // about the up axis -- a uniform 2x scale in x/z instead of a rotation,
    // which is exactly the steered-forward defect (fwd0 = 2 * fwd3).
    {
        static const bool s_d = (std::getenv("MASHED_COUPLING_DIAG") != nullptr);
        static bool s_once = false;
        if (s_d && !s_once) {
            s_once = true;
            if (std::FILE* lf = std::fopen("coupling_diag.log", "a")) {
                std::fprintf(lf, "RWMATROT-CONSTS kDegToRad=%g (expect 0.0174533) "
                                 "kOne=%g (expect 1) @%p/%p\n",
                             kDegToRad, kOne,
                             (void*)kDegToRadAddr, (void*)kOneAddr);
                std::fclose(lf);
            }
        }
    }

    // angle_deg * (π/180), rounded to f32 (matches FLD; FMUL [0x5cd7a8]; FSTP).
    const float angle_rad = angle_deg * kDegToRad;

    // Normalize the axis (same sum-of-squares association as the original: (x²+y²)+z²).
    const float invLen = FastInvSqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
    float axis_n[3];
    axis_n[0] = invLen * axis[0];
    axis_n[1] = axis[1] * invLen;
    axis_n[2] = axis[2] * invLen;

    // Hardware FSIN / FCOS + (1 - cos), mirroring 0x004c4d91..0x004c4da7 exactly.
    float s             = 0.0f;
    float one_minus_cos = 0.0f;
    __asm {
        fld   dword ptr [angle_rad]
        fsin
        fstp  dword ptr [s]
        fld   dword ptr [angle_rad]
        fcos
        fsubr dword ptr [kOne]            // 1.0f - cos
        fstp  dword ptr [one_minus_cos]
    }

    RwMatrixRotateInner(static_cast<float*>(matrix), axis_n, one_minus_cos, s, mode);
    return matrix;
}

RH_ScopedInstall(RwMatrixRotate, 0x004c4d20);
