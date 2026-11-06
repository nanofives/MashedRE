# RESULT D4-DC-INC-0046d4a0-REPLAYORDER

**Verdict: GREEN (2026-11-06).** Every PREREG gate passes. The replay after `VehiclePhysics_Init` gives
`rows=3888 exact=3888 entry_bad=0 calls_bad=0` on all four arms, as predicted. G-READS-ON has 60 rows with one base. The
flipped exe keeps the default gates on the refs: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3. Section 4 has
the scored table and the raw-file hashes. This closes D4-DC-INC-0046d4a0 and D4-DC-INC-0046d4a0-KNOB. The allowlist key
`DUP-IN-TARGET:0046d4a0` is gone (`lint_rva_bodies.py` on the tip: `1 finding(s): CROSS-TARGET=1 allowlisted=1 NEW=0`).

History: on 2026-11-02 every `scripts/diag.py probe-render` returned BLOCKED, D3DERR `0x8876086A` (display asleep), so
the first pass ended as HANDOFF with no arm run. On 2026-11-06 the branch was merged with `orch/integration`
(`9a991247c`, text only, no `mashedmod/` change), rebuilt, `probe-render` gave `RENDER: OK`, and the arms ran.

Branch `orch/D4-DC-INC-0046d4a0-REPLAYORDER`, worktree `.worktrees/D4-DC-INC-0046d4a0-REPLAYORDER`. It is based on
`orch/integration` `bf8abf3d0` and carries `orch/D4-DC-INC-0046d4a0-KNOB` `db3b9bb6e` (merge `8e88ae071`), so the merge
train takes both as one unit.

## 1. Commits

| sha | what |
|---|---|
| `8e88ae071` | merge of `orch/D4-DC-INC-0046d4a0-KNOB` |
| `3a3533f01` | order fix: `Gameplay::PickupCb_MaybeReplay()` leaves the track-load block (`D3d9Render/TrackRenderer.cpp`, path-progress block). It now runs once in `TrackRenderer::UpdateCar` right after the one-shot `VehiclePhysics_Init` / `VehiclePhysics_SetWorld` block, so `g_vehicleArrayBase` exists. Env-gated (`MASHED_PKCB_REPLAY`), and the first line returns when the variable is unset |
| `ddd8e0b77` | PREREG, committed alone, before any arm |
| `cd148e9ec` | flip: `0x00458581` calls `PtrCompute881ec8` directly (`re/tools/naked_specs/pickups_m3.json`, `Gameplay/PickupCollect_naked.inc` regenerated with `re/tools/gen_naked_asm.py`, a 3-hunk diff: the forward declaration, the call line, and the deleted 46-byte `PK_46d4a0` body). `PK_46d4a0_sel`, `MASHED_PK_ONEBODY_46D4A0` and `_WIT` are deleted (`Gameplay/PickupTick.cpp`, the RVA comment cites `0x00458579`, `0x00458586..0x0045858d`, `0x004589e2`, `0x0046d4a4..0x0046d4c8`). The allowlist key `DUP-IN-TARGET:0046d4a0` is removed |
| `8cc5a0f58` | `run_replay.py` (PREREG section 3 replay arms) |
| `3c2b2e466` | `score.py` (scores every PREREG gate from the arms folder) |

**Deviation from the brief, stated.** The brief asked for "default ON, opt-out knob". The flip has no opt-out. An
opt-out would need the second body (`PK_46d4a0`), and that body is exactly what the allowlist row tracks, so the row
could not close. The precedent is D4-DC-4d20-FLIP, which deleted `MASHED_GRID_ROTATE_RW` along with its inline form.
If the lead wants an opt-out anyway, revert `cd148e9ec` and keep the KNOB form with its default inverted. The
allowlist key then stays.

`verify/savelink_20261023/run_arms.py` stacks `PK46`, `PK46W`, `DEFW`, `PK46R4W`, `DEFR4W` were left as they are. They
now set env vars that nothing reads.

## 2. Static gates (done)

| Gate | Result | Detail |
|---|---|---|
| Build `E_FIX` (`3a3533f01`) | OK | `MASHED_NO_DEPLOY=1`. `mashed_re.exe` `724c00f6304b17ebce8019db1786381cab079e644dc830760d7c4113cebd472b`, `.asi` `83b7b82375edc65667860df6fb5fea3207569e551f03d72ac24a0173de8e5553`. Log `D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/build1.log` |
| Build `E_FLIP` (`cd148e9ec`) | OK | `mashed_re.exe` `53c6b649d22526210b78eca0ffce9fe287aad7f14cca94e6309cfbbed2e18008`, `.asi` `cdaa8506f36b810109fb929c2090797790b246f0bf1b5b0ca0faac3614bdcb70`. Log `.../build2.log` |
| **G-LINT** | **PASS** | `lint_rva_bodies.py`: `E_FIX` tree `2 finding(s) allowlisted=2 NEW=0`, `E_FLIP` tree `1 finding(s): CROSS-TARGET=1 allowlisted=1 NEW=0`. `archive_lint.py --ref orch/D4-DC-INC-0046d4a0-REPLAYORDER` (`cd148e9ec`) OK (include-closure, rva-lint, asm-lint 1133 files) |

`original\mashed_re_dev.asi` was not touched. Both exes are copied to `D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/exe_fix/`
and `exe_flip/`, and `run_replay.py` runs them from there. The private root
`D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_root` (robocopy of `original\`) is ready.

## 3. HANDOFF: remaining steps (exact)

From the worktree, once `py -3.12 scripts/diag.py probe-render` (run from the main checkout) prints `RENDER: OK`:

1. Replay arms (PREREG G-RP-OFF, G-RP-ON, G-READS-ON, G-RP-FLIP = G-M3-REPLAY-SYNTH, plus the PC_53 diagnostic):
   `test-slot bash -c 'py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D4-DC-INC-0046d4a0-REPLAYORDER -- py -3.12 verify/d4dc_0046d4a0_replayorder_20261102/run_replay.py RP_FIX_OFF RP_FIX_ON RP_FLIP RP_FLIP53'`
   If an arm prints `no replay report` and exited 0 on its own, `UpdateCar` was not reached in 60 frames. Rerun that arm with `A4_DET_FRAMES=600` (the PREREG procedural fallback).
2. Default gates on `E_FLIP`:
   `test-slot bash -c 'A4_EXE=D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/exe_flip/mashed_re.exe A4_ROOT=D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_root A4_OUT=D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/arms py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --label D4-DC-INC-0046d4a0-REPLAYORDER -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26'`
3. `py -3.12 verify/d4dc_0046d4a0_replayorder_20261102/score.py`. GREEN needs every PREREG bold gate PASS. G-DEF compares against `re/orchestrator/merge_train.json` at score time (today G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3).
   If G-DEF misses, run step 2 with `exe_fix` to split the order fix from the flip.
4. Paste the score table and the raw-file sha256 list into this RESULT, commit the RESULT plus the small extracts
   (`*.replay.txt`, `*.pk46.tsv`, `RUNLOG_replay.txt`), then run
   `roadmap.ps1 set D4-DC-INC-0046d4a0-REPLAYORDER review -Evidence verify/d4dc_0046d4a0_replayorder_20261102/RESULT_D4-DC-INC-0046d4a0-REPLAYORDER.md`.
5. Trackers: no confidence change is claimed (`0x0046d4a0` stays C3, `hooks.csv:671`). The optional re-classify note
   for that row: "pickup collect 0x00458581 calls the one body, the allowlist key was dropped by this step".

Predictions (PREREG): the replay arms give `rows=3888 exact=3888 entry_bad=0 calls_bad=0`, arms
`bump=60 collect=60 no-op=3522 vanish=246`. G-READS-ON gives 60 rows, all `one_ret=1` with one base. G-DEF matches the refs.

## 4. Arms and score (2026-11-06)

The section 3 steps were run as written. Exe for the `FLIP` arms and the default gates: the rebuild of `9a991247c`
(`MASHED_NO_DEPLOY=1`, log `D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/build3.log`), staged as `exe_flip/`.
`mashed_re.exe` `6bed629e4f157d7578977d9dac83a091f7faee65be2a14120e098699f9428de4`, `.asi`
`2a9d278395d555f73fe25e3b208a9abb1c4afc10aee59f279375f6131efb9045`. The `cd148e9ec` build moved to `exe_flip_cd148e9ec/`.
The `FIX` arms used `exe_fix/` from section 2 (`724c00f6`). Runs were wrapped in the test slot (outside) and
`mashed_lock.py run` (inside). Replay arms ran 01:54 local, default gates 01:54 to 02:32 local.

`py -3.12 verify/d4dc_0046d4a0_replayorder_20261102/score.py` (also in `SCORE.md`):

| Gate | Arm | Result | Detail |
|---|---|---|---|
| G-RP-OFF | `RP_FIX_OFF` | PASS | G-M3-REPLAY rows=3888 exact=3888 entry_bad=0 calls_bad=0, arms bump=60 collect=60 no-op=3522 vanish=246, MISMATCH lines 0 |
| G-RP-ON | `RP_FIX_ON` | PASS | same |
| G-RP-FLIP | `RP_FLIP` | PASS | same |
| G-RP-FLIP53 (diag) | `RP_FLIP53` | PASS | same, pc=53 |
| G-READS-ON (OFF arm) | `RP_FIX_OFF` | PASS | rows=60 one_ret!=1_or_ptr0=0 distinct(one_ptr-idx*0xd04)=1 00d26940 |
| G-READS-ON (ON arm) | `RP_FIX_ON` | PASS | rows=60 one_ret!=1_or_ptr0=0 distinct(one_ptr-idx*0xd04)=1 00d26940 |
| G-DEF | `H_DEF + B_DEF_K0/K1/K26` | PASS | G-CTRL 8284be46/ba407a16/31fe043e (ref same), B 1d5bf4f9 x3 (ref same) |

Committed extracts: `extracts/` (`*.replay.txt`, `*.pk46.tsv`, `RUNLOG_replay.txt`, `RUNLOG.txt`, 32 KB).

Raw files (not committed), `D:/MashedWT/D4-DC-INC-0046d4a0-REPLAYORDER_data/arms/`:

| file | sha256 |
|---|---|
| `H_DEF.step.csv` | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| `H_DEF.gates.csv` | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| `H_DEF.ptrace.log` | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| `B_DEF_K0.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `B_DEF_K1.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| `B_DEF_K26.step.csv` | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |

Trackers: no confidence change (`0x0046d4a0` stays C3). The optional re-classify note from section 3 step 5 is left to
the lead.
