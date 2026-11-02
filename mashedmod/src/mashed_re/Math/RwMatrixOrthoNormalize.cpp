// Mashed RE - the RwMatrix re-orthonormalize 0x004c4680, the one body in the exe (D4-DC-MATH-4680, W0-8).
//
// Callers: Track/WarzonePontoon.cpp OrthoNormalize (FUN_00451a50's ortho and the replay's rest check) and, under
// MASHED_ORTHO_LUT=1 (u9765; the standalone default since faithflip 2026-10-20, Core/FaithDefaults.cpp),
// Vehicle/BodyOrientationIntegrate.cpp (FUN_0046e9e0's call at 0x0046ef5e).
// The two transcriptions this replaces (WarzonePontoon.cpp OrthoNormalize, BodyOrientationIntegrate.cpp
// MatrixOrthoNormalizeRw) were the same listing (verify/pontoon_20261016/disasm_ortho.txt) with the same operation order.
//
// SSE2 TU (not in x87_tus.txt). The pontoon copy was in an x87 TU, called under Pc24 (WarzonePontoon.cpp Pontoon_Tick and
// RunReplay); at PC_24 each x87 multiply / add rounds to a 24-bit significand, as SSE single does. The pontoon replay
// (MASHED_PONTOON_REPLAY on OP3) is the check that the move kept its result: verify/dcmath4680_20261025/RESULT_dcmath4680.md.
//
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

#include "RwMatrixOrthoNormalize.h"
#include "RwLutTables.h"

#include <cstdint>
#include <cstring>

namespace mashed_re {
namespace Math {

namespace {
inline std::uint32_t& Flags(float* m) { return *reinterpret_cast<std::uint32_t*>(m + 3); }
}  // namespace

// 0x004c4680 (thunk_FUN_004c4680): 0x004c4680..0x004c4a41 as disassembled. Each row is scaled by the LUT inverse sqrt (FUN_004c3b90,
// Math::LutInvSqrt) of (y*y + x*x) + z*z, NOT 1/sqrt, so the rows are not exactly unit (car 1's up.y reaches 1.00000024 at call 4
// in o_w93c, u9765). The pair is selected by the abs dots with the original's comparison order.
void RwMatrixOrthoNormalizeLut(float* dst, const float* src) {
    const float k0 = 0.0f;                                                     // [0x005d757c]
    float r[3] = { src[0], src[1], src[2] }, u[3] = { src[4], src[5], src[6] }, a[3] = { src[8], src[9], src[10] };
    const float px = src[12], py = src[13], pz = src[14];
    const float ir = LutInvSqrt((r[1] * r[1] + r[0] * r[0]) + r[2] * r[2]);    // 0x004c469d..0x004c4702
    r[0] = ir * r[0]; r[1] = ir * r[1]; r[2] = ir * r[2];
    const float iu = LutInvSqrt((u[1] * u[1] + u[0] * u[0]) + u[2] * u[2]);    // 0x004c472b..0x004c474a
    u[0] = iu * u[0]; u[1] = iu * u[1]; u[2] = iu * u[2];
    const float ia = LutInvSqrt((a[1] * a[1] + a[0] * a[0]) + a[2] * a[2]);    // 0x004c4773..0x004c4792
    a[0] = a[0] * ia; a[1] = a[1] * ia; a[2] = a[2] * ia;
    float *p4, *p5, *p6;                                                       // ebx, esi, edi
    if (!(ir > k0))      { p4 = u; p5 = a; p6 = r; }                           // 0x004c47c9 -> 0x004c48ed/ef
    else if (!(iu > k0)) { p4 = a; p5 = r; p6 = u; }                           // 0x004c47e0 -> 0x004c48dd
    else if (!(ia > k0)) { p4 = r; p5 = u; p6 = a; }                           // 0x004c47f3 -> 0x004c48b4
    else {
        float dau = (a[1] * u[1] + a[0] * u[0]) + a[2] * u[2];               // 0x004c47f9..0x004c4815
        if (dau < k0) dau = -dau;
        float dar = (a[1] * r[1] + a[0] * r[0]) + a[2] * r[2];               // 0x004c4832..0x004c484e
        if (dar < k0) dar = -dar;
        float dur = (u[1] * r[1] + u[0] * r[0]) + u[2] * r[2];               // 0x004c486b..0x004c4887
        if (dur < k0) dur = -dur;
        if (dau < dar) {                                                       // 0x004c4898 fcomp, jp when >=
            if (dau < dur) { p4 = u; p5 = a; p6 = r; }                         // 0x004c48b2 jnp -> 0x004c48ef
            else           { p4 = r; p5 = u; p6 = a; }                         // 0x004c48b4
        } else {
            if (dar < dur) { p4 = a; p5 = r; p6 = u; }                         // 0x004c48cf
            else           { p4 = r; p5 = u; p6 = a; }                         // 0x004c48cd jp -> 0x004c48b4
        }
    }
    p6[0] = p5[2] * p4[1] - p5[1] * p4[2];                                     // 0x004c48fb..0x004c490a
    p6[1] = p4[2] * p5[0] - p5[2] * p4[0];                                     // 0x004c490c..0x004c4918
    p6[2] = p5[1] * p4[0] - p4[1] * p5[0];                                     // 0x004c491b..0x004c4927
    const float i6 = LutInvSqrt((p6[0] * p6[0] + p6[1] * p6[1]) + p6[2] * p6[2]);   // 0x004c492a..0x004c4948
    p6[0] = i6 * p6[0]; p6[1] = p6[1] * i6; p6[2] = p6[2] * i6;               // 0x004c494d..0x004c4960
    p5[0] = p4[2] * p6[1] - p4[1] * p6[2];                                     // 0x004c4963..0x004c496f
    p5[1] = p6[2] * p4[0] - p4[2] * p6[0];                                     // 0x004c4977..0x004c4983
    p5[2] = p4[1] * p6[0] - p6[1] * p4[0];                                     // 0x004c4986..0x004c4992
    const float i5 = LutInvSqrt((p5[0] * p5[0] + p5[1] * p5[1]) + p5[2] * p5[2]);   // 0x004c4995..0x004c49b3
    p5[0] = i5 * p5[0]; p5[1] = p5[1] * i5; p5[2] = p5[2] * i5;               // 0x004c49b8..0x004c49d5
    dst[0] = r[0]; dst[1] = r[1]; dst[2] = r[2];                               // 0x004c49dd..0x004c4a1c copy-out
    dst[4] = u[0]; dst[5] = u[1]; dst[6] = u[2];
    dst[8] = a[0]; dst[9] = a[1]; dst[10] = a[2];
    dst[12] = px; dst[13] = py; dst[14] = pz;
    Flags(dst) = (Flags(dst) & 0xfffdffffu) | 3u;                              // 0x004c4a35..0x004c4a41
}

}  // namespace Math
}  // namespace mashed_re
