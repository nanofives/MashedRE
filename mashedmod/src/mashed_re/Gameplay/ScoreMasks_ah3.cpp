// Mashed RE - c3_batch_ah s3: gameplay score-mask out-buffer family + getters.
// Bit-faithful ports matching re/analysis/bucket_gameplay_0040ad90_00412010/0x0040{...}.md
// (decomp pool2 2026-06-02). Score-mask fns read the per-player score table at
// 0x008a94e0[4] (DAT_008a94e0/e4/e8/ec) and write a 4-DWORD flag mask to the
// caller-supplied out pointer. Getters are pure-leaf global reads.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x0046c7b0 VehicleSlotGetter — C4 port in Vehicle/VehicleState.cpp; call it
// directly instead of trampolining to the raw original RVA (account2 rewire).
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);

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

// 0x0040b970  FUN_0040b970  (40 bytes)  void(undefined4* out)
// For each of 4 players: out[i] = (score[i] == 0) ? 1 : 0.
extern "C" __declspec(dllexport) void __cdecl PlayerScoreZeroTest(uint32_t* param_1) {
    int iVar1 = reinterpret_cast<int>(&*reinterpret_cast<uint32_t*>(0x008a94e0)) - reinterpret_cast<int>(param_1);
    int iVar2 = 4;
    do {
        *param_1 = 0;
        if (*reinterpret_cast<int*>(iVar1 + reinterpret_cast<int>(param_1)) == 0) *param_1 = 1;
        param_1 = param_1 + 1;
        iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
}

RH_ScopedInstall(PlayerScoreZeroTest, 0x0040b970);  // c3_batch_ah s3

// 0x0040ba00  FUN_0040ba00  (90 bytes)  void(undefined4* out)
// min = min(99, score[0..3]); out[i] = (score[i] == min) ? 1 : 0.
extern "C" __declspec(dllexport) void __cdecl PlayerScoreMinTest(uint32_t* param_1) {
    int* score = reinterpret_cast<int*>(0x008a94e0);
    int iVar1 = 99;
    if (score[0] < iVar1) iVar1 = score[0];
    if (score[1] < iVar1) iVar1 = score[1];
    if (score[2] < iVar1) iVar1 = score[2];
    if (score[3] < iVar1) iVar1 = score[3];
    int base = reinterpret_cast<int>(score) - reinterpret_cast<int>(param_1);
    int iVar2 = 4;
    do {
        *param_1 = (*reinterpret_cast<int*>(base + reinterpret_cast<int>(param_1)) == iVar1) ? 1 : 0;
        param_1 = param_1 + 1;
        iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
}

RH_ScopedInstall(PlayerScoreMinTest, 0x0040ba00);  // c3_batch_ah s3

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

// 0x0040b9a0  FUN_0040b9a0  (91 bytes)  void(undefined4* out, int sel)
// max over selected players (sel==0: all; else only FUN_0046c7b0(i)!=0);
// out[i] = (score[i] == max) ? 1 : 0.
extern "C" __declspec(dllexport) void __cdecl PlayerScoreMaxTest(uint32_t* param_1, int param_2) {
    int* score = reinterpret_cast<int*>(0x008a94e0);
    int iVar3 = 0;
    for (int iVar2 = 0; iVar2 < 4; iVar2++) {
        bool take;
        if (param_2 == 0) {
            take = true;
        } else {
            take = (VehicleSlotGetter(iVar2) != 0);
        }
        if (take && iVar3 < score[iVar2]) iVar3 = score[iVar2];
    }
    int base = reinterpret_cast<int>(score) - reinterpret_cast<int>(param_1);
    int iVar2 = 4;
    do {
        *param_1 = (*reinterpret_cast<int*>(base + reinterpret_cast<int>(param_1)) == iVar3) ? 1 : 0;
        param_1 = param_1 + 1;
        iVar2 = iVar2 + -1;
    } while (iVar2 != 0);
}

RH_ScopedInstall(PlayerScoreMaxTest, 0x0040b9a0);  // c3_batch_ah s3

// 0x0040ba60  FUN_0040ba60  (45 bytes)  void(int out)
// out[i] = (FUN_0046c7b0(i) == 0) ? 1 : 0  (complement of the b9a0 gate).
extern "C" __declspec(dllexport) void __cdecl PlayerScoreGateInvert(int param_1) {
    int iVar2 = 0;
    do {
        *reinterpret_cast<uint32_t*>(param_1 + iVar2 * 4) = 0;
        std::uint32_t iVar1 = VehicleSlotGetter(iVar2);
        if (iVar1 == 0) *reinterpret_cast<uint32_t*>(param_1 + iVar2 * 4) = 1;
        iVar2 = iVar2 + 1;
    } while (iVar2 < 4);
}

RH_ScopedInstall(PlayerScoreGateInvert, 0x0040ba60);  // c3_batch_ah s3

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
