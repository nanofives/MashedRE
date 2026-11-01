// Mashed RE — FUN_00443300 AI Catmull-Rom spline eval: ONE body for both build targets (D4-DC-INC-ai, 2026-11-01).
//
// Before this file the RVA had three bodies:
//   Ai/AiSplineHooks.cpp  .asi-only: AiSplineEval_x87 (naked verbatim, the installed body the C3 self-test and
//                         diff judged) and AiSplineEval (a portable C reimpl, 1-4 ULP off, never called; deleted)
//   AL_443300             exe-only, a private verbatim naked copy in Ai/AiLeafTargets_naked.inc, called 12 times
//                         by FUN_00443440 / FUN_00443dc0's transcriptions
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp and holds AiSplineEval_x87, moved here unchanged except
// for its linkage (extern "C", it was file-static). AL_443300 and AiSplineEval_x87 were the same instruction
// stream: 112 instructions, identical operands and branch targets once AL_443300's exe-safe constant symbols
// (g_al_c_5cc31c..g_al_c_5d757c) are mapped onto the k-constants below, which hold the same bits. The exe's
// naked callers now `call AiSplineEval_x87` (re/tools/naked_specs/aileaf_g1.json). The .asi installs it through
// Ai/AiSplineHooks.cpp's AiSpline_Entry (self-test dispatcher), as before.
//
// Anchored to MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// .rdata bit patterns (re-confirmed 2026-07-01, Mashed_pool0 memory_read):
//   _DAT_005cc31c=0x40400000=3.0  _DAT_005cc320=0x3f800000=1.0
//   _DAT_005cc32c=0x3f000000=0.5  _DAT_005cc358=0x40a00000=5.0
//   _DAT_005cc35c=0x40800000=4.0  DAT_005d757c =0x00000000=0.0

#include <cstdint>

namespace {

// exact float32 constants (all exactly representable → bit-identical to .rdata).
// Non-const so storage is always emitted (the naked __asm below references them by
// symbol as `fmul dword ptr [k5_0]` etc. — MSVC __asm rejects bare absolute-address
// FPU operands `[05cc358h]` with C2415; the symbol form is portable + identical).
float k3_0 = 3.0f;   // _DAT_005cc31c (cubic P1*3 / P2*3)
float k1_0 = 1.0f;   // _DAT_005cc320 (t clamp upper)
float k0_5 = 0.5f;   // _DAT_005cc32c (Catmull-Rom final factor)
float k5_0 = 5.0f;   // _DAT_005cc358 (quadratic P1*5)
float k4_0 = 4.0f;   // _DAT_005cc35c (quadratic P2*4)
float k0_0 = 0.0f;   // DAT_005d757c  (t clamp lower)

}  // namespace

// ===========================================================================
// AiSplineEval_x87 — NAKED VERBATIM transcription of 0x00443300..0x00443432.
// The INSTALLED .asi body: exact 80-bit x87 ST chain → bit-identical. Callable as
// __cdecl (args at [esp+4..]) — layout matches the original entry, so the
// verbatim [esp+8]/[esp+1ch]/[esp+20h] offsets are correct whether reached by the
// installed inline-JMP or a normal call. Reads its 6 float consts from the exact copies above (the
// standalone's .rdata reads 0, so the live addresses are not used). Each ST
// index is annotated with the RVA it transcribes.
//   EBX=splineBase  ECX=index  EDI=&P0  ESI=&P1  EDX=&P2  EBP=outXZ
//   [ebx+ecx*8]=P3.x  [eax]=P3.z ; x87 keeps t^3(ST0),t^2(ST1),t(ST2) at bottom.
// ===========================================================================
// 0x00443300
extern "C" __declspec(naked) void __cdecl AiSplineEval_x87(int /*splineBase*/, int /*segIdx*/, float /*t*/, float* /*outXZ*/)
{
    __asm {
        mov     ecx, dword ptr [esp+8]        // 00443300  segIdx
        push    ebx                           // 00443304
        mov     ebx, dword ptr [esp+8]        // 00443305  splineBase
        push    ebp                           // 00443309
        dec     ecx                           // 0044330a
        test    ecx, ecx                      // 0044330b
        push    esi                           // 0044330d
        push    edi                           // 0044330e
        jge     L317                          // 0044330f
        add     ecx, dword ptr [ebx+200h]     // 00443311  P0 wrap +count
    L317:
        mov     eax, dword ptr [ebx+200h]     // 00443317  count
        lea     edi, [ebx+ecx*8]              // 0044331d  &P0
        inc     ecx                           // 00443320
        cmp     ecx, eax                      // 00443321
        jl      L327                          // 00443323
        sub     ecx, eax                      // 00443325
    L327:
        lea     esi, [ebx+ecx*8]              // 00443327  &P1
        inc     ecx                           // 0044332a
        cmp     ecx, eax                      // 0044332b
        jl      L331                          // 0044332d
        sub     ecx, eax                      // 0044332f
    L331:
        lea     edx, [ebx+ecx*8]              // 00443331  &P2
        inc     ecx                           // 00443334
        cmp     ecx, eax                      // 00443335
        jl      L33b                          // 00443337
        sub     ecx, eax                      // 00443339  ECX = P3 index
    L33b:
        fld     dword ptr [esp+1ch]           // 0044333b  t
        fld     dword ptr [esp+1ch]           // 0044333f  t
        fcomp   dword ptr [k1_0]          // 00443343  vs 1.0
        fnstsw  ax                            // 00443349
        test    ah, 41h                       // 0044334b
        jnz     L35a                          // 0044334e
        fstp    st(0)                         // 00443350
        fld     dword ptr [k1_0]          // 00443352  -> 1.0
        jmp     L373                          // 00443358
    L35a:
        fld     dword ptr [esp+1ch]           // 0044335a  t
        fcomp   dword ptr [k0_0]          // 0044335e  vs 0.0
        fnstsw  ax                            // 00443364
        test    ah, 5                         // 00443366
        jp      L373                          // 00443369
        fstp    st(0)                         // 0044336b
        fld     dword ptr [k0_0]          // 0044336d  -> 0.0
    L373:
        fld     st(0)                         // 00443373  ST: t, t
        mov     ebp, dword ptr [esp+20h]      // 00443375  outXZ
        fmul    st(0), st(1)                  // 00443379  ST0 = t*t = t^2
        lea     eax, [ebx+ecx*8+4]            // 0044337b  &P3.z
        fld     st(0)                         // 0044337f  ST: t^2, t^2, t
        fmul    st(0), st(2)                  // 00443381  ST0 = t^2*t = t^3
        // ---- X channel (P*.x = [reg]) ----
        fld     dword ptr [edi]               // 00443383  P0x
        fadd    st(0), st(0)                  // 00443385  2*P0x
        fld     dword ptr [esi]               // 00443387  P1x
        fmul    dword ptr [k5_0]          // 00443389  5*P1x
        fsubp   st(1), st(0)                  // 0044338f  2P0x-5P1x
        fld     dword ptr [edx]               // 00443391  P2x
        fmul    dword ptr [k4_0]          // 00443393  4*P2x
        faddp   st(1), st(0)                  // 00443399  +4P2x
        fsub    dword ptr [ebx+ecx*8]         // 0044339b  -P3x  => Qx
        fmul    st(0), st(2)                  // 0044339e  Qx * t^2
        fld     dword ptr [esi]               // 004433a0  P1x
        fmul    dword ptr [k3_0]          // 004433a2  3*P1x
        fsub    dword ptr [edi]               // 004433a8  3P1x-P0x
        fld     dword ptr [edx]               // 004433aa  P2x
        fmul    dword ptr [k3_0]          // 004433ac  3*P2x
        fsubp   st(1), st(0)                  // 004433b2  (3P1x-P0x)-3P2x
        fadd    dword ptr [ebx+ecx*8]         // 004433b4  +P3x  => Cx
        fmul    st(0), st(2)                  // 004433b7  Cx * t^3
        faddp   st(1), st(0)                  // 004433b9  Cx*t^3 + Qx*t^2
        fld     dword ptr [edx]               // 004433bb  P2x
        fsub    dword ptr [edi]               // 004433bd  P2x-P0x
        fmul    st(0), st(4)                  // 004433bf  (P2x-P0x)*t
        faddp   st(1), st(0)                  // 004433c1  + ...
        fld     dword ptr [esi]               // 004433c3  P1x
        fadd    st(0), st(0)                  // 004433c5  2*P1x
        faddp   st(1), st(0)                  // 004433c7  + 2P1x
        fmul    dword ptr [k0_5]          // 004433c9  * 0.5
        fstp    dword ptr [ebp]               // 004433cf  store X
        // ---- Z channel (P*.z = [reg+4]; P3.z = [eax]) ----
        fld     dword ptr [edi+4]             // 004433d2  P0z
        fadd    st(0), st(0)                  // 004433d5  2*P0z
        fld     dword ptr [esi+4]             // 004433d7  P1z
        fmul    dword ptr [k5_0]          // 004433da  5*P1z
        fsubp   st(1), st(0)                  // 004433e0  2P0z-5P1z
        fld     dword ptr [edx+4]             // 004433e2  P2z
        fmul    dword ptr [k4_0]          // 004433e5  4*P2z
        faddp   st(1), st(0)                  // 004433eb  +4P2z
        fsub    dword ptr [eax]               // 004433ed  -P3z  => Qz
        fmul    st(0), st(2)                  // 004433ef  Qz * t^2
        fld     dword ptr [esi+4]             // 004433f1  P1z
        fmul    dword ptr [k3_0]          // 004433f4  3*P1z
        fsub    dword ptr [edi+4]             // 004433fa  3P1z-P0z
        fld     dword ptr [edx+4]             // 004433fd  P2z
        fmul    dword ptr [k3_0]          // 00443400  3*P2z
        fsubp   st(1), st(0)                  // 00443406  (3P1z-P0z)-3P2z
        fadd    dword ptr [eax]               // 00443408  +P3z  => Cz
        fmul    st(0), st(2)                  // 0044340a  Cz * t^3
        faddp   st(1), st(0)                  // 0044340c  Cz*t^3 + Qz*t^2
        fld     dword ptr [edx+4]             // 0044340e  P2z
        fsub    dword ptr [edi+4]             // 00443411  P2z-P0z
        pop     edi                           // 00443414
        fmul    st(0), st(4)                  // 00443415  (P2z-P0z)*t
        faddp   st(1), st(0)                  // 00443417  + ...
        fld     dword ptr [esi+4]             // 00443419  P1z
        pop     esi                           // 0044341c
        fadd    st(0), st(0)                  // 0044341d  2*P1z
        faddp   st(1), st(0)                  // 0044341f  + 2P1z
        fmul    dword ptr [k0_5]          // 00443421  * 0.5
        fstp    dword ptr [ebp+4]             // 00443427  store Z
        pop     ebp                           // 0044342a
        pop     ebx                           // 0044342b
        fstp    st(0)                         // 0044342c  clear t^3
        fstp    st(0)                         // 0044342e  clear t^2
        fstp    st(0)                         // 00443430  clear t
        ret                                   // 00443432  (caller-cleans)
    }
}
