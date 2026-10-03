# RESULT — STEP 2 / 2B: what carries the AI over-speed, boost ON

Pre-registrations: `PREREG_STEP2.md` (committed unrun at `23cb9d7d`'s parent) and the
amendment `PREREG_STEP2B.md` (committed unrun at `23cb9d7d`). Branch
`race/first-frame-parity`. The start boost is **ON in every arm** (user decision, `e0b35ff9`).

## Verdict in one line

**The over-speed is a COMMAND defect, not a physics one. Before the port's AI first disagrees
with the original's commanded throttle, the port's whole force chain reproduces the
original's speed to 0.23–0.98 %. After it, the original lifts or brakes on 149 of 660 window
calls where the port holds full throttle — and all 149 are accounted for, with zero
unexplained, by THREE unported sources, two of them located by RVA.**

## A PRE-REGISTERED GATE FAILED AND THE REGISTERED ANALYSIS DID NOT RUN

`PREREG_STEP2.md` §2.0's **G2-KA** required the four-rule accel/brake model to reproduce the
**ORIGINAL's** own logged `c4`/`c5` on ≥ 95 % of its 220 window calls per car. Measured:

| car | ORIGINAL | PORT |
|---|---|---|
| 1 | **140/220 = 0.636 FAILED** | 220/220 = 1.000 PASS |
| 2 | **206/220 = 0.936 FAILED** | 220/220 = 1.000 PASS |
| 3 | **165/220 = 0.750 FAILED** | 220/220 = 1.000 PASS |

`re/tools/ai_speed_budget.py` stopped at the gate and printed nothing further. **§2.1, §2.2,
§2.3, §2.4 and §2.5 of `PREREG_STEP2.md` did not run and are reported nowhere.**
`PREREG_STEP2B.md` replaced them, committed unrun, and withdrew the old thresholds rather
than reusing them. **This is the one gate replacement in this step and it is stated here, in
the commit message, and at the top of the amendment.**

The gate was right to refuse, and the reason it refused is the finding: the model is the
**port's mode-0 arm**, and the port pins `mode = 0` (`AiStandalone.cpp:844`) because the
targeting chain is stubbed. The original does not stay in mode 0.

## STEP 2B — EVERY AMENDED GATE PASSED

| gate | outcome |
|---|---|
| **G2B-SENS** | **PASS.** All three registered onset thresholds (0.5 %/25, 1 %/50, 2 %/100) give the same classification on all three cars. |
| **G2B-DET** | **PASS.** `o_e1` vs `o_e2` and `ea1` vs `ea2` give the same onset call and the same classification on all three cars. The targeting accounting below is **cell-for-cell identical** between `o_t1` (2 inner hooks) and `o_t2` (4), so the added hooks perturbed nothing. |
| **G2B-LIVE** | **FAILED AS WRITTEN, and the failure is reported, not re-thresholded.** See below. |
| **G2B-NOFIT** | **PASS.** No constant fitted; no free parameter introduced anywhere. |
| **G2B-SCOPE** | **PASS.** The per-car split is reported per car; car 3's mode-0 share is not absorbed into car 1's mode-7 finding. |

## 2B.1 / 2B.2 — THE ONSET IS A COMMAND EVENT ON ALL THREE CARS

| car | onset `k` (0.5 % / 1 % / 2 %) | classification | what differs at `k` |
|---|---|---|---|
| 1 | 101 / **102** / 103 | **COMMAND** | `c4` orig **64** vs port **255**; `ai_mode` orig **7** vs port **0** |
| 2 | 24 / **24** / 24 | **COMMAND** | `c5` orig **255** vs port **0** (from `k = 23`); `ai_mode` orig **3** vs port **0** |
| 3 | 41 / **41** / 41 | **COMMAND** | at `k = 40` `(c4, c5)` orig **(0, 255)** vs port **(255, 0)**; `ai_mode` orig **3** vs port **0** |

Car 1 at the onset — the original has been commanding `c4 = 64` since before `k = 100` and
the speeds separate as the throttle cut bites:

| `k` | speed orig | speed port | Δ | `c4` o\|p | `c5` o\|p | `ai_mode` o\|p |
|---:|---:|---:|---:|---|---|---|
| 100 | 3527.0 | 3524.1 | **−2.9** | 64\|255 | 0\|0 | 7\|0 |
| 101 | 3501.3 | 3535.6 | +34.2 | 64\|255 | 0\|0 | 7\|0 |
| 102 | 3476.1 | 3546.9 | +70.8 | 64\|255 | 0\|0 | 7\|0 |
| 104 | 3427.4 | 3568.6 | +141.2 | 64\|255 | 0\|0 | 7\|0 |

Car 2 at `k = 24` the original's speed falls **2374.5 → 1050.4 in one call** while it is
braking (`c5 = 255`) and the port is not.

## 2B.3 — THE PRE-ONSET BUDGET, AND IT SUPERSEDES THE FORCE-TERM BUDGET

| car | span | median \|Δspeed\| | max \|Δspeed\| | max as a fraction |
|---|---|---:|---:|---:|
| 1 | calls 0..101 | **0.873** | 34.242 | **0.00978** |
| 2 | calls 0..23 | **0.715** | 1.197 | **0.00228** |
| 3 | calls 0..40 | **0.473** | 1.341 | **0.00228** |

All three are under the registered 1 %, so by `PREREG_STEP2B.md` §2B.3 **§2.5's per-term
force budget is SUPERSEDED, not skipped**: across those spans the port's **drive force
(`+0xb14`/`+0xb1c`, A4 `0x00470670`), A5 drag (`0x0046ddb0`), A6a's clamps including
grip-clamp #6 (`0x00467650`), the contact solver (`0x0046f6c0`) and the gear/rev channel
(`+0xb0c`, `0x00470724`/`0x0047072c`) jointly reproduce the original to under 1 %**, and
none of them is the carrier. That is stronger than a per-term table would have been and it
cost **no new instrumentation on either side**.

Car 1's `max` of 34.242 is reached at call **101**, the call immediately before its own
onset; over calls 0..95 its max is far smaller, and the median over the whole span is
**0.873 units on a 2000–3500 scale**.

**This also disposes of attempt 20's contact-collector change inside the window**, as
`verify/d3_rebase_20261002` predicted: its first effect was at AI-step call 455, 235 calls
past the window, and nothing here contradicts that.

## 2B.4 + the live leg — THE THREE UNPORTED SOURCES, 149 of 149 ACCOUNTED FOR

`--statediff-aistep` was extended (this commit) with four **entry-hook** return values
recorded in the same `FUN_00416250` call: `FUN_00414a70` (`0x00414a70`, closest-vehicle),
`FUN_00414c30` (`0x00414c30`, obstacle avoidance), `FUN_004150e0` (`0x004150e0`), and
`FUN_00416060` (`0x00416060`, line-of-sight), plus `ctrl[4]`/`ctrl[5]` **on entry**.
`-1` means "not called in this call".

Grouping **every** window call of **both** `o_t1` and `o_t2` by that signature — 660 calls
per run, identical cell-for-cell between the two runs:

| `(ret14a70, ret14c30, ret150e0, ret16060)` | calls | model | logged `(c4, c5)` | what the port is missing |
|---|---:|---|---|---|
| `(0, 0, 0, −1)` | **448** | **AGREE** | — | nothing. The port's mode-0 accel/brake tail is exact. |
| `(1, −1, −1, 1)` | **63** | **AGREE** | — | mode 3 has no tail; its effect is through the target swap and the curvature multiplier, and the model is fed the original's own `err`. |
| `(2, −1, −1, −1)` | **36** | **MISMATCH** | `(0, 255)` on all 36 | **`FUN_00414a70 == 2` → the immediate-brake return at `0x00416405`** (`ctrl[4] = 0`, `ctrl[5] = 0xff`, return before the mode commit at `0x00416590` and before the steer-history stores at `0x004165cc` / `0x0041670c`). |
| `(0, 2, −1, 1)` | **49** | **MISMATCH** | `(64, 0)` on all 49 | **`FUN_00414c30 == 2` → mode 7 → `ctrl[4] = 0x40`** (`MOV EBX,7` at `0x0041643f`; tail at `AiStandalone.cpp:924`, already written in the port and never reached). |
| `(0, 0, 0, 1)` | **64** | **MISMATCH** | `(0, 255)` on all 64 | **`FUN_004148b0 != 0` && LOS `!= 0` → a SECOND immediate return** (`ctrl[5] = 0xff`, `ctrl[0] = ctrl[1] = 0`, return — and it does **not** touch `ctrl[4]`, which keeps its entry value). Located in the decompilation and then confirmed live; see below. |
| | **660** | | | |

**0 of 149 mismatches are unexplained**, on both runs.

### The third source: located in the decompilation, then confirmed live 64/64

`PREREG_STEP2B.md` left this family `[UNCERTAIN]` and named its resolution path: *"decompile
`0x004162f7..0x0041645e`'s `ret14c30 == 0` arm and find the second early return."* Done, via
`py -3.12 re/tools/decomp_pc.py 0x00416250` (headless `analyzeHeadless`, `-readOnly`, pool
slot `Mashed_pool0`, **the master Ghidra project was not opened or written**). The
decompilation of `FUN_00416250` shows, verbatim, inside the
`gameMode == 6 && FUN_00443080() == 0` block (`0x0041649b` / `0x004164a8`):

```c
    if (local_48 == 0) {                                   // no behaviour mode committed
      iVar5 = FUN_004148b0(param_1,&local_24,&local_2c,param_2);
      if ((iVar5 != 0) && (iVar5 = FUN_00416060(&local_1c,&local_2c), iVar5 != 0)) {
        param_3[5] = 0xff;                                 // ctrl[5] = brake
        *param_3   = 0;                                    // ctrl[0] = 0
        param_3[1] = 0;                                    // ctrl[1] = 0
        return;                                            // <-- before 0x00416590
      }
      iVar5 = FUN_00415020(param_2);
      if (iVar5 != 0) { local_48 = 5; }
    }
```

and the first one, in the `FUN_00414a70` arm:

```c
      iVar5 = FUN_00414a70(param_1,&local_24,&local_2c,param_2);
      if (iVar5 == 1) { ... local_48 = 3; }
      else {
        if (iVar5 == 2) {
          param_3[4] = 0;                                  // ctrl[4] = 0
          param_3[5] = 0xff;                               // ctrl[5] = 0xff
          return;                                          // 0x00416405
        }
```

Both return **before** the mode commit `*(int *)(&DAT_0089a52c + param_2*0x74) = local_48`
and before the steer-history stores — which is exactly why the logged `ai_mode` is stale and
`hist_d8` / `hist_dc` are frozen on those calls, and why the `0x004148b0` family's `c4` is
whatever `c4_in` was (measured **0** on every one of car 1's 31).

**Live confirmation, `o_t3`, with `FUN_004148b0` added as a fifth entry hook:**

| predicate | calls | the family | intersection |
|---|---:|---:|---:|
| `ret148b0 != 0 && ret16060 > 0` | **64** | **64** | **64** |

**64 of 64, no false positives**, and all 448 agreeing calls have `ret148b0 == 0`.
`FUN_004148b0` is called on every mode-0 call and returns 1 on exactly the 64 brake calls.

| run | hook set | `(0,0,0,−1[,0])` | `(1,−1,−1,1[,−1])` | `(2,−1,…)` | `(0,2,…)` | `(0,0,0,1[,1])` |
|---|---|---:|---:|---:|---:|---:|
| `o_t1` | 2 inner | 448 | 63 | 36 | 49 | 64 |
| `o_t2` | 4 inner | 448 | 63 | 36 | 49 | 64 |
| `o_t3` | 5 inner | 448 | 63 | 36 | 49 | 64 |

**Three independent live runs, cell-for-cell identical.**

Per car, which source carries which:

| car | `FUN_00414a70 == 2` return | mode-7 `0x40` tail | `FUN_004148b0` return | total lift/brake calls |
|---|---:|---:|---:|---:|
| 1 | 0 | **49** | 31 | 80 (36.4 % of the window) |
| 2 | 10 | 0 | 4 | 14 (6.4 %) |
| 3 | 26 | 0 | 29 | 55 (25.0 %) |
| **all** | **36** | **49** | **64** | **149 of 660 (22.6 %)** |

### The c4 command comparison the kickoff asked for

| | car 1 | car 2 | car 3 |
|---|---|---|---|
| ORIGINAL `c4` set | `{0, 64, 255}` | `{0, 255}` | `{0, 255}` |
| ORIGINAL `c4 = 255` | **63.6 %** | 91.8 % | 70.9 % |
| PORT `c4` set | `{0, 255}` | `{0, 255}` | `{0, 255}` |
| PORT `c4 = 255` | **97.7 %** | 95.5 % | 97.7 % |
| ORIGINAL `c5 = 255` | 21.4 % | 18.6 % | 30.0 % |
| PORT `c5 = 255` | **2.3 %** | 4.5 % | 2.3 % |

**`c4 == 0x40` is emitted on 49 calls and every one of them is a mode-7 call — 49/49 on
`o_e1`, `o_e2`, `o_t1` and `o_t2`.** The port never emits `0x40` in this window.

### G2B-LIVE: FAILED AS WRITTEN. Reported, not re-thresholded.

`PREREG_STEP2B.md` §2B.5 registered *"≥ 99 % of live mode-7 calls must show
`ctrl[4] == 0x40`"*. Measured on the running original: **49 of 80 = 61.3 %. FAIL.**

The gate was written in the wrong direction and the correct relation is reported instead of
the failed one being quietly dropped:

- **The direction that is exact (49/49 = 1.000, four live runs):** every `c4 == 0x40` is a
  mode-7 call, and every mode-7 call **whose mode was committed this call**
  (`ret14c30 == 2`) carries `c4 == 0x40` — 49 of 49.
- The other 31 mode-7 calls carry a **stale** `ai_mode` (`ret14c30 == 0`, nothing committed,
  steer history frozen) and belong to the unlocated family above. The 99 % gate conflated
  "the dumped mode reads 7" with "mode 7 was committed this call", and the four extra
  targeting columns are what separated them.
- **The other live relation registered is exact: `FUN_00414a70 == 2` ⟹ the `0x00416405`
  signature, 36/36 = 1.000**, on ≥ 50 calls across the two runs combined (72). On a single
  run it is 36 and therefore **under-powered against §2B.5's own n ≥ 50 clause**, which is
  stated rather than waved through; the two-run total is reported beside it.

## STEP 3 — CONFIRMED LIVE, AND **NOT LANDED**. Why, and what it would take.

The term is named and confirmed on the running original. **The fix is not landed, and
attempting it in this session would have produced a build whose (b)/(e) numbers mix one
partial fix with two unfixed sources — the exact non-single-cause measurement STEP 1 existed
to remove.** The scope, measured not estimated:

| source | calls | port state (`hooks.csv`) | what it would take |
|---|---:|---|---|
| **`FUN_004148b0`** `0x004148b0` → the `local_48 == 0` immediate return | **64** | **C3, `impl`, `Ai/AiLeaderTimer.cpp`** — and `FUN_00416060` is **C3, `impl`, `Ai/AiTargeting.cpp`** with a committed `frida_diff` | **No new reversing.** Both callees already have faithful bodies. What is missing is (i) the three TUs are in **`asi_sources.rsp` only**, not `exe_sources.rsp`, and (ii) `AiStandalone.cpp:845-855` has the enclosing `gameMode == 6 && ai_target_enable() == 0` block but the comment *"FUN_004148b0 / FUN_00415020 are stubbed (return 0)"* where the call belongs. **RISK, named now and not discovered later:** `LeaderTimer` reads per-car rank/progress tables through `Prog` / `RankAt` / `E470` / `kLimitTbl`, and `verify/d3_modes37_20261002/RESULT_STEP2.md:201-203` already measured the matching trap for `FUN_00484c70` — the standalone's copies of such globals can be `.bss` zeros, in which case the wiring is **inert, not a fix**. That has to be measured before the wiring is believed. |
| `FUN_00414c30` `0x00414c30` → mode 7 | 49 | C2, `mapped`, **no body at all** | **STRUCTURAL.** Its loop iterates `FUN_00484c70` world-objects, and `RESULT_STEP2.md:201-203` measured those globals as `.bss` zeros in the standalone: *"seeding the globals would not be a port."* Mode 7 — car 1's entire carrier — cannot fire until the standalone owns a world-object list. |
| `FUN_00414a70` `0x00414a70` → `0x00416405` | 36 | C2, `mapped`, **no body**; stub call-through at `Ai/AiControlStep.cpp:91` | its own callee `FUN_00414300` is **C2 with no port**; everything else it needs is C3/C4. A real port, but a smaller share than the first row. |

**No code was written for any of them. No C-level moved. The default build is unchanged
except for the default-OFF `MASHED_NO_ELIM` knob landed in STEP 1.**

### This does NOT contradict the 2026-10-02 modes-3/7 refutation

That session refuted *"porting modes 3 and 7 closes criterion (b)"*, on the **steer bands**,
by splicing the original's mode sequence into `ai_band_sim`. **Speed was never an output in
any of its arms** — it was an input spliced in, and that report says so itself
(`RESULT_STEP2.md:167-170`). The present finding is about **speed** and about the
accel/brake tail, which that session's G2-JOINT explicitly set aside as *"the accel/brake
bands, which already PASS"* (`:76-77`). Both statements stand; they are about different
outputs.

## STEP 4 — RE-SCORE, with the arms this session took

**(e): 6/6 PASS**, identical on all six STEP-1 arms and on `verify/d3_noboost_20261003/a1`:

| car | `launch` | band | `ft_median_m0` | n | `regime0` | `flag` |
|---|---:|---|---:|---:|---:|---|
| 1 | **1426.4** | 1397.2..1454.2 | **2550.6** | 100 | 1 | `[0]` |
| 2 | **2053.0** | 2011.5..2093.6 | **2053.0** | 23 | 1 | `[0]` |
| 3 | **2055.2** | 2013.9..2096.1 | **2278.2** | 39 | 1 | `[0]` |

**No regression. (e) stays MET 3/3.**

**(b): 13 failing bands of 30**, unchanged (car 1 five, car 2 four, car 3 four), median call
index **109.5** on every row of every arm and on the original, n = 220 per car.

**Window speed**, median `rec_9e4`, n = 220, median call index 109.5 both sides:

| car | ORIGINAL | port arm A | delta |
|---|---:|---:|---:|
| 1 | 2419.5 | 3421.7 | **+41.4 %** |
| 2 | 2397.0 | 3346.2 | **+39.6 %** |
| 3 | 2617.6 | 3495.9 | **+33.6 %** |

**Guards.**
- **Powerups**: `pwsh re/tools/pu_replay/sweep.ps1` — **11/11 decision CLEAN**, contact CLEAN
  on 10 of 11 with `g3` DIVERGES, which is the known R_FLAME 2-of-546 residue. **At baseline.**
- **Modes (rule-3 oracle)**: `scenario_launch.py --oracle --rule 3 --hold 50` —
  **ORACLE VERDICT GREEN**, `SegmentCheck 0x00410d10` calls 2478 agree 2478 **MISMATCH 0**,
  `EvaluateResult 0x00410510` 2/2, `FinishOrder 0x004177b0` 3381/3381, appends 0,
  round-resets 0.
  **DISCLOSURE:** the oracle writes `log/rules_oracle_rule3.json` by design, so **this run
  overwrote the uncommitted modification that file carried at session start and that is not
  this session's.** It was never staged and is not in any commit here. Both the overwritten
  content and the new content are GREEN with MISMATCH 0 and differ only in the run's call
  counts (3073/3925 vs 2478/3381, i.e. run length), so the information lost is one run's
  counters — but the overwrite happened and is reported rather than left to be noticed.

## Collateral review — run AFTER the verdicts

Nothing in `mashedmod/src` changed in STEP 2 (the only edit is `re/frida/scenario_launch.py`,
an original-side probe, plus two offline tools), so the collateral review that applies is
STEP 1's: `coll_paired_v{1,2,3}.txt`, **1 / 0 / 1 of 35 fields diverging on cars 1 / 2 / 3
over the whole run, and the only one is `seq`, the dump's own row counter** — the single
outside-scope row. The probe's own non-perturbation is shown twice: `o_e1`/`o_e2` reproduce
`o1` on every printed digit of both scorers, and the targeting accounting is cell-identical
between `o_t1` (2 inner hooks) and `o_t2` (4).

## D2 WATCH (D-11071) — NO REOPEN CANDIDATE

`PREREG_STEP2B.md` registered that a **NON-COMMAND** onset on any car would make that car a
D2 REOPEN CANDIDATE. **All three onsets are COMMAND**, and the pre-onset agreement is
0.23–0.98 %, so the shared physics terms are exonerated over those spans rather than
implicated. None of D-11071's five triggers (`+0x4a4`, the contact collector / `FUN_00538c80`,
grip-clamp #6, the substep/chunk loop, `ReassertContacts`) is named by anything in this step,
and **no D2 code was read into a verdict or changed.**

## What is still open

1. **The `FUN_004148b0` wiring, 64 of 149 calls (43 %) — the next thing to do**, and the one
   that needs no new reversing. Add `Ai/AiLeaderTimer.cpp`, `Ai/AiTargeting.cpp` and
   `Ai/AiLineOfSight.cpp` to `exe_sources.rsp` and replace `AiStandalone.cpp:846`'s *"stubbed
   (return 0)"* comment with the decompiled `local_48 == 0` arm quoted above. **Measure
   whether it is inert first** — `LeaderTimer`'s rank/progress inputs may be `.bss` zeros in
   the standalone, which is the same trap `FUN_00484c70` already fell into. A pre-registered
   knob-took witness is mandatory (memory `verify-the-harness-knob-actually-took`).
2. `FUN_00414a70` + `FUN_00414300` port — 36 calls, a real port.
3. `FUN_00414c30` / mode 7 — 49 calls, **blocked** on the standalone's missing world-object
   list. Car 1's entire carrier.
4. U-9185 item (c) is **answered**: the carrier of the surviving over-speed is the commanded
   throttle, not a force term. Item (b) (which heading candidate carries the steering
   residual) is untouched by this step.
5. `[UNCERTAIN]` — the exact byte address of the `FUN_004148b0` early return is not pinned.
   The decompilation places it inside the `gameMode == 6 && FUN_00443080() == 0` block whose
   head the port cites as `0x0041649b` / `0x004164a8`, before the mode commit at
   `0x00416590`; no listing was taken to resolve it further, and none of this step's
   conclusions depends on it.

## Provenance

```
# ORIGINAL, four runs, the o1 argv unchanged:
py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
    --poke-ctrl-slots --statediff-out verify/d3_elim_20261003/<tag>.msd \
    --statediff-car 1 --statediff-aistep [--alive-probe] --hold 60
  o_e1 o_e2   STEP 1, with --alive-probe
  o_t1        + ret14a70 / ret14c30 / c4_in / c5_in
  o_t2        + ret150e0 / ret16060

# offline tools, no game:
py -3.12 re/tools/ai_speed_budget.py --orig <o> --port <p>      # section 2.0, the gate
py -3.12 re/tools/ai_speed_onset.py  --orig <o> --orig2 <o2> \
                                     --port <p> --port2 <p2>    # 2B.1-2B.4
```

Band files proved unedited: `git diff --stat` and `git status --porcelain` on
`re/tools/ai_ctrl_window.py` and `re/tools/ai_speed_env.py` are both empty. Every
`MASHED.exe` spawned and killed **by PID**; no blanket kill by name.
`original/MASHED.exe` untouched; no `unlock_*` patch applied.
`log/rules_oracle_rule3.json` carries an uncommitted modification that is **not this
session's** and was never staged.
