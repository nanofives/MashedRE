# Session End — memory_pool_d2-20260506-1512

**Session:** QQQQQ  
**Date:** 2026-05-06  
**Pool slot:** Mashed_pool10  
**Bucket:** re/analysis/memory_pool_d2/  
**Parent:** memory_pool (batch 2, session M)  
**Subsystem:** util (memory_pool root) / boot (CRT callees)

## Work loop summary

cap_count: 1 (1 function analyzed)

| # | RVA | Name | Confidence | Notes |
|---|-----|------|------------|-------|
| 1 | 0x005c1ea0 | _longjmp | C1 | VS2003 library single match; depth-2 callee of FUN_00517250 via _longjmp chain |

## Depth tree outcome

```
0x005208c0 FUN_005208c0      (depth-0, already C1 in memory_pool bucket)
  ├── 0x004a45fb _malloc      (depth-1, C1 boot; callee __nh_malloc already D-0160 — not re-deferred)
  └── 0x00517250 FUN_00517250 (depth-1, C1 in memory_pool bucket)
       └── 0x005c1ea0 _longjmp (depth-2 — ANALYZED THIS SESSION → C1)
            ├── 0x004a3f90 __global_unwind2   (depth-3 → S-2360, D-7000)
            ├── 0x004a3fd2 __local_unwind2    (depth-3 → S-2361, D-7001)
            ├── 0x004a4066 FUN_004a4066       (depth-3 → S-2362, D-7002)
            └── 0x005c318c __rt_probe_read4@4 (depth-3 → S-2363, D-7003)
```

## ID ranges consumed

| Range | Assigned | Available remaining |
|-------|----------|---------------------|
| U=2367..2386 | 0 used | U-2367..U-2386 fully available |
| S=2360..2379 | S-2360..S-2363 (4 used) | S-2364..S-2379 available |
| D=7000..7059 | D-7000..D-7003 (4 used) | D-7004..D-7059 available |

## Trackers updated

- `hooks.csv`: +1 row (0x005c1ea0 `_longjmp`, boot, C1)
- `STUBS.md`: +4 rows S-2360..S-2363
- `DEFERRED.md`: +4 rows D-7000..D-7003 (depth-3 callees of _longjmp)
- `UNCERTAINTIES.md`: no new entries (library function, no uncertainties)

## Depth-3 deferred (D-7000..D-7003)

All 4 depth-3 callees of `_longjmp` are CRT/SEH helpers. Pick up as bucket `memory_pool_d2-cont1`.

| D | RVA | Description |
|---|-----|-------------|
| D-7000 | 0x004a3f90 | __global_unwind2 — SEH chain global unwind |
| D-7001 | 0x004a3fd2 | __local_unwind2 — SEH local scope unwind |
| D-7002 | 0x004a4066 | FUN_004a4066 — called with arg 0 before register restore |
| D-7003 | 0x005c318c | __rt_probe_read4@4 — readable-memory probe |

## Notes

- `__nh_malloc` (D-0160) was already deferred from the boot_crt_env session — not re-filed.
- `_longjmp` body is 119 bytes (0x005c1ea0..0x005c1f17). The `jmp_buf` struct is ≥ 40 bytes (10 int slots used).
- The VC20 magic `0x56433230` is the Visual C++ 2.0 SEH identifier stored by `setjmp` for structured-exception–aware `longjmp`.

## Scribe queue entry

To be drained by the scribe tool. RVAs: `0x005c1ea0`
