# PRE-REGISTRATION — D3: what the port-only AI start boost does to criteria (b) and (e). **UNRUN.**

Written and committed **before** any capture, any score and any diff in this directory.
Branch `race/first-frame-parity`, HEAD at write time `df27a28d`. Nothing is pushed.

## What is being measured, and why

`mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:703-707` seeds **every slot
`!= 0`** — i.e. exactly the three cars criteria (b) and (e) are scored on — with
`+0xbf8 = 1` (boost state) and `+0xbf4 = 1300` (boost timer), one-shot, guarded by
`MASHED_NO_START_BOOST`. The player, slot 0, is **not** seeded (`slot != 0` at `:704`).
The seed is a **fitted port-only scaffold**: the original's arming writer is unknown and
recorded as `[UNCERTAIN] U-D3-BOOST-ARM` in the same comment block (`:694-702`), and
`re/analysis/D3_DRIVE_FORCE_2026-09-29.md` carries the force law it feeds.

U-9185 records the consequence: in the scored window the port's AI runs
**3421.7 / 3346.2 / 3495.9** against the original's **2419.5 / 2397.0 / 2617.6**
(**+41 / +40 / +34 %**), and the steer magnitude is `m = err * speed * 0.0030034`
(`_DAT_005cd0e8`, `0x00416656`), so speed is a **linear multiplier** on the steer byte.
U-9185's own remedy list puts this first: *"FIRST, and it is one environment variable with
no code change: re-capture the port side with `MASHED_NO_START_BOOST=1` and re-score
**both** (b) and (e)."* That is this step and nothing more. **No fix, no code change, no
C-level move.**

## Build, and why arm A is re-captured instead of reused

| fact | value |
|---|---|
| last commit touching `mashedmod/src` | `015537a2` 2026-10-02 17:58:10 -0300 |
| `mashedmod/build/mashed_re.exe` mtime | 2026-10-02 **21:33:35** |
| `mashed_re.exe` SHA-256 | `a6b07bd10ec86a78870108e7619afd30d708efb74c3664477acf0556d88e99e0` |
| `mashedmod/build/mashed_re.map` timestamp | `6ac04d5f` = Fri Oct 2 **21:33:35** 2026 (same build) |
| any `mashedmod/src` file newer than the exe | **none** |
| `git status --porcelain mashedmod/src` | **empty** |

The exe is **not stale** versus the last source commit, so **no build is run** (the
briefing's condition for building is not met).

**But the exe is NEWER than `verify/d3_rebase_20261002/r1/r2/r3.csv` (21:06–21:10).** The
briefing allows reusing `r1/r2/r3` as arm A *only if the exe is unchanged since then, proven
by SHA* — there is no recorded SHA for the 21:06 exe, so that cannot be proven. Therefore:

> **Arm A is re-captured fresh (`a1`,`a2`,`a3`) with the SHA above.** `r1/r2/r3` are
> reported as a **cross-check only**, never as arm A.

## Recipe — unchanged from the 2026-10-02 re-baseline (`verify/d3_rebase_20261002/PREREG_STEP1.md`)

```
py -3.12 re/tools/sa_capture.py verify/d3_noboost_20261003/<tag> 8,30,60 \
    MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="D3 noboost <tag>" \
    MASHED_AI_STEPDUMP=verify/d3_noboost_20261003/<tag>.csv
```

| tag | arm |
|---|---|
| `a1` `a2` `a3` | **arm A** — default build, boost **ON**. Three repeats. |
| `b1` `b2` `b3` | **arm B** — identical plus `MASHED_NO_START_BOOST=1`. Three repeats. |

Scorers and bands **unchanged, and neither file is edited in this step**:

- (b) `py -3.12 re/tools/ai_ctrl_window.py --check <csv>` — `TOLERANCE` at
  `ai_ctrl_window.py:48-54`, 10 bands x 3 cars = **30 bands**.
- (e) `py -3.12 re/tools/ai_speed_env.py --check <csv>` — `ENVELOPE`/`MODE_CLEAN_K`,
  2 gated stats x 3 cars = **6 values**.
- `git diff --stat` on both files is printed in the RESULT as proof they are untouched.

Original reference for every cross-side number:
`verify/d3_modes37_20261002/o1.msd.aistep.csv` (the capture U-9182/U-9183 were closed on).

## The knob-took witness — separate runs, never the scored ones

`MASHED_AI_STEPDUMP` has **no `+0xbf8` / `+0xbf4` column** (header checked) and dumps only
`v = 1,2,3`, so the seed is not observable in the scored captures. A second pair of runs,
**not scored and not part of any (b)/(e) number**, carries the witness:

```
py -3.12 re/tools/sa_boostwatch.py verify/d3_noboost_20261003/<aw|bw> 40 \
    --map mashedmod/build/mashed_re.map  <same MASHED_* env as the arm> \
    MASHED_PLAYERTRACE=1
```

`re/tools/sa_boostwatch.py` (added in this step, **no `mashedmod/src` change**) spawns the
exe and polls it with `ReadProcessMemory` — no injection, no Frida, read-only. The exe links
`/BASE:0x10000 /DYNAMICBASE:NO` (`mashedmod/build.bat:218`), ASLR is off, so the linker
map's Rva+Base column is the runtime VA: `g_records` `0x00c09b88`, `g_startBoosted`
`0x00c16ff8`, stride `0xd04` (`VehiclePhysicsRun.cpp:109`). These are **this exe's** static
arrays (`VehiclePhysicsRun.cpp:122`, `:148`), not the original's `DAT_008815a0`.

**Self-check, registered before the run:** a sample counts only when
`record[v] + 0x000 == v` for all four slots — the self-ref index the original writes at
`0x0046bab8` (`Util/PromoLoop_round80.cpp:31`) and the port keeps. **Zero passing samples
makes the witness VOID and the RESULT says VOID**, it does not print zeros as if they were
measurements (memory `absent-log-proves-nothing-run-a-control`).

## Gates — all registered here, before any number exists

- **G-KNOB** — the witness **passes** iff, with the self-check OK:
  (i) **arm A**: `g_startBoosted[v] == 1` and `+0xbf8 == 1` observed with `+0xbf4` at/below
  `1300` for **each of v = 1, 2, 3**;
  (ii) **arm B**: `g_startBoosted[v] == 0` and `+0xbf8 == 0` for the **whole run**, all of
  v = 1, 2, 3;
  (iii) **slot 0**: `g_startBoosted[0] == 0` and `+0xbf8` never seeded, in **both** arms.
  If any leg fails, the knob is **not** proven live and **I STOP** rather than score.
- **G-DET-A** — `a1`/`a2`/`a3` must agree to **every printed digit** on all (b) and (e)
  lines. If not, arm A's numbers are samples, not measurements, and the **spread is
  reported** in place of a single value.
- **G-DET-B** — the same for `b1`/`b2`/`b3`. Same consequence: report the spread.
- **G-BOOT** — a run whose CSV is missing, or gives fewer than 220 window calls on any car,
  or reports `speed = 0`, is **retried once**. A second failure is reported as a failure and
  not retried again. **Every failed boot is reported**, retried or not.
- **G-PLAYER** — `player_trace.log` from the two witness runs is diffed arm A vs arm B. The
  player is **not seeded in either arm** by construction (`:704`), so any difference is a
  **car-to-car** effect and is filed as a **D2 REOPEN CANDIDATE** row with the first
  differing field and line. It is **not** treated as a knob failure.
- **G-READ (the reading rule, fixed now)** —
  **(b) IMPROVES** iff the failing-band count drops on **all three** cars.
  **(e) REGRESSES** iff **any** of its six gated values leaves its band.
  Both are reported **separately and without trading one against the other.** Which
  trade-off to accept is the **user's decision**, stated as such and not made here.
- **G-WINDOW** — every (b) and (e) number is printed with its `n`, the **median window
  speed** and the **median call index** of its window, beside the **original's** window
  speed (memory `band-on-speed-compares-different-moments`).

## STEP 3 — only if (b) still fails in arm B

Descriptive, **no fix**:

1. `re/tools/ai_band_sim.py --orig verify/d3_modes37_20261002/o1.msd.aistep.csv --sa <b1>` —
   the counterfactual matrix, so the **speed residual** that survives the knob is separated
   from the error residual. Its own validation gate (>= 95 % reproduction of the observed
   bytes) must pass or the arm is not believed.
2. `re/tools/ai_posmatch.py --orig ... --port <b1> --part A|B` — the **matched-position**
   decomposition, i.e. U-9185's two body-heading candidates: the reconstructed yaw scalar
   `aib_own_fwd_xz` (`TrackRenderer.cpp:97-100`, filled `(cos(yaw), sin(yaw))` at `:3729`)
   versus the physics yaw (`:3315` in / `:3327` out).

## Collateral review — mandatory, run after the verdicts and declared as such

`re/tools/statediff/collateral.py`, with the scope file
`verify/d3_noboost_20261003/scope.txt` written **before** the run:

- **paired**, arm A vs arm B, per car, `--floor-a` from the A repeats and `--floor-b` from
  the B repeats, so no field is called divergent below its own measured noise floor.
- **banded**, cross-side, port arm B vs the original `o1`, `--speed rec_9e4`, because the
  two trajectories separate and a frame-paired cross-side table is meaningless
  (memory `a-band-scored-off-regime-is-not-a-measurement`).
- **matched position** cross-side via `ai_posmatch.py`, R = 0.12 world units.

**Scope, fixed now** (`writeset | downstream | outside`, first match wins):
the knob's direct write set (`+0xbf8`, `+0xbf4`) is **not present in this channel**, so it
is declared empty; `rec_9e4`, `rec_b0c`, `own_x`, `own_z`, `c0`, `c1`, `c3`, `c4`, `c5`,
`curv`, `look_*`, `march_*`, `hist_*` are **downstream**; **everything else is outside**,
explicitly including `ai_mode`, `substate`, `diff_a360`, `flag_a368`, `tgt_7ffc`,
`clk_0ff4`, `step_1008`, `c7`, `block`, `spline`, `ai_type`, `ai_spline_idx`,
`ai_override`. Anything outside that moves is reported.

## D2 WATCH (D-11071 triggers)

`+0x4a4`, the contact collector / `FUN_00538c80`, grip-clamp #6, the substep/chunk loop,
`ReassertContacts`, and player speed on Training. **No D2 code is changed in this session.**
Player slot 0 moving between arms is a **D2 REOPEN CANDIDATE** row (G-PLAYER).

## Out of scope

No port, no edit under `mashedmod/src`, no band edit, no C-level move, no `hooks.csv`
change, no push. `log/rules_oracle_rule3.json` carries an uncommitted modification that is
**not this session's** and is **never staged**. Trackers are mutated only through
`re-classify` (U-9185 amended with the numbers), CRLF preserved. Commits use explicit
pathspecs, never `-A` or `.`.

## PID hygiene

Every `mashed_re.exe` is spawned and killed **by PID** (`sa_capture.py`,
`sa_boostwatch.py`). No blanket kill by name.
