// Mashed RE - c3_batch_ah s3: gameplay score-mask out-buffer family + getters.
// Bit-faithful ports matching re/analysis/bucket_gameplay_0040ad90_00412010/0x0040{...}.md
// (decomp pool2 2026-06-02). Score-mask fns read the per-player score table at
// 0x008a94e0[4] (DAT_008a94e0/e4/e8/ec) and write a 4-DWORD flag mask to the
// caller-supplied out pointer. Getters are pure-leaf global reads.
#include "../Core/HookSystem.h"
#include <cstdint>

// ── getters ────────────────────────────────────────────────────────────────

// 0x0040dc70  FUN_0040dc70  (5 bytes)  undefined4(void)
// Pure leaf getter: returns the global state register at 0x0063b90c.
extern "C" __declspec(dllexport) uint32_t __cdecl GetAnimationState(void) {
    return *reinterpret_cast<uint32_t*>(0x0063b90c);
}

RH_ScopedInstall(GetAnimationState, 0x0040dc70);  // c3_batch_ah s3

// 0x0040d430  FUN_0040d430  (5 bytes)  undefined*(void)
// Pure leaf getter: returns the pointer value stored at 0x005f2770.
extern "C" __declspec(dllexport) uint32_t __cdecl GetPointerGlobal(void) {
    return *reinterpret_cast<uint32_t*>(0x005f2770);
}

RH_ScopedInstall(GetPointerGlobal, 0x0040d430);  // c3_batch_ah s3

// 0x0040b420  FUN_0040b420  (11 bytes)  undefined4(int idx)
// Pure indexed leaf getter: returns the DWORD at 0x008a9500 + idx*4.
extern "C" __declspec(dllexport) uint32_t __cdecl IndexedTableLookup(int param_1) {
    return *reinterpret_cast<uint32_t*>(0x008a9500 + param_1 * 4);
}

RH_ScopedInstall(IndexedTableLookup, 0x0040b420);  // c3_batch_ah s3

// ── score-mask out-buffer family ─────────────────────────────────────────────
// All five take a single caller-supplied 4-DWORD out pointer (no leading index)
// and read the LIVE score table at 0x008a94e0[4]. Authored bit-faithful here;
// DEFERRED for C3 verification (no harness arg_type seeds the score globals and
// passes the out pointer as arg0 — int_outbuf4 is fn(idx,buf), wrong shape).

// FUN_0040b970 / FUN_0040ba00 / FUN_0040b9a0 / FUN_0040ba60 (the score-zero, score-min,
// score-max and slot-gate-invert masks) were ported here first (PlayerScoreZeroTest /
// PlayerScoreMinTest / PlayerScoreMaxTest / PlayerScoreGateInvert). Their one-body-per-RVA
// copies are Util/PromoLoop_round67.cpp (Bool0Out8a94e0, BoolMinOut8a94e0) and
// Util/PromoLoop_sessionB.cpp (MaxScoreFlags40b9a0, Active4Slots40ba60), the bodies
// hooks.csv's evidence cites. The four duplicates were removed 2026-10-16 (dupinstall).

// 0x0040b930  FUN_0040b930  (50 bytes)  void(undefined4* out)
// out[i] = (FUN_0040b8e0() <= score[i]) ? 1 : 0; threshold re-evaluated per i.
// Callee FUN_0040b8e0 (0x0040b8e0) called via original VA.
extern "C" __declspec(dllexport) void __cdecl PlayerScoreThresholdMask(uint32_t* param_1) {
    int iVar3 = reinterpret_cast<int>(reinterpret_cast<int*>(0x008a94e0)) - reinterpret_cast<int>(param_1);
    int iVar2 = 4;
    do {
        *param_1 = 0;
        int iVar1 = reinterpret_cast<int(__cdecl*)()>(0x0040b8e0)();
        if (iVar1 <= *reinterpret_cast<int*>(iVar3 + reinterpret_cast<int>(param_1))) *param_1 = 1;
        param_1 = param_1 + 1;
        iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
}

RH_ScopedInstall(PlayerScoreThresholdMask, 0x0040b930);  // c3_batch_ah s3

// 0x0040cd90  thunk_FUN_00425ef0  ->  int FUN_00425ef0(void)
// Counts how many of 8 record-pairs in the DAT_00899260 table have BOTH dwords
// nonzero: pair i = (base_i, hdr_i) where the 8 (hdr,base) addresses come from
// the decomp (pool14 2026-06-08). The records are 0x4C bytes apart
// (0x00899260, 0x008992ac, ...; hdr at base+0x44). Bit-faithful port — left
// unrolled to match the original's explicit 8 compares.
extern "C" __declspec(dllexport) int __cdecl CountNonZeroPairs(void) {
    int iVar1 = 0;
    auto g = [](uintptr_t a) -> uint32_t { return *reinterpret_cast<uint32_t*>(a); };
    if (g(0x008992a4) != 0 && g(0x00899260) != 0) iVar1 = 1;
    if (g(0x008992f0) != 0 && g(0x008992ac) != 0) iVar1 += 1;
    if (g(0x0089933c) != 0 && g(0x008992f8) != 0) iVar1 += 1;
    if (g(0x00899388) != 0 && g(0x00899344) != 0) iVar1 += 1;
    if (g(0x008993d4) != 0 && g(0x00899390) != 0) iVar1 += 1;
    if (g(0x00899420) != 0 && g(0x008993dc) != 0) iVar1 += 1;
    if (g(0x0089946c) != 0 && g(0x00899428) != 0) iVar1 += 1;
    if (g(0x008994b8) != 0 && g(0x00899474) != 0) iVar1 += 1;
    return iVar1;
}

RH_ScopedInstall(CountNonZeroPairs, 0x0040cd90);  // harness/scenario-attach 2026-06-08

// MATCH-PORTSTATE-GAMEFLOW-FU: the body of 0x0040b540 (PlayerScoreRankOrder) moved to Match/MatchS02_0040b540.cpp
// (byte-matched re/match/s02/0040b540.c). Its callee 0x00417740 has a body now: Match/MatchGFFU_00417740.cpp.
// The listing of 0x00417740 is mov eax,[esp+4]; fld dword [eax*4+0x0089a870]; jmp 0x004a2c48 (__ftol).
