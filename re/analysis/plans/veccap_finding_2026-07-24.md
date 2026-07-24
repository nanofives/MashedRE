# VECCAP-2 — FUN_00566200 port is not x87-faithful (found 2026-07-24, account2)

**What:** attempting to onboard `FUN_00566200` (0x00566200, `Collision/RwpSolverLeaves1.cpp`,
AABB transformed by a 4×4 matrix; `v_out_2in`, n_a=7 / n_in=22 / n_out=7, pure float32) into the
veccap offline lane surfaced a **port ≠ original divergence**.

**Evidence (all offline, account2):**
- **Unicorn differ: PASS 513/513** — the *original machine code* reproduces the live-captured
  ground truth (as expected; same code).
- **Offline replay: FAIL 490/513, both modes** — the *ported C++* (MSVC `/O2`) does NOT match
  the original.
- The failure is **not an overflow artifact**: the first run used the default full-range synth
  (values ~1e38 → the function's `fVar5 - fVar8` sums-of-products overflow to inf, whose bits
  diverge under different codegen). A **bounded ~[-8,8]** synth (added: `synth_domain: 'bounded'`)
  removed all overflow and replay STILL failed 490/513.
- Python cross-check on the bounded ground truth: recomputing the decompiled expression with
  **float32 intermediates matched 23/513**, with **double intermediates 50/513** — *neither*
  reproduces the original. So the literal decompiled sum-order / intermediate precision is not
  x87-faithful.

**Diagnosis:** the port transcribes the Ghidra decomp expression order verbatim, but x87 addition
is order-sensitive and the original keeps 80-bit intermediates. The sibling file
`RwpSolverMath2.cpp` documents exactly this hazard for its own functions ("op1 rows in DISASM
summation order … the decomp prints the reversed order; x87 addition is order-sensitive [X87]").
`FUN_00566200`'s port in `RwpSolverLeaves1.cpp` did **not** apply a disasm-order / float10
correction — its intermediates are plain `float` and the sum order is the decomp's.

**Why this matters:** the menu-scenario live diff can never catch this — physics never runs at the
menu, so `FUN_00566200`'s inputs are all zero there. veccap's synthetic offline replay is the only
gate that exercises it. This is the VECCAP-1 class of finding (a port that silently diverges
outside the menu-idle input domain).

**Status / fix (→ account3, needs Ghidra):**
1. Disasm `0x00566200` and record the exact x87 FADD/FMUL evaluation order + which intermediates
   stay 80-bit vs round to float32.
2. Re-transcribe `FUN_00566200` in `RwpSolverLeaves1.cpp` to that order, using `float10`
   intermediates where the original keeps ST0 extended (mirror the `RwpSolverMath2.cpp` quat/
   translate pattern).
3. Re-add the registry entry (kept as a documented comment in `veccap_registry.py`) with
   `'synth_domain': 'bounded'` and re-verify replay == unicorn == ground truth.

**Tool improvements KEPT from this attempt (all green, enable future wide leaves):**
- `replay_offline.cpp`: fixed buffers `[16]→[32]` (supports n_in up to ~22) + extern/`kExports`
  scaffold. `capture_vectors.py`: scratch `0x100→0x200`, `outBuf`/`retBuf` moved to `+0x100`/`+0x180`
  (no inBuf/outBuf overlap for wide n_in). `veccap_registry.synth_inputs`: `synth_domain: 'bounded'`
  path (deterministic ~[-8,8], any kind). `unicorn_diff.py` needed no change (maps the whole image;
  ARGBUF→OUTBUF gap 0x100 already fits n_in=22).
