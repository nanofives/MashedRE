# SESSION_END — input_lua_d3

**SESSION_SHORT_ID:** input_lua_d3-20260507-1700  
**Slot used:** Mashed_pool4 (pool14 had persistent stale lock; pool4 substituted)  
**Session date:** 2026-05-07  
**Bucket:** re/analysis/input_lua_d3/

## Pre-flight notes
- pool14 lock cleared but Ghidra still raised LockException (no Java processes live); pivoted to pool4.
- D-7120..D-7124 were filed as fixup (commit 2d36790) because input_lua_d2 hadn't written them.

## Functions analyzed (5 of 5 DEFERRED targets)

| D-ID | RVA | Confidence | Pattern identified |
|---|---|---|---|
| D-7120 | 0x004ba1b0 | C1 | `lua_Alloc` callback: free/realloc/error |
| D-7121 | 0x004b7be0 | C1 | `luaD_rawrunprotected`: setjmp-based protected call |
| D-7122 | 0x004ba210 | C1 | lua_State init dispatcher: 6 sequential sub-inits |
| D-7123 | 0x004b9850 | C1 | Lua GC block teardown: memory accounting + free x2 |
| D-7124 | 0x004b64e0 | C1 | Inline `memset`: word+byte fill loops; leaf |

## Trackers updated
- **hooks.csv**: 5 new C1 rows (004ba1b0, 004b7be0, 004ba210, 004b9850, 004b64e0)
- **DEFERRED.md**: 9 new depth-4 rows filed (D-8680..D-8688), bucket `input_lua_d4`
- **DEFERRED.md**: D-7120..D-7124 now consumed by this session

## New stubs (depth-4, DEFERRED)
S-2920: 0x004b7b00 → D-8680  
S-2921: 0x004b7ba0 → D-8681  
S-2922: 0x004b7c70 → D-8682  
S-2923: 0x004ba3e0 → D-8683  
S-2924: 0x004ba470 → D-8684  
S-2925: 0x004ba310 → D-8685  
S-2926: 0x004ba2d0 → D-8686  
S-2927: 0x004ba250 → D-8687  
S-2928: 0x004ba290 → D-8688  

## New uncertainties (5)
U-2927..U-2931: error handlers, lua_State field offsets (0x6c, 0x10), 6 sub-init purposes

## Cap status
cap_count=0; all 5 DEFERRED targets consumed; session complete.
