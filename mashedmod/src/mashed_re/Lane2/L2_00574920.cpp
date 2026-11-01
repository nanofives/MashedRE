// ============================================================================
//  L2_00574920.cpp -- Lane 2 port, decomp2port.py output (2026-11-01) with the body HAND-WRITTEN:
//  Ghidra's `cpuid_basic_info(0)` pseudo-op has no C equivalent and the SEH frame is not in the
//  decompilation. Checked line by line against the disassembly. A row moves to C3 only through the
//  shadow A/B plus the caller/callee gate in re-classify. .asi target only (NOT in exe_sources.rsp).
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>
#include <intrin.h>

// ---------------------------------------------------------------------------
// 0x00574920  FUN_00574920  -- MASHED.exe.unpatched, verify/d4wsb_portnarrowtail_20261101/disasm_lin.py
// 00574920 0057497e. Size 94 B. Plate: hooks.csv row 0x00574920.
//  - SEH frame (`push 0x5e5948` / `push 0x4ac660` / fs:[0] at 0x00574923..0x00574936). Scope table 0x005e5948 =
//    {-1, filter 0x0057497e, handler 0x00574984}: the filter is `mov eax,1; ret` (EXCEPTION_EXECUTE_HANDLER)
//    and the handler sets EAX = 0 (`xor eax,eax` at 0x0057498e) and returns. Reproduced with __try / __except(1).
//  - `mov eax,0` / `cpuid` at 0x0057494d..0x00574953, then EBX -> [p], EDX -> [p+4], ECX -> [p+8]
//    (0x00574958..0x0057495e). __cpuid returns {EAX, EBX, ECX, EDX}. Ghidra's `cpuid_basic_info` record
//    orders them {EAX, EBX, EDX, ECX}, which is why its text reads +4, +8, +0xc.
//  - Returns 1 (`mov eax,1` at 0x00574968).
// ---------------------------------------------------------------------------
extern "C" unsigned int __cdecl L2_FUN_00574920(unsigned int * param_1);
static unsigned int __cdecl L2_FUN_00574920_impl(unsigned int * param_1)
{
  __try {
    int r[4];
    __cpuid(r, 0);                                   // 0x0057494d..0x00574953
    param_1[0] = (unsigned int)r[1];                 // EBX, 0x00574958
    param_1[1] = (unsigned int)r[3];                 // EDX, 0x0057495b
    param_1[2] = (unsigned int)r[2];                 // ECX, 0x0057495e
  }
  __except (1) {                                     // filter 0x0057497e
    return 0;                                        // handler 0x00574984..0x0057498e
  }
  return 1;
}
extern "C" unsigned int __cdecl L2_FUN_00574920(unsigned int * param_1) {
    SHADOW_AB_COUNTER(ab, "L2_FUN_00574920", 0x00574920u, ShadowAB::kPhaseAny);
    return ShadowAB::RunTracked(ab, L2_FUN_00574920_impl, SHADOW_STACK_WINDOW(), param_1);
}
RH_ScopedInstall(L2_FUN_00574920, 0x00574920);
