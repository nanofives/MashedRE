# SESSION_END — game_state_d4

**Date:** 2026-05-06  
**Slot used:** Mashed_pool10 (pre-assigned pool13 was locked by TTTTT session; pool10 selected as fresh available alternate)  
**Session ID:** game_state_d4  
**SHA-256 anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Items processed

| D | RVA | File | Confidence | New stubs |
|---|---|---|---|---|
| D-7240 | 0x00445aa0 | 0x00445aa0.md | C1 | S-2500, S-2501 |
| D-7241 | 0x00441d40 | 0x00441d40.md | C1 | (S-2500 shared) |
| D-7242 | 0x00442440 | 0x00442440.md | C1 | — |

All three functions fully mechanically described. No UUUUU-output DEFERRED rows in D=7240..7299 range were found.

## Stubs resolved

| S | RVA | Resolution |
|---|---|---|
| S-2440 | 0x00445aa0 | Analyzed as C1 — D-7240 complete |
| S-2441 | 0x00441d40 | Analyzed as C1 — D-7241 complete |
| S-2442 | 0x00442440 | Analyzed as C1 — D-7242 complete |

Note: S-2440/S-2441/S-2442 were never written to STUBS.md by game_state_d3 session. Resolved here without retroactive addition.

## New stubs

| S | D | RVA | Description |
|---|---|---|---|
| S-2500 | D-7420 | 0x00441c80 | FUN_00441c80 — interpolated XYZ getter; two vehicle slots via FUN_0046d4a0; lerp by _DAT_005cc32c |
| S-2501 | D-7421 | 0x004430a0 | FUN_004430a0 — 9B setter: DAT_00897fe0 = param_1 |

## New uncertainties

| U | Function | Topic |
|---|---|---|
| U-2507 | 0x00445aa0 | Player-slot byte array DAT_007f1042 stride 0x4c (40 entries); FUN_004430a0(0) loop purpose |
| U-2508 | 0x00445aa0 | State flag at param_1+0xa8; set/clear conditions |
| U-2509 | 0x00445aa0 + 0x00441d40 | DAT_007f101c sin-table oscillation; secondary matrix guard param_1+8!=1 |
| U-2510 | 0x00441d40 | Range gate at param_1+0xd0/0xd4 vs FUN_00408a50 result → weight |
| U-2511 | 0x00442440 | FUN_0040dc90 slot identity; 16-float block source |
| U-2512 | 0x00442440 | Saved matrix columns pre-overwrite; row-major vs column-major |

## U-2451 partial resolution

U-2451 (game_state_d3): "Semantic meaning of type 0/1/2 in entry array at 0x008964c0."  
Mechanical descriptions now available:
- Type 0: timestep-driven velocity/angle update with approach-state toggle and player-slot loop.
- Type 1: PD yaw+pitch controller toward interpolated target; weight gated on race-progress range.
- Type 2: snaps entity matrix from vehicle struct; zeroes weight.  
Semantic role (what these entries represent in game logic) remains [UNCERTAIN].

## Queue changes

- Removed: `game_state_d3-cont1` (all 3 RVAs drained).
- Added: `game_state_d4-cont1` → bucket `re/analysis/game_state_d5/`, D-7420..D-7421.

## Pool note

Pool13 was pre-assigned but held by TTTTT batch-17 lock (5:24 PM ART). Pool10 used as read-only substitute. No slot conflict — different slot, read-only session, no tracker intersection with TTTTT's work.
