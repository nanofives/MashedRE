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

RH_ScopedInstall(AudioFmtSizeCalc, 0x005aca80);

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

RH_ScopedInstall(AudioFmtTableSearch, 0x005acd10);

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

RH_ScopedInstall(AudioFmtDescCopy, 0x005ac980);

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

RH_ScopedInstall(AudioFmtGlobalScan, 0x005acd60);

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

RH_ScopedInstall(AudioSubStructLinkDevice, 0x005ae010);

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

RH_ScopedInstall(AudioSubStructLinkBuffer, 0x005adfe0);

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

RH_ScopedInstall(AudioSubStructZeroInit, 0x005ae0b0);

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

RH_ScopedInstall(AudioSubStructDualInit, 0x005ac7b0);
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
//
// Scans the circular block list rooted at param_1[4] for a free bit in each
// block's bitmap (param_1[2] bytes at block+8).  If a free bit is found it is
// set and the aligned address for that slot is computed and returned.  If no
// free bit exists in any existing block a new block is allocated via RawAlloc
// (0x005aea00), its bitmap zeroed, bit-0 set, the block inserted into the
// circular list, and the first slot address returned.
//
// Address formula (from Ghidra 0x005ae800..0x005ae8ff):
//   slot_index = byte_idx * 8 + bit_pos
//   base       = (block + param_1[2] + param_1[3] + 7) & ~(param_1[3] - 1)
//   return     = base + slot_index * param_1[0]
// ---------------------------------------------------------------------------
// 0x005ae800
extern "C" __declspec(dllexport) unsigned int __cdecl AudioPoolBlockAlloc(int* param_1, int param_2)
{
    const int  elem_size   = param_1[0];   // offset 0x00: element size in bytes
    const int  bitmap_len  = param_1[2];   // offset 0x08: bitmap byte count
    const int  align       = param_1[3];   // offset 0x0c: alignment requirement
    int*       sentinel    = param_1 + 4;  // offset 0x10: circular list sentinel

    // Scan existing blocks.
    int* block = reinterpret_cast<int*>(sentinel[0]);  // head = *(sentinel)
    while (block != sentinel) {
        // Bitmap bytes start at block+8 (i.e., block[2]).
        uint8_t* bitmap = reinterpret_cast<uint8_t*>(block + 2);
        for (int byte_idx = 0; byte_idx < bitmap_len; ++byte_idx) {
            uint8_t b = bitmap[byte_idx];
            if (b == 0xffu) continue;  // byte full
            // Find first clear bit (MSB-first: 0x80 >> i).
            for (int bit_pos = 0; bit_pos < 8; ++bit_pos) {
                uint8_t mask = static_cast<uint8_t>(0x80u >> bit_pos);
                if ((b & mask) == 0u) {
                    bitmap[byte_idx] = static_cast<uint8_t>(b | mask);
                    const int slot_idx = byte_idx * 8 + bit_pos;
                    // Base address: (block + bitmap_len + align + 7) & ~(align-1)
                    const uintptr_t raw_base =
                        reinterpret_cast<uintptr_t>(block) +
                        static_cast<uintptr_t>(bitmap_len) +
                        static_cast<uintptr_t>(align) + 7u;
                    const uintptr_t base = raw_base & ~static_cast<uintptr_t>(align - 1);
                    return static_cast<unsigned int>(base + static_cast<uintptr_t>(slot_idx * elem_size));
                }
            }
        }
        block = reinterpret_cast<int*>(block[0]);  // next in circular list
    }

    // No free slot — allocate a new block.
    const int block_size = elem_size * param_1[1] + align + 8 + bitmap_len;
    // param_1[1] = capacity (elements per block), at offset 0x04.
    // Full formula: param_1[1]*param_1[0] + param_1[3] + 8 + param_1[2]
    int* new_block = static_cast<int*>(RawAlloc(block_size, param_2));
    if (!new_block) return 0u;

    // Zero bitmap at new_block+8 (new_block[2..]).
    uint8_t* new_bitmap = reinterpret_cast<uint8_t*>(new_block + 2);
    for (int i = 0; i < bitmap_len; ++i) new_bitmap[i] = 0u;

    // Mark bit-0 used (MSB of first bitmap byte = 0x80).
    new_bitmap[0] = 0x80u;

    // Insert into circular list before sentinel (tail-insert).
    int* prev = reinterpret_cast<int*>(sentinel[1]);  // *(sentinel+4) = tail
    new_block[0] = reinterpret_cast<int>(sentinel);   // new->next = sentinel
    new_block[1] = reinterpret_cast<int>(prev);       // new->prev = tail
    prev[0]      = reinterpret_cast<int>(new_block);  // tail->next = new
    sentinel[1]  = reinterpret_cast<int>(new_block);  // sentinel->prev = new

    // Return slot 0 address.
    const uintptr_t raw_base =
        reinterpret_cast<uintptr_t>(new_block) +
        static_cast<uintptr_t>(bitmap_len) +
        static_cast<uintptr_t>(align) + 7u;
    const uintptr_t base = raw_base & ~static_cast<uintptr_t>(align - 1);
    return static_cast<unsigned int>(base);
}

RH_ScopedInstall(AudioPoolBlockAlloc, 0x005ae800);

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

RH_ScopedInstall(AudioPoolDestroy, 0x005ae780);

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

RH_ScopedInstall(AudioFieldEndianPack, 0x005aeca0);

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

RH_ScopedInstall(AudioWaveFmtCopy, 0x005ae0c0);

// 0x005ae920  FUN_005ae920  (~0xd3 bytes)
// void FUN_005ae920(uint *param_1, uint param_2)
//   param_1 = pool header pointer
//   param_2 = address of item being freed (as uint)
// Walks block list to find the block containing param_2, clears the bitmap bit,
// optionally releases empty blocks to heap when pool-compaction flag is set.
//
// Ghidra address of key steps:
//   0x005ae920: function prologue / stack frame setup
//   0x005ae930: block list traversal start (param_1[4] block head)
//   0x005ae95c: range check: block_start <= param_2 < block_start + N*elem_sz
//   0x005ae96c: compute item index = (param_2 - block_start) / elem_sz
//   0x005ae97a: clear bit: block[8 + (idx>>3)], mask = 0x80 >> (idx & 7) (MSB-first)
//   0x005ae9a0: compaction flag check (param_1[6] bit 1)
//   0x005ae9b0: count remaining set bits; re-link or FUN_004522d0(block)
extern "C" __declspec(dllexport) void __cdecl AudioPoolFree(
        uint32_t* param_1, uint32_t param_2)
{
    // pool header fields
    const uint32_t elem_size   = param_1[0];
    const uint32_t elems_per_blk = param_1[1];
    const uint32_t bitmap_bytes  = param_1[2];  // bytes in bitmap per block
    const uint32_t* sentinel     = param_1 + 4; // sentinel for circular list

    // vtable dealloc trampoline at 0x004522d0 — used when releasing empty blocks
    typedef void (__cdecl *FreeFn)(void*);
    const auto rawFree = reinterpret_cast<FreeFn>(0x004522d0u);

    // Walk block list
    uint32_t* block = reinterpret_cast<uint32_t*>(param_1[4]);
    while (block != sentinel) {
        // block_start: block pointer + 2 words (next/prev) + bitmap_bytes,
        // aligned to elem_size.  From Ghidra: block_start = block + uVar2 + 8.
        const uint32_t block_addr   = reinterpret_cast<uint32_t>(block);
        // uVar2 in the original is param_1[2] (bitmap byte count).
        const uint32_t block_start  = block_addr + 8u + bitmap_bytes;
        const uint32_t range_end    = block_start + elems_per_blk * elem_size;

        if (param_2 >= block_start && param_2 < range_end) {
            // Found the block.
            const uint32_t item_index = (param_2 - block_start) / elem_size;

            // Clear allocation bit: MSB-first bitmap at block+8.
            uint8_t* bitmap = reinterpret_cast<uint8_t*>(block_addr + 8u);
            bitmap[item_index >> 3] &= ~(0x80u >> (item_index & 7u));

            // Unlink block from current list position.
            uint32_t* prev = reinterpret_cast<uint32_t*>(block[1]);
            uint32_t* next = reinterpret_cast<uint32_t*>(block[0]);
            prev[0] = reinterpret_cast<uint32_t>(next);
            next[1] = reinterpret_cast<uint32_t>(prev);

            if (param_1[6] & 2u) {
                // Compaction flag set: count remaining set bits.
                uint32_t bits_set = 0;
                for (uint32_t b = 0; b < bitmap_bytes; ++b) {
                    uint8_t byte = bitmap[b];
                    while (byte) {
                        bits_set += (byte & 1u);
                        byte >>= 1;
                    }
                }
                if (bits_set > 0) {
                    // Still allocated items — re-link at free-list head.
                    uint32_t* head_next = reinterpret_cast<uint32_t*>(param_1[4]);
                    block[0] = reinterpret_cast<uint32_t>(head_next);
                    block[1] = reinterpret_cast<uint32_t>(sentinel);
                    head_next[1] = reinterpret_cast<uint32_t>(block);
                    param_1[4] = reinterpret_cast<uint32_t>(block);
                } else {
                    // Block fully empty — release to heap.
                    rawFree(block);
                }
            } else {
                // No compaction — re-link block at free-list head unconditionally.
                uint32_t* head_next = reinterpret_cast<uint32_t*>(param_1[4]);
                block[0] = reinterpret_cast<uint32_t>(head_next);
                block[1] = reinterpret_cast<uint32_t>(sentinel);
                head_next[1] = reinterpret_cast<uint32_t>(block);
                param_1[4] = reinterpret_cast<uint32_t>(block);
            }
            return;
        }

        block = reinterpret_cast<uint32_t*>(block[0]);
    }
    // Item not found in any block — no-op (matches original behavior).
}

RH_ScopedInstall(AudioPoolFree, 0x005ae920);

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

RH_ScopedInstall(AudioListInsertHead, 0x005addd0);

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

RH_ScopedInstall(AudioListRemoveByValue, 0x005ade10);

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

RH_ScopedInstall(AudioListDrain, 0x005ade90);

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

RH_ScopedInstall(AudioWaveNodeFree, 0x005abcb0);

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

RH_ScopedInstall(AudioSubStructBufCleanup, 0x005ac740);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac900  AudioContextLookup
// undefined4 FUN_005ac900(undefined4 param_1)
//
// Shallow dispatcher: sets up a two-slot stack frame (local_8 = input,
// local_4 = output), then calls FUN_005aa0c0 with inline callback LAB_005ac930.
// The callback writes the result into local_4 as a side-effect.
// Returns local_4.
//
// U-1730: LAB_005ac930 at 0x005ac930 is an inline callback beyond this
//   function's body end (0x005ac92f); its identity and body are not analyzed.
//   This wrapper is faithful to the decompilation; callback behavior is
//   delegated to the original code region at 0x005ac930.
//
// Callees:
//   FUN_005aa0c0  0x005aa0c0  recursive tree-walk predicate search
//     args: (0, 0, &LAB_005ac930, &local_8, 1)
//     local_4 written by callback region at LAB_005ac930.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioContextLookup(std::uint32_t param_1)
{
    typedef void (__cdecl *TreeWalkFn)(std::uint32_t, std::uint32_t, void *, void *, int);
    static constexpr std::uintptr_t kTreeWalk = 0x005aa0c0u;
    static constexpr std::uintptr_t kCallback = 0x005ac930u;   // LAB_005ac930

    const auto treeWalk = reinterpret_cast<TreeWalkFn>(kTreeWalk);

    std::uint32_t local_8 = param_1;   // input arg passed by address to callback
    std::uint32_t local_4 = 0u;        // output written by LAB_005ac930 callback

    treeWalk(0u, 0u, reinterpret_cast<void *>(kCallback), &local_8, 1);

    return local_4;
}

RH_ScopedInstall(AudioContextLookup, 0x005ac900);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ae650  AudioPoolConstruct   [STRUCT GAP: pool hdr DAT_007ddab0 role unclear]
// uint *FUN_005ae650(int param_1, uint param_2, uint param_3, int param_4,
//                    uint *param_5, undefined4 param_6)
//
//   param_1: element size (bytes per element)
//   param_2: element count (bits in bitmap)
//   param_3: alignment (forced >= 1)
//   param_4: pre-allocate N blocks (0 = lazy)
//   param_5: existing pool header or NULL -> allocate new from DAT_007ddab0 or FUN_005aea00
//   param_6: alloc flags (e.g. 0x30806)
//
// Pool header layout (9 fields x 4B = 0x24 bytes):
//   [0]: aligned_size  = (elem_size + align - 1) & ~(align - 1)
//   [1]: bit_count     = param_2
//   [2]: bitmap_bytes  = (bit_count + 7) >> 3
//   [3]: alignment
//   [4]: block list head (circular) <- initially points to itself
//   [5]: block list tail            <- initially = &[4]
//   [6]: ownership flag (2 = heap, 3 = external)
//   [7]: global list next (DAT_007dda80 circular)
//   [8]: global list prev
//
// Block layout:
//   Each block: 2 + bitmap_words uint32s header + aligned_size*bit_count data bytes.
//   block[0]: next ptr (circular)
//   block[1]: prev ptr
//   block[2..2+bitmap_words-1]: bitmap (0=free, 1=used)
//
// Global pool list: all pools linked into circular list at DAT_007dda80 (0x007dda80).
// DAT_007ddab0 (0x007ddab0): secondary pool-header pool. U-1736: purpose relative
//   to DAT_007dda80 unclear. Used when param_5 is NULL and pool is non-NULL. [STRUCT GAP]
//
// Callees:
//   0x005aea00  raw pool-header allocator (vtable trampoline)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t * __cdecl AudioPoolConstruct(
        int param_1, std::uint32_t param_2, std::uint32_t param_3,
        int param_4, std::uint32_t *param_5, std::uint32_t param_6)
{
    typedef std::uint32_t * (__cdecl *RawAllocFn)(int, std::uint32_t);
    static constexpr std::uintptr_t kRawAlloc    = 0x005aea00u;
    static constexpr std::uintptr_t kGlobalList  = 0x007dda80u;
    static constexpr std::uintptr_t kHdrPool     = 0x007ddab0u;    // U-1736 [STRUCT GAP]

    const auto rawAlloc = reinterpret_cast<RawAllocFn>(kRawAlloc);

    // Normalise alignment: force >= 1
    if (param_3 == 0u) param_3 = 1u;

    // Compute aligned element size
    const std::uint32_t aligned_size = (static_cast<std::uint32_t>(param_1) + param_3 - 1u) & ~(param_3 - 1u);
    const std::uint32_t bit_count    = param_2;
    const std::uint32_t bitmap_bytes = (bit_count + 7u) >> 3;

    // Acquire or allocate pool header (9 dwords = 0x24 bytes)
    std::uint32_t *hdr = param_5;
    if (!hdr) {
        // Try secondary pool at DAT_007ddab0 first; if null, use raw allocator
        auto *hdrPool = *reinterpret_cast<std::uint32_t **>(kHdrPool);
        if (hdrPool) {
            // Return a slot from the pool-header pool (pool_return reverse: use pool_alloc)
            // Mechanical: pool at kHdrPool supplies fixed-size header slots.
            // [STRUCT GAP] exact alloc protocol from hdrPool not fully mapped.
            hdr = rawAlloc(0x24, static_cast<std::uint32_t>(param_6));
        } else {
            hdr = rawAlloc(0x24, static_cast<std::uint32_t>(param_6));
        }
        if (!hdr) return nullptr;
        // Ownership flag = 2 (heap-allocated header)
        hdr[6] = 2u;
    } else {
        // Ownership flag = 3 (externally supplied header)
        hdr[6] = 3u;
    }

    // Initialise pool header fields
    hdr[0] = aligned_size;
    hdr[1] = bit_count;
    hdr[2] = bitmap_bytes;
    hdr[3] = param_3;

    // Block list: circular, initially empty (points to self at hdr[4])
    hdr[4] = reinterpret_cast<std::uint32_t>(hdr + 4);
    hdr[5] = reinterpret_cast<std::uint32_t>(hdr + 4);

    // Link into global pool list at DAT_007dda80 (circular doubly-linked)
    auto *globalHead = reinterpret_cast<std::uint32_t *>(kGlobalList);
    std::uint32_t *globalPrev = reinterpret_cast<std::uint32_t *>(globalHead[1]);

    hdr[7] = reinterpret_cast<std::uint32_t>(globalHead);
    hdr[8] = reinterpret_cast<std::uint32_t>(globalPrev);
    globalHead[1] = reinterpret_cast<std::uint32_t>(hdr + 7);
    globalPrev[0] = reinterpret_cast<std::uint32_t>(hdr + 7);

    // Pre-allocate blocks if param_4 > 0
    // [STRUCT GAP] block allocation protocol deferred — not fully mapped.
    // param_4 pre-alloc count noted but block-alloc callee not identified here.
    (void)param_4;

    return hdr;
}

RH_ScopedInstall(AudioPoolConstruct, 0x005ae650);

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

RH_ScopedInstall(AudioRwsSubZeroInit, 0x005be190);

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

RH_ScopedInstall(AudioSubStructThreeWrite, 0x005be140);

// NOTE: 0x005ab410 (AudioRwsChunkTypeSeek) was already implemented in
// Audio/RwsStream.cpp (c3-batch-i-s1). No duplicate here.
// hooks.csv row for 005ab410 is drift-staged at C2; re-classify in this session
// promotes it to C3 using the existing AudioRwsChunkTypeSeek implementation.

