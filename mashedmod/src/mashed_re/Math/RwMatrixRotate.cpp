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
// DELEGATION: the Rodrigues inner builder FUN_004c4a50 (716 B, separate C2 render fn,
// re/analysis/render_4_c1_to_c2_s2/FUN_004c4a50.md) is called at its original RVA. In the
// dev .asi (injected into MASHED) that is the genuine original code, so the matrix output
// is identical by construction and a diff-original of THIS function isolates exactly its
// own preprocessing (deg->rad, axis normalize, sin/cos, 1-cos). For the standalone
// (mashed_re.exe) wiring (WS-A8 / WS-E render), FUN_004c4a50 must itself be ported — it is
// not yet, so RwMatrixRotate is dormant in the exe target (never called there yet).
#include "../Core/HookSystem.h"

#include <cstdint>

// Ported, bit-identical RW fast inverse-sqrt (Math/RwSqrt.cpp, 0x004c3b90).
extern "C" float __cdecl FastInvSqrt(float x);

static constexpr std::uintptr_t kDegToRadAddr = 0x005cd7a8u;  // float π/180 = 0x3c8efa35
static constexpr std::uintptr_t kOneAddr      = 0x005cc320u;  // float 1.0f
static constexpr std::uintptr_t kFUN_004c4a50 = 0x004c4a50u;  // Rodrigues inner builder

typedef void* (__cdecl *RwMatrixRotateInner)(void* matrix, const float* axis,
                                             float one_minus_cos, float sin_a, int mode);

// 0x004c4d20
extern "C" __declspec(dllexport)
void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode)
{
    const float kDegToRad = *reinterpret_cast<const float*>(kDegToRadAddr);
    const float kOne      = *reinterpret_cast<const float*>(kOneAddr);

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

    reinterpret_cast<RwMatrixRotateInner>(kFUN_004c4a50)(matrix, axis_n, one_minus_cos, s, mode);
    return matrix;
}

RH_ScopedInstall(RwMatrixRotate, 0x004c4d20);
