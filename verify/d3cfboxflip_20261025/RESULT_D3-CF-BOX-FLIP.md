# RESULT D3-CF-BOX-FLIP: MASHED_BOX_PRODUCERS default ON (decision L-11, run 4)

**Verdict: GREEN.** The flip is in. On top of the U9779 default it moves no gate hash: G-CTRL stays
`ef5f2e10 / 57f7ff83 / b4f4b55a` and B stays `c37d4c0c / 0a8f9806 / 7013671d`. D2 passes 3/3 and the 13 tracks give 878 rows,
0 RED, with a table byte-identical to rpct's. The knob is live in every default run (box trace below). With U9779's four knobs
opted out, the flipped exe reproduces the BOX step's G-CTRL re-base, `2b695761 / d7cda332 / 2ec66daf`. So the G-CTRL move that
`RESULT_D3-CF-BOX.md` section 5 predicted is absorbed by the U9779 set. It is not lost.

Branch `orch/D3-CF-BOX-FLIP` from `orch/integration @ 2d56e8312`. Flip commit `1a351c99c`, exe `a03bbe9f` (built with
`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not touched). Private roots `D:\MashedWT\D3-CF-BOX-FLIP_root` (arms) and
`_root13` (tracks). The arms ran inside `mashed_lock.py run --solo` and the tracks under the plain lock.

## 1. The flip (`1a351c99c`)

`BoxProducersEnabled()` (`TrackRenderer.cpp`) returns true unless `MASHED_NO_BOX_PRODUCERS` is set, the same form as U9779's four
getters (`47d3c3519`). The old ON name `MASHED_BOX_PRODUCERS` is no longer read. The opt-out gives the old path: the per-frame
1 / 4 write and dispatcher state 6. `MASHED_BOX_TRACE` is unchanged.

`run_arms.py` here is a copy of `verify/d3cfbox_20261025/run_arms.py` with new stacks: `NBOX` (the opt-out), `NU7` (U9779's four
opt-outs), `NBNU7` (all five) and `BT` (the default plus the box trace).

## 2. Gates on the flipped exe `a03bbe9f` (`data/arms/`, `data/run_arms1.txt`, `data/run_arms2.txt`)

| arm | stack | sha8 (step / gates / ptrace) | against |
|---|---|---|---|
| **G-CTRL `H_DEF-1`, `H_DEF-2` (new default)** | none | **`ef5f2e10 / 57f7ff83 / b4f4b55a`** x2 | = the pre-flip default (U9779 flip). **Unchanged, 0 of 3 hashes move** |
| `H_BT-1` | box trace | `ef5f2e10 / 57f7ff83 / b4f4b55a` | = default, so the trace is inert |
| opt-out `H_NBOX-1` | `NBOX` | `ef5f2e10 / 57f7ff83 / b4f4b55a` | = the pre-flip default |
| cross-check `H_NU7-1` | `NU7` (box ON, U9779 off) | `2b695761 / d7cda332 / 2ec66daf` | = `RESULT_D3-CF-BOX.md` `H_BOX` (box ON on the pre-U9779 default), all 3 hashes |
| **B `B_DEF_K0 / K1 / K26` (new default)** | none | **`c37d4c0c / 0a8f9806 / 7013671d`** | = the pre-flip default. **Unchanged, 0 of 3 arms move** |
| `B_BT_K0` | box trace | `c37d4c0c` | = default |
| opt-out `B_NBOX_K0 / K1 / K26` | `NBOX` | `c37d4c0c / 0a8f9806 / 7013671d` | = the pre-flip default |
| opt-out `B_NBNU7_K0 / K1 / K26` | `NBNU7` | `f9deb9b9 / d3601fcc / 9da9a26a` | = the pre-U9779 default |

Every arm listed exits 0 and self-exits. Expected hashes from this commit on: **G-CTRL `ef5f2e10 / 57f7ff83 / b4f4b55a`,
B `c37d4c0c / 0a8f9806 / 7013671d`**, the same as before the flip.

### 2.1 The knob fires in the default (box trace, `data/arms/*.box.csv`)

| run | start | grid | kill (value 2) | dispatcher 2 -> 3 |
|---|---|---|---|---|
| `B_BT_K0` | 4 | 16 | 6 (frames 220 x3, 573, 583 x2) | 1 (frame 574, slot 3) |
| `H_BT-1` | 4 | 28 | 18 | 4 |

Against the BOX step's `H_BOX` (box ON, U9779 off: 18 kills, 18 latches to 3), the U9779 default keeps all 18 kills and latches
4 of them. The dispatcher gets `aib_game_sub_mode()` under the knob, and the U9779 set (`ROUND_END7`, default ON) returns 7 after
a round end, which gates the dispatcher (BOX RESULT section 4, P3a: "sub-state 7 gates the dispatcher"). The 4 latches that remain
change no hashed field in G-CTRL. [UNCERTAIN] which field a latch would change: the step / gates / ptrace dumps do not record the
box state or the armed power-up of FUN_0045bac0, so this run cannot say whether those 4 cars held an armed power-up.

## 3. D2 x3, solo (`score_d2_v2.py port`, `data/score_d2_flip.txt`)

| arm | motion_diag lines | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| `D2_DEF_1` | 1633 | 0.1900 in | 0.2550 in | 1965.67 in | 398/400 |
| `D2_DEF_2` | 1637 | 0.1900 in | 0.2548 in | 1970.58 in | 398/400 |
| `D2_DEF_3` | 1636 | 0.1900 in | 0.2549 in | 1970.58 in | 398/400 |

**PASS 3/3.** U9779 flip default: 0.1900 x3, 0.2548 / 0.2550 / 0.2550, 1970.58 / 1965.67 / 1965.67. The values come from the same
set, in a different order.

## 4. 13 tracks (`run13.py` + `verify/rres_20261016/cites_rres.json`, `data/tracks_flip_score.txt`, `data/t13_flip/`)

39 frames, 878 rows, 0 RED, INSTRUMENT GATES PASS. All 13 tracks were OK on attempt 1. `tracks_flip_score.txt` is
**byte-identical** to `verify/rpct_20261022/tracks_flip_score.txt` (`diff` empty), so no row changed. BMPs are left out of the
commit.

## 5. Re-runs (rerun rule; `data/arms/RUNLOG.txt` keeps every attempt)

The first batch (`run_arms1.txt`, 10:54-11:15) overlapped another load on the machine. CPU load was 100% from a game started at
10:43 (`YARG.exe`, not a MASHED process) and another session's 2-job Python sweep. Neither was touched. Arms in that batch that
are invalid by the rerun rule:

- `D2_DEF_1` 465 motion_diag lines and `D2_DEF_2` no motion_diag. Both under 1600, so both were re-run (`run_arms2.txt`: 1633 / 1637).
  `D2_DEF_3` had 1636 lines and counts as run.
- Six B arms exited 0 after 7-14 s, before 1200 frames: `B_DEF_K26`, `B_NBOX_K1`, `B_NBOX_K26`, `B_NBNU7_K0/K1/K26` and
  `B_BT_K0`. Their logs stop at different points (`B_NBNU7_K0` before the race starts), with no crash record. That is an outside
  exit, not a run outcome: DirectInput reads global key state (memory `nav-demo-bypasses-focus-gate`). All seven were re-run in
  batch 2, and each run is a full-length dump with the hash shown in section 2.

No piz-collision crash (U-9677) occurred. All H arms ran their full 14400 frames on the first attempt.

## 6. Data layout

`data/arms/`: every run's `.log` / `.ptrace.log` / `.motion_diag.log` / `.a5g.log`, `*.box.csv`, all B `step.csv`, the D2
motion_diag, and `H_NU7-1` step + gates. The other H `step.csv` / `gates.csv` (`ef5f2e10` / `57f7ff83`) are byte-identical by
hash to the committed `verify/u9779_20261024/data/arms_flip/H_DEF-1.*`, so they are left out (the ptrace logs are in).
`data/run_arms*.txt` (batch logs with the solo lock lines), `data/t13_flip.out`, `data/t13_flip/` (scores, gates, per-row diffs).

## 7. Owed (not done here)

- The merge train writes the status line. The `default-ctrl-hashes-after-b2-flip` memory and `re/NEXT_SESSION.md` need no hash
  change: G-CTRL and B are the same after this flip.
- BOX slice 2 (FUN_00424eb0's respawn writes `0x0042512f` / `0x004251c8`, FUN_0040be50 case 6 `0x0040c578`) is still unported,
  as in `RESULT_D3-CF-BOX.md` section 6.
- Tracker: no hooks.csv move here. Any C-level change goes through `re-classify`.
