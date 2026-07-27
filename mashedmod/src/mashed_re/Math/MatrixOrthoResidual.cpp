// Mashed RE - x87 matrix orthonormality-residual leaves.
//
// Rows for the pointer-seeded ST0 float10-return diff arg_type `st0_ret_mat3_ptr`
// (HARNESS_BACKLOG #1 follow-on).
//
// 0x004c4270  FUN_004c4270  render  C2 -> C3 candidate
// 0x004c42d0  FUN_004c42d0  render  C2 -> C3 candidate
//
// LABEL CORRECTION (NO-GUESSING): re/analysis/plans/frontier_shape_refinement_2026-07-24.md
// lines 27-29 label 0x004c4270 / 0x004c42d0 / 0x004c4360 as "RwV3d bbox Y/X/Z accessors".
// The raw bytes disprove that. re/PROMOTION_QUEUE.md:285 (2026-06-08, 48 days EARLIER)
// had already recorded the disproof for 0x004c4270; the plan reasserts the label without
// reconciling it. Both functions below take ONE pointer arg at [ESP+4] and return ST0,
// reading nine dwords at {0x00,0x04,0x08, 0x10,0x14,0x18, 0x20,0x24,0x28} -- a 3-row,
// stride-0x10 float matrix (the RwMatrix right/up/at layout, pads at 0x0c/0x1c/0x2c
// never read). Neither is a single-field accessor.
//
// 0x004c4360 is a DIFFERENT SHAPE from this pair: it opens `SUB ESP,0x18` with a stack
// frame and reads a fourth row at +0x30/+0x34/+0x38. It is now ported at the bottom of
// this file (2026-07-27) and U-9022 is resolved -- it is the identity-deviation residual.
//
// Return convention: float10 in ST0, __cdecl, caller-cleaned (RET, not RET n).
// Declared `double` (NEVER void) -- a void-declared forward leaks the x87 stack
// (feedback memory x87_st0_float10_return_fnptr).
//
// Bit-identity: bodies are inline __asm mirroring the original instruction sequence
// operand-for-operand, so the x87 rounding at every intermediate is identical. The
// original ends with the MSVC discard idiom `FSTP ST(3); FSTP ST(0); FSTP ST(0)`
// (leaving the result in ST0 for RET); the reimpl instead stores the result with
// `FSTP qword ptr [result]` -- the same 80->64 round-to-nearest that libffi's
// NativeFunction('double') performs when reading ST0 on the original side -- and then
// pops the three leftovers to keep the x87 stack balanced. Same basis as
// Util/SineOscillators.cpp.
//
// [UNCERTAIN] The caller-side meaning of the residual (tolerance band, renormalise
// trigger, assert) is not derivable from these leaves. Reported mechanically only.
#include "../Core/HookSystem.h"

// 0x004c4270
// Disasm (Ghidra Mashed_pool0, read_only, 2026-07-27; bytes at 0x004c4270):
//   8b442404      MOV   EAX,[ESP+4]
//   d94024 d84814 d94020 d84810 dec1 d94028 d84818 dec1
//        A = ((m[0x24]*m[0x14]) + (m[0x20]*m[0x10])) + (m[0x28]*m[0x18])   ; dot(row1,row2)
//   d94024 d84804 d94020 d808   dec1 d94028 d84808 dec1
//        B = ((m[0x24]*m[0x04]) + (m[0x20]*m[0x00])) + (m[0x28]*m[0x08])   ; dot(row0,row2)
//   d94014 d84804 d94010 d808   dec1 d94018 d84808 dec1
//        C = ((m[0x14]*m[0x04]) + (m[0x10]*m[0x00])) + (m[0x18]*m[0x08])   ; dot(row0,row1)
//   d9c1 d8ca d9c3 d8cc dec1 d9c1 d8ca dec1
//        result = ((B*B) + (A*A)) + (C*C)
//   dddb ddd8 ddd8 c3   FSTP ST(3); FSTP ST(0); FSTP ST(0); RET
//
// Sum of squares of the three pairwise row dot products: zero exactly when the three
// rows are mutually orthogonal. Off-diagonal (orthogonality) residual.
extern "C" __declspec(dllexport)
double __cdecl MatrixOrthoResidual4c4270(const float* m)
{
    double result;
    __asm {
        mov   eax, m
        // A = dot(row1, row2)
        fld   dword ptr [eax+0x24]
        fmul  dword ptr [eax+0x14]
        fld   dword ptr [eax+0x20]
        fmul  dword ptr [eax+0x10]
        faddp st(1), st
        fld   dword ptr [eax+0x28]
        fmul  dword ptr [eax+0x18]
        faddp st(1), st
        // B = dot(row0, row2)
        fld   dword ptr [eax+0x24]
        fmul  dword ptr [eax+0x04]
        fld   dword ptr [eax+0x20]
        fmul  dword ptr [eax]
        faddp st(1), st
        fld   dword ptr [eax+0x28]
        fmul  dword ptr [eax+0x08]
        faddp st(1), st
        // C = dot(row0, row1)
        fld   dword ptr [eax+0x14]
        fmul  dword ptr [eax+0x04]
        fld   dword ptr [eax+0x10]
        fmul  dword ptr [eax]
        faddp st(1), st
        fld   dword ptr [eax+0x18]
        fmul  dword ptr [eax+0x08]
        faddp st(1), st
        // result = ((B*B) + (A*A)) + (C*C)      ; ST0=C ST1=B ST2=A
        fld   st(1)
        fmul  st, st(2)
        fld   st(3)
        fmul  st, st(4)
        faddp st(1), st
        fld   st(1)
        fmul  st, st(2)
        faddp st(1), st
        fstp  qword ptr [result]     // 80 -> 64, matches libffi 'double' read of ST0
        fstp  st(0)                  // discard C
        fstp  st(0)                  // discard B
        fstp  st(0)                  // discard A
    }
    return result;
}

RH_ScopedInstall(MatrixOrthoResidual4c4270, 0x004c4270);

// 0x004c42d0
// Disasm (Ghidra Mashed_pool0, read_only, 2026-07-27; bytes at 0x004c42d0).
// Per row r in {0x00, 0x10, 0x20}:
//     resid_r = ((m[r+4]*m[r+4] + m[r+0]*m[r+0]) + m[r+8]*m[r+8]) - *(float*)0x005cc320
// then the same tail as 0x004c4270:
//     result  = ((resid_1 * resid_1) + (resid_0 * resid_0)) + (resid_2 * resid_2)
//
// *(float*)0x005cc320 == 0x3f800000 == 1.0f  (memory_read at 0x005cc320, 2026-07-27).
// So each term is (|row|^2 - 1): zero exactly when the row is unit length. Diagonal
// (normality) residual -- the companion to 0x004c4270's off-diagonal residual.
//
// Row 0 uses a different-but-equivalent x87 register schedule in the original
// (FMULP ST(3) / FADDP ST(3) / FXCH ST(2)) than rows 1 and 2 (FLD ST(1) / FADDP ST(1));
// both are mirrored verbatim below rather than normalised, so the intermediate rounding
// order matches the original exactly.
extern "C" __declspec(dllexport)
double __cdecl MatrixNormResidual4c42d0(const float* m)
{
    double result;
    __asm {
        mov   eax, m
        mov   ecx, 0x005cc320        // &1.0f
        // ---- row 0 (offsets 0x00/0x04/0x08) ----
        fld   dword ptr [eax+0x04]
        fld   dword ptr [eax]
        fld   dword ptr [eax+0x08]
        fld   st(2)
        fmulp st(3), st
        fld   st(1)
        fmul  st, st(2)
        faddp st(3), st
        fld   st(0)
        fmul  st, st(1)
        faddp st(3), st
        fxch  st(2)
        fsub  dword ptr [ecx]
        fstp  st(2)
        fstp  st(0)                  // ST0 = resid_0
        // ---- row 1 (offsets 0x10/0x14/0x18) ----
        fld   dword ptr [eax+0x14]
        fld   dword ptr [eax+0x10]
        fld   dword ptr [eax+0x18]
        fld   st(1)
        fmul  st, st(2)
        fld   st(3)
        fmul  st, st(4)
        faddp st(1), st
        fld   st(1)
        fmul  st, st(2)
        faddp st(1), st
        fsub  dword ptr [ecx]
        fstp  st(3)
        fstp  st(0)
        fstp  st(0)                  // ST0 = resid_1, ST1 = resid_0
        // ---- row 2 (offsets 0x20/0x24/0x28) ----
        fld   dword ptr [eax+0x24]
        fld   dword ptr [eax+0x20]
        fld   dword ptr [eax+0x28]
        fld   st(1)
        fmul  st, st(2)
        fld   st(3)
        fmul  st, st(4)
        faddp st(1), st
        fld   st(1)
        fmul  st, st(2)
        faddp st(1), st
        fsub  dword ptr [ecx]
        fstp  st(3)
        fstp  st(0)
        fstp  st(0)                  // ST0=resid_2 ST1=resid_1 ST2=resid_0
        // result = ((resid_1^2) + (resid_0^2)) + (resid_2^2)
        fld   st(1)
        fmul  st, st(2)
        fld   st(3)
        fmul  st, st(4)
        faddp st(1), st
        fld   st(1)
        fmul  st, st(2)
        faddp st(1), st
        fstp  qword ptr [result]     // 80 -> 64, matches libffi 'double' read of ST0
        fstp  st(0)                  // discard resid_2
        fstp  st(0)                  // discard resid_1
        fstp  st(0)                  // discard resid_0
    }
    return result;
}

RH_ScopedInstall(MatrixNormResidual4c42d0, 0x004c42d0);

// ---------------------------------------------------------------------------
// 0x004c4360  FUN_004c4360  render  C2 -> C3 candidate      (resolves U-9022)
//
// Disasm from original/MASHED.exe.unpatched (SHA-anchored), 0x004c4360..0x004c4427.
// Shape: `double __cdecl(const float* m)` — ONE pointer arg at entry [ESP+4] (read as
// [ESP+0x1c] after `SUB ESP,0x18` at 0x004c4360), x87 float10 return in ST0.
//
// Reads TWELVE dwords — the full 4-row, stride-0x10 RwMatrix INCLUDING the translation
// row at 0x30/0x34/0x38. That extra row is why st0_ret_mat3_ptr (which allocates 0x30 and
// seeds only nine floats) cannot drive it; a new arg_type `st0_ret_mat4x3_ptr` was
// authored for this leaf.
//
// It accumulates ||M - I||^2: the top-left 3x3 measured against the identity diagonal
// (each diagonal element minus the 1.0f at 0x005cc320) and the translation row measured
// against zero.
//   d0 = m[0x00]-1  d1 = m[0x14]-1  d2 = m[0x28]-1      0x004c4367..0x004c4381
//   T0 = (m01^2 + m02^2) + d0^2                          0x004c439c..0x004c43c6
//   T3 = (m30^2 + m31^2) + m32^2                         0x004c43c8..0x004c43e2
//   T2 = (m20^2 + m21^2) + d2^2                          0x004c43e6..0x004c43fc
//   T1 = (m10^2 + m12^2) + d1^2                          0x004c4400..0x004c4416
//   return ((T0 + T3) + T2) + T1                         0x004c43e4/0x004c43fe/0x004c4418
//
// This CONFIRMS the role U-9021 predicted from the caller: FUN_004c4530
// (= RwMatrixOptimize) compares this return against tolerance slot [2] and uses it to gate
// bit 0x20000 = rwMATRIXINTERNALIDENTITY. The "RwV3d bbox Z accessor" label at
// re/analysis/plans/frontier_shape_refinement_2026-07-24.md:29 is RETRACTED — an accessor
// takes no tolerance, reads no fourth row, and gates no flag.
//
// The original spills seven of the matrix floats to its own stack frame ([ESP+0x1c], [ESP],
// [ESP+4], [ESP+8], [ESP+0xc], [ESP+0x10], [ESP+0x14]) purely for register allocation; each
// spill is a raw f32 dword copy, so loading the same value directly from [EAX+off] is
// bit-identical. The x87 stack layout and the FADD/FMUL ORDER are what matter for
// bit-identity, and those are mirrored operand-for-operand, including the st(N) indices.
//
// Tail: the original ends with `FSTP ST(5)` + 4x `FSTP ST(0)` (its discard idiom, leaving
// the result in ST0 for RET). We instead `FSTP qword ptr [result]` — the same 80->64
// round-to-nearest that libffi's NativeFunction('double') applies when reading ST0 on the
// original side — then pop the FIVE leftovers to keep the x87 stack balanced.
// Declared `double`, NEVER void (memory x87_st0_float10_return_fnptr).
extern "C" __declspec(dllexport)
double __cdecl MatrixIdentityResidual4c4360(const float* m)
{
    double result;
    __asm {
        mov   eax, m
        mov   ecx, 0x005cc320        // &1.0f
        // ---- seed the x87 stack exactly as 0x004c4367..0x004c438a leaves it ----
        fld   dword ptr [eax]        // m00
        fsub  dword ptr [ecx]        // d0 = m00 - 1
        fld   dword ptr [eax+0x14]   // m11
        fsub  dword ptr [ecx]        // d1 = m11 - 1
        fld   dword ptr [eax+0x28]   // m22
        fsub  dword ptr [ecx]        // d2 = m22 - 1
        fld   dword ptr [eax+0x38]   // t38 = m32
        fld   dword ptr [eax+0x34]   // t34 = m31
        // stack now: ST0=t34 ST1=t38 ST2=d2 ST3=d1 ST4=d0
        // ---- T0 = (m01^2 + m02^2) + d0^2 ----   0x004c439c..0x004c43c6
        fld   dword ptr [eax+0x04]
        fmul  dword ptr [eax+0x04]
        fld   dword ptr [eax+0x08]
        fmul  dword ptr [eax+0x08]
        faddp st(1), st
        fld   st(5)                  // d0
        fmul  st, st(6)              // d0*d0
        faddp st(1), st              // ST0 = T0
        // ---- T3 = (m30^2 + m31^2) + m32^2 ----  0x004c43c8..0x004c43e2
        fld   dword ptr [eax+0x30]
        fmul  dword ptr [eax+0x30]
        fld   st(2)                  // t34
        fmul  st, st(3)
        faddp st(1), st
        fld   st(3)                  // t38
        fmul  st, st(4)
        faddp st(1), st              // ST0 = T3
        faddp st(1), st              // ST0 = T0 + T3
        // ---- T2 = (m20^2 + m21^2) + d2^2 ----   0x004c43e6..0x004c43fc
        fld   dword ptr [eax+0x20]
        fmul  dword ptr [eax+0x20]
        fld   dword ptr [eax+0x24]
        fmul  dword ptr [eax+0x24]
        faddp st(1), st
        fld   st(4)                  // d2
        fmul  st, st(5)
        faddp st(1), st              // ST0 = T2
        faddp st(1), st              // ST0 = (T0+T3) + T2
        // ---- T1 = (m10^2 + m12^2) + d1^2 ----   0x004c4400..0x004c4416
        fld   dword ptr [eax+0x10]
        fmul  dword ptr [eax+0x10]
        fld   dword ptr [eax+0x18]
        fmul  dword ptr [eax+0x18]
        faddp st(1), st
        fld   st(5)                  // d1
        fmul  st, st(6)
        faddp st(1), st              // ST0 = T1
        faddp st(1), st              // ST0 = ((T0+T3)+T2) + T1
        // ---- store + balance (5 leftovers: t34 t38 d2 d1 d0) ----
        fstp  qword ptr [result]
        fstp  st(0)
        fstp  st(0)
        fstp  st(0)
        fstp  st(0)
        fstp  st(0)
    }
    return result;
}
RH_ScopedInstall(MatrixIdentityResidual4c4360, 0x004c4360);
