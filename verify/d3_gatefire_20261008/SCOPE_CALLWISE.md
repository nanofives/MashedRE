# SCOPE — what `GF1-CALLWISE` actually needs. It is SMALL, and I previously said otherwise.

Date 2026-10-08. Scoping only — **no code written, nothing run beyond re-scoring committed
captures.** `original/` untouched. No C-level.

## 0. Verdict first, and it corrects me

> **`RESULT_CALLWISE.md` §2 said the blocker was that "the port is driven by a different race
> construction" and called `GF1-CALLWISE` "a harness task". That was WRONG.**
>
> The comparison window is **call-indexed and self-anchoring**, not clock- or construction-based:
> `ai_ctrl_window.py:25` takes car `v`'s calls `[i0, i0+220)` with `i0` = the first call where
> `c4 != 0`. The project **already** compares both sides this way — it is how criterion (b) is
> scored, and `PREREG_STEP2.md:10` records "median call index **109.5 on both sides**".
>
> **The original side is already captured, per-call, and committed.** The remaining work is
> **one port-side column pair plus a scorer**.

## 1. The original side is DONE — verified by re-scoring it here

`verify/d3_elim_20261003/o_t3.msd.aistep.csv`, 5,318 rows, cars 1-3, `i0 = 683` on all three.
Scoring the committed family definition — the tuple
`(ret14a70, ret14c30, ret150e0, ret16060) == (0,0,0,1)` — over each car's 220-call window:

| car | branch-2 firings |
|---|---:|
| v1 | **31** |
| v2 | **4** |
| v3 | **29** |
| **total** | **64** |

That reproduces `RESULT_STEP2.md:108`'s **64** exactly, and `:165-167` records the same 64 on
`o_t1`/`o_t2`/`o_t3` independently. **This is the reference `GF1-CALLWISE` pairs against.**

**Schema caution, found the hard way.** The three captures have **three different schemas** and
only `o_t3` carries `ret148b0`:

| capture | cols | branch-2 columns present |
|---|---:|---|
| `o_t1` | 36 | `ret14a70`, `ret14c30` only |
| `o_t2` | 38 | `+ ret150e0`, `ret16060` |
| `o_t3` | **39** | `+ ret148b0` — **the only one** |

A `grep -l ret148b0` matches `o_t2` on the *substring* `ret16060`-adjacent text, not the column;
`o_t2` has **zero** occurrences. Use `o_t3`, and use the **tuple**, not `ret148b0 != 0` — the
latter counts the `-1` "not evaluated" sentinel and yields **176**, not 64. (I made exactly that
mistake while scoping; the committed definition is the one to use.)

## 2. What the port is missing — one column pair

The port's `MASHED_AI_STEPDUMP` is already **per-ControlStep-call** and already shares the first
32 columns with the original's aistep schema (`det_prefix.py --common-cols` exists for this).

What it does **not** carry is branch 2's per-call outcome. The `w_reach` / `w_ret` / `w_fire`
counters added in `RESULT_WIRE.md` are **cumulative per car** and live in the **gates** dump — the
wrong granularity and the wrong file for a call-wise pairing.

**The work:**

1. Add two per-call columns to `AiStepDump`: the branch-2 predicate return and the LOS return, i.e.
   the port's `ret148b0` / `ret16060` analogues, written by the wired site in `ControlStep` into a
   per-car scratch the stepdump reads in the same frame.
2. Score the port's 220-call window with the **same** `i0 = first c4 != 0` anchor and the **same**
   tuple, and compare to **31 / 4 / 29**.

That is one build and one run, not a harness.

## 3. The one real obstacle, and it is not construction

`ControlStep`'s visit set is car-alive gated and **rule-independent** (`RESULT_CALLWISE.md` §1):
v1 **1,446**, v2 **3,177**, v3 **13,858** calls. The window needs **220 per car** — all three
clear that, so a window exists on every car. **Enough calls is not the problem.**

The real risk is that the port's window covers a different *phase* of the race than the original's
`i0 = 683`, because the two races diverge. That is a **comparability caveat to report with the
result**, the same one criterion (b) already lives with — not a reason the measurement cannot be
taken.

## 4. What this scope does NOT resolve

- Whether branch 2 fires at all on the port once the columns exist. `RESULT_WIRE.md` measured
  **0 firings in 18,481 calls**, so the likely outcome is **0 vs 64** — a real, reportable
  comparison, and the first genuinely like-for-like one in this lane.
- The car-alive anomaly (v1 alive 10% of frames). Still unmeasured against the original and still
  the better next question (`RESULT_CALLWISE.md` §4 item 1).
- Any C-level. A call-count comparison is not a behavioural diff.

## 5. Recommended order

1. **Measure car-alive on the original first.** If the port eliminates cars the original keeps
   alive, the port's window covers a different race and `GF1-CALLWISE` would be comparing phases,
   not implementations. Cheap: `orig_rampwatch.py` already does this shape of poll.
2. Then the two columns + scorer (§2).
3. Report `0 vs 64` — or whatever it is — **with** the phase caveat from §3 stated, not buried.
