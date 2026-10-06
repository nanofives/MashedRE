# PRE-REGISTRATION — D-11072 leg C liveness probe + leg A scale measurement

**COMMITTED UNRUN.** Nothing in §3..§6 has been executed. §1 and §2 are already-established
facts and are stated as *results*, not gates, so a reader can tell the two apart.

Anchor `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` to be re-verified
before any tool runs and before each arm is captured. Scope doc:
[`re/analysis/RACE_POSITION_RECON_SCOPE_2026-10-06.md`](../../re/analysis/RACE_POSITION_RECON_SCOPE_2026-10-06.md).

## 0. The USER DECISION this rests on, recorded

Asked and answered 2026-10-06 (Mariano): **a bridged, non-bit-identical race-position
substrate IS acceptable in the default build, knob-gated, with no C-level promotion.** Same
class as `CarDropNonRenderAtomics`. Every leg of D-11072 inherits that: **no function's
C-level moves because of a bridge**, and each leg ships only behind a knob that reverts
cleanly. This pre-registration covers the two cheapest legs and **writes no bridge**.

## 1. ALREADY MEASURED (results, not gates) — three facts the legs stand on

1. **`0x005f2770` is a load-time `.data` constant whose value is `0x005f2728`, and nothing
   writes it at runtime.** Ghidra `decomp_pc.py 0x005f2770 --datarefs`: `DATA in .data`,
   `type pointer len 4`, **`value 005f2728`**, **`WRITES: (none)`**, 40 reads. Independently,
   the port's own header read it out of `MASHED.exe.unpatched` at file offset `0x1f2770`
   (`Ai/AiStandalone.cpp:1388-1390`). Two witnesses, one static and one from the file image.
   **Consequence for fidelity:** seeding it reproduces a *static initializer the binary itself
   carries*. Leg C is therefore **not** a bridge and does not consume the §0 decision.
2. **The consumer expression is exact.** `FUN_0040e470` is 15 bytes and is
   `return *(undefined4 *)(PTR_PTR_005f2770 + param_1 * 4 + 0x34);` (`0x0040e474` reads the
   pointer). `FUN_0040e480` is the matching setter at the same displacement. 29 callers of the
   getter, all original-side.
3. **The original's `race_pct` is ALREADY CAPTURED and committed.**
   `verify/d3_elim_20261003/o_e1.msd.alive.csv` and `o_e2...` carry `pct0..pct3` =
   `0x008a96ec + v*0x30c` (`re/frida/scenario_launch.py:1560,1605`), 220 rows each. **No new
   original-side run is needed for leg A.** (While scoping I read the header and the first and
   last data rows of `o_e1`; every statistic gated in §6 is computed after this file is
   committed.)

## 2. ALREADY ESTABLISHED (result, not gate) — leg C is inert by construction, and that is the registered prediction

The scope doc lists leg C's payoff as `FUN_0040e470`'s consumers (`LeaderInRange`
`FUN_00415190`, the participant-count loop at `0x0040ff40`). **Those are original-side
functions the standalone never calls** — `0x00400000..0x004fffff` is entirely unmapped in the
standalone (`Compat/StandaloneRvaThunks.h:7`). The standalone's one consumer of the *concept*
is `Ai::Host::veh_type` (`Ai/AiStandalone.h:42`, read at `AiStandalone.cpp:717,1533,1709`),
and it is wired to a **synthesized constant**: `aib_veh_type(v) { return v == 0 ? 0 : 2; }`
(`D3d9Render/TrackRenderer.cpp:93`).

So the seed makes `CarSlotStateSet` (`AiStandalone.cpp:1404-1409`) stop early-returning and
land its already-coded `(v, 2)` pokes in real memory — **and nothing in the standalone reads
that memory.** Leg C1 is therefore predicted **behaviourally inert**, and `G-C-INERT` below
is that prediction stated as a falsifiable gate rather than an assumption. The behavioural
content of leg C is a *separate* step **C2 — re-point `aib_veh_type` at the table**, which is
not written, not run and not gated here; it matters because the table holds **0** for a car
`CarSlotStateSet` never marked, so a table-backed `veh_type` becomes death-aware where the
constant is not.

**Name collision, recorded so a later session cannot misread it:** `Ai::kSlotTableBase` is
**`0x007f1a14`** (`Ai/AiState.h:41`, the AI *ctrl-slot index* table, already seeded at
`TrackRenderer.cpp:363`). It is a different table from the slot-**state** table at
`0x005f2728 + 0x34 + v*4`. Nothing in §3 touches `0x007f1a14`.

## 3. THE CHANGE — one default-OFF seed and seven appended default-OFF columns

Two files, both changes inert unless an env var is set.

**(a) The seed.** In `Ai_BridgeLoad` (`D3d9Render/TrackRenderer.cpp:361`, beside the existing
one-time pad pokes at `:362-379`), behind a **default-OFF** knob in this file's established
idiom (`std::getenv(...) != nullptr`, as `MASHED_GATE_RIBBON_AI` at `:2176`):

```
MASHED_SLOTSTATE_SEED  ->  Ai::I32(0x005f2770u) = 0x005f2728;   // value cited §1.1
```

**(b) The instrument.** Seven columns **APPENDED** to the end of the existing default-OFF
`MASHED_AI_STEPDUMP` header and row (`TrackRenderer.cpp:3973-4080`), so all 65 existing
columns keep their positions and `ai_speed_env.py` / `ai_ctrl_window.py` / `ai_posmatch.py` /
`ai_yawrate.py` / `ai_armregime.py` are unaffected (the same append pattern legs 1 and 3 used):

| column | expression | why |
|---|---|---|
| `ss_base` | `U32(0x005f2770)`, **no deref** | leg C: did the seed take |
| `ss_v` | guarded `*(u32*)(ss_base + v*4 + 0x34)`, or **-1** when `ss_base == 0` | leg C: the exact `FUN_0040e470(v)` expression of §1.2 |
| `ss_raw` | raw dword at `0x005f2728 + 0x34 + v*4`, **absolute, deref-free** | leg C: a witness that needs no pointer and so cannot AV in either arm |
| `prog` | `race_[v].progress` | leg A |
| `lap` | `race_[v].laps` | leg A |
| `gate` | `race_[v].gate` | leg A |
| `racepct` | `TrackRenderer::RacePct(v)` = `fmod(progress, n)/n*100` (`TrackRenderer.cpp:4616-4619`) | leg A: the port's existing 0..100 per-lap analogue, the candidate bridge value |

**`ss_v` is guarded and the deviation is stated here, not discovered later.** Without the
seed the literal `FUN_0040e470` expression dereferences 0 and the port's own header records
that exact measured AV (`0xC0000005` at `AiStandalone.cpp:1385-1393`). "Reads 0 without the
seed" is therefore only achievable on a guarded path, so `ss_v` reports **-1** for
`ss_base == 0` and `ss_raw` carries the deref-free witness instead.

No other file is touched. No default-path behaviour changes: both knobs default OFF and the
dump is already default-OFF.

## 4. THE RECIPE — identical to the car<->car legs, reused verbatim

Standalone, via `re/tools/sa_capture.py` from the repo root, sequential runs, **PIDs tracked
and only this session's PIDs killed**:

```
py -3.12 re/tools/sa_capture.py verify/d3_racepos_20261006/<tag> 8,30,60 \
    MASHED_MUTE=1 MASHED_CAR=1 MASHED_ROUND=1 MASHED_WIN_POS=primary-bl \
    MASHED_AI_STEPDUMP=verify/d3_racepos_20261006/<tag>.csv [MASHED_SLOTSTATE_SEED=1]
```

Arms: **`U1`,`U2`,`U3`** (unseeded, three repeats) and **`S1`,`S2`,`S3`** (seeded, three
repeats). Baseline **`B1`** = the committed build with the instrument `git stash`ed out and
rebuilt, for `G-INERT`. Scorers: `re/tools/ai_speed_env.py --check` (criterion (e)) and
`re/tools/ai_ctrl_window.py --check` (criterion (b) bands), both unedited.

## 5. LEG C GATES — four, each with its threshold and the number it is compared against

| gate | threshold | compared against | denominator |
|---|---|---|---|
| `G-C-SEED` | `ss_base` == **0x005f2728** on **100 %** of seeded rows, and == **0** on **100 %** of unseeded rows | the §1.1 static value `005f2728` | print `ok / total_rows` for each arm |
| `G-C-EXPR` | seeded: `ss_v` == **2** on **>= 95 %** of rows for each v in {1,2,3}; unseeded: `ss_v` == **-1** on **100 %** | the `(v, 2)` poke at `AiStandalone.cpp:1699-1701` | per car: `rows_with_2 / rows_for_v / total_rows` |
| `G-C-TABLE` (**the control that can fail**) | `ss_raw` == **0** on **100 %** of unseeded rows **AND** == **2** on **>= 95 %** of seeded rows, for each v in {1,2,3} | the null hypothesis "the seed changes nothing" predicts **0 in both arms**; deref-free, so it cannot be an artefact of the guard | per car and per arm, all three printed |
| `G-C-NOCRASH` | all **3 of 3** seeded runs reach the 60 s shot and write a non-empty CSV | the unseeded arm's own 3/3 | `runs_ok / runs` |

**Registered FAIL clause.** If `G-C-SEED` or `G-C-TABLE` fails, **leg C stops** and the
reported finding is "the table hypothesis is wrong" — step C2 is not attempted and the scope
doc §2 is corrected. `G-C-EXPR` failing while `G-C-TABLE` passes means the pokes are not
running (an `aiRound`/`car_alive` gate), which is a different and separately reported defect.

| gate | threshold | compared against |
|---|---|---|
| `G-C-INERT` (**the §2 prediction, falsifiable**) | criterion (e): all **six** `ai_speed_env.py --check` digits identical between the seeded and unseeded arms; criterion (b): **identical** per-car band counts and band names | the unseeded arm, captured in the same session from the same exe; (e) reference `launch` **1426.4 / 2053.0 / 2055.2**, `ft_median_m0` **2550.6 / 2053.0 / 2278.2** |
| `G-INERT` (instrument) | **0** differing cells over the pre-existing columns | `B1`, the committed build with the instrument stashed out and rebuilt; print `cols_before x shared (frame,seq,v) keys` |
| `G-DET` | the two gated (e) statistics and the band counts identical across **3 of 3** repeats, each arm | each arm's own repeats |
| `G-BANDS-UNEDITED` | `git diff --stat` and `git status --porcelain` **empty** for both scorers, before the first run and after the last | — |

**If `G-C-INERT` fails, that is the session's headline, not an error**: it would mean a
standalone consumer of `0x005f2728` exists that §2's survey missed, and the consumer map in
the scope doc is wrong.

**Not claimed, registered now so it cannot be claimed later:** leg C moves **no** C-level.
`0x0040e470` stays where it is; a seeded pointer is not a behavioural diff. Nothing here
ships default-ON — the shipping decision for C1/C2 is a separate, later ask.

## 6. LEG A GATES — the scale map is DECIDED BY A RULE WRITTEN BEFORE THE NUMBERS

New tool `re/tools/racepct_scale.py`, read-only, offline over committed CSVs.

**M1 — the original's `race_pct` semantics**, over `o_e1` + `o_e2` `alive.csv`, cars 0..3
(8 series, 220 rows each; print `n` per series):

| gate | threshold | compared against |
|---|---|---|
| `G-A-RANGE` | every sample in **[0, 100]** on **100 %** of samples | the 0..100 percentage hypothesis; print min/max per car |
| `G-A-WRAP` | every negative step `pct[i+1] - pct[i] < 0` has magnitude **> 50** (i.e. it is a 100->0 wrap, not a regression) on **100 %** of negative steps | the alternative "pct can decrease mid-lap"; print `wraps / negative_steps / total_steps` |
| `G-A-MONO` | **>= 99 %** of non-wrap consecutive steps are **>= 0** | the monotone-within-lap hypothesis |

**M2 — the port's metric**, over `S1`/`U1` `racepct`, cars 1..3, and **the discriminator that
decides the whole leg**. The port's `frac` is **0 whenever the car is more than 8 units from
the next gate centre** (`TrackRenderer.cpp:4859`), so `progress` — and `RacePct` with it — is
predicted to be a **staircase with flat treads**, while the original's spline projection is
predicted smooth. Define, identically on both sides:

> `flat_frac` = (consecutive steps with `|Δpct| < 1e-6` while the car is moving, i.e.
> `rec_9e4 > 200` on the port and `vstep != 0` on the original) / (such steps total).
> Both numerator and denominator printed per car.

| gate | threshold | compared against |
|---|---|---|
| `G-A-SHAPE-ORIG` | original `flat_frac` **< 0.05** on each of cars 1..3 | — |
| `G-A-SHAPE-PORT` | port `flat_frac`, **reported**; the registered *prediction* is **>= 0.50** | the original's, same statistic, same definition |

**The verdict rule, fixed now:**

- **If `port flat_frac < 5 x orig flat_frac`** (and `G-A-RANGE`/`G-A-WRAP`/`G-A-MONO` pass):
  the two metrics share a scale by range-mapping, and the **candidate map is registered UNRUN**
  as `0x008a96ec + v*0x30c = RacePct(v)` and `0x008a96e8 + v*0x30c` left for a separate
  measurement (it is `path_prog`, a different field, `camera_probe.py:133-134`).
- **If `port flat_frac >= 5 x orig flat_frac`**: the registered verdict is **"NOT on a common
  scale by range-mapping"** — the port's gate-ordinal metric is piecewise-flat where the
  original's is smooth, so the bridge needs an arc-length-proportional port metric (or the AI
  gate thresholds re-derived), and **that is the reported finding**. Leg A does **not** then
  proceed to write a map, and leg B stays unstarted.

Either way **no bridge code is written this session** and `0x008a96e8`/`0x008a96ec`/
`0x008989b0` are **not** seeded — `verify/d3_modes37_20261002/RESULT_STEP2.md`'s rule
"seeding the globals would not be a port" is applied unchanged.

## 7. What this pre-registration refuses to do

1. **Not call "the table is writable" a payoff.** `G-C-INERT` predicts inertness and the
   result will say so plainly, as U-9186 did for its two branches.
2. **Not pick the scale map after seeing the numbers.** §6's verdict rule names the
   multiplier (5x) and both outcomes before the tool exists.
3. **Not score a denominator without its population.** Every gate above prints numerator,
   denominator and the total drawn from.
4. **Not start leg B.** Explicitly out of scope (scope doc §6: "Do not start Leg B first").
