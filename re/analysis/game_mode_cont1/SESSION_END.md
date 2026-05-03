# Session GGGG End — game_mode_cont1

**Completed:** 2026-05-03  
**Pool used:** Mashed_pool4 (released)  
**cap_count:** 15 (soft cap hit)  
**Functions covered (C1):** 15  
**Functions deferred (C0):** 29 (D-4840..D-4868)  
**Stubs added:** S-1640..S-1656 (17 entries)  
**Uncertainties added:** U-1647..U-1656 (10 entries)

## Next session queue: game_mode_cont2

Deferred subset: D-4840..D-4868 (28 callees + 1 caller of 0x0043dfd0).  
Priority order by size:
1. 0x0042aa00 (168B, D-4846) — called from FUN_004322c0 as S-1655
2. 0x0042ae10/0042aeb0/0042af50 (156B each, D-4841/42/43) — triple-family like 0x0042b310 trio
3. 0x004323c0 (135B, D-4840)
4. 0x00492d30 (265B, D-4868) — sole caller of 0x0043dfd0; may reveal game-mode classification
5. 0x00430910 (137B, D-4844, S-1656), 0x00429aa0 (134B, D-4845)
6. Remainder in size order

Subsystem: frontend (most), util (0x00492d30, tiny utilities)  
ID-range suggestion: U=1657..1676  D=4869..4929  S=1660..1679
