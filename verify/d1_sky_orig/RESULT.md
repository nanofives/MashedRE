# D1 sky — original-side capture at a matched pose: no sky defect found, verdict is COARSE (2026-08-16)

Follow-on to `verify/d1_fxcut/RESULT.md`, which closed the D3D9-vs-librw divergence and
showed the "orange sky" was the FX particle bloom, not a sky. This lane asks the question
that survived: **is the standalone's sky faithful to the original's?**

Note the shape of the question. **librw draws no sky at all** (`RwRaceSubmit.h:24-28`) — the
sky comes from the D3D9 path in both runs. Measured here and confirming it: the sa_d3d9 and
sa_librw sky bands are identical to the byte (`#726962` both). So this is D3D9-vs-original.

## Method

1. Original: `py -3.12 re/frida/race_draw_burst.py --out verify/d1_sky_orig/orig.bmp --settle 4.0`
   - Track (measured, not assumed): **TRAINING** — Quick Battle always loads it.
   - Pose read from `DAT_00897fe0` with the post-92912a60 `+0x4c`-is-a-delta fix:
     `MASHED_CAM_POSE=-0.0959,3.1566,4.4876,-0.0916,0.4860,-1.3796`
   - Shim counters: `{"draw_calls": 235, "prims": 26058, "verts": 16990, "dp": 5, "di": 230}`
2. Standalone at that pose, both renderers, `MASHED_TRACK_SEL=12` (Training in `kAreas[]`).
   Track load verified from the log, not assumed:
   `original/TOASTART/TRACKS/training.piz tris=11469 verts=12767 sectors=42 radius=265.32`.

## Sky measurement

Sampled a strip that is sky in **both** frames (rows 0-20, cols 200-440):

| | R | G | B |
|---|---:|---:|---:|
| original | 168.2 | 169.5 | 174.9 |
| standalone D3D9 | 164.0 | 163.9 | 166.0 |

Both are neutral overcast grey-white. Max channel delta **8.9** (blue); the original is very
slightly cooler, the standalone very slightly flatter. Over the full top-60 band the means
are `#8E8C88` vs `#726962`, but that band includes terrain in the standalone frame and is
not a like-for-like comparison — the strip above is.

**No orange in either frame.** This independently re-confirms the `d1_fxcut` correction: the
orange was FX particles, and there is no sky-colour defect of the kind `d1_nopart` Finding 2
claimed.

Worth recording: the original's sky here is the **dome texture**, not the clear colour.
`SkyDomeRender` (`0x004492b0`) sets a camera-follow clear of R=0x50 G=0x58 B=0x60 = 80/88/96,
and the measured sky is ~168/170/175 — far lighter, so the dome is drawn and visible.

## Why this verdict is COARSE, not parity

The pose transplant is **not frame-accurate**, so this settles hue and brightness and
nothing finer:

- Whole-frame diff original vs standalone is **88.12%** — consistent with the 90.15% already
  recorded in `verify/sametrack3/SAME_TRACK_RESULT.md` for the same transplant, i.e. no
  better and no worse. The two frames show the same road, the same banners and the same
  buildings, from a visibly different height and pitch.
- The two candidates named in that note — **FOV** (the standalone hardcodes 60 deg at
  `TrackRenderer.cpp:4021`; the original's is unknown) and **sim moment** — remain untested.
  Until one of them is closed, a sky *parity* number cannot be quoted from this pair.
- `re/analysis/DIVERGENCE_LEDGER_3D.md:17-19` stays open on its own terms: the original's sky
  has cloud layers and UV scroll that a static clump will not animate. A single still cannot
  speak to animation.

So: **no defect found, at the precision available.** That is weaker than "the sky is
faithful", and is deliberately not written as the latter.

## New observation, not diagnosed

The standalone frame at this pose carries a **dark horizontal band across mid-screen**
(`verify/d1_sky_sa_d3d9/race1/01_inrace_track.bmp`) that has no counterpart in the original
and does not appear in the Arctic captures at the normal chase camera. Present on both
renderer paths, so it is D3D9-side scene content rather than a submit artefact. Not
investigated here — filed as an observation so it is not lost.

## Files

| file | what |
|---|---|
| `orig.bmp` | original, TRAINING, 4.0s settle |
| `orig.bmp.draw3d.json` | shim draw counters for that frame |
| `orig_campose.txt` / `orig_campose_raw.txt` | the pose, resolved and raw |
| `orig_track.txt` | the `.piz` the original actually opened |
| `verify/d1_sky_sa_d3d9/`, `verify/d1_sky_sa_librw/` | standalone at that pose, both paths |

---

**Capture-set note:** some `verify/d1_*` directories cited above were pruned on
2026-08-16 to reclaim disk. They are regenerable from the recipe in this file; the exact list
of what was deleted and what was deliberately kept is in `verify/d1_evidence/README.md`.
