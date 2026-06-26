// Mashed RE — Render/PixReadU32_wf.cpp
// Pixel-format 32-bit passthrough reader (c3-batch worktree wf).
//
// Covers (bucket 0x004ddfb0 pixel-format encoders):
//   0x004dfa70  PixReadU32  — 4-byte unaligned little-endian u32 read from byte ptr
//
// Binary anchor (unpatched):
//   MASHED.exe SHA-256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis note:
//   re/analysis/bucket_004ddfb0/0x004dfa70.md
//
// This is a pure leaf reader: no callees, no globals read/written.
// Takes a byte pointer; reads 4 bytes in little-endian order and returns
// the assembled uint32. Decoder match: D3DFORMAT 0x20 / 0x3f
// (both arms in FUN_004df750 dispatch).

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x004dfa70  FUN_004dfa70  PixReadU32   (33 bytes)
//
// uint PixReadU32(byte *p)
//   param_1 = pointer to 4-byte input buffer
//   Returns 32-bit little-endian assembly of bytes [0..3].
//
// Assembly (0x004dfa70 .. 0x004dfa91):
//   MOV EAX, [ESP+4]          ; EAX = p
//   XOR ECX, ECX
//   XOR EDX, EDX
//   MOV CH, [EAX+3]           ; ECX[15:8] = p[3]
//   MOV DL, [EAX+1]           ; EDX[7:0]  = p[1]
//   MOV CL, [EAX+2]           ; ECX[7:0]  = p[2]
//   SHL ECX, 8                 ; ECX = (p[3]<<16)|(p[2]<<8)
//   OR  ECX, EDX               ; ECX = (p[3]<<16)|(p[2]<<8)|p[1]
//   XOR EDX, EDX
//   MOV DL, [EAX]              ; EDX = p[0]
//   SHL ECX, 8                 ; ECX = (p[3]<<24)|(p[2]<<16)|(p[1]<<8)
//   OR  ECX, EDX               ; ECX = (p[3]<<24)|(p[2]<<16)|(p[1]<<8)|p[0]
//   MOV EAX, ECX
//   RET
//
// This is an unaligned little-endian 32-bit read:
//   result = p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24)
//   which equals *(uint32_t*)p on a little-endian machine.
//
// Decoder match: D3DFORMAT 0x20 / 0x3f (identity passthrough cases).
// ---------------------------------------------------------------------------

// 0x004dfa70
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixReadU32(std::uint8_t* p)
{
    // Byte-by-byte little-endian read — cited 0x004dfa70 assembly.
    // Identical to *(uint32_t*)p on LE but avoids alignment constraint.
    return ( (std::uint32_t)p[0]
           | ((std::uint32_t)p[1] << 8u)
           | ((std::uint32_t)p[2] << 16u)
           | ((std::uint32_t)p[3] << 24u) );
}

RH_ScopedInstall(PixReadU32, 0x004dfa70);
