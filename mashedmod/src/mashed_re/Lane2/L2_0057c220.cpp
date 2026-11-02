// ============================================================================
//  L2_0057c220.cpp -- Lane 2 port, HAND-WRITTEN (decomp2port.py refuses it: CALLEE_REG_ARG on
//  0x0055dff0), 2026-11-01. Checked line by line against the disassembly. A row moves to C3 only
//  through the shadow A/B plus the caller/callee gate in re-classify. Globals are read through their
//  absolute addresses (.asi target only; NOT in exe_sources.rsp).
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x0057c220  FUN_0057c220  -- MASHED.exe.unpatched, verify/d4wsb_portnarrowtail_20261101/disasm_lin.py
// 0057c220 0057c26a. Size 74 B. Plate: hooks.csv row 0x0057c220.
// The generator refused the 0x0055dff0 call: Ghidra's prototype shows 1 stack param, the site pushes 2
// (`push ecx` (0) / `push esi` at 0x0057c252..0x0057c253, `add esp,8` at 0x0057c25c). 0x0055dff0 is the
// C4 port RwpBodyRefreshGate(int body, int idx) (Collision/RwpIntegrator.cpp:485), 2 cdecl args.
// ---------------------------------------------------------------------------
#define DAT_007dc8d8 (*(int*)0x007dc8d8u)   // .data
static inline void L2T_FUN_0055dff0(int a0, int a1) { reinterpret_cast<void(__cdecl*)(int, int)>(0x0055dff0u)(a0, a1); }   // 0x0055dff0 (raw-RVA thunk)

extern "C" int __cdecl L2_FUN_0057c220(int param_1, int param_2);
static int __cdecl L2_FUN_0057c220_impl(int param_1, int param_2)
{
  int *piVar1 = (int *)(DAT_007dc8d8 + param_1);                      // 0x0057c220, `lea ebx,[eax+edi]` at 0x0057c22e
  const int iVar2 = *piVar1;                                          // 0x0057c231
  if (iVar2 != 0) {                                                   // 0x0057c234..0x0057c236
    *(int *)(*(int *)(iVar2 + 0x10) + 8) = 0;                         // 0x0057c238..0x0057c23b
    *(int *)(iVar2 + 0x30) = 0;                                       // 0x0057c23e
  }
  if (param_2 != 0) {                                                 // 0x0057c245..0x0057c247
    *(int *)(*(int *)(param_2 + 0x10) + 8) = *(int *)(param_1 + 4) + 0x10;   // 0x0057c249..0x0057c254
    L2T_FUN_0055dff0(param_2, 0);                                     // 0x0057c252..0x0057c25c
    *(int *)(param_2 + 0x30) = param_1;                               // 0x0057c25f
  }
  *piVar1 = param_2;                                                  // 0x0057c264
  return param_1;                                                     // `mov eax,edi` at 0x0057c262
}
extern "C" int __cdecl L2_FUN_0057c220(int param_1, int param_2) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0057c220", 0x0057c220u, ShadowAB::kPhaseAny);
    return ShadowAB::RunTracked(ab, L2_FUN_0057c220_impl, SHADOW_STACK_WINDOW(), param_1, param_2);
}
RH_ScopedInstall(L2_FUN_0057c220, 0x0057c220);
#undef DAT_007dc8d8
