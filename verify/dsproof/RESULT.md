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
