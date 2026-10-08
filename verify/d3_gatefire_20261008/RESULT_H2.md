# RESULT — `H2` DECIDED and SPLIT. `MASHED_SLOTSTATE_SEED` is **default-ON**; `MASHED_SLOT_PLAYER` stays **OFF**.

Date 2026-10-08. Pre-registration: [`PREREG_H2.md`](PREREG_H2.md), written before the default was
touched. **No C-level.** `original/` untouched.

Raw: `H2_def.step.csv`, `H2_optout.step.csv`, `H2_{def,optout}.log`. Driver `run_h2.ps1`.

## 0. Verdict first

> **All five `H2` gates PASS. The seed is now default-ON.** It is a **data-initialisation
> correction, not a behavioural improvement** — measured inert on (b), (e), and **81 of 84**
> stepdump columns.
>
> **`RESULT_SP.md` §5's "the two should be decided together" is declined.** The dependency is real
> (the player store is a no-op without the seed) but the *standard of evidence* is not shared. The
> seed restores a constant **the binary's own image carries**; the player store is, in its own
> source comment's capitals, "**A BRIDGE, EXPLICITLY, AND NOT A PORT OF CONTROL FLOW**", placed at
> a site the source itself calls unfaithful, reproducing **4 of 6** effects of an original function
> the port never calls — and existing only to enable **branch 2, which is itself default-OFF**.
>
> Shipping the enabler without the thing it enables buys nothing and carries risk the metrics
> cannot see: `RESULT_SP.md` §2 records that the stepdump **never logs car 0**, so the player
> store's effect on the player is unmeasured.

## 1. Gate verdicts

| gate | verdict | evidence |
|---|---|---|
| `H2-FAITHFUL` | **PASS** | `0x005f2728` is the dword the image carries at `0x005f2770`, file offset `0x1f2770`, **identical in `MASHED.exe` and `MASHED.exe.unpatched`**. `.data` offset `0x008770` < RawSize `0x04d000`, so it is genuinely file-backed — checked per-address, not assumed (memory `data-section-is-mostly-bss`). Independent of the decomp witness the source cites. |
| `H2-SCOPE` | **PASS** | `SP_base` vs `SP_seed`, 19,418 shared keys × 80 columns: exactly **3** differ — `ss_base`, `ss_v`, `ss_raw`, the seed's own witnesses. |
| `H2-DEFAULT` | **PASS** | `WS_ctl` (pre-flip default) vs `H2_def` (post-flip default), 19,418 keys × 84 columns, identical key sets: exactly `ss_base` / `ss_v` / `ss_raw` differ, **81/84 columns identical on every row**. `ss_base` 0 → **6,235,944** (`0x005f2728`), `ss_v` −1 → 2, `ss_raw` 0 → 2. |
| `H2-OPTOUT` | **PASS, at the strongest level** | `MASHED_NO_SLOTSTATE_SEED` gives sha8 **`7ced2aa3`** — **byte-identical** to `WS_ctl.step.csv`, not merely column-identical. The pre-flip default is exactly reachable. |
| `H2-BE` | **PASS** | (e) `launch` **1426.4 / 2053.0 / 2055.2**, `ft_median_m0` **2550.6 / 2053.0 / 2278.2** — the committed baseline, digit for digit. (b) v1/v3 PASS, v2 fails the **same five bands with the same numbers** (`c0_distinct=39`, `c1_distinct=75`, `steer_distinct=113`, `c1_median=5.5`, `abs_steer_median=44.5`). |

As always this is PASS **as unchanged**, not "(b) passes" — v2 still fails, which is what U-9186
exists to fix.

## 2. What changed in source

`TrackRenderer.cpp`, the slot-state block:

```cpp
static const bool s_noSlotStateSeed = (std::getenv("MASHED_NO_SLOTSTATE_SEED") != nullptr);
if (!s_noSlotStateSeed) Ai::I32(0x005f2770u) = 0x005f2728;
```

The opt-out follows the house convention for default-ON features (`MASHED_NO_FOG`,
`MASHED_NO_UVSCROLL`, `MASHED_NO_COPTERS`, `MASHED_NO_PARTICLES`, `MASHED_NO_ELIM`). It exists
because a default flip that cannot be undone from the environment destroys the A/B lane this
project runs on.

`MASHED_SLOTSTATE_SEED` is **now a no-op**: existing drivers that set it (`run_gf0.ps1`,
`run_wshape.ps1`, `run_h2.ps1`) keep working unchanged, and arms that *clear* it no longer get the
old behaviour. That second half is the one to remember.

## 3. Consequence for every future knob-off control — read this before comparing to an old capture

**The knob-off reference has moved.** `GF0step.csv` / `86b7b2bb` and `WS_ctl.step.csv` /
`7ced2aa3` are **pre-seed** captures. A new default capture differs from both in `ss_base`,
`ss_v`, `ss_raw` **by design**, and a leg that diffs against them without expecting those three
columns will read a false regression.

| capture | schema | knob state | sha8 |
|---|---|---|---|
| `GF0step.csv` | 80 col | pre-seed default | `86b7b2bb` |
| `WS_ctl.step.csv` | 84 col | pre-seed default | `7ced2aa3` |
| **`H2_def.step.csv`** | **84 col** | **NEW default (seed on)** | **`c741a4c5`** |
| `H2_optout.step.csv` | 84 col | `MASHED_NO_SLOTSTATE_SEED` | `7ced2aa3` |

**`H2_def.step.csv` / `c741a4c5` is the current default-build control.**

## 4. Why take it at all, given it is inert

Because the value is a **precondition other ports will read**, not a behaviour. The port's
image-pad zero-fills an RVA the original's `.data` initialises, so `CarSlotStateSet` early-returns
on `base == 0` and every future consumer of that table silently reads a wrong answer instead of a
missing one (memory `zeroed-granule-vs-minus-one-sentinel`). D3's phase goal is a faithful default
build; this is one dword of that, bought for no measured behavioural cost.

It is **not** progress on (b) or (e) and must not be reported as such.

## 5. What is NOT claimed

- **No C-level.** No function moves on the rubric. `TrackRenderer.cpp:379-380` already said "no
  C-level follows"; this leg does not change that.
- **Not a behavioural improvement.** Inert on every metric the project has.
- **Not that the player car is unaffected by anything.** The metrics never covered car 0
  (`RESULT_SP.md` §2). That is precisely why `MASHED_SLOT_PLAYER` was not taken.
- Not that `MASHED_SLOT_PLAYER` is wrong — only that it is a partial bridge whose purpose is to
  enable a default-OFF feature, so it should be decided **with** `MASHED_WIRE_B2`, in a leg that
  measures the player car.

## 6. Still OFF

`MASHED_SLOT_PLAYER`, `MASHED_WIRE_B2`, `MASHED_A364_RESET`, `MASHED_RACEPCT_BRIDGE`,
`MASHED_RACEMETRIC_ARC`.
