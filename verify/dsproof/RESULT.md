# Drawstream at the result frame — the world-coverage hypothesis is REFUTED (2026-08-16)

## What was being tested

The previous writeup (`verify/d1_measure/BISECT_ROUND_BOUNDARY.md`) concluded that the
D3D9 result screen is a full-frame sky because **the D3D9 world draw stops covering that
view from round 2**, and named the decisive test: get `MASHED_DBG_DRAWSTREAM3D` at the
result-screen frame, where `world.batches == 0` would prove the skip.

## Result: it does not skip

```
window 1:1000, Arctic, deterministic race demo
frames captured        1000
world-batch histogram  {13: 1000}
zero-world frames      NONE
```

**Every in-race render call submits the world, 13 batches, with no exception** — and the
result screens fall inside that window (the whole demo is 601-1200 in-race calls, bracketed
by testing which windows flush). So the world is drawn at the result screen. The coverage
hypothesis is wrong and is retracted.

## What this leaves

The orange fill is therefore drawn **over** a world that was submitted normally, not in
place of a missing one. That reopens the question of what draws it, with a new constraint:
whatever it is must also be present in the librw run, since only the world-submit path
differs between the two. The likeliest remaining shape is an overlay that librw's world
submit happens to paint over because of draw ORDER — which would make the orange a defect
present in BOTH paths that librw merely masks.

That is a hypothesis, not a finding. It has not been tested.

## Harness notes for whoever runs this next

- `MASHED_DBG_DRAWSTREAM3D` accepts `a:b`. **The file is only written once the counter
  passes `b`** (`DrawStreamDump_Race3DBegin`), so a window whose end exceeds the run's
  total in-race calls produces NO output at all — silently. `1:3000` and `1:1200` both
  wrote nothing for this reason before the range was bracketed.
- The counter counts **in-race render calls**, not global frames. `MASHED_DET_FRAMES=3000`
  yields roughly 601-1200 of them here.
- Default (`=1`) is the 60:62 early-race window, which is why every earlier look at this
  data showed a healthy world and said nothing about the result screens.


---

# CORRECTION (same day): this refutation is WEAKER than stated above

Checked the rest of the capture and the instrument does not support the conclusion as
firmly as written.

## Every category is constant for all 1000 frames

```
cars      (284, 184296)   x1000
copters   ( 25,   5793)   x1000
props     ( 23,   5607)   x1000
sky       (  1,    258)   x1000
world     ( 13,  49440)   x1000
```

One distinct state each. Nothing varies anywhere in the run. These counters report
**geometry presence**, which is static once a track is loaded — they do not vary with what
the frame looks like, and they carry **no marker for which frames are result screens**.

## Why that matters

The claim "the world is drawn at the result screen" requires that result-screen frames are
among the 1000 captured. That was assumed, not shown:

- The whole demo is 601-1200 in-race calls; the window captured 1..1000, so any frames
  beyond 1000 — plausibly including `round3_result` — were **not** observed.
- Nothing in the dump identifies a frame as a result screen, so even for the frames that
  were captured, there is no way to point at one and say "this is round2_result".

The librw result frame does render 3D, which is good reason to think the result screen goes
through `TrackRenderer::Render` and therefore increments this counter. But that is an
inference, not a measurement.

## Honest status

**The coverage hypothesis is NOT refuted. It is untested** — the instrument as configured
cannot observe the specific frames in question. The earlier "REFUTED" heading overstated
what this data can carry.

## What would actually settle it

Tag each drawstream record with the GameFlow mode (`InRace` / `Results`) — one field in
`Frame3D`, written in `DrawStreamDump_Race3DBegin`. Then a result-screen frame is
identifiable by name and `world.batches` for that frame answers the question directly.
Until then this line of investigation is blocked on instrumentation, not on analysis.
