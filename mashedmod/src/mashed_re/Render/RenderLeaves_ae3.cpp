// Mashed RE — Render leaf reimplementations (c3-batch-ae session 3).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// HARVEST RESULT (11 candidates classified):
//   VIABLE (1): 0x004b65e0  PizGlobalsZero6  — void(void); zeroes 6 globals.
//   SKIP   (10) — reasons recorded in re/PROMOTION_QUEUE.md row for this session:
//     0x0047b160  needs-live-state — script-VM handler; calls FUN_004b6fc0 /
//                 FUN_004b7090(x2) / FUN_004a2c48 and writes a float to
//                 &DAT_0086cbc8 + FUN_004a2c48()*0x110 (slot indexed by live
//                 VM state). NOT a force-callable leaf (decomp at 0x0047b160).
//     0x004b52c0  needs-new-arg_type — void(int* base, uint mask, int set);
//                 (ptr,scalar,scalar)->read field+8 has no existing
//                 diff_template.js arg_type (sort_dispatch_out4 misuse rejected).
//     0x004b65c0  needs-live-state — copies 0x82b dwords from &DAT_008ab7e0;
//                 source block is all-zero on disk (.bss, runtime-populated;
//                 verified via Ghidra memory_read 0x008ab7e0), so a struct_call_observe
//                 test compares zeros vs zeros -> false-GREEN vs a `return;` stub.
//     0x004516d0  needs-new-arg_type — undefined4(undefined4 val, int* cursor);
//                 (scalar, cursor-struct-ptr w/ seeded index+base) has no existing
//                 arg_type.
//     0x004b68f0  needs-live-state — name lookup over table at &DAT_0090dac0 with
//                 count DAT_008ad9a8; count is 0 on disk (verified Ghidra read),
//                 so the loop never runs -> always returns 0 -> false-GREEN vs
//                 `return 0;` stub.
//     0x004b4650  needs-new-arg_type — Vec3Lerp void(out*, A*, B*, float t);
//                 3 vec3 pointers + a float scalar — no existing arg_type.
//     0x004ec720  D3DX9-PSGP library band (statically-linked Microsoft PSGP,
//                 calibrated 0x004ec000..0x004fc9e0) — reclass-OUT material, not
//                 a C3-promotable game leaf.
//     0x004ec740  D3DX9-PSGP library band (same as 0x004ec720).
//     0x004ec79d  D3DX9-PSGP library band (IEEE float32->half packer).
//     0x004ecaab  D3DX9-PSGP library band (Vec2 barycentric interp).
//
// Analysis note for the viable leaf:
//   re/analysis/render_3_c1_to_c2_s5/FUN_004b65e0.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// PizGlobalsZero6  --  0x004b65e0
//
// Original: FUN_004b65e0 (32 bytes, 0x004b65e0..0x004b6600)
// Signature: void FUN_004b65e0(void)
// Returns: void
//
// Decompilation (cited from 0x004b65e0, verified via Ghidra 2026-06-04):
//   _DAT_008eda28 = 0;
//   _DAT_008eda2c = 0;
//   _DAT_008eda30 = 0;
//   _DAT_0090daa0 = 0;
//   _DAT_0090daa4 = 0;
//   _DAT_0090daa8 = 0;
//   return;
//
// Zeroes six global dwords in two contiguous groups of three.
//
// Constants (cited from 0x004b65e0 body):
//   0x008eda28, 0x008eda2c, 0x008eda30 — group 1 (three contiguous dwords)
//   0x0090daa0, 0x0090daa4, 0x0090daa8 — group 2 (three contiguous dwords)
//
// Uncertainties (non-blocking, data-semantic only):
//   [UNCERTAIN] semantic role of the two triples — the C1 note reads them as
//   piz I/O counters / read-position accumulators cleared on open/shutdown,
//   but that meaning is not derivable from this function alone.
//
// Callers (cited): FUN_004b67a0 (piz shutdown), FUN_004b6940 (piz open/parse reset).
//
// ref: re/analysis/render_3_c1_to_c2_s5/FUN_004b65e0.md
// ---------------------------------------------------------------------------

// 0x004b65e0
extern "C" __declspec(dllexport) void __cdecl PizGlobalsZero6(void)
{
    // Group 1 — three contiguous dwords. [0x004b65e0 body]
    *reinterpret_cast<std::uint32_t*>(0x008eda28u) = 0u;  // _DAT_008eda28 = 0
    *reinterpret_cast<std::uint32_t*>(0x008eda2cu) = 0u;  // _DAT_008eda2c = 0
    *reinterpret_cast<std::uint32_t*>(0x008eda30u) = 0u;  // _DAT_008eda30 = 0
    // Group 2 — three contiguous dwords. [0x004b65e0 body]
    *reinterpret_cast<std::uint32_t*>(0x0090daa0u) = 0u;  // _DAT_0090daa0 = 0
    *reinterpret_cast<std::uint32_t*>(0x0090daa4u) = 0u;  // _DAT_0090daa4 = 0
    *reinterpret_cast<std::uint32_t*>(0x0090daa8u) = 0u;  // _DAT_0090daa8 = 0
}

RH_ScopedInstall(PizGlobalsZero6, 0x004b65e0);
