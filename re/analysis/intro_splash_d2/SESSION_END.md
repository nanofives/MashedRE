# Session End — intro_splash_d2

**Session ID:** intro_splash_d2-20260506
**Pool slot:** Mashed_pool9
**Binary anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓
**Parent:** session O / intro_splash batch 2 (D-2321..D-2327)

## Work completed

Analyzed all 7 depth-2 deferred callees (D-2321..D-2327) from the parent intro_splash session. All 7 new analysis notes written.

### Functions analyzed (new — C1)

| RVA | Name in Ghidra | Notes file | Key finding |
|---|---|---|---|
| 0x004c75e0 | FUN_004c75e0 | 0x004c75e0.md | Reads two undefined2 values (shorts) from video-object+0x1c and +0x1e into two out-params; both initialised to 0 by allocator |
| 0x00494320 | FUN_00494320 | 0x00494320.md | Video close cleanup: vtable-slot-2 release on 8 globals (DAT_00771a14/20/24/28/2c/30/34/10); extra vtable-slot-9 call on DAT_00771a34 before release |
| 0x004c7650 | FUN_004c7650 | 0x004c7650.md | Video texture release: FUN_004d8060 list iterate + vtable slot 23 (DAT_007d3ff8+0x5c) + vtable slot 71 (DAT_007d3ff8+0x11c); returns 1 |
| 0x004942b0 | FUN_004942b0 | 0x004942b0.md | Video advance/tick: vtable slot 8 on DAT_00771a28 (5-arg); gated by piVar2==1 condition (U-2352); sets DAT_00771a04=0 (video done) when condition + DAT_00771a08==0 |
| 0x004938e0 | FUN_004938e0 | 0x004938e0.md | Linked-list traversal to self-referential terminal; writes param_2 at DAT_00911ae4+(int)terminal_node |
| 0x004c77c0 | FUN_004c77c0 | 0x004c77c0.md | Video texture allocator: vtable slot 70 alloc (type 0x30407), initialises 10 fields inc. self-pointer sentinel, vtable slot 22 init (arg 0x84), FUN_004d8000 list-insert on success |
| 0x004c7730 | FUN_004c7730 | 0x004c7730.md | FUN_004d8c40() pre-call + vtable slot 38 (DAT_007d3ff8+0x98) dispatch; gated return param_1-or-0 |

## Stubs resolved

| Stub | RVA | Resolved by |
|---|---|---|
| S-0814 | 0x004c75e0 | analyzed above |
| S-0815 | 0x00494320 | analyzed above |
| S-0816 | 0x004c7650 | analyzed above |
| S-0817 | 0x004942b0 | analyzed above |
| S-0818 | 0x004938e0 | analyzed above |
| S-0819 | 0x004c77c0 | analyzed above |
| S-0820 | 0x004c7730 | analyzed above |

## Stubs introduced (depth-3, deferred)

| ID | RVA | Size | Description | D-ID |
|---|---|---|---|---|
| S-2340 | 0x004d8000 | 82b | List insertion; called from FUN_004c77c0 with (&DAT_00618180, alloc_result) | D-6940 |
| S-2341 | 0x004d8c40 | 117b | No-arg pre-call before vtable dispatch in FUN_004c7730; purpose unknown | D-6941 |

Note: FUN_004d8060 (called from FUN_004c7650) is already C1/mapped (re/analysis/rw_engine_teardown_d2/0x004d8060.md) — no new stub.

## Uncertainties introduced

U-2347 through U-2361 (15 entries). Key open items:

| ID | Function | Summary |
|---|---|---|
| U-2347 | 0x004c75e0 | Video-object offsets +0x1c/+0x1e short semantic (viewport x/y? width/height?) |
| U-2348 | 0x00494320 | 5 new globals DAT_00771a20..34 types and init sites unknown |
| U-2349 | 0x00494320 | DAT_00771a34 vtable slot 9 (0x24) extra call before release — purpose unknown |
| U-2350 | 0x004c7650 | `*(DAT_007d40a8+0x60+DAT_007d3ff8)` address expression meaning |
| U-2351 | 0x004c7650 | Vtable slot 23 (0x5c) called (0, param_1, 0) in release path — purpose unknown |
| U-2352 | 0x004942b0 | `piVar2 == (int *)0x1` condition — likely decompiler misread; needs listing check |
| U-2353 | 0x004942b0 | DAT_00771a28 vtable slot 8 (0x20) 5-arg call; out-param semantics unknown |
| U-2354 | 0x004942b0 | DAT_00771a2c vtable slot 8 (0x20) 3-arg vs 5-arg pattern difference |
| U-2355 | 0x004938e0 | Self-referential terminal traversal depth at runtime unknown |
| U-2356 | 0x004938e0 | DAT_00911ae4: indexed storage base; type and size unknown |
| U-2357 | 0x004938e0 | Caller FUN_00494480 shows 1-arg call; function has 2 params; param_2 source unknown |
| U-2358 | 0x004c77c0 | Constant 0x30407 (allocation type code) meaning unknown |
| U-2359 | 0x004c77c0 | Vtable slot 22 (0x58) arg 0x84 (= param_4 from FUN_00494a80) meaning unknown |
| U-2360 | 0x004c7730 | FUN_004d8c40 purpose (no-arg pre-call before vtable dispatch) |
| U-2361 | 0x004c7730 | Vtable slot 38 (0x98) identity at DAT_007d3ff8 |

## Deferred cleared

D-2321, D-2322, D-2323, D-2324, D-2325, D-2326, D-2327 — all analyzed in this session.

## Depth-3 continuation

Queued as `intro_splash_d2-cont1` in re/queue.md.
- D-6940: 0x004d8000 FUN_004d8000 (list insertion, 82b)
- D-6941: 0x004d8c40 FUN_004d8c40 (no-arg pre-call, 117b)

## Tracker changes

- **hooks.csv**: +7 rows (004c75e0, 00494320, 004c7650, 004942b0, 004938e0, 004c77c0, 004c7730) all C1/frontend/mapped
- **STUBS.md**: S-0814..S-0820 → resolved; +S-2340 (FUN_004d8000), +S-2341 (FUN_004d8c40) passthrough
- **UNCERTAINTIES.md**: +U-2347..U-2361
- **DEFERRED.md**: D-2321..D-2327 → cleared; +D-6940 (FUN_004d8000), +D-6941 (FUN_004d8c40)
- **re/queue.md**: +intro_splash_d2-cont1 section
