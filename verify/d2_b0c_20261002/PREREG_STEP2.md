# PRE-REGISTRATION — D2 attempt 17, STEP 2: what `+0xb0c` is downstream of

Written and committed **before any run or reduction**. Nothing below is amendable.
If a gate fails, the attempt STOPS at that gate and the failure is reported as-is.

STEP 1 (the mechanical writer/reader listing and the transcribed expression) is already
complete and is recorded in `RESULT_STEP1.md`, committed in the same commit as this file.
STEP 1 used only static instruments (`findoffset.py`, a capstone folded-base sweep with a
known-answer self-check on `+0xbf8`, `disasm_va.py`, `memread.py`) and required no gate.

---

## 1 The question

`+0xb0c`'s producer is `A4 FUN_00470670` and its expression is transcribed byte-exact in
`RESULT_STEP1.md` §2:

```
speed == 0.0          ->  +0xb0c = 0            (0x0047072c, integer store of EBX)
otherwise             ->  +0xb0c = (1.0 - |dot| / speed) * speed      (0x00470724)
  dot   = (fwd.y*vel.y + fwd.x*vel.x) + fwd.z*vel.z      0x004706db..0x00470701
  |dot| = FCHS when dot < 0.0                            0x00470703..0x00470710
  1.0   = _DAT_005cc320 = 0x3f800000                     (read from the anchored exe)
  speed = +0x9e4                                         0x00470712 / 0x0047071e
```

The port computes the same expression (`VehicleControl.cpp:108-116`, exe;
`PhysicsChainHooks.cpp:296-303`, asi). Therefore `+0xb0c` **cannot diverge on its own
arithmetic**; it can only diverge because one of its SEVEN inputs diverges:

```
vel = (+0x9b0, +0x9b4, +0x9b8)      fwd = (+0x9d4, +0x9d8, +0x9dc)      speed = +0x9e4
```

STEP 2 asks: **which input, at which `d`.** It does not assume `+0xb0c` is a defect.

## 2 Instrument O — the ORIGINAL side

The `.msd` statediff capture is the **raw 0xd04 vehicle record** per frame
(`re/tools/statediff/FORMAT.md`), so every one of the seven inputs AND the stored
`+0xb0c` are already recorded, live, on the original, at every frame of three existing
captures. A new Frida hook would re-measure values the capture already holds.

The one thing the capture does not give is the writer's own phase: it is a render-tick
snapshot, and `vel` is integrated later in the same frame. **Gate KA decides whether the
snapshot phase is sound.** It is the brief's "recompute the stored value from the recorded
inputs on the original itself, and it must match before anything is trusted".

Captures used (all pre-existing, none produced by this attempt):

| capture | frames | provenance |
|---|---:|---|
| `verify/d2_writer_20261001/orig_bp1.msd` | 2334 | `orig_bp1.msd.provenance.json`, `git_head 15b31fc4` |
| `verify/d2_writer_20261001/orig_bp2.msd` | 2336 | `orig_bp2.msd.provenance.json` |
| `verify/d2_reopen_20260929/orig_solo3.msd` | 2333 | attempt 13/14 reference |

### GATE KA — known-answer self-check (original side). NOT AMENDABLE.

For each integer lag `lam` in `{-1, 0, +1}`, pair the stored `+0xb0c` at frame `n` with
the seven inputs at frame `n + lam`, recompute

```
pred = 0.0 if speed == 0.0 else (1.0 - |dot| / speed) * speed
```

in the ORIGINAL's association order, and score

```
rel = |pred - b0c| / max(|b0c|, 1e-3)        hit iff rel <= 1e-4
```

over every frame of the capture with `speed > 0`.

**KA PASSES** iff there exists a **single** `lam` whose hit fraction is `>= 0.99` on
**all three** captures. That `lam` is then fixed for the rest of STEP 2 and is reported.

**KA FAILS** otherwise. On failure the offline route is VOID, no number derived from it
may be reported as evidence, and STEP 2 must instead be re-run as a **live Frida entry
hook on `FUN_00470670`** (record base arrives in `EAX`, `0x00470679 mov edi, eax`),
filtered to `rec == 0x008815a0`, reading the seven inputs and the stale `+0xb0c` at entry,
with count-first arming and coverage counters, carrying the same KA gate across
consecutive calls. **The gate is not weakened in that case; it is re-run.**

Coverage, reported either way: frames scored, frames skipped for `speed == 0`, frames with
a non-finite input.

## 3 Instrument P — the PORT side

`MASHED_MOTION_DIAG` (already default-OFF, env-gated, `VehiclePhysicsRun.cpp:1200`) logs
`sp`, `horiz` and `b0c` but not `vel` or `fwd` componentwise. STEP 2 **appends two fields
to the END of that one line**:

```
vel=[%g,%g,%g] fwd=[%g,%g,%g]        <- F(r,0x9b0/0x9b4/0x9b8), F(r,0x9d4/0x9d8/0x9dc)
```

Appending at the end keeps every existing `name=value` token byte-identical, so
`a8_launch.py`'s `sp=` / `b14=[...]` regexes and `a8_medframe.py` are unaffected.
**No other source change is made before STEP 3.**

### GATE D-0 — the control. NOT AMENDABLE.

After the rebuild, one port run on the §16.7 arm (below) must reproduce attempt 15's
`verify/d2_writer_20261001/r1/motion_diag.log` **bit-identically on every pre-existing
field token** (`reseed gear gtmr ftot susp p15 p16 w1b fl b14 gt in steer gnd sp horiz
velH bodyH slip d av wf wax wle4 wld4 b0c gb498 gb49c`) on every shared frame index.

**D-0 PASSES** iff the count of differing tokens is **0** and the frame count is within
+/-3 of 1626 (attempt 15's `r1`).

**D-0 FAILS** otherwise -> the diagnostic change perturbed behaviour, the attempt STOPS,
and the change is reverted.

KA is also re-run on the PORT side with the same `lam`, as a self-check that the port's
own log is internally consistent. A port-side KA failure is reported but does not void the
original-side result.

## 4 Alignment

`L = 0` (attempt 15, re-confirmed attempt 16). `d = frame - R`, `R_orig = 886`,
`R_port = 1`, exactly `a8_launch.py`'s definitions. **Every number reported in STEP 2
carries `n`, median speed and `d`.** No speed-banded table is opened.

## 5 GATE DR — the decision rule. NOT AMENDABLE.

Scan `d = 0 .. 400` in increasing `d`. At each `d` compute, on each side:

| # | term | definition | floor |
|---|---|---|---|
| 1 | `fwdlen` | `sqrt(fwd.x^2+fwd.y^2+fwd.z^2)` | 1e-6 |
| 2 | `speed` | `+0x9e4` | 1.0 |
| 3 | `vellen` | `sqrt(vel.x^2+vel.y^2+vel.z^2)` | 1.0 |
| 4 | `mis` | `1 - |dot| / (vellen * fwdlen)` (the misalignment) | 1e-3 |

gap(X) = `|O - P| / max(|O|, |P|, floor)`. Tolerance **2 %**, chosen to match the §3a
metric tolerance of 2 % of the original's mean.

**The diverging term is the first entry in the table order 1,2,3,4 whose gap first
exceeds 2 % at a `d` where every EARLIER entry's gap is within 2 % at that same `d`.**
Scanning is by increasing `d`; the earliest such `d` is reported with its term.

If no term exceeds 2 % anywhere in `d = 0..400`, DR reports **NO DIVERGING INPUT** and
`+0xb0c` is declared float noise.

## 6 GATE CB — the consequence bound. NOT AMENDABLE.

`+0xb0c` has exactly **two** readers in `.text` (`RESULT_STEP1.md` §3): A6a
`0x004676de` and the accessor `FUN_0046d6a0` `0x0046d6b6`. The A6a channel is

```
fVar5 = 1500.0 - b0c        0x004676cd (fld _DAT_005cd0ac = 1500.0) / 0x004676de (fsub)
fVar5 = max(fVar5, 500.0)   0x00467702 (fcom _DAT_005ccd04 = 500.0) / 0x00467721
```

Report `max |b0c|` over `d = 0..400` on each side, and
`gapF = max_d |fVar5_O - fVar5_P| / max(fVar5_O, fVar5_P)`.

- **CB-SMALL** iff `gapF < 5 %` at every `d` in `0..400`. Then it is recorded that the
  A6a channel of `+0xb0c` cannot carry more than a 5 % multiplicative effect on
  `fVar5`, `+0xb0c` is **NOT** the ~10x recovery deficit, and STEP 3 is NOT a `+0xb0c`
  fix: it is whatever DR named, confirmed live first.
- **CB-LARGE** otherwise. Then `+0xb0c` is a live channel and STEP 3 targets DR's term
  with `+0xb0c` as the witness.

This bounds the `b0c -> fVar5` channel only. `fVar5` is then multiplied by other terms
(`0x00467727` onward); a `k %` error in `fVar5` is a `k %` error in that product and
nothing is claimed about the other factors. The accessor reader is the AI
(`FUN_0046d6a0 -> [+0x20]`, `AiStandalone.cpp:833`); on the solo arm `participants = 1`,
so whether it is live for slot 0 is **[UNCERTAIN]** and will be marked so unless
witnessed.

## 7 STEP 3 and STEP 4

STEP 3 is entered **only** if DR names a term AND that term is confirmed on the running
original. The fix carries the full promotion leg: one faithful body at the RVA in the
existing port TU for that RVA (never a parallel copy), dual-copy guard `NEW=0`, Frida
path1 + path2, `re-classify` with only what was earned. **No knobs, no clamps, no fitted
constants.** If DR names no term, STEP 3 is NOT entered and the next first-diverging term
across the trough is named and reported unfixed.

STEP 4 re-measures under the already-standing rules, unchanged and not restated here:
(a) DR's term tracks at matched `d` within 2 %; (b) `a8_launch.py` still PASSES;
(c) H1 recovery (`>= 50 %` of the 400 post-trough frames `>= 100` AND median `>= 900`);
(d) the three `d81a8df6` bounds — slip 1500-2000 `0.18855..0.19635`, slip 2000-2600
`0.24488..0.25487`, driving-median `1904.70..1982.44` — 3 runs, §16.7 arm
(`MASHED_MEASURE_SOLO=1`, `MASHED_TRACK_SEL=12`, `MASHED_STEER_HOLD_AFTER=0`),
participants=1 confirmed, median frames via `a8_medframe.py`.

## 8 The arm

Port runs: `a8_run_port.py`, 90 s, env exactly as attempt 15's `r1` PROVENANCE:
`MASHED_REAL_PHYSICS=1 MASHED_RACE_DEMO=1 MASHED_PLAY_DEMO=1 MASHED_GOTO=6
MASHED_CAR_SEL=0 MASHED_DRIVE_HOLD=1 MASHED_WIN_POS=primary-bl MASHED_MOTION_DIAG=1
MASHED_STEER_HOLD=1 MASHED_MUTE=1`, plus `MASHED_MEASURE_SOLO=1 MASHED_TRACK_SEL=12
MASHED_STEER_HOLD_AFTER=0`, and `MASHED_TITLE` set on every run. Only PIDs spawned by this
session are killed.

## 9 Collateral

Closing leg, mandatory regardless of outcome: `collateral.py` with a floor, paired
pre-fix vs post-fix on the same side, and cross-side at matched `d`. Outside-scope rows
are reported.
