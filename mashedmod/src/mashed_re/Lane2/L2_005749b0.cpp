// ============================================================================
//  L2_005749b0.cpp -- Lane 2 port, decomp2port.py output (2026-11-01) with the body HAND-WRITTEN:
//  the decompilation drops the SEH frame and the `xorps` probe it guards. Checked against the
//  disassembly. A row moves to C3 only through the shadow A/B plus the caller/callee gate in
//  re-classify. .asi target only (NOT in exe_sources.rsp).
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x005749b0  FUN_005749b0  -- MASHED.exe.unpatched, verify/d4wsb_portnarrowtail_20261101/disasm_lin.py
// 005749b0 005749fd. Size 77 B. Plate: hooks.csv row 0x005749b0.
//  - SEH frame (0x005749b3..0x005749c6). Scope table 0x005e5958 = {-1, filter 0x005749fd, handler 0x00574a03}:
//    the filter is `mov eax,1; ret` and the handler sets EAX = 0 (`xor eax,eax` at 0x00574a0d) and returns.
//  - Guarded body: `xorps xmm0, xmm0` at 0x005749dd, then `mov eax,1` at 0x005749e7.
//    Ghidra's text keeps only the `return 1`; the probe instruction is reproduced in inline asm.
// ---------------------------------------------------------------------------
extern "C" unsigned int __cdecl L2_FUN_005749b0(void);
static unsigned int __cdecl L2_FUN_005749b0_impl(void)
{
  __try {
    __asm xorps xmm0, xmm0                           // 0x005749dd
  }
  __except (1) {                                     // filter 0x005749fd
    return 0;                                        // handler 0x00574a03..0x00574a0d
  }
  return 1;                                          // 0x005749e7
}
extern "C" unsigned int __cdecl L2_FUN_005749b0(void) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_005749b0", 0x005749b0u, ShadowAB::kPhaseAny);
    return ShadowAB::RunTracked(ab, L2_FUN_005749b0_impl, SHADOW_STACK_WINDOW());
}
RH_ScopedInstall(L2_FUN_005749b0, 0x005749b0);
