# Session End Report — credits_screen-20260506

**Date:** 2026-05-06
**Slot:** Mashed_pool7
**Anchor strategy:** String anchors ("Supersonic", ".mpg") + frontend call-graph (screen-type pointer == &DAT_005f6980)

## Anchor outcome: FOUND (Strategy 1 + Strategy 3 combined)

- No "Credits"/"CREDITS"/credits.mpg string found (all 0 hits).
- "Supersonic Software Ltd." at `0x005f64d8` found but 0 Ghidra xrefs — copyright notice embedded in data section, not used in function calls.
- No credits.mpg in movie list — credits are NOT an MCI video.
- Frontend call-graph: `FUN_0042e3a0` (chrome drawer) checks `(&DAT_0067ed38)[DAT_0067e9f8 * 0x10] == &DAT_005f6980` and calls `FUN_0042d5a0(DAT_0067ebc0 - 15000)` when true.
- `DAT_005f6980` confirmed as credits screen descriptor (contains `ff`-tagged script data, same format as other screen descriptors).

## CREDITS_FN

- **`FUN_0042e3a0`** (0x0042e3a0) — already C1 from hud_frontend session. The credits scroll controller is embedded inside the chrome-band drawer. When `DAT_0067ed38[DAT_0067e9f8 * 0x10] == &DAT_005f6980`, it accumulates `DAT_0067ebc0 += DAT_007f1000` and calls `FUN_0042d5a0(DAT_0067ebc0 - 15000)`.

## Credits-related strings observed (verbatim)

| Address | String |
|---------|--------|
| 0x005f64c8 | `'Toast' is an original` |
| 0x005f64e0 | `game concept by` |
| 0x005f64d8 | `Supersonic Software Ltd.` |
| 0x005f6500 | `© 2003, all rights reserved.` |
| 0x005f6520 | `Please address enquiries / comments` |
| 0x005f6538 | `to toast@supersonic-software.com` |
| 0x005f6580 | `Toast Toast Toast Toast Toast Toast Toast Toast Toast Toast Toast Toast Toast ` |
| 0x005cf918 | `toastart\pc\movies\supersonic.mpg` |

(These strings are in the .rdata section with 0 Ghidra xrefs — likely referenced via pointer in a data structure table, not via direct immediate in code.)

## Functions covered

| RVA | Name | Old | New | Notes |
|-----|------|-----|-----|-------|
| 0x0042d5a0 | FUN_0042d5a0 | stub S-0449 | C1 | Credits sprite-timeline renderer |
| 0x00472f40 | FUN_00472f40 | stub S-0450 | C1 | Top-gradient quad draw |
| 0x004730b0 | FUN_004730b0 | stub S-0451 | C1 | Bottom-gradient quad draw |

## Tracker counts

- hooks.csv: +3 C1 rows
- STUBS.md: -3 rows (S-0449, S-0450, S-0451 cleared)
- UNCERTAINTIES.md: +3 rows (U-2177, U-2178, U-2179)
- DEFERRED.md: 0 new rows (all depth-2 callees pre-tracked)
- CHANGELOG.md: +3 entries

## cap_count = 3 / cap = 20 — early finish

## MCP failures: 0

## Observations

- The credits screen is NOT a video (no credits.mpg). Credits are rendered as a sprite-timeline: 84 hardcoded (x, y, scroll_trigger, sprite_id) entries, sprite IDs 0x15c–0x1b0, drawn at scale 0.6 with fade-in/plateau/fade-out alpha per entry.
- The `ff`-tagged script data at 0x005f6800 / 0x005f6c00 / 0x005f6980 (screen descriptors) is a separate format not yet decoded; it appears to be the menu/frontend screen configuration system (not credits text).
- The copyright notice strings ("Supersonic Software Ltd.", email, etc.) at 0x005f64d8 are in a data block with 0 Ghidra xrefs. They may be displayed via a lookup table that references this region indirectly, or they are the game's internal copyright metadata not displayed at runtime.
- U-2178 (sprite ID → TXD/spritesheet mapping) is the key unresolved question for a full credits reimplementation: which asset file provides sprites 0x15c–0x1b0?
- Credits entry point (what sets the screen-type pointer to &DAT_005f6980) was not located this session; that's a follow-up for credits_screen-cont1 or hud_frontend_d3.

## Scribe outcome: queued (see below)
