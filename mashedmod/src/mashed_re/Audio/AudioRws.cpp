// Mashed RE - RWS audio sub-struct lifecycle and format-key comparator.
//
// Four deterministic leaves from the audio_rws_loader cluster (C2->C3):
//   0x005ae080  AudioSubStructAFree  -- pool-return for sub-struct A
//   0x005ae050  AudioSubStructBFree  -- heap-free for sub-struct B
//   0x005ae030  AudioSubStructCleanup -- sequential A+B cleanup
//   0x005adf30  AudioFmtKeyCompare   -- 16-byte lexicographic memcmp
//
// All four are pure leaves or thin wrappers; no side effects beyond the
// allocator operations they explicitly invoke.
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x005ae080  FUN_005ae080 -- sub-struct A pool-return
//
// Signature: int* FUN_005ae080(int* param_1)
// param_1 points to a 12-byte sub-struct at audio_obj+0x24:
//   +0x00 (param_1[0]): data pointer (int)
//   +0x04 (param_1[1]): (unused here)
//   +0x08 (param_1[2]): flags word; bit0 = pool-owned
//
// If *param_1 != 0 AND bit0 of param_1[2] is set:
//   call FUN_005ae920(&DAT_007dda28, *param_1) to return the block to pool.
//   clear bit0 of param_1[2].
// Return param_1.
//
// Pool global: DAT_007dda28 at 0x007dda28.
// Pool-return callee: FUN_005ae920 at 0x005ae920.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int* __cdecl AudioSubStructAFree(int* param_1)
{
    // Guard: data pointer non-null AND pool-owned flag set
    if ((*param_1 != 0) && ((*(uint8_t*)(param_1 + 2) & 1u) != 0)) {
        // FUN_005ae920(&DAT_007dda28, *param_1) — return block to pool
        typedef void (__cdecl *PoolFreeFn)(void*, int);
        auto poolFree = reinterpret_cast<PoolFreeFn>(0x005ae920u);
        poolFree(reinterpret_cast<void*>(0x007dda28u), *param_1);
        // Clear bit0 of flags word at param_1[2]
        param_1[2] &= static_cast<int>(0xfffffffeu);
    }
    return param_1;
}

RH_ScopedInstall(AudioSubStructAFree, 0x005ae080);

// ---------------------------------------------------------------------------
// 0x005ae050  FUN_005ae050 -- sub-struct B heap-free
//
// Signature: int FUN_005ae050(int param_1)
// param_1 is the base address of the sub-struct at audio_obj+0x34:
//   +0x00: (unused here)
//   +0x04: data pointer (int)
//   +0x08: flags word; bit1 = heap-allocated
//
// If *(int*)(param_1+4) != 0 AND bit1 of *(uint*)(param_1+8) is set:
//   call FUN_004522d0(*(int*)(param_1+4)) to heap-free the data.
//   clear bit1 of *(uint*)(param_1+8).
// Return param_1.
//
// Heap-free callee: FUN_004522d0 at 0x004522d0 (vtable dealloc trampoline).
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int __cdecl AudioSubStructBFree(int param_1)
{
    int* const base = reinterpret_cast<int*>(static_cast<uintptr_t>(param_1));
    // Guard: data pointer at +4 non-null AND heap-owned flag set
    if ((*(int*)(base + 1) != 0) && ((*(uint8_t*)(base + 2) & 2u) != 0)) {
        // FUN_004522d0(*(int*)(param_1+4)) — heap free
        typedef void (__cdecl *HeapFreeFn)(void*);
        auto heapFree = reinterpret_cast<HeapFreeFn>(0x004522d0u);
        heapFree(reinterpret_cast<void*>(static_cast<uintptr_t>(*(base + 1))));
        // Clear bit1 of flags word at param_1+8
        *(uint32_t*)(base + 2) &= 0xfffffffdu;
    }
    return param_1;
}

RH_ScopedInstall(AudioSubStructBFree, 0x005ae050);

// ---------------------------------------------------------------------------
// 0x005ae030  FUN_005ae030 -- combined sub-struct cleanup
//
// Signature: undefined4 FUN_005ae030(undefined4 param_1)
// Sequential cleanup: calls AudioSubStructAFree(param_1) then
// AudioSubStructBFree(param_1). Returns param_1.
//
// Called by FUN_005a7ea0 (D-0347) as the first step of audio object finalization.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int __cdecl AudioSubStructCleanup(int param_1)
{
    AudioSubStructAFree(reinterpret_cast<int*>(static_cast<uintptr_t>(param_1)));
    AudioSubStructBFree(param_1);
    return param_1;
}

RH_ScopedInstall(AudioSubStructCleanup, 0x005ae030);

// ---------------------------------------------------------------------------
// 0x005adf30  FUN_005adf30 -- 16-byte format descriptor lexicographic memcmp
//
// Signature: int FUN_005adf30(byte* param_1, byte* param_2)
// Compares two 16-byte audio format keys byte-by-byte.
// Returns:
//   0  if all 16 bytes are equal (or param_1 == param_2).
//  -1  if first differing byte: *param_1 < *param_2.
//  +1  if first differing byte: *param_1 > *param_2.
//
// Functionally equivalent to a sign-normalised memcmp(param_1, param_2, 16)
// returning exactly -1/0/+1 (not arbitrary negative/positive like memcmp).
//
// Body: 0x005adf30--0x005adf55 (0x26 bytes). No callees. Pure.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int __cdecl AudioFmtKeyCompare(
        const uint8_t* param_1, const uint8_t* param_2)
{
    // Pointer-identity short-circuit (matches 0x005adf30 entry test).
    if (param_1 == param_2) return 0;

    int count = 16;
    bool equal = true;
    bool less  = false;
    while (equal && count > 0) {
        less  = (*param_1 < *param_2);
        equal = (*param_1 == *param_2);
        ++param_1;
        ++param_2;
        --count;
    }
    if (!equal) {
        // Reproduce original: (1 - (uint)bVar3) - (uint)(bVar3 != 0)
        // less==true  -> 1 - 1 - 1 = -1
        // less==false -> 1 - 0 - 0 = +1
        return less ? -1 : 1;
    }
    return 0;
}

RH_ScopedInstall(AudioFmtKeyCompare, 0x005adf30);
