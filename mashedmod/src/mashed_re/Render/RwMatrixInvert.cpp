// Mashed RE - RenderWare 4x3 affine matrix inverse (cofactor path).
// Original: 0x004c4eb0  FUN_004c4eb0  render  C2 -> C3
//
// void FUN_004c4eb0(float* out /*param_1, [esp+4]->eax*/,
//                   float* in  /*param_2, [esp+8]->ecx*/):
// Inverts the 3x3 rotation part of `in` by the classic adjugate/determinant
// method, then computes the inverse translation (-R^-1 * t). RwMatrix layout is
// 4 rows x 4 floats (0x40 bytes): right @+0x00, up @+0x10, at @+0x20, pos @+0x30,
// with flag/pad words at +0x0c/+0x1c/+0x2c/+0x3c. The body reads only the 3x3 +
// translation of `in` and writes the 3x3 + translation of `out`; out+0x0c is
// forced to 0 and the three pad words are left at their incoming value.
//
// PURE LEAF: no globals except the reciprocal-numerator constant
//   0x005cc320 = 0x3f800000 (+1.0)
// and no callees. The determinant guard (det==0 -> numerator stays 1.0, no fdiv)
// matches the original exactly.
//
// BIT-IDENTITY: the original keeps every intermediate in the 80-bit x87 stack;
// the cofactors, the 1/det reciprocal, and the translation dot-products all round
// at 80-bit, not 32-bit. A plain-C /arch:SSE2 reimpl would round each intermediate
// to a 24-bit mantissa and diverge by ULPs in the stored floats. This is a
// __declspec(naked) verbatim transcription of the 0x152-byte body
// (0x004c4eb0..0x004c5001). __cdecl: caller cleans -> plain RET. The incoming
// `in` stack slot [esp+8] is reused as an f32 scratch for the determinant after
// `in` is safely held in ecx, exactly as the original does.
#include "../Core/HookSystem.h"

// 0x004c4eb0
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
RwMatrixInvert(void* /*out*/, void* /*in*/)
{
    __asm {
        mov    ecx, dword ptr [esp + 8]        // in
        mov    eax, dword ptr [esp + 4]        // out
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx + 0x14]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx + 0x18]
        fsubp  st(1), st(0)
        fstp   dword ptr [eax]
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx + 0x4]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x4]
        fld    dword ptr [ecx + 0x18]
        fmul   dword ptr [ecx + 0x4]
        fld    dword ptr [ecx + 0x14]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fst    dword ptr [eax + 0x8]
        fld    dword ptr [ecx + 0x10]
        fmul   dword ptr [eax + 0x4]
        fld    dword ptr [ecx + 0x20]
        fmul   st(0), st(2)
        faddp  st(1), st(0)
        fld    dword ptr [eax]
        fmul   dword ptr [ecx]
        faddp  st(1), st(0)
        fstp   dword ptr [esp + 8]
        mov    edx, dword ptr [esp + 8]
        fstp   st(0)
        fld    dword ptr ds:[0x005cc320]        // +1.0 reciprocal numerator
        test   edx, edx
        je     L_skipdiv
        fdiv   dword ptr [esp + 8]              // 1.0 / determinant
    L_skipdiv:
        fld    st(0)
        fmul   dword ptr [eax]
        fst    dword ptr [eax]
        fld    st(1)
        fmul   dword ptr [eax + 0x4]
        fst    dword ptr [esp + 8]
        fstp   dword ptr [eax + 0x4]
        fld    st(1)
        fmul   dword ptr [eax + 0x8]
        fstp   dword ptr [eax + 0x8]
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx + 0x10]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x18]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fchs
        fstp   dword ptr [eax + 0x10]
        fld    dword ptr [ecx + 0x28]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fstp   dword ptr [eax + 0x14]
        fld    dword ptr [ecx + 0x18]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x10]
        fmul   dword ptr [ecx + 0x8]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fchs
        fstp   dword ptr [eax + 0x18]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx + 0x10]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x14]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fstp   dword ptr [eax + 0x20]
        fld    dword ptr [ecx + 0x24]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x20]
        fmul   dword ptr [ecx + 0x4]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fchs
        fstp   dword ptr [eax + 0x24]
        fld    dword ptr [ecx + 0x14]
        fmul   dword ptr [ecx]
        fld    dword ptr [ecx + 0x10]
        fmul   dword ptr [ecx + 0x4]
        fsubp  st(1), st(0)
        fmul   st(0), st(2)
        fstp   dword ptr [eax + 0x28]
        fld    dword ptr [eax + 0x20]
        fmul   dword ptr [ecx + 0x38]
        fld    dword ptr [ecx + 0x34]
        fmul   dword ptr [eax + 0x10]
        faddp  st(1), st(0)
        fld    dword ptr [ecx + 0x30]
        fmul   st(0), st(2)
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x30]
        fstp   st(0)
        fstp   st(0)
        fld    dword ptr [eax + 0x24]
        fmul   dword ptr [ecx + 0x38]
        fld    dword ptr [ecx + 0x34]
        fmul   dword ptr [eax + 0x14]
        faddp  st(1), st(0)
        fld    dword ptr [ecx + 0x30]
        fmul   dword ptr [esp + 8]
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x34]
        fld    dword ptr [ecx + 0x34]
        fmul   dword ptr [eax + 0x18]
        fld    dword ptr [ecx + 0x30]
        fmul   dword ptr [eax + 0x8]
        faddp  st(1), st(0)
        fld    dword ptr [eax + 0x28]
        fmul   dword ptr [ecx + 0x38]
        mov    dword ptr [eax + 0xc], 0
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x38]
        ret
    }
}

RH_ScopedInstall(RwMatrixInvert, 0x004c4eb0);

// Mashed RE - RenderWare matrix inverse PUBLIC ENTRY (the flag-dispatched wrapper).
// Original: 0x004c4dc0  FUN_004c4dc0  util  C2 -> C3
//
// float* FUN_004c4dc0(float* out /*param_1, [esp+4]*/, float* in /*param_2, [esp+8]*/):
// The real RwMatrixInvert entry point (23 callers across the whole image). Reads the
// RW3 engine "matrix optimisation" flag table to pick one of three inverse paths, then
// returns `out` in eax. RwMatrix is 4x4 floats (0x40B); the flag word is at +0x0c.
//   Branch 1 (identity fast-path): if the engine's supported-opt mask ANDed with in's
//     flag word has bit 0x20000 set, memcpy 0x10 dwords in->out and return (the matrix
//     is flagged identity/pre-inverted, so the inverse is a straight copy).
//   Branch 2 (orthonormal fast-path): else if (in_flags & 3) == 3, transpose the 3x3
//     rotation (integer float moves) and compute the inverse translation -R^T*t as three
//     inlined x87 dot-products; force out's flag word (+0x0c) to 3.
//   Branch 3 (general fallback): else tail-call the 3x3 cofactor inverse (0x004c4eb0);
//     that path zeroes out+0x0c.
//
// GLOBALS: reads DAT_007d3ff8 (RW device/engine state) and DAT_007d4028 (matrix-opt
// flags-array base) as absolute addresses, exactly as the original does; the index is
// [DAT_007d4028 + DAT_007d3ff8 + 4]. These are live at menu-attach (same globals the
// FastInvSqrt LUT uses). Because the original and this reimpl read the identical live
// globals at the same instant, both select the same branch, so the byte-for-byte output
// matches whichever path fires.
//
// BIT-IDENTITY: branches 1/2 are copies and 80-bit x87 dot-products; branch 3 forwards
// to the already-verified cofactor reimpl. The one deviation from a literal transcription
// is that branch 3 emits `call RwMatrixInvert` (this DLL's linked cofactor symbol) instead
// of `call 0x004c4eb0`, avoiding an absolute call target in the standalone; the two are the
// same code, so the result is unchanged. __cdecl: caller cleans -> plain RET.
//
// 0x004c4dc0
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
RwMatrixInvertEntry(void* /*out*/, void* /*in*/)
{
    __asm {
        mov    ecx, dword ptr ds:[0x007d3ff8]      // DAT_007d3ff8 RW engine state
        mov    edx, dword ptr ds:[0x007d4028]      // DAT_007d4028 matrix-opt flags base
        push   esi
        mov    esi, dword ptr [esp + 0xc]          // in (param_2)
        mov    ecx, dword ptr [edx + ecx + 4]      // engine supported-opt flags
        mov    eax, dword ptr [esi + 0xc]          // in flag word (in[3])
        and    ecx, eax
        test   ecx, 0x20000
        je     L_not_identity
        mov    eax, dword ptr [esp + 8]            // out (param_1)
        push   edi
        mov    ecx, 0x10
        mov    edi, eax
        rep    movsd                                // copy 0x10 dwords in -> out
        pop    edi
        pop    esi
        ret
    L_not_identity:
        and    eax, 3
        cmp    al, 3
        jne    L_general
        mov    eax, dword ptr [esp + 8]            // out
        mov    edx, dword ptr [esi]                // --- 3x3 transpose (integer moves) ---
        mov    dword ptr [eax], edx
        mov    ecx, dword ptr [esi + 0x10]
        mov    dword ptr [eax + 4], ecx
        mov    edx, dword ptr [esi + 0x20]
        mov    dword ptr [eax + 8], edx
        mov    ecx, dword ptr [esi + 4]
        mov    dword ptr [eax + 0x10], ecx
        mov    edx, dword ptr [esi + 0x14]
        mov    dword ptr [eax + 0x14], edx
        mov    ecx, dword ptr [esi + 0x24]
        mov    dword ptr [eax + 0x18], ecx
        mov    edx, dword ptr [esi + 8]
        mov    dword ptr [eax + 0x20], edx
        mov    ecx, dword ptr [esi + 0x18]
        mov    dword ptr [eax + 0x24], ecx
        mov    edx, dword ptr [esi + 0x28]
        mov    dword ptr [eax + 0x28], edx
        fld    dword ptr [esi + 0x34]              // --- inverse translation -R^T * t ---
        fmul   dword ptr [esi + 4]
        fld    dword ptr [esi + 0x30]
        fmul   dword ptr [esi]
        faddp  st(1), st(0)
        fld    dword ptr [esi + 8]
        fmul   dword ptr [esi + 0x38]
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x30]
        fld    dword ptr [esi + 0x30]
        fmul   dword ptr [esi + 0x10]
        fld    dword ptr [esi + 0x34]
        fmul   dword ptr [esi + 0x14]
        faddp  st(1), st(0)
        fld    dword ptr [esi + 0x38]
        fmul   dword ptr [esi + 0x18]
        faddp  st(1), st(0)
        fchs
        fstp   dword ptr [eax + 0x34]
        fld    dword ptr [esi + 0x20]
        fmul   dword ptr [esi + 0x30]
        fld    dword ptr [esi + 0x34]
        fmul   dword ptr [esi + 0x24]
        faddp  st(1), st(0)
        fld    dword ptr [esi + 0x28]
        fmul   dword ptr [esi + 0x38]
        faddp  st(1), st(0)
        mov    dword ptr [eax + 0xc], 3            // out flag word = 3 (orthonormal)
        pop    esi
        fchs
        fstp   dword ptr [eax + 0x38]
        ret
    L_general:
        push   esi                                 // in  -> [esp+8] at call = second arg
        mov    esi, dword ptr [esp + 0xc]          // out
        push   esi                                 // out -> [esp+4] at call = first arg
        call   RwMatrixInvert                       // 0x004c4eb0 cofactor path (linked)
        add    esp, 8
        mov    eax, esi                            // return out
        pop    esi
        ret
    }
}

RH_ScopedInstall(RwMatrixInvertEntry, 0x004c4dc0);
