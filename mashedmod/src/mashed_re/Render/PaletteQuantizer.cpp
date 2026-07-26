// Mashed RE — RenderWare octree palette-quantizer leaves (c3-batch-render-p2w1a-s1).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched)
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Subsystem: rw-palette-quantizer (octree-based palette reduction, on the
// RenderWare texture-conversion path). Three pure-leaf x87 float routines, all
// with callees_depth1 == []. Callers (all render, C2+): FUN_004d9050 (insert
// main loop), FUN_004d9530 (palette-build entry), FUN_004da480 (single-cluster
// emit), FUN_004d9d00 (recursive stats-merge).
//
// Functions promoted C2 -> C3 in this file:
//   0x004d9360  PaletteStatsAccumulate — weighted per-channel + energy accumulate
//   0x004d9a60  PaletteMeanToRgba      — mean-of-cluster -> clamped RGBA bytes
//   0x004d9ee0  PaletteStatsCombine    — merge two stats blobs + between-set energy
//
// BIT-IDENTITY NOTE (why naked __asm, not C):
//   The original keeps every float intermediate on the 80-bit x87 stack with a
//   specific pattern of 32-bit spills to the local frame. The .asi is compiled
//   with the MSVC x86 default (/arch:SSE2), whose scalar SSE math carries only
//   24-bit-mantissa 32-bit intermediates and would diverge by ULPs — and both
//   0x004d9360 and 0x004d9a60 feed their float results through truncation
//   (FISTP with an explicit round-toward-zero control word for 0x004d9a60), so a
//   sub-ULP difference flips the stored integer at boundary inputs. The only way
//   to match every input bit-for-bit is to reproduce the exact x87 instruction
//   stream. Each function below is therefore a __declspec(naked) verbatim
//   transcription of the listing, using the same stack frame and the same
//   absolute-address .rdata/.data constant reads as the original. Precedent:
//   Math/CosineLerp.cpp, Math/FPURound.cpp.
//
// Constants read live from MASHED's image (values documented, but the reimpl
// dereferences the absolute addresses — never hardcodes them):
//   0x00618430  s32  octree depth (6)          — residual bit-width = 8 - depth
//   0x005ceb90  f32  0x3b808081 (~1/255)       — per-byte residual normalizer
//   0x005d8d74  f32  0x3e46de6b                — blue prescale (0x004d9360)
//   0x005d8d78  f32  0x3f02660d                — red  prescale (0x004d9360)
//   0x005d8d7c  f32  0x437ffd71 (~256)         — shared per-channel scale (0x004d9a60)
//   0x005d8d80  f32  0x40a4c59d                — blue post-scale (0x004d9a60)
//   0x005d8d84  f32  0x3ffb4a7d                — red  post-scale (0x004d9a60)
//   0x005cc320  f32  0x3f800000 (1.0)          — numerator for 1/count
//   0x005d757c  f32  0x00000000 (0.0)          — count epsilon (0x004d9ee0)
//
// Analysis plates (C2):
//   re/analysis/bucket_004d7ac0/0x004d9360.md
//   re/analysis/bucket_004d7ac0/0x004d9a60.md
//   re/analysis/bucket_004d7ac0/0x004d9ee0.md

#include "../Core/HookSystem.h"

// ---------------------------------------------------------------------------
// PaletteStatsAccumulate — 0x004d9360
//   void FUN_004d9360(float *param_1 /*accumulator*/, byte *param_2 /*residual
//   bytes*/, float param_3 /*weight*/)
//
// Extracts four channel residuals (each byte & ((1<<(8-depth))-1)), normalizes
// them (* 0x005ceb90), applies red/blue prescales, and adds weighted (*param_3)
// contributions to the accumulator:
//   param_1[0] += weight
//   param_1[5] += (r^2 + g^2 + b^2 + a^2) * weight
//   param_1[1..4] += channel * weight
// Verbatim transcription of 0x004d9360..0x004d9493 (SUB ESP,0x18 frame; args at
// [esp+0x1c]=param_1, [esp+0x20]=param_2, [esp+0x24]=param_3 after the sub).
// ---------------------------------------------------------------------------

// 0x004d9360
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
PaletteStatsAccumulate(float* /*param_1*/, unsigned char* /*param_2*/, float /*param_3*/)
{
    __asm {
        sub   esp, 18h
        mov   eax, dword ptr ds:[0x00618430]   // octree depth
        mov   ecx, 8
        sub   ecx, eax                          // 8 - depth
        mov   al, 1
        shl   al, cl                            // 1 << (8 - depth)
        mov   ecx, dword ptr [esp+20h]          // param_2 (residual bytes)
        mov   dl, byte ptr [ecx]
        dec   al                                // mask = (1<<(8-depth)) - 1
        and   dl, al
        mov   byte ptr [esp], dl
        mov   dl, byte ptr [ecx+1]
        and   dl, al
        mov   byte ptr [esp+1], dl
        mov   dl, byte ptr [ecx+2]
        mov   cl, byte ptr [ecx+3]
        and   dl, al
        and   cl, al
        mov   byte ptr [esp+2], dl
        mov   byte ptr [esp+3], cl
        mov   edx, dword ptr [esp]
        mov   eax, dword ptr [esp+1]
        and   edx, 0FFh
        mov   ecx, dword ptr [esp+2]
        mov   dword ptr [esp+20h], edx
        and   eax, 0FFh
        fild  dword ptr [esp+20h]
        mov   dword ptr [esp+20h], eax
        mov   edx, dword ptr [esp+3]
        and   ecx, 0FFh
        and   edx, 0FFh
        fmul  dword ptr ds:[0x005ceb90]
        fild  dword ptr [esp+20h]
        mov   dword ptr [esp+20h], ecx
        mov   eax, dword ptr [esp+1Ch]          // param_1 (accumulator)
        fmul  dword ptr ds:[0x005ceb90]
        fild  dword ptr [esp+20h]
        mov   dword ptr [esp+20h], edx
        fmul  dword ptr ds:[0x005ceb90]
        fild  dword ptr [esp+20h]
        fmul  dword ptr ds:[0x005ceb90]
        fstp  dword ptr [esp+14h]
        fxch  st(2)
        fmul  dword ptr ds:[0x005d8d78]         // red prescale
        fxch  st(2)
        fmul  dword ptr ds:[0x005d8d74]         // blue prescale
        fstp  dword ptr [esp+10h]
        fld   dword ptr [esp+24h]               // weight
        fadd  dword ptr [eax]
        fstp  dword ptr [eax]                   // param_1[0] += weight
        fld   dword ptr [esp+14h]
        fmul  dword ptr [esp+14h]
        fld   dword ptr [esp+10h]
        fmul  dword ptr [esp+10h]
        faddp st(1), st(0)
        fld   st(1)
        fmul  st(0), st(2)
        faddp st(1), st(0)
        fld   st(2)
        fmul  st(0), st(3)
        faddp st(1), st(0)
        fmul  dword ptr [esp+24h]
        fadd  dword ptr [eax+14h]
        fstp  dword ptr [eax+14h]               // param_1[5] += energy * weight
        fxch  st(1)
        fmul  dword ptr [esp+24h]
        fstp  dword ptr [esp+8]
        fmul  dword ptr [esp+24h]
        fld   dword ptr [esp+10h]
        fmul  dword ptr [esp+24h]
        fld   dword ptr [esp+14h]
        fmul  dword ptr [esp+24h]
        fstp  dword ptr [esp+14h]
        fld   dword ptr [eax+4]
        fadd  dword ptr [esp+8]
        fstp  dword ptr [eax+4]                 // param_1[1] += r * weight
        fld   dword ptr [eax+8]
        fadd  st(0), st(2)
        fstp  dword ptr [eax+8]                 // param_1[2] += g * weight
        fld   dword ptr [eax+0Ch]
        fadd  st(0), st(1)
        fstp  dword ptr [eax+0Ch]               // param_1[3] += b * weight
        fstp  st(0)
        fstp  st(0)
        fld   dword ptr [eax+10h]
        fadd  dword ptr [esp+14h]
        fstp  dword ptr [eax+10h]               // param_1[4] += a * weight
        add   esp, 18h
        ret
    }
}

RH_ScopedInstall(PaletteStatsAccumulate, 0x004d9360);

// ---------------------------------------------------------------------------
// PaletteMeanToRgba — 0x004d9a60
//   void FUN_004d9a60(undefined1 *param_1 /*out RGBA bytes*/, float *param_2
//   /*stats: [0]=count, [1..4]=channel sums*/)
//
// Recovers the per-channel mean (sum / count), applies scale + post-scale,
// truncates toward zero (FISTP under an OR-AH-0x0C round-to-zero control word),
// and clamps to [0..0xfe] with an explicit 0xff default. Verbatim transcription
// of 0x004d9a60..0x004d9b9e (SUB ESP,0x28 frame; args [esp+0x2c]=param_1,
// [esp+0x30]=param_2 after the sub — [esp+0x30] is later reused as the FLDCW
// scratch, exactly as the original does).
// ---------------------------------------------------------------------------

// 0x004d9a60
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
PaletteMeanToRgba(unsigned char* /*param_1*/, float* /*param_2*/)
{
    __asm {
        sub   esp, 28h
        mov   eax, dword ptr [esp+30h]          // param_2 (stats)
        fld   dword ptr ds:[0x005cc320]         // 1.0
        fdiv  dword ptr [eax]                    // 1.0 / count
        fld   dword ptr [eax+4]
        fmul  st(0), st(1)
        fld   dword ptr [eax+8]
        fmul  st(0), st(2)
        fstp  dword ptr [esp+1Ch]
        fld   dword ptr [eax+0Ch]
        fmul  st(0), st(2)
        fstp  dword ptr [esp+20h]
        fld   dword ptr [eax+10h]
        fmul  st(0), st(2)
        fstp  dword ptr [esp+24h]
        fmul  dword ptr ds:[0x005d8d84]         // red post-scale
        fstp  st(1)
        fld   dword ptr [esp+20h]
        fmul  dword ptr ds:[0x005d8d80]         // blue post-scale
        fstp  dword ptr [esp+20h]
        fmul  dword ptr ds:[0x005d8d7c]         // shared scale (ch0)
        fstp  dword ptr [esp+4]
        fnstcw word ptr [esp+2]
        fld   dword ptr [esp+4]
        mov   ax, word ptr [esp+2]
        or    ah, 0Ch                           // round toward zero
        mov   word ptr [esp+30h], ax
        fldcw word ptr [esp+30h]
        fistp dword ptr [esp+8]                  // ch0 -> int (trunc)
        fldcw word ptr [esp+2]
        fld   dword ptr [esp+1Ch]
        fmul  dword ptr ds:[0x005d8d7c]         // shared scale (ch1)
        fstp  dword ptr [esp+4]
        fnstcw word ptr [esp+2]
        fld   dword ptr [esp+4]
        mov   ax, word ptr [esp+2]
        or    ah, 0Ch
        mov   word ptr [esp+30h], ax
        fldcw word ptr [esp+30h]
        fistp dword ptr [esp+0Ch]               // ch1 -> int
        fldcw word ptr [esp+2]
        fld   dword ptr [esp+20h]
        fmul  dword ptr ds:[0x005d8d7c]         // shared scale (ch2)
        fstp  dword ptr [esp+4]
        fnstcw word ptr [esp+2]
        fld   dword ptr [esp+4]
        mov   ax, word ptr [esp+2]
        or    ah, 0Ch
        mov   word ptr [esp+30h], ax
        fldcw word ptr [esp+30h]
        fistp dword ptr [esp+10h]               // ch2 -> int
        fldcw word ptr [esp+2]
        fld   dword ptr [esp+24h]
        fmul  dword ptr ds:[0x005d8d7c]         // shared scale (ch3)
        fstp  dword ptr [esp+4]
        fnstcw word ptr [esp+2]
        fld   dword ptr [esp+4]
        mov   ax, word ptr [esp+2]
        or    ah, 0Ch
        mov   word ptr [esp+30h], ax
        fldcw word ptr [esp+30h]
        fistp dword ptr [esp+14h]               // ch3 -> int
        fldcw word ptr [esp+2]
        mov   eax, dword ptr [esp+2Ch]          // param_1 (out)
        mov   ecx, dword ptr [esp+8]
        mov   edx, 0FFh
        cmp   ecx, edx
        mov   byte ptr [eax], dl                // default 0xff
        jge   pmr_c1
        mov   byte ptr [eax], cl                // < 0xff -> store value
    pmr_c1:
        mov   ecx, dword ptr [esp+0Ch]
        mov   byte ptr [eax+1], dl
        cmp   ecx, edx
        jge   pmr_c2
        mov   byte ptr [eax+1], cl
    pmr_c2:
        mov   ecx, dword ptr [esp+10h]
        mov   byte ptr [eax+2], dl
        cmp   ecx, edx
        jge   pmr_c3
        mov   byte ptr [eax+2], cl
    pmr_c3:
        mov   ecx, dword ptr [esp+14h]
        mov   byte ptr [eax+3], dl
        cmp   ecx, edx
        jge   pmr_c4
        mov   byte ptr [eax+3], cl
    pmr_c4:
        add   esp, 28h
        ret
    }
}

RH_ScopedInstall(PaletteMeanToRgba, 0x004d9a60);

// ---------------------------------------------------------------------------
// PaletteStatsCombine — 0x004d9ee0
//   void FUN_004d9ee0(float *param_1 /*out*/, float *param_2, float *param_3)
//
// Combines two stats blobs. Default energy = param_2[5] + param_3[5]. When both
// counts exceed the epsilon (0x005d757c), adds the between-set variance term
//   (dr^2 + dg^2 + db^2 + da^2) / (1/count_A + 1/count_B)
// where d = sumA/countA - sumB/countB. Channel sums and counts are summed
// directly. Verbatim transcription of 0x004d9ee0..0x004d9ff3 (SUB ESP,0x20 then
// PUSH ESI; args after the push at [esp+0x28]=param_1, [esp+0x28]/[esp+0x2c]
// pre-push = param_2/param_3 in ECX/EDX; ESI = param_1 out).
// ---------------------------------------------------------------------------

// 0x004d9ee0
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl
PaletteStatsCombine(float* /*param_1*/, float* /*param_2*/, float* /*param_3*/)
{
    __asm {
        sub   esp, 20h
        mov   ecx, dword ptr [esp+28h]          // param_2
        mov   edx, dword ptr [esp+2Ch]          // param_3
        push  esi
        mov   esi, dword ptr [esp+28h]          // param_1 (out) after push
        fld   dword ptr [ecx+14h]
        fadd  dword ptr [edx+14h]               // param_2[5] + param_3[5]
        fstp  dword ptr [esp+2Ch]
        mov   eax, dword ptr [esp+2Ch]
        mov   dword ptr [esi+14h], eax          // param_1[5] = default energy
        fld   dword ptr [ecx]
        fcomp dword ptr ds:[0x005d757c]         // count_A vs epsilon
        fnstsw ax
        and   eax, 4100h
        jnz   psc_skip
        fld   dword ptr [edx]
        fcomp dword ptr ds:[0x005d757c]         // count_B vs epsilon
        fnstsw ax
        and   eax, 4100h
        jnz   psc_skip
        fld   dword ptr ds:[0x005cc320]
        fdiv  dword ptr [ecx]                    // 1/count_A
        fld   dword ptr ds:[0x005cc320]
        fdiv  dword ptr [edx]                    // 1/count_B
        fld   dword ptr [ecx+4]
        fmul  st(0), st(2)                       // sumA1 * (1/cA)
        fld   dword ptr [ecx+8]
        fmul  st(0), st(3)
        fld   dword ptr [ecx+0Ch]
        fmul  st(0), st(4)
        fstp  dword ptr [esp+0Ch]
        fld   dword ptr [ecx+10h]
        fmul  st(0), st(4)
        fstp  dword ptr [esp+10h]
        fld   dword ptr [edx+4]
        fmul  st(0), st(3)                       // sumB1 * (1/cB)
        fld   dword ptr [edx+8]
        fmul  st(0), st(4)
        fstp  dword ptr [esp+18h]
        fld   dword ptr [edx+0Ch]
        fmul  st(0), st(4)
        fstp  dword ptr [esp+1Ch]
        fld   dword ptr [edx+10h]
        fmul  st(0), st(4)
        fstp  dword ptr [esp+20h]
        fxch  st(2)
        fsub  st(0), st(2)                       // d1 = meanA1 - meanB1
        fstp  dword ptr [esp+4]
        fstp  st(1)
        fsub  dword ptr [esp+18h]                // d2 = meanA2 - meanB2
        fld   dword ptr [esp+0Ch]
        fsub  dword ptr [esp+1Ch]                // d3
        fld   dword ptr [esp+10h]
        fsub  dword ptr [esp+20h]                // d4
        fld   st(0)
        fmul  st(0), st(1)
        fld   st(2)
        fmul  st(0), st(3)
        faddp st(1), st(0)
        fld   st(3)
        fmul  st(0), st(4)
        faddp st(1), st(0)
        fld   dword ptr [esp+4]
        fmul  dword ptr [esp+4]
        faddp st(1), st(0)
        fxch  st(4)
        fadd  st(0), st(5)                       // (1/cA) + (1/cB)
        fdivp st(4), st(0)
        fxch  st(3)
        fadd  dword ptr [esp+2Ch]                // + default energy
        fstp  dword ptr [esi+14h]                // param_1[5] = combined energy
        fstp  st(2)
        fstp  st(1)
        fstp  st(0)
        fstp  st(0)
    psc_skip:
        fld   dword ptr [ecx+4]
        fadd  dword ptr [edx+4]
        fstp  dword ptr [esi+4]                  // param_1[1] = sums
        fld   dword ptr [ecx+8]
        fadd  dword ptr [edx+8]
        fstp  dword ptr [esi+8]
        fld   dword ptr [ecx+0Ch]
        fadd  dword ptr [edx+0Ch]
        fstp  dword ptr [esi+0Ch]
        fld   dword ptr [ecx+10h]
        fadd  dword ptr [edx+10h]
        fstp  dword ptr [esi+10h]
        fld   dword ptr [ecx]
        fadd  dword ptr [edx]
        fstp  dword ptr [esi]                    // param_1[0] = count sum
        pop   esi
        add   esp, 20h
        ret
    }
}

RH_ScopedInstall(PaletteStatsCombine, 0x004d9ee0);
