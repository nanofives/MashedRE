# Same-track, same-pose capture — first working three-way (2026-08-16)

## The headline: there is NO gross coord-frame mismatch

`std_d3d9.png` is the standalone rendered from the ORIGINAL's captured pose, on the
ORIGINAL's track. It is a coherent view of the same desert/quarry road — barriers,
buildings, hills — not the degenerate "camera buried in geometry" frame that the first
U-9039 filing rested on.

That frame was **entirely** explained by the track mismatch (pose from Training fed to a
standalone rendering Arctic). With both sides on Training, the transplant works.

## What it took

Two defects, both found by measuring rather than assuming:

1. **`+0x4c` is a direction delta, not a look-at point** (Xbox twin of 0x00446520). The
   reader now returns `{eye, dir, at}` with `at = eye + dir`, and writes
   `orig_campose_raw.txt` so the resolution can be re-checked.
2. **The original's Quick Battle ALWAYS races TRAINING** — `--track-sel 0` still produced
   TRAINING, so there is no track choice on that path. Measured by a new CreateFileA/W
   watcher that reports the `.piz` actually opened, rather than inferring from menu indices.
   And `MASHED_TRACK_SEL=12` was silently clamped to **7 (SuperG)** by
   `Campaign_SetSelectedTrack`'s 8-track cup clamp — so a "same-track" run was quietly
   comparing two different tracks. Added `Campaign_SetSelectedTrackDev()` for the override.

## Numbers

| comparison | delta |
|---|---|
| original vs standalone (either renderer) | 90.15% of pixels; mean abs R=64.5 G=59.0 B=53.0 |
| standalone D3D9 vs librw, same pose, Training | **1.13%** |

**librw and D3D9 are at near-parity on Training** — very different from the Arctic
result-screen shots where they diverged ~69%. Consistent with the coverage failure being
specific to those views.

The original-vs-standalone 90% is NOT a frame mismatch: both render the same road from the
same direction, but the original's eye sits higher and further back. Remaining candidates
are field-of-view and the sim moment (the original was captured 3s after race start; the
standalone at its own capture frame). Neither has been tested.

## A harness trap worth knowing

The first librw run used a shell conditional (`${EXTRA:+MASHED_RENDER_LIBRW=1}`) that did
not expand, so librw never engaged and the output was byte-identical to D3D9. That reads
exactly like "librw is a no-op". Set the variable with an explicit prefix and verify
engagement before trusting any A/B from this harness.
