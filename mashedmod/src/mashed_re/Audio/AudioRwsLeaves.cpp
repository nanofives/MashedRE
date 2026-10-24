// Mashed RE — Audio/AudioRwsLeaves.cpp
// Seven pure leaves split out of Audio/AudioRws.cpp (audiolink AudioRws slice 1, 2026-10-23) so they link into the exe.
// In both rsp files. No address, no callee outside this file (AudioWaveFmtCopy calls AudioFieldEndianPack by name).
// Bytes: verify/audiolink_20261023/disasm_rws1.txt (compared function by function; 0x005aca80's block also holds the
// unported 0x005acaa0, which starts at 0x005acaa0 with no padding). Four ports returned void where the bytes leave a
// pointer in EAX; they now return it.
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
// audiolink: the bytes leave EAX = param_2 (0x005ac988) at the ret 0x005ac9dc; the port returns it (was void).
extern "C" __declspec(dllexport) uint32_t* __cdecl AudioFmtDescCopy(
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
    return param_2;
}

RH_ScopedInstall(AudioFmtDescCopy, 0x005ac980);  // re-enabled 2026-05-24 phase-a1 fmt_desc_copy (buffer-alloc fix) GREEN

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
// audiolink: 15 bytes; the bytes leave EAX = param_1 at the ret, the port returns it (was void).
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
AudioSubStructZeroInit(std::uint32_t* param_1)
{
    param_1[2] = 0u;
    param_1[1] = 0u;
    param_1[0] = 0u;
    return param_1;
}

RH_ScopedInstall(AudioSubStructZeroInit, 0x005ae0b0);  // re-enabled 2026-05-24 c3-audio-b

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
// audiolink: 18 bytes; EAX = param_1 at the ret, returned (was void).
extern "C" __declspec(dllexport) std::uint32_t* __cdecl AudioRwsSubZeroInit(std::uint32_t* param_1)
{
    param_1[5] = 0u;   // offset 0x14
    param_1[3] = 0u;   // offset 0x0c
    param_1[4] = 0u;   // offset 0x10
    param_1[0] = 0u;   // offset 0x00
    return param_1;
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
// audiolink: 26 bytes, stores at 0x005be14c / 0x005be14f / 0x005be152 (the "cited" addresses above are off);
// EAX = param_1 at the ret, returned (was void).
extern "C" __declspec(dllexport) std::uint32_t* __cdecl AudioSubStructThreeWrite(
    std::uint32_t* param_1,   // pointer to sub-struct
    std::uint32_t  param_2,   // value to write at +0x14
    std::uint32_t  param_3)   // value to write at +0x10
{
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(param_1) + 0x14u) = param_2;  // cited 0x005be143
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(param_1) + 0x10u) = param_3;  // cited 0x005be149
    *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uint8_t*>(param_1) + 0x0cu) = 0u;       // cited 0x005be14f
    return param_1;
}

RH_ScopedInstall(AudioSubStructThreeWrite, 0x005be140);  // re-enabled 2026-05-24 c3-audio-b
