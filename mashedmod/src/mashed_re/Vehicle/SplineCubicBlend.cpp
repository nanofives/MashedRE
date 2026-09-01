// Mashed RE — 3-component cubic control-point blend (0x00482ae0).
//
// Zero-callee pure leaf (area-vehicle round 2). Straight-line x87, no branches,
// no calls. Disasm 0x00482ae0 (verbatim; log/area-vehicle/disasm_482ae0.txt).
//
// Signature (from stack refs + the C2 plate): void(float t, float* out,
//   float* p0, float* p1, float* p2, float* p3). Entry stack: [esp+4]=t,
//   [esp+8]=out, [esp+c]=p0, [esp+10]=p1, [esp+14]=p2, [esp+18]=p3. After the
//   in-body PUSH ESI/PUSH EDI: ESI=p3 ([esp+0x1c]), EDI=out ([esp+0x10]),
//   EAX=p0, ECX=p1, EDX=p2. Writes out[0..2].
//
// Per component c (c = 0, 4, 8): with s = t*t and u = t (as staged on the x87
// stack, ST2 = t after the FADD ST0,ST0 = 2t? — reported mechanically, no
// semantic claim), the body is the fixed polynomial
//   out[c] = (((2*p0[c] - [5cc358]*p1[c] + [5cc35c]*p2[c] - p3[c]) * A
//             + ([5cc31c]*p1[c] - p0[c] - [5cc31c]*p2[c] + p3[c])) * B
//             + (p2[c] - p0[c]) * t
//             + 2*p1[c]) * [5cc32c]
// transcribed VERBATIM below (a C reimpl rounds x87 differently). This function's
// own semantics are fully captured by the verbatim body; the *caller-side* role
// varies (0x00404fa0 uses it as "quat slerp", 0x00482c10 Replay::ReadFrame as
// "SLERP", 0x00483a70 as "spline") — a caller concern, not an ambiguity about
// this leaf, so no high-level basis name (Catmull-Rom/slerp) is asserted here.
//
// Constants (image .rdata, present when injected):
//   0x005cc31c, 0x005cc32c, 0x005cc358, 0x005cc35c  (blend coefficients)
// Callers (caller-half): 0x00404fa0 (render C2), 0x004752f0 (render C2),
//   0x00482c10 Replay::ReadFrame (vehicle C2), 0x00483a70 (render C2).
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x00482ae0  SplineCubicBlend3
//   void SplineCubicBlend3(float t, float* out, float* p0..p3)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl SplineCubicBlend3(
        float /*t*/, float* /*out*/, float* /*p0*/, float* /*p1*/, float* /*p2*/,
        float* /*p3*/) {
    __asm {
        mov  eax, dword ptr [esp+0Ch]           // p0
        fld  dword ptr [esp+4]                   // t
        fmul dword ptr [esp+4]                   // t*t
        mov  ecx, dword ptr [esp+10h]            // p1
        fld  dword ptr [esp+4]                   // t
        mov  edx, dword ptr [esp+14h]            // p2
        push esi
        fmul st(0), st(1)
        mov  esi, dword ptr [esp+1Ch]            // p3
        fld  dword ptr [eax]                     // p0[0]
        push edi
        fadd st(0), st(0)
        mov  edi, dword ptr [esp+10h]            // out
        fld  dword ptr [ecx]                     // p1[0]
        fmul dword ptr ds:[05CC358h]
        fsubp st(1), st(0)
        fld  dword ptr [edx]                     // p2[0]
        fmul dword ptr ds:[05CC35Ch]
        faddp st(1), st(0)
        fsub dword ptr [esi]                     // p3[0]
        fmul st(0), st(2)
        fld  dword ptr [ecx]                     // p1[0]
        fmul dword ptr ds:[05CC31Ch]
        fsub dword ptr [eax]                     // p0[0]
        fld  dword ptr [edx]                     // p2[0]
        fmul dword ptr ds:[05CC31Ch]
        fsubp st(1), st(0)
        fadd dword ptr [esi]                     // p3[0]
        fmul st(0), st(2)
        faddp st(1), st(0)
        fld  dword ptr [edx]                     // p2[0]
        fsub dword ptr [eax]                     // p0[0]
        fmul dword ptr [esp+0Ch]                 // t (post-2-push: [esp+0xc]=[esp+4] orig)
        faddp st(1), st(0)
        fld  dword ptr [ecx]                     // p1[0]
        fadd st(0), st(0)
        faddp st(1), st(0)
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [edi]                     // out[0]

        fld  dword ptr [eax+4]                   // p0[1]
        fadd st(0), st(0)
        fld  dword ptr [ecx+4]                   // p1[1]
        fmul dword ptr ds:[05CC358h]
        fsubp st(1), st(0)
        fld  dword ptr [edx+4]                   // p2[1]
        fmul dword ptr ds:[05CC35Ch]
        faddp st(1), st(0)
        fsub dword ptr [esi+4]                   // p3[1]
        fmul st(0), st(2)
        fld  dword ptr [ecx+4]                   // p1[1]
        fmul dword ptr ds:[05CC31Ch]
        fsub dword ptr [eax+4]                   // p0[1]
        fld  dword ptr [edx+4]                   // p2[1]
        fmul dword ptr ds:[05CC31Ch]
        fsubp st(1), st(0)
        fadd dword ptr [esi+4]                   // p3[1]
        fmul st(0), st(2)
        faddp st(1), st(0)
        fld  dword ptr [edx+4]                   // p2[1]
        fsub dword ptr [eax+4]                   // p0[1]
        fmul dword ptr [esp+0Ch]                 // t
        faddp st(1), st(0)
        fld  dword ptr [ecx+4]                   // p1[1]
        fadd st(0), st(0)
        faddp st(1), st(0)
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [edi+4]                   // out[1]

        fld  dword ptr [eax+8]                   // p0[2]
        fadd st(0), st(0)
        fld  dword ptr [ecx+8]                   // p1[2]
        fmul dword ptr ds:[05CC358h]
        fsubp st(1), st(0)
        fld  dword ptr [edx+8]                   // p2[2]
        fmul dword ptr ds:[05CC35Ch]
        faddp st(1), st(0)
        fsub dword ptr [esi+8]                   // p3[2]
        fmul st(0), st(2)
        fld  dword ptr [ecx+8]                   // p1[2]
        fmul dword ptr ds:[05CC31Ch]
        fsub dword ptr [eax+8]                   // p0[2]
        fld  dword ptr [edx+8]                   // p2[2]
        fmul dword ptr ds:[05CC31Ch]
        fsubp st(1), st(0)
        fadd dword ptr [esi+8]                   // p3[2]
        fmul st(0), st(2)
        faddp st(1), st(0)
        fld  dword ptr [edx+8]                   // p2[2]
        fsub dword ptr [eax+8]                   // p0[2]
        fmul dword ptr [esp+0Ch]                 // t
        faddp st(1), st(0)
        fld  dword ptr [ecx+8]                   // p1[2]
        fadd st(0), st(0)
        faddp st(1), st(0)
        fmul dword ptr ds:[05CC32Ch]
        fstp dword ptr [edi+8]                   // out[2]

        pop  edi
        pop  esi
        fstp st(0)
        fstp st(0)
        ret
    }
}
RH_ScopedInstall(SplineCubicBlend3, 0x00482ae0);
