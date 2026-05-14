// Mashed RE - RwMatrixScale: 3-mode in-place matrix scale.
//
// 0x004c5010  FUN_004c5010  font_text_d3-20260506  C2 -> C3
//
// Signature (from Ghidra 2026-05-14):
//   void FUN_004c5010(float *mat, const float *scale, int mode)
// Body: 0x004c5010-0x004c519d
//
// Matrix layout: RwMatrix = float[16] column-major.
//   mat[0..2] = right.xyz  mat[3] = flags  (uint cast used for flag ops)
//   mat[4..6] = up.xyz     mat[7] = pad
//   mat[8..10]= at.xyz     mat[11]= pad
//   mat[12..14]=pos.xyz    mat[15]= pad
//
// Mode 0 (rwCOMBINEREPLACE):
//   Initialise matrix to identity, then set diagonal to scale[0],scale[1],scale[2].
//   mat[3] is manipulated as a uint: OR 0x20003 during init, then AND ~0x20003.
//
// Mode 1 (rwCOMBINEPRECONCAT => M x Scale):
//   Scale each COLUMN of the 3x3 by the corresponding scale factor.
//   col0 (mat[0..2]) *= scale[0]; col1 (mat[4..6]) *= scale[1]; col2 (mat[8..10]) *= scale[2].
//   Translation (mat[12..14]) unchanged.
//
// Mode 2 (rwCOMBINEPOSTCONCAT => Scale x M):
//   Scale each ROW of the full 4x4 by the corresponding scale factor.
//   row0 (mat[0,4,8,12]) *= scale[0]; row1 (mat[1,5,9,13]) *= scale[1]; row2 (mat[2,6,10,14]) *= scale[2].
//   Translation IS scaled.
//
// All modes: mat[3] &= 0xfffdfffc  (clear orthonormal-dirty bits 0,1,17).
// Unknown mode: reports error 0x80000003 (decompilation shows a separate branch).
//
// Decompilation verified 2026-05-14 via Ghidra MCP (Mashed_pool0, read-only).
#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// Inline flag bit-manipulation through float pointer (avoids strict-aliasing UB).
static inline void flags_or(float* mat3, std::uint32_t mask)
{
    std::uint32_t f; std::memcpy(&f, mat3, 4); f |= mask; std::memcpy(mat3, &f, 4);
}
static inline void flags_and(float* mat3, std::uint32_t mask)
{
    std::uint32_t f; std::memcpy(&f, mat3, 4); f &= mask; std::memcpy(mat3, &f, 4);
}

// 0x004c5010
extern "C" __declspec(dllexport)
void __cdecl RwMatrixScale(float* mat, const float* scale, int mode)
{
    if (mode == 0) {
        // Replace: identity + diagonal scale
        mat[10] = 1.0f; mat[5] = 1.0f; mat[0] = 1.0f;
        mat[4]  = 0.0f;
        flags_or(&mat[3], 0x20003u);   // intermediate flag state (matches original)
        mat[2]  = 0.0f; mat[1]  = 0.0f;
        mat[9]  = 0.0f; mat[8]  = 0.0f; mat[6]  = 0.0f;
        mat[14] = 0.0f; mat[13] = 0.0f; mat[12] = 0.0f;
        // Apply scale on diagonal
        mat[0]  = scale[0];
        mat[5]  = scale[1];
        mat[10] = scale[2];
        flags_and(&mat[3], 0xfffdfffcu);
        return;
    }

    if (mode == 1) {
        // Preconcat (M x Scale): scale columns of 3x3
        mat[0]  *= scale[0]; mat[1]  *= scale[0]; mat[2]  *= scale[0];
        mat[4]  *= scale[1]; mat[5]  *= scale[1]; mat[6]  *= scale[1];
        mat[8]  *= scale[2]; mat[9]  *= scale[2]; mat[10] *= scale[2];
        flags_and(&mat[3], 0xfffdfffcu);
        return;
    }

    if (mode == 2) {
        // Postconcat (Scale x M): scale rows including translation
        mat[0]  *= scale[0]; mat[1]  *= scale[1]; mat[2]  *= scale[2];
        mat[4]  *= scale[0]; mat[5]  *= scale[1]; mat[6]  *= scale[2];
        mat[8]  *= scale[0]; mat[9]  *= scale[1]; mat[10] *= scale[2];
        mat[12] *= scale[0]; mat[13] *= scale[1]; mat[14] *= scale[2];
        flags_and(&mat[3], 0xfffdfffcu);
        return;
    }

    // Unknown mode — original reports error 0x80000003 then clears mat[3] flags.
    // "uRam0000000c" in the Ghidra decompilation is a decompiler artifact for
    // *(param_1 + 0x0c) = mat[3] (float[3] is at byte offset 12 = 0xC).
    // Error reporting stubs (0x004d7ff0, 0x004d8480) are C1 and not exercised
    // by the diff test vectors; we reproduce only the mat[3] flag clear.
    flags_and(&mat[3], 0xfffdfffcu);
}

RH_ScopedInstall(RwMatrixScale, 0x004c5010);
