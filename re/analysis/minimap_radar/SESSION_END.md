# minimap_radar — SESSION_END

**Session ID**: minimap_radar-20260506  
**Slot held**: Mashed_pool11 (read-only)  
**SHA-256 anchor**: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓  
**Outcome**: HALTED — confirming absence (expected success path for top-down game)

---

## Anchor outcome: NO MINIMAP / RADAR FOUND

All four strategies ran to completion. No separate minimap or radar overlay exists in MASHED.exe.

---

## Strategy 1 — string anchors (result: 0 hits)

Searched `search_defined_strings` for each of: `Minimap`, `minimap`, `MINIMAP`, `Radar`, `radar`, `RADAR`, `trackmap`, `TrackMap`.  
**All returned 0 results.**

---

## Strategy 2 — file-path anchors (result: 0 relevant hits)

| Query | Result |
|---|---|
| `map.txd` | 0 hits |
| `MINIMAP\` | 0 hits |
| `.tga` | 0 hits |
| `map_` | 1 hit: `"map_%d.dff"` @ 0x005cc330 |

`"map_%d.dff"` @ 0x005cc330: referenced from `FUN_00401630` (0x00401630–0x00401686).  
`FUN_00401630` calls `sprintf(buf, "map_%d.dff", param_1)` then `FUN_004b3bf0(buf, 0, 0)` — a RenderWare DFF geometry loader. This is **track collision/geometry loading**, not a minimap.

---

## Strategy 3 — hud_ingame call-graph (result: no minimap callee)

`FUN_0040dfc0` (HUD_INGAME_FN, 0x0040dfc0, 306 bytes) has 13 depth-1 callees (pre-existing analysis, session hud_ingame-20260502):

| Callee | Notes from prior analysis |
|---|---|
| 0x00403160 | Sub-mode 0xb handler — shrinks viewport to 0.8×0.8 |
| 0x0041a3e0 | Mode 10 draw |
| 0x0041b630 | 4-entry HUD slot loop |
| 0x0041c0c0 | 2-entry HUD loop |
| 0x0041c300 | Mode 5 draw |
| 0x0041ccc0 | Unknown HUD draw |
| 0x0041d870 | Unknown HUD draw |
| 0x0041db80 | 9-byte stub |
| 0x0041ded0 | Mode 4/7/8/9 draw |
| 0x0041e850 | Mode 2 draw |
| 0x00426ba0 | Render-enabled guard getter |
| 0x0042f500 | Sub-mode getter |
| 0x0042f6a0 | Game sub-mode getter |

**`FUN_00403160`** (sub-mode 0xb) — decompiled and checked:
- Shrinks viewport to `0.8 × 0.8`
- Calls `FUN_00402fb0()`: draws sprite `0x2A4` (676) twice at screen center-bottom (320, 380) / (316, 376) with pulsing sine-wave alpha — a **loading/transition pulse effect**, not a track map
- Conditionally calls `FUN_00428760(DAT_00771964, 50.0, 30.0, 240.0, 120.0, 0)` → `FUN_00428610` → `FUN_00450b10`: a viewport-scaled rectangle draw using screen-scale ratios — **a generic 2D textured rectangle**, not a track map
- This sub-mode 0xb context is an isolated special phase, not a per-frame minimap renderer

No callee in the HUD_INGAME_FN call graph shows a render-quad-with-rotated-icon pattern consistent with minimap/radar.

---

## Strategy 4 — race_state position icons (result: Arrow = rank display, not map)

`"Arrow"` @ 0x005cd828 has **17 cross-references**, all from functions in range 0x0042f0c0–0x0043b5xx.

`FUN_0042f0c0` (0x0042f0c0–0x0042f3e0, 800 bytes) decompiled:
- Iterates 3 times (players in race)
- Calls `FUN_0040bb50("Arrow", x, y, w, h, color, ...)` — texture lookup by name
- Selects sprite IDs 0x3a, 0x154, 0x5f, 0x60 based on `DAT_0067ea8c` (position/mode selector)
- Renders sprites at computed screen positions using `FUN_00427e00`
- **This is a race-position rank display (1st/2nd/3rd)** with an "Arrow" marker for the local player's current position in the standings — not vehicle positions on a track map

`"TrackImages.txd"` @ 0x005cccc0, referenced from `FUN_0040bbb0`:  
`FUN_0040bbb0` is a **game init function** that loads: `sfx.piz`, `fx.txd`, `badges.txd`, `TrackImages.txd`, `Interface.txd`.  
`TrackImages.txd` → `DAT_0063b900`. This is the **track selection screen thumbnail texture dictionary**, not a minimap overlay.

---

## Why no minimap exists

Mashed is a fixed-overhead top-down camera game. The main render camera already shows the entire track from above. A separate minimap overlay would be redundant — the game view itself serves as the "map." This is consistent with the hypothesis in the session prompt.

---

## Strings observed (verbatim, complete list)

| Address | String | Verdict |
|---|---|---|
| 0x005cc330 | `map_%d.dff` | Track geometry loader format string |
| 0x005cccc0 | `TrackImages.txd` | Track select thumbnails TXD |
| 0x005cd828 | `Arrow` | Race position rank indicator |
| 0x005ce5d6 | `HDTargetHUD.txd` | HUD target texture (no xrefs found) |

No "Minimap", "MINIMAP", "minimap", "Radar", "RADAR", "radar", "trackmap", "TrackMap", "map.txd", or ".tga" strings found.

---

## Tracker impact

- Functions covered: 0 (halted — no new functions to classify)
- New hooks.csv rows: 0
- STUBS.md additions: 0
- UNCERTAINTIES.md additions: 0
- DEFERRED rows: 0
- IDs used: none (U/D/S ranges 2067–2086 / 6100–6159 / 2060–2079 all unused)

---

## MCP / operational notes

- Pool slot race: `.pool_slot` was being overwritten by concurrent session startup hooks. Slot 11 was pre-assigned and used without contention at the MCP level (only one session opened Mashed_pool11).
- Slot 11 was stale; manually synced from master before opening.
- cap_count: 0 (halted before any cap check was needed)
- MCP failures: 0
- Scribe outcome: **n/a — halted** (no commit, no queue entry, no push)
