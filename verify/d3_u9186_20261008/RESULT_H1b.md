# RESULT — U-9186 leg H1b: a standalone body for `FUN_0046d4a0`

Date 2026-10-08. Pre-registration: `PREREG_H1.md` §1 (H1b). **RAN.** No C-level. Nothing
default-ON. `original/` untouched.

## 0. Verdict first

**H1b lands, and it landed as a CONSOLIDATION rather than the second copy it was scoped as.**
`FUN_0046d4a0` now has one body, in a TU shared by both targets, and the `.asi`-only copy is gone.

| gate | verdict | figure |
|---|---|---|
| `H1b-OK` | **PASS** | `PtrCompute881ec8` returns 1 on **53,992/53,992 (100.0000%)**, all three arms |
| `H1b-MATCH` | **PASS** | the x/z read off the returned pointer equals `rec_x_rec`/`rec_z_rec` on **100.0000%** of rows |
| `H1b-LIVE` | **PASS** | `ptr_x` has **22,031** distinct values — the pointer resolves into live state, not a constant |
| `rva-lint` | **PASS** | `122 known, NEW=0` — the consolidation **removed** a pair instead of adding one |
| `H1-KNOBOFF` | **PASS** | `H1step.csv` vs `E2off_1.csv` still **identical over the whole overlap** |
| `H1-CONVERGE` / `H1-GATE2` / `H1-ALL` | **PASS** | unchanged from H1a: 100% / 100% / 75% |

Raw: `H1b_KNOBOFF.txt`, `H1{base,seed,both}.gates.csv`. H1a's run is preserved as `*_r2`.

## 1. Why the shape changed

H1b was scoped as an exe-only second body beside the `.asi` copy — the `file` / `exe_file`
dual-copy pattern `hooks.csv` already uses in many rows. The build's `scripts/lint_rva_bodies.py`
rejected it twice:

- `DUP-IN-TARGET` when the body went into `Vehicle/VehicleState.cpp` behind `#ifdef
  MASHED_STANDALONE` — the lint is textual and cannot see the preprocessor, and it was right that
  a both-targets TU is the wrong home.
- `CROSS-TARGET` when it moved to an exe-only TU — which is the dual-copy class itself.

The lint's own policy note is the reason the second shape is also wrong:

> Policy (2026-09-29): WARN on anything already listed in `re/tools/dual_copy_allowlist.txt`,
> **FAIL on anything new**. The allowlist is a burn-down list, not a permanent exemption —
> ROADMAP D4 takes it to zero by consolidating each pair into one shared TU judged against the
> original.

A new pair would have been new debt D4 is committed to removing. So the body was **consolidated**
into `Vehicle/VehicleRecordPtr.cpp`, compiled into **both** targets, and the copy at
`Util/PromoLoop_round58.cpp:43-50` was deleted (a pointer comment and the disassembly
transcription stay, since that transcription is this RVA's primary citation).

## 2. Why the consolidation is safe for the `.asi`

`VehRecord()` (`Vehicle/VehicleRecordBase.h`) resolves to `0x008815a0 + idx*0xd04` when
`MASHED_STANDALONE` is **not** defined, and `kOff_WheelSetSel` / `kOff_WheelMatrices` are `0x9a8` /
`0x928`, which reproduce `0x00881f48` / `0x00881ec8` exactly. The `.asi` arm therefore computes the
same addresses the deleted copy did, and keeps the behaviour its C3 Frida evidence
(`log/diff_ptr_compute_881ec8.csv`, 10/10 GREEN 2026-06-13) measured. `RH_ScopedInstall` moved with
the body and is a no-op in the exe build.

**That evidence still says nothing about the standalone arm** — it exercised the absolute form.
The standalone arm's own witness is `H1b-MATCH` above: the body is called, and the world x/z read
through the pointer it returns agree with `VehiclePhysics_RecordF32` on every row.

## 3. What this changes about the tracker update

The scope expected `hooks.csv:659` to gain an `exe_file`. There is no second file, so the update is
simpler: `file` moves from `Util/PromoLoop_round58.cpp` to `Vehicle/VehicleRecordPtr.cpp`, and the
notes record the consolidation plus the standalone arm's separate witness. `exe_file` stays empty
because one shared TU serves both targets. Applied through `re-classify`.

Confidence is **not** promoted by this leg. The `.asi` arm keeps C3 on unchanged arithmetic; the
standalone arm has a liveness-and-agreement witness, not a Frida A/B against the original, so it
does not carry C3 on its own.

## 4. Next

- **H2** — the `MASHED_SLOTSTATE_SEED` default-ON question.
- **H3** (`FUN_00442a60`) — still under the `DEFERRED.md:15` "DO NOT start leg B" prohibition.
  H1a and H1b together mean its three readers would now return live values, which is what the
  prohibition's risk assessment did not assume. Lifting it is a user decision.
