# Frontend State Machine — `DAT_008a9584` cluster

**Produced by:** Session 6 (frontend_struct_extract), 2026-05-11  
**Pool slot:** Mashed_pool5  
**Source plates:** hud_frontend/, hud_frontend_d2/, hud_frontend_d3/, hud_frontend_d4/, c0_promotion_frontend_a/

---

## Struct: `FrontendStateMachine` (base 0x008a9584)

Global state machine that drives the front-end screens (main menu, character/vehicle select, multiplayer lobby, credits, etc.).  
Observed in `FUN_0040acd0` (dispatcher) and all frontend init/tick functions.

All fields are `int32` unless noted.

| Offset | Address | Name (tentative) | Width | First write RVA | Notes |
|--------|---------|-----------------|-------|-----------------|-------|
| +0x00 | 0x008a9584 | `ms_mode` | int32 | 0x0040acd0 | Outer dispatch key: cases 1–6. Written by all state-transition paths. |
| +0x04 | 0x008a9588 | `ms_subState1` | int32 | 0x0040acd0 | Sub-state for mode 1. |
| +0x08 | 0x008a958c | `ms_subState2` | int32 | 0x0040acd0 | Sub-state for mode 2. |
| +0x0C | 0x008a9590 | `ms_subState3` | int32 | 0x0040acd0 | Sub-state for mode 3. |
| +0x10 | 0x008a9594 | `ms_subState4` | int32 | 0x0040acd0 | Sub-state for mode 4. |
| +0x14 | 0x008a9598 | `ms_subState5` | int32 | 0x0040acd0 | Sub-state for mode 5 (debug-printed via wprintf). |
| +0x18 | 0x008a959c | `ms_subState6OneShotFlag` | int32 | 0x0040acd0 | Set to 1 externally, cleared to 0 after FUN_0042bfb0(0x288,...) fires. One-shot. |
| +0x1C | 0x008a95a0 | `ms_subState7` | int32 | 0x0040acd0 | Written in sub-state output; exact semantics not observed independently. |
| +0x24 | 0x008a95a8 | `ms_timerAccum` | float | 0x0040acd0 | `+= param_1` each tick; reset to 0.0f on state transitions. Compared against float thresholds at 0x005cc31c and 0x005cc320. |
| +0x28 | 0x008a95ac | `ms_subState8` | int32 | 0x0040acd0 | Written in sub-state output; also returned by FUN_0040b6c0 (same address). |
| +0x2C | 0x008a95b0 | `ms_tickCounter` | int32 | 0x0040acd0 | Incremented when FUN_005c9d00() == 0; gate for FUN_0045b350()/FUN_0042c1a0() when >1. |
| +0x30 | 0x008a95b4 | `ms_subState9` | int32 | 0x0040acd0 | Written in sub-state output; meaning unknown. |

**Total observed range:** 0x008a9584 – 0x008a95b4 (0x30 = 48 bytes minimum)

### Constants
| Address | Value | Role |
|---------|-------|------|
| 0x005cc31c | (float) | Timer threshold A (compared against `ms_timerAccum`) |
| 0x005cc320 | (float) | Timer threshold B |

### Uncertainties
- [UNCERTAIN U-3427] `FUN_0040acd0` body is 48 bytes but decompiler shows goto-linked code outside that range; struct writes may span a broader function. Evidence needed: listing disassembly of 0040acd0–0040ad00.
- [UNCERTAIN U-3428] FUN_0042bfb0 IDs 0x1b5–0x2a3 are unknown; mode semantics for values 1–6 are unconfirmed. Evidence needed: decompilation of FUN_0042bfb0.

---

## Struct: `FrontendModeSelector_007f1a0c` cluster

Controls which player's slot-search algorithm is active and tracks active player count.  
Observed in FUN_0042b960 (init), FUN_0042ae10, FUN_0042aeb0 (search tick).

| Address | Name (tentative) | Width | First write RVA | Notes |
|---------|-----------------|-------|-----------------|-------|
| 0x007f1a0c | `g_slotSearchMode` | int32 | 0x0042b960 | Dispatch key: 0x1000 = "scan all" (Path A); 1 = "slot-indexed" (Path B). Written 1 by FUN_0042b960. [UNCERTAIN U-3451] what sets it to 0x1000. |
| 0x007f1a14[0..3] × 0x10 | `g_playerSlotArray` | int32[4] (stride 0x10) | 0x0042b960 | 4-entry array; each entry's first int is the player slot ID (initialized by scanning loop). Loop upper bound 0x7f1a54. Read as `DAT_007f1a14[iVar5 * 4]` for sign check (< 0 = inactive). |
| 0x007f1a1c | `g_renderSlotSelector` | int32 | [UNCERTAIN] | At byte offset 0x8 within player-slot-0 entry. Used as base index into ESI-relative dispatch in FUN_0041de80: `ESI[DAT_007f1a1c + 7]`. |
| 0x007f1a24 | `g_playerSlot1Sentinel` | int32 | 0x0042b960 | First int of player-slot-1 entry; written -1 (0xffffffff) on init. [UNCERTAIN U-3450] |
| 0x007f1a34 | `g_playerSlot2Sentinel` | int32 | 0x0042b960 | First int of player-slot-2 entry; written -1 on init. |
| 0x007f1a44 | `g_playerSlot3Sentinel` | int32 | 0x0042b960 | First int of player-slot-3 entry; written -1 on init. |

**PlayerSlot entry layout** (0x10 bytes each):

| Byte offset within entry | Name (tentative) | Notes |
|-------------------------|-----------------|-------|
| +0x00 | `slotId` | int32; -1 = unassigned/inactive |
| +0x08 | `renderSlotSelector` | int32; used as base-7 index in FUN_0041de80 (only entry 0 read this way) |
| +0x04, +0x0C | [UNCERTAIN] | Not observed in plates |

### Char table pair (`DAT_007f1042` / `DAT_007f1502`)

Used by FUN_0042b960 / FUN_0042ae10 / FUN_0042aeb0 to scan for ready-slots.

| Address | Name | Stride | Entry count | Notes |
|---------|------|--------|-------------|-------|
| 0x007f1042 | `g_slotFlagTableA` | 0x4c (76 bytes) | 8 | char at entry[i] = flag A for slot i |
| 0x007f1502 | `g_slotFlagTableB` | 0x4c (76 bytes) | 8 | char at entry[i] = flag B; base is 0x7f1042 + 0x4c0 (16 entries × 76 bytes) |
| 0x007f1762 | — | — | — | exclusive upper-bound address for table-B scan |

- [UNCERTAIN U-3446] Per-entry structure within the 76-byte entries not characterized.
- [UNCERTAIN U-3448] `piVar3 += 4` on int* advances 16 bytes; whether each player-slot array entry is genuinely 0x10 bytes or layout differs needs listing verification.

---

## Struct: `GameMode_007f0fd0` + friends

Singleton globals that control the active game/HUD mode.

| Address | Name (tentative) | Width | First write RVA | Notes |
|---------|-----------------|-------|-----------------|-------|
| 0x007f0fd0 | `g_gameMode` | int32 | [UNCERTAIN] | Outer mode discriminant for HUD dispatch. Observed values: 2 (2P), 4, 5, 7, 8, 9, 10. [UNCERTAIN U-0569] full mode table. |
| 0x007f0fcc | `g_gameModeSubFlag` | int32 | [UNCERTAIN] | Sub-flag read when `g_gameMode == 9`: 0 → player 0 draws first; ≠0 → player 1 draws first. |
| 0x007f0fe8 | `g_mpOverlayFlag` | int32 | 0x00430120 | MP-mode overlay icons enable (non-zero = show icons 0x244–0x278). Cleared to 0 in FUN_00430120. |
| 0x007f1004 | `g_animDelta` | float? | [UNCERTAIN] | Per-frame animation delta; added to animation accumulators at 0x0039210 / 0x0043af10. |
| 0x007f1000 | `g_frameDeltaCounter` | int32? | [UNCERTAIN] | Frame delta accumulator: `+= DAT_007f1000` in FUN_0042e3a0; compared against threshold 15000. |

---

## Struct: `FrontendRenderState_0067ec` cluster

Per-draw-call render path selector and associated vertex buffer fill state.

| Address | Name (tentative) | Width | First write RVA | Notes |
|---------|-----------------|-------|-----------------|-------|
| 0x0067eca4 | `g_renderPath` | int32 | [UNCERTAIN] | 0 or 1 → early-out/alternate path; 2/3 → iterate menu items; 4 → fullscreen mode. Read in FUN_0043c5b0. |
| 0x0067eca8 | `g_vertexBufferReady` | int32 | [UNCERTAIN] | Non-zero → `RwIm2DRenderPrimitive(4, &DAT_0067ec30, 4)` is called at end of FUN_0042aae0. |
| 0x0067ec30 | `g_im2dVtxBuf2` | RwIm2DVertex[4] | [UNCERTAIN] | Second shared Im2D vertex buffer (4 verts × 7 dwords = 112 bytes). Used in FUN_0042aae0. |
| 0x0067ed30 | `g_animAccum` | float | [UNCERTAIN] | Animation accumulator; `+= DAT_007f1004` per call, then passed to sin() in FUN_00439210. |
| 0x0067eaa8 | `g_playerDataAlt` | int32 | [UNCERTAIN] | Read by FUN_00431710 as alternative to g_playerBarValues. |
| 0x0067eab0 | `g_state2Guard` | int32 | [UNCERTAIN] | Checked == 2 in FUN_0042ae10/FUN_0042aeb0 guard conditions. |
| 0x0067e7c8 | `g_screenGuard` | int32 | [UNCERTAIN] | Checked == 0 in FUN_0042ae10 guard (but not FUN_0042aeb0). |
| 0x0067e830 | `g_selectorMode` | int32 | [UNCERTAIN] | 1 → use DAT_0067ed38 table; else → DAT_0067ed78 table. Read by FUN_004314b0. |
| 0x0067e834 | `g_selectorAlpha` | int32 (byte-sized) | [UNCERTAIN] | Alpha byte for selector render in FUN_004314b0. |
| 0x0067e9f8 | `g_selectorSlotIndex` | int32 | [UNCERTAIN] | Current slot cursor index; used as table index in FUN_004314b0. |
| 0x0067ed38 | `g_slotConfigPtrTable` | ptr[?] | [UNCERTAIN] | Mode-1 slot config pointer table; stride 0x10; used by FUN_004314b0. |
| 0x0067ed78 | `g_slotConfigPtrTableAlt` | ptr[?] | [UNCERTAIN] | Mode-0 (alt) slot config pointer table; stride 0x10. |
| 0x0067e7f8 | `g_barRenderFlag` | int32 | [UNCERTAIN] | Early-out guard in FUN_00430b90 (0 = skip). |
| 0x0067e7fc | `g_barAlpha` | int32 (byte-sized) | [UNCERTAIN] | Alpha byte for bar render in FUN_00430b90. |
| 0x0067f0c0 | `g_vehicleMode` | int32 | [UNCERTAIN] | Passed to FUN_0042bcb0 condition: == 6 selects alternate icon. |
| 0x0067f0c4 | `g_vehicleXPos` | float? | [UNCERTAIN] | Vehicle icon X position in FUN_004335f0. |

---

## Struct: `FrontendSingletons_0089` cluster

Miscellaneous frontend globals in the 0x008990xx–0x008991xx range.

| Address | Name (tentative) | Width | First write RVA | Notes |
|---------|-----------------|-------|-----------------|-------|
| 0x008990e0 | `g_slideInCounter` | int32 | [UNCERTAIN] | Slide-in panel animation counter; range 0..0x200; decrements by 0x10 per frame in FUN_0042e5b0. Zero = fully slid in. |
| 0x008990e4 | `g_dimOneShotFlag` | int32 | 0x0042aad0 | One-shot dim/greyed-out flag. Set to 1 by FUN_0042aad0; read and cleared to 0 by FUN_00428140 which caps sprite alpha at 0x60 when set. |
| 0x00898ab0 | `g_screenActiveFlag` | int32 | [UNCERTAIN] | Checked != 0 in FUN_0042ae10/FUN_0042aeb0 guard (screen must be active). |
| 0x00898ab8 | `g_bgTexture1` | ptr | [UNCERTAIN] | Background texture 1 pointer, passed to FUN_00473c20 in FUN_0042e5b0. [UNCERTAIN: also seen in array context in hud_frontend_d2/0x00431240.md] |
| 0x008991a0 | `g_bgTexture2` | ptr | [UNCERTAIN] | Background texture 2 pointer, passed to FUN_00473c20 in FUN_0042e5b0. |
| 0x008991bc | `g_initClearFlag` | int32 | [UNCERTAIN] | Zeroed in FUN_004298c0 init path. |

---

## Struct: `PerPlayerBarValues_007f0f` cluster

Per-player HUD progress bar values.

| Address | Name (tentative) | Width | First write RVA | Notes |
|---------|-----------------|-------|-----------------|-------|
| 0x007f0f00 | `g_barValueP1` | float? | [UNCERTAIN] | Bar fill value for player 1 (slot index 1); read in FUN_00430b90 case 1. |
| 0x007f0f04 | `g_barValueP0` | float? | [UNCERTAIN] | Bar fill value for player 0 (slot index 0); read in FUN_00430b90 case 0. |
| 0x007f0f08 | `g_barValueP2` | float? | [UNCERTAIN] | Bar fill value for player 2 (slot index 2); read in FUN_00430b90 case 2. |
| 0x007f0f10 | `g_itemSelectorP3` | int32 | [UNCERTAIN] | Item selector for slot 3; values 0→0x59, 1→0x1b2, 2→0x1b1 in FUN_00430b90. |
| 0x007f0f30 | `g_boolArraySlotFlags` | int32[?] | [UNCERTAIN] | Bool array; piVar10 = &DAT_007f0f30 + (puVar6==sentinel); read per item in FUN_004314b0. |

---

## Struct: `PlayerScoreArrays_008a94f0` cluster

Sorting source arrays for leaderboard/results rendering.

| Address | Name (tentative) | Stride | Entry count | Notes |
|---------|-----------------|--------|-------------|-------|
| 0x008a94f0 | `g_scoreValArray` | 4 bytes (int32) | 4 | Value-to-sort per player; read by FUN_0040b460 loop A. If corresponding 0x7f1a14 entry < 0, score is overridden to -1. |
| 0x008a9530 | `g_rankArrayA` | 4 bytes (int32) | 4 | Returned by FUN_0040b6b0 / FUN_0040b7b0 case 0. |
| 0x008a9540 | `g_rankArrayB` | 4 bytes (int32) | 4 | Returned by FUN_0040b7b0 case 2. |
| 0x008a9550 | `g_rankArrayC` | 4 bytes (int32) | 4 | Returned by FUN_0040b7b0 case 3. |
| 0x008a9560 | `g_rankArrayD` | 4 bytes (int32) | 4 | Returned by FUN_0040b7b0 case 1. |
| 0x008a95ac | `ms_subState8` | int32 | 1 | Also referenced as FrontendStateMachine+0x28. |

- [UNCERTAIN U-2089] Semantics of `g_scoreValArray` (0x008a94f0) vs `g_rankArrayA` (0x008a9530): relationship unknown. Evidence: xrefs to write sites.

---

## Struct: `ProfileSubStateMachine_008a9584-outer`

Additional sub-states referenced in `FUN_0040acd0` outer dispatch (debug strings indicate save/load/format).

| Address | Name (tentative) | Width | Notes |
|---------|-----------------|-------|-------|
| 0x008a9384 | `g_initReset1` | int32 | Zeroed in FUN_00414120. |
| 0x008a9378 | `g_initReset2` | int32 | Zeroed in FUN_00414120. |
| 0x008a9380 | `g_initReset3` | int32 | Zeroed in FUN_00414120. |
| 0x008a9420 | `g_profileScratch` | byte[0x9c] | Zeroed (0x9c bytes) in FUN_00414120. |
| 0x008a9384 | `g_profileDataCopy` | byte[0x9c] | Destination; receives copy from ROM default at 0x005f2a70 (0x9c bytes). |

- [UNCERTAIN U-3432] Semantics of structs at 0x0089a384 / 0x0089a420 / 0x005f2a70 unknown. 0x9c = 156 bytes = 39 dwords. Evidence: xrefs + type analysis.
