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
#include <cstdint>

namespace mashed_re {
namespace Math {

// points are vec3 (12-byte stride). count points, dst[i] = M * src[i].
void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m)
{
    for (int i = 0; i < count; ++i) {
        const float* s = src + i * 3;
        float* d = dst + i * 3;
        const float x = s[0], y = s[1], z = s[2];
        d[0] = x * m[0] + y * m[4] + z * m[8]  + m[12];
        d[1] = x * m[1] + y * m[5] + z * m[9]  + m[13];
        d[2] = x * m[2] + y * m[6] + z * m[10] + m[14];
    }
}

} // namespace Math
} // namespace mashed_re
