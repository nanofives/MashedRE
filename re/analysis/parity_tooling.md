# Parity harness — composition-layer verification tooling

Built 2026-06-12 (branch `tools/parity-harness`) in response to the 25-item
user review: nearly every item the review caught was a COMPOSITION bug
(bit-identical C4 functions wired wrong — wrong texture, wrong place, wrong
order, missing draw, wrong alpha) or a TEMPORAL bug (animation phase), i.e.
classes that per-function bit-identity diffs cannot see and that single
screenshots + model eyeballs reliably miss. This harness makes both classes
mechanical: diff structured draw streams, not pixels or impressions.

## Tool inventory

| Tool | Side | What it does |
|---|---|---|
| `MASHED_DBG_DRAWSTREAM=<N|N:M>` (env, standalone) | RE | dumps every bridge Im2D draw of frontend frames N..M to `log/drawstream_re.json` (raw vert blobs + module-relative retaddrs + mirrored blend/tex state). `1` = frame 200 (BBDUMP timing). Pair with `MASHED_GOTO=<screen>`. |
| `re/frida/menu_draw_burst.py` | original | K consecutive frames of the original's device draw stream, raw blobs + RVA retaddr chains, nav-pushed to a screen. `--screen N --frames K`. Set `MASHED_ROOT` when running a worktree copy. |
| `re/tools/drawlist_diff.py` | both | aligns the two streams (LCS + tolerance), classifies each divergence: MISSING / EXTRA / MISMATCH(color) / MISMATCH(moved) / MISMATCH(reordered). `--map mashedmod/build/mashed_re.map` resolves standalone retaddrs to function names. Exit 0 = GREEN. |
| `re/tools/nav_coverage.py` | RE source | static walk of the kT* descriptor tables + ActionToScreen push map; flags table holes, dead-end actions, kind-inconsistent screens (screens of a kind where only some have `sid == N` handling — the "whole screen missing" class of items 20-25). |
| `re/tools/imgdiff.py` | both | numeric pixel backstop (mean abs diff, % over threshold, region grid, amplified heatmap) for what draw lists can't encode: texture decode, font rasterization. Use BBDUMP as the standalone's pixel source. |
| `re/frida/menu_draw_dump.py` | original | (pre-existing) single-frame decoded dump; superseded for new captures by `menu_draw_burst.py` (raw blobs, bursts) but its existing `log/menu_draw_dump.json` remains a valid `A` input to the differ. |

## Standard recipes

Composition check for screen S (the per-fix acceptance gate):

```powershell
# 1. original reference (short Frida run; keep bursts small)
py -3.12 re\frida\menu_draw_burst.py --screen S --frames 4

# 2. standalone capture (MASHED_GOTO pushes at boot, skips splash/title)
$env:MASHED_GOTO="S"; $env:MASHED_DBG_DRAWSTREAM="200:203"
mashedmod\build\mashed_re.exe   # from repo root; kill after log/drawstream_re.json appears
$env:MASHED_GOTO=$null; $env:MASHED_DBG_DRAWSTREAM=$null

# 3. diff (font glyphs excluded: the original capture can't see its text pipe)
py -3.12 re\tools\drawlist_diff.py log\menu_draw_burst.json log\drawstream_re.json `
    --exclude-tex 9 --map mashedmod\build\mashed_re.map
```

Animation phase check: same, but compare frame-by-frame — the differ pairs
multiple `--label-a/--label-b` in order, so per-frame alpha/position ramps
line up as numbers.

Pixel backstop (fonts, texture decode):

```powershell
py -3.12 re\tools\imgdiff.py <original_shot.png> verify\dbg_backbuffer.bmp --out verify\heat.png
```

## Integration into the frontend fixing process

For any feedback item that is visual-composition (placement, color, alpha,
draw order, missing element):

1. **Before the fix**: capture both sides at the affected screen; run the
   differ; the item's divergence should appear as concrete rows. If it does
   NOT appear, the bug is below the draw-list level (texture decode, font
   raster) → use imgdiff, or above it (nav/flow) → use nav_coverage.
2. **After the fix**: re-run the same diff. Acceptance = the targeted rows
   are gone AND no new MISSING/EXTRA rows appeared. A full-GREEN verdict is
   the goal state per screen but is not required per item while known
   residuals remain — cite the remaining rows in the tracker entry instead.
3. **New screens**: run nav_coverage after wiring a screen; the
   kind-consistency table should not list it as unhandled.

## Known asymmetries and limits (read before trusting a RED)

- **Scale**: original renders 640x480 virtual, standalone 800x600; the differ
  defaults `--scale-b 0.8`. Like-vs-like comparisons need `--scale-b 1`.
- **Text pipe**: the original's RtCharset glyph draws do NOT pass through the
  hooked device vtbl+0x30 draw, so original captures contain NO text glyphs.
  The standalone's font glyphs are tex handle 9 → `--exclude-tex 9` for chrome
  comparisons. Original-side glyph capture would need a hook on the
  FUN_00554940 charset renderer family (future extension).
- **Native draws**: standalone draws that bypass the bridge are invisible to
  the dump; the ONE frontend case (menu video quad, exe_main.cpp) is mirrored
  into the stream explicitly (tex sentinel -1). In-race TrackRenderer paths
  are out of scope (menu parity only).
- **Retaddr namespaces**: side-A rets are MASHED RVAs, side-B rets are
  mashed_re.exe module offsets (resolve via the .map). They are attribution
  within a side, never compared across sides.
- **Frida hygiene**: keep original-side bursts short (default 4 frames), one
  run at a time, don't minimize MASHED mid-intro, and expect the driver-wedge
  caveats from the frontend tracker to apply to any original-side capture.
- **Color bytes** are compared raw; both sides write the identical RW Im2D
  vertex layout by construction (the standalone's draw reimpls are the C3/C4
  ports of the original's emitters).

## Validation findings (2026-06-12, scr1 settled, harness's first run)

Handed to the frontend-feedback session as candidate items; each row is
machine-derived, citations = capture files in log/:

1. **Arc-wash settled alpha**: original scr1 (settled, 2.5 s after push)
   draws the 15-strip wash at alpha **0x00** (`00ffffff`); the standalone
   draws **0x60/0x78** (`60ffffff`/`78ffffff`) — the "permanent left haze"
   may not be permanent in the original at settle. Cross-check against item
   #15's "alpha 0x60 wash is faithful" conclusion; the two claims conflict.
2. **Preview crossfade form**: original emits TWO 176x352 half-quads per
   layer (corner-faded left half: `00ffffff/c0ffffff/...`), standalone emits
   single full-width quads → MISSING rows at x=288/x=464. Geometry form of
   FUN_00474890's two-half-quad split (item #14's adaptation) diverges.
3. **Gradient band draw order**: the 128x64 top/bottom gradient caps at
   x=384 draw at a different stream position than the original
   (MISMATCH(reordered)) — benign unless blending order matters over the
   video; flagged for completeness.
4. **nav_coverage first run**: table hole screen 27; dead-end action
   0xff1d0000 (screen 4 item); kind-4 screens 31/33 (insults family) lack
   the sid-specific widget handling that 19/32 have; kind-2 screens 2/3/4
   have no sid-specific code while 8 does (verify intended).
