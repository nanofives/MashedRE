# Acceptance-grade composition check, screen 1 — 2026-08-19

Draw-list diff (the primary verifier per `re/analysis/parity_tooling.md`), original vs
standalone, same screen, same settled state.

## Verdict

```
VERDICT: RED (match=448 mismatch=20 missing=0 extra=0)
```

**`missing=0 extra=0` is the headline.** The standalone emits exactly the same draw set as
the original, in the same order, at the same coordinates, with the same textures and blend
modes. Composition is correct. Every divergence is colour-only.

## The defect: 20 quads drawn at full alpha instead of 0x30

All 20 mismatches share one signature — **identical RGB, wrong alpha**:

| shape | count | original | standalone |
|---|---:|---|---|
| solid | 12 | `col=30131550` | `col=ff131550` |
| gradient | 8 | `col=30131550/00131550/…` | `col=ff131550/00131550/…` |

RGB is `0x131550` on both sides. In the gradient quads the **transparent stop
(`00131550`) matches**; only the opaque end differs. So the standalone is drawing this
quad family at `0xff` (fully opaque) where the original draws at `0x30` (48/255, ~19%).

Emitters: `HudIm2DQuad+0x188` and `HudIm2DQuadCorners+0x1dc`. Geometry places them on the
menu list rows (x 58–270, y 258–282, w 2/100/210, h 2/26) — row separators and the
selection band. Visually the standalone's list rows read solid where the original's are
dimmed.

## Ruled out: this is not animation phase

`parity_tooling.md` warns that unsynced captures can never agree on animated values, so
the alpha was checked frame by frame before calling it a defect. Decoding the ARGB dword
at offset 16 of every 28-byte vertex in the original burst:

| frame | alpha histogram for RGB `0x131550` |
|---|---|
| `scr1_f0` | `{0: 16, 48: 16, 255: 48}` |
| `scr1_f1` | `{0: 16, 48: 16, 255: 48}` |
| `scr1_f2` | `{0: 16, 48: 16, 255: 48}` |
| `scr1_f3` | `{0: 16, 48: 16, 255: 48}` |

**Identical across all four frames.** No ramp. The original holds alpha `0x30` steadily, so
the difference is reproducible and belongs to the port, not to capture timing.

## Two harness parameters that had to be right first

The first attempt returned `RED (match=0 … missing=468 extra=468)` — total alignment
failure, which is a *harness* result, not a rendering one. Two causes, both mine:

1. **`--scale-b` defaults to 0.8**, mapping an 800×600 standalone into the original's
   640×480 space. This standalone runs at 640×480 (`exe_main.cpp:345`), so every B
   coordinate was shrunk by 0.8. Fixed with `--scale-b 1`.
2. **The standalone had not settled.** At `MASHED_DBG_DRAWSTREAM=200:203` it was still
   emitting `LogoGradientQuadPx` / `LogoOverlayDraw` — the logo overlay — while the
   original burst was a settled menu. Moved to `700:703`.

A `match=0` diff should always be read as "the streams did not align", never as "the
renderer is wrong".

## Reproduce

```
py -3.12 re/frida/menu_draw_burst.py --screen 1 --frames 4

MASHED_GOTO=1 MASHED_DBG_DRAWSTREAM=700:703 MASHED_DETERMINISTIC=1 \
  MASHED_DET_FRAMES=900 MASHED_WIN_POS=left-bl mashedmod/build/mashed_re.exe

py -3.12 re/tools/drawlist_diff.py log/menu_draw_burst.json log/drawstream_re.json \
  --exclude-tex 9 --map mashedmod/build/mashed_re.map \
  --rotate-a 0x42e65a --tol-anim 4 --scale-b 1
```

Note the documented baseline is *"settled scr1 is GREEN 118/118 per frame (2026-06-12)"*.
This run captured 117 draws per original frame and 468 aligned pairs across the burst, so
the counts are not directly comparable to that line; whether the baseline has drifted or
the burst framing differs is **[UNCERTAIN]** and not resolved here.
