# STEP 1 PRE-REGISTRATION — the player-elimination confound. **UNRUN.**

Written and committed **before** any capture in this directory is taken. Branch
`race/first-frame-parity`, HEAD at write time `e0b35ff9` (the decision record).

## Why this step exists

`verify/d3_noboost_20261003/RESULT.md`'s D2-WATCH table files a **measurement confound**:
`race_[0].alive` differs between arm A (boost ON) and arm B (`MASHED_NO_START_BOOST=1`) at
`rt = 1.8667 s`, **inside** the 220-call scored window, so the reported arm A vs arm B
deltas on criteria (b) and (e) are *the seed plus that divergence* and that session did not
separate the two shares. U-9185's path-to-resolution item (2) says in as many words: *"until
that is done no arm A vs arm B delta here is single-cause."*

The user's 2026-10-03 decision (ROADMAP §D3, commit `e0b35ff9`) keeps the seed, so the
remaining work is (1) kill this confound and (2) attack the surviving over-speed. This is (1).

## A CORRECTION TO THE PREMISE, recorded BEFORE the run

The kickoff and `verify/d3_noboost_20261003/RESULT.md` both state that
`AiStandalone.cpp:1708`'s loop *"runs `VehicleStep(0)` in one arm and not the other"*.
**That is false, and it is false by reading, not by measurement.** The loop is:

```
for (int v = 0; v < 4; ++v) {                                 // AiStandalone.cpp:1708
    int t = s_host.veh_type(v);
    if ((t != 0 && t != 1) || s_host.ai_target_enable() == 1) {   // :1709-1710
        if (s_host.car_alive(v) == 1) { ... VehicleStep(v); ... } // :1711-1714
    }
}
```

`aib_veh_type(0)` returns **0** (`TrackRenderer.cpp:86`) and `aib_ai_target_enable()`
returns **0** (`:96`), so for `v == 0` the test at `:1709` is false and the body — including
the `car_alive(0)` call — is **never reached in either arm**. `VehicleStep(0)` therefore runs
in **neither** arm, and `g_aib.alive[0]` is not consulted by the AI tick at all.

This is consistent with both sides' step dumps: `o1.msd.aistep.csv` has **0 rows with
`v == 0`** out of 5463, and `a1.csv` / `b1.csv` have **0 rows with `v == 0`** out of 7927 /
7714. (The port's dump loops `v = 1..3` by construction, `TrackRenderer.cpp:3775`, so its
zero is weaker evidence than the original's.)

So the confound, if it is real, must travel by some **other** path. This step does not assume
it is real and does not assume it is zero. It measures.

## The scenario, unchanged from the reference captures

ORIGINAL side — the exact argv of `verify/d3_modes37_20261002/o1.msd.provenance.json`, plus
the new probe flag and nothing else:

```
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/d3_elim_20261003/<tag>.msd \
    --statediff-car 1 --statediff-aistep --alive-probe --hold 60
```

(`--track 0` = **Training**, `scenario_launch.py:2428-2430`; the standalone recipe's
`MASHED_TRACK_VIEW=Training` is the same track. `--rule` is left at its default **0**.
`MASHED_MUTE=1` is the tool default, `scenario_launch.py:2721`. `MASHED_TITLE` is set on
every launch; `MASHED_WIN_POS=primary-bl`; `MASHED_NAV_DEMO` is never used.)

PORT side — the recipe of `verify/d3_noboost_20261003/RESULT.md`, unchanged:

```
py -3.12 re/tools/sa_capture.py verify/d3_elim_20261003/<tag> 8,30,60 \
    MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="D3 elim <tag>" \
    MASHED_PLAYERTRACE=1 \
    MASHED_AI_STEPDUMP=verify/d3_elim_20261003/<tag>.csv
```

**Bands and scorers are NOT edited.** `re/tools/ai_ctrl_window.py` and
`re/tools/ai_speed_env.py` are proved unedited in the RESULT by `git diff --stat` and
`git status --porcelain` on both.

## The probe, and what validates it

`--alive-probe` (`re/frida/scenario_launch.py`, this commit) attaches **three entry hooks
only** — Frida's `Interceptor` is an entry hook and a mid-body attach overwrites a local
(memory `frida-interceptor-is-entry-only`):

| hook | RVA | rate | what it records |
|---|---|---|---|
| `SegmentCheck` | `0x00410d10` | 1 / race update | one CSV row: `alive[0..3]` on entry **and** on leave, the return value, `clk_0ff4` (`0x007f0ff4`), `substate` (`0x0063ba8c`), `rule` (`0x007f0fd0`), `participants` (`0x008a94d0`), the elimination gate's float (`0x00898980`, what `FUN_00442df0` returns and `0x00410d10` compares against 10.0), and `race_pct[0..3]` (`0x008a96ec + i*0x30c`) |
| `AiVehicleStep` | `0x00418560` | ≤ 4 / frame | per-slot call tally `vstep[0..3]` |
| `AiTickLoop` | `0x00418860` | 1 / frame | tick count |

Total ≈ 360 calls/s at 4 cars, under the 1000/s ceiling in `CLAUDE.md`.

`alive[i]` is read at `*(i32*)(0x008815a4 + i*0xd04)` — the address `FUN_0046c7b0` itself
reads (`hooks_registry.py:1322-1323`: `DAT_008815a4[idx*0x341]`, and `0x341*4 == 0xd04`);
`re/frida/camera_probe.py:128-137` reads the same address on the same side.

### Gates, registered before running

- **G-KA1 (the probe's liveness control).** On the first 200 `SegmentCheck` calls, for all
  four slots, the direct read must equal `FUN_0046c7b0(i)` called as a `NativeFunction`.
  **Required: 200/200.** Anything less and the probe is **VOID**; I STOP and report that
  rather than any alive claim. *An all-ones alive vector is a finding only if this passes*
  (memory `absent-log-proves-nothing-run-a-control`).
- **G-LIVE.** The run must record **≥ 1** `SegmentCheck` row and **≥ 1** `AiTickLoop` tick.
  Zero of either = the hooks never fired = **VOID**.
- **G-XFER (does the probe perturb the reference?).** The probe run's own `.aistep.csv` must
  reproduce `verify/d3_modes37_20261002/o1.msd.aistep.csv` on **every printed digit** of
  `ai_ctrl_window.py --check` and `ai_speed_env.py --check`, cars 1..3. If it does not, the
  elimination answer is still reported but is labelled **probe-perturbed** and no (b)/(e)
  number from this run is used.
- **G-DET-O.** Two original-side runs (`o_e1`, `o_e2`). The **alive answer** (Q1/Q2 below)
  must agree between them. Disagreement is reported as non-determinism, not averaged.

## The questions, and the answers' definitions

- **Q1 — is the PLAYER eliminated on the ORIGINAL in this scenario?**
  **YES** iff column `a0_out` (or `a0_in`) transitions `1 → 0` anywhere in the alive CSV.
  Reported with `frame`, `tick`, `clk_0ff4`, and its position relative to the original's own
  scored window. *The window is not a clock constant*: `ai_ctrl_window.py:25` defines it as
  car 1's calls `[i0, i0+220)` with `i0` = the first call where `c4 != 0`. I convert that to
  a `clk_0ff4` interval from the **same run's** aistep CSV and state whether the transition
  falls before / inside / after it.
- **Q2 — does the AI tick run for slot 0 on the ORIGINAL?**
  **YES** iff `vstep[0] > 0` at the end of the run. The full per-slot tally is printed
  beside the aistep per-`v` row counts, which are an independent channel for slots 1..3.
- **Q3 — what does the PORT do, and by which RVA?** From the port's own existing channels
  (`MASHED_PLAYERTRACE`'s `alive=` field, `TrackRenderer.cpp:3209`; the step dump's per-`v`
  row counts), no new port instrumentation.

## Branch rules, registered before the answers exist

- **R1.** If Q1 = YES **and** the transition falls before or inside the original's scored
  window: the port's arm A is qualitatively the same as the original on this axis, so the
  **arm A vs ORIGINAL** comparison carries no elimination confound, and the confound is
  confined to the **arm A vs arm B** contrast.
- **R2.** If Q1 = NO: the port's arm A diverges from the original. That is a **candidate
  defect**, to be named by RVA and confirmed on the running original before anything is
  coded. It is **not** fixed in this step.
- **R3.** Either way, the confound's **share of the arm A vs arm B delta** is measured by
  STEP 1C below, not argued. The static reach test (STEP 1B) is **evidence of a mechanism
  only and cannot by itself set the share to 0.**

## STEP 1B — static reach test (port), declared as non-measurement

Enumerate every reader of `race_[0].alive` and of `g_aib.alive[0]` under `mashedmod/src`
with `file:line`, and classify each as reaching / not reaching (i) the AI control bytes
`c0,c1,c4,c5` or (ii) AI-car physics, **within the 220-call window**. Result is reported as
a table. It frames STEP 1C; it does not replace it.

## STEP 1C — the live control (port)

A **default-off dev knob `MASHED_NO_ELIM=1`** that makes the two elimination blocks
(`TrackRenderer.cpp:4661-4670` and `:4716-4725`) a no-op and changes **nothing else**. It
does not alter the default build: unset, the code path is byte-for-byte what it is today.
No band, no scorer, no physics constant is touched.

| arm | recipe |
|---|---|
| **A** | default (boost ON, `MASHED_ROUND=1`), 3 repeats `ea1 ea2 ea3` |
| **C** | A + `MASHED_NO_ELIM=1`, 3 repeats `ec1 ec2 ec3` |

- **G-CTL-KNOB (two independent channels, both required).**
  1. `MASHED_PLAYERTRACE`'s `alive=` must be **1 on every line** of arm C and must go to
     **0** in arm A (arm A's own witness that there was something to suppress).
  2. Arm A's step dump stops producing **car 2** rows at `clk ≈ 64450` because `AiStepDump`
     skips `!g_aib.alive[v]` (`TrackRenderer.cpp:3777`); arm C's must carry car 2 **past**
     that clk. If either channel fails, arm C is **VOID** and no share is claimed from it.
- **G-CTL-DET.** `ec1`/`ec2`/`ec3` must agree to every printed digit on both scorers, as
  `ea1`/`ea2`/`ea3` must. Any spread is reported, not averaged.
- **DECISION RULE.** Compare arm C to arm A over the 220-call window, all three cars, on the
  four scored bytes `c0`, `c1`, `c4`, `c5`:
  - **bit-identical on all three cars → the confound's share of the arm A vs arm B delta is
    ZERO**, reported as zero, and every (b)/(e) number already taken stands unchanged.
  - **any difference → the share is that difference**, reported per band and per car, with
    the first divergent window call index and the carrying `file:line` named.
- **Arm A is re-scored from `ea1..ea3` regardless** — 3 runs, with `n`, median speed and
  median call index beside every number — and compared digit-for-digit to
  `verify/d3_noboost_20261003`'s `a1/a2/a3`. A mismatch means the build or recipe moved and
  is reported as such before any other number.

## Both-sides rule

If Q1 shows the ORIGINAL's elimination behaviour differs from the port's **inside the
window**, then per the kickoff a harness-level control must be applied to **both** sides, and
the original-side arm (a Frida write that holds slot 0 alive) will be pre-registered as a
separate amendment **before** it is run, and labelled a contrived control arm. If Q1 shows
they agree, no original-side control is needed and the RESULT says so explicitly.

## D2 WATCH (D-11071)

`MASHED_NO_ELIM` touches **none** of D-11071's five triggers: it writes nothing in `+0x4a4`,
the contact collector, grip-clamp #6, the substep/chunk loop or `ReassertContacts`. If arm C
moves **player** physics (`MASHED_PLAYERTRACE` columns `pos/yaw/sp/vel/a144/av/gnd/b14/b1c/
v9e4`), that is reported as a **D2 REOPEN CANDIDATE** with evidence, and no D2 code is
changed in this step.

## Out of scope for this step

- The start boost. It stays ON in every arm here (the user's decision).
- The +15..31 % over-speed. That is STEP 2.
- Any port of a physics or AI term. No C-level moves in this step.

## Hygiene

Every `MASHED.exe` is spawned and killed **by PID** by `scenario_launch.py`; every
`mashed_re.exe` by `sa_capture.py`. No blanket kill by name. `original/MASHED.exe` is not
patched further and no `unlock_*` script is run. `log/rules_oracle_rule3.json` carries an
uncommitted modification that is **not this session's** and is never staged. Commits name
explicit pathspecs.
