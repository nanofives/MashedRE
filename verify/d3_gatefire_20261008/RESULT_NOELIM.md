# RESULT — the scenario EXISTS: round mode + `MASHED_NO_ELIM`. All gates pass. One passes BLIND.

Date 2026-10-08. Follows `RESULT_WIRE2.md` §6 item 2, chosen by **USER DECISION (Mariano,
2026-10-08)**: "Find or build a scenario with cars alive AND round mode, keeping the current
baselines." No C-level. `original/` untouched, nothing default-ON.

Raw: `NE_{base,off,on}.{gates,step}.csv`, `NE_GATES.txt`.

## 0. Verdict first

> **The scenario exists and needed no building.** `MASHED_NO_ELIM` — a registered default-OFF
> measurement control (`TrackRenderer.cpp:91-97`, D3 2026-10-03) — suppresses elimination while
> leaving `round_mode_` on. With it: all three AI cars stepped on **100%** of frames, the committed
> baselines **reproduce exactly**, and branch 2 **fires 18 times**.
>
> **`W-TOOK` PASS. `W-NOREG-E` PASS. `W-NOREG-B` PASS (unchanged). `W-DET` PASS.**
>
> **But `W-NOREG` is BLIND:** **0 of 18** firings fall inside the scored window. The criteria
> instrument frames 1-220; branch 2 fires at **643-660**. The passes are real but they are **not
> evidence the wiring is harmless**.

| arm | round | NO_ELIM | wire | frames | reached v1/v2/v3 | fires |
|---|---|---|---|---:|---|---|
| `NE_base` | on | — | off | 13,858 | — | — |
| `NE_off` | on | **on** | off | 14,218 | — | — |
| `NE_on` | on | **on** | **on** | 14,218 | **14,218 / 14,218 / 14,218** | **18 (v2)** |

## 1. The baselines survive — which is what the decision required

| | committed baseline | `NE_base` | `NE_off` | `NE_on` |
|---|---|---|---|---|
| (e) | 3/3 PASS | **3/3 PASS** | **3/3 PASS** | **3/3 PASS** |
| (b) v1 / v3 | PASS | PASS | PASS | PASS |
| (b) v2 | 5 bands: 39 / 75 / 113 / 5.5 / 44.5 | **identical** | **identical** | **identical** |

So **`MASHED_NO_ELIM` does not move (b) or (e)** (`NE_base` ≡ `NE_off` on every digit), and
**neither does the wiring** (`NE_off` ≡ `NE_on`). The constraint "keeping the current baselines"
is satisfied: this configuration scores against the committed references without re-baselining.

Note `NE_base` and `NE_off` run different frame counts (13,858 vs 14,218), so `NO_ELIM` **does**
change the race — it just does not change the scored statistics.

## 2. `W-TOOK` PASS

`NE_off` (`d0119dcd`) and `NE_on` (`3421aced`) hash differently. The wiring changes the run. Branch
2 fires **18 times on v2**, frames **643-660**, one contiguous episode, LOS passing on all 18.

## 3. The blindness, stated plainly because the gate colour hides it

`ai_ctrl_window.py` scores car `v`'s calls `[i0, i0+220)`. For v2, `i0 = 0`, so the window spans
frames **1-220**. The firings are at **643-660**. **0 of 18 are inside.**

So `W-NOREG-E`/`-B` passing means *"the first 220 calls are unchanged"*, which is true and
uninformative about a branch that fires at frame 643. **This was predicted before the run** —
`RESULT_WIRE2.md` §3a found the same non-overlap in the no-round configuration (firings 558-660,
window 0-219) — and `NO_ELIM` did not move the firings earlier.

**A green `W-NOREG` here must not be reported as "the wiring does not regress (b)/(e)".** The
honest statement is: *the wiring does not regress the first 220 calls, and its actual effects fall
outside every window the project currently scores.*

## 4. What is established

- A configuration exists where **round mode is on, cars stay alive, the committed baselines
  reproduce exactly, and branch 2 fires**. That was the open question.
- `W-TOOK`, `W-DET`, `W-NOREG-E`, `W-NOREG-B` all **PASS** — the full registered gate set for
  `PREREG_WIRE.md`, for the first time.
- The wiring is faithful, reached on 100% of frames for all three AI cars, and live.

## 5. What is NOT claimed

- **No C-level.** No behavioural diff against the original was run.
- **Not that the wiring is behaviourally harmless** — §3. The gates cannot see it.
- Not that 18 (v2) resembles the original's 31/4/29. Different car, different count, and the port
  fires in a single late episode where the original spreads firings through its window.
- `W-SHAPE` remains unverified per-frame: no capture records `ctrl[4]`'s entry value on a firing
  frame.
- Not that `MASHED_NO_ELIM` is faithful to the original. It is a **measurement control** that
  suppresses a real game mechanic; it makes the comparison possible, it does not make the race
  more correct.

## 6. Next

1. **Make a window that contains the firings.** `ai_ctrl_window.py`'s anchor is `i0` = first call
   with `c4 != 0`; a second, later-anchored window would let (b)/(e)-style statistics actually see
   branch 2. That is a scorer change and needs registering — it touches how criteria are measured.
2. `GF1-CALLWISE` is now genuinely reachable: both sides fire, and `SCOPE_CALLWISE.md` §2's two
   per-call columns are the remaining work.
3. `MASHED_WIRE_B2` and `MASHED_SLOT_PLAYER` stay default-OFF.
