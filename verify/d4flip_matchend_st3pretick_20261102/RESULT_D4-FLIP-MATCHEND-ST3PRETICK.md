# RESULT D4-FLIP-MATCHEND-ST3PRETICK (GREEN)

Verdict: **GREEN** (2026-11-06). The flip is committed (ec8c2c340) and all four default-gate arms on the flipped exe,
rebuilt on top of `orch/integration` @ d351e7dcb (merge 6a788de6b), reproduce the current refs in
`re/orchestrator/merge_train.json` (G-CTRL 8284be46 / ba407a16 / 31fe043e, B 1d5bf4f9 x3). No `merge_train.json` change.
SOURCE change (two knob defaults in `TrackRenderer.cpp`), default gate hashes unchanged.

## Gate arms, 2026-11-06 (resume after the 2026-11-02 HANDOFF)

Exe: `mashedmod/build/mashed_re.exe` built from the worktree with `MASHED_NO_DEPLOY=1` after merging `orch/integration`,
SHA-256 `f47b16de1e7bd3fded4cb40a3e91bcd0beec990ade601106dcda5d8c50395bda`. Render probe first
(`scripts/diag.py probe-render` against the private root): `RENDER: OK, last CreateDevice hr=0x00000000`.
Run: test slot outside, `mashed_lock.py run --label D4-FLIP-MATCHEND-ST3PRETICK` inside, `bash verify/d4flip_matchend_st3pretick_20261102/run_all.sh`.

| Arm | Expected (merge_train.json refs) | Got | exit / secs |
|---|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | 0 / 245 |
| B_DEF_K0 | 1d5bf4f9 | 1d5bf4f9 | 0 / 24 |
| B_DEF_K1 | 1d5bf4f9 | 1d5bf4f9 | 0 / 22 |
| B_DEF_K26 | 1d5bf4f9 | 1d5bf4f9 | 0 / 22 |

Raw files (not committed), `D:/MashedWT/D4-FLIP-MATCHEND-ST3PRETICK_data/arms/`, SHA-256:

```
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  H_DEF.step.csv
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  H_DEF.gates.csv
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  H_DEF.ptrace.log
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  B_DEF_K0.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  B_DEF_K1.step.csv
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  B_DEF_K26.step.csv
```

All spawned PIDs (804, 6852, 15764, 17100) self-exited. `orch/integration` has since moved to f565f1946 (MATCH-T13-SEED, D4-DC-INC-0046d4a0-REPLAYORDER, mashedmod changes), so the combined tree is not gated here. The merge train should run this row alone in its own `-Full`
flip pass, as decided.

## History: the 2026-11-02 HANDOFF

The first attempt (below) built the flip but could not boot: every `mashed_re.exe` boot on the machine failed D3D9
CreateDevice with `hr=0x8876086A` (D3DERR_NOTAVAILABLE, D3D9-NOTAVAIL) because the display was asleep. Not a finding.

## What changed (commit ec8c2c340 on orch/D4-FLIP-MATCHEND-ST3PRETICK)

Decisions: `log/orch/DECISIONS.md` L-80 (a) and L-83 (a), answered by Mariano 2026-11-02. Base:
`orch/integration` @ 6a997bfa3, which contains both knob rows (D4-DC-77b0-MATCHEND-SUBSTATE df9d1839d,
D4-DC-77b0-ST3-PRETICK 30ec68b69).

`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`:

| Knob | Before | After | Opt-out |
|---|---|---|---|
| `MatchEndSubstateEnabled()` (FUN_00410510 match-end stores, case 0xb, live FUN_00443080) | ON only with `MASHED_MATCHEND_SUBSTATE=1` | default ON | `MASHED_NO_MATCHEND_SUBSTATE=1` |
| `s_st3Pretick` (FUN_00418860 once per case-3 tick) | ON only with `MASHED_ST3_PRETICK=1` | default ON | `MASHED_NO_ST3_PRETICK=1` |

The old names are still accepted and are no-ops (same pattern as `MASHED_NO_HEADING_ASSIST`). No other source
line changed. Powerup teardown, case-9 teardown and the ctrl shadow copy are not in this row (separate follow-ups).
No `merge_train.json` change: L-80 and L-83 both record the gates unchanged with the knob unset and set
(verify/d4dc77b0matchend_20261101/RESULT_D4-DC-77b0-MATCHEND-SUBSTATE.md,
verify/d4dc77b0st3pretick_20261101/RESULT_D4-DC-77b0-ST3-PRETICK.md), so the expected hashes are the current refs.

Build: `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"` from the worktree, Build OK, `original\mashed_re_dev.asi`
not deployed. `mashedmod/build/mashed_re.exe` SHA-256
`45cece5350283d442d3528525b1acd1cd2d0f366979a74c1dad84384f4c3ce45`.

## 2026-11-02 state: the four default-gate arms (since run, see the table above)

| Arm | Expected (merge_train.json refs) | Got |
|---|---|---|
| H_DEF (step / gates / ptrace) | 8284be46 / ba407a16 / 31fe043e | not run: D3D9-NOTAVAIL |
| B_DEF_K0 | 1d5bf4f9 | not run: D3D9-NOTAVAIL |
| B_DEF_K1 | 1d5bf4f9 | not run: D3D9-NOTAVAIL |
| B_DEF_K26 | 1d5bf4f9 | not run: D3D9-NOTAVAIL |

Exact command, from the worktree root `C:\Users\maria\Desktop\Proyectos\Mashed\.worktrees\D4-FLIP-MATCHEND-ST3PRETICK`
(Bash), after `py -3.12 scripts/diag.py probe-render` (or a probe against the private root) shows a device:

```
test-slot bash -c 'py -3.12 ../orch-integration/re/orchestrator/mashed_lock.py run --label D4-FLIP-MATCHEND-ST3PRETICK -- bash verify/d4flip_matchend_st3pretick_20261102/run_all.sh'
```

`run_all.sh` runs `verify/d4flip_matchend_st3pretick_20261102/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`
(a copy of `verify/savelink_20261023/run_arms.py`, unchanged apart from the docstring) with
`A4_ROOT=D:\MashedWT\D4-FLIP-MATCHEND-ST3PRETICK_root` (robocopy of `original\`, staged, with `videocfg.bin`),
`A4_OUT=D:/MashedWT/D4-FLIP-MATCHEND-ST3PRETICK_data/arms`, `A4_EXE=<worktree>/mashedmod/build/mashed_re.exe`.
Note: `diag.py probe-render` run from the worktree reports UNKNOWN (the worktree has no game data, so no log);
probe with `MASHED_ROOT` = the private root and `cwd` = that root instead.

If any hash differs from the refs: STOP and report RED / DECISION (the decided flip assumed gate neutrality).

## What happened (run log)

1. 09:20, first arm pass under the machine lock: all four arms `exit=3 self_exited=True` in 1-12 s. The root's
   `mashed_re.log` shows all 8 CreateDevice attempts failing `hr=0x8876086A`. That data folder was moved to
   `D:/MashedWT/D4-FLIP-MATCHEND-ST3PRETICK_data_sandboxfail` (no hash files, logs only).
2. Cross-check: other sessions' roots logged the same failure from 09:22 on (MATCH-PORTSTATE-RECORDS,
   HB-GATE-FLAKE-GAMEFLOW, D4-DC-77b0-RUNTIME-PARKED, MATCH-T13-SEED-WRITER, D4-DC-INC-SQRT-FOLD), while the merge
   train's `mtrain_root` got a device at 09:17:30. Machine-wide, not this exe. L0 confirmed the display asleep since
   about 09:20.
3. 09:53-11:23: a 12 s render probe every 2 min against the private root, 45 probes, every one `8876086A`
   (one at 10:23 wrote no CreateDevice line). Stopped on the lead's instruction.
