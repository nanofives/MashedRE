// Mashed RE — WS-A-DEVXFORM: plain C++ RwV3dTransformPoints (no RW device).
//
// FUN_004c3df0 (RwV3dTransformPoints) is a pass-through thunk to the RW *device*
// transform via the device table — it gives wrong results in the standalone without
// RW device init (WS-A-VERIFY finding). The vehicle physics (A5/A6a) only ever feed
// it ROTATION matrices built by FUN_004c4d20 (RwMatrixRotate, mode 0) with count=1,
// so a plain CPU 3x4 matrix*vec3 is the faithful standalone substitute.
//
// RwMatrix layout (FUN_004c4d20 output, confirmed vs FUN_00468980 asm: ESI+0x20 =
// at.x = m[8], ESI+0x4 = right.y = m[1]): right@m[0..2], flags@m[3], up@m[4..6],
// pad@m[7], at@m[8..10], pad@m[11], pos@m[12..14], pad@m[15].
//   out = in.x*right + in.y*up + in.z*at + pos
// Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
//
// ============================================================================
// D3-CONTACT 2026-09-27 — MEASURED: the header above names the WRONG device slot.
//
// Device slot +0x14 (dispatched by FUN_004c3df0) transforms VECTORS: it ignores
// the matrix translation row. Device slot +0xc (dispatched by FUN_004c3d90)
// transforms POINTS: it adds it. Both measured on the live engine through the
// existing diff lane (re/frida/run_diff.py, arg_type `device_transform_points`,
// 10 identical vectors, m[3] = 0 on both sides so the RwMatrix flags word cannot
// explain the split):
//
//   log/diff_rw_v3d_transform_points_cpu.csv       (this impl vs 0x004c3df0, slot +0x14)
//       RED 4/10 — and the 4 are EXACTLY the vectors whose matrix has a nonzero
//       pos row. _TRANS pos=(1,2,3), in=(1,1,1): original (1,1,1), this impl
//       (2,3,4). _MIXED pos=(11,12,13), in=(1,1,1): original (15,18,21), this
//       impl (26,30,34). The other 6 (pos = 0) are bit-identical.
//   log/diff_rw_device_dispatch_0c_points.csv     (this impl vs 0x004c3d90, slot +0xc)
//       9/10 bit-identical, the 10th off by 1 ULP on one component
//       (3227474774 vs 3227474773 on _ROTA) — a summation-order difference, not a
//       semantic one.
//
// CONSEQUENCE. This function is the correct CPU stand-in for slot +0xc, NOT for
// slot +0x14. `RwV3dTransformVectorsCPU` below is the stand-in for +0x14 and is
// what Collision/ContactStubs.cpp binds Rw_TransformPoints to. Vehicle/
// ForceIntegratorStubs.cpp:39 still binds the POINTS form to the same +0x14
// contract; that is a measured faithfulness defect, NOT changed here because the
// D2 physics gate was closed against the current behaviour. It is LATENT today —
// VehicleControl.cpp:186 passes a pure yaw world-ROTATION matrix as `xform`
// (VehicleControl.cpp:178-185), whose pos row is zero, and ForceIntegrator.cpp:70
// memsets its local `m` before RwMatrixRotate mode 0, which writes m[12..14] = 0
// (RwMatrixRotateInner.cpp:150). It becomes live the moment any caller passes a
// matrix with a real translation row. Tracked as an UNCERTAINTIES row for the D2
// re-measure; the stale comment at ForceIntegrator.cpp:55-56 ("Rw_TransformPoints
// is a POINT transform and consumes the translation row") is refuted by the above.
//
// This also resolves the long-standing U-1891 ("identity of the device method at
// slot +0x14"): it is the vectors method.
// ============================================================================
#include <cstdint>
#include <cstdlib>

namespace mashed_re {
namespace Math {

// [u9759 2026-10-18] MASHED_RW_SSE_POINTS=1 (default OFF, verify/u9759_20261018/PREREG_u9759.md): the live slot +0xc is
// NOT 0x004c3670 (the x87 default the installer 0x004c3ebf writes). Read in game (batch probe, o_bq3): the vector-module
// slots *(*(0x007d3ff8) + *(0x007d3ffc) + 8 / 0xc / 0x10 / 0x14) hold 0x004dc4a0 / 0x004dc370 / 0x004dc530 / 0x004dc410.
// 0x004dc370 is SSE: xmm0 = x*right + pos (0x004dc3d2 mulps, 0x004dc3de addps), xmm2 = z*at + y*up (0x004dc3d8,
// 0x004dc3e1), out = xmm2 + xmm0 (0x004dc3e7), i.e. per component (z*m[8+c] + y*m[4+c]) + (x*m[c] + m[12+c]). On car 1's
// spawn pass that order gives wheel 3 x 0xbf59093a = the original; the order below gives 0xbf590939. The vectors slot
// 0x004dc410 sums (x*right + y*up) + z*at (0x004dc472 / 0x004dc478), which RwV3dTransformVectorsCPU already does.
static bool RwSsePoints()
{
    static const bool s = [] { const char* e = std::getenv("MASHED_RW_SSE_POINTS"); return e && e[0] == '1'; }();
    return s;
}

// points are vec3 (12-byte stride). count points, dst[i] = M * src[i].
void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m)
{
    if (RwSsePoints()) {
        for (int i = 0; i < count; ++i) {
            const float* s = src + i * 3;
            float* d = dst + i * 3;
            const float x = s[0], y = s[1], z = s[2];
            for (int c = 0; c < 3; ++c) {
                const float hi = z * m[8 + c] + y * m[4 + c];   // xmm2: 0x004dc3d8 mulps, 0x004dc3e1 addps
                const float lo = x * m[c] + m[12 + c];          // xmm0: 0x004dc3d2 mulps, 0x004dc3de addps
                d[c] = hi + lo;                                 // 0x004dc3e7 addps xmm2, xmm0
            }
        }
        return;
    }
    for (int i = 0; i < count; ++i) {
        const float* s = src + i * 3;
        float* d = dst + i * 3;
        const float x = s[0], y = s[1], z = s[2];
        d[0] = x * m[0] + y * m[4] + z * m[8]  + m[12];
        d[1] = x * m[1] + y * m[5] + z * m[9]  + m[13];
        d[2] = x * m[2] + y * m[6] + z * m[10] + m[14];
    }
}

// D3-CONTACT 2026-09-27. The CPU stand-in for device slot +0x14 (the slot
// FUN_004c3df0 dispatches): same 3x3 product, translation row NOT added. Measured
// bit-identical to the original on all 10 vectors —
// log/diff_rw_v3d_transform_vectors_cpu.csv, hook `rw_v3d_transform_vectors_cpu`.
// points are vec3 (12-byte stride). count vectors, dst[i] = M3x3 * src[i].
void RwV3dTransformVectorsCPU(float* dst, const float* src, int count, const float* m)
{
    for (int i = 0; i < count; ++i) {
        const float* s = src + i * 3;
        float* d = dst + i * 3;
        const float x = s[0], y = s[1], z = s[2];
        d[0] = x * m[0] + y * m[4] + z * m[8];
        d[1] = x * m[1] + y * m[5] + z * m[9];
        d[2] = x * m[2] + y * m[6] + z * m[10];
    }
}

} // namespace Math
} // namespace mashed_re

// D3-CONTACT 2026-09-27: C-linkage export so the standalone substitute can be diffed
// against the ORIGINAL device method with the existing lane (re/frida/run_diff.py,
// hook `rw_v3d_transform_points_cpu`, arg_type `device_transform_points`). Without an
// export the .asi symbol is not resolvable by Module.getExportByName and the substitute
// could only be argued about, not measured. Signature mirrors 0x004c3df0's measured
// contract (dst, src, count, matrix) and its `MOV EAX,ESI` return of arg1 (0x004c3e17).
extern "C" __declspec(dllexport)
void* __cdecl RwV3dTransformPointsCPU_C(void* dst, const void* src, int count, const void* m)
{
    mashed_re::Math::RwV3dTransformPointsCPU(static_cast<float*>(dst),
                                             static_cast<const float*>(src),
                                             count,
                                             static_cast<const float*>(m));
    return dst;
}

// Same, for the vectors form (device slot +0x14, the one FUN_004c3df0 dispatches).
extern "C" __declspec(dllexport)
void* __cdecl RwV3dTransformVectorsCPU_C(void* dst, const void* src, int count, const void* m)
{
    mashed_re::Math::RwV3dTransformVectorsCPU(static_cast<float*>(dst),
                                              static_cast<const float*>(src),
                                              count,
                                              static_cast<const float*>(m));
    return dst;
}
