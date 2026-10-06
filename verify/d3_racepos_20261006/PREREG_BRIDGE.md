# PRE-REGISTRATION — D-11072 the race_pct bridge write (0x008a96ec)

**COMMITTED UNRUN.** Nothing in §3..§6 has been executed. §1/§2 are already-established static
facts, stated as results. Anchor `BDCAE093…` re-verified before any tool runs and before each
arm. Builds on [`RESULT_LEGA_MONOTONE.md`](RESULT_LEGA_MONOTONE.md) §4 (the near-identity map,
registered there) and the fidelity decision (knob-gated bridge, no C-level).

## 0. What this is

Write the port's monotone `arcpct(v)` into the original per-car `race_pct` slot
`*(float*)(0x008a96ec + v*0x30c)`, behind a **default-OFF** knob, and measure (a) that a **live
standalone reader** consumes it and (b) whether it changes any behaviour. No C-level moves; a
bridged write is not a behavioural diff (registered in advance).

## 1. ALREADY ESTABLISHED (static, results not gates) — the one live race-path reader, and its gate

`0x008a96ec + v*0x30c` is read on the standalone's **live** path at exactly one race-path site:
`Ai/AiStandalone.cpp:1468`, inside `AiPreTickRubberBand` (called unconditionally from
`Ai_Standalone_Tick:1705`, which runs every frame via `TrackRenderer.cpp:3462`):

```
const float ra  = F32(0x008a96ecu + v*0x30cu);          // :1468  (FUN_00408ad0)
const float val = ra * kRaceMetricScale + a20f;         // :1470  kRaceMetricScale=0.01
F32(0x0089a880u + v*4u) = val;                           // :1471  (FUN_00417730 storage)
```

So `ra` → `val` → `0x0089a880`. The **downstream of `val` is game-mode-gated**: the finish-order
candidate slots (`0x0089a870..87c`) are written only when `fd0 ∈ {4,9,7,8}` (`:1472`), and
`0x0089a880` is re-read only by the mode-9 (`:1492`) and mode-4 (`:1505`) speed-scaling blocks.
The standalone runs **`fd0 = aib_game_mode_fd0() = 0`** (`TrackRenderer.cpp:97`), so all three
downstream consumers are skipped. The other `0x008a96ec` readers (`SmallLeaves_t1.cpp`,
`MenuLeaves_af4.cpp`) are frontend/menu, not the race path.

**Therefore the static prediction is: the write reaches a live READ (`ra`/`val` change) but has
NO behavioural effect at `fd0=0`.** §4's gate states that prediction as a falsifiable measurement.

**Frame order is correct for a same-frame read:** `UpdateRace` (`:3423`, where `arcprog` is
computed) runs before `Ai_Standalone_Tick` (`:3462`), so a write placed in `UpdateRace` is seen by
`AiPreTickRubberBand` the same frame.

**Separate field, out of scope here (noted so it is not conflated):** the start-boost order
`Fi_UpdateBoostOrder` (`Vehicle/ForceIntegratorStubs.cpp:176`) reads a *different* field,
`0x008a96e8` (path_prog, FUN_00408a50), ungated — but at the lights all cars' progress is ~equal
so its boost-window effect is null (its own `[UNCERTAIN] U-D3-BOOST-ORDER`). This pre-registration
writes **only** `0x008a96ec`; `0x008a96e8` is a later question.

## 2. ALREADY ESTABLISHED — arcpct is the right value for this slot

`RESULT_LEGA_MONOTONE.md`: `arcpct` is monotone (0/0/0 backward-midlap) and is a physical-arc
0..100 per-lap percentage — the same semantics as the original `race_pct` this slot holds. So the
near-identity write `slot = arcpct(v)` is the registered candidate map.

## 3. THE CHANGE — one default-OFF write + two appended witness columns

**(a) The write.** In `TrackRenderer::UpdateRace`, after the `step()` loop computes `arcprog` for
all cars, behind a **default-OFF** knob `MASHED_RACEPCT_BRIDGE` (file idiom
`getenv(...) != nullptr`):

```
for v in 0..3:  *(float*)(0x008a96ec + v*0x30c) = arcpct(v)
```
where `arcpct(v) = fmod(race_[v].arcprog, total_len_)/total_len_*100`. Writes all four cars (the
original writes race_pct for every participant and `AiPreTickRubberBand` loops v=0..3).

**(b) Witnesses.** Two columns APPENDED to the default-OFF `MASHED_AI_STEPDUMP` (positions of the
67 existing columns preserved):

| column | expression | proves |
|---|---|---|
| `ra_ec` | `F32(0x008a96ec + v*0x30c)` read at dump time | the write took (== arcpct ON, == 0 OFF) |
| `val_880` | `F32(0x0089a880 + v*4)` read at dump time | **the live reader consumed it**: `AiPreTickRubberBand:1471` stores `ra*0.01 + lap` here, so ON it tracks `arcpct*0.01 + lap`, OFF it is `0`/lap-only |

No other file touched; both knob and dump default OFF.

## 4. THE RECIPE + GATES

Recipe identical to `RESULT_LEGC_LEGA.md` (Training, `sa_capture.py`, own PIDs only). Arms:
**`N1,N2,N3`** (bridge OFF) and **`Y1,Y2,Y3`** (`MASHED_RACEPCT_BRIDGE=1`). Baseline `B1R` = the
committed build with the instrument stashed out and rebuilt, for `G-INERT`. Scorers
`ai_speed_env.py --check`, `ai_ctrl_window.py --check`, unedited.

| gate | threshold | compared against |
|---|---|---|
| `G-WROTE` | `ra_ec` == `arcpct` on **100 %** of `Y` rows; == 0 on **100 %** of `N` rows | the write itself; per car, print `ok/rows` |
| `G-LIVE` (**the liveness gate; the control that can fail**) | on `Y`, `val_880` within `1e-3` of `arcpct*0.01 + lap_a9648` on **≥ 95 %** of rows AND differs from the `N` arm's `val_880`; on `N`, `val_880` does not track arcpct | null "no live reader consumes the write" predicts `val_880` identical in both arms; the finding predicts `Y`'s `val_880` tracks the bridged `ra`. Distinguishable by inspection. `lap_a9648` = `I32(0x008a9648 + v*0x30c)` dumped or 0 |
| `G-EFFECT` | criterion (e) six digits and (b) per-car bands, `Y` vs `N` | **prediction: IDENTICAL (inert at fd0=0)**; (e) ref `launch` 1426.4/2053.0/2055.2, `ft_median_m0` 2550.6/2053.0/2278.2; (b) 5 bands (car 2) |
| `G-INERT` | 0 differing cells over the 67 pre-existing columns | `B1R`; print `cols × shared (frame,seq,v) keys` |
| `G-DET` | (e) digits + band counts identical across 3 repeats each arm | each arm's repeats |
| `G-BANDS-UNEDITED` | both scorers' `git diff`/`status` empty | before first, after last run |

## 5. The verdict rule, fixed now

- **`G-WROTE` + `G-LIVE` PASS and `G-EFFECT` identical (predicted):** the finding is **the
  race_pct bridge is CORRECT and SEEN by a live reader, but behaviourally INERT at `fd0=0`** because
  `AiPreTickRubberBand`'s downstream is game-mode-gated. The bridge is **necessary but not
  sufficient**; the next D-11072 blocker is the mode-0 execution path / consumer un-gating, not the
  progress state. This is a real advance (the state is now live and consumed) and is reported as
  such, not as a failure — same class as leg C.
- **`G-WROTE` PASS, `G-LIVE` FAIL:** the write took but no live reader consumed it — contradicts
  §1; re-examine the read-site map before trusting any consumer.
- **`G-EFFECT` CHANGES:** an ungated live effect exists; characterize it (which statistic, which
  car) and gate it on the full car<->car no-regression set + modes oracle + powerup sweep before
  any ship decision. Do **not** ship default-ON from this session regardless.

## 6. What this refuses to do

1. **Not call "the write took" a payoff.** `G-WROTE` alone proves nothing; `G-LIVE` is the real
   liveness gate and `G-EFFECT` is the behavioural one.
2. **Not ship default-ON.** The knob stays default-OFF this session; shipping is a later,
   separately-gated decision (and §5 predicts there is nothing to ship yet).
3. **Not write 0x008a96e8 / 0x008989b0 / seed any other global.** Only `0x008a96ec`.
4. **Not move any C-level.** A bridged write is not a behavioural diff.
5. **Not start leg B.**
