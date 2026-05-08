# Session End — intro_splash_d3

This file covers two sessions that used this bucket.

---

## Session 1 — intro_splash_d3-20260506 (prior, pool0)

**Binary anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
**Parent:** intro_splash_d2-cont1 (D-6940..D-6941)

### Work completed

Analyzed D-6940 (0x004d8000) and D-6941 (0x004d8c40). Both C1. See per-RVA .md files.

| RVA | Key finding |
|---|---|
| 0x004d8000 | Find-in-list + secondary dispatch: walks main list via node+0x30 calling comparator at node+0x20; on match walks secondary chain via node+0x34 calling node+0x24; no static callees |
| 0x004d8c40 | Doubly-linked list splice + swap + counter clear: merges list at (DAT_00911ad8+DAT_007d3ff8+0x20) into +0x24, swaps pointers, clears +0x08=0; no static callees |

Tracker changes: +S-2340..S-2341 (resolved), +U-2527..U-2529, D-6940..D-6941 cleared; hooks.csv +2 rows.

---

## Session 2 — intro_splash_d3-20260508-1829 (this session, pool3)

**Binary anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
**Pool slot:** Mashed_pool3
**Parent:** intro_splash + intro_splash_d2 (~22 RVAs combined)

### Purpose

Tree-exhaustion check. Apply the common uncovered-callees rule against the full intro_splash + intro_splash_d2 callee graph, including depth-3 nodes.

### Methodology

1. Listed all callee RVAs from intro_splash (13 RVAs) and intro_splash_d2 (7 RVAs).
2. Cross-checked each against analysis buckets and function_inventory.csv.
3. Verified with Ghidra `function_callees` on all intro_splash_d2 functions and depth-3 nodes.

### Callee coverage map

| RVA | Covered by | Status |
|---|---|---|
| 0x004967e0 | re/analysis/input_dinput_d2/ | C1 ✓ |
| 0x00499710 | re/analysis/input_dinput_d2/ | C1 ✓ |
| 0x004671a0 | re/analysis/camera_follow/ | C1 ✓ |
| 0x004c75e0 | re/analysis/intro_splash_d2/ | C1, 0 callees ✓ |
| 0x00494320 | re/analysis/intro_splash_d2/ | C1, 0 static callees ✓ |
| 0x004c7650 | re/analysis/intro_splash_d2/ | C1, callee=0x004d8060 ✓ |
| 0x004942b0 | re/analysis/intro_splash_d2/ | C1, 0 static callees ✓ |
| 0x004938e0 | re/analysis/intro_splash_d2/ | C1, 0 static callees ✓ |
| 0x004c77c0 | re/analysis/intro_splash_d2/ | C1, callee=0x004d8000 ✓ |
| 0x004c7730 | re/analysis/intro_splash_d2/ | C1, callee=0x004d8c40 ✓ |
| 0x004d8060 | re/analysis/rw_engine_teardown_d2/ | C1 ✓ |
| 0x004d8000 | re/analysis/intro_splash_d3/ (session 1) | C1, 0 static callees — Ghidra confirmed ✓ |
| 0x004d8c40 | re/analysis/intro_splash_d3/ (session 1) | C1, 0 static callees — Ghidra confirmed ✓ |

Sleep and all other intro_splash-internal RVAs are external imports or already in the intro_splash bucket.

### Result

**INTRO_SPLASH CALLEE TREE EXHAUSTED.**

0 unmapped callees found across all reachable depth levels. The tree terminates at:
- Vtable/function-pointer leaves in 0x004c7650, 0x00494320, 0x004942b0, 0x004c77c0, 0x004c7730
- External leaves: Sleep (kernel32), 0x004671a0 (camera_follow subsystem)
- Depth-3 leaves with 0 callees: 0x004d8000, 0x004d8c40

Fewer than 5 unmapped callees identifiable — halt per prompt rule.

### Tracker changes

None. No new functions analyzed, no new stubs/uncertainties/deferrals. This session is read-only audit only.

### No SCRIBE_QUEUE entry

Nothing to scribe — no new plates, bookmarks, or renames produced.
