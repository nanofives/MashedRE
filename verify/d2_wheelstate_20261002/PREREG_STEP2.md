# D2 attempt 20 — PRE-REGISTRATION, STEP 2: the per-wheel state-machine comparison

Committed **UNRUN**, before the port is rebuilt and before any capture is taken.
Closes the question `RESULT_STEP1.md` §6 leaves open: STEP 1 found **no transcription
defect**, so both of U-9179's remaining arms (`:163`'s latch and `:170`'s `key == -1`) are
reachable and both are driven by the **same** upstream quantity — whether the classifier
`0x0046cc40` filled that wheel's record this call (`RESULT_STEP1.md` §5).

Every metric reported from this step carries `n`, the median speed, and `d` with
`L = 0` and `R` taken from **each capture's own `+0xbf8` marker** (`0x0046d7a2`;
first frame with `+0xbf8 != 0`), per `a18_gate.py:127-130`.

---

## 1 What is measured, and the channel on each side

### 1.1 PORT — extend `MASHED_D2SINK`, do not rebuild it

Two new `MarkWheel` tags in `WheelContactSolver.cpp`, both inside the existing probe and
both behind a **second** env var so the attempt-19 channel's volume is unchanged:

| tag | site | wheel | `a` | `b` | `c` | `d` |
|---|---|---|---|---|---|---|
| `wcs_ent` | top of `0x0046f6c0`, **before** the init loop (`:91`) | 0..3 | `state` | `fv` = `+0x194+w*0xc4` | `key` = `+0x1ec+w*0xc4` | `0` |
| `wcs_sm`  | end of each state-machine iteration (`:197`, before `:198`'s stride) | 0..3 | `stateIn` | `fv` | `key` | `stateOut` |

`key` is an integer field (`-1` = empty) and is emitted as `(float)(int)key`.
`stateIn` is captured into a local at `:165` before any write.
Channel control: `MASHED_D2SINK_SM` must ALSO be set. With `MASHED_D2SINK` alone the log is
byte-for-byte attempt 19's channel; with neither, nothing is written. **One `if` per site
when unset. No record field is touched, so `NEW = 0` for the port's behaviour.**

### 1.2 ORIGINAL — a new entry-hook-only probe, `--wheelstate-probe`

ENTRY HOOKS ONLY (memory `frida-interceptor-is-entry-only`). Grepped first (memory
`grep-the-harness-for-the-rva-before-writing-a-probe`): `0x0046f6c0` is already
`FP_WHEEL`, `--fixup-probe` site 3, but that sample carries `+0x9b0/+0x9e0/+0x9e4` and the
18 contact slots and **not one per-wheel field**, so it cannot answer this. The new probe
adds the per-wheel columns and nothing else.

| site | RVA | filter | columns |
|---|---|---|---|
| 0 | `0x0046f6c0` solver entry | row records `edi`; reducer drops `edi != REC` | per wheel w=0..3: `state` `+0x198+w*0xc4`, `fv` `+0x194+w*0xc4`, `key` `+0x1ec+w*0xc4` (S32); plus `+0x9e4` speed, `+0x9e0`, `+0xbf8`, `+0x9b0/b4/b8` |
| 1 | `0x00467650` A6a entry | `esi == REC` (as `latBracketArm` does) | frame marker only; gives every row a frame index |

Rate: the original runs 2 substeps per frame (`RESULT.md` §2 of attempt 19), so site 0
fires ~2/frame ≈ 70/s — two orders under the ~1000/s destabilise floor. **COUNT-FIRST
safety gate:** `--wheelstate-probe-count` arms the two hooks with the callback body reduced
to `n++`, prints the rates, and takes no sample. The sampling run is taken **only** if
site 0 measures `< 400/s`.

### 1.3 Why an entry hook is sufficient, stated mechanically

At the entry of solver call `N` the init loop has not yet run, so the row carries:
`state[w]` = the state **after** call `N-1`'s full body (state machine, then the 4-wheel
drop `0x0046fbb2`, then the `bVar16 == 3` promotion `0x0046fd17`), and `fv[w]` / `key[w]`
= exactly what call `N-1`'s classifier left (`RESULT_STEP1.md` §5: the only writers are the
init loop, the classifier, and the promotion's wheel-0 copy). Therefore a **consecutive
pair** of site-0 rows carries the complete input/output record of call `N-1`:

```
in:  state[w] from row N-1          (= the state machine's stateIn)
in:  fv[w], key[w] from row N       (= the classifier's output for call N-1)
out: state[w] from row N            (= post state machine + drop + promotion)
```

`bVar4` is a function of those same inputs (`RESULT_STEP1.md` §2), so it is derived, not
read.

## 2 The replay, and the two known-answer gates

`re/tools/statediff/a20_wheelstate.py` implements the transcribed rule **verbatim from
`RESULT_STEP1.md` §2-§4** and nothing else — no fitted constant, no tolerance, integer
state values only:

```
bVar4  = (count_w(state[w]==1 and fv[w] >= 0.02) != 0) and (count_w(state[w]==1 and fv[w] <= -0.005) == 0)
per w:  if state[w] != 0:  state'[w] = 0 if ((fv[w] > 0.02 and bVar4) or key[w] == -1) else 1
        else:              state'[w] = 2 if (-2.0 < fv[w] <= 0.0) else 0
bVar16 = count_w(state'[w] == 1)
if bVar16 == 4: sel = argmax_w |fv[w]| with the 0x0046fae6..0x0046fbb2 tie-break; state'[sel] = 0; bVar16 = 3
if bVar16 == 3: the single w with state'[w] != 1 becomes 1      (0x0046fc09..0x0046fd29)
```

**Gate KA-P (the replay is validated where ground truth exists).** On the PORT, where both
`wcs_ent` and `wcs_sm` are logged, `replay(wcs_ent[N-1], wcs_ent[N])` must reproduce
`wcs_ent[N]`'s four states on **>= 99 %** of consecutive pairs in the window, AND the
replay's pre-drop `state'` must equal `wcs_sm[N-1]`'s four `stateOut` values on
**>= 99.5 %** of pairs. This validates the replay code itself against a directly measured
state machine before it is applied to the original.

**Gate KA-O (the replay reproduces the ORIGINAL's own states).** On the ORIGINAL,
`replay(row[N-1], row[N])` must reproduce `row[N]`'s four states on **>= 90 %** of
consecutive site-0 pairs over the full capture. A failure here means a state writer outside
`0x0046f6c0` is on the path (`RESULT_STEP1.md` §5's `[UNCERTAIN]` `0x0046bb47` is the named
candidate) and **STOPS the step**: the original's inputs would not be readable this way and
the finding would be reported unfixed.

**Gate CV (coverage, counted not assumed).** Reported before any verdict: site-0 rows,
site-1 rows, rows dropped by the `edi` filter, consecutive pairs usable in the window,
rows with any unparsed field, and the `nsub` histogram (site-0 rows between two site-1
rows). Zero rows in the window, or `nsub` not concentrated on `2` (ORIG) / `3` (PORT),
STOPS the step.

**Gate CH (the new channel is inert).** One PORT run with `MASHED_D2SINK` set and
`MASHED_D2SINK_SM` **unset** must produce a sink log whose non-`wcs_ent`/`wcs_sm` lines are
byte-identical to a run with both set, and `motion_diag.log` must be byte-identical between
`MASHED_D2SINK_SM` set and the fully unarmed default build on all shared lines.

**Gate EV (the window is on-regime).** `n >= 25` frames at `d = 222..250` on each side,
and the port's median speed within 25 % of **633.2** (attempt 19's figure for the same
window). Median frame index reported via `a8_medframe.py` discipline, per §26.10 — a band
whose median frame indices are far apart is reported `!!` OFF-REGIME and **not read**
(memory `a-band-scored-off-regime-is-not-a-measurement`).

## 3 The decision rule — registered now, honoured as written

Scored at matched `d` over `d = 222..250` (the carrier window) and `d = 0..100`
(the launch control window, where attempt 19 measured the two sides to agree: `L = 0` at
0.19 %).

Define, per side and per `d`, from the state machine's own inputs:
- `K(d)` = number of wheels with `key != -1`
- `F(d)` = number of wheels with `-2.0 < fv <= 0.0`
- `S1(d)` = number of wheels with `stateOut == 1` (the gate's `bVar16` before the drop)

**Rule 1 (the naming rule).** The first of `K`, `F` in that order whose **median over the
window differs between the two sides by >= 1.0 whole wheel** names the diverging term.
`key` is tested first because it is the single field both of U-9179's arms share.
If `K` diverges, the named term is **the classifier's per-wheel yield** and the trace
continues upstream to `0x0046cc40`'s gates. If `K` agrees and `F` diverges, the named term
is **`fv` = the classifier's `depth`**, and the trace continues to `depth`'s producer.

**Rule 2 (no divergence in the inputs).** If neither `K` nor `F` diverges by >= 1.0 while
`S1` does, the state machine is being fed matching inputs and producing different outputs,
which contradicts STEP 1 — report that contradiction as the finding and **STOP**.

**Rule 3 (both agree).** If `K`, `F` and `S1` all agree within 1.0 at matched `d`, U-9179
is **not** reproduced on this arm; report it named and unfixed and do not fix anything.

**Rule 4 (the arm attribution, reported either way).** The port's `wcs_sm` rows are
classified into the four exclusive arms by `(stateIn, fv, key, bVar4, stateOut)` —
`A-demote-bVar4`, `A-demote-key`, `A-hold`, `B-latch`, `B-stay` — and the counts are
reported with `n`. This is a *description*, not a decision input.

## 4 What STEP 3 may change, pre-registered

A fix is authored **only** if Rule 1 fires **and** the named term's first diverging
producer is confirmed by RVA on the running original. The fix must be
**one faithful body at the RVA**, in a TU already in both `exe_sources.rsp` and
`asi_sources.rsp` or the existing body at its own RVA — never a parallel copy — with
`dual_copy` guard `NEW = 0`, and the promotion leg (`run_diff` path1 + `run_verify_hook`
path2, or `re/CONFIDENCE.md`'s non-repeatable-function clause). **No knob, no clamp, no
fitted constant, no threshold chosen to close a branch.** If the named producer is the
already-recorded U-9154 DEVIATION at `WheelContactSolver.cpp:110-123` (the standalone's
substitute wheel positions), that is a **stated deviation, not a transcription defect**,
and the registered outcome is: report it as the confirmed root of U-9179, author a fix only
if the faithful path can be restored without a fitted constant, and otherwise report it
named and unfixed with the re-pickup condition.

The substep loop `0x00471106..0x00471141` is **NOT** ported in this step
(attempt 19 `RESULT.md` §3's measured reason: 26.03 % against the carrier's 84.33 %).
If STEP 3 lands and STEP 4's rule (a) passes, the loop may be ported as a **separately
pre-registered** step with its own promotion leg.

## 5 Commands (muted, own PID, tracked and reaped)

PORT (3 runs for STEP 4; 1 run with the SM channel for STEP 2):
```
MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12 MASHED_STEER_HOLD_AFTER=0
MASHED_WIN_POS=primary-bl MASHED_TITLE=<per-run>
MASHED_D2SINK=<path> MASHED_D2SINK_SM=1
py -3.12 re/tools/statediff/a8_run_port.py ...
```
ORIGINAL:
```
py -3.12 re/frida/scenario_launch.py --statediff-out verify/d2_wheelstate_20261002/orig_ws1.msd
    --statediff-drive --statediff-drive-late --statediff-steer 1 --hold 38
    --poke-ctrl-slots --wheelstate-probe
```
(`--track` omitted = engine track 0 = **Training**, the same arm as `orig_lb18.msd`;
memory `read-the-references-own-provenance-argv`.)

## 6 STEP 4, pre-registered unchanged from the session brief

(a) `wcs_drift` fires **0** times at `d = 222..250` on the port, and `T_post` within
attempt 18's bars (5.5560 / 8.6373); (b) `a8_launch.py` still PASSES; (c) recovery under
H1 (`>= 50 %` of the 400 post-trough frames `>= 100` AND median `>= 900`); (d) the three
D2 metrics, 3 runs, against the **UNCHANGED** `d81a8df6` bounds (slip 1500-2000
0.18855..0.19635, slip 2000-2600 0.24488..0.25487, driving-median 1904.70..1982.44) on the
§16.7 arm with `participants=1` confirmed from the game's own `MATCH-SEED` line and median
frames via `a8_medframe.py`.

Collateral closes the attempt either way: `collateral.py` with a floor, paired pre-fix vs
post-fix classified against the fix's write set, plus the cross-side banded leg at
matched `d`; outside-scope rows reported.
