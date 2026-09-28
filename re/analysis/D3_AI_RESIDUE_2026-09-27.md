# D3 AI (b) residue — the step INPUTS instrumented, car 1's split localised

**Date:** 2026-09-27. Branch `race/first-frame-parity`. **Follows:**
`D3_AI_PORT_2026-09-26.md` (the port and the first (b) measurement) and
`D3_MODES_2026-09-26.md` §6. Shape: **measured / refuted / open**. Anchor: every RVA
below was read in Ghidra pool slot `Mashed_pool0` (read-only) on `MASHED.exe`
(SHA-256 `BDCAE093…3C0E`), or disassembled with `re/tools/disasm_va.py` against
`original/MASHED.exe.unpatched`. **No tolerance band was changed.**

## 0. Verdict

| Criterion (ROADMAP §D3 AI) | Verdict 2026-09-27 |
|---|---|
| (b) per-car ctrl distribution inside the tolerance | **still NOT met, unchanged: cars 2 and 3 PASS all 10 bands, car 1 fails the same 2** (`c0` distinct 7 vs 13..37, `c1` distinct 82 vs 17..70). §1 |
| (b) cause | **localised, §3**: `DAT_0089a368` is 0 for the whole standalone window and 1 for 159 of the original's 220 calls. Downstream that is the spline BANK, the curvature, the steer-magnitude multiplier and the accel byte |
| (a), (c), (d) | unchanged (met 2026-09-26) |

**§9 is a plan committed before any capture; §10 is its executed result (2026-09-28).**
§10 refutes §8 item 1 option (ii) and hands the user a decision, unmade here.

Two prior hypotheses from `D3_AI_PORT_2026-09-26.md` §6 item 1 are **REFUTED** here
(§4). One item, **U-D3-AIRAND, is RESOLVED** and its function ported verbatim (§5),
which did **not** close (b).

## 1. MEASURED — the re-capture on the new `MASHED_ROUND` route

`4ff428ad` changed that route to run the rule engine from score 6, so the 2026-09-26
numbers had to be re-taken before anything was compared to them. Three standalone
captures this session (`verify/d3_ai_20260927/b0.csv`, `s2.csv`, `s6rng.csv`) give
**call-for-call the same window result as `sa2` did**:

| car | `c0`/`c1` distinct | steer distinct | accel | brake | (b) |
|---|---|---|---|---|---|
| 1 | **7** / **82** | 88 | {0,255} med 255 | {0,255} med 0 | **FAIL** 2 of 10 |
| 2 | 28 / 69 | 96 | {0,255} med 255 | {0,255} med 0 | PASS |
| 3 | 14 / 67 | 80 | {0,255} med 255 | {0,255} med 0 | PASS |

So the modes change does not touch the 220-call window. That is a measurement, not an
assumption: `py -3.12 re/tools/ai_ctrl_window.py --check verify/d3_ai_20260927/s6rng.csv`.

## 2. MEASURED — both step dumps now carry the step's INPUTS

Nine columns were added to **both** sides, so they still diff directly:
`look_x, look_z, curv, own_x, own_z, hist_d8, hist_dc, march_n, march_idx0`
(the standalone adds three of its own after those: `look_best, look_idx, look_blk`).
New comparison tool: `py -3.12 re/tools/ai_step_compare.py <orig.aistep.csv> <sa.csv>`.

### 2.1 How each column is taken, and three dead ends that are worth not repeating

- **`curv`** — entry hook on `FUN_00443440` (`0x00443440`), filtered to the
  `FUN_00416250` call site by `arg3 == 0x41200000` (10.0f) and `arg5 == 0`
  (`0x0041629a..0x004162ab`); `FUN_00414570` calls the same function with 7.5f
  (`0x40f00000`) and is therefore excluded. The caller's fold
  `if (180 < c) c = 360 - c` (`0x004162be..0x004162d5`) is applied, because that is
  what the bands see.
- **`look_x/look_z`** — entry hook on `FUN_00415e20` (`0x00415e20`), whose three stack
  args ARE the final target: `0x00416580..0x0041658f` pushes `[F+0x38]`, `[F+0x34]`,
  `esi`. This is the target AFTER the targeting chain, not the seed.
- **`own_x/own_z`** — **corrected mid-session.** `0x008815a0 + v*0xd04 + 0x30/+0x38`
  (the base the existing `rec_9e4` column uses) reads **0.0 on every row**; that base
  is not where the position lives. `FUN_00416250` gets it from
  `FUN_0046d4a0(&p, v)` then `*(p+0x30)` / `*(p+0x38)` (`0x00416284`, `0x0041628d`,
  `0x00416297`), so the capture learns the per-vehicle record POINTER from
  `FUN_0046d4a0` once and **detaches that listener** (it is far too hot to leave on).
  An entire hypothesis was built and then thrown away on the zeroed version — see §4.1.
- **`hist_d8/hist_dc`** — the two globals `0x008032d8 + v*0x14` and `0x008032dc + v*0x14`.
  `FUN_00416250` writes them unconditionally inside each band:
  band `err < 180` sets `d8 = 360.0` (`0x004165cc`) then `dc = err` (`0x004165f7`);
  band `err > 180` sets `dc = 0.0` (`0x004166db`) then `d8 = err` (`0x0041670c`).
  So `d8 == 360` decodes to band 1 with `err = dc`, and `dc == 0` to band 2 with
  `err = d8`. That gives the steering error AND which band took it with no stack read.
  Band 1 commits `ctrl[0]` and replays `ctrl[1]`; band 2 the reverse
  (`0x00416648`/`0x00416623`, `0x0041675c`/`0x00416738`) — i.e. the `c0`/`c1` distinct
  split the tolerance tests is a direct readout of the band mix.
- **`march_n/march_idx0`** — entry hook on `FUN_00416230` (`0x00416230`, whole body
  `[0x89a500 + v*0x74] = arg2`, `0x0041623b`). It is called once per pass of
  `FUN_00443dc0`'s phase-8 wall-march (`0x00444a2c`, inside the `je 0x4446a4` loop at
  `0x00444a3a`), so its call count between two `FUN_00416250` entries is the number of
  target step-backs.

**Dead ends (measured, with a control).** `Interceptor.attach` at `0x004165a5`
(`fld [0x5d757c]`, esp == locals base) killed the game in 2 render ticks; so did
`0x0041657c` (`mov edx,[esp+0x38]`, same esp). The control —
`MASHED_AISTEP_LOCALS=0`, same build, same recipe — completed a 5463-call capture.
The cause is not x87 and not call relocation: Frida's Interceptor is an **entry** hook,
it swaps the dword at `[esp]` to route the return through its leave trampoline, so
pointing it mid-body overwrites a local. No branch in `FUN_00416250` targets either
window (capstone scan: the only `0x4165xx` branch targets are `0x0041650f`,
`0x00416565`, `0x0041657c`, `0x004165e9`). Every probe above is therefore at a genuine
function entry.

## 3. MEASURED — the cause of car 1's split: `DAT_0089a368`

`verify/d3_ai_20260927/o4.msd.aistep.csv` (original, 6280 calls) vs
`verify/d3_ai_20260927/s6rng.csv` (standalone), 220-call window, per car:

| field | orig v1 | sa v1 | orig v2 | sa v2 | orig v3 | sa v3 |
|---|---|---|---|---|---|---|
| `flag_a368` over the window | 0×61 then **1×159** | **0×220** | 0×61 then 1×159 | 0×220 | 0×61 then 1×159 | 0×220 |
| `ai_type` (line type) | 0×61, **2×159** | 0×220 | 0×61, 2×159 | 0×220 | 0×61, 2×159 | 0×220 |
| spline bank | 0, **6, 7** | **0 only** | 0, **6** | 0 only | 0, **6** | 0 only |
| `curv` median | **105.8** | 10.3 | **48.4** | 7.3 | **76.2** | 5.3 |
| `curv > 20` share | 0.723 | 0.450 | 0.659 | 0.409 | 0.705 | 0.382 |
| band-1 share | 0.341 | 0.409 | 0.523 | 0.455 | 0.459 | 0.400 |
| `c0`/`c1` distinct | 13 / 17 | 7 / **82** | 37 / 46 | 28 / 69 | 34 / 39 | 14 / 67 |
| brake share | 0.250 | 0.005 | 0.109 | 0.005 | 0.268 | 0.005 |
| behaviour modes | 0:105, **7:115** | 0:220 | 0:176, **3:44** | 0:220 | 0:207, **3:13** | 0:220 |
| `rec_9e4` median | 2423 | 2822 | 2377 | 2818 | 2299 | 2821 |

The chain, every link cited:

1. **`DAT_0089a368` is set by one 20% roll that the standalone loses.**
   `FUN_004177b0`'s tail (`0x00417c43..0x00417c7a`): when the flag is 0 and a time band
   fired, it draws `FUN_00472650(0, 100.0)` and sets the flag to 1 if the draw is under
   `DAT_005f30a0[row*5 + band]`, else draws again for `DAT_005f3180[...]` → flag 2.
   `row = __ftol(DAT_0089a360)` and `DAT_0089a360 == 2.5` on 220/220 rows of BOTH
   captures, so `row = 2`. Band 0 needs `tickscale - DAT_0089a370 > 1.0` AND
   `tickscale < 2.0` (`tickscale = DAT_007f0ff8 * 0.000333…`), and firing a band writes
   `DAT_0089a370 = tickscale`, so **band 0 is a one-shot**. Table row 2 band 0 is
   `20` (`0x005f30a0` dumped: row 2 = `[20, 40, 60, 75, 100]`; `0x005f3180` row 2 =
   `[10, 10, 15, 30, 0]`). The original won that 20% roll at `clk_0ff4 = 45550`, 61
   calls into its window; the standalone reaches band 0 too and loses it.
2. **flag 1 → line type 2.** `FUN_00417180` (`BankSwitch`):
   `if (DAT_0089a368 == 1) { line_type = 2; }`. Measured: the original's `ai_type`
   flips 0 → 2 on the same call the flag flips, on all three cars. Car 1 additionally
   reaches `ai_spline_idx = 1` (97 of 220 calls) — it is the only car that does, and it
   is the failing car.
3. **line type 2 → a different spline bank → different curvature.** Original banks in
   the window: `{0:61, 6:62, 7:97}` for car 1 (bank = `(spline - 0x801aa0) / 0x204`);
   standalone: bank 0 on 220/220. Curvature median 105.8 vs 10.3.
4. **curvature → the steer magnitude.** `0x0041665c..0x00416679`:
   `if (mode == 0 && 20.0 < curv) m *= curv * 0.05`. With the original's curvature the
   multiplier is ~5x and the magnitude saturates at the clamp `_DAT_005cd04c`
   (`0x0041667b`), so few distinct values (`c1` 17). Without it the standalone's
   magnitudes stay in the graded region, so **82** distinct `c1` on car 1.
5. **flag 1 → accel.** `0x004169e0`: `if (DAT_0089a368 == 1) ctrl[4] = trunc(ctrl[4]*0.4)`.
   Original accel 255 → 102; standalone stays 255.

### 3.1 A/B on the flag (diagnostic knob, default OFF)

`MASHED_AI_DIFFFLAG=1` seeds `DAT_0089a368 = 1` at race reset (`Ai_ResetRace`). It ports
nothing; it exists so the two captures can be compared in the same regime.
`verify/d3_ai_20260927/s5flag.csv`:

| car | with flag 0 (default) | with flag seeded 1 |
|---|---|---|
| 1 | FAIL `c0`=7, `c1`=82 | **both close**; fails only `c1_median = 3` (band 0..0) |
| 2 | PASS | PASS |
| 3 | PASS | FAIL `c0`=41, `c1`=10 |

So the flag is a first-order driver of exactly the two metrics car 1 fails, and it moves
every car. Neither regime alone reproduces the original, which has the flag 0 for the
first 61 window calls and 1 for the remaining 159.

## 4. REFUTED

### 4.1 "The ported lookahead target is 2-4x too close" — refuted, it was a zeroed field

The first pass of §2 read `own_x/own_z` from `0x008815a0 + v*0xd04 + 0x30/+0x38`, which
returns 0.0 on every original row, so the "distance to target" it computed was really
`|target|` from the world origin. With the record pointer taken from `FUN_0046d4a0`
(§2.1) the two sides are in the same range — restricted to `ai_mode == 0`, which is the
only regime where the target IS the spline lookahead:

| car | orig p05 / p50 / max | standalone p05 / p50 / max |
|---|---|---|
| 1 | 3.37 / 5.62 / 7.99 | 1.05 / 3.06 / 6.79 |
| 2 | 1.04 / 4.45 / 11.57 | 1.29 / 7.44 / 11.64 |
| 3 | 1.09 / 5.28 / 10.45 | 1.07 / 6.10 / 10.41 |

The maxima agree to within 15% on all three cars. `FUN_00443dc0`'s 16-step, 0.05-param
forward walk (`_DAT_005cc9a0 = 0.05`, `while (iVar8 < 0x10)`) and its unrolled argmax
(`0x00443440`-adjacent listing, strict `<`, first wins) both match the port line for line.

### 4.2 "The phase-8 wall-march keeps rejecting the target" — refuted

`march_n == 1` on **220/220** window calls on BOTH sides and all three cars, and
`march_idx0 == 0` on 219/220 (original car 1) and 220/220 everywhere else. The
standalone's own `look_best == look_idx` on every row, i.e. the march never steps the
target back. The wall-march contributes nothing to the difference.

### 4.3 "Porting the RNG verbatim will close (b)" — refuted by measurement

§5 ports `FUN_00534870`+`FUN_00534990` verbatim. `verify/d3_ai_20260927/s6rng.csv` is
byte-for-byte the same window verdict as before the port: the band-0 roll is still lost
and `flag_a368` is still 0 on 220/220.

## 5. PORTED — `FUN_00534870` and its opener `FUN_00534990`, verbatim

**U-D3-AIRAND is RESOLVED**, and its premise was wrong: the ring does **not** have to be
read out of a live original. `FUN_00534990` builds it from a hard-coded seed with no
entropy input (`0x005349a0` allocates `0x7c` = 31 dwords; `0x005349e2` seeds
`ring[0] = 0x9a319039`; `0x005349fb..0x00534a07` fills
`ring[i] = ring[i-1] * 0x41c64e6d + 0x3039`; `0x005349bd..0x005349da` and `0x00534a0e`
set `p = base + 0xc`, `q = base`, `end = base + 0x7c`; `0x00534a22..0x00534a2f`
discards `0x136` = 310 draws). `FUN_00534870` is the additive lagged step
`*p += *q; u = *p >> 1; p++; q++` with the two wrap arms at `0x0053489b` and
`0x005348c1` (note the first arm advances `q` **without** wrapping it — ported as-is).

The 32-bit LCG stand-in is gone. What this does **not** buy is the individual draw: the
original's other callers of the same ring (`FUN_00472690`, `FUN_004b44f0`,
`FUN_004b4510`) pull from it too, so the standalone's call INDEX cannot match. The
distribution is now the original's; the specific roll is not.

Regression check: `verify/d3_ai_20260927/s7pu.csv` — the AI still arms and fires
power-ups with the new generator, `ctrl[7] == 1` on 107 / 164 / 22 steps of cars 1/2/3.

## 6. NOT PORTED — the targeting chain and `FUN_00442a60`, with the reason

Both were on this session's list. Both are blocked on a subsystem the standalone does
not have, and inventing one would break NO-GUESSING, so they are recorded rather than
faked.

- **`FUN_00414c30`** (the mode-3/mode-7 producer, and the one that matters most: the
  original's car 1 is in mode 7 for **115 of 220** window calls) iterates the WORLD
  OBJECT list returned by `FUN_00484c70(&count)` at stride `0x23` dwords, and per entry
  calls `FUN_0041f030`, `FUN_0048a630`, `FUN_00414300` / `FUN_00414490` and
  `FUN_00442cc0`. The standalone has no world-object query.
- **`FUN_00442a60`** (the `0x008989b0[0..3]` fire reference distances) is a
  **spectator-camera** routine: it zeroes the four floats, picks a car pair with
  `FUN_0040e180(&a, &b)`, orders them by `FUN_00408ad0`, and only then fills
  `0x008989b0[c] = |own - car_c|_xz * _DAT_005cc9bc` for the cars passing
  `FUN_0040e370(c) != 0 && FUN_0046c7b0(c) == 1`. It needs `FUN_0040e180`, which the
  standalone does not have. Until it is ported the array stays 0, so MORTAR/DRUM/
  P_MINE/R_FLAME/SHOTGUN cannot pass their fire gates (unchanged from 2026-09-26 §6.4).
- **`FUN_00416060`** (the LOS every arm of the chain calls) IS reachable: its tile probe
  is byte-identical to the already-ported `TileBlocked`
  (`rz/rx = __ftol(·*4 ±0.5) - 0x10`, `s = word[0x007f1a9c + (((rz+0x200)>>3)*0x80 +
  ((rx+0x200)>>3))*2]`, blocked iff `0 < s < 0x200` and
  `byte[0x007f9a9c + ((rx&7) + s*8)*8 + (rz&7)]` is 0 or 3), marching A→B in 0.25 steps
  (`_DAT_005cc564`) with the same "skip a sample whose x equals A.x or z equals A.z"
  rule, returning 0 blocked / 1 clear. It is **deliberately not ported this session**:
  with every producer in the chain still returning 0 it cannot change a single byte, so
  it would be an unverifiable change.

## 7. MEASURED — the brake rate and the mode distribution (ROADMAP (b) side notes)

- **Brake share of window calls:** original 0.250 / 0.109 / 0.268 (cars 1/2/3);
  standalone **0.005 on all three**, unchanged by the RNG port and unchanged by the
  `MASHED_AI_DIFFFLAG=1` A/B. The rule is `0x00416818`: brake when the x87 history value
  X exceeds 20 AND `rec+0x9e4 > 2000`. The port's X reproduces the listing (band 1
  `X = hist_dc` when `hist_dc < err`; band 2 `X = 360 - hist_d8` when `hist_d8 > err`),
  so the gap is that the standalone's steer history rarely swings more than 20° — i.e.
  downstream of §3, but **not** explained by the flag alone. Still open.
- **Behaviour modes:** original `{0:105, 7:115}` / `{0:176, 3:44}` / `{0:207, 3:13}`;
  standalone `{0:220}` on all three. Blocked on §6.
- **Speed:** standalone `rec_9e4` median 2818-2822 on all three cars against the
  original's 2299-2423, i.e. the standalone's opponents run 16-23% faster through the
  window. Not tested by any (b) band. New, open.

## 8. OPEN

1. **(b) car 1.** Cause localised to §3 but not closed. The honest options are (i) port
   `FUN_00414c30` + its world-object dependencies so modes 3/7 exist, or (ii) establish
   whether the original's own run-to-run spread on `DAT_0089a368` (2 of 5 runs took the
   flag path per `D3_AI_PORT_2026-09-26.md` §1) means the pooled envelope already covers
   both regimes and the standalone simply lands outside both. Do NOT move the bands.
2. **Brake share** 0.005 vs 0.109-0.268 (§7). Next: dump the per-call X and `rec_9e4`
   on both sides (both are already derivable from `hist_d8`/`hist_dc`/`rec_9e4` in the
   two CSVs) and find which of the two conjuncts fails.
3. **Opponent speed** 16-23% high (§7). Next: compare `rec_9e4` against the D2 physics
   capture, not the AI one — this may be a physics residue, not AI.
4. **Targeting chain** and **`FUN_00442a60`** (§6), each with its blocking dependency named.
5. **[UNCERTAIN] U-D3-DIFF360** — the writer of `DAT_0089a360` (2.5 measured on both
   sides, 220/220). Unchanged. Next: `reference_to 0x0089a360` WRITE sites in a pool slot.
6. **Human slot state** (`FUN_0040e470(c) == 1`) — unchanged from 2026-09-26 §6.7.
7. No C-level moved. `hooks.csv` untouched (trackers only via `re-classify`).

## 9. PLAN — the (b) car-1 decision evidence (written BEFORE the captures)

This section was committed on its own, with no capture run, so the analysis cannot be
accused of having been shaped by its result. It takes **option (ii)** of §8 item 1 and
`re/NEXT_SESSION.md` DO-1(b): establish by measurement whether the original's pooled (b)
envelope spans **two** `DAT_0089a368` regimes, and whether the standalone lands outside
both. **No tolerance band and no criterion text is touched by this session.** The output
is evidence; the decision is the user's.

### 9.1 What is being tested, and why the pooled envelope is suspect

The envelope in `re/tools/ai_ctrl_window.py:50` `TOLERANCE` is the min..max over the
original's 12 (run, car) observations. §1 of `D3_AI_PORT_2026-09-26.md` already records
that **2 of those 5 runs took the flag path**, and §3 above shows the flag switches the
spline bank, the curvature, the steer multiplier (`0x0041665c`) and the accel byte
(`0x004169e0`). So the pooled envelope may be the **union of two disjoint regimes**
rather than one population, in which case "inside the envelope" is a weaker statement
than the criterion intends, and "outside it" for car 1 may mean outside BOTH regimes or
outside only the one it is in. That distinction is the decision.

### 9.2 The four steps, fixed in advance

1. **N = 10 original captures** (N ≥ 8 required; 10 chosen for the rate estimate in
   step 4), each on the **exact** recipe already in
   `verify/d3_ai_20260927/o4.msd.provenance.json`:
   ```
   py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0 \
       --poke-ctrl-slots --statediff-out verify/d3_ai_20260927b/pN.msd \
       --statediff-car 1 --statediff-aistep --hold 60
   ```
   `--poke-ctrl-slots` always; muted (`scenario_launch.py:1448` defaults
   `MASHED_MUTE=1`). Runs are **sequential**, PIDs tracked and killed only by this
   session. For each run and each car v ∈ {1,2,3} record the `flag_a368` sequence over
   the 220-call window that `ai_ctrl_window.py` defines (first call with `c4 != 0`), and
   `rec_9e4`.
2. **Per-regime envelopes.** Split every (run, car) window into its `flag_a368 == 0`
   prefix and its `flag_a368 == 1` suffix, and recompute **every one of the 10 band
   columns** of `TOLERANCE` separately per regime. Because `*_distinct` counts grow with
   the row count, each regime is measured over a **fixed** number of calls `K_r`, equal
   to the minimum sub-window length over the qualifying observations (an observation
   qualifies for regime r only if it has ≥ 30 calls of that regime), so all observations
   in one envelope are measured on the same N. `K_0` and `K_1` are reported. New tool:
   `re/tools/ai_flag_regime.py`, which reuses `ai_ctrl_window.py`'s window definition and
   its stat set verbatim.
3. **The standalone against each regime.** Two standalone captures on the recipe of
   `D3_AI_PORT_2026-09-26.md` §4, one default (`DAT_0089a368` = 0 throughout) and one
   with `MASHED_AI_DIFFFLAG=1` (§3.1; seeds the flag to 1 at `Ai_ResetRace`, ports
   nothing). Each is scored with the same `K_r` truncation against the regime-0 and the
   regime-1 envelope, per car. The result is a 3-way statement per car: inside regime 0,
   inside regime 1, inside neither.
4. **The flag-1 rate against 20%.** §3 item 1 reads the table entry as `20` at
   `0x005f30a0` row 2 band 0. Band 0 is a one-shot, and §3 measures all three cars
   flipping on the same call, so the trial is **once per run**, not per car. Report the
   count of runs out of N that reached `flag_a368 == 1` inside the window, and its exact
   binomial interval against p = 0.20. This is a consistency check on the reading of the
   table, not a criterion.

Also recorded in every run, per car, because §7 flagged it and no (b) band tests it:
`rec_9e4` (speed) min / p05 / median / p95 / max, original vs both standalone regimes,
to test the "standalone opponents run 16-23% fast" finding against the original's own
run-to-run spread rather than against a single capture.

### 9.3 What this plan does NOT do

It does not port `FUN_00414c30` (option (i) of §8 item 1), it does not build, it does not
touch `mashedmod/src`, and it does not move a band. If the per-regime envelopes turn out
to be disjoint, the plan **reports that** and states the options; choosing among them is
the user's call, recorded in §10.

## 10. EXECUTED — the result of the §9 plan

**Deviations from §9: none in the recipe.** Two additions are marked as such below
(§10.5 the full-window speed view, §10.3 the `--pure` view) and both use code paths that
were in `re/tools/ai_flag_regime.py` before any capture was scored. No band, no
criterion text, no `hooks.csv` row was touched. No build was run and `mashedmod/src` was
not edited (a second session was building in this tree).

Captures: 12 original launches attempted, **N = 10 usable**. `p5` and `p7` produced no
data (`verify/d3_ai_20260927b/p5.log`, `p7.log`: `error: could not attach`, 167 bytes,
no `.msd`), so they are excluded and are not counted in N. Every usable run carries a
provenance JSON whose `argv` is the §9.2 step-1 command verbatim
(`--poke-ctrl-slots`, `--hold 60`, muted), `git_head b747e7c6`. Standalone: `sa_d1`,
`sa_d2` default and `sa_f1`, `sa_f2` with `MASHED_AI_DIFFFLAG=1`, all on the already-built
`mashed_re.exe`.

Reproduce:
```
py -3.12 re/tools/ai_flag_regime.py \
  --envelope verify/d3_ai_20260927b/{p1,p2,p3,p4,p6,p8,p9,p10,p11,p12}.msd.aistep.csv \
  --score   verify/d3_ai_20260927b/{sa_d1,sa_d2,sa_f1,sa_f2}.csv
```
Outputs committed: `verify/d3_ai_20260927b/regime_split.txt`, `regime_split.json`,
`regime_pure220.txt`.

### 10.1 MEASURED — the flag-1 rate: 2 of 10, and all three cars flip together

| run | v1 flag0/flag1 | v2 | v3 |
|---|---|---|---|
| p1, p2, p3, p4, p8, p10, p11, p12 | 220 / 0 | 220 / 0 | 220 / 0 |
| **p6** | **61 / 159** | 61 / 159 | 61 / 159 |
| **p9** | **61 / 159** | 61 / 159 | 61 / 159 |

No row carried a flag value other than 0 or 1 (`other: []` on all 30 observations), so
the flag-2 arm of `0x00417c43` never fired in any window.

- **Rate: 2 / 10 = 0.20.** Exact two-sided binomial test against p = 0.20: **p = 1.000**.
  Clopper-Pearson 95% interval **[0.025, 0.556]**, which contains 0.20. The §3 item 1
  reading of `DAT_005f30a0` row 2 band 0 = `20` is consistent with the observed rate.
  N = 10 makes this a weak test (the interval also contains 0.05 and 0.50); it is a
  consistency check on the table read, not a criterion, exactly as §9.2 step 4 states.
- **The trial is once per run, confirmed rather than assumed.** In both flipping runs the
  split is `61 / 159` on *all three* cars, i.e. the flip lands on the same window call
  index for every car and in both runs. The per-car breakdown was printed precisely so
  this could be re-checked; it holds.

### 10.2 MEASURED — the per-regime envelopes (§9.2 step 2)

`minrows = 30`. `K_0 = 61` (30 observations: all 10 runs x 3 cars qualify, and the
minimum flag-0 prefix is the 61 of the two flipping runs). `K_1 = 159` (6 observations:
2 runs x 3 cars). **The two regimes are measured on different call counts, so their
columns are not comparable to each other, and neither is comparable to the pooled
`TOLERANCE` (N = 220).** The pooled column is printed for orientation only.

| band column | regime 0 (K=61, 30 obs) | regime 1 (K=159, 6 obs) | pooled `TOLERANCE` (N=220) |
|---|---|---|---|
| `c0_distinct` | 13..24 | 3..24 | 13..37 |
| `c1_distinct` | **1..8** | **17..41** | 17..70 |
| `steer_distinct` | 13..30 | 19..64 | 29..96 |
| `c0_median` | 0..10 | 0..0 | 0..0 |
| `c1_median` | 0..0 | 0..0 | 0..0 |
| `abs_steer_median` | 8..37 | 0..13 | 0..23 |
| `accel_distinct` | 1..2 | 2..3 | 2..4 |
| `accel_median` | **255..255** | **25..102** | 25..255 |
| `brake_distinct` | 1..2 | 2..2 | 2..2 |
| `brake_median` | 0..0 | 0..0 | 0..0 |

Two columns are **disjoint** between the regimes: `accel_median` (255 vs 25..102, the
direct signature of `0x004169e0`'s `ctrl[4] *= 0.4`) and `c1_distinct` (1..8 vs 17..41,
though this pair is confounded with K_0 != K_1). So the pooled envelope on those two
columns **is** a union of two populations. `accel_median`'s pooled span `25..255` in
particular admits any value in between, none of which the original ever produces.

### 10.3 MEASURED (addition) — the same-K view: the pooled envelope is essentially regime 0

To remove the K_0 != K_1 confound, `--pure` keeps only windows that are 100% one regime,
so K = the full 220 and the envelope is directly comparable to `TOLERANCE`. Regime 1 has
**no** pure observation (the flag is 0 at every window start, so no original window is
all-flag-1), and regime 0 has 24 (the 8 non-flipping runs x 3 cars):

| band column | pure regime 0 (K=220, 24 obs) | pooled `TOLERANCE` (K=220) |
|---|---|---|
| `c0_distinct` | 13..35 | 13..37 |
| `c1_distinct` | 21..70 | 17..70 |
| `steer_distinct` | 33..96 | 29..96 |
| `abs_steer_median` | 7..23 | 0..23 |
| `accel_distinct` | 2..3 | 2..4 |
| `accel_median` | 255..255 | 25..255 |
| the other 4 | identical to pooled | |

**This is the answer to §8 item 1 option (ii), and it is negative.** On the six columns
that vary, the single-regime envelope is within a few counts of the pooled one on five of
them. The pooled envelope's *width* is therefore **not** an artifact of mixing two
regimes, with the one exception of `accel_median` (and `accel_distinct`), where the
pooled span is genuinely a union.

### 10.4 MEASURED — the standalone against each regime (§9.2 step 3)

`sa_d1` and `sa_d2` produce **identical** values on every statistic reported here, as do
`sa_f1` and `sa_f2` (the CSVs differ in total length because the 60 s hold yields
different total call counts, but the 220-call windows agree). The standalone is
deterministic run-to-run on this recipe; only one of each pair is quoted.

**Default build** (`flag_a368` = 0 on 220/220, so it is in regime 0 and `NOT_IN_REGIME`
for regime 1):

| car | vs regime 0, K=61 | vs regime 0 pure, K=220 |
|---|---|---|
| 1 | **FAIL** `c0_distinct`=7 not in 13..24; `steer_distinct`=7 not in 13..30; `abs_steer_median`=4 not in 8..37 | **FAIL** `c0_distinct`=7 not in 13..35; `c1_distinct`=82 not in 21..70; `abs_steer_median`=6 not in 7..23 |
| 2 | FAIL `c0_distinct`=28 not in 13..24; `c0_median`=23 not in 0..10 | **PASS** |
| 3 | **PASS** | **PASS** |

**Flag-seeded build** (`MASHED_AI_DIFFFLAG=1`, `flag_a368` = 1 on 220/220, so
`NOT_IN_REGIME` for regime 0 and no pure regime-1 envelope exists to score against):

| car | vs regime 1, K=159 |
|---|---|
| 1 | FAIL `c1_median`=5 not in 0..0 |
| 2 | FAIL `c0_distinct`=35 not in 3..24; `c1_distinct`=11 not in 17..41; `c0_median`=10 not in 0..0 |
| 3 | FAIL `c1_distinct`=5 not in 17..41 |

**The three-way statement §9.2 step 3 asked for, per car:**

- **Car 1: inside NEITHER regime.** It fails regime 0 on 3 of 10 bands at both K=61 and
  K=220, and it fails regime 1 on 1 band. This is the load-bearing result: car 1's
  failure is **not** explained by the pooled envelope being a two-regime union. Measured
  against 24 same-regime, same-K original observations it still falls outside on
  `c0_distinct` (7, floor 13), `c1_distinct` (82, ceiling 70) and `abs_steer_median`
  (6, floor 7).
- **Car 2: inside regime 0 at K=220, outside it at K=61, outside regime 1.** The K=61
  disagreement is a launch-window effect, not a contradiction: K=61 is the first 61 calls
  of the race and covers the launch, where the original's own spread on `c0_median` is
  0..10 and the standalone sits at 23.
- **Car 3: inside regime 0 at both K, outside regime 1.**

Both standalone regimes are *reachable* regimes of the original, so no comparison here is
against an unobserved population. Neither standalone configuration reproduces the
original's actual window, which is flag 0 for 61 calls then flag 1 for 159 (§3).

### 10.5 MEASURED (addition) — speed, against the original's own run-to-run spread

§7 recorded "standalone opponents run 16-23% fast" from a single original capture (`o4`,
which was a flag-1 run). §9.2's closing paragraph asked for the same comparison against
the original's run-to-run spread. `rec_9e4` median over the full 220-call window:

| car | orig, 8 non-flip runs | orig, 2 flip runs | standalone default | standalone flag=1 |
|---|---|---|---|---|
| 1 | 2419..2419 | 2422..2422 | **2821** | 1864 |
| 2 | 2396..2396 | 2376..2376 | **2818** | 1860 |
| 3 | 2618..2618 | 2298..2298 | **2820** | 858 |

**The original's run-to-run spread on this statistic is zero to the integer** within each
regime group, on all three cars. So the excess is not run noise:

- default standalone vs non-flip original: **+16.6% / +17.6% / +7.7%** (cars 1/2/3).
- default standalone vs flip original: +16.5% / +18.6% / +22.7%.

§7's "16-23%" therefore holds for cars 1 and 2 against any original regime, and for car 3
only against the flag-1 original (against the flag-0 original car 3 is +7.7%). The
finding survives; its range is **+7.7% to +22.7%** depending on car and original regime.
The flag-seeded standalone overshoots the other way (-23% / -22% / -67%). §8 item 3's
next step (compare against the D2 physics capture rather than the AI one) is unchanged
and still open.

Regime-truncated speed is also in `regime_split.json`. Note it inverts: over the first
K_0 = 61 calls the default standalone is **slower** (median 705 vs the original's
1586..2278). The standalone is slow off the line and fast thereafter. Not tested by any
(b) band.

### 10.6 What this changes in §8

- **§8 item 1 option (ii) is REFUTED by §10.3 + §10.4.** The pooled envelope is not
  meaningfully wider than the single-regime envelope on the columns car 1 fails, and car
  1 lands outside both regimes. "The standalone simply lands outside both" is the
  measured answer, and it is not exculpatory.
- **§8 item 1 option (i) is untouched and remains the only identified path that would
  close (b) by porting.** Its blocker is unchanged (§6: `FUN_00414c30` needs the world-
  object query `FUN_00484c70`).
- §8 items 2, 4, 5, 6, 7 unchanged. §8 item 3 refined by §10.5.

### 10.7 The decision this hands to the user

(b) for car 1 cannot be closed by reinterpreting the envelope. The options, stated
neutrally, are in `ROADMAP.md` §D3's terms and none of them is taken here:

1. **Port option (i).** Port `FUN_00414c30` and its world-object dependency chain
   (`FUN_00484c70`, `FUN_0041f030`, `FUN_0048a630`, `FUN_00414300`/`FUN_00414490`,
   `FUN_00442cc0`) so behaviour modes 3 and 7 exist in the standalone. This is the only
   route identified that would close (b) by making the port more faithful. Cost: a
   subsystem the standalone does not have at all. No evidence here says it *would* close
   (b), only that the modes it produces are the largest unported difference (car 1 is in
   mode 7 for 115 of 220 original calls).
2. **Accept (b) as met on 2 of 3 cars and record car 1 as a named D3 residue** with the
   three failing band values cited, deferring it behind D5 like the D1 residue block.
3. **Change the criterion** so that (b) is scored per `DAT_0089a368` regime rather than
   against a pooled envelope, and state which regime the standalone is required to match.
   §10.2 shows the regimes are genuinely disjoint on `accel_median`, so this is a
   defensible reading, but it would not make car 1 pass (§10.4) and it is a criterion
   change, which is the user's call and is not made here.
4. **Re-scope (b)'s bands.** Not evaluated and not recommended from this evidence: no
   measurement here supports a specific new band, and moving a band to admit the value
   that currently fails would make the criterion unfalsifiable on this function.
