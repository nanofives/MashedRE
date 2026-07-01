// Mashed RE — WS-R6-D: AI track-wall lateral-zone query (FUN_004150e0) verbatim port.
//
// Target FUN_004150e0 (0x004150e0..0x0041518f, 175 bytes), __cdecl. The caller pushes
// 2 args but the callee reads ONLY arg1 = a float* pair {x,z}; arg2 is never touched
// (verified from raw asm — the only stack read is MOV EAX,[ESP+4]; the later [ESP+0x8]
// reads are the FSTP spill slot, not param_2). Plain RET → caller-cleans.
//   int WallLateral(float* p)   // p[0]=x (col axis), p[1]=z (row axis)
//
// A SINGLE-POINT tile-grid query — the same tile-check FUN_00416060 (AiLineOfSight)
// ray-marches, minus the loop and the Vec2Length distance. Returns 1 iff the sub-cell
// surface byte at the point == 2 (a distinct surface class from LOS, which tested 0||3),
// else 0. PURE: reads p + the two static tile grids, no side effects.
//
// This is one of FUN_00416250's mode-9 gate callees (also called by FUN_00416a30 /
// FUN_00417da0); draining it to C3 de-risks the FUN_00416250 orchestrator port
// (see R6_FUN_00416250_SCOPE_20260701.md, "NEXT leaf ready to port").
//
// AUTHORED FROM RAW ASM (Mashed_pool13 RO, 2026-07-01), not the lossy decomp — the
// decomp's `(void)` signature + "fastcall register-consuming" reading is wrong; the asm
// reads a single float* at [ESP+4]. Exact facts: *4.0 tile scale (_DAT_005cc35c),
// round-half-away-from-zero (±0.5 = _DAT_005cc32c, sign chosen by FCOM coord*4 > 0)
// before __ftol (FUN_004a2c48, truncate toward zero); SAR (signed >>3) tile index; the
// second float p[1] (Z) is the row (<<7 = *0x80), the first float p[0] (X) is the column
// — byte-identical grid math to AiLineOfSight.cpp's LosWallAt. The ONLY divergence from
// LOS is the surface test (== 2 here vs == 0||3 there) and the missing ray-march.
//
// float discipline (feedback_x87_st0_float10_return_fnptr): no float10-ST0 callee here
// (no Vec2Length) — the only callee is __ftol. coord*4.0 is an exact power-of-2 scale so
// float and x87 agree; the ±0.5-then-truncate is computed in `double` to mirror x87's
// single-round (bit-exact for the bounded track-coord range). Anchored to MASHED.exe
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

namespace {

inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
const float k4_0  = Cf(0x40800000);  // _DAT_005cc35c tile coord scale (4.0)
const float kHalf = Cf(0x3f000000);  // _DAT_005cc32c round-half bias (0.5)

const std::uintptr_t kTileGrid    = 0x007f1a9cu;   // 128x128 shorts (DAT_007f1a9c)
const std::uintptr_t kSubCellGrid = 0x007f9a9cu;   // 8x8 sub-cell chars/tile (DAT_007f9a9c)

// Single-point tile query (asm-decoded 0x004150e0): sx=p[0] (col axis), sz=p[1] (row
// axis). Grid math byte-identical to AiLineOfSight.cpp's LosWallAt; surface test is
// `== 2` (vs LOS's 0||3). Returns true iff sub-cell surface byte == 2.
inline bool WallLatAt(float sx, float sz) {
    double sz4 = static_cast<double>(sz) * k4_0;   // FMUL 4.0 (row axis, first ftol)
    double sx4 = static_cast<double>(sx) * k4_0;   // FMUL 4.0 (col axis, second ftol)
    int zc = static_cast<int>(sz4 > 0.0 ? sz4 + kHalf : sz4 - kHalf);   // __ftol(round-half-away)
    int xc = static_cast<int>(sx4 > 0.0 ? sx4 + kHalf : sx4 - kHalf);
    int tileIdx = (((zc + 0x1f0) >> 3) << 7) + ((xc + 0x1f0) >> 3);     // SAR (signed >>); zc=row
    short tile = *reinterpret_cast<short*>(kTileGrid + static_cast<std::uintptr_t>(tileIdx) * 2);
    if (tile > 0 && tile < 0x200) {
        char sub = *reinterpret_cast<char*>(
            kSubCellGrid + static_cast<std::uintptr_t>((zc & 7) + ((xc & 7) + static_cast<int>(tile) * 8) * 8));
        if (sub == 2) return true;
    }
    return false;
}

// ===========================================================================
// WallLateral — greenfield reimpl of FUN_004150e0. Returns 1 (surface==2) / 0.
// ===========================================================================
int WallLateral(float* param_1) {
    return WallLatAt(param_1[0], param_1[1]) ? 1 : 0;
}

// ── Orig trampoline: the 5-byte E9-rel32 hook clobbers bytes 0x004150e0..0x004150e4 —
// the whole 4-byte MOV EAX,[ESP+4] AND the first byte of FLD float ptr [EAX+4] (which
// begins at 0x004150e4). So BOTH clobbered instructions must be re-executed, then jump
// to the first INTACT instruction boundary 0x004150e7 (FMUL float ptr [0x005cc35c]).
// Re-exec MOV (EAX=p) + FLD (st0=p[1]) reproduces the exact register/x87 state the
// original body expects at 0x004150e7. ESP unchanged by the prologue, so [ESP+4]=the
// forwarded pointer. (The earlier jmp-to-0x004150e4 landed inside the hook's own jmp
// byte → crash right at phase-3 AI start.) ──
void* g_orig_4150e7 = reinterpret_cast<void*>(0x004150e7);
__declspec(naked) int OrigWallLateral(float*) {
    __asm {
        mov  eax, dword ptr [esp + 4]
        fld  dword ptr [eax + 4]
        jmp  dword ptr [g_orig_4150e7]
    }
}

// ── in-process A/B self-test (env MASHED_AI_WALLLAT_SELFTEST) ────────────────
inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_WALLLAT_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_walllat_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_calls = 0, g_mismatch = 0;
const long kMaxCompare = 40000;

// Pure fn -> the A/B compares the 0/1 return. Return ORIG's value so game behavior is
// unchanged during measurement; SAFE PASSTHROUGH when not self-testing.
int WallLatDispatch(float* p) {
    if (SelfTestEnabled() && g_calls < kMaxCompare) {
        int o = OrigWallLateral(p);
        int m = WallLateral(p);
        ++g_calls;
        if (o != m) {
            ++g_mismatch;
            char line[160];
            std::uint32_t px, pz;
            std::memcpy(&px, &p[0], 4); std::memcpy(&pz, &p[1], 4);
            wsprintfA(line, "[%ld] MISMATCH o=%d m=%d  p=(%08x,%08x)\r\n", g_calls, o, m, px, pz);
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
    return OrigWallLateral(p);
}

// ── naked entry installed at 0x004150e0 (forwards param_1; caller cleans its 2 args) ──
__declspec(naked) void AiWallLat_Entry() {
    __asm {
        // [esp]=ret [esp+4]=param_1(p) [esp+8]=param_2(ignored)
        push dword ptr [esp + 4]    // param_1
        call WallLatDispatch
        add  esp, 4                 // cdecl clean our 1 forwarded arg
        ret                         // original is caller-cleans; EAX = 0/1 result preserved
    }
}

}  // namespace

RH_ScopedInstall(AiWallLat_Entry, 0x004150e0);
