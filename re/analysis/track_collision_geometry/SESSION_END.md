---
session: track_collision_geometry_s14
date: 2026-05-11
pool_slot: Mashed_pool13
model: claude-sonnet-4-6
rva_count: 20
hard_cap: 20
---

## Summary

20 RVAs reversed at C1 in one session. No overflow to cont1 needed.

**Collision geometry primitives (2 RVAs):**
- `0x00547bf0` FUN_00547bf0 — AABB vs triangle SAT (12-axis, 5807 bytes). Outcode-based vertex classification + 9 edge-edge cross-product axes. Leaf function. Clears S-1964.
- `0x00547450` FUN_00547450 — sphere vs triangle closest-point test (1951 bytes). 3-phase: slab rejection, vertex/edge/face proximity, normal output via FastInvSqrt. Clears S-1965.

**COURSE.LUA Lua C handlers (18 RVAs, 0x0047a1b0..0x0047aa50):**
All are registered in the Lua C function table at LAB_00440bc0. Called during track init via FUN_0047a020 → FUN_0047b9b0. They write course asset filenames and resolved handles into BSP struct A (DAT_006bf1c8) and BSP struct B (DAT_006bf1cc) using three shared Lua C API wrappers (S-3571..S-3573, deferred to input_lua session).

Key structural finding: the "BSP setters" in the range 0x0047a1b0..0x0047ab28 are NOT physics-primitive setters as initially hypothesized in physics_collision/findings.md — they are course asset filename registrars. The physics primitives are wired at runtime via the RWP37Active vtable path (D-1792, still open).

**New struct document:** `re/analysis/structs/bsp_struct_a.md` — first comprehensive slot-map of BSP struct A (0x2ae0 bytes, stride 0x40 per slot, ~166 slots total, ~20 mapped).

## Trackers

| Tracker | New entries | IDs |
|---|---|---|
| hooks.csv | 20 rows | 0x00547bf0..0x0047aa50 |
| STUBS.md | 3 new | S-3571..S-3573 |
| UNCERTAINTIES.md | 5 new | U-3568..U-3572 |
| DEFERRED.md | 3 new | D-10542..D-10544 |
| Stubs cleared | 2 | S-1964, S-1965 |

## Deferred

- D-10542..D-10544: FUN_004b6fc0/004b70d0/004b7090 (S-3571..S-3573) — Lua C API binding analysis deferred to input_lua session.
- D-1792: runtime collision test via RWP37Active vtable — still requires Frida hardware watchpoint; not solvable statically.

## Ghidra MCP note

Ghidra MCP tools were unavailable in this session. Decompilation obtained via direct pyghidra script (`scripts/decomp_collision.py`). All analysis is from actual decompiler output saved to `log/decomp_collision_s14.txt`.
