# s24/s18 power-up preview row — blocker status, 2026-08-29

The four circular power-up icons at the bottom-right of screens 24 and 18. The
port draws **nothing** there (measured: the whole region is a single value,
`(0,0,0)`). User-reported across three review rounds.

Prior state (2026-08-27, lane 5): assets identified, geometry measured at one
resolution, and **blocked** on "which 4 of 11 a track exposes, and in what
order — nothing in `Nav_GameState()` is known to carry this, and no ASM
function that builds the list has been located."

Two of those blockers are now closed by runtime experiment.

## 1. The set is gated on the Power Ups mode (`DAT_0067ea74`) — CLOSED

Method: spawn the original, force `DAT_0067ea74` to each value, redraw screen 24,
dump the backbuffer through `MASHED_ORIG_BBDUMP_REQ`, and read the icon row.

| `ea74` | s24 row-1 value | icons drawn |
|---|---|---|
| 0 | `0x59` "Off" | **none at all** |
| 1 | `0x5b` "Standard" | **4** |
| 2 | `0x5c` "Chaos" | **3**, different colours |

So the list is **mode-driven, not track-driven** — which is the opposite of the
assumption in the earlier note ("which 4 of 11 a *track* exposes"). It also
means the Off case is a real, portable behaviour: draw nothing.

## 2. Geometry — CLOSED, confirmed at two resolutions

Lane 5 measured at 1024x768 and could not separate the virtual constants from
rounding. The 640x480 capture disambiguates them:

| | 1024x768 (lane 5) | 640x480 (this round) | virtual |
|---|---|---|---|
| icon size | 51 device | 32 device | **32** |
| pitch | 61/61/60 device | 38 device | **38** |
| first x | 718 device | 448 device | **448** |

kVScale is 1.6 and 1.0 respectively, so both reduce to the same virtual numbers.

## 3. Standard-mode contents — CLOSED, independently reproduced

Matching each on-screen icon's square-fill corner against `POWERUPICONS.TXD`
mip-0 palette corners:

| x (640) | measured corner | texture |
|---|---|---|
| 448 | (52, 185, 230) | `mortar` (45,194,242) |
| 486 | (230, 33, 36) | `mine` (235,28,36) |
| 524 | (14, 6, 236) | `oil` (0,0,255) |
| 562 | (246, 214, 6) | `machinegun` (255,224,0) |

Same four lane 5 identified at 1024x768, arrived at from a different capture at
a different resolution. Order is left-to-right as listed.

## Still open

- **The Chaos-mode list.** Randomised (two runs gave different sets, with
  duplicates). It does NOT come from `0x007f0cb0` -- that table is identical at
  ea74 = 1 and 2 -- so Chaos is generated in the `0x0043be4a` branch. Not
  followed. [UNCERTAIN]
- ~~The full id -> sprite map~~ **RESOLVED** -- see the corrected table above.
  Rows 1-3's extra ids resolve too: 11 = missile, 16 = flamethrower,
  17 = shotgun, 18 = flare.
- **Where the per-mode list lives.** Still unknown. Two static hypotheses were
  tried and BOTH REFUTED:
  1. *A dword table of icon indices.* The Standard set is `mortar, mine, oil,
     machinegun` = `4, 3, 0, 2` in `PowerUpIcons.lst` order, and that exact
     dword run appears once, at `0x005f2d5c` in `.data`, inside what looks like
     a table of 4-value rows (a neighbour even has a duplicate, like Chaos).
     Overwriting it at runtime with `1, 5, 9, 7` changed NOTHING -- the icons
     still drew mortar/mine/oil/machinegun. Coincidental match.
  2. *A table of pointers to the texture-name strings.* The names do live in an
     `.rdata` pool (`oil` 0x5ce4fc, `mortar` 0x5ce58c, `mine` 0x5ce594,
     `machinegun` 0x5ce59c), but no 4-pointer run anywhere in `.data`/`.rdata`
     contains three or more of them.
  The runtime trace was then done and **located the draw site**, though not yet
  the list's storage. Chain, all measured:
  - `rw_set_state` is `*(*(0x007d3ff8) + 0x20)` with `(state, value)`; state 1
    binds the texture raster. Hooking it on s24 shows caller `0x0047397d`
    binding four rasters at exactly 25 calls each (one per frame).
  - That site reads the raster as `*(texture)` from a texture passed in, i.e.
    it is a generic sprite-draw helper entered at **`0x00473870`** (37 callers).
  - Hooking THAT and logging `(returnAddress, arg0, arg1)` finds the icon row:
    caller **`0x0043bbea`**, drawing BY NAME with arg1 = the x coordinate --
    `mortar` x=448, `mine` x=486, `oil` x=524, `machinegun` x=562. Those are
    exactly the measured on-screen positions.
  - Two siblings in the same drawer: **`0x0043baee`** draws a sprite named
    `PowerupSurround` at x = 444/482/520/558 (the same row, offset 4) BEFORE
    the icons, and `0x0043b595` draws the `Arrow` sprites for the value column.
  - No call targets exist in `0x0043b000..0x0043bc00`, so `0x0043bbea` is inside
    `FUN_0043af10` -- the s18/s24 screen drawer whose row loop was traced for
    the greying work.
  **RESOLVED.** Disassembling from the function entry `FUN_0043af10` (starting
  mid-function decodes garbage; only an entry-anchored decode aligns) gives the
  loop body verbatim:

      0x0043b9c3  cmp dword ptr [0x67ea74], 1      ; Standard?
      0x0043b9da  jne 0x43be4a                     ; not-Standard branch
      0x0043b9e0  lea ecx, [esi + edi*4]
      0x0043b9e5  add edx, ecx                     ; edx = esi + edi*5
      0x0043b9e7  mov eax, [edx*4 + 0x7f0cb0]      ; the LIST
      0x0043b9ee  cmp eax, -1
      0x0043b9f1  jle 0x43bbf3                     ; -1 -> skip this slot
      0x0043b9f8  call 0x458630                    ; id -> sprite pointer
      0x0043ba05  mov edi, eax                     ; sprite = arg0 of the draw
      0x0043bbf7  cmp esi, 5                       ; five columns per row

  So the list is a table at **`0x007f0cb0`**, `int32[row][5]`, indexed
  `[row*5 + col]`, with `-1` marking an empty slot; the row comes from
  **`DAT_0067f17c`**; and `FUN_00458630` maps an id to a sprite. That is why the
  loop runs five times and draws four icons.

  Read live on s24 (`DAT_0067f17c` = 0):

      row0: [ 9, 19, 12,  7, -1]      <- the drawn set
      row1: [ 9, 19, 12,  7, 11]
      row2: [ 9, 18, 12,  7, 11]
      row3: [ 9, 17, 16, 19,  7]

  **CORRECTED**: calling `FUN_00458630(id)` directly and reading the sprite's
  name (offset 16 in the returned struct) gives the real map --

      id  2 flamethrower   id  6 mine        id  7 mortar
      id  9 machinegun     id 10 depthcharge id 11 missile
      id 12 mine           id 13 mine        id 16 flamethrower
      id 17 shotgun        id 18 flare       id 19 oil
      id 21 Chaos          id 22 Airstrike

  So `row0 = [9, 19, 12, 7]` is **machinegun, oil, mine, mortar** -- the REVERSE
  of the drawn left-to-right order (mortar, mine, oil, machinegun at x =
  448/486/524/562). The loop therefore fills the row right-to-left. An earlier
  revision of this note read the ids off by pairing table order with draw order
  and got all four wrong; the direct call is authoritative.
  [UNCERTAIN] ids 6/12/13 all return the same sprite pointer (mine), as do 2
  and 16 (flamethrower), so the id space is not 1:1 with textures -- either
  several power-up types share an icon, or `FUN_00458630` falls back for
  unmapped ids. Not distinguished. The table reads IDENTICALLY at ea74 = 1 and 2, so
  Chaos does not use it -- it takes the `jne 0x43be4a` branch, which is also
  where Off lands. Note `0x007f0cb0` sits INSIDE the save span
  (0x007f0a40..0x007f0f60), so this is save/game state, not a static table --
  which is why byte-searching the image for it was never going to work.
- **No TXD loader for `Powerups.piz`.** `LoadPngAssetToSlot` is PNG-only; this
  needs the `Txd::Dictionary` path used by the badge/car loaders. Mechanical,
  not blocked.
- **Insertion point** is now ASM-anchored: the original draws this row inside
  `FUN_0043af10` at `0x0043bbea`, right after the `PowerupSurround` pass at
  `0x0043baee`. Our draw sits in the equivalent screen block.
- **`PowerupSurround` is NOT drawn by us at all.** The original lays a sprite
  of that name under each icon at x = 444/482/520/558. Worth checking whether
  it is visible or fully covered by the 32-wide icon on top. [UNCERTAIN]

## Why this was not implemented yet

Hardcoding the Standard four would reproduce every capture taken so far and be
wrong the moment the mode changes — and the mode is a live setting the user can
toggle on that very screen. The Off case alone (draw nothing) is now portable;
Standard is portable; Chaos is not.

## Method note

Both closures came from **forcing a state and re-measuring**, not from reading
disassembly. The same approach settled the s18 greying predicate the same day,
after two static-reading attempts produced confident wrong answers.
