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

// ── snapshot/restore A/B self-test (env MASHED_AI_LEADER_SELFTEST) ───────────
inline int SelfTestEnabled() {
    static int v = -1;
    if (v < 0) { const char* s = std::getenv("MASHED_AI_LEADER_SELFTEST"); v = (s && s[0]) ? 1 : 0; }
    return v;
}
void SelfTestLog(const char* s) {
    HANDLE h = CreateFileA("ai_leader_selftest.log", FILE_APPEND_DATA, FILE_SHARE_READ,
                           nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD wrote; WriteFile(h, s, (DWORD)std::strlen(s), &wrote, nullptr); CloseHandle(h);
}
long g_calls = 0, g_mismatch = 0;
const long kMaxCompare = 40000;

// Snapshot/restore A/B: FUN_004148b0 is stateful (writes the 2 per-vehicle ints + *outXZ).
// Snapshot the 2 ints, run mine (scratch out), capture, restore, run orig (scratch out),
// capture, restore to ORIGINAL so the driven call is non-perturbing, then compare
// return + both int-writes + the (return-1) output. This is the reusable orchestrator
// pattern. vehIdx = param_4.
int LeaderAB(int p1, void* p2, int vehIdx) {
    int save_t = TimerAt(vehIdx), save_r = RankAt(vehIdx);

    int mine_out[2] = {0, 0};
    int m = LeaderTimer(p1, p2, mine_out, vehIdx);
    int mine_t = TimerAt(vehIdx), mine_r = RankAt(vehIdx);

    TimerAt(vehIdx) = save_t; RankAt(vehIdx) = save_r;   // restore before orig

    int orig_out[2] = {0, 0};
    int o = OrigLeader(p1, p2, orig_out, vehIdx);
    int orig_t = TimerAt(vehIdx), orig_r = RankAt(vehIdx);

    TimerAt(vehIdx) = save_t; RankAt(vehIdx) = save_r;   // roll back — non-perturbing

    ++g_calls;
    bool outMismatch = (o == 1) && (mine_out[0] != orig_out[0] || mine_out[1] != orig_out[1]);
    if (m != o || mine_t != orig_t || mine_r != orig_r || outMismatch) {
        ++g_mismatch;
        char line[224];
        wsprintfA(line, "[%ld] MISMATCH v=%d ret o=%d m=%d  timer o=%d m=%d  rank o=%d m=%d  out o=(%08x,%08x) m=(%08x,%08x)\r\n",
                  g_calls, vehIdx, o, m, orig_t, mine_t, orig_r, mine_r,
                  orig_out[0], orig_out[1], mine_out[0], mine_out[1]);
        SelfTestLog(line);
    }
    if ((g_calls & 0xff) == 1) {
        char line[128];
        wsprintfA(line, "[%ld] calls=%ld mism=%ld %s\r\n", g_calls, g_calls, g_mismatch,
                  g_mismatch ? "" : "ALL-GREEN");
        SelfTestLog(line);
    }
    return o;
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

// ── coverage driver at the orchestrator entry FUN_00416250 (WS-R6-D) ─────────
// param_1=[esp+4], param_2(vehIdx)=[esp+8]. Drive the snapshot/restore A/B for this
// vehicle (non-perturbing), then run the original orchestrator. FUN_004148b0's real args
// are (aiLine=orch.param_1, out, out, vehIdx=orch.param_2); it ignores p1/p2, so we pass
// the orchestrator's param_1 and null scratch pointers. Only installs under
// MASHED_HOOK_ONLY=0x00416250.
void* g_orch_416256 = reinterpret_cast<void*>(0x00416256);
int g_driverP1 = 0;
__declspec(naked) void OrchLeaderCoverage_Entry() {
    __asm {
        pushad
        mov  eax, dword ptr [esp + 0x28]   // param_2 = vehIdx ([S+8]; 0x20 pushad + 8)
        push eax                            // vehIdx (3rd arg, cdecl right-to-left)
        push 0                              // p2 (unused by FUN_004148b0)
        push 0                              // p1 (unused by FUN_004148b0)
        call LeaderAB                       // LeaderAB(0, 0, vehIdx); non-perturbing
        add  esp, 0x0c
        popad
        push ebp
        mov  ebp, esp
        and  esp, 0xfffffff8
        jmp  dword ptr [g_orch_416256]
    }
}

}  // namespace

RH_ScopedInstall(AiLeader_Entry, 0x004148b0);
RH_ScopedInstall(OrchLeaderCoverage_Entry, 0x00416250);  // coverage driver (remove at orch port)
