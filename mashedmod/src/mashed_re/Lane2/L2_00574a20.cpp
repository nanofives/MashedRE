// ============================================================================
//  L2_00574a20.cpp -- Lane 2 port, HAND-WRITTEN (decomp2port.py refuses it: UNKNOWN_TYPE, the
//  s_AuthenticAMD string global), 2026-11-01. Checked line by line against the disassembly. A row
//  moves to C3 only through the shadow A/B plus the caller/callee gate in re-classify. Globals are
//  read through their absolute addresses (.asi target only; NOT in exe_sources.rsp).
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>
#include <intrin.h>

// ---------------------------------------------------------------------------
// 0x00574a20  FUN_00574a20  -- MASHED.exe.unpatched, verify/d4wsb_portnarrowtail_20261101/disasm_lin.py
// 00574a20 00574ab4. Size 148 B. Plate: hooks.csv row 0x00574a20.
// Ghidra types it void; its one caller (0x00567c40) KILLs EAX after the call (scan_eax_use: KILL@0x00567c45).
// ---------------------------------------------------------------------------
#define DAT_007dc8d4 (*(unsigned int*)0x007dc8d4u)   // .data
// .rdata strings: 0x00618448 "GenuineIntel", 0x00618438 "AuthenticAMD" (12 bytes each, NUL-padded).
static inline int L2T_FUN_00574920(char * a0) { return reinterpret_cast<int(__cdecl*)(char *)>(0x00574920u)(a0); }   // 0x00574920 (original thunk)
static inline int L2T_FUN_004a3a90(const char * a0, const char * a1, unsigned int a2) { return reinterpret_cast<int(__cdecl*)(const char *, const char *, unsigned int)>(0x004a3a90u)(a0, a1, a2); }   // 0x004a3a90 _strncmp (original thunk)
static inline int L2T_FUN_005749b0(void) { return reinterpret_cast<int(__cdecl*)(void)>(0x005749b0u)(); }   // 0x005749b0 (original thunk)

extern "C" void __cdecl L2_FUN_00574a20(void);
static void __cdecl L2_FUN_00574a20_impl(void)
{
  char local_c[12];                                                    // [esp+4..0xf] (`lea eax,[esp+4]` at 0x00574a23)
  unsigned int uVar2 = 1;                                              // `mov esi,1` at 0x00574a2a
  if (L2T_FUN_00574920(local_c) != 0) {                                // 0x00574a2f..0x00574a39
    if (L2T_FUN_004a3a90(local_c, (const char *)0x00618448u, 0xc) != 0 &&     // 0x00574a3b..0x00574a51
        L2T_FUN_004a3a90(local_c, (const char *)0x00618438u, 0xc) != 0) {     // 0x00574a53..0x00574a69
      uVar2 = 0;                                                       // 0x00574a6b
    }
    int r[4];
    __cpuid(r, 1);                                                     // `mov eax,1` / `cpuid` at 0x00574a75..0x00574a7b
    if ((r[3] & 0x2000000) == 0) {                                     // EDX & 0x2000000 at 0x00574a7e..0x00574a8e
      uVar2 = 0;                                                       // 0x00574a90
    }
    if (L2T_FUN_005749b0() != 0) {                                     // 0x00574a92..0x00574a99
      DAT_007dc8d4 = uVar2;                                            // 0x00574aa8
      return;
    }
  }
  DAT_007dc8d4 = 0;                                                    // 0x00574a9b..0x00574a9d
}
extern "C" void __cdecl L2_FUN_00574a20(void) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_00574a20", 0x00574a20u, ShadowAB::kPhaseAny);
    ShadowAB::RunTracked(ab, L2_FUN_00574a20_impl, SHADOW_STACK_WINDOW());
}
RH_ScopedInstall(L2_FUN_00574a20, 0x00574a20);
#undef DAT_007dc8d4
