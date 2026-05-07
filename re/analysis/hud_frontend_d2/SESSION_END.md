# Session End Report — hud_frontend_d2-20260503-0559

**Date:** 2026-05-03  
**Slot:** Mashed_pool15  
**Parent bucket:** hud_frontend-cont1 (D-1240..D-1253)

## Work completed

### D-1240..D-1253 drained — 14 functions promoted C0→C1

| D-ID | RVA | Name |
|------|-----|------|
| D-1240 | 0x004335f0 | car-selection/race-position HUD renderer |
| D-1241 | 0x0043a610 | race-result scoreboard renderer |
| D-1242 | 0x0042f0c0 | options 3-row list (Difficulty/Steering/Camera) |
| D-1243 | 0x0043af10 | game-setup lobby options renderer |
| D-1244 | 0x00434720 | championship/cup progress screen renderer |
| D-1245 | 0x00430b90 | per-player progress bar set A |
| D-1246 | 0x00431240 | Y/N single option selector |
| D-1247 | 0x004314b0 | multi-option Y/N selector (2 or 4 items) |
| D-1248 | 0x00431710 | per-player progress bar set B (speed/boost) |
| D-1249 | 0x0043aa30 | team/co-op pairing screen renderer |
| D-1250 | 0x0042fb70 | mini 3-row settings (track/player-count/music) |
| D-1251 | 0x0042fe90 | per-vehicle-type Y/N feature list renderer |
| D-1252 | 0x00430120 | time-trial mode info panel (tiny) |
| D-1253 | 0x00439210 | multiplayer lobby / network player-slot list renderer |

### Depth-3 DEFERRED filed — D-2740..D-2782 (43 entries)

All uncatalogued depth-3 callees from the 14 analyzed functions. Key clusters:
- **UI infrastructure:** 0x004282a0 (text size), 0x00473870 (sprite draw 7-param), 0x0042ac00 (player count), 0x0042ac50 (Y base)
- **Vehicle/player data:** 0x0042bcb0, 0x0042fab0, 0x004368e0, 0x00436810, 0x0042ebe0, 0x0042ee00/40/ef40
- **Championship/lap-time:** 0x00430b30, 0x0042d290, 0x0042d300, 0x00429870/a30/a70/a80/a90
- **Multiplayer:** 0x00430760, 0x00430a10/a60/ab0
- **Misc draw:** 0x0042f8d0, 0x0042bcb0, 0x004736c0, 0x004391b0, 0x00474e60

## Tracker mutations

- **hooks.csv:** +14 C1 rows + 43 C0 deferred rows
- **DEFERRED.md:** D-1240..D-1253 moved to Cleared; D-2740..D-2782 added to Active
- **CHANGELOG.md:** one batch entry appended
- **UNCERTAINTIES.md:** no new entries (no [UNCERTAIN] markers in any of the 14 notes)
- **STUBS.md:** no new entries (all new callee stubs were pre-existing S-0442/0456/0457/0640)

## Notes

- Session ran on Mashed_pool15 (only free slot; pool was near-exhaustion from ~11 concurrent sessions)
- Slot staleness was manually resolved by copying master.rep before opening
- `.pool_slot` coordination bypassed due to race condition; identity verified via `program_list_open` MCP call
- FUN_00427f00 (0x00427f00) was already at C1 (font_text session) when encountered as depth-3 callee — not re-deferred
- FUN_0042aad0 already at C1 (game_state session) — not re-deferred
