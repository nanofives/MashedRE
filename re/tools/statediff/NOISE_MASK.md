# Vehicle-record noise mask (stock-vs-stock, 2026-07-31)

Empirical cross-boot noise floor of the car-0 record (0x008815a0, 0xd04
bytes), measured on the canonical scenario (Training, QuickRace, 1 car,
idle, 20 s hold, ~1250 frames/side): **32 of 833 dwords** diverge between
two stock boots; the other 801 are bit-stable. Machine-readable mask:
`verify/statediff_proto/noise_floor.json` (regenerate any time with two
stock `--statediff-out` runs + `statediff.py --json`).

Two classes observed (raw values only — semantics per NO-GUESSING left
uninterpreted; offsets are relative to the record base):

1. **Per-boot pointers, diverge from frame 1**: `+0x954`, `+0x9a4` —
   values in both runs read as heap-range addresses (e.g. `0x0fd93010` vs
   `0x10d63010`). Expected to differ every boot; permanently maskable.
2. **Frame-~775 event cluster, small numeric drift**: `+0x210..+0x21c`,
   `+0x2d4..+0x2e0`, `+0x398..+0x3a4`, `+0x45c..+0x468` (three mirrored
   triplets + a half-scale copy), `+0xad0`, `+0xaf8`, `+0xbec`, and a
   stride-0xc run `+0xb30..+0xbe4` filling one entry per frame ~889..902
   (ring-buffer shape; values like f32 −1.0000264 vs −1.0). Cause of the
   cross-boot drift not established. [UNCERTAIN] — do not treat these
   offsets' divergence in a hooked run as hook-caused until the driver of
   this cluster is identified.

Caveat: the mask is offset-based and was measured on ONE scenario with an
idle car. A different scenario (driving, more cars, other track) needs its
own stock-vs-stock control pair before any stock-vs-hooked claim.

## Driving scenario (`--statediff-drive`, 2026-07-31)

Stock-vs-stock with the frame-locked cook injector (full accel, straight):

- The race-GO countdown start FLOATS vs the phase-3 capture anchor
  (observed 12-frame skew across boots; earliest witness `+0xbf4` 0→0x32).
  Align with `--anchor-nonzero 0xbf4` — a raw `--shift-b` cannot work
  because the spawn transient stays locked to the phase-3 anchor (two
  clocks in one capture).
- After re-anchoring, the two boots are **bit-identical for 314 frames**
  past countdown start (full countdown + ~2-3 s of driving at speed).
- Chaos onset at relative frame 314: `+0x148` flips sign discretely
  (A −0.263 / B +0.215) and everything follows within ~20 frames —
  consistent with the RNG consumption documented on the A5 hook
  (RNG-cursor / random-surface branch); a per-boot RNG-stream offset is
  not fixable by alignment. **Driving A/B verdicts are valid only inside
  `--until 314`** (re-measure the window for any new scenario).
