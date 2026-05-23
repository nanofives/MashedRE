// Mashed RE — Util mixed-cluster C3 sweep (wave1-s6).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// ─── 0x00412f30  FUN_00412f30 (HUD event/icon batch builder) — DEFERRED ─────
//   Callee gate blocks C2->C3:
//     0x0046d4a0  FUN_0046d4a0 — C1 (vehicle pos ptr getter)
//     0x00467210  FUN_00467210 — C1 (vehicle sub-obj getter)
//     0x0041f0d0  FUN_0041f0d0 — C1 (player state getter)
//     0x00412e30  FUN_00412e30 — C1 (event array appender)
//   All four depth-1 callees are at C1 — cannot satisfy the C2->C3 callee gate
//   requirement ("at least one callee at C2 or higher").
//   Additionally, four [UNCERTAIN] items remain in the body:
//     event IDs 0x19-0x1c/0x37-0x3a/0x2a semantics unknown
//     DAT_007f0fd0 game-mode enum values not confirmed
//     FUN_00412cf0/FUN_00412e30 output structure (stride 0x2c) not documented
//     _DAT_005cc750/_DAT_005cd05c player-count fraction path unresolved
//   Prior session c3-batch-k already filed D-9340 for this function;
//   no new DEFERRED row needed.
//   Re-pickup condition: bring 0x0046d4a0, 0x00467210, 0x0041f0d0, 0x00412e30
//   to C2+; resolve the four [UNCERTAIN] items.
//   Analysis: re/analysis/util_c0_promote/0x00412f30.md

// This file is intentionally empty of implementations.
// It is not compiled into the ASI (no entry in build.bat).
