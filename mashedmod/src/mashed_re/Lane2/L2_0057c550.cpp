// ============================================================================
//  L2_0057c550.cpp -- Lane 2 port, HAND-WRITTEN (decomp2port.py refuses it: CALLEE_REG_ARG on
//  0x0055b930), 2026-11-01. Checked line by line against the disassembly. A row moves to C3 only
//  through the shadow A/B plus the caller/callee gate in re-classify. Globals are read through their
//  absolute addresses (.asi target only; NOT in exe_sources.rsp).
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x0057c550  FUN_0057c550  -- MASHED.exe.unpatched, verify/d4wsb_portnarrowtail_20261101/disasm_lin.py
// 0057c550 0057c58e. Size 62 B. Plate: hooks.csv row 0x0057c550.
// The two callees share one pushed block (`push esi` / call 0x0055b930 / `push eax` / call 0x00564190 /
// `add esp,8` at 0x0057c55e..0x0057c56d). Each reads exactly one stack arg: 0x0055b930 is
// `mov eax,[esp+4]; mov ecx,[eax]; mov [esp+4],ecx; jmp 0x55bad0` (Ghidra shows 0 params) and 0x00564190
// reads only [esp+4] before `jmp [RwGlobals+0x10c]`. Both rewrite their own arg slot; nothing here reads it
// back, so each port call pushes one fresh arg.
// ---------------------------------------------------------------------------
static inline void L2T_FUN_0055b930(int a0) { reinterpret_cast<void(__cdecl*)(int)>(0x0055b930u)(a0); }   // 0x0055b930 (original thunk)
static inline void L2T_FUN_00564190(unsigned int a0) { reinterpret_cast<void(__cdecl*)(unsigned int)>(0x00564190u)(a0); }   // 0x00564190 (original thunk)
// `call dword ptr [ecx+0x10c]` / `add esp,4` at 0x0057c57e..0x0057c584: RwGlobals slot +0x10c, one cdecl arg.
static inline void L2_slot_007d3ff8_0x10c_1(unsigned int a0) { reinterpret_cast<void(__cdecl*)(unsigned int)>(*reinterpret_cast<unsigned int*>(*reinterpret_cast<unsigned int*>(0x007d3ff8u) + 0x10c))(a0); }

extern "C" unsigned int __cdecl L2_FUN_0057c550(int param_1);
static unsigned int __cdecl L2_FUN_0057c550_impl(int param_1)
{
  const int iVar1 = *(int *)(param_1 + 0x50) - 1;                     // 0x0057c555..0x0057c558
  *(int *)(param_1 + 0x50) = iVar1;                                   // 0x0057c559
  if (iVar1 == 0) {                                                   // jne at 0x0057c55c
    L2T_FUN_0055b930(param_1);                                        // 0x0057c55e..0x0057c55f
    L2T_FUN_00564190(*(unsigned int *)(param_1 + 0x4c));              // 0x0057c564..0x0057c568
  }
  if (*(int *)(param_1 + 0x50) == 0) {                                // re-read at 0x0057c570..0x0057c575
    L2_slot_007d3ff8_0x10c_1((unsigned int)param_1);                  // 0x0057c577..0x0057c584
  }
  return 1;                                                           // 0x0057c587
}
extern "C" unsigned int __cdecl L2_FUN_0057c550(int param_1) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_0057c550", 0x0057c550u, ShadowAB::kPhaseAny);
    return ShadowAB::RunTracked(ab, L2_FUN_0057c550_impl, SHADOW_STACK_WINDOW(), param_1);
}
RH_ScopedInstall(L2_FUN_0057c550, 0x0057c550);
