// Mashed RE — the race camera's RW frame build, as the renderer consumes it (D1-residue R2).
//
//   0x00441760  Camera::Apply      FUN_004c15c0 identity, FUN_004c4d20 x3 (yaw about world Y,
//                                  pitch about the frame's own right, roll about its own at),
//                                  FUN_004c1340 translate. SetupFOV 0x00441700 is published as
//                                  the view window, not applied here.
//   0x004481c1..0x00448208         the director tail: Apply always, then 0x00442a20 (copy entry
//                                  0's matrix 0x0089650c + view window 0x00896580) only when the
//                                  phase-3 hold flag cam[0] = DAT_00897fe0 is set.
// Evidence: verify/d1r2_20261016/PREREG_d1r2.md section 1.
#pragma once

namespace mashed_re {
namespace Race {

class RaceCamera;

struct RaceRenderCamera {
    float pos[3];
    float right[3];
    float up[3];
    float at[3];
    float view_window;   // cam+0x58 after the tail: SetupFOV's input
    int   src;           // kSrcApply or kSrcEntry0
};
constexpr int kSrcApply  = 1;   // Camera::Apply from the director's angles (0x004481c1)
constexpr int kSrcEntry0 = 2;   // 0x00442a20, hold flag set (phase 3)

// 0x00441760's frame build. out16 = RwMatrix layout (right, flags, up, pad, at, pad, pos, pad).
void CameraApplyFrame(const float pos[3], float yaw_deg, float pitch_deg, float roll_deg,
                      float out16[16]);

// The director tail as the renderer sees it. Reads DAT_00897fe0 / 0x0089650c / 0x00896580, which
// the exe maps (zero-filled; only the MASHED_SUBSTATE chain writes them).
void ResolveRaceRenderCamera(const RaceCamera& rc, RaceRenderCamera* out);

}  // namespace Race
}  // namespace mashed_re
