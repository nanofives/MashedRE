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
// call, if any, is separate). Gate: MASHED_HOOK_ONLY=0x00416250. (An earlier comment also
// named MASHED_AI_LEADER_SELFTEST=1; no such accessor exists anywhere in the tree —
// dead name retired 2026-09-09, FLAG_INVENTORY_2026-08-15 class C.)

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

// ── threshold globals ────────────────────────────────────────────────────────
// [GATEFIRE GF1 2026-10-08] CONSOLIDATED to a shared TU. In the .asi these are live
// reads of the loaded image, bit-faithful by construction. Under MASHED_STANDALONE
// the image .rdata is never loaded, so they are the values READ OUT of
// original/MASHED.exe.unpatched (gate GF1-CONSTS, re/tools/dump_rdata_floats.py):
//   0x005cd0a8  raw 0x40d00000 = 6.5
//   0x005cd0a4  raw 0x40b00000 = 5.5
//   0x005cd0a0  raw 0x40c00000 = 6.0
//   0x005cc35c  raw 0x40800000 = 4.0
// Independently corroborated: the 2026-10-03 witness read the same four LIVE via
// Frida and they agreed to every digit (RESULT_WITNESS.md:23, :107).
#ifdef MASHED_STANDALONE
inline float G_a8()  { return Cf(0x40d00000u); }   // _DAT_005cd0a8  6.5
inline float G_a4()  { return Cf(0x40b00000u); }   // _DAT_005cd0a4  5.5
inline float G_a0()  { return Cf(0x40c00000u); }   // _DAT_005cd0a0  6.0
inline float G_35c() { return Cf(0x40800000u); }   // _DAT_005cc35c  4.0
#else
inline float G_a8()  { return *reinterpret_cast<float*>(0x005cd0a8u); }  // _DAT_005cd0a8
inline float G_a4()  { return *reinterpret_cast<float*>(0x005cd0a4u); }  // _DAT_005cd0a4
inline float G_a0()  { return *reinterpret_cast<float*>(0x005cd0a0u); }  // _DAT_005cd0a0
inline float G_35c() { return *reinterpret_cast<float*>(0x005cc35cu); }  // _DAT_005cc35c (4.0)
#endif

inline int   Gi(std::uintptr_t a) { return *reinterpret_cast<int*>(a); }
inline int&  TimerAt(int v) { return *reinterpret_cast<int*>(kTimerBase + (std::uintptr_t)v * 0x74); }
inline int&  RankAt (int v) { return *reinterpret_cast<int*>(kRankBase  + (std::uintptr_t)v * 0x74); }

// ── live callee forwards ─────────────────────────────────────────────────────
typedef int   (__cdecl* fn_e470_t)(int);              // FUN_0040e470 (active-vehicle test)
typedef float (__cdecl* fn_442cc0_t)(int);            // FUN_00442cc0 (progress; float10-ST0 -> float)
typedef void  (__cdecl* fn_rec_t)(void**, int);       // FUN_0046d4a0 (vehicle-record ptr getter)
#ifdef MASHED_STANDALONE
// [GATEFIRE GF1] The .text range 0x00400000..0x004fffff is NOT mapped in the
// standalone (Compat/StandaloneRvaThunks.h:7), so the three call-throughs below would
// access-violate — this is exactly what RESULT_WITNESS.md's W-SAFE leg measured. Each
// is replaced by the port's own equivalent, NOT by a thunk.
//
// FUN_0040e470 CarSlotStateGet, 14 bytes, no branches/calls:
//   return *(int*)(*(int**)0x005f2770 + param_1*4 + 0x34)
// (re/analysis/race_results/0040e470.md). NOTE it had NO exe body: hooks.csv's
// exe_file named Frontend/MenuStateMachine.cpp, which only holds a call-through to
// the absolute address. The same stale-column trap already flagged for 0x004148b0.
// Its substrate 0x005f2770 is live under MASHED_SLOTSTATE_SEED (D-11072 leg C) and
// is the formula TrackRenderer::U9186GateDump's slot_state column already uses.
inline int E470(int i) {
    const std::uintptr_t p = *reinterpret_cast<std::uintptr_t*>(0x005f2770u);
    if (!p) return 0;                      // pre-race: table not seeded
    return *reinterpret_cast<int*>(p + 0x34u + static_cast<std::uintptr_t>(i) * 4u);
}
// FUN_00442cc0 reads the reference-distance array H3 now produces.
inline float Prog(int i) {
    return *reinterpret_cast<float*>(0x008989b0u + static_cast<std::uintptr_t>(i) * 4u);
}
// FUN_0046d4a0's standalone body (H1b, Vehicle/VehicleRecordPtr.cpp), which rebinds
// the record base to g_vehicleArrayBase instead of the blank 0x008815a0 pad.
extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out, std::uint32_t idx);
inline void VehRecPtr(void** o, int i) {
    std::uint32_t p = 0;
    *o = PtrCompute881ec8(&p, static_cast<std::uint32_t>(i))
             ? reinterpret_cast<void*>(static_cast<std::uintptr_t>(p)) : nullptr;
}
#else
inline int   E470(int i)        { return reinterpret_cast<fn_e470_t>(0x0040e470)(i); }
inline float Prog(int i)        { return reinterpret_cast<fn_442cc0_t>(0x00442cc0)(i); }
inline void  VehRecPtr(void** o, int i) { reinterpret_cast<fn_rec_t>(0x0046d4a0)(o, i); }
#endif

// __ftol forwarded via x87 shim: FLD the value onto ST0, CALL FUN_004a2c48 (consumes ST0,
// returns the int in EAX). Guarantees the exact rounding of DAT_0089a360. (Inline asm can't
// CALL a numeric literal, so route through a pointer.)
#ifdef MASHED_STANDALONE
// [GATEFIRE GF1-FTOL] The forwarding shim cannot run standalone (unmapped .text), so
// the rounding must be RESOLVED rather than assumed — and it is load-bearing, because
// DAT_0089a360 is 2.5, exactly the truncate-vs-round-half ambiguous case:
//   truncate(2.5)        = 2  -> table index 10 -> limit 1 -> the :99 gate PASSES
//   round-half-away(2.5) = 3  -> table index 15 -> limit 0 -> the :99 gate FAILS
// Resolved from ORIGINAL-SIDE MEASUREMENT, not from a guess: RESULT_WITNESS.md:92
// records that the original's "only index is 10 (bias374 = 0, iVar1 = 2 on all 512
// calls)" while reading DAT_0089a360 = 2.5. So FUN_004a2c48 TRUNCATES here. That also
// matches Ai/AiStandalone.cpp:270's independently-established "FUN_004a2c48 truncates".
inline int Ftol(double x) { return static_cast<int>(x); }
#else
void* g_ftol_4a2c48 = reinterpret_cast<void*>(0x004a2c48);
__declspec(naked) int Ftol(double /*x*/) {
    __asm {
        fld  qword ptr [esp + 4]
        call dword ptr [g_ftol_4a2c48]
        ret
    }
}
#endif

// ===========================================================================
// [GATEFIRE GF1b 2026-10-08] EXIT-SITE recorder. GF1-FIRE measured 0 on
// 53,992/53,992 rows with GF1-REACH at 100%, i.e. the body runs and always
// returns 0. The pre-registered decision rule for that case is "report the next
// blocker rather than widen scope" — which requires knowing WHICH early return
// is taken, not guessing. g_ltExit carries the ORIGINAL's line/step id of the
// return actually executed; the gates dump logs it per car per frame.
// Standalone-only: the .asi arm keeps its C3 body byte-for-byte unchanged, so
// this cannot disturb the evidence that copy carries.
// ===========================================================================
#ifdef MASHED_STANDALONE
int g_ltExit = 0;
#define LT_EXIT(n) (g_ltExit = (n))
#else
#define LT_EXIT(n) ((void)0)
#endif

// ===========================================================================
// LeaderTimer — greenfield reimpl of FUN_004148b0. Returns 0/1.
// ===========================================================================
int LeaderTimer(int /*param_1*/, void* /*param_2*/, int* param_3, int param_4) {
    if (Gi(kMode368) == 2) { LT_EXIT(91); return 0; }
    int iVar1 = Ftol(static_cast<double>(*reinterpret_cast<float*>(kFlt360)));
    int idx364 = Gi(kIdx364);
    if (idx364 != -1 && E470(idx364) == 1) {
        iVar1 += 3;
        if (iVar1 > 10) iVar1 = 10;
    }
    const int tblIdx = Gi(kBias374) + iVar1 * 5;
#ifdef MASHED_STANDALONE
    // [GATEFIRE GF1] kLimitTbl is image .data the standalone never loads, so it reads
    // as 64 zeros (GF0-CTL measured tbl10 = 0 on 53,992/53,992 rows) and EVERY index
    // would yield limit 0 -> `0 <= RankAt(0)` -> return 0. Transcribed from
    // original/MASHED.exe.unpatched at file offset 0x1f2dd8, all 64 ints; the static
    // read reproduces the 2026-10-03 witness's LIVE Frida read to every digit (same
    // head of 16, same 14-of-64 non-zero, same tbl[10] == 1 -- RESULT_WITNESS.md:106,
    // verify/d3_gatefire_20261008/RESULT_LIMITTBL.md).
    static const int kLimitTblData[64] = {
        2, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8,
    };
    // The original indexes without a bound check; the port guards because a
    // blank-mapped bias374 could stray outside the 64 entries and this body runs in
    // a measurement build. Out-of-range reports 0, the same value the blank table
    // gave, so the guard can never manufacture a pass.
    const int limit = (tblIdx >= 0 && tblIdx < 64) ? kLimitTblData[tblIdx] : 0;
#else
    const int limit = *reinterpret_cast<int*>(kLimitTbl + (std::uintptr_t)tblIdx * 4);
#endif
    if (limit <= RankAt(param_4)) { LT_EXIT(99); return 0; }   // rank_counter >= limit

    float fVar4 = Prog(param_4);
    if (fVar4 == kZero) {
        int last = -1;
        for (int i = 0; i < 4; ++i) { if (E470(i) == 1) last = i; }
        if (last == -1) { LT_EXIT(105); return 0; }
        fVar4 = Prog(last);
        if (G_a8() < fVar4) {
            TimerAt(param_4) += Gi(kFrameDt);
            void* rec = nullptr; VehRecPtr(&rec, param_4);
            std::uintptr_t r = reinterpret_cast<std::uintptr_t>(rec);
            param_3[0] = *reinterpret_cast<int*>(r + 0x30);
            param_3[1] = *reinterpret_cast<int*>(r + 0x38);
            if (TimerAt(param_4) > 0x2710) { TimerAt(param_4) = 0; RankAt(param_4) += 1; }
            LT_EXIT(114); return 1;
        }
        if (G_a4() < fVar4 && TimerAt(param_4) != 0) TimerAt(param_4) += Gi(kFrameDt);
        int iVar1c = 1;
        if (G_a0() <= fVar4) iVar1c = 0;          // local_4 == 0
        if (G_35c() <= fVar4) {
            if (iVar1c == 1) goto LAB_reset;       // -> the 0x1194 gate
            if (iVar1c != 2) { LT_EXIT(121); return 0; }
        }
        if (TimerAt(param_4) == 0) { LT_EXIT(123); return 0; }
    } else {
    LAB_reset:
        if (TimerAt(param_4) < 0x1195) { LT_EXIT(126); return 0; }   // <= 0x1194
    }
    TimerAt(param_4) = 0;
    RankAt(param_4) += 1;
    LT_EXIT(130);
    return 0;
}

// ── Orig trampoline: the 5-byte E9 hook clobbers exactly MOV EAX,[0x0089a368] (a1 68 a3
// 89 00, 5 bytes) at 0x004148b0. Re-exec it then jmp to 0x004148b5 (SUB ESP,8). Called as
// OrigLeader(p1,p2,p3,p4); the original reads param_4 at [ESP+0x28] after its own
// SUB ESP,8 + 4 pushes, matching this call frame. ──
void* g_orig_4148b5 = reinterpret_cast<void*>(0x004148b5);
__declspec(naked) int OrigLeader(int, void*, int*, int) {
    __asm {
        // BUGFIX 2026-07-28: `ds:` added. Without it MSVC assembles this as
        // `B8 68 A3 89 00  mov eax,89A368h` -- the ADDRESS as an immediate -- instead of the
        // original's `A1 68 A3 89 00  MOV EAX,[0x0089a368]`. See AiLeader_Entry below for the
        // behavioural consequence; this trampoline had the identical defect.
        mov  eax, dword ptr ds:[0x0089a368]
        jmp  dword ptr [g_orig_4148b5]
    }
}

// ── SAFE-PASSTHROUGH hook installed at 0x004148b0 (game's own calls) ─────────
// When only this leaf is hooked (not the orchestrator driver), just run the original so
// game behavior is unchanged. The A/B verification is driven from the orchestrator entry.
__declspec(naked) void AiLeader_Entry() {
    __asm {
        // [esp]=ret [esp+4]=p1 [esp+8]=p2 [esp+0xc]=p3 [esp+0x10]=p4
        // BUGFIX 2026-07-28 — MISSING `ds:` DISABLED A MODE GATE ON A DEFAULT-INSTALLED HOOK.
        // This trampoline exists to re-execute the prologue byte-exactly. Without `ds:` MSVC
        // emits `B8 68 A3 89 00  mov eax,89A368h` (the address as an immediate) rather than
        // the original `A1 68 A3 89 00  MOV EAX,[0x0089a368]` -- it compiles clean, so nothing
        // flagged it. The very next original instructions consume EAX as a mode value:
        //     004148b5  SUB ESP,0x8
        //     004148b8  CMP EAX,0x2
        //     004148bb  JNZ 0x004148c3
        //     004148bd  XOR EAX,EAX / ADD ESP,8 / RET      <- the early-out
        // EAX held 0x0089a368 (9020776), never 2, so the JNZ always fell through and the
        // early-out was UNREACHABLE for as long as this hook was installed -- i.e. in every
        // race run to date. 64 other inline-asm sites already use `ds:`; these two did not.
        // Found by sweeping for absolute-address `__asm` operands lacking a segment override.
        mov  eax, dword ptr ds:[0x0089a368]   // re-exec clobbered prologue
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

#ifdef MASHED_STANDALONE
// [GATEFIRE GF1 2026-10-08] MEASUREMENT entry point for the standalone. LeaderTimer
// lives in this TU's anonymous namespace, so the gates probe in
// D3d9Render/TrackRenderer.cpp reaches it through this flat extern "C" wrapper.
//
// NOTHING IN THE STANDALONE'S GAME PATH CALLS THIS. It exists so GF1-REACH and
// GF1-FIRE can count the branch WITHOUT wiring it into ControlStep, which remains a
// separate registered leg (PREREG_GATEFIRE.md section 7 non-goals). The arg shape is
// the original's: param_1/param_2 are unread (AiLeaderTimer.cpp header), param_3 is
// the out-XZ pair the LOS conjunct then uses, param_4 is the vehicle index.
extern "C" int __cdecl AiLeaderTimerProbe(int* outXZ, int vehIdx) {
    g_ltExit = 0;
    return LeaderTimer(0, nullptr, outXZ, vehIdx);
}
// The exit site of the call AiLeaderTimerProbe just made.
extern "C" int __cdecl AiLeaderTimerExitSite(void) { return g_ltExit; }

// [PREREG_WIRE 2026-10-08] The FULL original signature, for the WIRED call site in
// ControlStep. FUN_004148b0(param_1 = spline, &local_24, &local_2c = outXZ, param_2 =
// vehicle) -- ctrlstep_decomp.txt:116. p1/p2 are unread by the body (this file's
// header records that), but they are passed anyway so the call site reads as the
// original's does rather than quietly dropping two arguments.
extern "C" int __cdecl AiLeaderTimerProbe2(std::uintptr_t spline, int* p2,
                                           int* outXZ, int vehIdx) {
    g_ltExit = 0;
    return LeaderTimer(static_cast<int>(spline), p2, outXZ, vehIdx);
}
#endif
