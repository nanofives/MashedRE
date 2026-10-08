# RESULT — the original's `bias374` ramp: the LADDER is corroborated; the gap is RACE LENGTH

Date 2026-10-08. Follows `RESULT_BIAS374.md` §5, chosen by **USER DECISION (Mariano,
2026-10-08)**. **READ-ONLY on the game** — `ReadProcessMemory` poll, no injection, no hooks of our
own, no writes, no build. `original/` untouched. No C-level.

Tool: `re/tools/orig_rampwatch.py` (new). Raw: `o_ramp.csv`, 2,715 samples at 10 Hz over 269 s.
Process hygiene: zero MASHED.exe were running before; one new PID (40612) was adopted by
before/after set difference; this tool never killed it.

## 0. Verdict first

> **The port's band ladder is CORROBORATED, not refuted.** The original fires band 1 at
> `tickscale` **11.05 / 11.07 / 11.08 / 11.10 s** across four independent crossings; the port fires
> it at frame 661 = **11.0 s**. Same threshold, both sides.
>
> **The original never reaches bands 2, 3 or 4 in this capture** — `tick_0ff8` peaks at **55,600 =
> 18.53 s**, below band 2's 22 s — because it is **RESET 20 times**. The port never resets it and
> so ramps monotonically to band 4.
>
> **The divergence is REAL and its cause is a single missing reset.** The original zeroes
> `0x007f0ff8` roughly **every 8 s**; the port never zeroes it. See §2, **corrected** by a second
> capture.

## 1. What is established

**Band 1's threshold matches to three significant figures.** Original crossings, `tickscale`
in seconds (`tick_0ff8 * 1/3000`, `_DAT_005cc948`):

```
 t=188.87s  tick=33200  tickscale=11.0667 -> bias374=1
 t=213.70s  tick=33150  tickscale=11.0500 -> bias374=1
 t=231.97s  tick=33250  tickscale=11.0833 -> bias374=1
 t=269.06s  tick=33300  tickscale=11.1000 -> bias374=1
```

Port: `bias374` 0→1 at frame **661** = 11.02 s. So `AiStandalone.cpp:1557-1573`'s ladder
reproduces the original's band-1 edge. The C2 demotion of `FUN_004177b0`'s exe copy does **not**
show up here.

**`tick_0ff8` is reset, 20 times, and the port never resets it.** Drops are sharp and to ~0:
`900→50`, `11000→50`, `19750→50`, `11500→0`, `25250→50`, `27550→50` … consistent with
`Ai/AiState.h:87`'s note that `0x007f0ff8` is zeroed on mode-5 entry. The port's
`A364on.gates.csv` shows a strictly monotone ramp with **zero** reversals over 13,498 frames.

## 2. CORRECTED — it is the RESET CADENCE, not race length

> **This section's original hypothesis was wrong and a second capture refuted it.** I wrote that
> the gap was scenario length. It is not. `o_ramp60.csv` (716 samples, the `o_t1` scenario,
> `--hold 60`) ran **2,600 frames = 43.3 s of race clock** — more than twice the ~17 s I had called
> the race length — and `tick_0ff8` **still never exceeded 15,050 = 5.02 s**, with `bias374` at
> `0` on **716/716**. The reason is **9 resets in 72 s, about one every 8.0 s**, all at `substate`
> 5 (×7) or 4 (×2) — exactly the mode-5 zeroing `Ai/AiState.h:87` names.
>
> So `tick_0ff8` in the original is a **sawtooth that is re-zeroed faster than the 11 s band-1
> threshold can be reached**, while the port's is a monotone 240 s ramp with **zero reversals in
> 13,498 frames**. The divergence is real, and it reduces to **one missing reset**, not to the
> ladder (§1 shows the ladder agrees) and not to race length.
>
> The superseded reasoning is kept below because it is why the second capture was taken.

### Superseded: the race-length hypothesis

At these settings the **original's race is short**. `o1.msd.aistep.csv` is 1,014 frames ≈ **17 s**,
and this capture's `tick_0ff8` never exceeds **18.53 s**. Band 2 needs **22 s**. So **the original
never exercises bands 2, 3 or 4 at all** — not because its ladder differs, but because the race
ends first.

The port's capture is **14,400 frames = 240 s**, held open by `MASHED_DET_FRAMES`. It therefore
walks the entire ladder and parks in band 4 for 9,834 frames.

**So `RESULT_GF0.md`'s "port `{0,1,2,3,4}` vs original `0`" is comparing a 240 s race against a
17 s one.** That is the third framing of this value in two days, and it is the first one measured
on both sides. The gap is scenario length; no ladder defect is in evidence.

## 3. What this capture CANNOT establish — the load-bearing caveat

**It almost never drove.** `substate` (`0x0063ba8c`) histogram over 2,715 samples:

| substate | 6 | 5 | 7 | 0 | 11 | 9 | 10 | 2 | 3 | 4 | 8 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| rows | 1653 | 378 | 268 | 146 | 100 | 88 | 67 | 5 | **4** | 4 | 2 |

`--hold 260` outlived a ~17 s race, so ~240 s of the capture is post-race standings and restarts.
**All 20 resets occurred at substate 5 / 4 / 2, none while driving.** So:

- The reset *cadence during a race* is **unmeasured**. The 20 resets are round transitions.
- "The original never exceeds 18.53 s" is a property **of this capture**, not proven of a race.
- `bias374` while driving: `0` on all 4 such rows, at `tickscale` 0.07-0.30 s — too early to inform
  anything.

The band-1 threshold figures in §1 stand regardless, because they are threshold crossings and do
not depend on which substate the game was in.

## 4. Two side findings, both correcting earlier text

**(a) `RESULT_A360.md`'s `[UNCERTAIN]` resolves AGAINST my inference.** I wrote that the
original's `0x0089a360 = 2.5` was "almost certainly `FUN_00431d80() * 2.5` with the tiebreak flag
at 1" — the **mode-6** arm. The measured joint distribution says **mode-10**:

| `submode` (`0x0067e9fc`) | `flt360` | rows |
|---|---|---|
| 10 | **2.5** | **442** |
| 3 | 0.0 | 1653 |
| 3 | 1.0 | 448 |
| 0 | 0.0 | 127 |
| 3 | 2.5 | 26 |
| 10 | 0.0 | 19 |

`0x0067ea7c` (`RaceConfig.difficulty`, `scenario_launch.py:47` — **not** a "tiebreak flag" as
`SplashGameMode_t5.cpp` names it) is **`0` on all 2,715 samples**, which would make the mode-6 arm
produce `0.0`. `flt360 = 2.5` occurs with `submode = 10` on 442 of 461 mode-10 rows. So the `2.5`
comes from `FUN_0042fe80() * 2.5` (the **mode-10** arm), not the mode-6 arm. Association, not
proof of causation — but it is evidence, and it points the other way from what I wrote.

**(b) `idx364` is NOT always `-1` across a session.** This capture sees `{1: 1672, -1: 570,
3: 327}`. The witness's `-1` on 512/512 was true *within a race window*. This is consistent with
the two writers (`RESULT_IDX364.md`): `FUN_00414060` resets to `-1`, `FUN_00414220` writes the
sole-finisher index once a race completes — and this capture is mostly post-race. The step-6 port
(`RESULT_A364.md`) writes only the `-1` reset, which remains correct **as a reset**; it does not
reproduce `FUN_00414220`'s finisher write, and never claimed to.

## 5. The second capture, and what both agree on

`o_ramp60.csv` — `o_t1`'s exact argv, `--hold 60`, 716 samples:

| | `o_ramp` (hold 260) | `o_ramp60` (hold 60) | **port** |
|---|---|---|---|
| race clock reached | 191,850 (3,837 frames) | 130,000 (**2,600 frames, 43.3 s**) | 14,400 frames (240 s) |
| `tick_0ff8` max | 55,600 = **18.53 s** | 15,050 = **5.02 s** | monotone to ~183,200 = **61.1 s** |
| resets of `tick_0ff8` | **20** | **9** (≈ one per 8.0 s) | **0** |
| `bias374` seen | `{0, 1}` | **`{0}` on 716/716** | `{0,1,2,3,4}` |

Both original captures reset; neither ever reaches band 2's 22 s. The port never resets and parks
in band 4 for 9,834 frames. **Two independent captures, same conclusion.**

Side finding (a) also replicates: `o_ramp60` has `submode = 10` on 608/716 and `flt360 = 2.5` on
**589/716**, so the mode-10 reading of `0x0089a360 = 2.5` now rests on two captures, not one.
`idx364` is again not pinned at `-1` (`{3: 233, 1: 201, 0: 127, -1: 78}`).

## 6. Next

**The remaining question is narrow and named: what zeroes `0x007f0ff8` in the original, and does
the port need it?** `Ai/AiState.h:87` already attributes it to `FUN_00418560`'s mode-5 path. Two
ways forward, and the choice is a scope call:

1. **Port the reset.** Find `FUN_00418560`'s zeroing site and reproduce it. If the port then
   re-zeroes on the same transitions, `bias374` should collapse to the original's behaviour and
   stop being a blocker for branch 2.
2. **Decide it does not matter for branch 2.** `bias374` feeds only the limit-table index
   (`bias374 + iVar1*5`). If the original is pinned at `0` and `iVar1 = 2`, its index is **10** —
   the entry `RESULT_WITNESS.md:106` already measured as the only one ever used, value `1`. A port
   that reproduced `bias374 = 0` would read the same entry. So the practical requirement may be
   just "keep `bias374` at 0", which the missing reset is actively preventing.

**[UNCERTAIN], carried:** neither capture sustained `substate == 3` (5 rows and 4 rows
respectively), so the reset cadence *while driving* is still not directly observed — though with
resets every ~8 s across 43 s of race clock, and `bias374` pinned at `0` throughout, the margin to
the 11 s threshold is large enough that a driving-only cadence would have to differ by more than 2x
to change the conclusion.
