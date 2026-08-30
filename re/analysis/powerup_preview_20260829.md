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

- **The Chaos-mode list.** Three icons at (88,133,14), (14,6,238) = `oil`, and
  (88,133,14) again. Two share a corner colour, which no single-texture reading
  explains; note the dictionary contains a texture literally named `Chaos`.
  Not identified. [UNCERTAIN]
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
  STILL OPEN: where the four NAMES come from. The helper takes a sprite struct
  and reads its name field, so the selector is doing a lookup per icon; what it
  indexes has not been identified.
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
