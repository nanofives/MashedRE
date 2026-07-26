// Mashed RE - Vec3 normalize returning the inverse-magnitude scale.
// Original: 0x004c3910  FUN_004c3910  render  C2 -> C3
//
// float FUN_004c3910(float* out, float* in): computes mag2 = dot(in,in) (x87,
// rounded to f32 at store), looks up the fast inverse-sqrt scale from the RW3
// two-level LUT (invsqrt table = slot +4 of the device globals), writes
// out[i] = in[i] * scale, and RETURNS the scale (1/|in|) in ST0. A zero-length
// input leaves scale = 0 -> out = {0,0,0}, returns 0. Sibling of RwV3dNormalize
// (0x004c39b0, which instead returns the magnitude via the sqrt table).
//
// LUT roots read live: DAT_007d3ff8 (rw globals), DAT_007d3ffc (rw offset);
// invsqrt table root = *(rw_globals + rw_offset + 4). Not a pure leaf -> the Frida
// diff must run against the live game (FastInvSqrt fires ~900/s at menu, so the
// device globals are populated at menu-state).
//
// BIT-IDENTITY: mag2 is accumulated in the 80-bit x87 stack then rounded once to
// f32 at the [esp+8] store; its 32 bits drive the LUT index, so a 1-ULP difference
// in mag2 would pick a different table entry. A plain-C /arch:SSE2 reimpl rounds at
// every step and diverges. This is a __declspec(naked) verbatim transcription of
// the body 0x004c3910..0x004c39a4. It reuses the incoming arg2 stack slot [esp+8]
// as the mag2 / scale scratch, exactly as the original does. __cdecl -> plain RET.
#include "../Core/HookSystem.h"

// 0x004c3910
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl
Vec3NormalizeScale(float* /*out*/, float* /*in*/)
{
    __asm {
        mov    eax, dword ptr [esp + 0x8]     // in
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
        fstp   dword ptr [esp + 0x8]          // mag2 (f32) -> scratch
        mov    ecx, dword ptr [esp + 0x8]     // mag2 bits
        fstp   st(0)
        fstp   st(0)
        test   ecx, ecx
        fstp   st(0)
        jz     L_skip
        mov    ecx, dword ptr ds:[0x007d3ff8]
        mov    edx, dword ptr ds:[0x007d3ffc]
        mov    ecx, dword ptr [edx + ecx + 0x4]   // invsqrt table root
        mov    edx, dword ptr [esp + 0x8]
        add    edx, 0x800
        mov    dword ptr [esp + 0x8], edx
        mov    edx, dword ptr [esp + 0x8]
        shr    edx, 0xc
        and    edx, 0xfff
        mov    ecx, dword ptr [ecx + edx*4]       // mantissa entry
        mov    edx, dword ptr [esp + 0x8]
        not    edx
        shr    edx, 1
        and    edx, 0x3fc00000                    // exponent bits
        add    ecx, edx
        mov    dword ptr [esp + 0x8], ecx          // scale bits
    L_skip:
        fld    dword ptr [esp + 0x8]          // scale
        fmul   dword ptr [eax]                // * in[0]
        mov    ecx, dword ptr [esp + 0x4]     // out
        fstp   dword ptr [ecx]                // out[0]
        fld    dword ptr [eax + 0x4]
        fmul   dword ptr [esp + 0x8]
        fstp   dword ptr [ecx + 0x4]          // out[1]
        fld    dword ptr [eax + 0x8]
        fmul   dword ptr [esp + 0x8]
        fstp   dword ptr [ecx + 0x8]          // out[2]
        fld    dword ptr [esp + 0x8]          // return scale in ST0
        ret
    }
}

RH_ScopedInstall(Vec3NormalizeScale, 0x004c3910);
