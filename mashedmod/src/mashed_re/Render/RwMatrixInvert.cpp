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
