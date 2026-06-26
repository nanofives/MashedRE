// Mashed RE — RGB byte-array to packed-DWORD encoder (worktree wf_b0f68acd).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Promotion target: 0x004dfa10  FUN_004dfa10  C2->C3
// Subsystem: render
// arg_type:  ptr_arg_int_get

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RgbPackEncoder4dfa10  --  0x004dfa10
//
// Original: FUN_004dfa10 (34 bytes, 0x004dfa10..0x004dfa31)
// Calling convention: __cdecl (plain RET at 0x004dfa31; one stack arg [ESP+4])
// Signature: uint32_t FUN_004dfa10(const uint8_t* rgb)
//
// Disassembly (verified via Ghidra, 2026-06-26):
//   004dfa10  MOV ECX, dword ptr [ESP + 0x4]   ; ECX = rgb ptr
//   004dfa14  XOR EAX, EAX
//   004dfa16  XOR EDX, EDX
//   004dfa18  MOV AL, byte ptr [ECX + 0x2]     ; AL = rgb[2] (B channel)
//   004dfa1b  MOV DL, byte ptr [ECX + 0x1]     ; DL = rgb[1] (G channel)
//   004dfa1e  OR  EAX, 0xffffff00              ; force upper 24 bits set
//   004dfa23  SHL EAX, 0x8                     ; EAX = 0xFFFF[rgb2]00
//   004dfa26  OR  EAX, EDX                     ; EAX = 0xFFFF[rgb2][rgb1]
//   004dfa28  XOR EDX, EDX
//   004dfa2a  MOV DL, byte ptr [ECX]           ; DL = rgb[0] (R channel)
//   004dfa2c  SHL EAX, 0x8                     ; EAX = 0xFF[rgb2][rgb1]00
//   004dfa2f  OR  EAX, EDX                     ; EAX = 0xFF[rgb2][rgb1][rgb0]
//   004dfa31  RET
//
// Returns a packed 32-bit DWORD with alpha=0xFF in the top byte and the three
// input bytes packed in channels [2][1][0] order (BGRA or XBGR layout).
// The 0xffffff00 OR ensures the top byte is always 0xFF after two 8-bit
// left shifts (proven by carry-chain: 0xffffff00 << 8 = 0xffff0000, +rgb2 <<
// 8 = 0xff[rgb2]00, +rgb0 = 0xff[rgb2][rgb1][rgb0]).
//
// Companion: FUN_004df9e0 reverses channel order (noted in analysis).
// Decoder dispatch: D3DFORMAT 0x21 / 0x3e arms in FUN_004df750.
// Callers cited: FUN_004e02d0 (render; C2).
//
// ref: re/analysis/bucket_004ddfb0/0x004dfa10.md
// ---------------------------------------------------------------------------

// 0x004dfa10
extern "C" __declspec(dllexport) std::uint32_t __cdecl RgbPackEncoder4dfa10(const std::uint8_t* rgb)
{
    // [0x004dfa18] AL = rgb[2]; [0x004dfa1e] OR 0xffffff00; [0x004dfa23] SHL 8
    // [0x004dfa26] OR rgb[1];   [0x004dfa2c] SHL 8;         [0x004dfa2f] OR rgb[0]
    return (((static_cast<std::uint32_t>(rgb[2]) | 0xffffff00u) << 8u)
             | static_cast<std::uint32_t>(rgb[1])) << 8u
           | static_cast<std::uint32_t>(rgb[0]);
}

RH_ScopedInstall(RgbPackEncoder4dfa10, 0x004dfa10);
