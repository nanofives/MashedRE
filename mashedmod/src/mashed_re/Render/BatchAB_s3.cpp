// Mashed RE — Render/BatchAB_s3.cpp
// Pixel-format BGRA encoder leaves (c3-batch-ab session s3).
//
// Covers (bucket 0x004ddfb0 encoders):
//   0x004df8d0  PixEncode1555     — 16-bit A1R5G5B5 packer from BGRA bytes
//   0x004df910  PixEncode4444     — 16-bit A4R4G4B4 packer from BGRA bytes
//   0x004df950  PixEncodeA8R3G3B2 — 16-bit A8R3G3B2 packer from 4-byte src
//   0x004df980  PixEncodeX4R4G4B4 — 16-bit X4R4G4B4 packer from BGR bytes (alpha forced 0xf)
//   0x004df9b0  PixEncodeR6G5B5   — 16-bit R6G5B5 packer from BGR bytes (byte-reversed 565)
//   0x004df9e0  PixEncodeX8R8G8B8 — 32-bit X8R8G8B8 packer from BGR bytes (alpha forced 0xff)
//
// Binary anchor (unpatched):
//   MASHED.exe SHA-256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis notes:
//   re/analysis/bucket_004ddfb0/0x004df8d0.md
//   re/analysis/bucket_004ddfb0/0x004df910.md
//   re/analysis/bucket_004ddfb0/0x004df950.md
//   re/analysis/bucket_004ddfb0/0x004df980.md
//   re/analysis/bucket_004ddfb0/0x004df9b0.md
//   re/analysis/bucket_004ddfb0/0x004df9e0.md
//
// All six functions are pure leaf encoders: no callees, no globals read/written,
// no live-state access.  Each takes a pointer to a 3- or 4-byte input buffer
// and returns a packed pixel word.

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x004df8d0  FUN_004df8d0  PixEncode1555   (52 bytes)
//
// uint PixEncode1555(byte *bgra)
//   param_1 = pointer to 4-byte BGRA input buffer
//   Returns 16-bit A1R5G5B5 packed word.
//
// Formula from re/analysis/bucket_004ddfb0/0x004df8d0.md:
//   (((b[3] & 0x80) * 2 | b[0] & 0xf8) << 5 | b[1] & 0xf8) << 2 | (b[2] >> 3)
//
// Constants cited in note:
//   0x80  — alpha bit mask (high-bit-only)
//   0xf8  — 5-bit truncation mask
//
// Decoder match: D3DFORMAT 0x19 (D3DFMT_A1R5G5B5).
// ---------------------------------------------------------------------------

// 0x004df8d0
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixEncode1555(std::uint8_t* bgra)
{
    // Alpha bit from high bit of b[3], cited 0x004df8d0 analysis
    std::uint32_t b0 = bgra[0];
    std::uint32_t b1 = bgra[1];
    std::uint32_t b2 = bgra[2];
    std::uint32_t b3 = bgra[3];
    // (b[3] & 0x80) * 2 | b[0] & 0xf8) << 5 | b[1] & 0xf8) << 2 | (b[2] >> 3)
    return ((((b3 & 0x80u) * 2u | b0 & 0xf8u) << 5u | b1 & 0xf8u) << 2u | (b2 >> 3u));
}

RH_ScopedInstall(PixEncode1555, 0x004df8d0);

// ---------------------------------------------------------------------------
// 0x004df910  FUN_004df910  PixEncode4444   (50 bytes)
//
// uint PixEncode4444(byte *bgra)
//   param_1 = pointer to 4-byte BGRA input buffer
//   Returns 16-bit A4R4G4B4 packed word.
//
// Formula from re/analysis/bucket_004ddfb0/0x004df910.md:
//   ((b[3] & 0xf0) << 4 | b[0] & 0xf0) << 4 | b[1] & 0xf0 | (b[2] >> 4)
//
// Constants cited in note:
//   0xf0  — 4-bit truncation mask
//
// Decoder match: D3DFORMAT 0x1a (D3DFMT_A4R4G4B4).
// ---------------------------------------------------------------------------

// 0x004df910
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixEncode4444(std::uint8_t* bgra)
{
    std::uint32_t b0 = bgra[0];
    std::uint32_t b1 = bgra[1];
    std::uint32_t b2 = bgra[2];
    std::uint32_t b3 = bgra[3];
    // ((b[3] & 0xf0) << 4 | b[0] & 0xf0) << 4 | b[1] & 0xf0 | (b[2] >> 4)
    return (((b3 & 0xf0u) << 4u | b0 & 0xf0u) << 4u | b1 & 0xf0u | (b2 >> 4u));
}

RH_ScopedInstall(PixEncode4444, 0x004df910);

// ---------------------------------------------------------------------------
// 0x004df950  FUN_004df950  PixEncodeA8R3G3B2   (43 bytes)
//
// uint PixEncodeA8R3G3B2(byte *src4)
//   param_1 = pointer to 4-byte input buffer
//   Returns 16-bit packed word consistent with D3DFMT_A8R3G3B2 (0x1d).
//
// Formula from re/analysis/bucket_004ddfb0/0x004df950.md:
//   (b[1] & 0xe0 | (b[2] >> 3)) >> 3 | (b[3] << 8) | (b[0] & 0xe0)
//
// Constants cited in note:
//   0xe0  — 3-bit truncation mask
//
// Uncertainty U-5550: whether the RW name is A8R3G3B2 vs internal format;
//   formula is consistent with A8R3G3B2.
// ---------------------------------------------------------------------------

// 0x004df950
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixEncodeA8R3G3B2(std::uint8_t* src4)
{
    std::uint32_t b0 = src4[0];
    std::uint32_t b1 = src4[1];
    std::uint32_t b2 = src4[2];
    std::uint32_t b3 = src4[3];
    // (b[1] & 0xe0 | (b[2] >> 3)) >> 3 | (b[3] << 8) | (b[0] & 0xe0)
    return ((b1 & 0xe0u | (b2 >> 3u)) >> 3u | (b3 << 8u) | (b0 & 0xe0u));
}

RH_ScopedInstall(PixEncodeA8R3G3B2, 0x004df950);

// ---------------------------------------------------------------------------
// 0x004df980  FUN_004df980  PixEncodeX4R4G4B4   (39 bytes)
//
// uint PixEncodeX4R4G4B4(byte *bgr)
//   param_1 = pointer to 3-byte BGR input buffer (b[3] not consumed)
//   Returns 16-bit X4R4G4B4 packed word with alpha nibble forced to 0xf.
//
// Formula from re/analysis/bucket_004ddfb0/0x004df980.md:
//   (CONCAT11(0xf, b[0] & 0xf0) << 4) | (b[1] & 0xf0) | (b[2] >> 4)
//   where CONCAT11(0xf, b[0] & 0xf0) = (0x0f << 8) | (b[0] & 0xf0)
//
// Constants cited in note:
//   0xf   — forced alpha nibble
//   0xf0  — 4-bit truncation mask
//
// Arithmetic: (0x0f00 | (b[0] & 0xf0)) << 4 = 0xf000 | ((b[0] & 0xf0) << 4)
// Decoder match: D3DFORMAT 0x1e (D3DFMT_X4R4G4B4).
// ---------------------------------------------------------------------------

// 0x004df980
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixEncodeX4R4G4B4(std::uint8_t* bgr)
{
    std::uint32_t b0 = bgr[0];
    std::uint32_t b1 = bgr[1];
    std::uint32_t b2 = bgr[2];
    // CONCAT11(0xf, b[0] & 0xf0) = (0x0fu << 8u) | (b0 & 0xf0u)
    std::uint32_t concat = (0x0fu << 8u) | (b0 & 0xf0u);
    // (concat << 4) | (b[1] & 0xf0) | (b[2] >> 4)
    return ((concat << 4u) | (b1 & 0xf0u) | (b2 >> 4u));
}

RH_ScopedInstall(PixEncodeX4R4G4B4, 0x004df980);

// ---------------------------------------------------------------------------
// 0x004df9b0  FUN_004df9b0  PixEncodeR6G5B5   (39 bytes)
//
// uint PixEncodeR6G5B5(byte *bgr)
//   param_1 = pointer to 3-byte input buffer
//   Returns 16-bit R6G5B5 packed word.
//   Source layout (byte-reversed vs FUN_004df8a0 / D3DFMT_R5G6B5):
//     b[0] = B channel (5 bits at result[4..0])
//     b[1] = G channel (5 bits at result[9..5])
//     b[2] = R channel (6 bits at result[15..10])
//
// Formula from re/analysis/bucket_004ddfb0/0x004df9b0.md:
//   ((b[2] & 0xfc) << 6 | b[1] & 0xf8) << 2 | (b[0] >> 3)
//
// Disasm (RVA 0x004df9b0, 39 bytes, plain ret = cdecl):
//   mov ecx,[esp+4]   ; ecx = src ptr
//   mov al,[ecx+2]    ; AL  = b[2]
//   mov dl,[ecx+1]    ; DL  = b[1]
//   and eax,0xfc      ; AL &= 0xfc (6 bits)
//   and edx,0xf8      ; DL &= 0xf8 (5 bits)
//   shl eax,6         ; EAX <<= 6  -> bits 13..8
//   or  eax,edx       ; | b[1]&0xf8 -> bits 7..3
//   xor edx,edx
//   mov dl,[ecx]      ; DL  = b[0]
//   shl eax,2         ; EAX <<= 2  -> b[2] at 15..10, b[1] at 9..5
//   shr edx,3         ; EDX = b[0]>>3 -> 5 bits at 4..0
//   or  eax,edx       ; combine
//   ret
//
// Constants cited in note:
//   0xfc  — 6-bit truncation mask (b[2] keeps bits 7..2)
//   0xf8  — 5-bit truncation mask (b[1] keeps bits 7..3)
//
// Decoder match: D3DFORMAT 0x3d (non-standard; [UNCERTAIN U-5551] internal RW id).
// ---------------------------------------------------------------------------

// 0x004df9b0
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixEncodeR6G5B5(std::uint8_t* bgr)
{
    std::uint32_t b0 = bgr[0];  // B (5 bits at result[4..0])
    std::uint32_t b1 = bgr[1];  // G (5 bits at result[9..5])
    std::uint32_t b2 = bgr[2];  // R (6 bits at result[15..10])
    // ((b[2] & 0xfc) << 6 | b[1] & 0xf8) << 2 | (b[0] >> 3)
    return (((b2 & 0xfcu) << 6u | b1 & 0xf8u) << 2u | (b0 >> 3u));
}

RH_ScopedInstall(PixEncodeR6G5B5, 0x004df9b0);

// ---------------------------------------------------------------------------
// 0x004df9e0  FUN_004df9e0  PixEncodeX8R8G8B8   (34 bytes)
//
// uint PixEncodeX8R8G8B8(byte *bgr)
//   param_1 = pointer to 3-byte BGR input buffer
//   Returns 32-bit X8R8G8B8 packed word with alpha byte forced to 0xff.
//
// Formula from re/analysis/bucket_004ddfb0/0x004df9e0.md:
//   ((b[0] | 0xffffff00) << 8 | b[1]) << 8 | b[2]
//   The `| 0xffffff00` forces the top byte to 0xff regardless of b[0] high bit.
//
// Constants cited in note:
//   0xffffff00 — forces alpha to 0xff
//
// Arithmetic (32-bit):
//   (0xffffff00u | b[0]) << 8 = 0xffff0000 | (b[0] << 8)   [32-bit mod 2^32]
//   then | b[1]:                0xffff0000 | (b[0] << 8) | b[1]
//   then << 8:                  0xff000000 | (b[0] << 16) | (b[1] << 8)
//   then | b[2]:                0xff000000 | (b[0] << 16) | (b[1] << 8) | b[2]
//
// Decoder match: D3DFORMAT 0x16 (D3DFMT_X8R8G8B8).
// ---------------------------------------------------------------------------

// 0x004df9e0
extern "C" __declspec(dllexport) std::uint32_t __cdecl
PixEncodeX8R8G8B8(std::uint8_t* bgr)
{
    std::uint32_t b0 = bgr[0];
    std::uint32_t b1 = bgr[1];
    std::uint32_t b2 = bgr[2];
    // ((b[0] | 0xffffff00) << 8 | b[1]) << 8 | b[2]
    return (((b0 | 0xffffff00u) << 8u | b1) << 8u | b2);
}

RH_ScopedInstall(PixEncodeX8R8G8B8, 0x004df9e0);
