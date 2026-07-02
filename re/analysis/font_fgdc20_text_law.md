# FGDC20 text law — canonical RE note (2026-06-12)

Complete render law for MASHED's menu font pipeline, from pool0 decomp
(Mashed_pool0, session 2026-06-12) + pixel calibration against original
backbuffer dumps (new instrument, see "Capture instrument" below). This is
the reference for `D3d9Render/MashedFont.{h,cpp}` and `DrawMashedString`
(exe_main.cpp). Supersedes the partial #9 notes in the frontend tracker.

## On-disk formats (FGDC20.RWF, chunk 0x199; FGDC20.TXD)

RWF after the 12-byte chunk header (loader FUN_00554390 read order; file
offsets):

| off  | charset field | FGDC20 value | meaning (from FUN_00554940 usage) |
|------|---------------|--------------|------------------------------------|
| 0x0c | (version dword) | 0x01000001 | top byte must be nonzero (0x005543c4 check) |
| 0x10 | cs+0x00 | 0 | type: 0 = texture font (the `*piVar2 == 0` branch) |
| 0x14 | cs+0x04 | 33.0f | atlas cell px (read by other code; not used by the print fn) |
| 0x18 | cs+0x08 | 0.151515f (=5/33) | anchor offset c8: print translates (0, −c8) inside the scaled space |
| 0x1c | cs+0x0c | 0.0f | tracking: added to every glyph's advance |
| 0x20 | cs+0x10 | 2 | flags: bit1 = TWO-BYTE chars (`(cs+0x10)&2` in FUN_00554940/FUN_005554d0) |
| 0x24 | cs+0x124 | 0x80 | extended-table base codepoint |
| 0x28 | cs+0x130 | 225 | glyph record count |
| 0x2c | cs+0x128 | 128 | extended-table entry count |

Then: ext table (ext_count × i16 → cs+0x12c), 0x100-byte ASCII LUT = **128
SIGNED i16** for codepoints 0..0x7f (cs+0x24; `(int)*(short*)(cs+0x24+cp*2)`,
−1 = no glyph), then `count` × 21-byte records, then u32 ntex + 32-byte
texture names ("fgdc20").

Glyph record, 21 bytes on disk → 32-byte runtime record (loader moves):
`[u_left f32][v_bot f32][u_right f32][v_top f32]` → rec+0x08..0x14,
`[width f32]` → rec+0x00, `[page u8]` → rec+0x1c; rec+0x04 = texture ptr
resolved from page at load. **The width float is the glyph width as a
fraction of the unit cell** and equals the atlas u-extent exactly:
`(u_right−u_left)×512 == width×33` for all 225 records.

TXD (corrected 2026-06-12 evening — byte-exact chunk math): root 0x23 →
4 raw bytes @0x0c → **rwID_IMAGE (0x18) chunk @0x14**, size 0x2041c =
12 (struct hdr) + 0x10 (w=512, h=256, depth=8, **stride=512** @0x2c..0x3b) +
0x400 (palette @0x3c..0x43b, ALL zeros → direct intensity) + 0x20000
(pixels). **Pixels start at +0x43c, NOT +0x438** — the original B19 parser
read the palette's last dword as the first four pixels, shifting the whole
atlas 4 columns left. That shift was the long-standing "leading tick"
(neighbor's edge column at every glyph's left) AND cut every glyph's
rightmost columns. With 0x43c, every glyph sits inside its .5-boundary
record window with clean margins (E ink profile in-window:
[0,0,17,17,17,10,7,7,7,7,7,5,0,0]). Pixel values are direct coverage
(0..255, smooth AA tail).

## Render law (FUN_00554940 + FUN_00427680)

- Vertex build: one 4-vert quad per glyph into DAT_00912a04 (0x24-byte
  stride: x, y, z=DAT_00912be0, color, u, v), submitted as indexed tri-list
  via FUN_004cd070/FUN_004cd170 (index buffer DAT_00912700) — the **Im3D**
  path, invisible to the Im2D device hook (why draw bursts never see text).
- Quad in model space: x = pen .. pen+rec.width, y = 0..1 (unit cell), UV:
  (y=top) v=rec+0x14, (y=bottom) v=rec+0x0c, u rec+0x08..rec+0x10.
- Transform: T(cursor) · S(scale_p, scale_p) · T(0, −c8), where scale_p =
  menu_scale × 0.0708 (**_DAT_005cd5fc = 0x3d90ff97**).
- Cursor (FUN_00427680): normalized [0,1]² over the FULL screen — x via
  1/640 (@0x005cd5a8), y via 1/480 (@0x005cc560), y flipped (1−…) and
  shifted by menu_scale × 0.025 (@0x005cc9a4). Align 2 = centered
  (−total×0.5, _DAT_005cc32c); align 1 = right; else left.
- **Anisotropy**: because the unit is the full screen on EACH axis, the
  glyph cell renders 0.0708·s of the screen WIDTH wide per unit-width and
  0.0708·s of the screen HEIGHT tall — i.e. glyphs draw 4/3 WIDER than
  their 33px atlas-cell aspect at 4:3 resolutions.
  Pixel calibration (640×480 original): "Press button to start" scale 1.2:
  predicted centered pen start 150.3 / measured 150; ink width 335px;
  cap-height ≈ 0.0708×480×1.2×(18/33) ≈ measured 20px.
- **Net screen mapping** (what the standalone implements):
  `cell_h = s×0.0708×ScreenH`, `cell_w = s×0.0708×ScreenW`,
  `glyph_w = rec.width × cell_w`, `advance = (rec.width + tracking) × cell_w`,
  `quad_top = anchor_y + cell_h×(0.025/0.0708 + c8 − 1) = anchor_y − 0.4954×cell_h`.
  The anchor is ≈ the cell center — this independently re-derives the
  bit-verified #10 plate centering (cell spans rec.y−13.5..+13.7 at s=0.8 vs
  plate rec.y−12..+14).
- Unmapped codepoints advance ZERO (both FUN_00554940 and the measurer
  FUN_005554d0 only advance when LUT idx ≥ 0). The SPACE has a real record:
  gi 96, width 5/33, blank UV rect → tiny word gaps are faithful.
- Measure (FUN_005554d0): Σ (rec.width + tracking) per mapped glyph,
  × scale arg.

## Color / vertex-alpha law (FUN_00556e90 + FUN_00428140)

- Color state struct (DAT_0067d83c): 4 RGBA corner colors — TL base
  [0xc..0xf] + TR slope [0x12..0x15], BL base [0..3] + BR slope [6..9];
  flat color dword at [0x18]; flag [0x19] bit0 = gradient mode. In gradient
  mode FUN_00554940 interpolates horizontally by pen/total_width per vertex
  row (top row TL→TR, bottom row BL→BR).
- Menu item drawer FUN_00428140(id, x, y, rgb, scale, align, param_7):
  - top alpha = min(param_7, 0xff); dim flag DAT_008990e4 caps it at 0x60.
  - bottom alpha = 0 (param_7 ≤ 0x80) / (param_7+0x80)&0xff (< 0x180) /
    0xff (≥ 0x180).
  - **Settled items run param_7 ≥ 0x180 → SOLID text, no gradient.** The
    vertical fade only exists mid fade-in. (The standalone's old hardcoded
    bottom=0.5 washed out every glyph's lower half.)
  - Sets render state 9=2 before printing; FUN_00554940 then re-binds
    state 9 from the raster's native +0x50 field on texture switch.
- Vertex color packing: RGBA bytes → D3DCOLOR (A<<24|R<<16|G<<8|B) — same
  packed-dword convention as the chrome (callers hold RwRGBA-packed dwords).

## Sampling / rasterization

- **LINEAR** (bridge default). FUN_00428140 sets render-state 9 = 2
  (rwFILTERLINEAR) before printing; FUN_00554940's per-raster override value
  (*(texture+0x50)&0xff) remains [UNCERTAIN] without a runtime read, but at
  the original's 640×480 (cell scale 1.03×) LINEAR and POINT are visually
  identical, and with the corrected 0x43c atlas base LINEAR renders clean
  solid glyphs matching the original's weight
  (verify/font/font_cmp_exit_offsetfix.png).
- (CORRECTION: an earlier intermediate state of this note claimed the
  original was point-sampled and added a half-texel UV inset — both were
  compensations for the 4-column atlas shift and are reverted.)
- The bridge submits vertex coordinates RAW (no −0.5 half-pixel shift): a
  −0.5 correction was briefly added against the glyph-edge "ticks", but
  those were the atlas pixel-base bug; with the atlas fixed, the shift only
  moved all texture content ~half a texel up-left vs the original (anaglyph
  evidence verify/font/font_anaglyph_exit.png → exit2.png).
- **Presentation enhancement (NOT RE law)**: the atlas coverage map is
  supersampled 2× with Catmull-Rom at load and edge-hardened with a
  smoothstep S-curve, fixed point 0.5 (`kAtlasSS`/`kT0`/`kT1`,
  MashedFont.cpp). At 800×600 the cell renders at ~1.3× the atlas
  resolution — bilinear magnification reads soft, edges spread ~2 device px
  and 1-texel strokes only reach ~60-70% opacity, while the original at
  640×480 (~1:1) shows 1px fringes and solid cores. After SS+hardening the
  standalone's luminance profiles match the original's steepness (115→105→0
  vs 96→12→0). Render law (UVs/geometry/advances) unchanged. User-requested
  sharpening 2026-06-12.

## Capture instrument (original-side backbuffer)

`mashedmod/src/d3d9_shim/d3d9_shim.cpp`: env `MASHED_ORIG_BBDUMP="N[,N…]"`
(≤16 frames) patches the device's Present (vtable slot 17) and dumps
`verify/orig_backbuffer_f<N>.bmp` (format-aware: MASHED's 640×480 mode is
R5G6B5). Inert when unset. Combine with `re/frida/menu_draw_burst.py
--screen N` (env-inheriting) to reach menu screens — note the synthetic
nav-push leaves title-layer text (press-button, music credits) composited
over the menu, so prefer regions away from those overlays for pixel
comparisons. Keyboard injection does NOT advance the original's title.

## Residuals (open)

1. ~~Stroke weight lighter~~ / ~~in-cell atlas bleed ticks~~ — **RESOLVED
   2026-06-12 evening**: both were the 4-column atlas shift (pixels read
   from +0x438 instead of +0x43c). With the correct base + LINEAR, weight
   and edges match the original (verify/font/font_cmp_exit_offsetfix.png,
   font_cmp_items_offsetfix.png).
2. Original watermark not visible at f1900 (mid-transition synthetic-push
   frame); verify whether settled scr1 draws the 0x41 watermark in the
   original before trusting ours at (600,52)/(596,48).
3. The splash "Loading" (id 0x222 at 580,140) right-clips when centered —
   check FUN_004282a0's alignment flag for that draw.
4. Disabled-row text alphas kept at the landed #18 look (top 0x80 /
   bottom×0.5); the exact param_7 the original passes for dim rows is
   unverified (the dim cap is 0x60 per FUN_00428140).
5. The raster-native filter value (*(texture+0x50)&0xff) — [UNCERTAIN],
   runtime read would settle it; LINEAR is the best-supported value
   (explicit FUN_00428140 state set + pixel match).
