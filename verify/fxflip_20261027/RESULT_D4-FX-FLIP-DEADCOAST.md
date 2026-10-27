# RESULT D4-FX-FLIP-DEADCOAST: `MASHED_DEAD_COAST` + `MASHED_KILL_DAMAGE` default ON (2026-10-27)

**Verdict: GREEN, flip ready.** L-34 answered (a): both knobs flip to default ON together. Evidence for the flip is in
`verify/fxinputs_20261026/RESULT_D4-FX-INPUTS.md` (attempt 3). On the flipped exe the default gates give exactly the
expected new refs, the B arms are unchanged, D2 is 3/3 in band, and the 13-track table is identical to the reference.
`re/orchestrator/merge_train.json` is not edited here (L0 changes the refs).

## 1. The change

`mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp`:
- `VehiclePhysics_DeadCoastEnabled()`: ON unless `MASHED_NO_DEAD_COAST=1`.
- `KillDamageEnabled()`: ON unless `MASHED_NO_KILL_DAMAGE=1`. It is still only read inside `MarkKilled` / `ClearKilled`,
  so it only acts while DEAD_COAST is on.
- Comments updated in the .cpp and `VehiclePhysicsRun.h`. No other code change. Branch base: orch/integration
  a92e3fd2f, which contains D4-FX-INPUTS 361e7fc5f (`git merge-base --is-ancestor`).

## 2. Gates and full set (exe 75ba2476, `merge_train.json` runner and scorers, driver `flip_driver.py`)

| check | result | expected |
|---|---|---|
| G-CTRL H_DEF step / gates / ptrace | 5943c5f1 / 364350ae / 52052890 | 5943c5f1 / 364350ae / 52052890 |
| B B_DEF_K0 / K1 / K26 | 3a03670c / e85ea6fb / 53778664 | unchanged |
| (b) `vsum.py` | 3/3 arms 3/3 (A / B / C PASS) | 3/3 |
| (e) `ai_speed_env.py --check` | 3/3 PASS, launch 1425.7 / 2052.5 / 2055.0 | same launches |
| D2 x3 solo, `score_d2_v2.py port` | 3/3 in: slip1500 0.1942, slip2000 0.2492, drvmed 1896.74 | 3/3 |
| 13 tracks, `run13.py` vs `verify/colllut_20261026/tracks_score.txt` | 39 rows identical, INSTRUMENT GATES: PASS | identical |

D2 arms: the first solo pass gave motion_diag 1590 / 1368 / 1496 lines, under the 1600 floor. One solo re-run per arm
(the merge_train HB-D2RERUN rule) gave 1630 / 1632 / 1628. The score reads the re-run logs.

The D2 numbers equal the current default (0.1942 / 0.2492 / 1896.74). The G-CTRL gates/ptrace files move because the
recipe kills the player and two AI cars at frame 220 and the killed cars now keep stepping.

Every game batch ran inside `proyectos_test_slot.py` and `mashed_lock.py run` (`--solo` for D2, `solo-begin` /
`solo-end` around the 13 tracks). No build ran during the D2 or 13-track windows. Only spawned PIDs were touched.

## 3. Files

Committed here: `tracks_score.txt`, `score_d2.txt`, `vsum_b.txt`, `e_check.txt`, `flip_driver.py`.

Raw data (D:/MashedWT/D4-FX-FLIP-DEADCOAST_data, not committed; also `tracks/` 109 MB, `driver.txt`, `build.log`), SHA-256:

```
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d arms/B_DEF_K26.step.csv
acab2155e8831f83b223b08e59e1e65dafad8c03f454d8cf22c6192b093bbd07 arms/D2_DEF_1.motion_diag.log
fb927e02ffe22d6a60d3ff30bef01032ff2dd814b62c1a5495c17c0123c6f314 arms/D2_DEF_2.motion_diag.log
ae9532cc93bea98d335c0e31aac5df66dd39a4b3e87d3b4b849fc00f7f9fcfef arms/D2_DEF_3.motion_diag.log
75ba247657c382c4ce98f5876df3ad5b46b461b1e9bbc1fd8753ee9abdd33d50 mashedmod/build/mashed_re.exe
```

## 4. For L0

New refs for `merge_train.json`: `gctrl.ref` = 5943c5f1 / 364350ae / 52052890, `b.ref` unchanged. Opt-outs:
`MASHED_NO_DEAD_COAST=1` (both off) and `MASHED_NO_KILL_DAMAGE=1` (damage write off).
