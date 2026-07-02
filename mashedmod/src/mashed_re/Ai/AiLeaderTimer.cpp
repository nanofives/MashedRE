// Mashed RE — WS-R6-D: AI last-place catch-up timer (FUN_004148b0) verbatim port.
//
// Target FUN_004148b0 (0x004148b0..0x00414a68, 440 bytes), __cdecl 4 args:
//   int LeaderTimer(int aiLine /*p1, unused*/, void* p2 /*unused*/,
//                   int* outXZ /*param_3*/, int vehIdx /*param_4*/)
// The last-place catch-up ("rubber-band") timer with a rank-counter cap. It reads the
// per-vehicle progress metric FUN_00442cc0 and, when this vehicle is last and far behind,
// accumulates a per-vehicle timer DAT_0089a4c8[v] by the frame-delta DAT_007f1008, writes
// the leader's XZ into *outXZ, and returns 1 once the timer trips (>0x2710) — bumping the
// rank counter DAT_0089a4c4[v] and resetting the timer. Otherwise returns 0 (often after
// resetting the timer + bumping the rank). Gates the orchestrator's mode-5 path.
//
// AUTHORED FROM RAW ASM (Mashed_pool13 RO, 2026-07-01). State writes (the entire A/B
// snapshot surface): the two per-vehicle ints DAT_0089a4c8 + DAT_0089a4c4 at +vehIdx*0x74,
// and *outXZ (2 floats, only on the return-1 path). All callees are PURE getters
// (FUN_0040e470, FUN_00442cc0, FUN_0046d4a0) or __ftol — none mutate global state, so the
// snapshot is exactly those two ints. FUN_00442cc0 (AiVehicleFloat4Get, C3) returns
// float10 but is f32-sourced (loads a stored float), so its comparisons against the
// _DAT_005cd0a*/_DAT_005cc35c thresholds are exact as f32 — declared float. The entry
// __ftol of DAT_0089a360 has no ±0.5 pre-bias, so its rounding matters; rather than assume
// truncate-vs-round, it is FORWARDED to the original __ftol (FUN_004a2c48) via an x87 shim
// (FLD the value, CALL, take EAX) for guaranteed bit-identity. Integer accumulations
// (timer += DAT_007f1008, thresholds 0x2710/0x1194) are exact. Anchored to MASHED.exe
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// COVERAGE: FUN_004148b0 fires only on the orchestrator's mode-5 path (render sub-mode 6,
// target-enable off, no higher mode) which a normal race rarely reaches. Because it is
// STATEFUL, it is verified via a snapshot/restore driver hooked at the orchestrator entry
// FUN_00416250 that runs BOTH orig and mine into scratch, then ROLLS BACK the two globals
// to their pre-call values (leaving the game state UNPERTURBED — the game's own natural
// call, if any, is separate). Gate: MASHED_HOOK_ONLY=0x00416250 + MASHED_AI_LEADER_SELFTEST=1.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <windows.h>

namespace {

inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }
const float kZero = Cf(0x00000000);  // DAT_005d757c 0.0

// globals (raw VAs)
const std::uintptr_t kMode368   = 0x0089a368u;  // DAT_0089a368
const std::uintptr_t kFlt360    = 0x0089a360u;  // DAT_0089a360 (float, __ftol'd at entry)
const std::uintptr_t kIdx364    = 0x0089a364u;  // DAT_0089a364
const std::uintptr_t kBias374   = 0x0089a374u;  // DAT_0089a374
const std::uintptr_t kLimitTbl  = 0x005f2dd8u;  // DAT_005f2dd8 (int limit table)
const std::uintptr_t kTimerBase = 0x0089a4c8u;  // DAT_0089a4c8 + v*0x74 (catch-up timer, int)
const std::uintptr_t kRankBase  = 0x0089a4c4u;  // DAT_0089a4c4 + v*0x74 (rank counter, int)
const std::uintptr_t kFrameDt   = 0x007f1008u;  // DAT_007f1008 (int frame-delta)

// live float threshold globals (read at call time to stay bit-faithful to the image)
inline float G_a8()  { return *reinterpret_cast<float*>(0x005cd0a8u); }  // _DAT_005cd0a8
inline float G_a4()  { return *reinterpret_cast<float*>(0x005cd0a4u); }  // _DAT_005cd0a4
inline float G_a0()  { return *reinterpret_cast<float*>(0x005cd0a0u); }  // _DAT_005cd0a0
inline float G_35c() { return *reinterpret_cast<float*>(0x005cc35cu); }  // _DAT_005cc35c (4.0)

inline int   Gi(std::uintptr_t a) { return *reinterpret_cast<int*>(a); }
inline int&  TimerAt(int v) { return *reinterpret_cast<int*>(kTimerBase + (std::uintptr_t)v * 0x74); }
inline int&  RankAt (int v) { return *reinterpret_cast<int*>(kRankBase  + (std::uintptr_t)v * 0x74); }

// ── live callee forwards ─────────────────────────────────────────────────────
typedef int   (__cdecl* fn_e470_t)(int);              // FUN_0040e470 (active-vehicle test)
typedef float (__cdecl* fn_442cc0_t)(int);            // FUN_00442cc0 (progress; float10-ST0 -> float)
typedef void  (__cdecl* fn_rec_t)(void**, int);       // FUN_0046d4a0 (vehicle-record ptr getter)
inline int   E470(int i)        { return reinterpret_cast<fn_e470_t>(0x0040e470)(i); }
inline float Prog(int i)        { return reinterpret_cast<fn_442cc0_t>(0x00442cc0)(i); }
inline void  VehRecPtr(void** o, int i) { reinterpret_cast<fn_rec_t>(0x0046d4a0)(o, i); }

// __ftol forwarded via x87 shim: FLD the value onto ST0, CALL FUN_004a2c48 (consumes ST0,
// returns the int in EAX). Guarantees the exact rounding of DAT_0089a360. (Inline asm can't
// CALL a numeric literal, so route through a pointer.)
void* g_ftol_4a2c48 = reinterpret_cast<void*>(0x004a2c48);
__declspec(naked) int Ftol(double /*x*/) {
    __asm {
        fld  qword ptr [esp + 4]
        call dword ptr [g_ftol_4a2c48]
        ret
    }
}

// ===========================================================================
// LeaderTimer — greenfield reimpl of FUN_004148b0. Returns 0/1.
// ===========================================================================
int LeaderTimer(int /*param_1*/, void* /*param_2*/, int* param_3, int param_4) {
    if (Gi(kMode368) == 2) return 0;
    int iVar1 = Ftol(static_cast<double>(*reinterpret_cast<float*>(kFlt360)));
    int idx364 = Gi(kIdx364);
    if (idx364 != -1 && E470(idx364) == 1) {
        iVar1 += 3;
        if (iVar1 > 10) iVar1 = 10;
    }
    int limit = *reinterpret_cast<int*>(kLimitTbl + (std::uintptr_t)(Gi(kBias374) + iVar1 * 5) * 4);
    if (limit <= RankAt(param_4)) return 0;   // rank_counter >= limit

    float fVar4 = Prog(param_4);
    if (fVar4 == kZero) {
        int last = -1;
        for (int i = 0; i < 4; ++i) { if (E470(i) == 1) last = i; }
        if (last == -1) return 0;
        fVar4 = Prog(last);
        if (G_a8() < fVar4) {
            TimerAt(param_4) += Gi(kFrameDt);
            void* rec = nullptr; VehRecPtr(&rec, param_4);
            std::uintptr_t r = reinterpret_cast<std::uintptr_t>(rec);
            param_3[0] = *reinterpret_cast<int*>(r + 0x30);
            param_3[1] = *reinterpret_cast<int*>(r + 0x38);
            if (TimerAt(param_4) > 0x2710) { TimerAt(param_4) = 0; RankAt(param_4) += 1; }
            return 1;
        }
        if (G_a4() < fVar4 && TimerAt(param_4) != 0) TimerAt(param_4) += Gi(kFrameDt);
        int iVar1c = 1;
        if (G_a0() <= fVar4) iVar1c = 0;          // local_4 == 0
        if (G_35c() <= fVar4) {
            if (iVar1c == 1) goto LAB_reset;       // -> the 0x1194 gate
            if (iVar1c != 2) return 0;
        }
        if (TimerAt(param_4) == 0) return 0;
    } else {
    LAB_reset:
        if (TimerAt(param_4) < 0x1195) return 0;   // <= 0x1194
    }
    TimerAt(param_4) = 0;
    RankAt(param_4) += 1;
    return 0;
}

// ── Orig trampoline: the 5-byte E9 hook clobbers exactly MOV EAX,[0x0089a368] (a1 68 a3
// 89 00, 5 bytes) at 0x004148b0. Re-exec it then jmp to 0x004148b5 (SUB ESP,8). Called as
// OrigLeader(p1,p2,p3,p4); the original reads param_4 at [ESP+0x28] after its own
// SUB ESP,8 + 4 pushes, matching this call frame. ──
void* g_orig_4148b5 = reinterpret_cast<void*>(0x004148b5);
__declspec(naked) int OrigLeader(int, void*, int*, int) {
    __asm {
        mov  eax, dword ptr [0x0089a368]
        jmp  dword ptr [g_orig_4148b5]
    }
}

// ── SAFE-PASSTHROUGH hook installed at 0x004148b0 (game's own calls) ─────────
// When only this leaf is hooked (not the orchestrator driver), just run the original so
// game behavior is unchanged. The A/B verification is driven from the orchestrator entry.
__declspec(naked) void AiLeader_Entry() {
    __asm {
        // [esp]=ret [esp+4]=p1 [esp+8]=p2 [esp+0xc]=p3 [esp+0x10]=p4
        mov  eax, dword ptr [0x0089a368]   // re-exec clobbered prologue
        jmp  dword ptr [g_orig_4148b5]
    }
}

// The LeaderAB snapshot/restore harness + OrchLeaderCoverage_Entry driver at
// 0x00416250 (the C3 verification apparatus for this port, 2026-07-01) were
// REMOVED 2026-07-02 when the orchestrator port landed at that RVA
// (Ai/AiControlStep.cpp AiControlStep). Evidence + code: git history
// (commit 5811fd0c) and log/diff_ai_leadertimer_004148b0_c3.log.

}  // namespace

RH_ScopedInstall(AiLeader_Entry, 0x004148b0);
