# RESULT — the later window is the WRONG instrument. Scored call-wise instead, branch 2's output pose **AGREES 82/82**.

Date 2026-10-08. Task (A) from the session kickoff ("the later-anchored window", RECOMMENDED).
**Scoping + re-scoring of committed captures only** — no game run, no build, nothing default-ON,
`original/` untouched. **No C-level.**

Raw: `verify/d3_elim_20261003/o_t3.msd.aistep.csv` (original, committed),
`CW_port.step.csv` / `NR_{on,off}.step.csv` (port, committed).
Tools: `re/tools/ai_fire_shape.py` (new), `re/tools/ai_ctrl_window.py` (`--skip` added).

## 0. Verdict first

> **Task (A) should NOT be done as specified.** Moving the window later does make it see the
> port's firings — but it moves it **off the original's**. The two sides' branch-2 firings are at
> **disjoint call offsets**: original **149-219**, port **558-660**. No single offset window
> contains both.
>
> **A later window also has no reference.** At `skip=558` only **2 of the original's 12
> observations** are full-length and live; 7 are short and 3 are frozen. The committed (b)
> envelope cannot be rebuilt there, and the 2 survivors disagree on every statistic.
>
> **Scored call-wise instead — on the calls where the branch actually fires — and it AGREES.**
> Original **64/64** firing calls and port **18/18** firing calls both produce
> `(c0, c1, c4, c5) = (0, 0, 0, 255)`. **82 of 82.** This is the first behavioural agreement
> measured for branch 2.

## 1. The firings are at disjoint offsets — this is what kills the window approach

Both sides anchor at `i0` = first call with `c4 != 0`.

| | original (`o_t3`) | port (`CW_port`) |
|---|---|---|
| v1 | **149-179** (31) | — (0) |
| v3 | **180-208** (29) | — (0) |
| v2 | **216-219** (4) | **643-660** (18) |
| total | **64** | **18** |

The original's firings are one contiguous episode handed off v1 → v3 → v2, entirely inside its
committed `[i0, i0+220)` window. The port's are one contiguous episode on a single car, **423
calls later**. The committed window sees the original's 64 and none of the port's; a window at
`skip=558` sees the port's and none of the original's. **The trade is total.**

## 2. Why no envelope exists at the later offset — measured

`ai_ctrl_window.py --skip 558 --n 103` over the original's five committed captures:

| capture | v1 | v2 | v3 |
|---|---|---|---|
| `o_spread1` | SHORT 0/103 | SHORT 0/103 | **DEGENERATE** |
| `o_spread2` | SHORT 0/103 | SHORT 0/103 | **DEGENERATE** |
| `orig_step_slots` | SHORT 0/103 | SHORT 0/103 | **DEGENERATE** |
| `o_spread3` | **live** | SHORT 73/103 | SHORT 73/103 |
| `o_inputs` | SHORT 79/103 | **live** | SHORT 79/103 |

**7 short, 3 degenerate, 2 live.** The degenerate rows hold every channel constant for the whole
window (`c0=0, c1=255, accel=0, brake=0`) — a car no longer being driven. Feeding those into a
min/max band would widen it to admit a frozen car, so `ai_ctrl_window.py` now **flags** them and
**refuses** short windows rather than scoring unequal spans.

The two live observations do not constrain anything — they disagree across the full range of
every statistic (`|steer|Med` **7** vs **52**, `steerD` **39** vs **79**, `c1_median` **0** vs
**38**).

**Why the original runs out of cars:** in each capture two of the three recorded cars stop being
stepped at the *same* offset (220 in three captures, 631/637 in two) while the third continues for
2,630-2,833 calls. `o_spread1/2/3` are three repeats of **identical** argv at the same `git_head`
and still split 220/220/2630 vs 2833/631/631 — consistent with memory
`round-level-outcomes-do-not-reproduce`. The committed `N=220` is the shortest such span, which is
exactly why the committed window stops there.

## 3. The call-wise measurement — the instrument that does work

`re/tools/ai_fire_shape.py` scores the calls where the branch fires, on each side, **in that
side's own regime**, with a control arm of the non-firing calls in the same window.

Fire predicate is schema-driven: the original's committed family tuple
`(ret14a70, ret14c30, ret150e0, ret16060) == (0,0,0,1)`, the port's `b2_ret == 1`. On `o_t3` the
literal branch-2 form (`ret148b0 != 0 and ret16060 != 0`) selects the **same 64 calls**, so the
two definitions coincide and the ambiguity flagged in `SCOPE_CALLWISE.md` §1 does not bite here.

### Output pose on firing calls

| | n | `c0` | `c1` | `c4` | `c5` |
|---|---:|---|---|---|---|
| ORIGINAL v1 | 31 | 0 | 0 | 0 | **255** |
| ORIGINAL v3 | 29 | 0 | 0 | 0 | **255** |
| ORIGINAL v2 | 4 | 0 | 0 | 0 | **255** |
| PORT v2 | 18 | 0 | 0 | 0 | **255** |

Unanimous on every channel on every call, **82/82**. That is exactly the documented effect of the
branch — `ctrl[5] = 0xff`, `ctrl[0] = ctrl[1] = 0`, early return (`RESULT_WIRE2.md` §4).

### Control arm — the pose is not the generic one

Non-firing calls in the same 220-call window:

| | n | `c0` distinct | `c1` distinct | `c4` | `c5` |
|---|---:|---:|---:|---|---|
| ORIGINAL v1 | 189 | 13 | 21 | 255 on 140/189 | 0 on 173/189 |
| ORIGINAL v2 | 216 | 35 | 50 | 255 on 202/216 | 0 on 179/216 |
| ORIGINAL v3 | 191 | 27 | 70 | 255 on 156/191 | 0 on 154/191 |
| PORT v2 | 220 | 39 | 75 | 255 on 184/220 | 0 on 184/220 |

Off a firing call both sides steer across dozens of distinct values and hold accel at 255 with
brake released. The all-zero + full-brake pose is specific to firings on both sides.

## 4. W-SHAPE — ~~partially closed, on the original side, from committed data~~ CORRECTED

> **CORRECTION 2026-10-08 ([`RESULT_WSHAPE.md`](RESULT_WSHAPE.md) §2).** This section said the
> original's `c4_in = c5_in = 0` on all 64 firing calls meant "the original never fires on a call
> that entered with accel applied". Literally true, but it implies a selectivity that does not
> exist: `c4_in` and `c5_in` are **0 on all 5,318 rows of `o_t3`**, firing or not. The original
> zeroes `ctrl` before every call, exactly as the port does. So this section established **less
> than it claimed** — not "the branch fires only from a released-throttle state" but "the entry
> column is constant and carries no information on either side". `W-SHAPE` is **not** partially
> closed; it is **not closable by this route at all**. The `ctrl[4]` claim stands on the static
> transcription only.



`RESULT_WIRE2.md` §5 recorded `W-SHAPE` unverified because "no capture records `ctrl[4]`'s entry
value on a firing frame". **`o_t3` does** — it carries `c4_in` and `c5_in`.

On all 64 original firing calls: **`c4_in = 0` and `c5_in = 0`.** So the original never fires on a
call that entered with accel applied, and the capture therefore **cannot** distinguish "the branch
zeroes `c4`" from "`c4` was already 0". Consistent with the branch not writing `ctrl[4]` at all.

**The port side of W-SHAPE remains open:** `CW_port.step.csv` and `NR_*.step.csv` carry no
`c4_in`/`c5_in`. Closing it is one stepdump column pair, the same shape of work
`SCOPE_CALLWISE.md` §2 specified for `b2_ret`.

## 5. The later window, scored anyway — what it does and does not show

For completeness, `[i0+558, i0+661)` on the port's two arms (`NR_off` vs `NR_on`, no-round):

| car | `NR_off` | `NR_on` |
|---|---|---|
| v1 | steerD=63, accel 255 on 92% | **steerD=1, accel=0, brake=255 on 100%** |
| v2 | steerD=53, accel=255 | steerD=53, accel=255 — identical |
| v3 | steerD=19, accel=255 | steerD=19, accel=255 — identical |

So the window **is** no longer blind: it resolves the wiring's effect on v1 completely, and
confirms v2/v3 are untouched. **But this is a port-vs-port regression reading only** — it has no
original reference (§2), so it cannot be a (b) verdict.

**Caution on the `degenerate` flag:** it fires on `NR_on` v1 here. Constancy has **two** causes in
these captures — a car that is not being driven, and a car the branch has pegged. The flag detects
constancy, not the cause. Do not read `NR_on` v1's flag as "undriven".

## 6. What is NOT claimed

- **No C-level.** Re-scoring committed captures is not a `diff-original` Frida diff. The pose
  agreement is a static comparison of two existing captures, not a behavioural diff.
- **Not that branch 2 is correct.** Its *output pose* agrees on every firing call. Its **timing**
  does not — offsets 149-219 vs 643-660 — and its **distribution across cars** does not (original
  31/4/29 over three cars, port 18 on one). Both of those remain open and both are downstream of
  `D-11073`.
- **Not that 64 vs 18 is agreement.** Different counts, different regimes: the original ran a
  QuickRace, the port ran `MASHED_NO_ELIM`, which suppresses a real mechanic.
- **Not that the committed (b)/(e) criteria were re-baselined.** They are untouched. `--skip`
  defaults to 0 and skip=0 output is byte-identical to `HEAD` (checked by diff against
  `git show HEAD:re/tools/ai_ctrl_window.py` over four captures).
- The entry-value finding is **original-side only** (§4).

## 7. Next

1. **Do not pursue a later-anchored (b) window.** §1-§2. If a late original reference is ever
   wanted it needs **new captures**, ~6-10 repeats of the `o_spread` recipe harvesting the one
   long-lived car per run — and in 3 of the 5 existing runs that car is frozen, so the observed
   yield is **2 usable observations per 5 runs**. Cost is poor against what §3 delivers for free.
2. ~~**Close the port side of W-SHAPE** — add `c4_in`/`c5_in` to the port stepdump.~~
   **DONE 2026-10-08, answer NEGATIVE** ([`RESULT_WSHAPE.md`](RESULT_WSHAPE.md)). The columns were
   added and the capture taken; `WS-LIVE` **FAILS** because the entry pose is uniformly 0 on both
   sides. The route is closed, not pending.
3. **Timing and car-distribution stay blocked on `D-11073`.** The 423-call offset between the two
   firing episodes is the `i0` anchor split (`RESULT_CALLWISE2.md` §2) — the same root.
4. `MASHED_WIRE_B2`, `MASHED_SLOT_PLAYER`, `MASHED_SLOTSTATE_SEED` stay default-OFF.
