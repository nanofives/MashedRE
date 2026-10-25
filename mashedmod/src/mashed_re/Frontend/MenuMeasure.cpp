// Mashed RE - menu message width FUN_004282a0: ONE body for both build targets (D4-DC-82a0, 2026-10-25).
//
// Before this file the RVA had two bodies (dual_copy_allowlist.txt CROSS-TARGET:004282a0):
//   Frontend/MenuMenusB.cpp  .asi-only naked copy of the instruction stream, calling the original's
//                            FUN_00427780 / FUN_004277a0 / FUN_005554d0 and reading [0x0067d838] /
//                            [0x0067d830] at their absolute addresses (the C3 evidence covers this one)
//   exe_main.cpp             exe-only C copy over g_msg_dat / g_font (MeasureMenuMessage)
// This TU is in BOTH exe_sources.rsp and asi_sources.rsp. The body below is the .asi's naked copy with the
// three callees and the two data reads moved behind cells (MenuMeasure.h) that each target fills:
//   .asi  Frontend/MenuMenusB.cpp: the original's 0x00427780 / 0x004277a0 / 0x005554d0, 0x0067d838, 0x0067d830
//   exe   exe_main.cpp: GetMenuMessage over g_msg_dat, the g_font glyph sum, the derived divisor 0x3f7fffff
// The two .rdata constants the body multiplies by are this TU's own floats, bit-equal to the original's
// (read from original/MASHED.exe.unpatched: [0x005cd5fc] = 0x3d90ff97 = 0.0708f, [0x005cd618] = 0x44200000
// = 640.0f), so the .asi no longer reads the original's .rdata for them.
//
// Precision: the body is x87 instructions, so it computes at the calling thread's precision control, as
// the original does. The exe caller (exe_main.cpp MeasureMenuMessage) scopes PC_24, the precision the
// original's game thread runs at (every return of the 2026-10-12 probe is float-exact).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include "MenuMeasure.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// MenuMenusBA  --  0x004282a0
//
// Original: FUN_004282a0  (body 0x004282a0..0x00428319, stack cookie present)
// Signature: float10 FUN_004282a0(undefined4 param_1, float param_2)
//   param_1: message id (passed to FUN_00427780)
//   param_2: scale factor (multiplied by _DAT_005cd5fc)
//   Returns: scaled logical width
//
// Logic:
//   FUN_00427780(param_1);
//   FUN_004277a0();
//   fVar1 = (float10)FUN_005554d0(DAT_0067d838, local_404, param_2 * _DAT_005cd5fc);
//   return (fVar1 / (float10)_DAT_0067d830) * (float10)_DAT_005cd618;
//
// Globals:
//   DAT_0067d838  font context           (0x004282dd)
//   _DAT_005cd5fc  size scale float      (0x004282d7)
//   _DAT_0067d830  divisor float         (0x004282f1)
//   _DAT_005cd618  logical width scale   (0x00428309)
//
// FUN_004277a0 is a REGISTER-ARG function (2026-06-07 navigate-C4 fix): it reads its source from EAX (the
// pointer FUN_00427780 returns) and writes to EBX (the caller's buffer local_404; 0x004282c7 LEA EBX,[ESP+8]).
//
// RE-PORTED FROM THE BYTES 2026-10-12 (negctl re-earn). A C body stored the FUN_005554d0 ST0 result in a
// float before the fdiv/fmul, so it rounded where the original does not: the original keeps the width in
// ST0 through fdiv [0x0067d830] @0x004282f1 and fmul [0x005cd618] @0x00428309 and returns it in ST0. With
// DAT_0067d830 = 800.0, (id 40, size 8.0) came back 0x4235b37c on the original and 0x4235b37d on the C body.
// The body is the instruction sequence, so the question of which precision C would round at does not arise.
//
// Calls go through memory cells (a naked body cannot emit the original E8 rel32). The /GS cookie
// (0x004282a6..0x004282b3, 0x004282f7/0x00428301, jmp 0x004a2be9 @0x00428315) is not ported: the slot at
// [esp+0x400] is kept so every displacement is the original's, and the tail jmp becomes ret.
//
// ref: re/analysis/frontend_promote_menus_b/004282a0.md
// ---------------------------------------------------------------------------

static const float s_menuBa_SizeScale    = 0.0708f;  // [0x005cd5fc] = 0x3d90ff97
static const float s_menuBa_LogicalScale = 640.0f;   // [0x005cd618] = 0x44200000

// 0x004282a0
extern "C" __declspec(dllexport) __declspec(naked) float __cdecl MenuMenusBA(
    std::uint32_t /*param_1*/, float /*param_2*/)
{
    __asm {
        sub  esp, 0x404                         // 0x004282a0  0x400 buffer + cookie slot
        push ebx                                // 0x004282b2
        mov  eax, dword ptr [esp + 0x40c]       // 0x004282ba  param_1
        push eax                                // 0x004282c1
        call dword ptr [g_menuBa_Lookup]        // 0x004282c2  FUN_00427780 -> EAX
        lea  ebx, [esp + 8]                     // 0x004282c7  EBX = buffer
        call dword ptr [g_menuBa_Remap]         // 0x004282cb  FUN_004277a0(EAX, EBX)
        fld  dword ptr [esp + 0x414]            // 0x004282d0  param_2
        fmul dword ptr [s_menuBa_SizeScale]     // 0x004282d7  * 0.0708
        mov  edx, dword ptr [g_menuBa_FontCell] // 0x004282dd  font = [0x0067d838]
        mov  edx, dword ptr [edx]
        lea  ecx, [esp + 8]                     // 0x004282e3  buffer
        fstp dword ptr [esp]                    // 0x004282e7  reuse the param_1 slot as arg3
        push ecx                                // 0x004282ea  arg2
        push edx                                // 0x004282eb  arg1
        call dword ptr [g_menuBa_Measure]       // 0x004282ec  FUN_005554d0 -> ST0
        mov  eax, dword ptr [g_menuBa_DivCell]
        fdiv dword ptr [eax]                    // 0x004282f1  ST0 / [0x0067d830]
        add  esp, 0xc                           // 0x004282fe
        pop  ebx                                // 0x00428308
        fmul dword ptr [s_menuBa_LogicalScale]  // 0x00428309  * 640.0, left in ST0
        add  esp, 0x404                         // 0x0042830f
        ret                                     // 0x00428315  (jmp 0x004a2be9 cookie check)
    }
}

RH_ScopedInstall(MenuMenusBA, 0x004282a0);  // installs in the .asi, no-op in the exe
