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


---

# CORRECTION (same day): the "per-channel R/G gain" reading was WRONG

Written after looking at the actual frames rather than the statistics. Recorded rather
than edited away, because the wrong turn is instructive.

## What I claimed, and why it was wrong

I inferred a per-channel gain from `round2_result` having **B=43.9 on both sides** while R
and G diverged, and called a coincidence across two shots "not plausible". Then I looked at
`D3D9_r2.png`: it is a **near-uniform saturated orange fill** with the standings overlay on
top and only faint geometry at the very bottom. It is not a washed-out render of the world.
The matching blue value was a coincidence, and I over-read one number into a mechanism.

## Two hypotheses tested and killed

| hypothesis | test | result |
|---|---|---|
| distance fog | `MASHED_NO_FOG=1` | round2_result **identical to 0.1** — fog is already off on that view (`chase_cam` is false there), so it was never a candidate |
| world relight | `MASHED_WORLD_PRELITONLY=1` | round2_result 249.1/112.7/44.7 vs 248.5/111.9/43.9 — no effect |

## What the frames actually show

`librw_r2.png` / `librw_r3.png` render the world: sunset sky band, terrain, water, the tank
prop, the car. `D3D9_r2.png` / `D3D9_r3.png` are filled with sky-coloured orange.

`TrackRenderer.cpp` draws SKY.DFF **first, camera-locked, with ZWRITE off**, then the world
over it. So a full-frame sky means **the D3D9 world draw is not covering the result-screen
view from round 2 onward**, while librw's world draw does.

That is a coverage/visibility failure, not a shading one — which is why every shading knob
did nothing.

## Not yet established

- WHY the D3D9 world stops covering that view at the round-2 boundary (culling? a camera
  the world path does not handle? sectors not re-submitted after a round restart?).
- The `MASHED_DBG_DRAWSTREAM3D` dump only samples three early frames (f60-f62), where the
  world draws normally (13 batches, 49,440 verts). **Getting that dump at the result-screen
  frame is the next step** and would settle it — batches=0 there proves the skip outright.

## Standing conclusion, unchanged by the correction

The defect is on the **D3D9 side**; librw is the stable path on these shots. Inverting
`MASHED_RENDER_LIBRW` would remove a visible defect from the shipping default rather than
introduce one — but still cannot be justified until faithfulness is adjudicated against the
ORIGINAL (U-9039).
