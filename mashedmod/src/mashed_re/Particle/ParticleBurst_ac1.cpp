// Mashed RE -- Particle burst wrapper (promote-round wf_b0f68acd, 2026-06-26).
//
// Binary anchor:
//   original\MASHED.exe SHA-256 (preserved in .unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//
// Functions:
//   0x00486610  ParticleBurst4  -- void __cdecl(uint32_t param_1, uint32_t param_2)

#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x00486610  FUN_00486610  ParticleBurst4
//
// 20-byte thin wrapper. Passes its two arguments to FUN_004864f0 with a
// hard-coded count of 4.
//
// Decompiler (Ghidra pool13, 2026-06-26):
//   void FUN_00486610(undefined4 param_1, undefined4 param_2)
//   {
//       FUN_004864f0(param_1, param_2, 4);   // 0x00486617: push 4; call 0x004864f0
//       return;
//   }
//
// Calling convention: __cdecl (caller-clean stack; no callee-clean RET N).
//
// Callee FUN_004864f0 (0x004864f0, C2, particle):
//   - Copies 16 uint32s from param_1 into a local stack matrix.
//   - Guards on squared distance to last-spawn position vs _DAT_005cc9a0
//     (= 0x3d4ccccd = 0.05f; confirmed Ghidra pool13, 2026-06-26).
//   - If distance >= threshold: updates _DAT_006fe780/84/88 and spawns
//     param_3 particles. Otherwise returns early.
//
// Called by FUN_00487020 (C2, render).
// ref: re/analysis/particle_promote_ac1/0x00486610.md
// ─────────────────────────────────────────────────────────────────────────────

// MATCH-S14: the body of 0x00486610 (ParticleBurst4) moved to Match/MatchS14_00486610.cpp,
// which compiles the byte-matched source re/match/s14/00486610.c.
