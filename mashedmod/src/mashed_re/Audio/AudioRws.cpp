// Mashed RE - Audio RWS format-descriptor utility functions.
// Four pure leaf utilities in the RWS audio format-descriptor cluster.
// None have callees deeper than C2; all promote to C3 in c3-batch-f-s2.
//
// Source analysis: re/analysis/promote_c2_audio_rws/005aca80.md
//                  re/analysis/promote_c2_audio_rws/005acd10.md
//                  re/analysis/promote_c2_audio_rws/005ac980.md
//                  re/analysis/promote_c2_audio_rws/005acd60.md
//
// D4-AUDIO-RWS3 slice 3 (2026-10-24): this file now links into BOTH targets (asi_sources.rsp + exe_sources.rsp).
// Calls to ported callees go by name (AudioFmtEntryMatch, AudioFmtKeyCompare, AudioPoolFree,
// AudioTreeWalkPredicateSearch, and the sub-struct family inside this file).
// Bytes: verify/audiorws3_20261024/disasm_rws3.txt. PREREG: verify/audiorws3_20261024/PREREG_rws3.md.
//
// Exe arm (MASHED_STANDALONE): RwFree 0x004522d0 = CRT free (Audio/AudioMemory.cpp's declared deviation); the two
// pool headers 0x007dda28 / 0x007dd634 are private zero storage (both past .data's raw end, BSS until the audio open,
// same as slice 2's pools); the 0x00633674 table is a private copy of its 9 pointers onto private copies of the
// 16-byte .rdata keys (verify/audiorws3_20261024/table_633674.txt); LAB_005ac930 (U-1730) is ported from the bytes
// for AudioContextLookup. Nothing in the exe calls these functions yet. .asi arm: the original's addresses.
#include "../Core/HookSystem.h"
#include "AudioFmtKey.h"
#include "AudioRwsCallees.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {
#ifdef MASHED_STANDALONE
std::uint32_t s_audioSubPool_007dda28[9] = {};     // stands in for DAT_007dda28 (pool header, 0x24 bytes)
std::uint32_t s_audioWavePool_007dd634[9] = {};    // stands in for DAT_007dd634 (pool header, 0x24 bytes)
std::uint32_t* SubStructPool() { return s_audioSubPool_007dda28; }
std::uint32_t* WaveNodePool()  { return s_audioWavePool_007dd634; }
void RwsRawFree(void* raw)     { std::free(raw); }

// The 16-byte keys at 0x005e6414.. (.rdata), and the 0x00633674 table's 9 pointers onto them, in table order.
const std::uint8_t s_fmtKey_005e6414[16] = {0x98,0x97,0xea,0xd9,0xbc,0xbb,0x7b,0x44,0x96,0xb2,0x65,0x47,0x59,0x10,0x2e,0x16};
const std::uint8_t s_fmtKey_005e6424[16] = {0x17,0xd2,0x1b,0xd0,0x87,0x35,0xed,0x4e,0xb9,0xd9,0xb8,0xe8,0x6e,0xa9,0xb9,0x95};
const std::uint8_t s_fmtKey_005e6434[16] = {0x82,0x43,0x1e,0xda,0x99,0x2c,0x61,0x4c,0xad,0x99,0x7f,0x36,0x4b,0x21,0x15,0x37};
const std::uint8_t s_fmtKey_005e6444[16] = {0xb0,0x15,0x62,0xf8,0xd5,0x31,0x29,0x4c,0xbd,0x37,0xcd,0xbf,0x9b,0xd1,0x0c,0x53};
const std::uint8_t s_fmtKey_005e6454[16] = {0x2b,0xa2,0x2f,0x63,0xdd,0x11,0x8f,0x45,0xaa,0x27,0xa5,0xc3,0x46,0xe9,0x79,0x0e};
const std::uint8_t s_fmtKey_005e6464[16] = {0x47,0x81,0x1d,0x3f,0xc4,0xb7,0xe6,0x41,0xa6,0x9b,0x3c,0xc0,0x02,0x5b,0x33,0xc7};
const std::uint8_t s_fmtKey_005e6474[16] = {0x6e,0xb3,0xcf,0xba,0x9d,0x52,0x92,0x46,0xbf,0x53,0x32,0x42,0x56,0xb0,0x73,0x4f};
const std::uint8_t s_fmtKey_005e64a4[16] = {0x90,0xb3,0x0d,0xa3,0xa9,0x58,0xc4,0x43,0xb9,0xd2,0x55,0xd8,0x4d,0x3a,0xe7,0x54};
const std::uint8_t s_fmtKey_005e64b4[16] = {0x93,0x65,0x38,0xef,0x11,0xb6,0x2d,0x43,0x95,0x7f,0xa7,0x1a,0xde,0x44,0x22,0x7a};
const std::uint8_t* const s_fmtTable_00633674[9] = {
    s_fmtKey_005e6414, s_fmtKey_005e6424, s_fmtKey_005e6434, s_fmtKey_005e6444, s_fmtKey_005e6454,
    s_fmtKey_005e64a4, s_fmtKey_005e6464, s_fmtKey_005e6474, s_fmtKey_005e64b4,
};
const std::uint8_t* FmtTableEntry(std::uint32_t i) { return s_fmtTable_00633674[i]; }

// LAB_005ac930 (U-1730), the AudioContextLookup predicate, from the bytes (disasm_rws3.txt 0x005ac930..0x005ac976).
// cdecl (root, node, ctx): walks the ring at node+0x20 (sentinel node+0x20, next = [link]); per link it compares the
// key pointer at link-0x40 against ctx[0] (re-read each pass, 0x005ac946), on 0 writes ctx[1] = link-0x40
// (0x005ac96a) and returns 1; exhausted returns 0.
int __cdecl ContextKeyMatch_005ac930(std::uint32_t*, std::uint32_t* node, std::uint32_t ctx_addr)
{
    std::uint32_t* const ctx = reinterpret_cast<std::uint32_t*>(ctx_addr);
    std::uint32_t* const sentinel = node + 8;                                     // 0x005ac93a lea ebx,[eax+0x20]
    for (std::uint32_t* link = reinterpret_cast<std::uint32_t*>(node[8]);         // 0x005ac937
         link != sentinel;                                                        // 0x005ac93d / 0x005ac95f
         link = reinterpret_cast<std::uint32_t*>(*link)) {                        // 0x005ac95d
        std::uint32_t* const entry = link - 0x10;                                 // 0x005ac94c lea edi,[esi-0x40]
        if (AudioFmtKeyCompare(reinterpret_cast<const std::uint8_t*>(entry[0]),   // 0x005ac949 / 0x005ac951
                               reinterpret_cast<const std::uint8_t*>(ctx[0])) == 0) {
            ctx[1] = reinterpret_cast<std::uint32_t>(entry);                      // 0x005ac96a
            return 1;
        }
    }
    return 0;
}
void* ContextPredicate() { return reinterpret_cast<void*>(&ContextKeyMatch_005ac930); }
#else
std::uint32_t* SubStructPool() { return reinterpret_cast<std::uint32_t*>(0x007dda28u); }
std::uint32_t* WaveNodePool()  { return reinterpret_cast<std::uint32_t*>(0x007dd634u); }
void RwsRawFree(void* raw) {
    typedef void (__cdecl *FreeFn)(void*);
    reinterpret_cast<FreeFn>(0x004522d0u)(raw);
}
const std::uint8_t* FmtTableEntry(std::uint32_t i) {
    return *reinterpret_cast<const std::uint8_t* const*>(0x00633674u + i * 4u);   // PTR_DAT_00633674
}
void* ContextPredicate() { return reinterpret_cast<void*>(0x005ac930u); }        // LAB_005ac930, the original's
#endif
}  // namespace

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
    // 0x005acd33: AudioFmtEntryMatch (Audio/RwsFmt.cpp, both targets), by name.
    const auto matchPredicate = [](int entry_ptr, int desc_ptr) -> int {
        return static_cast<int>(AudioFmtEntryMatch(reinterpret_cast<std::uint32_t*>(entry_ptr),
                                                   reinterpret_cast<std::uint32_t*>(desc_ptr)));
    };

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
    static constexpr uint32_t kEntryCount = 9u;              // 0x005acd7e cmp esi,9

    for (uint32_t i = 0u; i < kEntryCount; ++i) {
        // 0x005acd68 / 0x005acd71: AudioFmtKeyCompare(table[i], key) (Audio/AudioFmtKey.cpp, both targets), by name.
        if (AudioFmtKeyCompare(FmtTableEntry(i), reinterpret_cast<const std::uint8_t*>(param_1)) == 0) {
            // Match: 0x005acd88 re-reads table[i]
            return const_cast<std::uint8_t*>(FmtTableEntry(i));
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
        // FUN_005ae920(&DAT_007dda28, *param_1) — return block to pool (0x005ae097, AudioPoolFree by name)
        AudioPoolFree(SubStructPool(), static_cast<std::uint32_t>(*param_1));
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
        // FUN_004522d0(*(int*)(param_1+4)) — heap free (0x005ae063)
        RwsRawFree(reinterpret_cast<void*>(static_cast<uintptr_t>(*(base + 1))));
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
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
AudioSubStructLinkDevice(std::uint32_t* param_1, std::uint32_t param_2);
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
AudioSubStructLinkBuffer(std::uint32_t* param_1, std::uint32_t param_2);

extern "C" __declspec(dllexport) void* __cdecl AudioSubStructTwoCallInit(
        void* param_1,
        void* param_2,
        void* param_3)
{
    // 0x005a9e1b: FUN_005adfe0(param_1, param_3) — sub-struct B initializer (by name)
    AudioSubStructLinkBuffer(static_cast<std::uint32_t*>(param_1),
                             static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(param_3)));

    // 0x005a9e26: FUN_005ae010(param_1, param_2) — sub-struct A initializer (by name)
    AudioSubStructLinkDevice(static_cast<std::uint32_t*>(param_1),
                             static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(param_2)));

    // 0x005a9e2e: return param_1 (pass-through)
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
    // Callee 0x005ae016: releases old device handle if owned (no-op when *param_1 == 0). By name.
    AudioSubStructAFree(reinterpret_cast<int*>(param_1));

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
    // Callee 0x005adfe6: frees old buffer if owned (no-op when *(param_1+4) == 0). By name.
    AudioSubStructBFree(static_cast<int>(reinterpret_cast<std::uintptr_t>(param_1)));

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
    // 0x005ac7bb / 0x005ac7cf, by name.
    const std::uint32_t* r1 = AudioSubStructLinkDevice(
        reinterpret_cast<std::uint32_t*>(param_1), param_2);
    if (r1 == nullptr) return 0u;

    const std::uint32_t* r2 = AudioSubStructLinkBuffer(
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
    // 0x005abcc8 / 0x005abcd6: AudioPoolFree by name. 0x005abce8: tail jmp RwFree 0x004522d0.
    const std::uint32_t flags = *reinterpret_cast<std::uint32_t *>(param_1 + 0x54);

    if (flags & 0x1u) {
        // bit0 set — pool-return path
        const int contextPtr = *reinterpret_cast<int *>(param_1 + 0x0c);
        const int perCtxPool = *reinterpret_cast<int *>(contextPtr + 0x3c);
        if (perCtxPool == 0) {
            AudioPoolFree(WaveNodePool(), static_cast<std::uint32_t>(param_1));   // DAT_007dd634
        } else {
            AudioPoolFree(reinterpret_cast<std::uint32_t*>(perCtxPool), static_cast<std::uint32_t>(param_1));
        }
        return;
    }

    if (flags & 0x4u) {
        // bit2 set — externally owned / static; do nothing
        return;
    }

    // bit0 = 0, bit2 = 0 — heap-free
    RwsRawFree(reinterpret_cast<void *>(param_1));
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
    const std::uint8_t flags = *reinterpret_cast<std::uint8_t *>(param_1 + 0x18);
    const int bufPtr = *reinterpret_cast<int *>(param_1 + 0x10);

    if ((flags & 0x2u) == 0 && bufPtr != 0) {
        RwsRawFree(reinterpret_cast<void *>(bufPtr));               // 0x005ac756 RwFree 0x004522d0
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
    std::uint32_t ctx[2];
    ctx[0] = param_1;   // 0x005ac917
    ctx[1] = 0u;        // 0x005ac91b

    // 0x005ac923: AudioTreeWalkPredicateSearch (Audio/RwsStream.cpp, both targets) by name. Predicate: LAB_005ac930
    // (.asi: the original's; exe: ContextKeyMatch_005ac930 above).
    AudioTreeWalkPredicateSearch(nullptr, nullptr, ContextPredicate(),
                                 static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(ctx)), 1);

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
    // 0x005ad54b / 0x005ad556, by name.
    AudioSubStructLinkDevice(reinterpret_cast<std::uint32_t*>(param_1), param_2);
    AudioSubStructLinkBuffer(reinterpret_cast<std::uint32_t*>(param_1), param_3);
    return param_1;
}

RH_ScopedInstall(AudioSubStructLinkBoth, 0x005ad540);  // wf_b0f68acd r42 C2->C3

