# Handoff — frontend visual parity session, 2026-08-27 → 30

All work pushed to `origin/main` (through `85a8176b`). Captures at 1024x768
unless noted. **12/17 → 14/17 FAITHFUL**, and the three still divergent are
explained below.

## What this session actually was

Six rounds of user visual review against the original, each round measured
rather than eyeballed. The headline fix was not a frontend defect at all.

## The big one: the font atlas was decoded backwards

`MashedFont.cpp` read FGDC20.TXD as *palette then pixels*. The documented format
(`Txd/TxdDecoder.h:6-26`, from `FUN_0054f8d0`) is **pixels then palette**.
Arithmetic proof: `0x3c + 512*256 = 0x2003c`, `+0x400 = 0x2043c`, plus the
trailing TEXTURE chunk = `0x20488` = the exact on-disk entry size.

The palette is real and **not** identity (`pal[67]=253`, `pal[192]=13`). Using
the index as alpha only looks right where the ramp is near-identity (`pal[0]=0`,
`pal[255]=255`) — i.e. glyph *bodies*. So **every anti-aliased edge pixel in the
game had the wrong alpha** for months, and letters still passed inspection.

Second half: coverage now fills all four channels, not RGB=255. The palette
entries used are greyscale with RGB==A and the Im2D stage modulates colour AND
alpha, so the original composites coverage². Text ink went 2235 → 1876 against
the original's 1816.

This closed **U-9045** (font jaggedness) and the standing "Select/Back not
rendered properly" report — one root cause, and neither was ever a geometry
defect. Three earlier attempts on the same symptom were tried and reverted as
wrong: POINT sampling, MIPFILTER NONE, alpha MODULATE2X.

## Everything else fixed, with its evidence

| area | fix | evidence |
|---|---|---|
| s2 greying | `avail[1]` → `avail[2]` | ASM `0x00432916` writes `0x67ed8c` |
| s6/s7 names | real track names from msgid `0x49+row`, cup = 4 tracks | Frida trace of `0x00427e00` |
| s6/s7 | plate `(120,72,8)`, border `(176,100,8)`, star 2x, row0 y 144 | measured |
| s24/s18 | button fade (solid 169 + fade 81), arrow via `MeasureMashedString` | pixel ramp + ASM |
| s24/s18 | disabled label alpha `0x80`, value alpha `0x30` | trace, keyed on row Y |
| s18 | rows 3/6 grey — our logic was right, inputs were wrong | avail at `0x0067edc4` |
| s4 | orange rows exist, bottom-anchored, icon 51.7x29 | 14.6% → 1.3% |
| s15/s16 | joypad tinted by player colour, icon 2x, plate alpha | `0xff3d3a98` = (152,58,61) |
| VS separator | was loaded and registered but **never drawn** | `vs_ok` was a local |
| power-ups | reads the original's real table | see below |
| 1024x768 boot | stray `DWM8And16BitMitigation` in the compat layer | `setup_mashed_compat.ps1` |

## Power-up preview — now table-driven

Ported verbatim: `int32[row][5]` at save-span `+0x270` (= `0x007f0cb0`), row from
`DAT_0067f17c`, `-1` terminates, id→icon via the `FUN_00458630` jump table,
filled **right-to-left** at `x = 562 - col*38`. All 11 icons load.

Gated on `DAT_0067ea74`: **0 Off draws nothing, 1 Standard uses the table,
2 Chaos is RANDOMISED** (two captures gave different sets with duplicates) and
is deliberately **not** drawn — reproducing it needs the original's RNG.

## Three silent-failure bugs found

All the same shape: a capacity/return check with no `else`, degrading quietly.

1. `QuadRenderer::kMaxSlots` was 64 while `kSlotVehPrev0` spans 61..68 —
   **vehicle previews 4..8 never loaded**. A/B measured: `veh 3/8` → `veh 8/8`.
2. `RwIm2DBridge::kMaxTexHandles` was 64; `RegisterTexture` drops silently when
   full. Loading 11 power-ups pushed past it and **the font was dropped** —
   every menu string rendered as solid white blocks while the metric said only
   "12.3% DIVERGENT". Raised to 96 (census ~70).
3. Our port never mirrors the save span back to `0x007f0a40`; it lives in
   `Nav_SaveSpanData()`. Reading the literal address returns zeroed `.bss`.

## Still open

- **s4 (14.4%)** — the original's row count is live device state; it rendered 3
  rows in one capture and 4 in another with no code change. Not a defect we can
  chase without modelling device join.
- **s6/s7 (18.2%)** — dominated by the map preview *we* draw that the harness
  nops on the original's side (`RVA_BACKDROP` kills `FUN_00474890`). Compare
  with `--keep-backdrop` before treating it as a defect.
- **Chaos power-up generation** — the `0x0043be4a` branch.
- **`PowerupSurround`** — the original draws this sprite under each icon at
  x = 444/482/520/558; we do not. Unknown whether it is visible.
- **Parity-walk desync** — the walk mis-captured three times in one day (a race
  launch, screen 2 as s1, wrong cursor row). Each looked like a regression and
  cleared on re-run. Treat single-run numbers with suspicion.
- The user reported the s15 player icon as more stretched on the original;
  measurement says the opposite (orig 35x63, ours 37x63). Unresolved conflict.

## Method notes worth carrying forward

- **Force a state and re-measure** beat static reading every time. It settled
  the s18 greying predicate, the power-up mode gate, and the Chaos randomness.
  Static byte-searching produced **three** confident wrong answers this session.
- **Always look at the render, not only the metric.** POINT sampling improved
  every number and made text visibly worse. A dropped font atlas showed up as a
  5-point metric move.
- Disassemble from a **function entry** — mid-function starts decode garbage,
  which cost two failed attempts on `0x0043bbea`.
- The parity metric scores ink *disagreement*, so correcting anti-aliasing
  makes faithful screens read slightly worse. Expect +0.2..0.5 there.
