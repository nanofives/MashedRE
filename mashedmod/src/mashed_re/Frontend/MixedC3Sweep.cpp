// Mashed RE — Frontend mixed-cluster C3 sweep (wave1-s6).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Both frontend candidates DEFERRED — see reasons below.
//
// ─── 0x00403050  FUN_00403050 (LoadingScreen renderer) — DEFERRED ───────────
//   Guardrail: "LoadingScreen (0x00403050) is live-render gated — if reimpl
//   needs live render state, DEFER."
//   The function calls vtable dispatch at DAT_007d3ff8+0x20 to set D3D render
//   states (Z-test/Z-write) and renders textured quads + animated sprites.
//   All calls require an active D3D9 device (DAT_007d3ff8 non-null, render
//   context initialised). Authoring without live render state risks crashing
//   the dev harness.
//   Re-pickup condition: when the D3D9 device init path is confirmed bootable
//   in the hook DLL and per-frame render dispatch is exercised past state 1
//   sub-state 0x21 with the dev harness loaded.
//   Note: confidence is C2 (not C3); callee gate holds but live-render gate blocks.
//   Analysis: re/analysis/loading_screen/0x00403050.md
//
// ─── 0x0042bcb0  FUN_0042bcb0 (input-icon renderer) — DEFERRED ──────────────
//   Confidence is C1 only (note ends with "Ready for C0->C1 promotion: YES",
//   no C2 promotion evidence on file). Three [UNCERTAIN] items in body:
//     U-3452: FUN_004c5c00 signature unknown
//     U-3453: FUN_004b5750 drawing-call semantics unknown
//     U-3454: DAT_007e96fc mode values unconfirmed
//   Two unresolved stubs:
//     S-3429: FUN_004c5c00
//     S-3430: FUN_004b5750
//   Authoring at C1 with unresolved stubs would require guessing semantics.
//   Re-pickup condition: promote to C2 (decompile end-to-end, document shape),
//   resolve U-3452/U-3453/U-3454, bring at least one callee to C2+.
//   Analysis: re/analysis/c0_promotion_frontend_a/0x0042bcb0.md

// This file is intentionally empty of implementations.
// It is not compiled into the ASI (no entry in build.bat).
