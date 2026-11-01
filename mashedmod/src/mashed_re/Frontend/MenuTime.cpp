// Mashed RE - Menu time-delta decomposition reimplementation.
// Analysis note: re/analysis/promote_c2_frontend_menus/0x0042d300.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// TimeDiffDecompose  --  0x0042d300
//
// Original: FUN_0042d300 (128 bytes, 0x0042d300..0x0042d380)
// No callees; pure arithmetic leaf.
//
// Signature:
//   void (int time_a, int time_b,
//         uint32_t* sign_out,   // out: 0=positive diff, 1=negative, 2=equal
//         int*      min_out,    // out: minutes component
//         int*      sec_out,    // out: seconds component
//         float*    csec_out)   // out: centiseconds remainder
//
// Time unit: centiseconds (1/100 s).
// Constants cited from 0x0042d300 body:
//   6000 — centiseconds per minute
//   100  — centiseconds per second
//   60 (0x3c) — seconds per minute (used in csec remainder formula)
//
// Sign code convention (from decomp at 0x0042d300):
//   diff == 0            → *sign_out = 2  (equal)
//   diff <  0 (negated)  → *sign_out = 1  (time_a < time_b; time_a was faster)
//   diff >  0            → *sign_out = 0  (time_a > time_b; time_b was faster)
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042d300.md
// ---------------------------------------------------------------------------

// MATCH-S07: the body of 0x0042d300 (TimeDiffDecompose) moved to Match/MatchS07_0042d300.cpp,
// which compiles the byte-matched source re/match/s07/0042d300.c.
