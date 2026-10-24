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

// audiolink 2026-10-23: the exe arm reads private copies of the two data the original reads by address, the .asi arm
// keeps the original's addresses (its body is unchanged). Bytes: verify/audiolink_20261023/bytes_step.txt.
//   0x005d757c (.rdata) = 0x00000000, the zero threshold of the fcomp.
//   0x00633b48 (.data, file-backed) = the 256-entry table below. The index is word [out+2] of a mantissa of at most
//   24 bits (0x005aed7a..0x005aed80), so 0..0xff.
#ifdef MASHED_STANDALONE
extern "C" const float g_audioZero_005d757c = 0.0f;
// 0x00633b48: 256 x s16 mantissa-sqrt table, dumped from MASHED.exe.unpatched (file-backed .data) by
// verify/audiolink_20261023 (audiolink 2026-10-23). Index = the upper word of the 24-bit mantissa (0..0xff).
extern "C" const short g_audioSqrtLut_00633b48[256] = {
         0,      0,      0,      1,      1,      2,      2,      3,      3,      4,      4,      5,      5,      6,      6,      7,
         7,      8,      8,      9,      9,     10,     10,     11,     11,     11,     12,     12,     13,     13,     14,     14,
        15,     15,     16,     16,     16,     17,     17,     18,     18,     19,     19,     19,     20,     20,     21,     21,
        22,     22,     22,     23,     23,     24,     24,     25,     25,     25,     26,     26,     27,     27,     27,     28,
        28,     29,     29,     29,     30,     30,     31,     31,     32,     32,     32,     33,     33,     33,     34,     34,
        35,     35,     35,     36,     36,     37,     37,     37,     38,     38,     39,     39,     39,     40,     40,     40,
        41,     41,     42,     42,     42,     43,     43,     43,     44,     44,     45,     45,     45,     46,     46,     46,
        47,     47,     48,     48,     48,     49,     49,     49,     50,     50,     50,     51,     51,     51,     52,     52,
        53,     53,     54,     55,     55,     56,     57,     57,     58,     59,     59,     60,     61,     61,     62,     63,
        64,     64,     65,     65,     66,     67,     67,     68,     69,     69,     70,     71,     71,     72,     73,     73,
        74,     75,     75,     76,     76,     77,     78,     78,     79,     80,     80,     81,     81,     82,     83,     83,
        84,     84,     85,     86,     86,     87,     87,     88,     89,     89,     90,     90,     91,     91,     92,     93,
        93,     94,     94,     95,     96,     96,     97,     97,     98,     98,     99,     99,    100,    101,    101,    102,
       102,    103,    103,    104,    104,    105,    106,    106,    107,    107,    108,    108,    109,    109,    110,    110,
       111,    112,    112,    113,    113,    114,    114,    115,    115,    116,    116,    117,    117,    118,    118,    119,
       119,    120,    120,    121,    121,    122,    122,    123,    123,    124,    124,    125,    125,    126,    126,    127,
};
#endif

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
#ifdef MASHED_STANDALONE
        fcomp  dword ptr [g_audioZero_005d757c]
#else
        fcomp  dword ptr ds:[0x005d757c]
#endif
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
#ifdef MASHED_STANDALONE
        movsx  ecx, word ptr [g_audioSqrtLut_00633b48 + ecx*2]
#else
        movsx  ecx, word ptr [ecx*2 + 0x00633b48]
#endif
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
