# First original-vs-standalone frontend capture — 2026-08-19

Run to exercise `no_focus_pause` end to end, and incidentally to produce the
original-side reference D1 has been missing.

## The thing this set out to prove: it worked

`py -3.12 re/frida/capture_orig_screens.py 1 6` returned **`CAPTURED 2/2`** with the
terminal holding foreground the entire time. Before the patch this class of run stalled
whenever MASHED lost focus. Original-side capture is now genuinely unattended.

Artifacts: `verify/orig_screens/s1.bmp`, `s6.bmp` (640×480, post-Present backbuffer via the
d3d9 shim, hooks OFF so stock behaviour), and the standalone's `MASHED_PARITY` walk
covering all 17 frontend screens in one launch.

## The pixel numbers, and why they are NOT a defect measure

| screen | mean abs diff | pixels over threshold 16 |
|---|---:|---:|
| s1 | 96.47 | 85.40% |
| s6 | 91.08 | 87.70% |

**Do not read these as an 85% failure.** Two known asymmetries dominate, and both are
harness choices rather than rendering defects:

1. **`MASHED_PARITY` clears to a flat colour.** `exe_main.cpp:348-351`: `g_parity_bg`
   defaults to opaque black and `MASHED_PARITY_BG` overrides it with a solid ARGB, *"only
   used in MASHED_PARITY"*, so dark UI reads against a chosen field. The original capture
   has its **live video backdrop** filling the frame. Comparing a flat-cleared capture
   against a video backdrop accounts for most of the difference on its own.
2. **Game state differs.** The original was driven to screen 6 from a real boot, so its
   list is populated (`Angel Peak`, `Kharga Temple`, `Neustein`, `Timgidski`). The
   standalone's parity walk pushes the screen without establishing cup/championship state,
   so it shows `Arctic`, `Egypt` and empty rows. That is the walk not setting up state, and
   **is not evidence of a data defect** — establishing which it is needs a like-for-like
   run, not this capture.

This is consistent with `re/analysis/parity_tooling.md`, which makes `drawlist_diff.py` the
primary verifier and pixel diffing the backstop for texture decode and font raster. A pixel
percentage across differing backdrops and differing state is not an acceptance signal.

## What the images do show, by eye

Composition matches closely on s6: the `Challenge Select` title and `MASHED` wordmark in the
same places, the selection-highlight bar, the track-preview panel top-right rendering a real
track image, four devil icons, and the `Select` / `Back` prompt strip. The layout port is
in good shape; what differs is backdrop and populated content.

Side-by-side PNGs: `orig_s1.png` / `re_s1.png`, `orig_s6.png` / `re_s6.png`.

## To make this an acceptance-grade comparison

- capture the standalone in **normal** mode via `MASHED_GOTO=6` so it renders its own video
  backdrop, instead of `MASHED_PARITY`'s flat clear; **or**
- set `MASHED_PARITY_BG` on the standalone *and* suppress the original's backdrop, so both
  sides sit on the same field; **and**
- drive both sides to the **same game state** before capturing; **then**
- verify with `drawlist_diff.py`, not with pixels.

Not done here. This run's purpose was the focus-stall fix, which is proven.
