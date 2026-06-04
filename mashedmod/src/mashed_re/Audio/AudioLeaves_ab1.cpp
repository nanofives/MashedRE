// Mashed RE - Audio leaf cluster (c3_batch_ab session 1).
// Six pure-leaf audio primitives promoted C2->C3.  Every function is a pure
// transform or read-only structure walk with NO global side effects, so each is
// safe to A/B-diff via Frida (synthetic, hook-bypassed => C3 evidence).
//
// Each reimpl is a verbatim transcription of the Ghidra decompiler output read
// read-only from Mashed_pool11 on 2026-06-04 (the [C2 2026-06-01] plate text is
// reproduced above each function).  RVA + body range cited per function.
//
// The seventh candidate of this session (0x005ae4c0, intra-block first-fit
// aligned allocator) is DEFERRED: it mutates the free list and returns a
// block-relative absolute pointer, so a faithful A/B needs a pointer-relative
// fingerprint harness that does not yet exist.  Queued in re/PROMOTION_QUEUE.md.
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x005ac540  FUN_005ac540  (15 bytes, body 005ac540..005ac54f)
// Decompiler ([C2 2026-06-01]):
//   uint FUN_005ac540(int param_1) {
//     return (*(byte *)(param_1 + 0x54) & 8) >> 3;
//   }
// Returns bit 3 (mask 0x08) of the byte at param_1+0x54, normalized to 0/1.
extern "C" __declspec(dllexport) unsigned int __cdecl AudioByte54Bit3Get(int param_1) {
    return (*(unsigned char*)(param_1 + 0x54) & 8) >> 3;
}

RH_ScopedInstall(AudioByte54Bit3Get, 0x005ac540);

// 0x005ade60  FUN_005ade60  (35 bytes, body 005ade60..005ade83)
// Decompiler ([C2 2026-06-01]):
//   int FUN_005ade60(int param_1,int param_2) {
//     int iVar1, iVar2;
//     iVar2 = 0;
//     iVar1 = *(int *)(param_1 + 4);
//     while( true ) {
//       if (iVar1 == param_1) return -1;
//       if (*(int *)(iVar1 + 8) == param_2) break;
//       iVar1 = *(int *)(iVar1 + 4);
//       iVar2 = iVar2 + 1;
//     }
//     return iVar2;
//   }
// Zero-based index of the first node (walking next@+4 from head=*(anchor+4))
// whose key@+8 == param_2; -1 if the scan wraps back to the anchor.
extern "C" __declspec(dllexport) int __cdecl AudioListIndexOfKey(int param_1, int param_2) {
    int iVar2 = 0;
    int iVar1 = *(int*)(param_1 + 4);
    while (true) {
        if (iVar1 == param_1) {
            return -1;
        }
        if (*(int*)(iVar1 + 8) == param_2) break;
        iVar1 = *(int*)(iVar1 + 4);
        iVar2 = iVar2 + 1;
    }
    return iVar2;
}

RH_ScopedInstall(AudioListIndexOfKey, 0x005ade60);

// 0x005aded0  FUN_005aded0  (21 bytes, body 005aded0..005adee5)
// Decompiler ([C2 2026-06-01]):
//   int FUN_005aded0(int param_1) {
//     int iVar1, iVar2;
//     iVar2 = 0;
//     for (iVar1 = *(int *)(param_1 + 4); iVar1 != param_1; iVar1 = *(int *)(iVar1 + 4)) {
//       iVar2 = iVar2 + 1;
//     }
//     return iVar2;
//   }
// Counts nodes of the circular list anchored at param_1 (walk next@+4 from
// *(anchor+4) until the chain returns to the anchor; anchor excluded).
extern "C" __declspec(dllexport) int __cdecl AudioListNodeCount(int param_1) {
    int iVar2 = 0;
    for (int iVar1 = *(int*)(param_1 + 4); iVar1 != param_1; iVar1 = *(int*)(iVar1 + 4)) {
        iVar2 = iVar2 + 1;
    }
    return iVar2;
}

RH_ScopedInstall(AudioListNodeCount, 0x005aded0);

// 0x005ae590  FUN_005ae590  (23 bytes, body 005ae590..005ae5a7)
// Decompiler ([C2 2026-06-01]):
//   bool FUN_005ae590(int param_1) {
//     return **(int **)(param_1 + 8) == *(int *)(param_1 + 0xc);
//   }
// Arena-block "is fully free": the first free node's next pointer
// (*(*(block+8))) equals the block-end sentinel (*(block+0xc)).
extern "C" __declspec(dllexport) bool __cdecl AudioArenaBlockIsFree(int param_1) {
    return **(int**)(param_1 + 8) == *(int*)(param_1 + 0xc);
}

RH_ScopedInstall(AudioArenaBlockIsFree, 0x005ae590);

// 0x005aeda0  FUN_005aeda0  (127 bytes, body 005aeda0..005aee1f)
// Decompiler ([C2 2026-06-01]):  void FUN_005aeda0(uint p1, uint p2, uint *p3, uint *p4)
// Shift-add multiply producing a two-word (high/low) fixed-point product.
// Loops over bits of p1 (while p1 & 0xfffffffe, then one trailing iteration);
// each addend treats bit 31 of p2 as a carry into the high accumulator.
// Final pack: *p3 = hi >> 1 ; *p4 = lo | (hi << 31).
extern "C" __declspec(dllexport) void __cdecl AudioShiftAddMul64(
        unsigned int param_1, unsigned int param_2,
        unsigned int* param_3, unsigned int* param_4) {
    unsigned int uVar2 = 0;
    unsigned int uVar3 = 0;
    unsigned int uVar1 = 0;
    for (; (param_1 & 0xfffffffe) != 0; param_1 = param_1 >> 1) {
        uVar1 = uVar1 * 2;
        if ((int)param_2 < 0) {
            uVar1 = uVar1 | 1;
            param_2 = param_2 & 0x7fffffff;
        }
        if ((param_1 & 1) != 0) {
            uVar2 = uVar2 + param_2;
            uVar3 = uVar3 + uVar1;
            if ((int)uVar2 < 0) {
                uVar3 = uVar3 + 1;
                uVar2 = uVar2 & 0x7fffffff;
            }
        }
        param_2 = param_2 * 2;
    }
    uVar1 = uVar1 * 2;
    if ((int)param_2 < 0) {
        uVar1 = uVar1 | 1;
        param_2 = param_2 & 0x7fffffff;
    }
    if ((param_1 & 1) != 0) {
        uVar2 = uVar2 + param_2;
        uVar3 = uVar3 + uVar1;
        if ((int)uVar2 < 0) {
            uVar3 = uVar3 + 1;
            uVar2 = uVar2 & 0x7fffffff;
        }
    }
    *param_3 = uVar3 >> 1;
    *param_4 = uVar2 | uVar3 << 0x1f;
}

RH_ScopedInstall(AudioShiftAddMul64, 0x005aeda0);

// 0x005b0700  FUN_005b0700  (56 bytes, body 005b0700..005b0738)
// Decompiler ([C2 2026-06-01]):
//   int FUN_005b0700(int param_1,uint param_2) {
//     int iVar1, iVar3; uint uVar2, uVar4;
//     iVar3 = 0; uVar4 = 0xffffffff;
//     for (iVar1 = *(int *)(param_1 + 4); iVar1 != param_1; iVar1 = *(int *)(iVar1 + 4)) {
//       uVar2 = *(uint *)(*(int *)(*(int *)(iVar1 + 8) + 0x54) + 0x10);
//       if ((param_2 <= uVar2) && (param_2 <= uVar4)) {
//         iVar3 = *(int *)(iVar1 + 8);
//         uVar4 = uVar2;
//       }
//     }
//     return iVar3;
//   }
// Over the circular list at param_1 (links@+4), select node payload *(node+8)
// whose key *(*(payload+0x54)+0x10) >= param_2 while the running bestKey uVar4
// stays >= param_2.  Returns the selected payload, or 0 if none qualify.
extern "C" __declspec(dllexport) int __cdecl AudioListMinKeySelect(int param_1, unsigned int param_2) {
    int iVar3 = 0;
    unsigned int uVar4 = 0xffffffff;
    for (int iVar1 = *(int*)(param_1 + 4); iVar1 != param_1; iVar1 = *(int*)(iVar1 + 4)) {
        unsigned int uVar2 = *(unsigned int*)(*(int*)(*(int*)(iVar1 + 8) + 0x54) + 0x10);
        if ((param_2 <= uVar2) && (param_2 <= uVar4)) {
            iVar3 = *(int*)(iVar1 + 8);
            uVar4 = uVar2;
        }
    }
    return iVar3;
}

RH_ScopedInstall(AudioListMinKeySelect, 0x005b0700);
