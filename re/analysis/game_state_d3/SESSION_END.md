# SESSION_END — game_state_d3 (UUUUU)

**Date:** 2026-05-06  
**Slot used:** Mashed_pool0  
**Session ID:** game_state_d3  

## Items processed

| D | RVA | File | Confidence | New stubs |
|---|---|---|---|---|
| D-2920 | 0x0042bf30 | 0x0042bf30.md | C2 | — |
| D-2921 | 0x0042d3a0 | 0x0042d3a0.md | C2 | — |
| D-2922 | 0x004248b0 | 0x004248b0.md | C2 | — |
| D-2923 | 0x00424920 | 0x00424920.md | C2 | — |
| D-2924 | 0x004464c0 | 0x004464c0.md | C1 | S-2440, S-2441, S-2442 |

## New uncertainties

| ID | Function | Topic |
|---|---|---|
| U-2447 | 0x0042bf30 | Meaning of DAT_0067ea5c read-and-clear |
| U-2448 | 0x0042bf30 | Semantic interpretation of 6-param event record |
| U-2449 | 0x004248b0 | Semantic meaning of copy+increment (lap/race counter?) |
| U-2450 | 0x00424920 | Semantic meaning of 32 accumulator targets |
| U-2451 | 0x004464c0 | Semantic meaning of type 0/1/2 in entry array at 0x008964c0 |

## New stubs → deferred

| S | D | RVA | Description |
|---|---|---|---|
| S-2440 | D-7240 | 0x00445aa0 | FUN_00445aa0 — type-0 handler in FUN_004464c0 dispatch |
| S-2441 | D-7241 | 0x00441d40 | FUN_00441d40 — type-1 handler |
| S-2442 | D-7242 | 0x00442440 | FUN_00442440 — type-2 handler |

## Queue entry added

`game_state_d3-cont1` → bucket `re/analysis/game_state_d4/`, D-7240..D-7242.
