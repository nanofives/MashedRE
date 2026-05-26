// Mashed RE — c3_batch_t session 4 — hud_frontend_d2 dispatcher cluster.
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Session: c3-batch-t-s4 (Opus 4.7 1M, K=10 high-throughput slot).
// Branch:  c3/batch-t-s4
// Cluster: 10 candidates, all C2->C3 attempts.
//
// RESULT: 0 GREEN. All 10 candidates DEFERRED.
//
// Root cause: every candidate's analysis note is shape-only (callees, globals,
// rectangle layout, control-flow sketch) — NONE contains the per-RVA decomp
// transcript needed to author a byte-faithful reimplementation. The C3
// promotion gate requires Frida A/B bit-identity; without the actual decomp
// body, authoring would be guessing, which violates NO-GUESSING (CLAUDE.md).
//
// Precedent for the gold-standard authoring shape: SpriteCluster.cpp lines
// 260..580 — ProgressBarSetA (0x00430b90, 1693 bytes) consumes ~250 lines of
// cited source plus 5 callee stubs. Our 10 candidates run 300..9400 bytes
// each; clean authoring would exceed this session's budget even with the
// decomp bodies in hand.
//
// To re-pick up: re-decompile each candidate via ghidra-pool slot, produce a
// per-RVA decomp transcript (the existing notes only describe shape), then
// run as a focused authoring session (one or two RVAs per session, not ten).
//
// ─── Per-candidate disposition ─────────────────────────────────────────────

// ─── 0x00430120  FUN_00430120 (time-trial mode info panel) ─────────────────
//   Body: 300 bytes (smallest candidate).
//   STATUS: DEFERRED (live-render gate + callee gate).
//   - Calls vtable `(**(DAT_007d3ff8 + 0x20))(6/8, 0/1)` — same live-render
//     vtable that MixedC3Sweep.cpp explicitly defers as a guardrail
//     ("LoadingScreen ... is live-render gated — if reimpl needs live render
//     state, DEFER"). At quiescent main menu this might `crash_equal_ok` like
//     ProgressBarSetA, but that needs path1 verification we don't have here.
//   - Only documented callee is FUN_00427e00 (C1) — anti-island violation
//     (need C2+).
//   Re-pickup: promote 0x00427e00 to C2; verify the render-state vtable
//   path crashes identically with the hook bypassed (crash_equal_ok pattern).
//   Analysis: re/analysis/hud_frontend_d2/0x00430120.md

// ─── 0x00431240  FUN_00431240 ──────────────────────────────────────────────
//   Body: 5-9 KB. STATUS: DEFERRED (insufficient decomp transcript).
//   Analysis: re/analysis/hud_frontend_d2/0x00431240.md (52 lines — shape only).
//   Re-pickup: produce full decomp transcript via Ghidra MCP, then dedicate
//   a focused authoring session.

// ─── 0x004314b0  FUN_004314b0 ──────────────────────────────────────────────
//   Body: 5-9 KB. STATUS: DEFERRED (insufficient decomp transcript).
//   Analysis: re/analysis/hud_frontend_d2/0x004314b0.md (50 lines — shape only).
//   Re-pickup: same as 0x00431240.

// ─── 0x00431710  FUN_00431710 (Per-player progress bar set B) ──────────────
//   Body: 975 bytes. STATUS: DEFERRED (described as analogous to 0x00430b90
//   ProgressBarSetA but with substantive differences — different alpha
//   threshold global, different fill-value source DAT_0067eaa8 vs per-slot
//   table, different case-2/case-3 callee pattern, ~720-byte size delta).
//   Cloning ProgressBarSetA without the actual decomp body would not produce
//   bit-identity. Need the per-RVA decomp transcript.
//   Analysis: re/analysis/hud_frontend_d2/0x00431710.md (42 lines).

// ─── 0x004335f0  FUN_004335f0 (car-selection / race-position HUD) ──────────
//   Body: ~3402 bytes. STATUS: DEFERRED.
//   Analysis: re/analysis/hud_frontend_d2/0x004335f0.md (74 lines — shape
//   only; lists MP/SP branch, 7-color inline table, struct iteration but no
//   per-instruction decomp). Callees include 4 uncatalogued (0x00430760
//   C3-impl, but 0x0042fab0/0x0042bcb0 C2-mapped, 0x004282a0 C3-impl, others
//   C1). One uncatalogued callee (0x00427f00) is C1 — anti-island risk.
//   Re-pickup: full decomp + callee promotion of 0x00427f00 to C2.

// ─── 0x00434720  FUN_00434720 ──────────────────────────────────────────────
//   Body: ~9400 bytes (largest of the 9-x candidates).
//   STATUS: DEFERRED (insufficient decomp transcript; size alone would
//   require ~400+ lines of cited source, exceeding budget).
//   Analysis: re/analysis/hud_frontend_d2/0x00434720.md (108 lines).

// ─── 0x0043a610  FUN_0043a610 (race-result scoreboard renderer) ────────────
//   Body: ~1567 bytes. STATUS: DEFERRED.
//   Analysis: re/analysis/hud_frontend_d2/0x0043a610.md (58 lines — shape).
//   Six uncatalogued callees including 0x0042f8d0, 0x0042fab0, 0x0042bcb0,
//   0x004282a0, 0x00427f00, 0x0042ac00 — most are mapped but 0x00427f00 is
//   C1 (anti-island risk). Need decomp body + 0x00427f00 promotion.

// ─── 0x0043aa30  FUN_0043aa30 ──────────────────────────────────────────────
//   Body: 5-9 KB. STATUS: DEFERRED (insufficient decomp transcript).
//   Analysis: re/analysis/hud_frontend_d2/0x0043aa30.md (62 lines).

// ─── 0x0043af10  FUN_0043af10 ──────────────────────────────────────────────
//   Body: 5-9 KB. STATUS: DEFERRED (insufficient decomp transcript).
//   Analysis: re/analysis/hud_frontend_d2/0x0043af10.md (84 lines).

// ─── 0x0043bf30  FUN_0043bf30 (large dispatcher; the parent of D-124x set) ─
//   STATUS: DEFERRED.
//   Analysis: re/analysis/hud_frontend/0x0043bf30.md (66 lines — describes
//   dispatcher table over flags 0x0067e7a8..0x0067e820 that route to the
//   other candidates in this batch). Because it dispatches into the rest
//   of this cluster — all of which are still C2 — promoting the parent
//   ahead of its children violates the bottom-up rule.
//   Re-pickup: promote at least one child path (e.g. ProgressBarSet B at
//   0x00431710) to C3 first; then this dispatcher becomes authorable.

// ─── No installs. This translation unit is intentionally empty of code.
// ─── It is not added to mashedmod/build.bat (no symbols to link).
