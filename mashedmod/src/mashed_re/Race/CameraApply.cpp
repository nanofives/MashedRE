// Mashed RE — see CameraApply.h. Exe only (exe_sources.rsp): the .asi runs the original's own
// Camera::Apply.
#include "CameraApply.h"
#include "RaceCamera.h"

#include <cstdint>
#include <cstring>

// Ported RW matrix primitives, the same symbols Race/CameraEntries.cpp uses in the exe.
extern "C" void* __cdecl RwMatrixRotate(void* m, const float* axis, float deg, int mode);  // 0x004c4d20
extern "C" void  __cdecl FUN_004c51a0(float* m, float* v, int mode);                      // 0x004c51a0

namespace mashed_re {
namespace Race {

namespace {
// DAT_006146fc = (0, 1, 0), the yaw axis Camera::Apply passes (00441760.md l.20).
const float kAxisY[3] = {0.f, 1.f, 0.f};
}  // namespace

// 0x00441760
void CameraApplyFrame(const float pos[3], float yaw_deg, float pitch_deg, float roll_deg,
                      float m[16]) {
    // FUN_004c15c0: identity, flags |= 0x20003 (c2gen/third-party-library_renderware/004c15c0.md).
    // The frame's earlier flag bits are not modelled; RwMatrixRotateInner reads only the identity
    // bit (0x20000) and the low type bits (& 3).
    std::memset(m, 0, 16 * sizeof(float));
    m[0] = 1.f; m[5] = 1.f; m[10] = 1.f;
    const std::uint32_t flags = 0x20003u;
    std::memcpy(&m[3], &flags, 4);
    RwMatrixRotate(m, kAxisY, yaw_deg, 2);       // FUN_004c4d20(frame+0x10, &DAT_006146fc, +0x38, 2)
    RwMatrixRotate(m, &m[0], pitch_deg, 2);      // FUN_004c4d20(frame+0x10, frame+0x10 (right), +0x34, 2)
    RwMatrixRotate(m, &m[8], roll_deg, 2);       // FUN_004c4d20(frame+0x10, frame+0x30 (at), +0x3c, 2)
    float t[3] = {pos[0], pos[1], pos[2]};
    FUN_004c51a0(m, t, 2);                       // FUN_004c1340 = RwMatrixTranslate on frame+0x10, postconcat
}

void ResolveRaceRenderCamera(const RaceCamera& rc, RaceRenderCamera* out) {
    float m[16];
    if (*reinterpret_cast<const std::int32_t*>(0x00897fe0u) != 0) {
        // 0x00448208 -> 0x00442a20: +0x58 = DAT_00896580 (entry 0 +0xc0), frame+0x10 = the 16
        // dwords at 0x0089650c (entry 0 +0x4c), overwriting what Apply wrote.
        std::memcpy(m, reinterpret_cast<const void*>(0x0089650cu), sizeof(m));
        out->view_window = *reinterpret_cast<const float*>(0x00896580u);
        out->src = kSrcEntry0;
    } else {
        CameraApplyFrame(rc.pos(), rc.yaw_deg(), rc.pitch_deg(), rc.roll_deg(), m);   // 0x004481c1
        out->view_window = rc.view_window();                                         // 0x0044800d
        out->src = kSrcApply;
    }
    for (int i = 0; i < 3; ++i) {
        out->right[i] = m[i];
        out->up[i]    = m[4 + i];
        out->at[i]    = m[8 + i];
        out->pos[i]   = m[12 + i];
    }
}

}  // namespace Race
}  // namespace mashed_re
