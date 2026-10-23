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

// ─────────────────────────────────────────────────────────────────────────────
// 0x005aca80  FUN_005aca80  AudioFmtSizeCalc  (~40 bytes)
// Computes the serialised byte size of a format descriptor.
//
// param_1 = pointer to format descriptor struct.
// Returns: serialised byte count as int.
//
// ASM key (0x005aca80..0x005aca9f):
//   005aca80: MOV EAX, 0x1c          ; default = 28
//   005aca86: CMP [param_1+4], 0     ; secondary-header ptr
//   005aca8a: JZ  skip_ext
//   005aca8c: MOV EAX, 0x2c          ; extended = 44
//   skip_ext:
//   005aca92: CMP [param_1+0x10], 0  ; extra-data ptr
//   005aca96: JZ  done
//   005aca98: ADD EAX, [param_1+0x14]; add extra-data byte count
//   done:
//   005aca9d: RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl AudioFmtSizeCalc(int param_1) {
    int size = 0x1c;                                   // base: 28 bytes (no secondary header)
    if (*reinterpret_cast<uint32_t*>(param_1 + 0x04) != 0u) {
        size = 0x2c;                                   // extended: 44 bytes (secondary header present)
    }
    if (*reinterpret_cast<uint32_t*>(param_1 + 0x10) != 0u) {
        size += *reinterpret_cast<int*>(param_1 + 0x14); // add extra-data byte count
    }
    return size;
}

RH_ScopedInstall(AudioFmtSizeCalc, 0x005aca80);  // re-enabled 2026-05-24 c3-audio-a

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

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac980  FUN_005ac980  AudioFmtDescCopy  (~70 bytes)
// Selective copy of fields from a source format-descriptor to a destination,
// with optional zero-initialisation of the destination first.
//
// param_1 = source format-descriptor ptr (undefined4*)
// param_2 = destination format-descriptor ptr (undefined4*)
// param_3 = if non-zero, zero-initialise param_2 before copy
//
// Source reads:   +0x00 (u32), +0x04 (u8 low), +0x05 (u8), +0x10 (u8 flags)
// Destination writes:
//   if param_3!=0: +0x00..+0x08, +0x10..+0x14 zeroed; +0x0c,+0x0d,+0x18,+0x19 zeroed
//   always: dst[+0x04] = src[+0x00]  (u32)
//           dst[+0x0c] = src[+0x04 byte]
//           dst[+0x0d] = src[+0x05 byte]
//           dst[+0x18] = 0, then bit0 from src[+0x10], then bit2 from src[+0x10]
//
// ASM key (0x005ac980..0x005ac9df):
//   005ac980: CMP param_3, 0
//   005ac983: JZ  skip_zeroinit
//   ; zero 9 fields of param_2 (+0x00,+0x04,+0x08,+0x0c,+0x0d,+0x18,+0x19,+0x10,+0x14)
//   skip_zeroinit:
//   ; selective copy from param_1 to param_2
//   005ac9bc: MOV AL, [param_1+4]    ; src u8 at +0x04
//   005ac9bf: MOV [param_2+0xc], AL  ; dst[+0x0c]
//   005ac9c2: MOV ECX, [param_1]     ; src u32 at +0x00
//   005ac9c4: MOV [param_2+4], ECX   ; dst[+0x04]
//   005ac9c7: MOV AL, [param_1+5]    ; src u8 at +0x05
//   005ac9ca: MOV [param_2+0xd], AL  ; dst[+0x0d]
//   005ac9cd: MOV byte [param_2+0x18], 0
//   005ac9d1: TEST byte [param_1+0x10], 1  ; bit0
//   005ac9d5: JZ  no_bit0
//   005ac9d7: MOV byte [param_2+0x18], 1
//   no_bit0:
//   005ac9d9: TEST byte [param_1+0x10], 4  ; bit2
//   005ac9dd: JZ  done
//   005ac9df: OR  byte [param_2+0x18], 4
//   done: RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioFmtDescCopy(
        uint32_t* param_1, uint32_t* param_2, int param_3) {
    if (param_3 != 0) {
        // Zero-initialise 9 fields of param_2
        param_2[0]                                   = 0u;  // +0x00
        param_2[1]                                   = 0u;  // +0x04
        param_2[2]                                   = 0u;  // +0x08
        *reinterpret_cast<uint8_t*>(param_2 + 3)     = 0u;  // +0x0c
        *reinterpret_cast<uint8_t*>(
            reinterpret_cast<uintptr_t>(param_2) + 0x0du) = 0u;  // +0x0d
        *reinterpret_cast<uint8_t*>(param_2 + 6)     = 0u;  // +0x18
        *reinterpret_cast<uint8_t*>(
            reinterpret_cast<uintptr_t>(param_2) + 0x19u) = 0u;  // +0x19
        param_2[4]                                   = 0u;  // +0x10
        param_2[5]                                   = 0u;  // +0x14
    }
    // Selective copy
    *reinterpret_cast<uint8_t*>(param_2 + 3) =
        *reinterpret_cast<uint8_t*>(param_1 + 1);           // src[+0x04 low byte] → dst[+0x0c]
    param_2[1] = param_1[0];                                 // src[+0x00] → dst[+0x04]
    *reinterpret_cast<uint8_t*>(
        reinterpret_cast<uintptr_t>(param_2) + 0x0du) =
        *reinterpret_cast<uint8_t*>(
            reinterpret_cast<uintptr_t>(param_1) + 0x05u); // src[+0x05] → dst[+0x0d]
    *reinterpret_cast<uint8_t*>(param_2 + 6) = 0u;          // dst[+0x18] = 0
    if ((*reinterpret_cast<uint8_t*>(param_1 + 4) & 0x01u) != 0u) {
        *reinterpret_cast<uint8_t*>(param_2 + 6) = 1u;      // propagate bit0 → dst[+0x18]
    }
    if ((*reinterpret_cast<uint8_t*>(param_1 + 4) & 0x04u) != 0u) {
        *reinterpret_cast<uint8_t*>(param_2 + 6) |= 4u;    // propagate bit2 → dst[+0x18]
    }
}

RH_ScopedInstall(AudioFmtDescCopy, 0x005ac980);  // re-enabled 2026-05-24 phase-a1 fmt_desc_copy (buffer-alloc fix) GREEN

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

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ae0b0  FUN_005ae0b0  AudioSubStructZeroInit  (14 bytes)
// Signature: void FUN_005ae0b0(undefined4 *param_1)
//
// Zeros the first 3 DWORD fields of param_1 in reverse order (fields 2, 1, 0).
// Pure leaf function, no callees.
//
// Decompilation:
//   param_1[2] = 0;
//   param_1[1] = 0;
//   *param_1   = 0;
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl
AudioSubStructZeroInit(std::uint32_t* param_1)
{
    param_1[2] = 0u;
    param_1[1] = 0u;
    param_1[0] = 0u;
}

RH_ScopedInstall(AudioSubStructZeroInit, 0x005ae0b0);  // re-enabled 2026-05-24 c3-audio-b

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
// 0x005aea00 — vtable-based raw alloc trampoline (C2).
typedef void* (__cdecl *RawAllocFn)(int, int);
static RawAllocFn const RawAlloc = reinterpret_cast<RawAllocFn>(0x005aea00u);

// 0x004522d0 — vtable-based raw free trampoline (C1).
typedef void (__cdecl *RawFreeFn)(void*);
static RawFreeFn const RawFree = reinterpret_cast<RawFreeFn>(0x004522d0u);

// 0x005ae920 — bitmap pool free (C2); call signature: void(pool_hdr*, void*)
typedef void (__cdecl *PoolFreeFn)(int*, int*);
static PoolFreeFn const PoolFree = reinterpret_cast<PoolFreeFn>(0x005ae920u);

// Global secondary pool pointer.
// 0x007ddab0 — DAT_007ddab0: pointer to secondary pool header used as the
//              default pool for pool-header allocation/return.
static int** const g_SecondaryPool = reinterpret_cast<int**>(0x007ddab0u);

// ---------------------------------------------------------------------------
// 0x005ae800  FUN_005ae800 — bitmap pool block allocator
// uint __cdecl FUN_005ae800(int *h, int flags)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn), disasm 0x005ae800..0x005ae912.
// Header h: [0] elem, [1] cap, [2] nbytes (bitmap bytes), [3] align, [4]/[5] block-list
// sentinel. Block: [0] next, [1] prev, bitmap bytes at +8.
//
// Per block the scan carries a COUNTDOWN seeded from h[1] (0x005ae828/0x005ae82f, kept
// in the param slot [esp+0x18]):
//   byte == 0xff            -> countdown -= 8, next byte        (0x005ae839, 0x005ae90d)
//   countdown == 0          -> next byte (not next block)       (0x005ae848 test/jbe = ==0)
//   bit set                 -> bit++, countdown--, up to 8 bits (0x005ae85b..0x005ae864)
//   bit clear (0x80 >> bit) -> set it, addr = ((blk + align + nbytes + 7) & ~(align-1))
//                              + (byte*8 + bit) * elem          (0x005ae868..0x005ae888)
//                              addr == 0 keeps scanning bytes   (0x005ae88c)
// The countdown is unsigned and only tested for zero, so a 0xff byte taken at
// countdown < 8 wraps it and later bytes are scanned again (faithful).
// No slot: RawAlloc(cap*elem + align + nbytes + 8, flags) (0x005ae8ac..0x005ae8c2); NULL
// -> 0 (0x005ae8d0). Bitmap zeroed (rep stos, 0x005ae8da/0x005ae8e1), block HEAD-inserted:
// new[1] = sentinel, new[0] = old head, old_head[1] = new, sentinel[0] = new
// (0x005ae8e3..0x005ae8f3); bitmap byte 0 = 0x80 (0x005ae8f5); returns the slot-0 address
// (new + align + nbytes + 7) & ~(align-1) (0x005ae8f9..0x005ae905).
// The old port scanned all 8 bits of every byte regardless of h[1] and tail-inserted.
// ---------------------------------------------------------------------------
// 0x005ae800
extern "C" __declspec(dllexport) unsigned int __cdecl AudioPoolBlockAlloc(int* param_1, int param_2)
{
    std::uint32_t* const h = reinterpret_cast<std::uint32_t*>(param_1);
    std::uint32_t* const sentinel = h + 4;                       // 0x005ae811
    const std::uint32_t nbytes = h[2];                           // 0x005ae80e

    std::uint32_t* blk = reinterpret_cast<std::uint32_t*>(h[4]);  // 0x005ae80b
    while (blk != sentinel) {                                    // 0x005ae818 / 0x005ae8a0
        std::uint32_t countdown = h[1];                          // 0x005ae828
        std::uint8_t* const bitmap = reinterpret_cast<std::uint8_t*>(blk) + 8;
        for (std::uint32_t byte_idx = 0; byte_idx < nbytes; ++byte_idx) {   // 0x005ae82d / 0x005ae893
            std::uint8_t bits = bitmap[byte_idx];                // 0x005ae835
            if (bits == 0xffu) {                                 // 0x005ae839
                countdown -= 8u;                                 // 0x005ae90d
                continue;
            }
            for (std::uint32_t bit = 0; bit < 8u; ++bit) {       // 0x005ae842 / 0x005ae85d
                if (countdown == 0u) break;                      // 0x005ae848
                const std::uint8_t mask = static_cast<std::uint8_t>(0x80u >> bit);   // 0x005ae851
                if ((bits & mask) == 0u) {                       // 0x005ae853
                    bits = static_cast<std::uint8_t>(bits | mask);                   // 0x005ae868
                    bitmap[byte_idx] = bits;                     // 0x005ae86d
                    const std::uint32_t idx = bit + byte_idx * 8u;                   // 0x005ae86a
                    const std::uint32_t align = h[3];            // 0x005ae875
                    const std::uint32_t addr =
                        ((reinterpret_cast<std::uint32_t>(blk) + align + nbytes + 7u) & ~(align - 1u))
                        + idx * h[0];                            // 0x005ae871..0x005ae888
                    if (addr != 0u) return addr;                 // 0x005ae88c
                    break;                                       // addr 0 -> next byte (0x005ae88e)
                }
                --countdown;                                     // 0x005ae85c
            }
        }
        blk = reinterpret_cast<std::uint32_t*>(blk[0]);          // 0x005ae89b
    }

    // 0x005ae8ac: no free slot, allocate a new block.
    const std::uint32_t size = h[1] * h[0] + h[3] + nbytes + 8u;  // 0x005ae8ac..0x005ae8bd
    std::uint32_t* const nb = static_cast<std::uint32_t*>(RawAlloc(static_cast<int>(size), param_2));  // 0x005ae8c2
    if (nb == nullptr) return 0u;                                // 0x005ae8d0

    std::uint8_t* const nbm = reinterpret_cast<std::uint8_t*>(nb) + 8;
    for (std::uint32_t i = 0; i < nbytes; ++i) nbm[i] = 0u;     // 0x005ae8da / 0x005ae8e1

    nb[1] = reinterpret_cast<std::uint32_t>(sentinel);           // 0x005ae8e5
    nb[0] = sentinel[0];                                         // 0x005ae8e3 / 0x005ae8e8
    reinterpret_cast<std::uint32_t*>(sentinel[0])[1] = reinterpret_cast<std::uint32_t>(nb);   // 0x005ae8ea / 0x005ae8f0
    sentinel[0] = reinterpret_cast<std::uint32_t>(nb);           // 0x005ae8f3
    nbm[0] = 0x80u;                                              // 0x005ae8f5

    const std::uint32_t align = h[3];                            // 0x005ae8f9
    return (reinterpret_cast<std::uint32_t>(nb) + align + nbytes + 7u) & ~(align - 1u);   // 0x005ae8fc..0x005ae905
}

RH_ScopedInstall(AudioPoolBlockAlloc, 0x005ae800);  // re-enabled 2026-05-24 c3-audio-b

// ---------------------------------------------------------------------------
// 0x005ae780  FUN_005ae780 — bitmap pool destructor
//
// Drains the circular block list at param_1+0x10: for each block, unlinks it
// and calls RawFree (0x004522d0). After all blocks are freed, frees the pool
// header itself: if DAT_007ddab0 secondary pool is available, returns the
// header there via PoolFree (0x005ae920); otherwise RawFree the header.
// If bit-0 of *(param_1+0x18) is set, the header is externally owned — skip
// the header free.
//
// Decompilation: Ghidra 0x005ae780..0x005ae7ff
// ---------------------------------------------------------------------------
// 0x005ae780
extern "C" __declspec(dllexport) void __cdecl AudioPoolDestroy(int param_1)
{
    // Block list head is at *(param_1 + 0x10), i.e. offset 4 DWORDs into header.
    int* sentinel = reinterpret_cast<int*>(param_1 + 0x10);
    int* head = reinterpret_cast<int*>(sentinel[0]);  // *(param_1+0x10)

    while (head != sentinel) {
        // Unlink from circular list.
        int* next_node = reinterpret_cast<int*>(head[0]);
        int* prev_node = reinterpret_cast<int*>(head[1]);
        *reinterpret_cast<int*>(head[1]) = head[0];  // *(prev->next) = next
        *reinterpret_cast<int*>(head[0] + 4) = head[1];  // *(next->prev) = prev
        // Free the block.
        RawFree(head);
        // Re-read head (may have changed if RawFree reshuffled, but in this
        // allocator the sentinel is stable).
        head = reinterpret_cast<int*>(sentinel[0]);
    }

    // Check ownership flag: bit-0 of *(param_1 + 0x18).
    const uint8_t flags = *reinterpret_cast<const uint8_t*>(param_1 + 0x18);
    if ((flags & 1u) == 0u) {
        // Header is internally owned — free it.
        int* secondary = *g_SecondaryPool;
        int* hdr = reinterpret_cast<int*>(param_1);
        if (secondary != nullptr && secondary != hdr) {
            // Return to secondary pool.
            PoolFree(secondary, hdr);
        } else {
            RawFree(hdr);
        }
    }
}

RH_ScopedInstall(AudioPoolDestroy, 0x005ae780);  // re-enabled 2026-05-24 c3-audio-b

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
// ---------------------------------------------------------------------------
// 0x005aeca0
extern "C" __declspec(dllexport) void __cdecl AudioFieldEndianPack(int* param_1, unsigned int* param_2, int param_3)
{
    if (param_3 == 1) {
        // 1-byte: copy low byte verbatim.
        *reinterpret_cast<uint8_t*>(*param_1) = static_cast<uint8_t>(*param_2);
        *param_1 += 1;
    } else if (param_3 == 2) {
        // 2-byte: big-endian swap.
        // Ghidra: *(uint16*)*param_1 = CONCAT11((char)*param_2, *(byte*)((int)param_2+1))
        // CONCAT11(hi, lo) = (hi << 8) | lo stored as uint16 little-endian →
        // byte[0] = lo = param_2[1], byte[1] = hi = param_2[0].
        const uint8_t b0 = static_cast<uint8_t>(*param_2);           // param_2[0] = hi
        const uint8_t b1 = *reinterpret_cast<const uint8_t*>(
                               reinterpret_cast<const char*>(param_2) + 1); // param_2[1] = lo
        *reinterpret_cast<uint8_t*>(*param_1)     = b1;  // lo byte first (little-endian store)
        *reinterpret_cast<uint8_t*>(*param_1 + 1) = b0;  // hi byte second
        *param_1 += 2;
    } else if (param_3 == 4) {
        // 4-byte: bswap32.
        // Ghidra formula: (v<<0x10 | v&0xff00 | v>>0x10&0xff)<<8 | *(byte*)(param_2+3)
        const unsigned int v = *param_2;
        const unsigned int swapped =
            (((v << 0x10u) | (v & 0xff00u) | ((v >> 0x10u) & 0xffu)) << 8u)
            | static_cast<uint8_t>(*reinterpret_cast<const uint8_t*>(
                reinterpret_cast<const char*>(param_2) + 3));
        *reinterpret_cast<unsigned int*>(*param_1) = swapped;
        *param_1 += 4;
    }
    // param_3 values other than 1/2/4: no-op (no else branch in Ghidra decomp).
}

RH_ScopedInstall(AudioFieldEndianPack, 0x005aeca0);  // re-enabled 2026-05-24 c3-audio-b

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
// ---------------------------------------------------------------------------
// 0x005ae0c0
extern "C" __declspec(dllexport) unsigned int* __cdecl AudioWaveFmtCopy(
        unsigned int* param_1, unsigned int* param_2, unsigned int* param_3)
{
    if (param_3 == nullptr) {
        // No-swap path: direct 4-DWORD copy.
        param_2[0] = param_1[0];
        param_2[1] = param_1[1];
        param_2[2] = param_1[2];
        param_2[3] = param_1[3];
    } else {
        // Swap path: build swapped 4-DWORD local buffer.
        unsigned int local_buf[4];
        int ptr = reinterpret_cast<int>(local_buf);

        // Byte-swap first 4-byte field (offsets 0-3).
        AudioFieldEndianPack(&ptr, param_1 + 0, 4);
        // Byte-swap 2-byte field at offset 4-5.
        AudioFieldEndianPack(&ptr, param_1 + 1, 2);
        // Byte-swap 2-byte field at offset 6-7.
        AudioFieldEndianPack(&ptr, reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(param_1) + 6), 2);
        // Remaining 8 bytes (offsets 8-15): direct copy.
        local_buf[2] = param_1[2];
        local_buf[3] = param_1[3];

        // Write to destination.
        param_2[0] = local_buf[0];
        param_2[1] = local_buf[1];
        param_2[2] = local_buf[2];
        param_2[3] = local_buf[3];
    }
    return param_1;
}

RH_ScopedInstall(AudioWaveFmtCopy, 0x005ae0c0);  // re-enabled 2026-05-24 c3-audio-b

// 0x005ae920  FUN_005ae920  (0xd3 bytes, 0x005ae920..0x005ae9f2)
// void __cdecl FUN_005ae920(uint *h, uint item)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn).
//   0x005ae93d  base = blk + nbytes + 8   (h[2] at 0x005ae92b)
//   0x005ae941  item <  base            -> next block (unsigned, jb)
//   0x005ae945  item <= base + cap*elem -> found      (unsigned, jbe: end INCLUSIVE)
//   0x005ae95d  idx = (item - base) / elem (unsigned div)
//   0x005ae966..0x005ae98b  bitmap[idx >> 3] &= ~(0x80 >> (idx & 7))
//   0x005ae98f..0x005ae99b  unlink blk
//   0x005ae99e  head = sentinel[0] (after the unlink)
//   0x005ae9a0  list empty            -> relink (no compaction test)
//   0x005ae9a4  (byte h[6] & 2) == 0  -> relink
//   0x005ae9aa..0x005ae9bd  sum of the bitmap BYTE VALUES (not a popcount); nbytes 0 skips
//   0x005ae9c1  sum != 0 -> relink, else 0x005ae9c4 RwFree(blk) via 0x004522d0
//   relink (0x005ae9d1 / 0x005ae9e2): blk[0] = head, blk[1] = sentinel,
//          sentinel[0][1] = blk, sentinel[0] = blk
// The old port used an exclusive end and tested compaction even when the block was the
// list's only member.
extern "C" __declspec(dllexport) void __cdecl AudioPoolFree(
        uint32_t* param_1, uint32_t param_2)
{
    uint32_t* const sentinel = param_1 + 4;                       // 0x005ae92e
    const uint32_t nbytes = param_1[2];                           // 0x005ae92b

    typedef void (__cdecl *FreeFn)(void*);
    const auto rawFree = reinterpret_cast<FreeFn>(0x004522d0u);   // 0x005ae9c4

    uint32_t* blk = reinterpret_cast<uint32_t*>(param_1[4]);      // 0x005ae928
    for (;;) {
        if (blk == sentinel) return;                              // 0x005ae931 / 0x005ae954
        const uint32_t base = nbytes + reinterpret_cast<uint32_t>(blk) + 8u;   // 0x005ae93d
        if (param_2 >= base && param_2 <= param_1[1] * param_1[0] + base)       // 0x005ae941 / 0x005ae94e
            break;
        blk = reinterpret_cast<uint32_t*>(blk[0]);                // 0x005ae952
    }

    const uint32_t base = nbytes + reinterpret_cast<uint32_t>(blk) + 8u;
    const uint32_t idx = (param_2 - base) / param_1[0];           // 0x005ae95d..0x005ae961
    uint8_t* const bitmap = reinterpret_cast<uint8_t*>(blk) + 8;
    bitmap[idx >> 3] = static_cast<uint8_t>(
        bitmap[idx >> 3] & static_cast<uint8_t>(~(0x80u >> (idx & 7u))));       // 0x005ae966..0x005ae98b

    reinterpret_cast<uint32_t*>(blk[1])[0] = blk[0];              // 0x005ae98f..0x005ae994
    reinterpret_cast<uint32_t*>(blk[0])[1] = blk[1];              // 0x005ae996..0x005ae99b

    const uint32_t head = sentinel[0];                            // 0x005ae99e
    if (head != reinterpret_cast<uint32_t>(sentinel) &&           // 0x005ae9a0
        (*reinterpret_cast<const uint8_t*>(param_1 + 6) & 2u) != 0u) {   // 0x005ae9a4
        uint32_t sum = 0;                                         // 0x005ae9aa
        for (uint32_t i = 0; i < nbytes; ++i) sum += bitmap[i];   // 0x005ae9b2..0x005ae9bd
        if (sum == 0u) {                                          // 0x005ae9bf
            rawFree(blk);                                         // 0x005ae9c4
            return;
        }
    }

    blk[0] = head;                                                // 0x005ae9d1 / 0x005ae9e2
    blk[1] = reinterpret_cast<uint32_t>(sentinel);                // 0x005ae9d3 / 0x005ae9e4
    reinterpret_cast<uint32_t*>(sentinel[0])[1] = reinterpret_cast<uint32_t>(blk);   // 0x005ae9d6 / 0x005ae9e7
    sentinel[0] = reinterpret_cast<uint32_t>(blk);                // 0x005ae9db / 0x005ae9ec
}

RH_ScopedInstall(AudioPoolFree, 0x005ae920);  // re-enabled 2026-05-24 c3-audio-b

// ---------------------------------------------------------------------------

// 0x005addd0  FUN_005addd0  (~60 bytes)
// void FUN_005addd0(int *param_1, int param_2)
//   param_1 = pointer to list head variable
//   param_2 = payload value to store in new node
// Allocates one node from the pool at DAT_009146c0 via FUN_005ae800,
// then inserts it at the head of the doubly-linked list.
//
// Ghidra key addresses:
//   0x005addd0: call FUN_005ae800(&DAT_009146c0, 0x30804) → piVar2
//   0x005adde3: NULL check; return if alloc failed
//   0x005adde8: piVar2[2] = param_2    (payload at +0x08)
//   0x005addec: iVar1 = *param_1       (old head)
//   0x005addf1: piVar2[1] = param_1   (back-link to head variable)
//   0x005addf4: *piVar2 = iVar1        (fwd ptr = old head)
//   0x005addf7: *(old_head+4) = piVar2 (old head's back-link → new node)
//   0x005addfc: *param_1 = piVar2      (list head = new node)
extern "C" __declspec(dllexport) void __cdecl AudioListInsertHead(
        int* param_1, int param_2)
{
    // DAT_009146c0 = global node pool base
    static constexpr uint32_t kNodePool = 0x009146c0u;
    static constexpr uint32_t kAllocTag = 0x00030804u;

    typedef int* (__cdecl *AllocFn)(uint32_t*, uint32_t);
    const auto poolAlloc = reinterpret_cast<AllocFn>(0x005ae800u);

    int* piVar2 = poolAlloc(reinterpret_cast<uint32_t*>(kNodePool), kAllocTag);
    if (!piVar2) return;

    const int iVar1  = *param_1;     // old head
    piVar2[2]        = param_2;      // node[+0x08] = payload
    piVar2[1]        = reinterpret_cast<int>(param_1); // node[+0x04] = back-link to head-ptr
    piVar2[0]        = iVar1;        // node[+0x00] = fwd ptr = old head
    *reinterpret_cast<int**>(iVar1 + 4) = piVar2; // old_head[+0x04] = new node
    *param_1         = reinterpret_cast<int>(piVar2);  // head = new node
}

RH_ScopedInstall(AudioListInsertHead, 0x005addd0);  // re-enabled 2026-05-24 c3-audio-a

// ---------------------------------------------------------------------------

// 0x005ade10  FUN_005ade10  (65 bytes)
// int* FUN_005ade10(list_sentinel *param_1, int param_2)
//   param_1 = doubly-linked list sentinel/head node
//   param_2 = data value to search for and remove
//   Returns param_1 on success (found + removed), NULL if not found.
// Traversal starts from param_1[1] (first real node after sentinel).
// Unlinks found node and returns it to the pool at DAT_009146c0 via AudioPoolFree.
//
// Ghidra key addresses:
//   0x005ade10: piVar2 = param_1[1]     (first real node)
//   0x005ade1d: iVar1 = piVar2[2]       (data field)
//   0x005ade22: if iVar1 == param_2: found path
//   0x005ade34: *(piVar2[0]+4) = piVar2[1]  (prev next → current next)
//   0x005ade3a: *(piVar2[1]) = *piVar2       (next prev → current prev)
//   0x005ade40: FUN_005ae920(&DAT_009146c0, piVar2)
//   0x005ade49: return param_1
//   0x005ade4f: if piVar2 == param_1: return NULL
//   0x005ade55: piVar2 = piVar2[1]   (advance: next node)
extern "C" __declspec(dllexport) int* __cdecl AudioListRemoveByValue(
        int* param_1, int param_2)
{
    static constexpr uint32_t kNodePool = 0x009146c0u;

    int* piVar2 = reinterpret_cast<int*>(param_1[1]); // first real node
    while (true) {
        const int iVar1 = piVar2[2];             // data field — read first (0x005ade1d)
        if (iVar1 == param_2) {                  // found check before sentinel (0x005ade22)
            // Unlink from doubly-linked list.
            // prev node's next = current node's next
            *reinterpret_cast<int**>(piVar2[0] + 4) =
                reinterpret_cast<int*>(piVar2[1]);
            // next node's prev = current node's prev
            *reinterpret_cast<int**>(piVar2[1]) =
                reinterpret_cast<int*>(static_cast<uintptr_t>(static_cast<unsigned>(piVar2[0])));
            // Return node to free pool.
            AudioPoolFree(reinterpret_cast<uint32_t*>(kNodePool),
                          reinterpret_cast<uint32_t>(piVar2));
            return param_1;
        }
        if (piVar2 == param_1) return nullptr;   // reached sentinel: not found (0x005ade4f)
        piVar2 = reinterpret_cast<int*>(piVar2[1]); // advance (0x005ade55)
    }
}

RH_ScopedInstall(AudioListRemoveByValue, 0x005ade10);  // re-enabled 2026-05-24 c3-audio-a

// ---------------------------------------------------------------------------

// 0x005ade90  FUN_005ade90  (49 bytes)
// void FUN_005ade90(int *param_1)
//   param_1 = list sentinel (head node)
// Drains the doubly-linked list by unlinking and returning all nodes to
// the free pool at DAT_009146c0.  After the call the sentinel points to itself.
//
// Ghidra key addresses:
//   0x005ade90: loop entry: while param_1[1] != param_1
//   0x005ade97: piVar1 = (int*)param_1[1]         (current node)
//   0x005adea0: *(int*)(piVar1[0] + 4) = piVar1[1] (prev node's next)
//   0x005adea9: *(int*)piVar1[1] = piVar1[0]       (next node's prev)
//   0x005adeae: FUN_005ae920(&DAT_009146c0, piVar1)
//   0x005adebb: (constant) pool address 0x009146c0
extern "C" __declspec(dllexport) void __cdecl AudioListDrain(int* param_1)
{
    static constexpr uint32_t kNodePool = 0x009146c0u;

    while (param_1[1] != reinterpret_cast<int>(param_1)) {
        int* piVar1 = reinterpret_cast<int*>(param_1[1]); // current node

        // Unlink: prev node's next = current node's next
        *reinterpret_cast<int**>(piVar1[0] + 4) =
            reinterpret_cast<int*>(piVar1[1]);
        // next node's prev = current node's prev
        *reinterpret_cast<int**>(piVar1[1]) =
            reinterpret_cast<int*>(static_cast<uintptr_t>(static_cast<unsigned>(piVar1[0])));

        // Return node to free pool.
        AudioPoolFree(reinterpret_cast<uint32_t*>(kNodePool),
                      reinterpret_cast<uint32_t>(piVar1));
    }
}

RH_ScopedInstall(AudioListDrain, 0x005ade90);  // re-enabled 2026-05-24 c3-audio-a

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

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ae650  AudioPoolConstruct
// uint *FUN_005ae650(int elem, uint count, uint align, int prealloc, uint *hdr, int flags)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn), disasm 0x005ae650..0x005ae773.
//   0x005ae654  align == 0 -> 1
//   0x005ae661  hdr != NULL -> hdr[6] = 3 (0x005ae6a2)
//               hdr == NULL -> DAT_007ddab0 != 0 ? FUN_005ae800(DAT_007ddab0, 0x30800)
//                                                (0x005ae669..0x005ae678)
//                                              : FUN_005aea00(0x24, flags) (0x005ae67f..0x005ae686)
//               NULL -> return 0 (0x005ae694); else hdr[6] = 2 (0x005ae699)
//   0x005ae6a9..0x005ae6d8  hdr[0] = (elem + align - 1) & ~(align - 1), hdr[1] = count,
//               hdr[2] = (count + 7) >> 3, hdr[3] = align, hdr[4] = hdr[5] = &hdr[4]
//   0x005ae6db  prealloc counted down as an unsigned dword until 0 (dec/jne 0x005ae736):
//     size = hdr[0]*count + nbytes + align + 8 (0x005ae6dd..0x005ae6e2)
//     blk = FUN_005aea00(size, flags) (0x005ae6f4); NULL -> FUN_005ae780(hdr), return 0
//       (0x005ae764)
//     blk[1] = 0, blk[0] = 0 (0x005ae702/0x005ae709), then HEAD insert: blk[1] = &hdr[4],
//     blk[0] = hdr[4], hdr[4][1] = blk, hdr[4] = blk (0x005ae70f..0x005ae722);
//     bitmap zeroed, nbytes bytes at blk+8 (0x005ae729/0x005ae730)
//   0x005ae73d..0x005ae75b  global pool list, DAT_007dda80 is a POINTER variable (head):
//     hdr[8] = 0x007dda80, hdr[7] = DAT_007dda80, DAT_007dda80[1] = &hdr[7] (re-read of the
//     global at 0x005ae750), DAT_007dda80 = &hdr[7]
//   returns hdr (0x005ae760)
// The old port always used FUN_005aea00 for the header, skipped the prealloc loop, and
// treated 0x007dda80 as a list node (tail insert through 0x007dda84).
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t * __cdecl AudioPoolConstruct(
        int param_1, std::uint32_t param_2, std::uint32_t param_3,
        int param_4, std::uint32_t *param_5, std::uint32_t param_6)
{
    typedef std::uint32_t * (__cdecl *RawAllocFn)(std::uint32_t, std::uint32_t);
    typedef std::uint32_t   (__cdecl *BlockAllocFn)(std::uint32_t *, std::uint32_t);
    typedef void            (__cdecl *DestroyFn)(std::uint32_t *);
    static constexpr std::uintptr_t kRawAlloc    = 0x005aea00u;
    static constexpr std::uintptr_t kBlockAlloc  = 0x005ae800u;
    static constexpr std::uintptr_t kDestroy     = 0x005ae780u;
    static constexpr std::uintptr_t kGlobalList  = 0x007dda80u;
    static constexpr std::uintptr_t kHdrPool     = 0x007ddab0u;    // U-1736

    const auto rawAlloc   = reinterpret_cast<RawAllocFn>(kRawAlloc);
    const auto blockAlloc = reinterpret_cast<BlockAllocFn>(kBlockAlloc);
    const auto destroy    = reinterpret_cast<DestroyFn>(kDestroy);

    std::uint32_t align = param_3;
    if (align == 0u) align = 1u;                                   // 0x005ae658 / 0x005ae65c

    std::uint32_t *hdr = param_5;                                  // 0x005ae661
    if (hdr == nullptr) {
        std::uint32_t *const hdrPool = *reinterpret_cast<std::uint32_t **>(kHdrPool);   // 0x005ae669
        if (hdrPool != nullptr)
            hdr = reinterpret_cast<std::uint32_t *>(blockAlloc(hdrPool, 0x30800u));     // 0x005ae678
        else
            hdr = rawAlloc(0x24u, param_6);                        // 0x005ae686
        if (hdr == nullptr) return nullptr;                        // 0x005ae690
        hdr[6] = 2u;                                               // 0x005ae699
    } else {
        hdr[6] = 3u;                                               // 0x005ae6a2
    }

    const std::uint32_t elem   = (static_cast<std::uint32_t>(param_1) + align - 1u) & ~(align - 1u);   // 0x005ae6ad..0x005ae6ba
    const std::uint32_t nbytes = (param_2 + 7u) >> 3;             // 0x005ae6c0 / 0x005ae6c6
    std::uint32_t *const sentinel = hdr + 4;                       // 0x005ae6c3

    hdr[0] = elem;                                                 // 0x005ae6cb
    hdr[1] = param_2;                                              // 0x005ae6cd
    hdr[3] = align;                                                // 0x005ae6d0
    hdr[2] = nbytes;                                               // 0x005ae6d3
    sentinel[0] = reinterpret_cast<std::uint32_t>(sentinel);       // 0x005ae6d6
    hdr[5] = reinterpret_cast<std::uint32_t>(sentinel);            // 0x005ae6d8

    std::uint32_t remaining = static_cast<std::uint32_t>(param_4);
    if (remaining != 0u) {                                         // 0x005ae6c9 / 0x005ae6db
        const std::uint32_t size = elem * param_2 + nbytes + align + 8u;   // 0x005ae6dd..0x005ae6e2
        do {
            std::uint32_t *const blk = rawAlloc(size, param_6);    // 0x005ae6f4
            if (blk == nullptr) {                                  // 0x005ae700
                destroy(hdr);                                      // 0x005ae765
                return nullptr;                                    // 0x005ae76d
            }
            blk[1] = 0u;                                           // 0x005ae702
            blk[0] = 0u;                                           // 0x005ae709
            blk[1] = reinterpret_cast<std::uint32_t>(sentinel);    // 0x005ae711
            blk[0] = sentinel[0];                                  // 0x005ae70f / 0x005ae714
            reinterpret_cast<std::uint32_t *>(sentinel[0])[1] = reinterpret_cast<std::uint32_t>(blk);   // 0x005ae716 / 0x005ae71d
            sentinel[0] = reinterpret_cast<std::uint32_t>(blk);    // 0x005ae722
            std::uint8_t *const bm = reinterpret_cast<std::uint8_t *>(blk) + 8;
            for (std::uint32_t i = 0; i < nbytes; ++i) bm[i] = 0u; // 0x005ae729 / 0x005ae730
        } while (--remaining != 0u);                               // 0x005ae736 / 0x005ae73b
    }

    std::uint32_t *volatile *const gHead = reinterpret_cast<std::uint32_t *volatile *>(kGlobalList);
    hdr[8] = static_cast<std::uint32_t>(kGlobalList);              // 0x005ae746
    hdr[7] = reinterpret_cast<std::uint32_t>(*gHead);              // 0x005ae73d / 0x005ae74e
    (*gHead)[1] = reinterpret_cast<std::uint32_t>(hdr + 7);        // 0x005ae750 / 0x005ae758
    *gHead = hdr + 7;                                              // 0x005ae75b

    return hdr;                                                    // 0x005ae760
}

RH_ScopedInstall(AudioPoolConstruct, 0x005ae650);  // re-enabled 2026-05-24 c3-audio-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x005be190  FUN_005be190  AudioRwsSubZeroInit  (0x11 bytes, leaf)
// Signature: void FUN_005be190(undefined4 *param_1)
//
// Zeroes four fields of a sub-struct pointed to by param_1:
//   param_1[5] = 0    -- offset 0x14
//   param_1[3] = 0    -- offset 0x0c
//   param_1[4] = 0    -- offset 0x10
//   *param_1   = 0    -- offset 0x00
//
// Pure leaf, no callees. Called from FUN_005be160 (conditional teardown).
//
// Smoke-test target for audio_sub_struct_zero harness arg_type (2026-05-22).
// Full C3 promotion is c3_batch_p's job.
//
// Constants (cited from 0x005be190 body):
//   index 5 -> offset 0x14 (20)
//   index 3 -> offset 0x0c (12)
//   index 4 -> offset 0x10 (16)
//   index 0 -> offset 0x00 (0)
//
// Analysis note: re/analysis/promote_c2_rws_audio_loader/5be190.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioRwsSubZeroInit(std::uint32_t* param_1)
{
    param_1[5] = 0u;   // offset 0x14
    param_1[3] = 0u;   // offset 0x0c
    param_1[4] = 0u;   // offset 0x10
    param_1[0] = 0u;   // offset 0x00
}

RH_ScopedInstall(AudioRwsSubZeroInit, 0x005be190);  // re-enabled 2026-05-24 c3-audio-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x005be140  FUN_005be140  AudioSubStructThreeWrite  (0x19 bytes, leaf)
// Signature: void FUN_005be140(int param_1, undefined4 param_2, undefined4 param_3)
//
// Writes three fields of the struct pointed to by param_1:
//   *(param_1 + 0x14) = param_2    -- cited 0x005be143
//   *(param_1 + 0x10) = param_3    -- cited 0x005be149
//   *(param_1 + 0x0c) = 0          -- cited 0x005be14f
//
// Pure leaf, no callees. Called from FUN_005be160 (conditional teardown).
// S-3687 cleared (see promote_c2_rws_audio_loader session notes).
//
// Smoke-test target for struct_three_write harness arg_type (2026-05-22 session B).
// Full C3 promotion is c3_batch_r's job.
//
// Constants (cited from 0x005be140 body):
//   0x14 (20)   -- param_2 field offset, cited 0x005be143
//   0x10 (16)   -- param_3 field offset, cited 0x005be149
//   0x0c (12)   -- zero field offset, cited 0x005be14f
//   0x0  (0)    -- value written at 0x0c, cited 0x005be154
//
// Analysis note: re/analysis/promote_c2_rws_audio_loader/5be140.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioSubStructThreeWrite(
    std::uint32_t* param_1,   // pointer to sub-struct
    std::uint32_t  param_2,   // value to write at +0x14
    std::uint32_t  param_3)   // value to write at +0x10
{
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(param_1) + 0x14u) = param_2;  // cited 0x005be143
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(param_1) + 0x10u) = param_3;  // cited 0x005be149
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(param_1) + 0x0cu) = 0u;       // cited 0x005be14f
}

RH_ScopedInstall(AudioSubStructThreeWrite, 0x005be140);  // re-enabled 2026-05-24 c3-audio-b

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

