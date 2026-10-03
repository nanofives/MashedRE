# PRE-REGISTRATION — D3 offline, U-9183 (position-matched curvature) and U-9182 (c0 branch split)

Written and committed **UNRUN**, before any comparison statistic was computed.
Offline session: no game run, no build, no edit under `mashedmod/src`. Evidence is the
committed captures only.

Captures:

| side | file | rows | provenance |
|---|---|---:|---|
| ORIGINAL | `verify/d3_modes37_20261002/o1.msd.aistep.csv` | 5463 | `scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --statediff-aistep --hold 60`, 3632 frames, cars [1,2,3], `joinMiss=0` |
| PORT | `verify/d3_rebase_20261002/r1.csv` | 4193 | `RESULT_STEP1.md` run `r1`; `r1`/`r2`/`r3` byte-identical on every column over their common prefix |

Scored window, unchanged from `re/tools/ai_ctrl_window.py`: per car `v`, the first **220**
calls starting at the first call with `c4 != 0`.

---

## PART 0 — transcription of `FUN_00443440` (the curvature function), by RVA

Read read-only from pool clone via `re/tools/decomp_pc.py` and `re/tools/disasm_fn.py`
against `original/MASHED.exe.unpatched`. The master Ghidra project was not opened.

**Call site**, `FUN_00416250` (`AiStandalone.cpp:837`):

```
0x00416284  call 0x46d4a0              ; FUN_0046d4a0(&p, v) -> per-vehicle record
0x0041628d  mov ecx,[eax+0x30]         ; ownX
0x00416293  mov [esp+0x54],ecx         ;   -> param_2[0]
0x00416297  mov edx,[eax+0x38]         ; ownZ
0x004162ac  mov [esp+0x6c],edx         ;   -> param_2[1]
0x0041629a  push 0                     ; param_5 = 0
0x0041629c  lea eax,[esp+0x34] / push  ; param_4 = &out
0x004162a1  push 0x41200000            ; param_3 = 10.0f  <-- LITERAL
0x004162a6  lea ecx,[esp+0x60] / push  ; param_2 = &{ownX, ownZ}
0x004162ab  push edi                   ; param_1 = spline  ([ebp+8], pushed at 0x0041877d)
0x004162b0  call 0x443440
0x004162b7  fld  [esp+0x44]            ; the OUT float, not the return value
0x004162be  fcomp [0x5cd09c]           ; 180.0
0x004162cb  fld  [0x5ccac4] / fsub     ; c = 360 - c when 180 < c   <-- the logged `curv`
```

**The complete input set of `FUN_00443440` under this call site:**

| input | where read | RVA |
|---|---|---|
| spline point count | `*(int *)(param_1 + 0x200)` | `0x0044348c` |
| spline point X[i] | `*(float *)(param_1 + i*8)` | `0x004434a5` |
| spline point Z[i] | `*(float *)(param_1 + i*8 + 4)` | `0x004434ae` |
| `ownX` | `param_2[0]` (`fsub [edi]`) | `0x004434a8` |
| `ownZ` | `param_2[1]` (`fsub [edi+4]`) | `0x004434b2` |
| look-ahead distance | `param_3`, pushed as the **literal** `0x41200000` = 10.0f | `0x004162a1` |
| draw flag | `param_5`, pushed as the **literal** `0` | `0x0041629a` |

`param_5 == 0` makes the only two other reads dead: the `_DAT_007f1a58` / `_DAT_007f1a5c`
pair is read only under `if (param_5 == 1)`, and the `FUN_004671a0` / `FUN_004b55a0`
debug-draw block only under `if (param_5 != 0)`. The body reads **no** vehicle record, **no**
clock, **no** speed and **no** history global.

**Therefore, under this call site, the logged `curv` is a pure function of
`(spline contents, ownX, ownZ)`.** This is the load-bearing claim of Part A and it is
checked, not assumed (KA-A1 / KA-A2 below).

**ANSWER to the speed/look-ahead question the kickoff asked to check:** the look-ahead
distance is `param_3`, pushed as a literal at `0x004162a1` and never scaled. It does
**not** depend on speed. Consequently **matched position does NOT additionally require
matched speed** for `FUN_00443440`. The speed at the matched rows is reported anyway as a
declared confound check: curv agreeing at matched position *despite* different speed is
independent confirmation of the no-speed-input transcription.

**Spline identity.** The original logs the raw `spline` pointer argument. It takes exactly
two values, `0x801aa0` (4712 calls) and `0x801ca4` (751 calls) — race-line bank index 0 and
1, `0x801aa0 + idx*0x204` (`kSplineRace` / `kSplineStride`, `0x004185fd` / `0x00418759`).
The PORT writes a **literal `0`** in that column (`TrackRenderer.cpp:3787`, the `,0,`
between `block` and `c0`), so it carries no information and must not be read. Both sides do
log `ai_type` (`0x0089a4cc + v*0x74`) and `ai_spline_idx` (`0x0089a4d0 + v*0x74`), which are
the two fields the original's own bank pick reads at `0x0041860a` / `0x004185f1`.

---

## PART A — U-9183: position-matched curvature

### A.0 Gate replaced — stated prominently

> **The kickoff's known-answer check ("recompute curv from the logged inputs on BOTH sides
> via the transcription, and it must reproduce the logged value") CANNOT be run offline and
> is REPLACED.**
>
> Reason: by Part 0 the inputs include the spline point array and count at
> `0x801aa0 + idx*0x204`. That is runtime memory. **No committed capture in this repo
> carries it** (`git ls-files | grep -i spline` returns only two source files and one
> analysis note; no spline point dump exists). Re-deriving it from the track assets would
> be a second, unverified transcription, and acquiring it live is forbidden by this
> session's no-run constraint.
>
> It is replaced by **KA-A1 + KA-A2**, which test the same load-bearing claim — that the
> logged `curv` is reproduced by position alone — directly on both sides' logged data.
> Both must pass before any cross-side number is quoted.

### A.1 KA-A1 (exact, no tolerance) — determinism at identical position

On each side independently, take every pair of rows of the **same car** with
**bit-identical** `own_x` AND `own_z` (string equality of the logged field) AND equal
`ai_spline_idx`. Their `curv` must be **string-equal**.

- PASS: zero violating pairs on BOTH sides, with at least 100 such pairs per side.
- FAIL: any violating pair ⇒ the purity transcription is wrong ⇒ **STOP**, report, and do
  not quote a cross-side number.
- If fewer than 100 pairs exist on either side, KA-A1 is UNDERPOWERED; say so and fall back
  to KA-A2 alone, declared.

### A.2 KA-A2 (graded) — single-valuedness within the matching radius

On each side independently, over all same-car, same-`ai_spline_idx` row pairs with
Euclidean `(own_x, own_z)` distance `<= R`:

- PASS: median `|dcurv| <= 2.0` degrees AND 90th percentile `|dcurv| <= 10.0` degrees.
- FAIL ⇒ **STOP** as above.

### A.3 The matching key

`R = 0.12` world units. **Formula, fixed before the test:** the median per-call Euclidean
displacement of the **original** inside its own scored window, which measured
`0.1120 / 0.1112 / 0.1213` for cars 1 / 2 / 3, rounded to two decimals. Rationale: the
reference's own sampling resolution — two positions closer than one of the original's own
AI-step displacements are not distinguishable at the capture's resolution. (Displacement is
the only quantity inspected before writing this file; no `curv` statistic was computed.)

For each **port** row in car `v`'s 220-call window, the match is the row from the
**original's same car `v`**, anywhere in the original capture, with the **same
`ai_spline_idx`**, minimising Euclidean distance in `(own_x, own_z)`. Keep the match only
if that distance `<= R`. Report the kept fraction per car and how many matches come from
outside the original's own scored window.

Every input of `FUN_00443440` is then either matched by construction (`ai_spline_idx`,
`ownX`, `ownZ`) or a pushed literal (`param_3`, `param_5`). The only input left free is the
**contents** of the spline array itself.

### A.4 Decision rule

Statistic: `d = |curv_port - curv_orig|` over the matched pairs, per car.

- **U-9183 CLOSES, verdict "position/speed artefact"**, iff on **all three cars**:
  `n_matched >= 50`, `median(d) <= 2.0` degrees, and `p90(d) <= 10.0` degrees.
- **U-9183 DOES NOT CLOSE** otherwise. In that case name the first input of
  `FUN_00443440` that diverges at matched position, with its RVA, from the table in Part 0.
- If any car has `n_matched < 50`, that car is **UNDERPOWERED**; report it and do **not**
  claim closure from the other cars.

The 2.0 / 10.0 degree band is an order of magnitude tighter than the effect under test
(`mode==0` medians `52.04 / 52.81 / 50.08` vs `6.61 / 9.05 / 23.64`), so it can neither
manufacture nor hide it.

---

## PART B — U-9182: the `c0_distinct = 6` on car 1 (band floor 13)

### B.1 Transcription — every site that writes `c0` (`ctrl[0]`)

Port file `mashedmod/src/mashed_re/Ai/AiStandalone.cpp`; original RVAs from
`original/MASHED.exe.unpatched`.

| id | what | port | original RVA |
|---|---|---|---|
| **ZERO** | `ctrl[0] = 0` before the call | `AiStandalone.cpp:1604` (`VehicleStep`) | `FUN_00418560` prologue |
| **MAG** | LO band magnitude: `ctrl[0] = ftol(m)`, `m = err*speed*0.0030034`, `*= curv*0.05` when `mode==0 && curv>20`, clamped to 255 | `:878` | `0x00416697` (`mov byte [ebx],al`); amplify `0x0041665e..0x00416679`; clamp `0x0041667b..0x0041668a`; `ftol` `call 0x4a2c48` |
| **CTRHI** | HI band counter-steer: `ctrl[0] = ftol((200-el)*0.005*stored)` | `:908` | `0x00416758` (`mov byte [ebx],al`) |
| **FF30** | `err < 180 && err > 30` ⇒ `ctrl[0] = 0xff` | `:919` | `0x00416860` (`mov byte [ebx],0xff`), gated `0x0041683e` (`fcom [0x5cd09c]`=180.0) and `0x0041684b` (`fcom [0x5cc72c]`=30.0) |
| **M5** | behaviour mode 5 tail | `:929` | `FUN_00416250` mode-5 tail |
| **M2** | behaviour mode 2 tail | `:942`/`:945` | `FUN_00416250` mode-2 tail |
| **OVR** | override replay overwrites `ctrl[0]` while the timer runs | `:1627` | `0x00418790..0x00418844` |

The two steer bands are independent `if`s on the same `err`, so at most one runs.
**FF30 runs after MAG/CTRHI and overwrites unconditionally when its gate holds.**

### B.2 The branch-inference rule for the ORIGINAL (stated up front)

The original's branch is not logged; it is inferred from the two steer-history globals the
capture does log at `FUN_00416250`'s `onLeave`:
`hist_d8` = `[0x008032d8 + v*0x14]`, `hist_dc` = `[0x008032dc + v*0x14]`.

From the band bodies: the LO band writes `hist_d8 = 360.0` (`0x004165cc`) and
`hist_dc = err` (`0x004165f7`); the HI band writes `hist_dc = 0.0` (`0x004166db`) and
`hist_d8 = err` (`0x0041670c`). Both globals persist across calls, so:

```
I-RULE (identical on both sides, applied to the logged row):
  band = LO     if hist_d8 == 360.0 and 0 <= hist_dc <  180 ;  err := hist_dc
  band = HI     if hist_dc ==   0.0 and 180 < hist_d8 <= 360 ;  err := hist_d8
  band = AMBIG  otherwise                       (neither band ran this call, or both
                                                 patterns hold and the row is stale)
```

Branch label, given `band` and `err` and the logged `c0`:

```
  AMBIG                      -> AMBIG
  LO and err <= 1.0          -> DEAD    (deadband 0x004165f1 / _DAT_005cc320; no c0 write)
  LO and err  > 30.0         -> FF30    (c0 must be 255)
  LO and 1.0 < err <= 30.0 and c0 != 0 -> MAG
  LO and 1.0 < err <= 30.0 and c0 == 0 -> CTR1   (the LO else-branch wrote c1, not c0)
  HI and c0 != 0             -> CTRHI
  HI and c0 == 0             -> MAGHI   (the HI gate was taken; it writes c1, not c0)
```

Rows with `ai_override != 0` (`[0x0089a4fc + v*0x74]`, logged as column `ai_override`) are
excluded from the known-answer checks and reported separately, because **OVR** can
overwrite `c0` after every site above.

### B.3 Known-answer checks (both sides, both must pass)

- **KA-B1 (exact).** Every LO-band row with `err > 30.0` and `ai_override == 0` must have
  `c0 == 255`. Zero violations required on BOTH sides.
- **KA-B2 (exact, reproduces the logged `c0` arithmetically).** For every LO-band row with
  `1.0 < err <= 30.0`, `ai_override == 0`, and `c0 not in {0}`, the recomputation
  `c0_hat = clamp(trunc(min(255, err * speed * 0.0030034 * (curv*0.05 if ai_mode==0 and curv>20 else 1))), 0, 255)`
  (constants `_DAT_005cd0e8`, `_DAT_005cc9a0`, `_DAT_005ccd6c`, `_DAT_005cd04c`;
  `speed = rec_9e4`; truncation = `call 0x4a2c48`) must equal the logged `c0`.
  PASS: `>= 95 %` of such rows exact-equal on BOTH sides, with `>= 20` rows per side.
  FAIL ⇒ **STOP**; the rule does not reproduce the logged `c0` and the split is not
  trustworthy.
  The 95 % (not 100 %) allowance is declared up front and is for float rounding at the
  truncation boundary only; any larger shortfall is a FAIL.

### B.4 The reported split and the decision

Split **car 1's 220-call window on both sides** by the label of B.2; report counts, the
distinct `c0` set contributed by each label, and the gate inputs (`err`, `speed`, `curv`,
`ai_mode`) at those calls.

- Name the branch the port **never** takes and the branch it **always** takes, and state
  **why**, from the gate inputs at those calls (not from the label counts alone).
- U-9182 is **RESOLVED** iff the split identifies a branch whose per-side call count differs
  such that the port's `c0_distinct` floor is accounted for; otherwise U-9182 is **AMENDED**
  with what the split did establish and what is still missing.

---

## Rules

NO-GUESSING: every claim cites an RVA or `file:line`. Unproven claims are marked
`[UNCERTAIN]`. No gate in this file may be amended after a run. If a gate fails, the run
STOPS and the failure is reported.
