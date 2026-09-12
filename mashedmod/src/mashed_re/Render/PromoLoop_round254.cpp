// Mashed RE — promote-round round 254 (orthonormal 4x3 matrix inverse).
//
// Function in this file:
//   0x004fb210  Mat4x3InvertOrthonormal — transposes the 3x3 rotation block and
//               rewrites the translation row as the negated dot
//               products of the old translation against each original row.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

// ---------------------------------------------------------------------------
// Mat4x3InvertOrthonormal  --  0x004fb210   (141 bytes, 0x004fb210..0x004fb29c)
//
// NAME: the GEOMETRY is read from the instruction stream (transpose of the 3x3
// block plus negated dot products of the translation), so Mat4x3InvertOrthonormal
// claims only what was seen. An earlier draft called it RwMatrixInvertOrthonormal;
// the Rw prefix was a TYPE inference from the 0/0x10/0x20/0x30 layout, and this
// address sits in a band the notes attest as statically-linked Microsoft PSGP
// rather than RenderWare — so the prefix was withdrawn in the r256 naming audit.
//
// void fn(float* out /*[esp+4] -> ECX*/, const float* in /*[esp+8] -> EAX*/)
//
// WHY THIS IS NAKED ASM AND NOT C.
// The Ghidra decompilation writes the first translation term as
//     param_1[0xc] = -(param_2[0xe]*param_2[2] + param_2[0xc]*param_2[0]
//                      + param_2[0xd]*param_2[1]);
// C's `+` is left-associative, so that text means ((e*2 + c*0) + d*1). The x87
// stream says something DIFFERENT: it computes (p2[13]*p2[1] + p2[12]*p2[0])
// first and adds p2[14]*p2[2] last. Floating-point addition is NOT associative,
// so the two groupings can round differently. The decompiler's operand ORDER is
// not evidence of the evaluation order — only the instruction stream is.
//
// Note also that the FIRST block's operand order differs from the other two:
// block 1 loads p2[13]*p2[1] before p2[12]*p2[0], while blocks 2 and 3 load
// p2[12]*p2[x] first. A single "obvious" C expression reused for all three rows
// would therefore be wrong for at least one of them. Transcribed instruction by
// instruction for exactly that reason.
//
// Instruction-level transcription (every line below is the original's, in order):
//
//   0x004fb210  mov eax,[esp+8]          ; in
//   0x004fb214  mov ecx,[esp+4]          ; out
//   -- translation X -> out[12] ------------------------------------------
//   0x004fb218  fld  [eax+0x34]          ; in[13]
//   0x004fb21b  fmul [eax+0x04]          ; * in[1]
//   0x004fb21e  fld  [eax+0x30]          ; in[12]
//   0x004fb221  fmul [eax+0x00]          ; * in[0]
//   0x004fb223  faddp st(1),st           ; (in13*in1) + (in12*in0)
//   0x004fb225  fld  [eax+0x38]          ; in[14]
//   0x004fb228  fmul [eax+0x08]          ; * in[2]
//   0x004fb22b  faddp st(1),st           ; + that
//   0x004fb22d  fchs
//   0x004fb22f  fstp [ecx+0x30]
//   -- transpose column 0 -------------------------------------------------
//   0x004fb232  mov edx,[eax+0x00] / mov [ecx+0x00],edx
//   0x004fb236  mov edx,[eax+0x04] / mov [ecx+0x10],edx
//   0x004fb23c  mov edx,[eax+0x08] / mov [ecx+0x20],edx
//   -- translation Y -> out[13] ------------------------------------------
//   0x004fb242  fld  [eax+0x30] / fmul [eax+0x10]    ; in[12]*in[4]
//   0x004fb248  fld  [eax+0x14] / fmul [eax+0x34]    ; in[5]*in[13]
//   0x004fb24e  faddp st(1),st
//   0x004fb250  fld  [eax+0x38] / fmul [eax+0x18]    ; in[14]*in[6]
//   0x004fb256  faddp st(1),st
//   0x004fb258  fchs
//   0x004fb25a  fstp [ecx+0x34]
//   -- transpose column 1 -------------------------------------------------
//   0x004fb25d  mov edx,[eax+0x10] / mov [ecx+0x04],edx
//   0x004fb263  mov edx,[eax+0x14] / mov [ecx+0x14],edx
//   0x004fb269  mov edx,[eax+0x18] / mov [ecx+0x24],edx
//   -- translation Z -> out[14] ------------------------------------------
//   0x004fb26f  fld  [eax+0x30] / fmul [eax+0x20]    ; in[12]*in[8]
//   0x004fb275  fld  [eax+0x24] / fmul [eax+0x34]    ; in[9]*in[13]
//   0x004fb27b  faddp st(1),st
//   0x004fb27d  fld  [eax+0x38] / fmul [eax+0x28]    ; in[14]*in[10]
//   0x004fb283  faddp st(1),st
//   0x004fb285  fchs
//   0x004fb287  fstp [ecx+0x38]
//   -- transpose column 2 -------------------------------------------------
//   0x004fb28a  mov edx,[eax+0x20] / mov [ecx+0x08],edx
//   0x004fb290  mov edx,[eax+0x24] / mov [ecx+0x18],edx
//   0x004fb296  mov eax,[eax+0x28] / mov [ecx+0x28],eax   ; NOTE: EAX, not EDX
//   0x004fb29c  ret
//
// The nine rotation elements are moved as DWORDS, not floats, so they are
// bit-exact copies and cannot round at all. Only the three translation terms go
// through the FPU.
//
// The write/read interleaving is preserved exactly: the original writes out[12]
// BEFORE copying column 0, and so on. That only matters if `out` and `in` alias,
// which nothing here rules out, so the order is kept rather than tidied.
//
// The final load uses EAX as the scratch register instead of EDX (0x004fb296),
// which is harmless but is transcribed as-is rather than normalised.
//
// out[3], out[7], out[11] and out[15] are NOT written by this function.
//
// Callers: FUN_004fb130 (C2), FUN_004fb2a0, FUN_004fb3e0.
// ---------------------------------------------------------------------------

// 0x004fb210
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
Mat4x3InvertOrthonormal(float* /*out*/, const float* /*in*/)
{
    __asm {
        mov     eax, dword ptr [esp + 8]
        mov     ecx, dword ptr [esp + 4]

        fld     dword ptr [eax + 34h]
        fmul    dword ptr [eax + 4]
        fld     dword ptr [eax + 30h]
        fmul    dword ptr [eax]
        faddp   st(1), st(0)
        fld     dword ptr [eax + 38h]
        fmul    dword ptr [eax + 8]
        faddp   st(1), st(0)
        fchs
        fstp    dword ptr [ecx + 30h]

        mov     edx, dword ptr [eax]
        mov     dword ptr [ecx], edx
        mov     edx, dword ptr [eax + 4]
        mov     dword ptr [ecx + 10h], edx
        mov     edx, dword ptr [eax + 8]
        mov     dword ptr [ecx + 20h], edx

        fld     dword ptr [eax + 30h]
        fmul    dword ptr [eax + 10h]
        fld     dword ptr [eax + 14h]
        fmul    dword ptr [eax + 34h]
        faddp   st(1), st(0)
        fld     dword ptr [eax + 38h]
        fmul    dword ptr [eax + 18h]
        faddp   st(1), st(0)
        fchs
        fstp    dword ptr [ecx + 34h]

        mov     edx, dword ptr [eax + 10h]
        mov     dword ptr [ecx + 4], edx
        mov     edx, dword ptr [eax + 14h]
        mov     dword ptr [ecx + 14h], edx
        mov     edx, dword ptr [eax + 18h]
        mov     dword ptr [ecx + 24h], edx

        fld     dword ptr [eax + 30h]
        fmul    dword ptr [eax + 20h]
        fld     dword ptr [eax + 24h]
        fmul    dword ptr [eax + 34h]
        faddp   st(1), st(0)
        fld     dword ptr [eax + 38h]
        fmul    dword ptr [eax + 28h]
        faddp   st(1), st(0)
        fchs
        fstp    dword ptr [ecx + 38h]

        mov     edx, dword ptr [eax + 20h]
        mov     dword ptr [ecx + 8], edx
        mov     edx, dword ptr [eax + 24h]
        mov     dword ptr [ecx + 18h], edx
        mov     eax, dword ptr [eax + 28h]
        mov     dword ptr [ecx + 28h], eax

        ret
    }
}

RH_ScopedInstall(Mat4x3InvertOrthonormal, 0x004fb210);
