// Mashed RE - x87 sine-of-global-product leaf.
//
// Proof row for the ST0 float10-return diff arg_type (HARNESS_BACKLOG #1).
//
// 0x00431b50  FUN_00431b50  util  C2 -> C3
// Verbatim x87 leaf. Disasm (Ghidra Mashed_pool0, 2026-07-26):
//   00431b50  d905040f7f00   FLD   dword ptr [0x007f0f04]   ; *(float*)  DAT_007f0f04  (32-bit)
//   00431b56  dc0df0d85c00   FMUL  qword ptr [0x005cd8f0]   ; * *(double*)_DAT_005cd8f0 (64-bit)
//   00431b5c  d9fe           FSIN                            ; ST0 = sin(ST0)
//   00431b5e  c3             RET                             ; return ST0 (80-bit extended)
//
// Return convention: float10 in ST0. No args, no stack cleanup (RET, not RET n).
// Declared `double` (NEVER void): forwarding this leaf through a void-declared
// function pointer leaks the x87 stack (SSE2 code never pops ST0 -> NaN + FPU
// corruption). See feedback memory x87_st0_float10_return_fnptr.
//
// Bit-identity: MSVC's CRT sin() is an argument-reduced polynomial, NOT the raw
// hardware FSIN this leaf emits, so the body is an inline __asm block mirroring
// the three instructions exactly (identical absolute operands and widths). The
// closing FSTP qword truncates the 80-bit ST0 result to 64 bits with the same
// round-to-nearest the Frida NativeFunction('double') read performs on the
// original side, so the two agree to the full double mantissa. Same basis as the
// FSIN block in Math/RwMatrixRotate.cpp.
//
// Operand semantics are unknown (U-1618): the units of DAT_007f0f04 and
// _DAT_005cd8f0 are not derivable from this leaf. Reported mechanically only.
#include "../Core/HookSystem.h"

// 0x00431b50
extern "C" __declspec(dllexport)
double __cdecl SinGlobalProduct431b50()
{
    double result;
    __asm {
        mov   eax, 0x007f0f04
        fld   dword ptr [eax]        // *(float*)0x007f0f04
        mov   eax, 0x005cd8f0
        fmul  qword ptr [eax]        // * *(double*)0x005cd8f0
        fsin
        fstp  qword ptr [result]     // 80 -> 64, matches libffi 'double' read of ST0
    }
    return result;
}

RH_ScopedInstall(SinGlobalProduct431b50, 0x00431b50);
