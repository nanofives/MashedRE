# Session End — intro_splash_d3

**Session ID:** intro_splash_d3-20260506
**Pool slot:** Mashed_pool0
**Binary anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
**Parent:** intro_splash_d2-cont1 (D-6940..D-6941)

## Pre-flight

- SHA256: matched anchor ✓
- Lock check: no batch-17 UUUUU lock; pool3 and pool6 locked by other sessions (unrelated) ✓
- Pool acquired: Mashed_pool0 (auto-selected; pool14 also free)
- Preflight asserts: slot-match ✓, staleness ✓, scribe-check ✓
- Open-program identity: confirmed Mashed_pool0 ✓
- health_ping: ok ✓, ghidra_info: Ghidra 12.0.3 ✓

## Work completed

Drained D-6940 and D-6941. Both analyzed to C1.

### Functions analyzed

| RVA | Name | Size | Key finding |
|---|---|---|---|
| 0x004d8000 | FUN_004d8000 | 82b | Find-in-list + secondary dispatch: walks main list (param_1+0x10) via node+0x30 calling comparator at node+0x20; on match walks secondary chain via node+0x34 calling node+0x24; returns 0 on match, param_1 on miss; 20 callers; part of trio with FUN_004d8060 (C1) and FUN_004d8090 |
| 0x004d8c40 | FUN_004d8c40 | 117b | Doubly-linked list splice + swap + counter clear: merges list at (DAT_00911ad8+DAT_007d3ff8+0x20) into list at +0x24, clears source to empty, swaps +0x20/+0x24 pointers, clears +0x08=0; called unconditionally before vtable slot 38 dispatch in FUN_004c7730; single caller |

### Note on D-6940 description

The intro_splash_d2 SESSION_END.md labelled FUN_004d8000 "list insertion function." The decompilation shows it is **not** an insertion; it is a conditional search-and-dispatch (find first match by comparator, then dispatch callbacks on the secondary chain). The companion insertion function is FUN_004d8090. This corrects the d2 description.

## Stubs resolved

| Stub | RVA | Resolution |
|---|---|---|
| S-2340 | 0x004d8000 | analyzed C1 this session (see also S-1930 from powerups context — same function) |
| S-2341 | 0x004d8c40 | analyzed C1 this session |

## Uncertainties resolved

| ID | Summary |
|---|---|
| U-2360 | FUN_004d8c40 purpose: now known — doubly-linked list splice + pointer swap + counter clear before vtable slot 38 dispatch |

## Uncertainties introduced

| ID | Type | Summary |
|---|---|---|
| U-2527 | structural | DAT_00911ad8: written at 0x004d8a8c (unbound address — no Ghidra function); role as index unknown |
| U-2528 | semantic | Struct at (DAT_00911ad8+DAT_007d3ff8+0x08) counter semantics: cleared to 0 each call; who increments it unknown |
| U-2529 | semantic | Two circular lists at +0x20/+0x24 alternate roles per call; relationship to vtable slot 38 context unknown |

## Depth-4 callees

Neither function has static callees (all dispatches through function pointers). No depth-4 stubs introduced. No entries in D=7480..7539.

## Cap check

- D range D=7480..7539: 0 new entries (cap not reached; no depth-4 callees).
- U range U=2527..2546: 3 entries used (U-2527, U-2528, U-2529).
- S range S=2520..2539: 0 new entries (S-2340 and S-2341 were pre-assigned from d2; carried forward and resolved).

## Tracker changes

- **hooks.csv**: +2 rows (004d8000 at C1/frontend/mapped, 004d8c40 at C1/frontend/mapped)
- **STUBS.md**: +S-2340 (resolved this session), +S-2341 (resolved this session)
- **UNCERTAINTIES.md**: +U-2527, +U-2528, +U-2529
- **DEFERRED.md**: +D-6940 (cleared), +D-6941 (cleared)
- **re/queue.md**: removed `intro_splash_d2-cont1` section
- **Analysis notes**: re/analysis/intro_splash_d3/0x004d8000.md, 0x004d8c40.md
