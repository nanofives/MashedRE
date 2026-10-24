// Mashed RE - Audio RWS format-descriptor utility functions.
// Four pure leaf utilities in the RWS audio format-descriptor cluster.
// None have callees deeper than C2; all promote to C3 in c3-batch-f-s2.
//
// Source analysis: re/analysis/promote_c2_audio_rws/005aca80.md
//                  re/analysis/promote_c2_audio_rws/005acd10.md
//                  re/analysis/promote_c2_audio_rws/005ac980.md
//                  re/analysis/promote_c2_audio_rws/005acd60.md
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// 0x005aca80 AudioFmtSizeCalc moved to Audio/AudioRwsLeaves.cpp (audiolink AudioRws slice 1, both targets).

// ─────────────────────────────────────────────────────────────────────────────
// 0x005acd10  FUN_005acd10  AudioFmtTableSearch  (~60 bytes)
// Linear search through an audio context's format-entry array.
//
// param_1 = audio context pointer.
//   +0x24: uint32  entry count
//   +0x28: ptr     base of array-of-entry-pointers (element stride: 4 bytes)
// param_2 = candidate format-descriptor pointer (passed to match predicate).
// Returns: 1 if any entry matches or array is empty; 0 if exhausted.
//
// STRUCT GAP: audio context layout at +0x24/+0x28 is not fully documented.
//   Offsets cited from Ghidra decompilation at 0x005acd10.
//
// Callee: FUN_005ac9e0 (0x005ac9e0, C2) — per-entry match predicate.
//   Returns non-zero on match, zero on no-match.
//
// ASM key (0x005acd10..0x005acd57):
//   005acd10: MOV ECX, [param_1+0x24]   ; count
//   005acd13: TEST ECX,ECX
//   005acd15: JZ  return_1
//   005acd17: MOV ESI, [param_1+0x28]   ; array base
//   005acd1a: XOR EDI,EDI               ; i=0
//   005acd1c: TEST ECX,ECX (redundant guard)
//   005acd1e: JZ  return_0
//   loop:
//   005acd20: MOV EAX, [ESI+EDI*4]      ; entry ptr = array[i]
//   005acd23: PUSH param_2
//   005acd24: PUSH EAX
//   005acd25: CALL FUN_005ac9e0
//   005acd2a: TEST EAX,EAX
//   005acd2c: JNZ  return_1
//   005acd2e: INC EDI
//   005acd2f: CMP EDI,ECX
//   005acd31: JB   loop
//   return_0: XOR EAX,EAX; RET
//   return_1: MOV EAX,1;   RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl AudioFmtTableSearch(int param_1, int param_2) {
    typedef int (__cdecl *MatchFn)(int entry_ptr, int desc_ptr);
    static const auto matchPredicate = reinterpret_cast<MatchFn>(0x005ac9e0u);

    const uint32_t count = *reinterpret_cast<uint32_t*>(param_1 + 0x24);
    if (count == 0u) {
        return 1;  // empty array: trivially "matches" (any format accepted)
    }
    const int array_base = *reinterpret_cast<int*>(param_1 + 0x28);
    // Compiler artifact: redundant guard on count == 0 is not reproduced
    for (uint32_t i = 0u; i < count; ++i) {
        const int entry_ptr = *reinterpret_cast<int*>(array_base + static_cast<int>(i) * 4);
        if (matchPredicate(entry_ptr, param_2) != 0) {
            return 1;  // match found
        }
    }
    return 0;  // exhausted with no match
}

RH_ScopedInstall(AudioFmtTableSearch, 0x005acd10);  // re-enabled 2026-05-24 phase-a1 fmt_table_search (buffer-alloc fix) GREEN

// 0x005ac980 AudioFmtDescCopy moved to Audio/AudioRwsLeaves.cpp (audiolink AudioRws slice 1, both targets).

// ─────────────────────────────────────────────────────────────────────────────
// 0x005acd60  FUN_005acd60  AudioFmtGlobalScan  (~50 bytes)
// Scans the fixed 9-entry global format-table pointer array at 0x00633674.
//
// param_1 = format key to search for (undefined4; treated as pointer by callee).
// Returns: pointer to matching table entry, or NULL if not found.
//
// Global: PTR_DAT_00633674 (0x00633674) — base of 9-entry pointer array.
// Callee: FUN_005adf30 (0x005adf30, C2) — 16-byte format-key compare;
//         returns non-zero on mismatch, zero on match.
//
// ASM key (0x005acd60..0x005acd9b):
//   005acd60: XOR EDI,EDI            ; i=0
//   loop:
//   005acd62: MOV EAX, [0x633674+EDI*4] ; entry_ptr = table[i]
//   005acd69: PUSH param_1
//   005acd6a: PUSH EAX
//   005acd6b: CALL FUN_005adf30      ; compare key
//   005acd70: TEST EAX,EAX
//   005acd72: JZ   found             ; zero = match
//   005acd74: INC EDI
//   005acd75: CMP EDI,9
//   005acd78: JB   loop
//   005acd7a: XOR EAX,EAX; RET      ; not found → NULL
//   found:
//   005acd7e: MOV EAX,[0x633674+EDI*4]; return entry ptr
//   005acd84: RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void* __cdecl AudioFmtGlobalScan(uint32_t param_1) {
    typedef int (__cdecl *CmpFn)(int entry_ptr, uint32_t key);
    static const auto fmtKeyCmp = reinterpret_cast<CmpFn>(0x005adf30u);

    static constexpr uintptr_t kFmtTableBase = 0x00633674u;  // PTR_DAT_00633674
    static constexpr uint32_t  kEntryCount   = 9u;

    for (uint32_t i = 0u; i < kEntryCount; ++i) {
        const int entry_ptr =
            *reinterpret_cast<int*>(kFmtTableBase + i * 4u);
        if (fmtKeyCmp(entry_ptr, param_1) == 0) {
            // Match: return pointer to this table entry
            return reinterpret_cast<void*>(
                *reinterpret_cast<uintptr_t*>(kFmtTableBase + i * 4u));
        }
    }
    return nullptr;  // not found
}

RH_ScopedInstall(AudioFmtGlobalScan, 0x005acd60);  // re-enabled 2026-05-24 c3-audio-a

// ─────────────────────────────────────────────────────────────────────────────
// c3-batch-f-s3 — RWS audio sub-struct lifecycle + format-key comparator.
// (Requires <cstring> for AudioFmtKeyCompare; declared above where needed.)
// ─────────────────────────────────────────────────────────────────────────────
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

RH_ScopedInstall(AudioSubStructAFree, 0x005ae080);  // re-enabled 2026-05-24 c3-audio-b

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

RH_ScopedInstall(AudioSubStructBFree, 0x005ae050);  // re-enabled 2026-05-24 c3-audio-b

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

RH_ScopedInstall(AudioSubStructCleanup, 0x005ae030);  // re-enabled 2026-05-24 c3-audio-b

// ---------------------------------------------------------------------------
// 0x005a9e10  FUN_005a9e10  (0x21 bytes)
// AudioSubStructTwoCallInit — two-call dispatcher; returns param_1.
//
// Decompilation (0x005a9e10–0x005a9e31):
//   FUN_005adfe0(param_1, param_3);   // sub-struct B init at +0x34 (C2)
//   FUN_005ae010(param_1, param_2);   // sub-struct A init at +0x24 (S-0345)
//   return param_1;
//
// No local state; pure pass-through wrapper.
// [U-0351]: Exact semantic role of param_2/param_3 not confirmed from this
//            function alone; confirmed from callee analysis notes.
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) void* __cdecl AudioSubStructTwoCallInit(
        void* param_1,
        void* param_2,
        void* param_3)
{
    // 0x005a9e13: FUN_005adfe0(param_1, param_3) — sub-struct B initializer
    typedef void (__cdecl *SubBFn)(void*, void*);
    auto sub_b = reinterpret_cast<SubBFn>(0x005adfe0u);
    sub_b(param_1, param_3);

    // 0x005a9e1f: FUN_005ae010(param_1, param_2) — sub-struct A initializer
    typedef void (__cdecl *SubAFn)(void*, void*);
    auto sub_a = reinterpret_cast<SubAFn>(0x005ae010u);
    sub_a(param_1, param_2);

    // 0x005a9e2c: return param_1 (pass-through)
    return param_1;
}

RH_ScopedInstall(AudioSubStructTwoCallInit, 0x005a9e10);  // re-enabled 2026-05-24 c3-audio-a

// 0x005adf30 AudioFmtKeyCompare moved to Audio/AudioFmtKey.cpp (audiolink slice 4, one body in both targets).
// 0x005ae010  FUN_005ae010  AudioSubStructLinkDevice  (31 bytes)
// Signature: undefined4* FUN_005ae010(undefined4 *param_1, undefined4 param_2)
//
// Links sub-struct field [0] with a new device handle.
// Step 1: FUN_005ae080(param_1) — if *param_1 != 0 and bit0(param_1[2]) set,
//         returns old handle to pool DAT_007dda28 and clears bit0(param_1[2]).
//         (no-op when *param_1 == 0, i.e. fresh/zeroed struct)
// Step 2: *param_1 = param_2   — store new handle at field [0].
// Step 3: param_1[2] &= 0xfffffffe  — clear bit0 (not-owned flag).
// Returns: param_1.
//
// ASM constants:
//   0x005ae027: AND mask 0xfffffffe  (clears bit 0)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
AudioSubStructLinkDevice(std::uint32_t* param_1, std::uint32_t param_2)
{
    // Callee: releases old device handle if owned (no-op when *param_1 == 0).
    typedef void (__cdecl *CleanFn)(std::uint32_t*);
    reinterpret_cast<CleanFn>(0x005ae080u)(param_1);

    param_1[0]  = param_2;
    param_1[2] &= 0xfffffffeu;   // 0x005ae027: clear bit 0 (not-owned)
    return param_1;
}

RH_ScopedInstall(AudioSubStructLinkDevice, 0x005ae010);  // re-enabled 2026-05-24 phase-a1 audio_sub_struct_link GREEN

// ─────────────────────────────────────────────────────────────────────────────
// 0x005adfe0  FUN_005adfe0  AudioSubStructLinkBuffer  (32 bytes)
// Signature: int FUN_005adfe0(int param_1, undefined4 param_2)
//
// Links sub-struct field at +4 with a new buffer pointer.
// Step 1: FUN_005ae050(param_1) — if *(param_1+4) != 0 and bit1(*(param_1+8))
//         set, frees old buffer via FUN_004522d0 and clears bit1.
//         (no-op when *(param_1+4) == 0)
// Step 2: *(param_1+4) = param_2  — store new buffer ptr at field +4.
// Step 3: *(param_1+8) &= 0xfffffffd  — clear bit1 (not-owned flag).
// Returns: param_1.
//
// ASM constants:
//   0x005adffe: AND mask 0xfffffffd  (clears bit 1)
//
// STRUCT GAP: sub-struct layout confirmed at offsets +0, +4, +8.
//   The relationship between this sub-struct and the parent audio object is
//   unknown at this level. See U-0143.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
AudioSubStructLinkBuffer(std::uint32_t* param_1, std::uint32_t param_2)
{
    // Callee: frees old buffer if owned (no-op when *(param_1+4) == 0).
    typedef void (__cdecl *CleanFn)(std::uint32_t*);
    reinterpret_cast<CleanFn>(0x005ae050u)(param_1);

    param_1[1]  = param_2;
    param_1[2] &= 0xfffffffdu;   // 0x005adffe: clear bit 1 (not-owned)
    return param_1;
}

RH_ScopedInstall(AudioSubStructLinkBuffer, 0x005adfe0);  // re-enabled 2026-05-24 phase-a1 audio_sub_struct_link GREEN

// 0x005ae0b0 AudioSubStructZeroInit moved to Audio/AudioRwsLeaves.cpp (audiolink AudioRws slice 1, both targets).

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac7b0  FUN_005ac7b0  AudioSubStructDualInit  (~50 bytes)
// Signature: uint FUN_005ac7b0(uint param_1, undefined4 param_2, undefined4 param_3)
//
// Thin two-step initializer that delegates to AudioSubStructLinkDevice and
// AudioSubStructLinkBuffer; returns param_1 if both succeed, 0 if either fails.
//
// Decompilation:
//   iVar1 = FUN_005ae010(param_1, param_2);
//   if (iVar1 == 0) { return 0; }
//   iVar1 = FUN_005adfe0(param_1, param_3);
//   return -(uint)(iVar1 != 0) & param_1;
//
// The expression `-(uint)(iVar1 != 0) & param_1` is the compiler's idiom for:
//   return (iVar1 != 0) ? param_1 : 0;
//
// No direct struct accesses; all field writes delegated to callees above.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl
AudioSubStructDualInit(std::uint32_t param_1,
                       std::uint32_t param_2,
                       std::uint32_t param_3)
{
    typedef std::uint32_t* (__cdecl *LinkFn)(std::uint32_t*, std::uint32_t);
    auto* const linkDevice = reinterpret_cast<LinkFn>(0x005ae010u);
    auto* const linkBuffer = reinterpret_cast<LinkFn>(0x005adfe0u);

    const std::uint32_t* r1 = linkDevice(
        reinterpret_cast<std::uint32_t*>(param_1), param_2);
    if (r1 == nullptr) return 0u;

    const std::uint32_t* r2 = linkBuffer(
        reinterpret_cast<std::uint32_t*>(param_1), param_3);
    return (r2 != nullptr) ? param_1 : 0u;
}

RH_ScopedInstall(AudioSubStructDualInit, 0x005ac7b0);  // re-enabled 2026-05-24 phase-a1 audio_sub_struct_dual GREEN
// 0x005ae800 AudioPoolBlockAlloc moved to Audio/AudioPool.cpp (D4-AUDIO-RWS slice 2, both targets).

// 0x005ae780 AudioPoolDestroy moved to Audio/AudioPool.cpp (D4-AUDIO-RWS slice 2, both targets).

// ---------------------------------------------------------------------------
// 0x005aeca0  FUN_005aeca0 — endian-swap field packer
//
// Writes 1, 2, or 4 bytes from *param_2 into the output buffer at **param_1,
// performing big→little endian byte reversal for 2-byte (bswap16) and 4-byte
// (bswap32) fields.  Advances *param_1 by the field size after each write.
//
// param_3 == 1: copy 1 byte verbatim.
// param_3 == 2: write 2 bytes in big-endian order (bytes[0]=low, bytes[1]=high).
// param_3 == 4: write 4 bytes bswap32.
//
// Decompilation: Ghidra 0x005aeca0..0x005aecff
// 0x005aeca0 AudioFieldEndianPack moved to Audio/AudioRwsLeaves.cpp (audiolink AudioRws slice 1, both targets).

// ---------------------------------------------------------------------------
// 0x005ae0c0  FUN_005ae0c0 — WAVEFORMATEX-like 16-byte format copy
//
// Copies a 16-byte audio format descriptor from param_1 to param_2.
// param_3 controls endian swapping:
//   param_3 == NULL: direct 4-DWORD copy (param_2[0..3] = param_1[0..3]).
//   param_3 != NULL: byte-swap copy through a 4-DWORD stack buffer:
//     AudioFieldEndianPack(&ptr, param_1+0, 4)   — first DWORD
//     AudioFieldEndianPack(&ptr, param_1+1, 2)   — bytes 4-5 (nSamplesPerSec lo)
//     AudioFieldEndianPack(&ptr, (int)param_1+6, 2) — bytes 6-7
//     direct copy: local_buf[2..3] = param_1[2..3]
//     param_2[0..3] = local_buf[0..3]
//
// Returns param_1.
//
// Constants cited from Ghidra at 0x005ae0e3 (4), 0x005ae0ee (2), 0x005ae0fa (2).
// 0x005ae0c0 AudioWaveFmtCopy moved to Audio/AudioRwsLeaves.cpp (audiolink AudioRws slice 1, both targets).

// 0x005ae920 AudioPoolFree moved to Audio/AudioPool.cpp (D4-AUDIO-RWS slice 2, both targets).

// 0x005addd0 AudioListInsertHead, 0x005ade10 AudioListRemoveByValue, 0x005ade90 AudioListDrain
// moved to Audio/AudioPool.cpp (D4-AUDIO-RWS slice 2, both targets).

// ─────────────────────────────────────────────────────────────────────────────
// 0x005abcb0  AudioWaveNodeFree
// void FUN_005abcb0(int param_1)
//
// Frees or returns a wave_node to its pool depending on flags at +0x54:
//   bit0 = 1  → pool-return mode: return to per-context pool or global pool
//   bit0 = 0, bit2 = 0  → heap-free: call FUN_004522d0(param_1)
//   bit0 = 0, bit2 = 1  → externally owned / static: do nothing
//
// Memory accesses:
//   param_1 + 0x54  u32  flags
//   param_1 + 0x0c  u32  context pointer (parent struct ptr)
//   *(param_1+0x0c) + 0x3c  u32  per-context pool handle
//
// Constants:
//   0x1  = flags bit0: pool-return mode     (inlined)
//   0x4  = flags bit2: no-free mode         (inlined)
//   0x007dd634 = global wave_node free pool
//
// Callees:
//   FUN_004522d0  0x004522d0  heap free (operator delete / free wrapper)
//   FUN_005ae920  0x005ae920  return node to pool
//
// U-0994: FUN_004522d0 call in heap-free branch: decompiler shows no argument
//   (calling convention artefact); param_1 is the wave_node pointer, passed
//   as the free() argument by the original code's calling convention.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioWaveNodeFree(int param_1)
{
    typedef void (__cdecl *HeapFreeFn)(void *);
    typedef void (__cdecl *PoolReturnFn)(int, int);

    static constexpr std::uintptr_t kHeapFree   = 0x004522d0u;
    static constexpr std::uintptr_t kPoolReturn  = 0x005ae920u;
    static constexpr std::uintptr_t kGlobalPool  = 0x007dd634u;

    const auto heapFree   = reinterpret_cast<HeapFreeFn>(kHeapFree);
    const auto poolReturn = reinterpret_cast<PoolReturnFn>(kPoolReturn);

    const std::uint32_t flags = *reinterpret_cast<std::uint32_t *>(param_1 + 0x54);

    if (flags & 0x1u) {
        // bit0 set — pool-return path
        const int contextPtr = *reinterpret_cast<int *>(param_1 + 0x0c);
        const int perCtxPool = *reinterpret_cast<int *>(contextPtr + 0x3c);
        if (perCtxPool == 0) {
            poolReturn(static_cast<int>(kGlobalPool), param_1);
        } else {
            poolReturn(perCtxPool, param_1);
        }
        return;
    }

    if (flags & 0x4u) {
        // bit2 set — externally owned / static; do nothing
        return;
    }

    // bit0 = 0, bit2 = 0 — heap-free
    heapFree(reinterpret_cast<void *>(param_1));
}

RH_ScopedInstall(AudioWaveNodeFree, 0x005abcb0);  // re-enabled 2026-05-24 c3-audio-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac740  AudioSubStructBufCleanup
// void FUN_005ac740(int param_1)
//
// Cleans up an audio sub-struct's buffer:
//   - If bit1 of flags byte at +0x18 is clear AND buffer ptr at +0x10 is non-null:
//       call FUN_004522d0 to free the buffer
//   - Always zero +0x10 (buffer ptr) and +0x14 (buffer size)
//
// Pass param_1 = wave_node + 0x10 or wave_node + 0x2c.
//
// Memory accesses:
//   param_1 + 0x18  u8   flags byte (bit1 = buffer-not-owned)
//   param_1 + 0x10  u32  buffer pointer (read/write)
//   param_1 + 0x14  u32  buffer size (write, zeroed)
//
// Constants:
//   0x2  bit1 mask: buffer is borrowed (not owned)
//
// Callees:
//   FUN_004522d0  0x004522d0  heap free
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioSubStructBufCleanup(int param_1)
{
    typedef void (__cdecl *HeapFreeFn)(void *);
    static constexpr std::uintptr_t kHeapFree = 0x004522d0u;
    const auto heapFree = reinterpret_cast<HeapFreeFn>(kHeapFree);

    const std::uint8_t flags = *reinterpret_cast<std::uint8_t *>(param_1 + 0x18);
    const int bufPtr = *reinterpret_cast<int *>(param_1 + 0x10);

    if ((flags & 0x2u) == 0 && bufPtr != 0) {
        heapFree(reinterpret_cast<void *>(bufPtr));
    }

    *reinterpret_cast<std::uint32_t *>(param_1 + 0x10) = 0u;
    *reinterpret_cast<std::uint32_t *>(param_1 + 0x14) = 0u;
}

RH_ScopedInstall(AudioSubStructBufCleanup, 0x005ac740);  // re-enabled 2026-05-24 c3-audio-a

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac900  AudioContextLookup
// undefined4 FUN_005ac900(undefined4 param_1)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn). Disasm 0x005ac900..0x005ac92f:
//   0x005ac900  sub esp,8                    ; ctx[2] at [esp], [esp+4]
//   0x005ac917  mov [esp+0x14],eax           ; ctx[0] = param_1
//   0x005ac91b  mov [esp+0x18],0             ; ctx[1] = 0
//   0x005ac923  call 0x005aa0c0              ; (0, 0, 0x005ac930, &ctx[0], 1), cdecl
//   0x005ac928  mov eax,[esp+0x18]           ; return ctx[1]
// The two dwords are ADJACENT: predicate 0x005ac930 reads ctx[0] ([ebp], 0x005ac946)
// and on a match writes ctx[1] ([ebp+4] = entry-0x40, 0x005ac96a). The old port kept
// them as two unrelated locals, so the compiler placed ctx[0] in the arg slot, the
// predicate's ctx[1] store landed in the caller's frame, and the port returned 0.
//
// Callees:
//   FUN_005aa0c0  0x005aa0c0  recursive tree-walk predicate search
//   LAB_005ac930  0x005ac930  predicate (U-1730), passed by address, never called here
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioContextLookup(std::uint32_t param_1)
{
    typedef void (__cdecl *TreeWalkFn)(std::uint32_t, std::uint32_t, void *, void *, int);
    static constexpr std::uintptr_t kTreeWalk = 0x005aa0c0u;
    static constexpr std::uintptr_t kCallback = 0x005ac930u;   // LAB_005ac930

    const auto treeWalk = reinterpret_cast<TreeWalkFn>(kTreeWalk);

    std::uint32_t ctx[2];
    ctx[0] = param_1;   // 0x005ac917
    ctx[1] = 0u;        // 0x005ac91b

    treeWalk(0u, 0u, reinterpret_cast<void *>(kCallback), ctx, 1);   // 0x005ac923

    return ctx[1];      // 0x005ac928
}

RH_ScopedInstall(AudioContextLookup, 0x005ac900);  // re-enabled 2026-05-24 c3-audio-a

// 0x005ae650 AudioPoolConstruct moved to Audio/AudioPool.cpp (D4-AUDIO-RWS slice 2, both targets).

// 0x005be190 AudioRwsSubZeroInit moved to Audio/AudioRwsLeaves.cpp (audiolink AudioRws slice 1, both targets).

// 0x005be140 AudioSubStructThreeWrite moved to Audio/AudioRwsLeaves.cpp (audiolink AudioRws slice 1, both targets).

// NOTE: 0x005ab410 (AudioRwsChunkTypeSeek) was already implemented in
// Audio/RwsStream.cpp (c3-batch-i-s1). No duplicate here.
// hooks.csv row for 005ab410 is drift-staged at C2; re-classify in this session
// promotes it to C3 using the existing AudioRwsChunkTypeSeek implementation.

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ad540  FUN_005ad540  AudioSubStructLinkBoth  (33 bytes)
// Signature: undefined4 FUN_005ad540(undefined4 param_1, undefined4 param_2,
//                                    undefined4 param_3)
//
// Thin wrapper that applies two independent attribute setters to sub-struct
// param_1, unconditionally:
//   1. AudioSubStructLinkDevice(param_1, param_2)  — links device handle at [0]
//      (FUN_005ae010 @ 0x005ae010)
//   2. AudioSubStructLinkBuffer(param_1, param_3)  — links buffer ptr at +4
//      (FUN_005adfe0 @ 0x005adfe0)
// Returns param_1 unconditionally (unlike AudioSubStructDualInit @ 0x005ac7b0
// which returns 0 if either call returns null).
//
// Analysis note: re/analysis/bucket_audio_005ab710_005af040/0x005ad540.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl
AudioSubStructLinkBoth(std::uint32_t param_1,
                       std::uint32_t param_2,
                       std::uint32_t param_3)
{
    typedef std::uint32_t* (__cdecl *LinkFn)(std::uint32_t*, std::uint32_t);
    auto* const linkDevice = reinterpret_cast<LinkFn>(0x005ae010u);  // AudioSubStructLinkDevice
    auto* const linkBuffer = reinterpret_cast<LinkFn>(0x005adfe0u);  // AudioSubStructLinkBuffer

    linkDevice(reinterpret_cast<std::uint32_t*>(param_1), param_2);
    linkBuffer(reinterpret_cast<std::uint32_t*>(param_1), param_3);
    return param_1;
}

RH_ScopedInstall(AudioSubStructLinkBoth, 0x005ad540);  // wf_b0f68acd r42 C2->C3

