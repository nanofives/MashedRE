# SESSION_END — input_lua_d4

**SESSION_SHORT_ID:** input_lua_d4-20260508-1445  
**Slot used:** Mashed_pool3  
**Session date:** 2026-05-08  
**Bucket:** re/analysis/input_lua_d4/

## Pre-flight notes
- SHA-256 BDCAE093... confirmed.
- Mashed_pool3 had no existing lock; lock written cleanly.
- All 9 D-8680..D-8688 rows present in DEFERRED.md.

## Functions analyzed (9 of 9 DEFERRED targets)

| D-ID | RVA | Confidence | Pattern identified |
|---|---|---|---|
| D-8680 | 0x004b7b00 | C1 | Error dispatcher: calls `_ERRORMESSAGE` handler then `luaD_throw(L, LUA_ERRRUN=1)` |
| D-8681 | 0x004b7ba0 | C1 | `luaD_throw`: longjmp via L+0x14 (errorJmp), or exit; LUA_ERRMEM=4 confirmed |
| D-8682 | 0x004b7c70 | C1 | Stack limit recalculate after unwind: adjusts `stack_last` when below capacity |
| D-8683 | 0x004ba3e0 | C1 | GC hash-bucket sweep: two-white pass, -0x15 header overhead, string/object table |
| D-8684 | 0x004ba470 | C1 | Sets GCthreshold = totalbytes×2; drains CI-slot finalizer lists via callgcTM |
| D-8685 | 0x004ba310 | C1 | `sweepstrings`: GC sweep of strt (L+0x2c/0x30/0x34), mark<2 two-white flip |
| D-8686 | 0x004ba2d0 | C1 | L+0x28 list: close-and-free Tables (self-ref trick at +0x14) via luaH_free |
| D-8687 | 0x004ba250 | C1 | L+0x20 list: two-pass sweep of Proto objects via luaF_freeproto |
| D-8688 | 0x004ba290 | C1 | L+0x24 list: close-and-free Closures (self-ref at +8) via luaF_freeclosure |

## Additional callees analyzed inline (6)

| RVA | Pattern | File |
|---|---|---|
| 0x004b7b30 | `_ERRORMESSAGE` dispatch (Lua 5.0 global error handler) | 0x004b7b30.md |
| 0x004ba3a0 | String table shrink check: nuse < sizestrt/4 → resize/2 | 0x004ba3a0.md |
| 0x004ba4f0 | `callgcTM`: look up GC tag method, push+call with depth guard | 0x004ba4f0.md |
| 0x004b9eb0 | `luaH_free`: frees Table (24 + n×40 bytes, Node=40) | 0x004b9eb0.md |
| 0x004bef20 | `luaF_freeproto`: frees 6 Proto sub-arrays + struct | 0x004bef20.md |
| 0x004befa0 | `luaF_freeclosure`: frees closure (nupvalues+1)×16 bytes | 0x004befa0.md |

## Key finding: Lua 5.0 confirmed
`_ERRORMESSAGE` global at 0x00617324 and tag method GC pattern in FUN_004ba4f0 are **Lua 5.0** artifacts. Lua 5.1 replaced `_ERRORMESSAGE` with `__tostring`/`pcall`. The embedded Lua runtime in MASHED.exe is Lua 5.0 (not 5.1).

## lua_State field deductions (new)

| Offset | Deduced field | Evidence |
|---|---|---|
| +0x14 | errorJmp (lua_longjmp*) | D-8681: longjmp pattern |
| +0x20 | GC list: Proto objects | D-8687 sweep |
| +0x24 | GC list: Closure objects | D-8688 sweep |
| +0x28 | GC list: Table objects | D-8686 sweep |
| +0x2c | strt.size (sizestrt) | D-8685 sweep |
| +0x30 | strt.nuse | D-8685: count-- on free |
| +0x34 | strt.hash (bucket array ptr) | D-8685 sweep |
| +0x38 | hash table size (FUN_004ba3e0) | D-8683 |
| +0x3c | live object count | D-8683 |
| +0x40 | hash array ptr (FUN_004ba3e0) | D-8683 |
| +0x48 | CI/tag method array ptr | D-8683, D-8684, D-8685 |
| +0x4c | CI/slot count | D-8684 |
| +0x5c | GCthreshold | D-8684 |
| +0x60 | totalbytes | all GC functions |
| +0x6c | GC call depth guard | D-8684 (callgcTM) |

## Trackers to update (scribe)
- **hooks.csv**: 15 new rows (9 DEFERRED + 6 callee inline), all C1, subsystem=input
- **DEFERRED.md**: D-8680..D-8688 consumed (cleared)
- **STUBS**: S-2920..S-2928 cleared (all 9 original stubs resolved)
- **STUBS new**: S-3260=FUN_004b9890, S-3261=FUN_004b9630, S-3262=FUN_004beef0
- **UNCERTAINTIES new**: U-3267 (L+0x28 identity), U-3268 (L+0x20 identity), U-3269 (L+0x24 identity), U-3270 (Lua version = 5.0 vs fork)
- **DEFERRED new**: none (cap_count=0; all targets consumed; no continuation needed)

## Cap status
cap_count=0; all 9 DEFERRED targets consumed; 6 callees analyzed inline; session complete. No input_lua_d4-cont1 needed.
