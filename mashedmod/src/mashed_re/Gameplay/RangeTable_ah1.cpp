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
// Two getters: FUN_00407640 (Copter key->index linear scan) and FUN_004098a0
// (.LED entry array base getter).
//
// Bit-faithful ports of the verified decomp (Mashed_pool13, 2026-06-08).
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
// FUN_00426090 (0x00426090, C3): returns &DAT_0066ce58 (range-table base).
inline int FUN_00426090() {
    return reinterpret_cast<int(*)()>(0x00426090u)();
}
// Copter-array globals read by FUN_00407640 (original-image fixed addresses).
inline int& CopterCount() { return *reinterpret_cast<int*>(0x0063a5d0u); }     // DAT_0063a5d0
inline int* CopterKey0()  { return  reinterpret_cast<int*>(0x00639dc4u); }     // DAT_00639dc4 (record[0]+0x44)
// .LED entry array base returned by FUN_004098a0.
inline int* LedEntryBase() { return reinterpret_cast<int*>(0x0063a5f0u); }     // DAT_0063a5f0
}

// ---------------------------------------------------------------------------
// 0x00407640  FUN_00407640  (0x29 bytes)  int(int key)
// Linear-scan "find Copter record index by key". Returns index of first record
// whose +0x44 key field == key; -1 if none. Stride 0x3b dwords (236-byte record).
extern "C" __declspec(dllexport) int __cdecl FindCopterByKeyIndex(int param_1) {
    int iVar1 = 0;
    if (0 < CopterCount()) {
        int* piVar2 = CopterKey0();
        do {
            if (*piVar2 == param_1) {
                return iVar1;
            }
            iVar1 = iVar1 + 1;
            piVar2 = piVar2 + 0x3b;
        } while (iVar1 < CopterCount());
    }
    return -1;
}
RH_ScopedInstall(FindCopterByKeyIndex, 0x00407640);  // c3_batch_ah s1

// ---------------------------------------------------------------------------
// 0x004098a0  FUN_004098a0  (0x5 bytes)  undefined4*(void)
// Pure getter: returns &DAT_0063a5f0 (.LED entry array base).
extern "C" __declspec(dllexport) uint32_t* __cdecl GetLedEntryArrayBase(void) {
    return reinterpret_cast<uint32_t*>(LedEntryBase());
}
RH_ScopedInstall(GetLedEntryArrayBase, 0x004098a0);  // c3_batch_ah s1

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

// ---------------------------------------------------------------------------
// 0x00407b00  FUN_00407b00  (0x62 bytes)  int(int value)
// Returns the group index whose range/value set contains `value`.
extern "C" __declspec(dllexport) int __cdecl RangeTableFindGroup(int param_1) {
    int iVar4 = FUN_00426090();
    int iVar6 = 0;
    int* piVar7 = reinterpret_cast<int*>(iVar4 + 8);
    int iVar3 = 0;
    int iVar5;
    int iVar1;
    while (true) {
        while (true) {
            while (iVar5 = iVar3, iVar1 = *reinterpret_cast<int*>(iVar4 + iVar6 * 4),
                   iVar1 == -1 || (iVar1 == -10)) {
                int iVar1b = iVar6 * 4;
                iVar6 = iVar6 + 1;
                piVar7 = piVar7 + 1;
                iVar3 = iVar6;
                if (*reinterpret_cast<int*>(iVar4 + 4 + iVar1b) == -2) {
                    return iVar5;
                }
            }
            int iVar2 = *reinterpret_cast<int*>(iVar4 + 4 + iVar6 * 4);
            iVar3 = iVar5;
            if (iVar2 == 0x100) break;
            if ((iVar1 <= param_1) && (param_1 <= iVar2)) {
                return iVar5;
            }
            iVar6 = iVar6 + 1;
            piVar7 = piVar7 + 1;
        }
        if (param_1 == iVar1) {
            return iVar5;
        }
        if (param_1 == *piVar7) break;
        iVar6 = iVar6 + 2;
        piVar7 = piVar7 + 2;
    }
    return iVar5;
}
RH_ScopedInstall(RangeTableFindGroup, 0x00407b00);  // c3_batch_ah s1

// ---------------------------------------------------------------------------
// 0x00407b70  FUN_00407b70  (0x61 bytes)  undefined4(int value, int slot)
// Is `value` a member of the group beginning at slot `slot`? 1 yes / 0 no.
extern "C" __declspec(dllexport) uint32_t __cdecl RangeTableMemberTest(int param_1, int param_2) {
    int iVar1 = FUN_00426090();
    int* piVar2 = reinterpret_cast<int*>(iVar1 + param_2 * 4);
    if (*reinterpret_cast<int*>(iVar1 + param_2 * 4) != -1) {
        int* piVar3 = reinterpret_cast<int*>(iVar1 + 8 + param_2 * 4);
        do {
            iVar1 = *piVar2;
            if (iVar1 == -10) {
                return 0;
            }
            if (piVar2[1] == 0x100) {
                if ((param_1 == iVar1) || (param_1 == *piVar3)) {
                    return 1;
                }
                piVar3 = piVar3 + 2;
                piVar2 = piVar2 + 2;
            }
            else {
                if ((iVar1 <= param_1) && (param_1 <= piVar2[1])) {
                    return 1;
                }
                piVar3 = piVar3 + 1;
                piVar2 = piVar2 + 1;
            }
        } while (*piVar2 != -1);
    }
    return 0;
}
RH_ScopedInstall(RangeTableMemberTest, 0x00407b70);  // c3_batch_ah s1

// ---------------------------------------------------------------------------
// 0x00407d70  FUN_00407d70  (0x3b bytes)  int(int slot)
// Count of distinct values in the group starting at slot `slot`. Span sum + 1.
extern "C" __declspec(dllexport) int __cdecl RangeTableCountGroup(int param_1) {
    int iVar1 = FUN_00426090();
    int* piVar2 = reinterpret_cast<int*>(iVar1 + param_1 * 4);
    iVar1 = piVar2[1];
    int iVar3 = 0;
    while ((iVar1 != -10) && (iVar1 = piVar2[1], iVar1 != -1)) {
        if (iVar1 == 0x100) {
            iVar3 = iVar3 + 1;
            piVar2 = piVar2 + 2;
        }
        else {
            iVar3 = iVar3 + (iVar1 - *piVar2);
            piVar2 = piVar2 + 1;
        }
        iVar1 = piVar2[1];
    }
    return iVar3 + 1;
}
RH_ScopedInstall(RangeTableCountGroup, 0x00407d70);  // c3_batch_ah s1

// ---------------------------------------------------------------------------
// 0x00407db0  FUN_00407db0  (0x6e bytes)  int(int slot, int value)
// Maps `value` to its group-local cumulative offset within the group at `slot`.
extern "C" __declspec(dllexport) int __cdecl RangeTableGroupOffset(int param_1, int param_2) {
    int iVar3 = FUN_00426090();
    int* piVar5 = reinterpret_cast<int*>(iVar3 + param_1 * 4);
    int iVar1 = piVar5[1];
    int iVar6 = 0;
    int iVar4 = param_2;
    while ((iVar1 != -10) && (iVar1 = piVar5[1], iVar1 != -1)) {
        if (iVar1 == 0x100) {
            iVar6 = iVar6 + 1;
            piVar5 = piVar5 + 2;
        }
        else {
            int iVar2 = *piVar5;
            if (((iVar2 <= param_2) && (param_2 <= iVar1)) &&
                (*reinterpret_cast<int*>(iVar3 + 4) < iVar2)) {
                iVar4 = iVar4 + (iVar6 - iVar2);
            }
            iVar6 = iVar6 + (iVar1 - iVar2);
            piVar5 = piVar5 + 1;
        }
        iVar1 = piVar5[1];
    }
    return iVar4;
}
RH_ScopedInstall(RangeTableGroupOffset, 0x00407db0);  // c3_batch_ah s1; C2->C3 scenario-attach 2026-06-08 (12 distinct, sentinel 0x0066ce58)
