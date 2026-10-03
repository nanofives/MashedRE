# RESULT — STEP 1: the player-elimination confound

Pre-registration: `PREREG_STEP1.md`, committed **unrun** at `e8d038ab`. Branch
`race/first-frame-parity`.

## Verdict in one line

**The confound's share of the 2026-10-03 arm A vs arm B delta is ZERO, measured not argued:
with the elimination suppressed, all 2640 scored byte-slots and all 660 speed/position slots
are bit-identical to the default arm. Two things the confound's original filing got wrong
are corrected below. The elimination TIME is not an independent variable at all — it is a
readout of the over-speed, which is STEP 2's target.**

## EVERY PRE-REGISTERED GATE PASSED

| gate | outcome |
|---|---|
| **G-KA1** (probe liveness) | **PASS. 200/200** on both original runs, all four slots, direct read `*(i32*)(0x008815a4 + i*0xd04)` vs `FUN_0046c7b0(i)`. `ka1bad = None`. |
| **G-LIVE** | **PASS.** `o_e1` 220 SegmentCheck rows / 2062 AI ticks; `o_e2` 220 / 2008. |
| **G-XFER** (does the probe perturb the reference?) | **PASS.** `o_e1` and `o_e2` reproduce `verify/d3_modes37_20261002/o1.msd.aistep.csv` on **every printed digit** of `ai_ctrl_window.py --check` and `ai_speed_env.py --check`, all three cars. Only `start_frame` differs (900 / 923 / 866 — a different wall frame for the same race moment). |
| **G-DET-O** | **PASS.** `o_e1` and `o_e2` give the same Q1 and Q2 answers and the same scorer digits. |
| **G-CTL-KNOB** (2 independent channels) | **PASS on both.** (1) `MASHED_PLAYERTRACE` `alive=` is **1 on every line** of `ec1`/`ec2`/`ec3` with **no transition**, and goes **1 → 0 at `rt = 1.8667 s`** in `ea1`/`ea2`/`ea3`. (2) arm A's step dump stops producing **car 2** rows at `clk = 64450` (`AiStepDump` skips `!g_aib.alive[v]`, `TrackRenderer.cpp:3777`); arm C carries car 2 to `clk` 165950 / 98450 / 161550, i.e. the full run. |
| **G-CTL-DET** | **PASS.** `ea1`/`ea2`/`ea3`/`ec1`/`ec2`/`ec3` — all **six** — agree to **every printed digit** on both scorers, all three cars. No spread to report. |
| **build control** | **PASS.** `ea1` reproduces `verify/d3_noboost_20261003/a1` with **0 differing slots** over 7 fields x 220 calls x 3 cars. The knob compiled in but unset does not move the default arm. |

**No gate was replaced in this step.**

## Q1 — is the PLAYER eliminated on the ORIGINAL? Measured, live, twice.

**NO — not during the scored window. It is eliminated on the FIRST FRAME AFTER IT, in the
same call that ends the race.**

The original's scored window (`ai_ctrl_window.py`: car 1's calls `[i0, i0+220)`, `i0` = the
first `c4 != 0`) is `o_e1` calls **786..1005**, frames **900..1119**, `clk_0ff4`
**44900..55850**. The alive record covers frames **901..1120** — **219 of its 220 rows lie
inside the window, and `a0 == 1` on every one of them.** The transition happens on row 219,
frame **1120**, `clk 55900`, **one frame past the window's last scored call**.

| `seq` | frame | clk | `zoom` (`0x00898980`) | `ret` | alive in → out |
|---:|---:|---:|---:|---:|---|
| 0 | 901 | 44950 | 2.7395 | 0 | `1/1/1/1` → `1/1/1/1` |
| 110 | 1011 | 50450 | 5.7533 | 0 | `1/1/1/1` → `1/1/1/1` |
| 218 | 1119 | 55850 | 9.9880 | 0 | `1/1/1/1` → `1/1/1/1` |
| **219** | **1120** | **55900** | **10.0 exactly** | **1** | `1/1/1/1` → **`0/0/0/1`** |

`o_e2` reproduces this exactly: first row frame 924, transition at `seq` 219, frame 1143,
`clk 57050`, `1/1/1/1 → 0/0/0/1`, `ret = 1`.

**The mechanism is visible in the table and it is the one the port transcribes.** The
elimination gate is `FUN_00442df0() == 10.0` (`0x00410ee3`, ported as
`RaceCamera::EliminationCheck`'s `if (required_zoom_ != 10.f) return -1;`,
`RaceCamera.cpp:501`). `0x00898980` climbs **monotonically 2.74 → 10.0** across the whole
window and **reaches 10.0 exactly once, on the last call**. At that moment
`race_pct = [8.16, 16.72, 18.98, 20.09]` — the player is last, so it is the straggler and
the victim. `SegmentCheck` returns **1** in the same call and the race ends.

**Rule 0 is in force** (`rule` column = 0 on all 220 rows, `participants` = 4,
`substate` = 6 throughout), which is `SegmentCheck`'s `default:` arm, i.e. the elimination
block is enabled — so the absence of an earlier elimination is **not** an absence of the
mechanism.

## Q2 — does the AI tick run for slot 0 on the ORIGINAL?

**NO. `FUN_00418560` (`AiVehicleStep`) is called 0 times for slot 0.**

| run | `vstep[0..3]` (`0x00418560` entries) | AI ticks (`0x00418860`) | aistep rows per `v` |
|---|---|---:|---|
| `o_e1` | **`[0, 1230, 1230, 2062]`** | 2062 | `v1` 1006, `v2` 1006, `v3` 1842 |
| `o_e2` | **`[0, 1253, 1253, 2008]`** | 2008 | `v1` 1029, `v2` 1029, `v3` 1784 |

This is the live confirmation of the correction `PREREG_STEP1.md` registered **before the
run**: the kickoff and `verify/d3_noboost_20261003/RESULT.md` both state that
`AiStandalone.cpp:1708`'s loop *"runs `VehicleStep(0)` in one arm and not the other"*. It
runs it in **neither**, and the original does not run it either. `aib_veh_type(0)` returns
**0** (`TrackRenderer.cpp:86`) and `aib_ai_target_enable()` returns **0** (`:96`), so for
`v == 0` the test at `:1709` is false and `car_alive(0)` is never even called. **That path
was never a channel for the confound.**

## Q3 — what the PORT does, and the SECOND correction

**Both port arms eliminate the player. The 2026-10-03 filing that arm B leaves it alive is a
line-alignment artefact and is corrected here.**

| arm | window speed (cars 1/2/3) | player eliminated at | as a window call index |
|---|---|---|---:|
| **ORIGINAL** | 2419.5 / 2397.0 / 2617.6 | `clk 55900`, frame 1120 | **220** (one past the last scored call) |
| **port arm A** (boost ON) | 3421.7 / 3346.2 / 3495.9 (+41/+40/+34 %) | `rt = 1.8667 s` | **112** |
| **port arm B** (`MASHED_NO_START_BOOST=1`) | 3169.4 / 2904.7 / 3011.0 (+31/+21/+15 %) | `rt = 2.4333 s` | **146** |

Arm B's transition is at `bw_player_trace.log` **line 292**, not line 223; the two traces
differ at line 223 because the flip happens at a *different line in each arm*, not because
arm B never flips. `alive=` goes `1 → 0` exactly once in each of `aw`, `bw`, `ea1`, `ea2`,
`ea3`, and never in `ec1`, `ec2`, `ec3`.

**Read the three rows together: the elimination call index is monotone in the window speed,
and the gate is a camera-zoom saturation that the cars reach by separating.** The port's
cars separate sooner because they are faster. So the elimination time is a **readout of the
over-speed, not an independent variable** — which is exactly why suppressing it changes
nothing downstream, and why STEP 2's target is the right one. (Stated as the reading of
three points, not as a fit. `[UNCERTAIN]`: no functional form is claimed.)

## STEP 1B — the static reach test (declared non-measurement)

Every reader of `race_[0].alive` / `g_aib.alive[0]` under `mashedmod/src`, and whether it
can reach the AI control bytes or AI-car physics inside the 220-call window:

| `file:line` | what it does | reaches the window's AI bytes / AI physics? |
|---|---|---|
| `TrackRenderer.cpp:3722` | `g_aib.alive[0] = (round_mode_ && !race_[0].alive) ? 0 : 1` | **No.** Its only reader is `aib_alive(0)` → `s_host.car_alive(0)`, and the AI loop's `:1709` type gate means that call never happens. |
| `TrackRenderer.cpp:2913` | the player's auto-drive branch: `accel = 0.82f` when alive, `0.f` when not | **No, measured.** The player is stationary in every arm (`sp = 0.6380385160446167` on every trace line) and arm A vs arm C player physics is bit-identical — see the D2 WATCH below. |
| `TrackRenderer.cpp:3209` | `MASHED_PLAYERTRACE` print | diagnostic only |
| `TrackRenderer.cpp:3777` | `AiStepDump`'s `if (!g_aib.alive[v]) continue` | **dump only**, and the loop is `v = 1..3` |
| `:4437`, `:4457`, `:4581`, `:4622`, `:4677`, `:4699`, `:4728` | survivor scan, `rc.alive[i]`, `round_winner_` | round-end bookkeeping; its only observable in these captures is **when car 2 stops being dumped** (`clk 64450`), **5.9x past the window's end at `clk 11000`** |
| `:3272`, `:3619`, `:3849`, `:3381` | AI-car gating (`race_[i+1]`), powerup `pu_ai_`, legacy `AiOptionBStep` | slots 1..3 only; `:3381` is the `MASHED_REAL_PHYSICS=0` branch the default build does not take |

This is **evidence of a mechanism and nothing more**; the share below is set by STEP 1C.

## STEP 1C — THE LIVE CONTROL. The share is ZERO.

Knob: `MASHED_NO_ELIM=1`, default OFF, implemented at `TrackRenderer.cpp:85-90` (`NoElim()`)
and read at exactly two call sites, the two elimination blocks
(`TrackRenderer.cpp:4667` and `:4728`). Unset, every caller is the old condition `&& true`.

**Decision rule as registered: the four scored bytes, 220 calls, three cars.**

| car | `c0` | `c1` | `c4` | `c5` |
|---|---:|---:|---:|---:|
| 1 | 0 / 220 | 0 / 220 | 0 / 220 | 0 / 220 |
| 2 | 0 / 220 | 0 / 220 | 0 / 220 | 0 / 220 |
| 3 | 0 / 220 | 0 / 220 | 0 / 220 | 0 / 220 |

**TOTAL differing byte-slots arm C vs arm A: 0 of 2640.** And on the channels the rule did
not require: `rec_9e4`, `own_x`, `own_z` — **0 of 660**.

(b) and (e) are therefore identical: **all six arms** (`ea1-3`, `ec1-3`) print the same
digits as `verify/d3_noboost_20261003/a1`, including (b) 13 failing bands and (e) 6/6 PASS
(`launch` 1426.4 / 2053.0 / 2055.2, `ft_median_m0` 2550.6 / 2053.0 / 2278.2,
n = 100 / 23 / 39, `regime0 = 1`, `flag = [0]`, median call index 109.5).

**→ The confound explains 0 % of the 2026-10-03 arm A vs arm B delta.** Every number in
`verify/d3_noboost_20261003/RESULT.md` stands unchanged, and U-9185's evidence-missing item
(a) is **RESOLVED**: the arm A vs arm B contrast *was* single-cause after all. The seed is
the whole of it.

## Collateral review — run AFTER the verdict, declared as such

`re/tools/statediff/collateral.py`, `--mode paired`, `--anchor c4:ge:1`, scope file
`scope.txt` (copied unchanged from `verify/d3_noboost_20261003`). Floors from the same-arm
repeats (`ea2` for A, `ec2` for C). **The floors are 0** — the repeats are bit-identical on
every column — so "within the noise floor on every aligned frame" means *bit-identical*, and
every nonzero difference is above floor by construction.

Scope is the **whole run**, not just the window: 3307 / 1289 / 3307 aligned frames.

| car | fields diverging | **OUTSIDE-SCOPE rows that moved** | first frame |
|---|---:|---|---:|
| 1 | **1 of 35** | `seq` (med\|A−C\| 364) | 1290 |
| 2 | **0 of 35** | — | — |
| 3 | **1 of 35** | `seq` (med\|A−C\| 365) | 1289 |

`seq` is the dump's own row counter, not game state: in arm C car 2 keeps producing rows, so
the interleaved numbering shifts by one row per frame from the frame where arm A's car 2
stops. **No downstream-scope field moved on any car, anywhere in the run.**

## D2 WATCH (D-11071) — NO REOPEN CANDIDATE

`MASHED_NO_ELIM` touches none of D-11071's five triggers: `+0x4a4`, the contact collector /
`FUN_00538c80`, grip-clamp #6, the substep/chunk loop and `ReassertContacts` are neither
read nor written by it, and **no D2 code was changed in this step.**

**Player physics is bit-identical between arm A and arm C.** `MASHED_PLAYERTRACE` on both,
6614 common lines: **exactly one field ever differs across the whole trace, and it is
`alive`.** `pos`, `yaw`, `sp`, `vel`, `dt`, `a144`, `av`, `c9ec`, `gnd`, `bodyfwd`, `n9c8`,
`r0up`, `r0at`, `b14`, `b1c`, `v9e4`, `gate`, `lap`, `prog`, `rt` are identical on every
line. This also disposes of `TrackRenderer.cpp:2913`: the auto-drive branch flips with
`alive` and the player's motion does not change, because the player never moves
(`sp = 0.6380385160446167` throughout).

## Both-sides rule — not triggered, and why

`PREREG_STEP1.md` required a contrived ORIGINAL-side control **if** the two sides' elimination
behaviour differed inside the window. It does not need to run, and the reason is stronger
than "they agree": **nothing downstream of the elimination moves on the port at all** (0 of
2640 byte-slots), so holding slot 0 alive on the original could not change an original-side
number either. Running it would spend a capture to re-measure a zero. **No original-side
control arm was run, and that is a choice recorded here rather than an omission.**

## Provenance

```
# ORIGINAL, two runs, the o1 argv + --alive-probe and nothing else:
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/d3_elim_20261003/<o_e1|o_e2>.msd \
    --statediff-car 1 --statediff-aistep --alive-probe --hold 60
  env: MASHED_MUTE=1 (tool default), MASHED_TITLE="D3 elim <tag>", MASHED_WIN_POS=primary-bl

# PORT, six runs; only MASHED_NO_ELIM differs between arms:
py -3.12 re/tools/sa_capture.py verify/d3_elim_20261003/<tag> 8,30,60 \
    MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="D3 elim <tag>" MASHED_PLAYERTRACE=1 \
    [MASHED_NO_ELIM=1] MASHED_AI_STEPDUMP=verify/d3_elim_20261003/<tag>.csv
  ea1 ea2 ea3   arm A, default
  ec1 ec2 ec3   arm C, + MASHED_NO_ELIM=1
```

`player_trace.log` is opened `"a"` (`TrackRenderer.cpp:3154`), so it is deleted before each
run and moved to `<tag>_player_trace.log` after it.

Every `MASHED.exe` was spawned and killed **by PID** by `scenario_launch.py`, every
`mashed_re.exe` by `sa_capture.py`. No blanket kill by name. `original/MASHED.exe` untouched;
no `unlock_*` patch applied. Band files proved unedited: `git diff --stat` and
`git status --porcelain` on `re/tools/ai_ctrl_window.py` and `re/tools/ai_speed_env.py` are
both empty. `log/rules_oracle_rule3.json` carries an uncommitted modification that is **not
this session's** and was never staged.

## What this closes and what it does not

- **Closes:** U-9185 evidence-missing item **(a)**. The arm A vs arm B delta is single-cause;
  the confound's share is 0.
- **Corrects:** (i) `VehicleStep(0)` runs in neither port arm and not on the original either;
  (ii) arm B eliminates the player too, 34 window calls later than arm A.
- **Does not close:** U-9185 items (b) (which heading candidate) and (c) (what carries the
  +15..31 %). (c) is STEP 2.
