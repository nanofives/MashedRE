# D1 bisect — where the D3D9/librw divergence comes from (2026-08-15)

## Result: it is NOT gradual accumulation, and it is on the D3D9 side

| shot | D3D9 mean RGB | librw mean RGB |
|---|---|---|
| `round1_go` | 28.9, 33.6, 34.8 | **identical** |
| `round1_result` | 67.1, 73.0, 75.6 | **identical** |
| `round2_go` | 28.9, 33.7, 34.8 | identical to 0.1 |
| `round2_result` | **248.5, 111.9, 43.9** | 103.5, 66.4, **43.9** |
| `round3_go` | 31.9, 38.2, 39.4 | **identical** |
| `round3_result` | **225.4, 159.2, 76.2** | 98.5, 77.2, 51.5 |

Three facts fall out:

1. **Every `_go` shot is identical at every round.** The two renderers agree on the
   race-start view throughout, so this is not a global renderer difference and not
   something that drifts continuously.
2. **`round1_result` is identical; `round2_result` and `round3_result` are not.** The
   divergence switches on at a specific event — the end of round 1 — and affects only the
   result-screen render.
3. **The BLUE CHANNEL IS UNTOUCHED.** `round2_result` is 43.9 on both sides while R goes
   248.5 vs 103.5 and G 111.9 vs 66.4. A coincidence across two independent shots is not
   plausible: this is a per-channel gain on R and G.

`01_inrace_track` carries the same signature — mean abs diff R=88.09, G=57.56, **B=19.40**.
So one mechanism plausibly explains all four divergent shots.

## Which side is wrong

The D3D9 side is the one that changes: it blows out to a near-saturated orange haze
(`D3D9_r3.png`) while librw keeps the scene legible (`librw_r3.png`) — and the two agree
exactly until the end of round 1. **librw is the stable path here; the default is the one
with the defect.**

This matches the D-S3-BANK characterisation already on record: "per-channel
brightness-dependent gain on D3D9 FF output". That work closed "at floor" against a
*single* frame; this shows the same term also fails to reset across rounds.

## Consequence for D1

Earlier framing ("inverting would ship a renderer that drifts") was backwards. On this
evidence, inverting `MASHED_RENDER_LIBRW` would REMOVE a visible defect from the shipping
default rather than introduce one.

That is still not sufficient to invert, for the reason U-9039 records: neither renderer has
been compared to the ORIGINAL. "librw is more stable than D3D9" is not "librw is faithful".
The fix path is (a) find the R/G gain that survives a round boundary in the D3D9 path, and
(b) resolve U-9039 so faithfulness can actually be adjudicated.
