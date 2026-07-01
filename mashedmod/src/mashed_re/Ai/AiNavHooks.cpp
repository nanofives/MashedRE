// Mashed RE — WS-R6-B: AI spline lookahead target finder (FUN_00443dc0) port.
//
// Target FUN_00443dc0 (0x00443dc0..0x00444b5b, ~3.5 KB / 994 instr), __cdecl 6 args:
//   void NavLookahead(int spline, float* vehXZ, float* outXZ, int aiCarIdx,
//                     int useCache, int debugRender)
// The AI's nav lookahead: finds the nearest spline point to the vehicle, refines it
// with a 16-step binary subdivision, forward-walks 16 samples scoring each by
// direction dot, picks the best, then wall-checks the ray to it (backing the target
// index off on a wall hit). Consumes FUN_00443300 (Catmull-Rom eval, WS-R6-A C3).
//
// GREENFIELD C++ reimpl (measure-first, user-chosen 2026-07-01). .asi-only
// (asi_sources.rsp). This 994-instr function's Ghidra decomp is LOSSY in the
// wall-check ray-march (drops round-to-nearest __ftol, SAR tile indexing, -0x10
// offset) and keeps the ray-march marching param + (1/len) in 80-bit x87 ST across
// the loop. This C `float` reimpl reproduces the DISCRETE logic exactly
// (rounding/shifts/selection) but not the 80-bit marching chain, so it is
// BEHAVIORALLY FAITHFUL, not necessarily bit-identical — expected mostly-GREEN with
// rare tile-boundary divergences in the wall-check (gameplay-inert). C3 needs no
// bit-identity (that is C4). Full asm decode:
// re/analysis/R6_FUN_00443dc0_SCOPE_20260701.md (Mashed_pool0 RO, 2026-07-01).
//
// Stack-adjacent float2 vectors: the callees (Spline/Vec2Norm write 2 floats through
// one pointer) require the decomp's paired locals (local_16c/168 etc.) to be
// CONTIGUOUS — ported as float[2] arrays ([0]=X, [1]=Z), per the AiTargeting.cpp
// precedent, not separate scalars.
//
// The debug-render path (debugRender != 0 → FUN_004671a0/FUN_004b55a0) is NOT ported
// (STUB): never executes on the canonical AI path (AiSplineTargetInit passes
// debugRender=0) or in the standalone. Carried; blocks C4 only, not C3.
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

namespace {

inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
const float kZero   = Cf(0x00000000);  // DAT_005d757c 0.0
const float kOne    = Cf(0x3f800000);  // _DAT_005cc320 1.0
const float k0_01   = Cf(0x3c23d70a);  // subdivision seed t (first eval)
const float k0_99   = Cf(0x3f7d70a4);  // subdivision seed t (second eval)
const float k0_25   = Cf(0x3e800000);  // _DAT_005cc564 ray-march step
const float k0_05   = Cf(0x3d4ccccd);  // _DAT_005cc9a0 forward-walk step
const float k0_333  = Cf(0x3eaaaa9f);  // _DAT_005ce034 subdivision bisection weight
const float k4_0    = Cf(0x40800000);  // _DAT_005cc35c tile coord scale
const float k100000 = Cf(0x47c35000);  // 100000.0 nearest-dist init
const float kHalf   = Cf(0x3f000000);  // 0.5 round-half bias

const std::uintptr_t kTileGrid     = 0x007f1a9cu;   // 128x128 shorts
const std::uintptr_t kSubCellGrid  = 0x007f9a9cu;   // 8x8 sub-cell chars/tile
const std::uintptr_t kNearestCache = 0x008032d4u;   // per-car int cache, stride 5 ints
const std::uintptr_t kArmFlagBase  = 0x0089a500u;   // FUN_00416230 slot, stride 0x74

// ── live callee forwards (all present, >=C3; forwarded so mine and orig call the
// identical originals; with MASHED_HOOK_ONLY=0x00443dc0 no other hook installs) ──
// CRITICAL x87-ABI note: FUN_004c3c60 (Vec2Normalize) and FUN_004c3bf0 (Vec2Length)
// each RETURN A float10 IN ST0 (verified in Ghidra: both signatures return float10).
// On x86 the caller must pop ST0 after such a call — even when the return is unused.
// If the fn-ptr is declared `void`, the SSE2-built caller never emits the fstp, so
// ST0 leaks one slot per call; Vec2Norm runs 2x/forward-walk-iteration, overflowing
// the 8-deep x87 stack after ~2 iterations -> every later x87 result is the indefinite
// NaN 0xffc00000 (measured: walk candidates 2..15 had NaN x + NaN score, so the argmax
// could never pick the far lookahead -> systematic wrong target; and the unbalanced
// stack on RETURN corrupted the caller's FPU -> the race froze). Declaring the fn-ptrs
// as returning `float` makes the compiler pop ST0 (as float32) after each call.
typedef void  (__cdecl* fn_spline_t)(int, int, float, float*);   // FUN_00443300 (WS-R6-A) — void, no ST0
typedef void  (__cdecl* fn_arm_t)   (int, int);                  // FUN_00416230 Table89a500Set — void
typedef float (__cdecl* fn_len_t)   (float*);                    // FUN_004c3bf0 Vec2Length -> float10 in ST0
typedef float (__cdecl* fn_norm_t)  (float*, float*);            // FUN_004c3c60 Vec2Normalize -> float10 in ST0
inline void  Spline(int b, int i, float t, float* o) { reinterpret_cast<fn_spline_t>(0x00443300)(b, i, t, o); }
inline void  Arm   (int v, int f)                    { reinterpret_cast<fn_arm_t>(0x00416230)(v, f); }
inline float Vec2Len(float* v)                       { return reinterpret_cast<fn_len_t>(0x004c3bf0)(v); }
// return the (unused) float10-as-float so the compiler emits the ST0 pop at the call site
inline float Vec2Norm(float* d, float* s)            { return reinterpret_cast<fn_norm_t>(0x004c3c60)(d, s); }

// ── x87 single-rounding emulation (WS-R6-B fix, 2026-07-01) ─────────────────
// The original FUN_00443dc0 is x87: it computes a*a+b*b (and the dot) keeping the
// products/sum in one 80-bit ST register and rounds to float32 ONCE, at the store.
// This .asi compiles SSE2 (build.bat has no /arch flag; MSVC x86 default = SSE2),
// which rounds each product to float32 BEFORE adding — a double-rounding that
// differs from x87 by ~2^-24. In the chaotically-sensitive golden-section search
// (and the seed iVar10 flip / dot-score argmax) that flips `<` comparisons on
// nearly every call (measurement #1: 84/85 large diffs; the robust nearest-scan
// argmin was the only match). Computing these expressions in `double` and rounding
// once at the float32 store reproduces x87's single-rounding to ~2^-53 (vs x87's
// 2^-64) — the residual flips are vanishingly rare (the "rare boundary divergence"
// the scope predicts for C3). The two hand-written x87 leaves in the chain
// (FUN_00443300 spline, FUN_004c3bf0/004c3c60 Vec2) are forwarded verbatim, so
// only THIS function's inline arithmetic needed the emulation. Operand order
// matches the decomp exactly. Full analysis: R6_FUN_00443dc0_SCOPE_20260701.md.
inline double D2(float dx, float dz) {                        // a*a + b*b  (dist^2)
    return static_cast<double>(dx) * dx + static_cast<double>(dz) * dz;
}
inline double Dot2(float ax, float az, float bx, float bz) {  // a.x*b.x + a.z*b.z
    return static_cast<double>(ax) * bx + static_cast<double>(az) * bz;
}

// Wall-check tile sample (asm-decoded; both ray-march branches share this tail).
// round-half-away-from-zero of coord*4, then -0x10; SAR tile index; sub-cell wall 0/3.
// X = row (*0x80), Z = col — matches AiLineOfSight (0x00416060) grid roles.
inline bool WallAt(float sampleX, float sampleZ) {
    float rx = sampleX * k4_0;
    float rz = sampleZ * k4_0;
    int ix = static_cast<int>(rx >= kZero ? rx + kHalf : rx - kHalf) - 0x10;   // row (X)
    int iz = static_cast<int>(rz >= kZero ? rz + kHalf : rz - kHalf) - 0x10;   // col (Z)
    int tileIdx = (((ix + 0x200) >> 3) << 7) + ((iz + 0x200) >> 3);            // SAR (signed >>)
    short tile = *reinterpret_cast<short*>(kTileGrid + static_cast<std::uintptr_t>(tileIdx) * 2);
    if (tile > 0 && tile < 0x200) {
        char sub = *reinterpret_cast<char*>(
            kSubCellGrid + static_cast<std::uintptr_t>(((iz & 7) + static_cast<int>(tile) * 8) * 8 + (ix & 7)));
        if (sub == 0 || sub == 3) return true;
    }
    return false;
}

// ===========================================================================
// NavLookahead — greenfield reimpl of FUN_00443dc0. Goto structure mirrors the
// decomp; paired locals are float[2] ([0]=X,[1]=Z).
// ===========================================================================
void NavLookahead(int param_1, float* param_2, float* param_3,
                  int param_4, int param_5, int param_6)
{
    float local_140[64];   // dist^2 scratch, then selected sample points (pairs)
    float local_40[16];    // per-sample direction-dot score
    float pt16c[2];        // local_16c/168  spline out; next-point in fwd-walk; dz_norm(A)
    float pt174[2];        // local_174/170  spline out (seed); dx/dz in retry
    float pt148[2];        // local_148/144  spline out; dx_norm in branch A
    float pt188[2];        // local_188/184  spline out; cur-point / sampleXZ
    float pt150[2];        // local_150/14c  dir in fwd-walk; start point in retry
    float local_18c, local_180, local_164f, local_194f, local_160, local_15c, fVar9;
    float local_190, local_1a0, local_19c;
    float local_158 = 0.0f, local_154 = 0.0f;
    int iVar4, iVar6, iVar8, iVar10, count;
    bool bVar3;
    int local_194i = 0;   // "done" flag (decomp local_194 used as float !=0 test)
    int local_164i = 0;   // "wall hit" flag (decomp local_164 used as float !=0 test)

    if (param_6 == 1) {   // debug screen extents (unused on canonical path)
        local_158 = *reinterpret_cast<float*>(0x007f1a58) + k4_0;
        local_154 = *reinterpret_cast<float*>(0x007f1a5c) - k4_0;
    }
    count = *reinterpret_cast<int*>(param_1 + 0x200);
    iVar10 = 0; iVar6 = 0; local_194f = k100000;
    if (0 < count) {
        do {
            local_1a0 = *reinterpret_cast<float*>(param_1 + iVar6 * 8) - param_2[0];
            local_19c = *reinterpret_cast<float*>(param_1 + 4 + iVar6 * 8) - param_2[1];
            fVar9 = local_1a0 * local_1a0 + local_19c * local_19c;
            local_140[iVar6] = fVar9;
            if (fVar9 < local_194f) { iVar10 = iVar6; local_194f = fVar9; }
            iVar6 = iVar6 + 1;
        } while (iVar6 < count);
    }
    if (param_5 != 0) {   // cache override + writeback
        iVar6 = *reinterpret_cast<int*>(kNearestCache + static_cast<std::uintptr_t>(param_4 * 5) * 4);
        iVar4 = iVar10 - iVar6;
        if ((iVar4 != 1 - count) && (((iVar4 < -1) || (1 < iVar4)) && (iVar6 < count))) {
            iVar10 = iVar6;
        }
        *reinterpret_cast<int*>(kNearestCache + static_cast<std::uintptr_t>(param_4 * 5) * 4) = iVar10;
    }
    // seed subdivision brackets from t=0.01 @ iVar10 and t=0.99 @ iVar10-1
    Spline(param_1, iVar10, k0_01, pt16c);
    local_1a0 = pt16c[0] - param_2[0];
    iVar8 = iVar10 + -1;
    local_19c = pt16c[1] - param_2[1];
    local_190 = static_cast<float>(D2(local_1a0, local_19c));   // dist^2 @ (iVar10, 0.01), float32
    if (iVar8 < 0) iVar8 = count + -1;
    Spline(param_1, iVar8, k0_99, pt174);
    local_1a0 = pt174[0] - param_2[0];
    local_19c = pt174[1] - param_2[1];
    // x87 compares the fresh 80-bit dist^2 (unrounded) to the float32-stored local_190
    if (D2(local_1a0, local_19c) < static_cast<double>(local_190)) {
        pt16c[0] = pt174[0]; pt16c[1] = pt174[1]; iVar10 = iVar8;
    }
    local_18c = kOne;
    Spline(param_1, iVar10, kOne, pt148);
    local_1a0 = pt148[0] - param_2[0];
    local_19c = pt148[1] - param_2[1];
    local_180 = kZero;
    local_164f = static_cast<float>(D2(local_1a0, local_19c));
    Spline(param_1, iVar10, kZero, pt188);
    local_1a0 = pt188[0] - param_2[0];
    iVar8 = 0x10;
    local_19c = pt188[1] - param_2[1];
    local_194f = static_cast<float>(D2(local_1a0, local_19c));
    do {   // 16-step binary subdivision toward the closest-approach t
        if (local_164f <= local_194f) {
            local_180 = static_cast<float>((static_cast<double>(local_180) + local_180 + local_18c)
                                           * static_cast<double>(k0_333));
            Spline(param_1, iVar10, local_180, pt188);
            local_1a0 = pt188[0] - param_2[0];
            local_19c = pt188[1] - param_2[1];
            local_194f = static_cast<float>(D2(local_1a0, local_19c));
        } else {
            local_18c = static_cast<float>((static_cast<double>(local_18c) + local_18c + local_180)
                                           * static_cast<double>(k0_333));
            Spline(param_1, iVar10, local_18c, pt148);
            local_1a0 = pt148[0] - param_2[0];
            local_19c = pt148[1] - param_2[1];
            local_164f = static_cast<float>(D2(local_1a0, local_19c));
        }
        iVar8 = iVar8 + -1;
    } while (iVar8 != 0);
    if (local_164f <= local_194f) { local_15c = pt148[1]; local_194f = local_18c; fVar9 = pt148[0]; }
    else                          { local_15c = pt188[1]; local_194f = local_180; fVar9 = pt188[0]; }
    local_160 = fVar9;
    if (param_6 != 0) { /* STUB S: debug-render seg (never on canonical path) */ }

    // forward-walk 16 samples, scoring each by direction dot
    local_190 = local_194f;      // start t = subdivision winner
    pt188[1] = local_15c;        // cur point Z
    pt188[0] = fVar9;            // cur point X
    iVar8 = 0;
    float* pfVar7 = local_140;
    do {
        local_190 = local_190 + k0_05;
        if (kOne < local_190) {
            local_190 = local_190 - kOne;
            iVar10 = iVar10 + 1;
            if (iVar10 == *reinterpret_cast<int*>(param_1 + 0x200)) iVar10 = 0;
        }
        Spline(param_1, iVar10, local_190, pt16c);   // next point
        pt150[0] = pt16c[0] - pt188[0];              // dir1 = next - cur
        pt150[1] = pt16c[1] - pt188[1];
        Vec2Norm(pt150, pt150);
        pt174[0] = pt188[0] - param_2[0];            // dir2 = cur - veh
        pt174[1] = pt188[1] - param_2[1];
        Vec2Norm(pt174, pt174);
        double dotd = Dot2(pt150[0], pt150[1], pt174[0], pt174[1]);   // dir1·dir2 (x87 single-round)
        if (dotd < 0.0) dotd = -static_cast<double>(local_190);       // neg dot -> -t (float32 t, exact)
        if (1.0 < dotd) dotd = 1.0;
        local_40[iVar8] = static_cast<float>(dotd);
        pt188[1] = pt16c[1];      // cur = next (Z first, matching decomp order)
        pfVar7[0] = pt16c[0];
        pt188[0] = pt16c[0];      // cur = next (X)
        pfVar7[1] = pt16c[1];
        if (param_6 != 0) { /* STUB: debug-render seg */ }
        iVar8 = iVar8 + 1;
        pfVar7 = pfVar7 + 2;
    } while (iVar8 < 0x10);

    // pick max-dot sample (argmax of local_40[0..15]; strict < => first max wins)
    iVar8 = 0; fVar9 = kZero;
    for (int k = 0; k < 0x10; ++k) { if (fVar9 < local_40[k]) { iVar8 = k; fVar9 = local_40[k]; } }
    param_3[0] = local_140[iVar8 * 2];
    param_3[1] = local_140[iVar8 * 2 + 1];
    if (param_6 != 0) { /* STUB: debug-render seg */ }

    // wall-check retry loop
    local_194i = 0; local_164i = 0;
    while (true) {
        pt174[0] = param_3[0] - param_2[0];
        local_190 = kZero;
        pt174[1] = param_3[1] - param_2[1];
        local_180 = Vec2Len(pt174);            // length to target
        pt150[0] = param_2[0];
        pt150[1] = param_2[1];
        bVar3 = false;
        if (local_180 <= kZero) {
            goto branchA;
        } else {
            double t = 0.0;   // x87 keeps the marching param in 80-bit ST across the loop
            do {
                if (bVar3) goto afterMarch;
                pt188[0] = static_cast<float>(static_cast<double>(pt174[0]) * (static_cast<double>(kOne) / local_180) * t + param_2[0]);
                pt188[1] = static_cast<float>((static_cast<double>(kOne) / local_180) * pt174[1] * t + param_2[1]);
                if ((pt188[0] != pt150[0]) && (pt188[1] != pt150[1])) {
                    if (WallAt(pt188[0], pt188[1])) { bVar3 = true; local_164i = 1; }
                }
                t = t + static_cast<double>(k0_25);
            } while (t < static_cast<double>(local_180));
            if (!bVar3) goto branchA;
            goto afterMarch;
        }
    branchA:
        pt174[0] = param_3[0] - param_2[0];
        local_190 = kZero;
        pt174[1] = param_3[1] - param_2[1];
        local_180 = Vec2Len(pt174);
        bVar3 = false;
        pt16c[0] = pt174[1] * (kOne / local_180);          // dz_norm (single op: x87==SSE2)
        local_160 = static_cast<float>(static_cast<double>(param_2[0]) - static_cast<double>(pt16c[0]) * k0_25);  // back-offset start X
        pt148[0] = (kOne / local_180) * pt174[0];          // dx_norm (single op)
        local_15c = static_cast<float>(static_cast<double>(param_2[1]) - static_cast<double>(pt148[0]) * k0_25);  // back-offset start Z
        pt150[0] = local_160;
        pt150[1] = local_15c;
        if (kZero < local_180) {
            double t = 0.0;
            do {
                if (bVar3) break;
                pt188[0] = static_cast<float>(static_cast<double>(pt148[0]) * t + local_160);
                pt188[1] = static_cast<float>(static_cast<double>(pt16c[0]) * t + local_15c);
                if ((pt188[0] != local_160) && (pt188[1] != local_15c)) {
                    if (WallAt(pt188[0], pt188[1])) { bVar3 = true; local_164i = 1; }
                }
                t = t + static_cast<double>(k0_25);
            } while (t < static_cast<double>(local_180));
            goto afterMarch;
        }
        local_194i = 1;   // degenerate zero-length
    afterMarch:
        if (!bVar3) {
            local_194i = 1;   // no wall found
        } else {
            iVar10 = iVar8;   // wall hit: back off target sample index by 2 (clamped)
            if ((iVar8 == 0) || (iVar10 = 0, iVar8 == 1)) { iVar8 = iVar10; local_194i = 1; }
            else { iVar8 = iVar8 + -2; }
            param_3[0] = local_140[iVar8 * 2];
            param_3[1] = local_140[iVar8 * 2 + 1];
        }
        Arm(param_4, iVar8 == 0 ? 1 : 0);
        if (local_194i != 0) {
            iVar10 = iVar8;
            if (local_164i != 0) {
                if ((iVar8 != 0) && (iVar10 = 0, iVar8 != 1)) iVar10 = iVar8 + -2;
                param_3[0] = local_140[iVar10 * 2];
                param_3[1] = local_140[iVar10 * 2 + 1];
            }
            if (param_6 != 0) { /* STUB: debug-render seg */ }
            return;
        }
        (void)local_190; (void)local_158; (void)local_154;
    }
}

// ── Orig trampoline: hook overwrites the 5-byte-split SUB ESP,0x1a0 → re-exec it
// then jmp to 0x00443dc6 (next instr). __cdecl arg layout matches the original entry.
void* g_orig_443dc6 = reinterpret_cast<void*>(0x00443dc6);
__declspec(naked) void OrigNavLookahead(int, float*, float*, int, int, int) {
    __asm {
        sub  esp, 0x1a0
        jmp  dword ptr [g_orig_443dc6]
    }
}

// ── in-process A/B self-test (env MASHED_AI_SPLINE_SELFTEST) ─────────────────
inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_SPLINE_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_nav_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_calls = 0, g_mismatch = 0;
const long kMaxCompare = 20000;

// snapshot/restore the 3 written pieces of state (outXZ, per-car nearest cache slot,
// arm-flag slot); run orig, capture, restore, run mine, compare; leave MINE's effects.
void NavDispatch(int spline, float* vehXZ, float* outXZ, int aiCarIdx, int useCache, int debugRender) {
    if (SelfTestEnabled() && g_calls < kMaxCompare) {
        int* cachePtr = reinterpret_cast<int*>(kNearestCache + static_cast<std::uintptr_t>(aiCarIdx * 5) * 4);
        int* armPtr   = reinterpret_cast<int*>(kArmFlagBase + static_cast<std::uintptr_t>(aiCarIdx) * 0x74u);
        int   cachePre = *cachePtr, armPre = *armPtr;
        float p3Pre[2] = { outXZ[0], outXZ[1] };

        OrigNavLookahead(spline, vehXZ, outXZ, aiCarIdx, useCache, debugRender);
        float oOut[2] = { outXZ[0], outXZ[1] };
        int   oCache = *cachePtr, oArm = *armPtr;

        *cachePtr = cachePre; *armPtr = armPre; outXZ[0] = p3Pre[0]; outXZ[1] = p3Pre[1];
        NavLookahead(spline, vehXZ, outXZ, aiCarIdx, useCache, debugRender);
        int mCache = *cachePtr, mArm = *armPtr;

        std::uint32_t ox0, ox1, mx0, mx1;
        std::memcpy(&ox0, &oOut[0], 4); std::memcpy(&ox1, &oOut[1], 4);
        std::memcpy(&mx0, &outXZ[0], 4); std::memcpy(&mx1, &outXZ[1], 4);
        ++g_calls;
        bool bad = (ox0 != mx0) || (ox1 != mx1) || (oCache != mCache) || (oArm != mArm);
        if (bad) {
            ++g_mismatch;
            char line[256];
            wsprintfA(line, "[%ld] MISMATCH car=%d uc=%d X:o=%08x m=%08x Z:o=%08x m=%08x cache o=%d m=%d arm o=%d m=%d\r\n",
                      g_calls, aiCarIdx, useCache, ox0, mx0, ox1, mx1, oCache, mCache, oArm, mArm);
            SelfTestLog(line);
        }
        if ((g_calls & 0xff) == 1) {
            char line[128];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld %s\r\n", g_calls, g_calls, g_mismatch,
                      g_mismatch ? "" : "ALL-GREEN");
            SelfTestLog(line);
        }
    } else {
        // WS-R6-B status (2026-07-01, run3): the catastrophic divergence was root-caused
        // and FIXED — FUN_004c3c60 (Vec2Norm) returns float10 in ST0 but was declared
        // `void`, so the SSE2-built caller never popped ST0; 2 calls/iter overflowed the
        // x87 stack -> NaN walk candidates + FPU corruption (AI froze). With the fn-ptr
        // fixed to a value return (ST0 popped), the A/B went 100% -> ~37% mismatch and the
        // AI drives. The RESIDUAL 37% is the predicted float10 issue: the dot/dist^2 kernels
        // run in `double` (~2^-53) not true x87 80-bit (2^-64), so on near-tied lookahead
        // scores the argmax flips (58% of mismatches are sub-ULP inert; ~15% of all calls
        // are large flips, clustered at the degenerate race-start geometry). Bit-identity
        // needs the full naked-asm x87 body (as originally scoped) — the whole
        // winner-t -> walk-t -> Spline -> Vec2Norm -> dot chain feeds the argmax, so partial
        // x87 can't close it. Until that lands, the installed hook is a SAFE PASSTHROUGH so a
        // default .asi run matches the original exactly; the reimpl is A/B-measured only under
        // MASHED_AI_SPLINE_SELFTEST. Detail: R6_FUN_00443dc0_SCOPE_20260701.md.
        OrigNavLookahead(spline, vehXZ, outXZ, aiCarIdx, useCache, debugRender);
    }
}

// ── naked entry installed at 0x00443dc0 (forwards the 6 __cdecl stack args) ──
__declspec(naked) void AiNav_Entry() {
    __asm {
        // [esp]=ret [esp+4]=spline [esp+8]=vehXZ [esp+c]=outXZ [esp+10]=aiCarIdx
        //                                        [esp+14]=useCache [esp+18]=debugRender
        push dword ptr [esp+0x18]   // debugRender
        push dword ptr [esp+0x18]   // useCache
        push dword ptr [esp+0x18]   // aiCarIdx
        push dword ptr [esp+0x18]   // outXZ
        push dword ptr [esp+0x18]   // vehXZ
        push dword ptr [esp+0x18]   // spline
        call NavDispatch
        add  esp, 24                // cdecl clean our 6 forwarded args
        ret                         // original is caller-cleans (ret no imm)
    }
}

}  // namespace

RH_ScopedInstall(AiNav_Entry, 0x00443dc0);
