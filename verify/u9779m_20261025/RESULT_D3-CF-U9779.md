# RESULT D3-CF-U9779 run 4: hand-merge onto orch/integration, re-gated. GREEN (2026-10-25)

Branch `orch/D3-CF-U9779-M`, merge commit `0f6435991` = `orch/integration` @ `70fac88db` (parent 1) + `orch/D3-CF-U9779` @
`54a89e585` (parent 2). Ancestry checked with `git merge-base`: base `30913130b` (orch: ledger snapshot after Phase 0 dry run),
so the step branch is 7 commits ahead of the base and `orch/integration` is 97 commits ahead (D3-CF-BOX, D4-DC-82a0, D4-DC-8ad0,
D1-SHADOWS, D3-CF-U9780, HB-*, D3-CF-U9779-RECON, the ref-swap commits).

## 1. Merge

| file | both sides changed | resolution |
|---|---|---|
| `re/frida/scenario_launch.py` | yes, 2 conflict hunks | kept both. Hunk 1 (argparse): `--box-probe` (D3-CF-BOX) and `--r7phys-probe` (D3-CF-U9779). Hunk 2 (arm calls): `E.box_probe_arm()` and `E.r7_phys_probe_arm(args.statediff_car)`. The JS bodies (`boxProbeArm`, `r7PhysProbeArm` / `Drain` / `Stats`) and their rpc exports auto-merged. `py_compile` OK |
| `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp` | yes | auto-merged, no conflict. The flip's four default-ON getters (`ElimResolveEnabled`, `RoundEnd7Enabled`, `RaceTailLateEnabled`, `RoundEnd7PhysActive`) are present as on `54a89e585` |

No other file was touched by both sides.

## 2. Build

`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`: Build OK, `mashed_re.exe` sha8 `b1f0f7cb`. `original\mashed_re_dev.asi` was not
redeployed.

## 3. Gates (private root `D:\MashedWT\D3-CF-U9779-M_root`, runner `verify/u9779_20261024/run_arms.py`, `mashed_lock.py`)

| gate | measured on `b1f0f7cb` | expected (D3-CF-U9779 section 7.3, flipped default) | |
|---|---|---|---|
| G-CTRL `H_DEF-1` | `ef5f2e10 / 57f7ff83 / b4f4b55a` (15941 / 53273 / 26636 lines) | `ef5f2e10 / 57f7ff83 / b4f4b55a` | equal |
| G-CTRL `H_DEF-2` | `ef5f2e10 / 57f7ff83 / b4f4b55a` | same | equal |
| B `B_DEF_K0 / K1 / K26` | `c37d4c0c / 0a8f9806 / 7013671d` (1561 / 2059 / 1541 lines) | `c37d4c0c / 0a8f9806 / 7013671d` | equal |
| D2 `D2_DEF_1/2/3`, solo window, `verify/d2rebase_20261019/score_d2_v2.py port` | PASS 3/3: slip1500 0.1900 x3, slip2000 0.2549 / 0.2548 / 0.2549, drvmed 1970.58 x3, H1 398/400 x3 | in band (step run 3: 0.1900 / 0.2548-0.2550 / 1965.67-1970.58) | in band |
| 13 tracks, `verify/rsky_20261016/run13.py` + `verify/rres_20261016/cites_rres.json` | 39 frames, 878 rows, 0 RED, INSTRUMENT GATES PASS, every track OK after 1 attempt | `verify/rpct_20261022/tracks_flip_score.txt` | score table identical (blank lines and CR ignored) |

D2 validity (L0 rule): `SOLO HELD` / `SOLO CLOSED` in `data/run_d2.txt`. motion_diag lines 1636 / 1637 / 1636, all at least 1600,
none re-run. `D2_DEF_2`'s motion_diag sha8 `0ecd557e` equals step run 3's `D2_DEF_1`. No piz-collision crash (U-9677) in any run, so
nothing was re-run. No other MASHED ran during the window: this was the only worker running it.

G-CTRL and B ran under the plain lock (`data/run_gates.txt`), the 13 tracks under the plain lock with private root
`D:\MashedWT\D3-CF-U9779-M_root13` (`data/t13.out`).

## 4. Data (`verify/u9779m_20261025/data/`)

`arms/` (H / B / D2 step dumps, gates, ptrace, motion_diag, per-arm logs, `RUNLOG.txt`), `run_gates.txt`, `run_d2.txt`,
`score_d2_M.txt`, `t13.out`, `tracks_M_score.txt`, `t13/` (scores, gates, per-row diffs and draw lists, cam / librw dumps, BMPs left
out).

## 5. Not done here

- No merge into `orch/integration`, `merge_train.json` untouched, no push. The lead enqueues `orch/D3-CF-U9779-M` @ the branch head.
- The tracker and NEXT_SESSION items in D3-CF-U9779 section 7.5 are still owed.
