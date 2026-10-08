# RESULT — `bias374`: it is a TIME RAMP, and the recorded "disagreement" compared different windows

Date 2026-10-08. Follows `RESULT_GF0.md` §6 item 2, chosen by **USER DECISION (Mariano,
2026-10-08)**. **Static + re-analysis of committed captures. No run, no build, no code change.**
`original/` untouched. No C-level.

Sources: `verify/d3_gatefire_20261008/A364on.gates.csv` (port, 13,498 frames),
`verify/d3_leader_20261003/o1.msd.leaderprobe.csv` + `o1.msd.aistep.csv` +
`o1.msd.provenance.json` (original, 2026-10-03).

## 0. Verdict first

> **`0x0089a374` is not a per-frame value — it is a strictly monotone band index over elapsed race
> time.** In the port it steps `0 → 1 → 2 → 3 → 4` at frames **0 / 661 / 1262 / 2463 / 3664**, each
> transition exactly once, then holds `4` for the remaining 9,834 frames.
>
> **The "port `{0,1,2,3,4}` vs original `0`" framing compares a 13,498-frame port capture against a
> 220-frame original slice.** On a like-for-like frame window the gap is **one band**, not five.

## 1. CORRECTION to `RESULT_GF0.md` §2

`RESULT_GF0.md` reported `bias374` as "**DISAGREES, and WIDER than recorded**", with "`4` is new
and is the majority value at 72.9%" presented as information that widened the gap. **That
implication was wrong, and the cause is capture length.**

- The 2026-10-03 witness sampled **1,089 samples over 55 s** ≈ 3,300 frames and saw `{0,1,2,3}`.
- Band `4` begins at frame **3,664** — roughly **360 frames past the end of that capture**.
- My capture ran 14,400 frames, so it reached one more band.

So `4` is not new divergence; it is the same ramp observed for longer. The `72.9%` is simply the
fraction of a 240 s race spent in the terminal band. The underlying observation in `RESULT_GF0.md`
stands; the inference drawn from it does not, and it is corrected here rather than quietly
restated. (Memory `band-on-speed-compares-different-moments`, `rate-stats-per-sample-not-totals`.)

## 2. The port's ramp, measured

Scalar, one value per frame, from `A364on.gates.csv` (`v==0` rows):

| `bias374` | frames | first | last | median |
|---|---|---|---|---|
| 0 | 661 | 0 | 660 | 330 |
| 1 | 601 | 661 | 1261 | 961 |
| 2 | 1201 | 1262 | 2462 | 1862 |
| 3 | 1201 | 2463 | 3663 | 3063 |
| 4 | **9834** | 3664 | 13497 | 8581 |

**Exactly four transitions, all forward, none reversed.** Writer is `AiStandalone.cpp:1575-1577`
(`if (ecx != 0) { F32(0x0089a370) = tickscale; I32(0x0089a374) = edx; … }`), where `edx` is set by
the `tickscale` band ladder at `:1557-1573` — thresholds 2.0 / 12.0 / 22.0 / 42.0 / 62.0 against
`tickscale = I32(0x007f0ff8) * kTickScale`. It is a function of elapsed time by construction.

## 3. Where the original's window actually sits

From the committed captures, not inferred:

- `o1.msd.aistep.csv` spans `clk_0ff4` **0 .. 50,700** → the whole original capture is **1,014
  frames** (`framedt = 50` on both sides).
- `o1.msd.leaderprobe.csv`'s 512 calls span `clk_0ff4` **39,700 .. 50,650**, 220 distinct values →
  frames **794 .. 1013**, i.e. the **last 220 frames** of that capture, beginning at **78.3%** of it.
- Across that window the original's `bias374` is `0` on 512/512.

**The like-for-like comparison.** At frames 794-1013 the port is in band **1** (its band-1 range is
661-1261). The original is at **0**. So the two differ by **one band** over the only window where
both have data — the port's ramp is ahead, or the original's has not started by frame 1013.

That is a much smaller and more tractable claim than the one on record.

## 4. What is NOT claimed, and it is the load-bearing gap

- **Nothing is known about the original's `bias374` after frame 1013.** Its capture ends there
  (~17 s). Whether the original ramps at all, ramps later, or stays `0` for a whole race is
  **unmeasured**. The `RESULT_GF0.md` comparison set the original's first 17 s against the port's
  full 240 s.
- **Frame-zero alignment is [UNCERTAIN].** The port's `frame` is the gates dumper's own counter;
  the original's is `clk_0ff4/50` from capture start. Both are "frames since the captured race
  began" and both tick at `framedt = 50`, but whether frame 0 denotes the same race moment (green
  light vs. load complete) is not established. A one-band offset is within the range a start-offset
  could explain.
- That the port's ladder transcription is correct. `FUN_004177b0`'s exe copy is **C2, demoted
  2026-09-29** as "the finish-order fragment only", so the thresholds at `AiStandalone.cpp:1557-1573`
  carry no behavioural evidence. A transcription error would also produce a shifted ramp.

## 5. Next — one measurement settles it

**Capture the original's `0x0089a374` and `0x007f0ff8` over a FULL race** (14,400 frames, matching
the port's window), then overlay the two ramps. Until that exists, "the port disagrees with the
original on `bias374`" is not established — only "they differ at frames 794-1013".

A per-call hook on `FUN_004148b0` will not do it: that function fires only on a rare mode-5 path,
which is why the existing capture is a 220-frame slice in the first place. Options, cheapest first:

1. **`ReadProcessMemory` poll of `MASHED.exe`**, the shape `re/tools/sa_leaderwatch.py` already
   uses port-side — no injection, no Frida, reads `0x0089a374` + `0x007f0ff8` + `clk_0ff4` per
   sample while `scenario_launch.py` drives a full race.
2. A per-frame Frida entry hook on something that runs every frame, logging the same three.

Either way the gate is: **do the two ramps have the same transition frames?** If yes, `bias374` is
resolved and branch 2 is measurable. If the original never leaves `0`, then the port's ladder is
firing when the original's does not, and `FUN_004177b0`'s C2 demotion is the place to look.
