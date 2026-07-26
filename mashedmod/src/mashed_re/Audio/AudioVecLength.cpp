// Mashed RE - Audio vector fast-length (mantissa-LUT sqrt).
// Original: 0x005aed20  FUN_005aed20  audio  C2 -> C3
//
// void FUN_005aed20(float* out, float* in): computes mag2 = dot(in,in) (x87,
// rounded to f32), stores it to out[0], and if mag2 != 0 replaces out[0] with a
// fast sqrt(mag2) computed from a 16-bit mantissa lookup table at 0x00633b48 plus
// exponent halving. Writes ONLY out[0]; out[1]/out[2] are untouched. Returns void.
// Part of the RenderWare audio spatialisation path (distance attenuation).
//
// Driven through the `vec3_normalize` handler (arg order fn(out, in) matches): the
// function returns void and writes only out[0], so out[1]/out[2] stay zero on both
// sides (equal) and out[0] is the sole real observable. Registry signature is
// {ret:'void', args:['pointer','pointer']} -> the harness reads no ST0 return.
// Pure leaf (only the .rdata zero-threshold 0x005d757c and the static mantissa
// table 0x00633b48; no live device globals). Avoid a zero-magnitude input: mag2==0
// hits the early-out (out[0] stays 0.0 on both sides -> degenerate/non-discriminating).
//
// BIT-IDENTITY: mag2 is summed in the 80-bit x87 stack and rounded once to f32 at
// the [edx] store; the resulting 32 bits drive the mantissa/exponent bit surgery,
// so a plain-C /arch:SSE2 reimpl would diverge. This is a __declspec(naked) verbatim
// transcription of 0x005aed20..0x005aed96. Args: [esp+4]=out (EDX), [esp+8]=in (EAX).
// __cdecl -> plain RET.
#include "../Core/HookSystem.h"

// 0x005aed20
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
AudioVectorLengthFast(float* /*out*/, float* /*in*/)
{
    __asm {
        mov    eax, dword ptr [esp + 0x8]     // in
        mov    edx, dword ptr [esp + 0x4]     // out
        fld    dword ptr [eax + 0x4]          // in[1]
        fld    dword ptr [eax]                // in[0]
        fld    dword ptr [eax + 0x8]          // in[2]
        fld    st(1)
        fmul   st(0), st(2)
        fld    st(3)
        fmul   st(0), st(4)
        faddp  st(1), st(0)
        fld    st(1)
        fmul   st(0), st(2)
        faddp  st(1), st(0)
        fstp   st(3)
        fstp   st(0)
        fstp   st(0)
        fld    st(0)
        fstp   dword ptr [edx]                // out[0] = mag2 (f32)
        fcomp  dword ptr ds:[0x005d757c]
        fnstsw ax
        test   ah, 0x44
        jnp    L_ret
        mov    ecx, dword ptr [edx]
        mov    eax, ecx
        and    ecx, 0x7fffff
        shr    eax, 0x17
        sub    eax, 0x7f
        mov    dword ptr [edx], ecx
        test   al, 0x1
        jz     L_noodd
        or     ecx, 0x800000
        mov    dword ptr [edx], ecx
    L_noodd:
        xor    ecx, ecx
        mov    cx, word ptr [edx + 0x2]
        sar    ax, 0x1
        movsx  ecx, word ptr [ecx*2 + 0x00633b48]
        movsx  eax, ax
        add    eax, 0x7f
        shl    ecx, 0x10
        shl    eax, 0x17
        or     ecx, eax
        mov    dword ptr [edx], ecx           // out[0] = fast sqrt
    L_ret:
        ret
    }
}

RH_ScopedInstall(AudioVectorLengthFast, 0x005aed20);
