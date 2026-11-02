# RESULT D4-DC-INC-0046d4a0-REPLAYORDER

**Verdict: HANDOFF.** The static and build work is done and committed: the order fix, the PREREG (committed alone), the
flip, the runner and the scorer. No arm has run yet. Every `scripts/diag.py probe-render` returned BLOCKED, D3DERR
`0x8876086A` (display asleep): at 09:4x, 09:51, 09:56 and 10:01 local, 2026-11-02. That is not a finding. The arms
below are the only remaining work.

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
