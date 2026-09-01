// Mashed RE - 3D point-centroid (average position) of a packed vec3 array.
// Original: 0x004b4550  FUN_004b4550  util  C2 -> C3
//
// void FUN_004b4550(float* out /*param_1, [esp+4]*/,
//                   float* points /*param_2, [esp+8]*/,
//                   int   count  /*param_3, [esp+0xc]*/):
// out[0..2] = (1/count) * sum(points[i][0..2]) for i in [0,count).
//   - Copies the first point (points[0..2]) straight into out[0..2] as the seed.
//   - Sums the remaining count-1 points; the inner loop is 4-unrolled (one iteration
//     per 4 points, ecx striding 0x30 = 4*12 bytes) with a 1..3-point remainder loop.
//   - Divides by count: fild(count) then fdivr against the +1.0f constant so ST0 =
//     1.0f / count, then multiplies out[0..2] by that reciprocal.
// points is a packed array of `count` vec3s, stride 12 bytes (3 floats). PURE LEAF:
// no callees; the sole global is the +1.0 reciprocal-numerator const @0x005cc320
// (0x3f800000), the same constant RwMatrixInvert reads.
//
// BIT-IDENTITY: every accumulation stays in the 80-bit x87 stack, so summation order
// and the single reciprocal matter to the low bits of the stored f32s; a plain-C
// /arch:SSE2 reimpl would round each partial sum at 24 bits and diverge. This is a
// __declspec(naked) 1:1 transcription of the 0x004b4550..0x004b4646 body. The unrolled
// loop's `add ecx,0x30` is deliberately kept interleaved between the x87 ops exactly
// where the original places it, because every subsequent fld offset ([ecx-0x34] ...
// [ecx-0xc]) is taken relative to the already-advanced ecx. __cdecl: caller cleans -> RET.
#include "../Core/HookSystem.h"

// 0x004b4550
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
Vec3Centroid(void* /*out*/, void* /*points*/, int /*count*/)
{
    __asm {
        mov    eax, dword ptr [esp + 4]            // out (param_1)
        push   ebx
        mov    ebx, dword ptr [esp + 0xc]          // points (param_2)
        push   esi
        mov    ecx, ebx
        mov    esi, dword ptr [ecx]                // seed out[0..2] = points[0..2]
        mov    edx, eax
        mov    dword ptr [edx], esi
        mov    esi, dword ptr [ecx + 4]
        mov    dword ptr [edx + 4], esi
        mov    ecx, dword ptr [ecx + 8]
        mov    esi, dword ptr [esp + 0x14]         // count (param_3)
        mov    dword ptr [edx + 8], ecx
        lea    ecx, [esi - 1]                       // count - 1
        cmp    ecx, 4
        mov    edx, 1                               // running index
        jl     L_rem_check
        push   edi
        mov    edi, 4
        lea    ecx, [ebx + 0x14]
    L_unroll:
        fld    dword ptr [ecx - 8]
        add    edi, 4
        fadd   dword ptr [eax]
        add    edx, 4
        add    ecx, 0x30                            // advance 4 points (must stay HERE)
        cmp    edi, esi
        fstp   dword ptr [eax]
        fld    dword ptr [ecx - 0x34]
        fadd   dword ptr [eax + 4]
        fstp   dword ptr [eax + 4]
        fld    dword ptr [eax + 8]
        fadd   dword ptr [ecx - 0x30]
        fstp   dword ptr [eax + 8]
        fld    dword ptr [ecx - 0x2c]
        fadd   dword ptr [eax]
        fstp   dword ptr [eax]
        fld    dword ptr [ecx - 0x28]
        fadd   dword ptr [eax + 4]
        fstp   dword ptr [eax + 4]
        fld    dword ptr [ecx - 0x24]
        fadd   dword ptr [eax + 8]
        fstp   dword ptr [eax + 8]
        fld    dword ptr [ecx - 0x20]
        fadd   dword ptr [eax]
        fstp   dword ptr [eax]
        fld    dword ptr [ecx - 0x1c]
        fadd   dword ptr [eax + 4]
        fstp   dword ptr [eax + 4]
        fld    dword ptr [ecx - 0x18]
        fadd   dword ptr [eax + 8]
        fstp   dword ptr [eax + 8]
        fld    dword ptr [ecx - 0x14]
        fadd   dword ptr [eax]
        fstp   dword ptr [eax]
        fld    dword ptr [ecx - 0x10]
        fadd   dword ptr [eax + 4]
        fstp   dword ptr [eax + 4]
        fld    dword ptr [ecx - 0xc]
        fadd   dword ptr [eax + 8]
        fstp   dword ptr [eax + 8]
        jl     L_unroll
        pop    edi
    L_rem_check:
        cmp    edx, esi
        jge    L_final
        lea    ecx, [edx + edx*2]                   // ecx = 3*index
        lea    ecx, [ebx + ecx*4 + 8]               // points + index*12 + 8
        sub    esi, edx
    L_remainder:
        fld    dword ptr [ecx - 8]
        add    ecx, 0xc
        dec    esi
        fadd   dword ptr [eax]
        fstp   dword ptr [eax]
        fld    dword ptr [ecx - 0x10]
        fadd   dword ptr [eax + 4]
        fstp   dword ptr [eax + 4]
        fld    dword ptr [eax + 8]
        fadd   dword ptr [ecx - 0xc]
        fstp   dword ptr [eax + 8]
        jne    L_remainder
    L_final:
        fild   dword ptr [esp + 0x14]               // (float)count
        pop    esi
        pop    ebx
        fdivr  dword ptr ds:[0x005cc320]            // ST0 = 1.0f / count
        fld    st(0)
        fmul   dword ptr [eax]
        fstp   dword ptr [eax]
        fld    st(0)
        fmul   dword ptr [eax + 4]
        fstp   dword ptr [eax + 4]
        fmul   dword ptr [eax + 8]
        fstp   dword ptr [eax + 8]
        ret
    }
}

RH_ScopedInstall(Vec3Centroid, 0x004b4550);
