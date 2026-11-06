// Mashed RE — promote-round round 250 (RenderWare driver-system dispatcher).
//
// Function in this file:
//   0x004c2c90  DriverSystemDispatch — dispatches a command through a
//               caller-supplied device vtable, then supplies defaults for the
//               commands the device declined to handle.
//
// Analysis note: re/analysis/render_promote_c2_rw_plugin/0x004c2c90.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// DriverSystemDispatch  --  0x004c2c90
//
// Original: FUN_004c2c90 (0x004c2c90..0x004c2d4e), jump table at 0x004c2d50.
//
// SIGNATURE, recovered from the prologue's stack reads (2026-09-12). Ghidra
// names only param_1; the rest are read through registers. With E = entry ESP:
//   0x004c2c97  mov ecx,[esp+0x0c]  -> E+0x04  arg1  device object
//   0x004c2ca1  mov ebp,[esp+0x18]  -> E+0x08  arg2  command
//   0x004c2c9c  mov ebx,[esp+0x18]  -> E+0x0c  arg3  out-pointer
//   0x004c2c93  mov eax,[esp+0x18]  -> E+0x10  arg4  in1
//   0x004c2ca7  mov edi,[esp+0x2c]  -> E+0x14  arg5  in2
// (Each read is at a different ESP depth because the pushes interleave; the
// three `[esp+0x18]` reads are three DIFFERENT slots for exactly that reason.
// Working the offsets rather than pattern-matching them is what separates
// arg2 from arg3 from arg4 here.)
//
// BODY:
//   1. Dispatch to the device first:                       [0x004c2caf]
//        status = (*(int(**)(int,int*,int,int))(arg1 + 4))(cmd, out, in1, in2);
//      The four pushes at 0x004c2cab..0x004c2cae and the `add esp,0x10` at
//      0x004c2cb4 fix the arity at 4 and the convention as __cdecl.
//   2. If the device handled it (status != 0) return status.[0x004c2cb7]
//   3. Otherwise index a 6-entry jump table with cmd-0xd,   [0x004c2cbf]
//      range-checked unsigned against 5:                    [0x004c2cc2]
//        0xd  -> *out = 1;  return 1                        [0x004c2cce]
//        0xe  -> if (in2 == 0) { error callback; return 1 }  [0x004c2ce1]
//                else fall to default
//        0xf  -> *out = 0;  return 1                        [0x004c2d04]
//        0x10 -> if (in2 == 0) return 1 else fall to default [0x004c2d17]
//        0x11 -> return 1   (*out NOT written)               [0x004c2d0a]
//        0x12 -> return 1   (*out NOT written)               [0x004c2d0a]
//      Table contents read from the image at 0x004c2d50:
//        0x004c2cce, 0x004c2ce1, 0x004c2d04, 0x004c2d17, 0x004c2d0a, 0x004c2d0a
//   4. Default (cmd outside 0xd..0x12, or the two fall-throughs):
//                                                            [0x004c2d24]
//        local = 1; local2 = FUN_004d7ff0(0x18, cmd); FUN_004d8480(&local);
//        return status;   // still 0 on the from-top path   [0x004c2d45]
//
// NOTE the three arms that all `return 1` and differ ONLY in the out-pointer:
// 0xd writes 1, 0xf writes 0, and 0x11/0x12 leave it untouched. The return
// value alone cannot tell them apart, which is why the A/B for this function
// observes the out-buffer and not just the return.
//
// Constants (all cited above): 0xd table base, 5 unsigned range bound,
// 0x18 and 0x006180f8 in the default/error arms.
//
// Uncertainties: U-0227, U-0228 (open, NOT gating — Blocks = nothing).
// Stubs: S-0220, S-0221.
// ---------------------------------------------------------------------------

typedef int(__cdecl* RwDeviceDispatch_t)(int /*cmd*/, int* /*out*/, int /*in1*/, int /*in2*/);
typedef int(__cdecl* RwErrorPush_t)(int /*a*/, int /*b*/);
typedef int(__cdecl* RwErrorReport_t)(int* /*rec*/);

// MATCH-S19: the body of 0x004c2c90 (DriverSystemDispatch) moved to Match/MatchS19_004c2c90.cpp,
// which compiles the byte-matched source re/match/s19/004c2c90.c.
