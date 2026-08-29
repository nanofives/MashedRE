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
- **Whether the mapping is a table or computed.** `ea74` selects the set, but
  nothing yet shows *where* the per-mode list lives. A write-watch on the icon
  draw call, or an xref hunt on `POWERUPICONS` handles, would name it.
- **No TXD loader for `Powerups.piz`.** `LoadPngAssetToSlot` is PNG-only; this
  needs the `Txd::Dictionary` path used by the badge/car loaders. Mechanical,
  not blocked.
- **Insertion point** is chosen by geometry and screen-guard, not an ASM call
  site. [UNCERTAIN]

## Why this was not implemented yet

Hardcoding the Standard four would reproduce every capture taken so far and be
wrong the moment the mode changes — and the mode is a live setting the user can
toggle on that very screen. The Off case alone (draw nothing) is now portable;
Standard is portable; Chaos is not.

## Method note

Both closures came from **forcing a state and re-measuring**, not from reading
disassembly. The same approach settled the s18 greying predicate the same day,
after two static-reading attempts produced confident wrong answers.
