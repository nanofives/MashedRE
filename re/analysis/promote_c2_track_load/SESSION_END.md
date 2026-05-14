---
session: promote_c2_track_load-20260513
date: 2026-05-13
pool_slot: Mashed_pool5
model: claude-sonnet-4-6
rva_count: 20
hard_cap: 24
---

## Summary

20 track functions promoted C1→C2. All sourced from the `track_collision_geometry` session
(session 14, 2026-05-11). C1 analysis notes already contained full mechanical descriptions;
this session added C2-format files with Purpose/Shape/Callees/Key-constants structure and
verified 2 representative Lua C handlers (0x0047a610, 0x0047a720) and confirmed all 4
spot-checked RVAs present via Ghidra MCP function_at.

**Group 1 — COURSE.LUA Lua C filename-setter handlers (18 RVAs, 0x0047a1b0..0x0047aa50):**

Three structural patterns:
- **Fixed-slot writers** (0x0047a1b0, 0x0047a5b0, 0x0047a5e0, 0x0047a580, 0x0047a880, 0x0047a6f0): get arg count, get string arg, copy to a fixed byte offset in BSP struct A. No counter.
- **Fixed-slot + flag writers** (0x0047a540, 0x0047a6b0): fixed-slot copy + writes 1 to a flag dword in BSP struct B.
- **Slot-group appenders** (0x0047a280, 0x0047a320, 0x0047a3a0, 0x0047a4a0, 0x0047a610, 0x0047a8b0, 0x0047aa50): two-branch design (one-arg: auto-counter; two-arg: FUN_004a2c48 resolve). Write string to `(counter + base_idx) * 0x40 + DAT_006bf1c8`.
- **Handle writers** (0x0047a720, 0x0047a790): numeric arg only, no string; write resolved handle into unknown struct at DAT_006bf1d8.
- **Flag-array setter** (0x0047aa20): no string; sets `*(BSP_B + 0xd4 + idx*4) = 1`.
- **DFF + handle appender** (0x0047a320): string + companion handle at BSP-A + 0x2ac0.

**Group 2 — Collision geometry leaf functions (2 RVAs):**
- `0x00547bf0` FUN_00547bf0 — AABB vs triangle SAT (12-axis, 5807 bytes). Leaf. Clears S-1964.
- `0x00547450` FUN_00547450 — sphere vs triangle closest-point (1951 bytes). Calls FastInvSqrt. Clears S-1965.

## Trackers

| Tracker | Change |
|---|---|
| hooks.csv | 20 rows C1→C2 |
| STUBS.md | No changes (S-3571..S-3573 remain deferred) |
| UNCERTAINTIES.md | No changes (U-3568..U-3571 carried forward as noted in C2 files) |

## Remaining C1 track functions (not targeted this session)

After this promotion the track subsystem has 20 C2 rows (all from this session) and the
remaining C1 rows are in the 0x0040xxxx range (track_world_initial_sweep) and include:
- 0x00426c00, 0x0040cea0..0x0040d440 (track_world_initial_sweep cluster)
- 0x0047b980 RegisterScriptCommand

These were explicitly out of scope per the mission (target = track-load chain, collision
geometry, 0x0047axxx range). Recommend next track C2 batch targets the track_world_initial_sweep cluster.

## Pool release

Slot Mashed_pool5 — lock file removed after session completion.
