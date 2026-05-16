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
