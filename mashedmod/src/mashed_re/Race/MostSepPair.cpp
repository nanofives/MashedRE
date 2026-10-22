// Mashed RE - most-separated car pair (original FUN_0040e180), the one body in both targets (dcmix 2026-10-22).
//
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
//
// Moved from Race/RaceCamera.cpp (exe) so the .asi runs the same body: Race/CameraClusterHooks.cpp's MostSepPair_0040e180
// fills RaceCamCar[4] from the original's getters and calls it. The only per-target input is the magnitude:
//   exe   Math/Vec3.cpp Vec3Magnitude (no LUT in the exe, so its exact-sqrt fallback), as before the move;
//   .asi  the original FUN_004c3ac0 by address, as Race/CameraClusterHooks.cpp called it before the move.
#include "RaceCamera.h"

#ifdef MASHED_STANDALONE
extern "C" float __cdecl Vec3Magnitude(const float* v);
#endif

namespace mashed_re {
namespace Race {

namespace {
inline float Vec3Mag(const float v[3]) {
#ifdef MASHED_STANDALONE
    return Vec3Magnitude(v);
#else
    return reinterpret_cast<float(__cdecl*)(const float*)>(0x004c3ac0)(v);
#endif
}
}  // namespace

// ---- 0x0040e180: most-separated pair (3D distance, active+alive+!dying) ---
void RaceCamera::MostSeparatedPair(const RaceCamCar cars[4], int* a, int* b) {
    int best_outer = -1, best_inner = -1;
    float best = 0.f;
    for (int i = 0; i < 4; ++i) {
        if (!cars[i].active || !cars[i].alive || cars[i].dead_flag) continue;
        for (int j = 0; j < 4; ++j) {
            if (!cars[j].active || !cars[j].alive || cars[j].dead_flag)
                continue;
            const float d[3] = {cars[i].pos[0] - cars[j].pos[0],
                                cars[i].pos[1] - cars[j].pos[1],
                                cars[i].pos[2] - cars[j].pos[2]};
            const float m = Vec3Mag(d);
            if (best <= m) {   // `<=` as in the original (later pair wins ties)
                best = m;
                best_outer = i;
                best_inner = j;
            }
        }
    }
    // tail fixups verbatim (0x0040e2f3..0x0040e330)
    if (best_inner == -1) best_inner = best_outer;
    if (best_outer == -1) best_outer = best_inner;
    if (best_inner == -1) best_inner = 0;
    if (best_outer == -1) best_outer = 0;
    // OUT-PARAM ORDER: first = INNER index, second = OUTER index. SOURCE-VERIFIED
    // against the PE (re/analysis/race_camera/FUN_0040e180.asm, dumped with
    // re/tools/pedisasm.py from MASHED.exe.unpatched at the SHA-256 anchor):
    //   0x0040e19e  xor ebx, ebx          ebx = OUTER counter
    //   0x0040e2c6  inc esi               esi = INNER counter (back edge 0x0040e2ca)
    //   0x0040e2db  inc ebx               outer back edge 0x0040e2e3
    //   on a new best:
    //   0x0040e2b9  mov ebp, esi          ebp  <- INNER
    //   0x0040e2bb  mov [esp+0x10], ebx   slot <- OUTER (-> edi at 0x0040e2d0)
    //   tail:
    //   0x0040e325  mov [eax], ebp        *out1 = INNER
    //   0x0040e327  mov [ecx], edi        *out2 = OUTER
    //
    // This was `*a = best_outer; *b = best_inner` and that is backwards. It is NOT
    // cosmetic: Update() leads only the A side (mid = (posA+posB)/2 + velA*k, the
    // sep leads having cancelled), so a swap moves the camera target by
    // k*(velA - velB) -- invisible while the cars are stationary, 0.22..0.45 at
    // the captured racing speeds.
    //
    // MEASURED 2026-08-26 over 766 frames of live original telemetry: before, the
    // port matched the original's stored pair (+0x994/+0x998) exactly 0.0% of the
    // time and matched it swapped 92.2%; after, 92.2% exact, and the moving-tail
    // pose residual went 0.231 -> 0.000. Two independent quantities, one change.
    //
    // The remaining 7.8% choose a genuinely DIFFERENT pair and are unexplained.
    // Leading suspect, untested: the offline driver derives `active` as
    // (alive != -1) instead of replicating IsCarSlotActive (0x0040e370), whose
    // table walk the original applies here at 0x0040e1b5 via PTR 0x005f2770.
    *a = best_inner;
    *b = best_outer;
}

}  // namespace Race
}  // namespace mashed_re
