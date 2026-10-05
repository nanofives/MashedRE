# RESULT — U-9194 RETRACTED. G-ARM's denominator was wrong, and the port's arm choice is CORRECT.

**RAN 2026-10-05, same day as `RESULT_ARM.md` and retracting its headline.** This document exists
because I filed a structural defect that is not one.

**No C-level moved. No band moved. No code behaviour changed.** One comment-only repair to
`BodyOrientationIntegrate.cpp` (reverting a comment I had wrongly "corrected" hours earlier).

---

## 0. Process disclosure, stated first because it matters for how this should be read

**This was reconnaissance, not a pre-registered leg.** The user asked me to pre-register and begin
porting the `+0x10` producer (U-9194's path-to-resolution item 1). Scoping that pre-registration
required knowing what `+0x10` correlates with, and the first reconnaissance query refuted the row
outright.

I am not dressing this up as a gated measurement. What makes it admissible anyway:

1. **It refutes my own committed claim.** A reconnaissance pass cannot p-hack its way into a
   retraction of the finding it was meant to support.
2. **The confirmation leg WAS pre-stated.** Before reading them, the criterion for the held-out
   captures `o_t2.msd` and `o_t3.msd` was fixed in writing as **`+0x10 == 0` on >= 99 % of frames
   where car 1's position changes**, with the stated consequence that a material share of moving
   frames on the other arm would mean U-9194 **survives in reduced form**. Those two files were
   deliberately not opened during the reconnaissance so they would be a genuine held-out test.

## 1. What `+0x10` actually is: ONE transition, not a per-frame fork

On `verify/d3_elim_20261003/o_t1.msd` (car 1, an AI car, `rec_size 0xd04`, `base_va 0x008822a4`,
frame_idx 0..3622 contiguous with no gaps):

- **`+0x10` has exactly TWO runs: `0` for frames 0..1064, then `1` for frames 1065..3622.**
  **One** transition in the entire capture.
- `+0x4` is its exact complement, `1` then `0`. `+0x2c` and `+0x30` go `0 -> 50`. Four per-wheel
  float pairs (`+0x1f8`/`+0x1fc`, `+0x2bc`/`+0x2c0`, `+0x380`/`+0x384`, `+0x444`/`+0x448`, stride
  `0xc4`) go `(0.15, 0.0125) -> (0.25, 0.25)`.
- **That single frame step moves 135 of 833 record dwords at once.**

**`RESULT_ARM.md` §1 states `+0x10` is "not constant, so this is a live per-frame fork and not a
fixed mode". That inference is WRONG** and so is the same sentence in U-9194 and in the 2026-10-05
CHANGELOG entry. Not-globally-constant was read off `len(hist) == 1` being false, which does not
distinguish one transition from per-frame chatter. It is one transition.

## 2. Car 1 is ELIMINATED at frame 1065 and never moves again

The capture's own provenance is `--mode 10` (elimination), `--cars 4`, `--hold 60`.

| frame block | `+0x10` | median speed `+0x9e4` | distance travelled | end position |
|---|---|---|---|---|
| 0..799 | 0 | 0.0 | 1.424 | (-1.10, -0.90) |
| 800..1199 | 0 | 1471.8 | **23.548** | (1.14, -22.91) |
| 1200..3622 | 1 | 0.0 | **0.000** | (1.14, -22.91) |

Car 1 drives once, for ~220 frames, then sits at a fixed position for the remaining **2423** frames
with **zero** distance travelled. Its speed **field** keeps a non-zero value for a further 222 frames
(845..1286 carry non-zero `+0x9e4` while the position is pinned from 1065), which is the stale-field-
on-a-dead-car family already tracked as **U-9189** — recorded, not re-investigated here.

## 3. The denominator that matters, and the held-out confirmation

**Restricted to frames where the car actually moves, the two sides take the SAME arm.**

| capture | frames | moving frames | `+0x10 == 0` on moving | all-frames `+0x10 == 0` | transitions | criterion >= 99 % |
|---|---|---|---|---|---|---|
| `o_t1.msd` | 3623 | 222 | **222 of 222 = 100.000 %** | 1065 of 3623 = 29.40 % | 1 | **PASS** |
| `o_t2.msd` | 3614 | 222 | **222 of 222 = 100.000 %** | 999 of 3614 = 27.64 % | 1 | **PASS** |
| `o_t3.msd` | 3624 | 222 | **222 of 222 = 100.000 %** | 1017 of 3624 = 28.06 % | 1 | **PASS** |

Unanimous on three independent captures, two of them held out. **Zero moving frames take the
non-zero arm.**

**And inside the (b) scored window specifically:** the window is frame_idx **845..1064**, and
`+0x10 == 0` on **220 of 220** of its frames. The port takes that arm on 100 % of substeps, so
**within the window where U-9191's 0.9866 deg is measured, the arms agree.** The window ends exactly
one frame before the transition — not a coincidence to be marvelled at but the same fact twice: the
window is "the first 220 calls from first throttle", the car's entire driving life is those 220
frames, and `+0x10` flips the moment it stops. The window's measured arclength (23.5394) equals the
capture's total distance travelled (23.548) for the same reason.

## 4. What this retracts, precisely

**RETRACTED from U-9194 / `RESULT_ARM.md` / the 2026-10-05 CHANGELOG entry:**

- that `+0x10` is a "live per-frame fork" — it is one transition;
- that the original "takes the non-zero arm on 70.60 % of frames" **as a statement about a driving
  car** — the figure is arithmetically right and materially misleading, because its denominator is
  dominated by 2558 frames of a parked, eliminated car;
- that the port is **on the wrong arm** — it is on the right one whenever the car drives, 222 of 222;
- that the `+0x10` **producer is owed** — an always-zero `+0x10` selects exactly the arm the original
  uses while the car moves, so no producer is required for driving fidelity;
- that `BodyOrient_OmegaFromAngVel`'s zero call sites are a **defect** — on this evidence it needs
  none;
- that the arm is a **candidate carrier of U-9191 / criterion (b)** — it cannot carry a phenomenon
  measured entirely inside a 220-frame region where the two sides provably agree.

**This is a denominator error of exactly the class the session's own instructions warned against:**
*"Put each gate's threshold and the number it will be compared against on the same line, with both
denominators spelled out."* G-ARM's denominator was all 3623 frames; U-9191's phenomenon lives in 220
of them. I spelled the denominator out and still chose the wrong population, which is the sharper
lesson: stating a denominator is not the same as checking it is the right one.

**What SURVIVES, and it is only this:** the port ignores the `+0x10` gate rather than reading it, and
`rec_10` is `0` on all **7705** rows of `A1.csv` across cars 1/2/3 and frames 0..3207. So if any
future scenario needs a vehicle record in the `+0x10 == 1` state — a dead, parked or eliminated car,
which is what that state demonstrably is here — the port would not reproduce it. That is a **latent
non-driving-regime gap with no current consumer**, not a defect blocking any gate, and it is what
U-9194 is reduced to.

## 5. Consequence for future (b) work — keep this one

**Do not lengthen the (b) window past frame_idx 1064 without accounting for the `+0x10` regime
change.** U-9191's path already refused to lengthen the window, on statistical grounds. There is now
a **mechanical** reason: frame 1065 changes 135 record dwords at once and ends the car's driving life,
so any window extended past it averages two different regimes — one of which is a parked car. The
refusal stands on a mechanism, not just on an n floor.

## 6. The work the user asked for is NOT being done, and why

U-9194's item 1 was "find and port the `+0x10` producer", and the instruction was to pre-register and
start that reversing. **On this evidence it would buy nothing measurable**: the port already behaves
as the original does on every frame where the car drives, on three captures, 222 of 222 each. Writing
a pre-registration for it would mean registering gates for a defect that the reconnaissance needed to
scope those very gates has just disproved.

**No pre-registration was written and no reversing was started.** The alternative paths are unchanged
and are listed in `re/NEXT_SESSION.md`.
