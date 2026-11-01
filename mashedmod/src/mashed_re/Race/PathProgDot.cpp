// Mashed RE — FUN_00408590 plane dot-product + counter: ONE body for both build targets (D4-DC-INC-ai, 2026-11-01).
//
// Before this file the RVA had two verbatim naked bodies, the dual-copy class
// (re/analysis/DUAL_COPY_AUDIT_2026-09-29.md, ROADMAP D4):
//   Util/PromoLoop_sessionB.cpp:Dot408590  .asi-only, installed, the body hooks.csv's C3 evidence covers
//   PP_408590 (Race/PathProgStep_naked.inc) exe-only, called by PP_408610 at 0x004088b7
// Same 39 instructions, same operands and branch targets; they differed in two operand bindings only:
//   * the three calls to FUN_00426cc0 (C3 pure: idx*0x4c + 0x663658): the .asi body called the original at
//     0x00426cc0 through `mov eax, 0x00426cc0 / call eax`, the exe body called PP_426cc0 (the exe's verbatim
//     copy in Race/PathProgStep_naked.inc; the original's VA holds nothing in the standalone).
//   * the 0.0 compare operand: ds:[0x005d757c] in the .asi, the exe-safe copy g_pp_c_5d757c in the exe
//     (the standalone's .rdata reads 0, which is also 0.0).
// The one body below takes the callee through s_fn_426cc0 (per target, `call dword ptr [...]`: EAX is the
// callee's return either way, so the dropped `mov eax, imm` changes no live value) and the 0.0 from s_k0_0,
// which holds the bits of 0x005d757c (0x00000000) in both targets.
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. RH_ScopedInstall installs it in the .asi and is a
// no-op in the exe; PP_408610's transcription now `call`s Dot408590 (re/tools/naked_specs/pathprog_p2.json).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

namespace {

#ifdef MASHED_STANDALONE
extern "C" void PP_426cc0();                                    // Race/PathProgStep_naked.inc (exe-only TU)
void* s_fn_426cc0 = reinterpret_cast<void*>(&PP_426cc0);
#else
void* s_fn_426cc0 = reinterpret_cast<void*>(0x00426cc0u);       // the original inside MASHED.exe
#endif
float s_k0_0 = 0.0f;                                            // DAT_005d757c = 0x00000000

}  // namespace

// Plate (from Util/PromoLoop_sessionB.cpp):
//   0x00408590  FUN_00408590 (gameplay, NEAR-LEAF: plane dot-product + counter)
// void f(arg1, arg2): idx=arg1[0x20]; rec=FUN_00426cc0(idx) (C3 pure: idx*0x4c+0x663658);
// dp = arg2[0x20]*rec[0] + arg2[0x24]*rec[4] + arg2[0x28]*rec[8]; arg1[0xac]=dp;
// if (dp < [0x5d757c]) arg1[0xa8]++ else arg1[0xa8]=0. Straight x87 (no st(N) juggling).
// Verbatim naked port; the 3 `call rel32` to the pure callee -> `call dword ptr [s_fn_426cc0]` (per target,
// see the header; the .asi copy used `mov eax,0x426cc0; call eax`).
// 0x00408590
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl Dot408590(void)
{
    __asm {
        push esi
        mov  esi, dword ptr [esp+8]          // arg1
        mov  eax, dword ptr [esi+0x20]       // idx
        push edi
        push eax
        call dword ptr [s_fn_426cc0]         // FUN_00426cc0
        fld  dword ptr [eax+4]
        mov  edi, dword ptr [esp+0x14]       // arg2
        mov  ecx, dword ptr [esi+0x20]
        fmul dword ptr [edi+0x24]
        push ecx
        fstp dword ptr [esp+0x14]
        call dword ptr [s_fn_426cc0]         // FUN_00426cc0
        fld  dword ptr [edi+0x20]
        mov  edx, dword ptr [esi+0x20]
        fmul dword ptr [eax]
        push edx
        fadd dword ptr [esp+0x18]
        fstp dword ptr [esp+0x18]
        call dword ptr [s_fn_426cc0]         // FUN_00426cc0
        fld  dword ptr [eax+8]
        fmul dword ptr [edi+0x28]
        add  esp, 0x0c
        fadd dword ptr [esp+0x0c]
        fst  dword ptr [esi+0xac]
        fcomp dword ptr [s_k0_0]             // DAT_005d757c (0.0)
        fnstsw ax
        test ah, 5
        jp   L_DOT_RESET
        mov  eax, dword ptr [esi+0xa8]
        inc  eax
        pop  edi
        mov  dword ptr [esi+0xa8], eax
        pop  esi
        ret
    L_DOT_RESET:
        pop  edi
        mov  dword ptr [esi+0xa8], 0
        pop  esi
        ret
    }
}
RH_ScopedInstall(Dot408590, 0x00408590);
