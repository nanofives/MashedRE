# Settled: the result screen never re-renders the 3D world (2026-08-16)

## The instrumentation

`Frame3D` now carries the GameFlow mode, written in `DrawStreamDump_Race3DBegin` and
emitted as the first field of each record:

```json
"f1": {"mode": "InRace", "sky": {...}, "world": {...}, ...}
```

Added because the previous capture could not answer the question it was taken for: every
counter reports geometry PRESENCE, which is static once a track loads, so 1000 frames
showed one identical state and nothing identified a result-screen frame.

## The measurement

| window | frames | modes seen |
|---|---:|---|
| 1:1000 | 1000 | `{InRace: 1000}` |
| 1:1100 | 1100 | `{InRace: 1100}` |

**Zero `Results` frames in 1100 consecutive in-race render calls.**

`DrawStreamDump_Race3DBegin` is called from `TrackRenderer::Render`. If the result screen
re-rendered the 3D world it would appear here. It does not appear at all.

## Conclusion

**`TrackRenderer::Render` does not run during `Results`. The 3D world is not re-rendered on
the result screen.**

This dissolves the question rather than answering it. The D3D9-vs-librw difference measured
on `round2_result` / `round3_result` (~69% of pixels) **cannot** be a difference in how the
two renderers draw the world, because on those frames neither of them draws it. Both the
"per-channel gain" and the "world coverage failure" hypotheses were asking about a code path
that does not execute on the frames in question.

## What the result screen therefore is

Whatever the results state draws — a clear, a fade, an overlay — composited over whatever
the backbuffer already held. The two runs differ in what the *last in-race frame* left
behind, and in whatever the results path itself draws. That is where the ~69% lives.

Not yet established, and deliberately not guessed at after three wrong diagnoses on this
divergence: which of those two it is.

## Cost of not having this tag

Three failed diagnoses, each internally plausible and each investigating a path that never
runs on the affected frames:

1. per-channel R/G gain — inferred from a coincidentally matching blue channel
2. D3D9 world-coverage failure — inferred from a full-frame sky
3. the refutation of (2) — inferred from `world: 13 batches` on frames that turned out to be
   the wrong frames

One field, `"mode"`, would have prevented all three. Worth remembering when an instrument
answers confidently: check that it can see the thing being asked about.
