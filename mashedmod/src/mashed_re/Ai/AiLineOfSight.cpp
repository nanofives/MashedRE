// Mashed RE — WS-R6-C: AI line-of-sight check (FUN_00416060) verbatim port.
//
// Target FUN_00416060 (0x00416060..0x004161df, ~383 bytes), __cdecl 2 args:
//   int LineOfSight(float* posA /*param_1*/, float* posB /*param_2*/)
// Ray-marches A→B (step 0.25 in world-length units) sampling the track tile grid;
// returns 0 the moment a wall/boundary tile (sub-cell type 0 or 3) blocks the line,
// else 1 (clear). PURE: reads posA/posB + the two static tile grids, no side effects.
//
// This is FUN_00416250's per-candidate LOS gate (called after every targeting
// sub-behavior) and one of its simpler C2 callees — draining it to C3 de-risks the
// eventual FUN_00416250 orchestrator port (see R6_FUN_00416250_SCOPE_20260701.md).
//
// AUTHORED FROM RAW ASM (Mashed_pool3 RO, 2026-07-01), not the lossy decomp. Exact
// facts the decomp drops: the *4.0 tile scale (_DAT_005cc35c) + round-half-away-from-
// zero bias (±0.5 = _DAT_005cc32c, chosen by FCOM coord*4 > 0) before __ftol
// (FUN_004a2c48, truncates toward zero); SAR (signed >>3) tile indexing; and the grid
// role: the sample's SECOND float (Z) is the row (*0x80), the FIRST float (X) is the
// column. (Note: FUN_00443dc0's WallAt uses the opposite X-row convention — the two
// originals genuinely differ; each is replicated to its own asm.)
//
// float10 discipline (learned in WS-R6-B, see feedback_x87_st0_float10_return_fnptr):
// FUN_004c3bf0 (Vec2Length) RETURNS float10 IN ST0. The forwarding fn-ptr MUST be
// declared returning a value (float) so the SSE2-built .asi pops ST0 — a `void` decl
// leaks the x87 stack. The precision-sensitive sample expression is computed in
// `double` (single round to float32 at the store) to emulate x87's single-rounding
// vs SSE2's per-op double-rounding; the marching t steps by exact 0.25 so plain float
// is bit-exact for it. Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

namespace {

inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
const float kZero = Cf(0x00000000);  // DAT_005d757c  0.0
const float kOne  = Cf(0x3f800000);  // _DAT_005cc320 1.0
const float k0_25 = Cf(0x3e800000);  // _DAT_005cc564 ray-march step
const float k4_0  = Cf(0x40800000);  // _DAT_005cc35c tile coord scale
const float kHalf = Cf(0x3f000000);  // _DAT_005cc32c round-half bias

const std::uintptr_t kTileGrid    = 0x007f1a9cu;   // 128x128 shorts
const std::uintptr_t kSubCellGrid = 0x007f9a9cu;   // 8x8 sub-cell chars/tile

// FUN_004c3bf0 (Vec2Length) returns float10 in ST0 — declare `float` so ST0 is popped.
typedef float (__cdecl* fn_len_t)(float*);
inline float Vec2Len(float* v) { return reinterpret_cast<fn_len_t>(0x004c3bf0)(v); }

// tile sample (asm-decoded): sx/sz are the float32 world sample; returns true if the
// tile there is a wall (sub-cell type 0 or 3). Z(second float)=row, X(first)=col.
inline bool LosWallAt(float sx, float sz) {
    double sz4 = static_cast<double>(sz) * k4_0;   // FMUL 4.0 (row axis)
    double sx4 = static_cast<double>(sx) * k4_0;   // FMUL 4.0 (col axis)
    int zc = static_cast<int>(sz4 > 0.0 ? sz4 + kHalf : sz4 - kHalf);   // __ftol(round-half-away)
    int xc = static_cast<int>(sx4 > 0.0 ? sx4 + kHalf : sx4 - kHalf);
    int tileIdx = (((zc + 0x1f0) >> 3) << 7) + ((xc + 0x1f0) >> 3);     // SAR (signed >>); zc=row
    short tile = *reinterpret_cast<short*>(kTileGrid + static_cast<std::uintptr_t>(tileIdx) * 2);
    if (tile > 0 && tile < 0x200) {
        char sub = *reinterpret_cast<char*>(
            kSubCellGrid + static_cast<std::uintptr_t>((zc & 7) + ((xc & 7) + static_cast<int>(tile) * 8) * 8));
        if (sub == 0 || sub == 3) return true;
    }
    return false;
}

// ===========================================================================
// LineOfSight — greenfield reimpl of FUN_00416060. Returns 1 (clear) / 0 (blocked).
// ===========================================================================
int LineOfSight(float* param_1, float* param_2) {
    float d[2];                              // {dx, dz} — contiguous (Vec2Length reads both)
    d[0] = param_2[0] - param_1[0];
    d[1] = param_2[1] - param_1[1];
    float len = Vec2Len(d);
    if (kZero < len) {
        float invlen = static_cast<float>(1.0 / static_cast<double>(len));   // FLD 1.0 / FDIV len -> f32
        float t = kZero;
        do {
            // sample = ((invlen*d)*t) + posA  in 80-bit x87 -> float32; emulate with double
            float sx = static_cast<float>((static_cast<double>(invlen) * d[0]) * t + param_1[0]);
            float sz = static_cast<float>((static_cast<double>(invlen) * d[1]) * t + param_1[1]);
            if ((sx != param_1[0]) && (sz != param_1[1])) {   // skip the start point
                if (LosWallAt(sx, sz)) return 0;              // wall blocks the line
            }
            t = t + k0_25;
        } while (t < len);
    }
    return 1;
}

// ── Orig trampoline: the hook overwrites the 5-byte-split SUB ESP,0x18 + MOV
// EAX,[ESP+0x20]; re-exec both then jmp to 0x00416067 (next instr, the FLD [EAX]). ──
void* g_orig_416067 = reinterpret_cast<void*>(0x00416067);
__declspec(naked) int OrigLineOfSight(float*, float*) {
    __asm {
        sub  esp, 0x18
        mov  eax, dword ptr [esp + 0x20]
        jmp  dword ptr [g_orig_416067]
    }
}

// ── in-process A/B self-test (env MASHED_AI_LOS_SELFTEST) ────────────────────
inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_LOS_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_los_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_calls = 0, g_mismatch = 0;
const long kMaxCompare = 40000;

// LOS is pure -> the A/B just compares the 0/1 return. Return ORIG's value so game
// behavior is unchanged during measurement; SAFE PASSTHROUGH when not self-testing.
int LosDispatch(float* a, float* b) {
    if (SelfTestEnabled() && g_calls < kMaxCompare) {
        int o = OrigLineOfSight(a, b);
        int m = LineOfSight(a, b);
        ++g_calls;
        if (o != m) {
            ++g_mismatch;
            char line[192];
            std::uint32_t ax0, az0, bx0, bz0;
            std::memcpy(&ax0, &a[0], 4); std::memcpy(&az0, &a[1], 4);
            std::memcpy(&bx0, &b[0], 4); std::memcpy(&bz0, &b[1], 4);
            wsprintfA(line, "[%ld] MISMATCH o=%d m=%d  A=(%08x,%08x) B=(%08x,%08x)\r\n",
                      g_calls, o, m, ax0, az0, bx0, bz0);
            SelfTestLog(line);
        }
        if ((g_calls & 0x7f) == 1) {
            char line[128];
            wsprintfA(line, "[%ld] calls=%ld mism=%ld %s\r\n", g_calls, g_calls, g_mismatch,
                      g_mismatch ? "" : "ALL-GREEN");
            SelfTestLog(line);
        }
        return o;
    }
    return OrigLineOfSight(a, b);
}

// ── naked entry installed at 0x00416060 (forwards the 2 __cdecl stack args) ──
__declspec(naked) void AiLos_Entry() {
    __asm {
        // [esp]=ret [esp+4]=param_1 [esp+8]=param_2
        push dword ptr [esp + 8]    // param_2
        push dword ptr [esp + 8]    // param_1 (esp shifted 4 after 1st push)
        call LosDispatch
        add  esp, 8                 // cdecl clean our 2 forwarded args
        ret                         // original is caller-cleans; EAX = 0/1 result preserved
    }
}

}  // namespace

RH_ScopedInstall(AiLos_Entry, 0x00416060);
