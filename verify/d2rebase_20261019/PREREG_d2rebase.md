# PREREG d2rebase: the D2 re-baseline (2026-10-19)

User decision 2026-10-19 (relayed by the orchestrator). Branch `work/d2rebase` from `9145c9c06`. Model claude-opus-5-5,
account2, host LIVING-PC. Written and committed before any port log is scored with the new recipe or band. No default changes.

## 1. Recipe

**Reductions** = `re/analysis/D2_REOPEN_2026-09-29.md` section 1, unchanged by its amendments 13.3 / 16.7 ("the reducers do
not change"): `a8_slip_axis.py --max-lines 1080` (`slip vs fwd`, 1500-2000 / 2000-2600) and `a8_momentum.py --max-lines 1080
--orig-steer-min 33.0 --port-steer-min 0.9` (`driving-only horiz median`). Scorer: new `verify/d2rebase_20261019/score_d2_v2.py`
(`port` mode = the above, `port --legacy` = `score_d2.py`'s whole-log reduction, `orig` mode = the original path).
`verify/d2settle_20261016/score_d2.py` is kept unchanged for provenance. The original side is the whole capture: all eight
originals score identically with and without `--max-lines` (`orig8_scores.txt`), and the four `d81a8df6` runs reproduce
that register's table exactly (0.1913 / 0.2498 / 1940.59, 0.1911 / 0.2501 / 1957.90, 0.1937 / 0.2498 / 1937.89 x2).

**Port run arm: PENDING a user decision** (asked 2026-10-19 before any port run). The instruction names `TRACK_SEL=0`,
`STEER_HOLD_AFTER=4` (the register's section 1). The same register amends it: 13.3 `MASHED_TRACK_SEL=12` (port `kAreas[0]` is
Arctic, `kAreas[12]` Training; the original races Training), 16.7 `MASHED_STEER_HOLD_AFTER=0` (full lock from sim step 0, the
original's `E.drive(1,+1)`). Option A (the register as amended): `a8_run_port.py <dir> 90 MASHED_MEASURE_SOLO=1
MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0`. Option B (literal section 1): `a8_run_port.py <dir> 90
MASHED_MEASURE_SOLO=1`. No port arm runs until the choice is made; this section gets an amendment naming it.

## 2. Band

All eight original solo captures (argv `--statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38
--poke-ctrl-slots`): steerR, elim, solo3, solo4 (MARIANO-PC) and u9763's r1..r4 (LIVING-PC). Rule unchanged from
`d81a8df6` section 3a: `mean ± max(3h, 2% of mean)` (`band.json`):

| metric | mean | min .. max | h | 3h | 2% | width set by | band |
|---|---:|---|---:|---:|---:|---|---|
| slip1500 | 0.194912 | 0.1911 .. 0.1984 | 0.00365 | 0.01095 | 0.003898 | 3h | 0.18396 .. 0.20586 |
| slip2000 | 0.250563 | 0.2498 .. 0.2517 | 0.00095 | 0.00285 | 0.005011 | 2% | 0.24555 .. 0.25557 |
| driving-median | 1945.5425 | 1937.06 .. 1971.67 | 17.305 | 51.915 | 38.911 | 3h | 1893.63 .. 1997.46 |

H1 unchanged (>= 200 of the 400 post-trough frames >= 100 and median >= 900; every original run: 398/400, median 1333.9).
Determinism precondition unchanged (section 3b of the register, with the new `h`): >= 3 port runs agreeing within `h`.

## 3. Arms

Three port runs each, each alone, on the chosen recipe: the current default; the pair `MASHED_A6A_LUT` + `MASHED_A6A_F32`;
the triple (pair + `MASHED_A5_SUSP_LUT`). Historic D2 logs (all from the 12 / 0 arm) rescored with both reductions where they
are on disk: d2close, d2ref, fwdsse, u9760, u9761, u9762.

## Amendment 1 (2026-10-19, the user's recipe choice, before any port run)

**Option A, the register as amended** (user decision via the orchestrator: "score the port with the same recipe as the
original"; the instruction's section-1 values were quoted by mistake): `MASHED_TRACK_SEL=12` (section 13.3, Training against
Training), `MASHED_STEER_HOLD_AFTER=0` (section 16.7, full lock from sim step 0), `--max-lines 1080`, the same reducers. The
port run is the existing D2 arm (`run_arms.py` `D2_*`: `a8_run_port.py`'s recipe with `MASHED_TRACK_SEL=12
MASHED_STEER_HOLD_AFTER=0 MASHED_MEASURE_SOLO=1`, 90 s, each run alone). **The only change from the current scorer
(`verify/d2settle_20261016/score_d2.py`) is restoring `--max-lines 1080`** on the port reduction (`score_d2_v2.py port`), plus
the 8-run band in section 2.
