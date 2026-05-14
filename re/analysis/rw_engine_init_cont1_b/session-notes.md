# Session: rw_engine_init_cont1_b — Full Drift-Skip

**Date:** 2026-05-12  
**Pool slot:** Mashed_pool1 (pre-assigned)  
**Model:** claude-sonnet-4-6

## Outcome: 19/19 candidates drift-skipped

All 19 candidate RVAs were already at C1+ in hooks.csv before this session began.
No new analysis plates produced. No Ghidra work required.

## Drift evidence

| RVA | D-ID | hooks.csv session | hooks.csv row |
|-----|------|-------------------|---------------|
| 0x004cae90 | D-0232 | rw_engine_init_d2-20260502-1905 | 166 |
| 0x004d7ff0 | D-0233 | rw_engine_init_cont1-20260512 | 1527 |
| 0x004d8480 | D-0234 | rw_engine_init_cont1-20260512 (+ audio row) | dup |
| 0x004d7ca0 | D-0235 | rw_engine_teardown_d2 | — |
| 0x004ccf20 | D-0236 | rw_engine_teardown_d2 | — |
| 0x004c7a60 | D-0237 | rw_engine_init_d2-20260502-1905 | 145 |
| 0x004cc7e0 | D-0238 | rw_engine_init_cont1-20260512 | 1529 |
| 0x004cce20 | D-0239 | rw_engine_init_cont1-20260512 | 1530 |
| 0x004d7c60 | D-0240 | rw_engine_init_cont1-20260512 | 1531 |
| 0x004d7de0 | D-0241 | rw_engine_init_cont1-20260512 | 1532 (+ dup 847) |
| 0x004d8560 | D-0242 | rw_engine_init_cont1-20260512 | 1533 |
| 0x004d8570 | D-0243 | rw_engine_init_cont1-20260512 | 1534 |
| 0x004e5d30 | D-0244 | rw_engine_init_cont1-20260512 | 1535 |
| 0x00549640 | D-10545 | rw_engine_init_d2_cont1_b | 1658 |
| 0x005578a0 | D-10546 | rw_engine_init_d2_cont1_b | 1659 |
| 0x005515a0 | D-10547 | rw_engine_init_d2_cont1_b | 1660 |
| 0x0052d8e0 | D-10548 | rw_engine_init_d2_cont1_b | 1661 |
| 0x0057c270 | D-10549 | rw_engine_init_d2_cont1_b | 1662 |
| 0x00561ee0 | D-10550 | rw_engine_init_d2_cont1_b | 1663 |

## DEFERRED.md changes

- Struck through D-0232..D-0244 in main active table (13 rows)
- Struck through D-10545..D-10550 in main active table (6 rows)
- Added resolution log entries for D-10545..D-10550

## Duplicate row note

hooks.csv has duplicate rows for 0x004d7ff0 (render row 1527 + audio row ~394),
0x004d8480 (render + audio), and 0x004d7de0 (rows 847 + 1532). The sweep should
deduplicate — whichever row has the more informative notes field should be kept.
The `audio` classification for 004d7ff0 and 004d8480 is suspect (those are
`identity passthrough` and `error-state writer` functions in the RW plugin registry
call tree; the `render` rows from rw_engine_init_cont1-20260512 are the correct
classification).
