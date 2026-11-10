// Mashed RE - c3_batch_ah s1: Gameplay range-table walker family + getters.
//
// Six of these (FUN_00407b00/b70/d70/db0, FUN_00409300/09330) walk a shared
// sentinel-encoded flat int range-table whose base is returned by FUN_00426090
// (= &DAT_0066ce58, a static .bss buffer populated at runtime during track load).
// Table marker layout (see re/analysis/.../00407b00.md):
//   lo == -1   : end of a group
//   lo == -10  : skip/blocked marker (group boundary)
//   hi == 0x100: single-value pair (lo, next_slot), consumes 2 slots
//   hi == -2   : terminate the whole walk
//   otherwise  : inclusive range [lo, hi]
//
// FUN_00407640 (Copter key->index linear scan) and FUN_004098a0 (.LED entry array
// base getter) were ported here first; their one-body-per-RVA copies live in
// Util/PromoLoop_round69.cpp (CopterFind639dc4) and Util/PromoLoop_round40.cpp
// (Ret63a5f0), the bodies hooks.csv's evidence cites. The duplicates that sat here
// (FindCopterByKeyIndex, GetLedEntryArrayBase) were removed 2026-10-16 (dupinstall).
//
// Bit-faithful ports of the verified decomp (Mashed_pool13, 2026-06-08).
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include <cstdint>

// Since D4-DC-INC-ai (2026-11-01) this TU is in BOTH exe_sources.rsp and asi_sources.rsp: FUN_00407b00/b70/d70/db0
// here are the one body each (Race/PathProgStep_naked.inc's PP_ copies were deleted, pathprog_p2.json maps the
// calls here). The callee below is the only line that differs by target: the .asi calls the original, the exe
// calls its verbatim copy of it (the original's VA holds nothing in the standalone).
#ifdef MASHED_STANDALONE
extern "C" void GlobalDat0066ce58Get();   // Match/MatchS06_00426090.cpp (MATCH-S06: the byte-matched FUN_00426090, replaces the naked PP_426090)
#endif

namespace {
// FUN_00426090 (0x00426090, C3): returns &DAT_0066ce58 (range-table base).
inline int FUN_00426090() {
#ifdef MASHED_STANDALONE
    return reinterpret_cast<int(*)()>(&GlobalDat0066ce58Get)();
#else
    return reinterpret_cast<int(*)()>(0x00426090u)();
#endif
}
}

// ---------------------------------------------------------------------------
// 0x00409300  FUN_00409300  (0x2f bytes)  int(int group)
// Returns the first int of the `group`-th group in the shared range-table.
// Skips `group` groups by walking to each -1/-10 boundary.
extern "C" __declspec(dllexport) int __cdecl RangeTableFirstOfGroup(int param_1) {
    int* piVar3 = reinterpret_cast<int*>(FUN_00426090());
    if (0 < param_1) {
        do {
            int iVar2 = *piVar3;
            while ((iVar2 != -1) && (iVar2 != -10)) {
                int* piVar1 = piVar3 + 1;
                piVar3 = piVar3 + 1;
                iVar2 = *piVar1;
            }
            piVar3 = piVar3 + 1;
            param_1 = param_1 + -1;
        } while (param_1 != 0);
    }
    return *piVar3;
}
RH_ScopedInstall(RangeTableFirstOfGroup, 0x00409300);  // c3_batch_ah s1

// ---------------------------------------------------------------------------
// 0x00409330  FUN_00409330  (0x77 bytes)  int(int value, int group)
// Successor of `value` within group ordinal `group`. After skipping `group`
// boundaries: returns value+1 inside a range, the paired value for a matched
// single-value entry, or the group's first slot at end.
extern "C" __declspec(dllexport) int __cdecl RangeTableSuccessorByGroup(int param_1, int param_2) {
    int* piVar2 = reinterpret_cast<int*>(FUN_00426090());
    if (0 < param_2) {
        do {
            int iVar1 = *piVar2;
            while ((iVar1 != -1) && (iVar1 != -10)) {
                int* piVar3 = piVar2 + 1;
                piVar2 = piVar2 + 1;
                iVar1 = *piVar3;
            }
            piVar2 = piVar2 + 1;
            param_2 = param_2 + -1;
        } while (param_2 != 0);
    }
    int iVar1 = *piVar2;
    int* piVar3 = piVar2;
    while (true) {
        if ((iVar1 == -1) || (iVar1 == -10)) {
            return *piVar2;
        }
        if (piVar3[1] == 0x100) {
            if (param_1 == iVar1) {
                return piVar3[2];
            }
        }
        else if ((iVar1 <= param_1) && (param_1 < piVar3[1])) {
            return param_1 + 1;
        }
        iVar1 = piVar3[1];
        piVar3 = piVar3 + 1;
    }
}
RH_ScopedInstall(RangeTableSuccessorByGroup, 0x00409330);  // c3_batch_ah s1

// PORT-BATCH-GAMEPLAY (D4, L-107 (a), 2026-11-09): the hand bodies of 0x00407b00 RangeTableFindGroup (with its
// TT-11 shadow A/B entry), 0x00407b70 RangeTableMemberTest, 0x00407d70 RangeTableCountGroup and 0x00407db0
// RangeTableGroupOffset were replaced by their byte-matched sources (re/match/s01/<rva>.c), compiled in both
// targets by Match/PortBatch_GAMEPLAY_<rva>.cpp under the same exported names.

// ---------------------------------------------------------------------------
// 0x00407a00  FUN_00407a00  (0x17 bytes)  undefined4(int row, int col)
// 2-D accessor over the flat per-AI dword table at &DAT_008a9914 (= per-AI row
// base 0x008a9620 + 0x2f4, the checkpoint hit-flag block). Row stride 0xc3 dwords.
//   return *(u32*)(&DAT_008a9914 + (row*0xc3 + col)*4);
// Bit-faithful port of verified pool7 decomp.
extern "C" __declspec(dllexport) uint32_t __cdecl ReadAiCheckpointField(int param_1, int param_2) {
    return *reinterpret_cast<uint32_t*>(0x008a9914u + (param_1 * 0xc3 + param_2) * 4);
}
RH_ScopedInstall(ReadAiCheckpointField, 0x00407a00);  // c3_batch_ah s2; C2->C3 scenario-attach 2026-06-08 (int_pair, 2 distinct, sentinel 0x008a9914)

// ---------------------------------------------------------------------------
// FUN_00407a20 GetAiLapCounter: MATCH-TOPDOWN-T13 moved the body to Match/MatchS01_00407a20.cpp,
// which compiles the byte-matched source re/match/s01/00407a20.c (one body per RVA, both targets).
