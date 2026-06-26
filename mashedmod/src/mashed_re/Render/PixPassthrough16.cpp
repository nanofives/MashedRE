// Mashed RE - Render/PixPassthrough16.cpp
// 16-bit pixel passthrough getter (C2->C3 worktree wf_b0f68acd).
//
// Covers:
//   0x004dfaa0  PixPassthrough16  -- 16-bit passthrough: returns *(uint16*)p
//
// Binary anchor (unpatched):
//   MASHED.exe SHA-256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis note: re/analysis/bucket_004ddfb0/0x004dfaa0.md
//
// Function is a pure leaf getter: no callees, no globals read/written, no live state.
// Decoder match: D3DFORMAT 0x3c / 0x75 (both arms in FUN_004df750 dispatch).

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x004dfaa0  FUN_004dfaa0  PixPassthrough16   (13 bytes)
//
// uint32 PixPassthrough16(const uint8_t *p)
//   param_1 = pointer to 2-byte (16-bit) input buffer  [ESP+4 at 0x004dfaa0]
//   Returns: (p[1]<<8) | p[0]  — little-endian 16-bit word in EAX
//
// Asm (RVA 0x004dfaa0..0x004dfaad):
//   004dfaa0  MOV EAX, [ESP+0x4]    ; load p from stack
//   004dfaa4  XOR ECX, ECX          ; clear ECX
//   004dfaa6  MOV CH, [EAX+0x1]     ; CH = p[1]
//   004dfaa9  MOV CL, [EAX]         ; CL = p[0]
//   004dfaab  MOV EAX, ECX          ; EAX = (p[1]<<8) | p[0]
//   004dfaad  RET                   ; cdecl — no stack cleanup
//
// Calling convention: __cdecl (single stack arg, plain RET).
// Return type: uint32 (low 16 bits only filled via ECX byte-load).
// No constants. No side effects.
// ---------------------------------------------------------------------------

// 0x004dfaa0
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixPassthrough16(const std::uint8_t* p)
{
    // Byte-by-byte LE read matching asm: CL=p[0], CH=p[1], return ECX
    return (std::uint32_t)p[0] | ((std::uint32_t)p[1] << 8u);
}

RH_ScopedInstall(PixPassthrough16, 0x004dfaa0);
