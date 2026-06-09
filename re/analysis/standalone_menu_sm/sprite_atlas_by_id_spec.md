# Sprite-draw primitives RE + id->quad pipeline (2026-06-08, Mashed_pool5)

Source: read-only Ghidra session on `Mashed_pool5` (pool12/pool8 leaked JVM
LockException; pool5 opened clean read_only, confirmed `MASHED.exe` image_base
`0x00400000`, PE32 x86, session `2a1a8f2c...`, `program_close` issued at end).
Anchor BDCAE093...  verified before AND after. NO-GUESSING: every offset/const
read from the pool5 image (decomp + listing + memory_read) or from the on-disk
asset files.

This closes the blocker named in `FUN_0043c5b0_port_spec.md` Part-3 PROGRESS
("the standalone has NO sprite-atlas-by-id pipeline").

---

## KEY FINDING — the menu-item "sprite ids" are USA.DAT STRING ids, not texture-atlas ids

The four primitives the draw loop calls split into THREE distinct mechanisms, only
one of which is an "atlas":

### (1) FUN_00428140 / FUN_00427e00 — the central item/glyph draw (id-keyed)

```c
// 0x00428140  FUN_00428140(id, x, y, argb, scale, mode, alpha)
FUN_00427780(id);                 // resolve id -> entry ptr (the "lookup table")
FUN_004277a0();                   // copy entry's UTF-16 into scratch + control remap
(**(DAT_007d3ff8+0x20))(9,2);     // render-state
FUN_00427680(x,y,mode,scale);     // compute pen position (screen-scaled)
FUN_00556ca0(DAT_0067d838, scratch, scale*_DAT_005cd5fc, ..., DAT_0067d83c);
                                  // = vtable+0x138 on the RtCharset object => RtCharsetPrint
```

- **The "id -> quad lookup table" is FUN_00427780** (0x00427780):
  ```c
  return &DAT_0066d828 + *(int*)(&DAT_0066d828 + id*4);
  ```
  i.e. `entry_ptr = base + *(u32*)(base + id*4)`, **base = 0x0066d828**. It is a
  base-relative offset table: the first `N*4` bytes are u32 offsets (ids 0..N-1),
  the rest is string data.

- **The table at 0x0066d828 is ALL ZEROES in the static image** (memory_read 128
  bytes => all 0x00). It is **runtime-loaded** by:
  ```c
  // 0x004274e0  FUN_004274e0  (caller 0x00402750)
  switch(DAT_007f0f60){0:"english.dat" 1:"french.dat" 2:"German.dat"
                       3:"Spanish.dat" 4:"Italian.dat" 5:"USA.dat"}
  iVar1 = FUN_004cc230(2,1, lang_dat);        // open .dat via RwStream
  FUN_004cbd30(iVar1, &DAT_0066d828, 0x10000);// RwStreamRead 0x10000 bytes into table
  ```
  So **the whole `<lang>.dat` file (<= 64KB) IS the id->entry table.** Asset file:
  `original/TOASTART/Common/FONT/{USA,English,...}.dat`.

- **FUN_004277a0** (0x004277a0) reads the resolved entry as `[u16 len][len * u16]`
  (UTF-16), copies the chars into a scratch buffer, NUL-terminates, and **remaps
  control codes to special FGDC20 glyph codes**:
  `8->0x81, 9->0x7f, 0xa->0x81, 0xb->0x8d, 0xc->0x80, 0xd->0x87, 0xe->0x8f`.
  These 0x7f..0x8f codes are the navigation/prompt glyphs (arrows, L/R, back)
  baked into the FGDC20 charset.

- **FUN_00556ca0(DAT_0067d838)** = `(**(obj+0x138))()` on the RtCharset object
  `DAT_0067d838` = RtCharsetPrint. **Same FGDC20 charset the standalone already
  loads** (`MashedFont`). `DAT_0067d83c` is the color/alpha pair; `_DAT_005cd5fc`
  the scale.

  **=> The menu records' primary/secondary ids (e.g. 0x18,0x47,0x150,0x224 and the
  prompt ids 0x42/0x43/0x48/0x13/0x58/0x133/0x225) are STRING ids into `<lang>.dat`,
  rendered as FGDC20 glyph runs. There is NO per-id texture atlas.** The standalone
  already has this pipeline (MashedFont + TextRenderer + USA.DAT); the only missing
  layer is the id->entry resolver + the control-char remap.

### USA.dat on-disk format (verified, original/TOASTART/Common/FONT/USA.dat, 2796 B)

- Offset table: u32[449] (first data offset = 0x704 => 0x704/4 = 449 ids, 0..448).
- Each entry @ off = `[u16 len][len * u16 UTF-16LE]`.
- id 0 @0x704 = len 7 "English"; id 1 = "Français"; etc.
- Most menu/prompt ids in USA.dat have **len 0** (blank placeholders) — the real
  localized labels live in `English.dat` (the default). id 0x224 has offset 0
  (the unused/special-centered item; the draw loop special-cases it, not a string).

### (2) FUN_00472c60 / FUN_00473540 — colored quad / gradient quad (NO texture)

Both build a 4-vertex RW Im2D quad in the vertex scratch at `0x00898a20` and submit
via `(**(DAT_007d3ff8+0x30))(4,&DAT_00898a20,4)` (RwIm2DRenderPrimitive,
RWPRIMTYPE 4 = trifan, 4 verts). Color is byte-swizzled ARGB->the device's vertex
color. `FUN_00472c60` = single flat color (highlight fill); `FUN_00473540` = a
2-color vertical gradient (verts 0/3 use `param_5`, verts 1/2 use a derived shade).
**No raster bound — these are solid background quads.** The standalone's existing
`HudIm2DQuad` colored-quad path covers this; no atlas needed.
Screen-scale consts: `_DAT_005cd5a8` (x), `_DAT_005cc560` (y); device vtable
`+0x20` = SetRenderState, `+0x30` = RenderPrimitive, `+0x18` field = a default
vertex z/rhw template.

### (3) FUN_0040bb50 (SpriteLookupC) + FUN_004739f0/FUN_00473870 — NAMED textured sprite

```c
// 0x0040bb50  FUN_0040bb50(const char* name)
FUN_004c5c00(DAT_0063b8fc, name);  // case-insensitive linked-list search by NAME
```
- `DAT_0063b8fc` is the **badges.txd** texture list. Populated in the global asset
  init `FUN_0040bbb0` (0x0040bbb0):
  ```c
  FUN_00495280("d:\\toastart\\common\\sfx.piz");
  DAT_0063b8f8 = FUN_0042a6b0("fx.txd",0,0);
  DAT_0063b8fc = FUN_0042a6b0("badges.txd",0,0);   // <- SpriteLookupC searches this
  DAT_0063b900 = FUN_0042a6b0("TrackImages.txd",0,0);
  DAT_0063b904 = FUN_0042a6b0("Interface.txd",0,0);
  ```
- `FUN_004739f0(spriteDesc, x,y,w,h, argb, u0,v0,u1,v1, coordMode, blendMode)`
  builds the same Im2D quad but binds the sprite's raster as render-state
  `(1, *spriteDesc)` (texture handle = first field of the descriptor) and writes the
  caller-supplied UVs (params 7-10) into the 4 verts. Each named sprite is a WHOLE
  texture drawn with caller UVs (chrome buttons/arrows usually 0,0,1,1).
- **This IS a real RW TXD atlas, but it is NAME-keyed, not numeric-id-keyed, and is
  used for menu CHROME decorations (Button/Arrow/SemiC...), NOT for the item
  labels.** Names used at call sites: "Button","SemiC","SemiC2","Arrow","scorch",
  "wfall","RWObjShad","RWObjShad".

#### badges.txd asset
- `original/TOASTART/Common/SFX.piz` contains (PC): `BADGES.TXD` @0x35f000 0x2876c,
  `INTERFACE.TXD` @0x308000, `TRACKIMAGES.TXD` @0x129000, `FX.TXD` @0x387800 (plus
  XBOX/ and PS2/ variants — use the root PC `BADGES.TXD`).
- Root chunk type 0x23, size 0x28760, libid 0x1803ffff. **[UNCERTAIN]** The internal
  layout is a multi-texture dictionary (FUN_0042a6b0 yields a named-texture LIST),
  which differs from FGDC20.TXD's single-raster 0x23 layout — the per-texture
  sub-chunk format + name records were NOT cracked this pass (not required for the
  item-label path). Cracking it is the follow-up for the named-chrome sprites.

---

## Draw call vertex/UV/color format (all three textured/colored paths)

Vertex scratch base `0x00898a20`, 4 verts x 7 dwords (stride 0x1c):
  +0x00 x, +0x04 y, +0x08 (z/rhw template from device+0x18), +0x0c rhw(1.0f
  =0x3f800000), +0x10 color (ARGB byte-swizzled to device order), +0x14 u, +0x18 v.
Submit: `(**(DAT_007d3ff8+0x30))(RWPRIMTYPE=4 /*trifan*/, &DAT_00898a20, 4)`.
Render-state pairs via `(**(DAT_007d3ff8+0x20))(state,val)`:
  (1,texture) bind raster, (9,1|2) addressing, (0xc,1)/(10,5)/(0xb,6) blend setup.

---

## What the standalone needs (and what this delivers)

The records' item ids => `<lang>.dat` string ids => FGDC20 glyph runs. The
**id->quad pipeline the menu draw loop (FUN_0043c5b0) actually needs is the
USA.DAT-string-id resolver feeding the FGDC20 charset** — implemented here as
`MenuStringTable` (D3d9Render/MenuStringTable.{h,cpp}):
  - loads `<lang>.dat` into a 64KB blob (mirror of FUN_004274e0),
  - `Resolve(id)` -> `base + *(u32*)(base + id*4)` (mirror of FUN_00427780),
  - `Decode(id, wchar out[])` -> reads `[u16 len][UTF-16]` + the FUN_004277a0
    control-char remap (8/9/a/b/c/d/e -> 0x81/7f/81/8d/80/87/8f),
  - feeds the existing `DrawMashedString` (FGDC20) — the "draw a quad per glyph"
    path the font already provides.

Named-chrome sprites (badges.txd) are a separate, name-keyed follow-up
(`SpriteAtlasTxd`), blocked only on cracking the multi-texture TXD dictionary
layout; documented above, not on the item-label critical path.

## Follow-up draw-loop port (FUN_0043c5b0) — what it needs from this loader
- For each record: call `MenuStringTable::Decode(primary_id)` /`(secondary_id)` ->
  `DrawMashedString` at the record's x/y/scale/color (the tag dispatch + shadow
  offset `_DAT_005cc31c`=3.0f + dim-alpha logic from the port spec).
- Highlight backgrounds: emit a colored quad (HudIm2DQuad) for the `-0xfc0000`
  selected branch (FUN_00472c60 flat / FUN_00473540 gradient) — already available.
- id 0x224 special centered item: triangle border (FUN_00472dc0) — colored, no atlas.
- Named chrome (FUN_0040bb50 "Button"/"Arrow"): needs `SpriteAtlasTxd` (badges.txd)
  — the only piece still requiring the TXD-dictionary crack.
