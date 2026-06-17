# WS-A-VERIFY — physics-chain verification findings (2026-06-16)

Verification pass on the ported vehicle-physics chain (A3/A4/A5/A6a/A6b). Anchor
SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

## Hard blocker for the behavioral lane
The booted-original `diff-original` lane could **not** run from this verification
worktree: a git worktree has **no `original/` directory** (the untouched install is
gitignored, present only in the main working copy). No `MASHED.exe`, no `.asi` deploy
target, no Frida boot from here. **The installed-hook canonical-race diff (the real
C4 gate) must run from the MAIN working tree, not a worktree.** (OS build = 26200, so
the EMULATEHEAP-on compat regime applies when it is run there.)

So this pass delivered the **static** half: resolving the open `[UNCERTAIN]`s by
decompilation (Ghidra pool11, read-only) and judging port completeness.

## Static resolutions (Ghidra pool11)
- **`FUN_004c3df0` (RwV3dTransformPoints) arg-order = (dst, src, count, matrix).**
  Body is a pure pass-through thunk: `(**(code**)(DAT_007d3ffc+0x14+DAT_007d3ff8))
  (p1,p2,p3,p4)`. The A6a call site `FUN_004c3df0(&dst, src, 1, matrix)` fixes the
  order. ForceIntegrator's `Rw_TransformPoints(dst,src,count,mtx)` decl is **CORRECT**.
  CAVEAT: it dispatches the RW **device** transform — in the standalone (no RW device
  init) the identity stub / device thunk produces wrong results; the bind step must
  supply a real C++ 3x4-matrix×vec3 transform (Math/RwV3dTransform exists). The
  Math/RwV3dTransformPoints.cpp header comment "(dst, matrix, 1, src)" is MISLEADING —
  ignore it for the physics binding.
- **`FUN_004a3384` = MSVC CRT `acos`** (CONFIRMED: `fpatan(SQRT((1-x²)(1+x²)), x)`).
  A6b's use is correct → bind to `std::acos`.
- **`FUN_004a2c48` = ROUND(ST0)** — per WS-C-WIRE's decomp (the ctrl-byte quantizer);
  not re-derived here.

## Per-function verdict
| Fn | RVA | Verdict |
|----|-----|---------|
| A3 VehicleInit | 0x0046b540 | **static-OK, behavioral BLOCKED-from-worktree.** Transcription complete for handling-table key0; `[U-A3-TABLE]` (keys beyond 0) still open. |
| A4 VehicleControl | 0x00470670 | **static-OK, behavioral BLOCKED.** Verbatim; 11 consts confirmed. |
| A5 ForceIntegrator | 0x0046ddb0 | **static-OK; dep-binding incomplete.** Transform arg-order confirmed; needs the real device-transform bind (above), not the identity stub. |
| A6a Integrate2 | 0x00467650 | **INCOMPLETE — not C4-eligible yet.** Spine only; the per-wheel contact-friction block + speed-limit grip-clamp + boost state-machine are still DEFERRED. float10→double divergence unresolved (needs behavioral). |
| A6b AeroStabilize | 0x00468980 | **INCOMPLETE.** acos dep confirmed, but the rotation-apply (two FUN_004c4d20 arg sets) is DEFERRED. |

**No function promoted to C4** (behavioral lane blocked here + A6a/A6b structurally
incomplete). No synthetic-bypass overclaim ([[feedback-no-overclaiming-c-levels]]).

## What A8 can safely wire now
A4 + A5 (control + wheel-force) are the most complete and their static deps are
resolved — but A5 needs the device-transform bind, and the chain is only meaningful
end-to-end once A6a's contact block + A6b's rotation-apply land. **Recommendation:
A8 stays gated OFF until A6a/A6b are completed AND a from-MAIN-tree behavioral diff is
run.** The next physics session should (1) finish A6a/A6b's deferred blocks, (2) add
the C++ device-transform, then (3) run the diff from the main tree.
