// Mashed RE - Cosine-eased linear interpolation primitive.
// Original: 0x00422440  FUN_00422440  bucket_gameplay_00422440_0044e070  C2 -> C3
//
// Pure-leaf math helper: float CosineLerp(float a, float b, float t) computing
//   w = (1 - cos(pi * t)) * 0.5            // cosine-ease weight
//   return w*b + (1-w)*a                   // lerp(a, b, w)
// Sole caller FUN_00422470 (0x00422470, C2 frontend). The three .rdata constants
// (pi, 1.0, 0.5) are read LIVE from MASHED's image so the reimpl is bit-identical
// regardless of any host-CRT rounding differences:
//   0x005cd310 = 0x40490fdb  (+3.14159265, pi)   - cos-argument scale
//   0x005cc320 = 0x3f800000  (+1.0)              - the "1 -" term
//   0x005cc32c = 0x3f000000  (+0.5)              - the "*0.5" averaging factor
//
// BIT-IDENTITY: the original keeps EVERY intermediate in the 80-bit x87 ST(0)
// stack (Ghidra renders them as `float10`); a plain-C reimpl with 32-bit `float`
// locals would truncate each intermediate to 24-bit mantissa and diverge by ULPs.
// The only way to match exactly is to reproduce the x87 instruction stream, so
// this is a __declspec(naked) verbatim transcription of the 44-byte body.
//
// Disasm at 0x00422440..0x0042246c (args at [esp+4]=a, [esp+8]=b, [esp+0xc]=t;
// no prologue in the original either, so a naked reimpl uses the same offsets):
//   0x00422440  D9 05 10 D3 5C 00   fld   dword ptr [0x005cd310]   ; ST0 = pi
//   0x00422446  D8 4C 24 0C         fmul  dword ptr [esp+0x0c]     ; ST0 = pi*t
//   0x0042244a  D9 FF               fcos                           ; ST0 = cos(pi*t)
//   0x0042244c  D8 2D 20 C3 5C 00   fsubr dword ptr [0x005cc320]   ; ST0 = 1.0 - cos
//   0x00422452  D8 0D 2C C3 5C 00   fmul  dword ptr [0x005cc32c]   ; ST0 = (1-cos)*0.5 = w
//   0x00422458  D9 05 20 C3 5C 00   fld   dword ptr [0x005cc320]   ; ST0 = 1.0, ST1 = w
//   0x0042245e  D8 E1               fsub  st(0), st(1)             ; ST0 = 1.0 - w
//   0x00422460  D8 4C 24 04         fmul  dword ptr [esp+0x04]     ; ST0 = (1-w)*a
//   0x00422464  D9 C9               fxch  st(1)                    ; ST0 = w, ST1 = (1-w)*a
//   0x00422466  D8 4C 24 08         fmul  dword ptr [esp+0x08]     ; ST0 = w*b
//   0x0042246a  DE C1               faddp st(1), st(0)             ; result = (1-w)*a + w*b
//   0x0042246c  C3                  ret
#include "../Core/HookSystem.h"

// 0x00422440
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl
CosineLerp(float /*a*/, float /*b*/, float /*t*/)
{
    __asm {
        fld   dword ptr ds:[0x005cd310]   // pi
        fmul  dword ptr [esp+0x0c]        // pi * t
        fcos                              // cos(pi*t)
        fsubr dword ptr ds:[0x005cc320]   // 1.0 - cos
        fmul  dword ptr ds:[0x005cc32c]   // * 0.5  -> w
        fld   dword ptr ds:[0x005cc320]   // 1.0
        fsub  st(0), st(1)                // 1.0 - w
        fmul  dword ptr [esp+0x04]        // (1-w) * a
        fxch  st(1)
        fmul  dword ptr [esp+0x08]        // w * b
        faddp st(1), st(0)                // (1-w)*a + w*b
        ret
    }
}

RH_ScopedInstall(CosineLerp, 0x00422440);
