# Frontend Menu Navigation State

**Session:** struct_extract_phase6_pt3 (2026-05-14, Session 96)
**Evidence sources:** re/analysis/promote_c2_frontend_menus/ plates, re/analysis/frontend_promote_menus_a/ plates, re/analysis/frontend_promote_menus_b/ plates
**Confidence:** C1 — fields mechanically observed from decompilation; no Frida confirmation
**Related struct:** re/analysis/structs/frontend_state.md (covers FrontendStateMachine at DAT_008a9584, mode selector, per-player bar values, score arrays)

---

## Overview

This file documents the menu-navigation sub-layer: cursor positions, entry validity masks, player slot data, menu entry structs, and score arrays used by the frontend menu screens (character/vehicle select, post-race leaderboard, slot-assignment). The outer FrontendStateMachine at `DAT_008a9584` is in frontend_state.md.

---

## Menu Cursor / Limit Arrays (DAT_0067ed40 cluster)

Observed in FUN_0042aa00.

| Address | Stride | Tentative name | Notes |
|---------|--------|----------------|-------|
| `DAT_0067ed40` | 0x40 per slot | `g_menuCursorArray` | Menu cursor position per slot; indexed as `[slot × 0x40]`; first cite promote_c2_frontend_menus/0x0042aa00.md |
| `DAT_0067ed74` | 0x40 per slot | `g_menuLimitArray` | Menu limit/item-count per slot; indexed as `[slot × 0x40]`; same function |
| `DAT_0067ed84` | 0x10 per slot | `g_menuEntryValidArray` | Entry validity flag array; int8 per entry; 1 = valid; indexed as `[cursor + slot × 0x10 − 0x10]`; first cite promote_c2_frontend_menus/0x0042aa00.md |

---

## Menu Animation State

| Address | Width | Tentative name | Notes |
|---------|-------|----------------|-------|
| `DAT_0067f17c` | 4 | `g_menuAnimFrameCounter` | Animation frame counter; first cite promote_c2_frontend_menus/0x0042f0b0.md |
| `DAT_0067e9f8` | 4 | `g_selectorSlotIndex` | Current slot cursor index; used as table index in FUN_004314b0 (also cross-referenced in frontend_state.md as g_selectorSlotIndex) |
| `DAT_0067ea90` | 4 | `g_raceEndFlagOrSlotGate` | Race-end flag or slot-entry gate; checked in FUN_0042fe80; first cite promote_c2_frontend_menus/0x0042fe80.md |

---

## Render Vertex Data Block (DAT_0067ec30 cluster)

4 Im2D vertex data blocks used for menu item rendering in FUN_0042aae0. These extend the Im2D vertex buffer cluster already noted in frontend_state.md.

| Address | Tentative name | Notes |
|---------|----------------|-------|
| `DAT_0067ec30` | `g_menuVtxBlockW0H0` | Render vertex data W/H for block 0 |
| `DAT_0067ec4c` | `g_menuVtxBlockW1H1` | Render vertex data W/H for block 1 |
| `DAT_0067ec68` | `g_menuVtxBlockH2` | Render vertex data H for block 2 |
| `DAT_0067ec84` | `g_menuVtxBlockW3H3` | Render vertex data W/H for block 3 |

---

## Player Slot Struct Array (DAT_007f0a48 cluster)

Observed in FUN_0042ebe0 (dispatch function with param_1 type codes 10/11/12).

| Address | Stride | Tentative name | Notes |
|---------|--------|----------------|-------|
| `DAT_007f0a48` | 0x1e0 per slot (480 bytes) | `g_playerSlotStructArray` | Player slot struct array; 0x78 dwords per slot; first cite promote_c2_frontend_menus/0x0042ebe0.md |
| `DAT_007f0a44` | 0x0c per param | `g_enableFlagTable` | Enable flag table; dispatch-indexed; stride 0x0c per param_1 value |
| `DAT_007f0c28` | — | `g_playerArrayLoopBound` | Loop boundary for player array iteration |

### Internal field observations within the 0x1e0-byte player slot struct

Three dispatch paths (param_1 = 10/11/12) access different field patterns:

| param_1 | Offsets accessed | Stride | Notes |
|---------|-----------------|--------|-------|
| 10 (0xa) | +0x2c, +0x44, +0x5c, +0x74, +0x8c, +0xa4, +0xbc, +0xd4, +0xec | 0x18 | 9-field check array (stride 0x18); field at offset −4 relative to each access point |
| 11 (0xb) | +0x0c, +0x18, +0x24, +0x30, +0x3c, +0x48, +0x54, +0x60, +0x6c | 0x0c | 9-field check array (stride 0x0c) |
| 12 (0xc) | +0x0f, +0x1b, +0x27, +0x33, +0x3f, +0x4b, +0x57, +0x63, +0x6f | 0x0c | Same as param_1=11 with +3 byte offset variant |

Slot activity flags (4 elements, stride 0x10): `DAT_007f1a14`; value −1 = inactive (cross-reference with frontend_state.md `g_playerSlotArray`).

---

## Menu Entry Struct Array (DAT_00898ac0)

52-byte stride per entry (0xd dwords), 30 entries total. Observed in FUN_0042d3e0 (bulk clear).

Base address: `DAT_00898ac0`. Array spans `0x00898ac0..0x00898c28` (30 × 52 bytes).

| Byte offset within entry | Width | Notes |
|--------------------------|-------|-------|
| −4 | 4 | Pre-field (partially cleared) |
| +0x00 | 4 | Field 0; cleared to 0 |
| +0x04 | 4 | Field 1; cleared to 0 |
| +0x08 | 4 | Field 2; cleared to 0 |
| +0x09 | 1 | Field 3 (byte); cleared to 0 |
| +0x0a | 1 | Field 4 (byte); cleared to 0 |
| +0x0b | 1 | Field 5 (byte); cleared to 0 |
| +0x0c | 1 | Field 6 (byte); cleared to 0 |
| +0x10 | 4 | Field 7; cleared to 0 |
| +0x14 | 4 | Field 8; cleared to 0 |
| +0x18 | 4 | Field 9; cleared to 0 |
| +0x1c | 4 | Field 10; **NOT** cleared (preserved across bulk-clear) |
| +0x20 | 4 | Field 11; cleared to 0 |
| +0x24 | 4 | Field 12; cleared to 0 |
| +0x28 | 4 | Field 13; cleared to 0 |

Internal semantics of all 14 fields are [UNCERTAIN U-3836]. The preservation of field 10 (+0x1c) across bulk-clear is a strong signal that it holds a type/ID tag or persistent state.

---

## Per-Slot Score / Rank Arrays (batch_n Session 79 evidence)

Observed in FUN_0040b460 and FUN_0040b620.

| Address | Stride | Count | Tentative name | Notes |
|---------|--------|-------|----------------|-------|
| `DAT_008a9500` | 4 | 4 | `g_scoreTimerGroupA` | Score/timer array group A; sentinel −1000; first cite frontend_promote_menus_b/0x0040b460.md |
| `DAT_008a9504` | — | — | (entry 1 of above) | |
| `DAT_008a9508` | — | — | (entry 2 of above) | |
| `DAT_008a950c` | — | — | (entry 3 of above) | |
| `DAT_008a9510..008a951c` | 4 | 4 | [UNCERTAIN U-3837] | Gap block between groups; not written in observed plates |
| `DAT_008a9520` | 4 | 4 | `g_scoreTimerGroupB` | Score/timer array group B; sentinel −1000; indexed as `[param_1 × 4]`; first cite frontend_promote_menus_b/0x0040b620.md |
| `DAT_008a94f0` | 4 | 4 | `g_scoreValArray` | Value-to-sort per player; first cite frontend_promote_menus_b (cross-ref frontend_state.md) |
| `DAT_008a9530` | 4 | 4 | `g_rankArrayA` | Rank sort result A (cross-ref frontend_state.md) |

Game mode globals:

| Address | Width | Notes |
|---------|-------|-------|
| `DAT_007f0fd0` | 4 | `g_gameMode` — mode selector (values 4, 8, 9 trigger special placement); also cross-ref frontend_state.md |
| `DAT_007f0fcc` | 4 | `g_gameModeSubFlag` — mode-9 variant flag |

---

## Open uncertainties

| U-ID | Gap |
|------|-----|
| U-3836 | Menu entry struct (DAT_00898ac0, 52-byte stride) — 14-field layout; only bulk-clear pattern observed; semantics of each field unknown |
| U-3837 | DAT_008a9510..008a951c gap block — not written; relationship to score arrays unknown |
| U-3838 | Player slot struct (DAT_007f0a48, 0x1e0 stride) — only 3 dispatch access-patterns seen; lower-offset fields +0x00..+0x0b not characterized |
