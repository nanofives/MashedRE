# Session End — hud_ingame_d3

**Session:** hud_ingame_d3-20260506  
**Slot:** Mashed_pool13  
**Bucket:** re/analysis/hud_ingame_d3/  
**Parent:** hud_ingame_d2  
**Subsystem:** hud  

## Work summary

### D-items cleared

| D-ID | RVA | disposition |
|------|-----|-------------|
| D-2680 | 0x00450b10 | CLEARED → C1/new; leaf Im2D textured quad draw; 0 callees; no new D-items spawned |

### New hooks.csv entries

| RVA | name | status | notes |
|-----|------|--------|-------|
| 0x00450b10 | FUN_00450b10 | C1/new | Im2D textured quad draw; leaf; static buf 0x00898a20 |
| 0x00428450 | FUN_00428450 | C1/new | spinning-coin animator; U-2107; caller of FUN_00450b10 |

### ID allocation used

- U-2107: FUN_00428450 (spinning-coin animator)
- D-2680: cleared (no new D-IDs created; depth-4 cap not triggered)
- S-range: 0 stubs assigned

### Cap check

FUN_00450b10 is a leaf (0 callees). No depth-4 cap trigger. No new D-entries in D=6220..6279 range.

## Open items

FUN_00428450 has 4 unknown callers not chased:
- 0x00428590 FUN_00428590
- 0x00428a30 FUN_00428a30
- 0x00428bf0 FUN_00428bf0
- 0x00428d30 FUN_00428d30

These are callers of a newly-discovered function (U-2107), not callees of the session root. Recommend chasing in a follow-up sweep if these callers are HUD-subsystem.

## Pre-flight results

- SHA256: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
- Slot: Mashed_pool13 (synced from master)
- health_ping: ok
- ghidra_info: Ghidra 12.0.3 ✓
- program_list_open: path contains Mashed_pool13 ✓
