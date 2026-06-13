// Mashed RE — promote-round round 15 (round-14 Ghidra-pass carry-over).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00402f40  Util636ad8Get — util; 6B global getter
//
// Caller gate filled in round 14 (Ghidra pass, Mashed_pool2 read-only):
// reference_to 0x00402f40 -> sole caller FUN_0043dfd0 (frontend, C2) via the
// UNCONDITIONAL_CALL at 0x0043fc07.
//
// NOT included (round-15 deferral): 0x004c9eb0 — the verbatim decomp transcript
// (now in its note) resolved the best-below variable-role logic, but the body
// makes two DOUBLE-INDIRECT vtable calls (*DAT_007d4108 + 0x18 / +0x1c) whose
// calling convention is tagged `unknown` and cannot be reproduced bit-faithful
// without disassembling the call sites. Deferred to a Ghidra pass that reads
// the push/stack-cleanup pattern (NO-GUESSING).
//
// Body byte-verified in original\MASHED.exe.unpatched 2026-06-12.
//
// Analysis:
//   re/analysis/timer_d3_cont1_a/0x00402f40.md
//   re/analysis/bucket_util_00095280_0040e460/00402f40.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// Util636ad8Get  --  0x00402f40   (subsystem: util)
//
// Original: FUN_00402f40 (6 bytes, 0x00402f40..0x00402f45)
// Bytes: A1 D8 6A 63 00 / C3   (mov eax,[0x00636ad8]; ret)
// Signature: undefined4 FUN_00402f40(void)
//
// Constants (cited from function body at 0x00402f40):
//   0x00636ad8 — global dword read and returned
//
// Caller: FUN_0043dfd0 (frontend, C2; call site 0x0043fc07). Pure leaf;
// no uncertainties (plate: "Uncertainties: none").
// ---------------------------------------------------------------------------

// 0x00402f40
extern "C" __declspec(dllexport) std::uint32_t __cdecl Util636ad8Get(void) {
    // 0x00636ad8 cited at 0x00402f40 body.
    return *reinterpret_cast<const std::uint32_t*>(0x00636ad8u);
}

RH_ScopedInstall(Util636ad8Get, 0x00402f40);
