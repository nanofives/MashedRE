# SESSION_END — input_lua_d2_cont1

**SESSION_SHORT_ID:** input_lua_d2_cont1-20260512  
**Slot used:** Mashed_pool5 (pool0 had persistent stale lock; pool5 substituted)  
**Session date:** 2026-05-12  
**Bucket:** re/analysis/input_lua_d2_cont1/

## Pre-flight notes
- pool0 had stale lock (`.lock` file persisted across cleanup); pivoted to pool5.
- D-7121, D-7122, D-7128, D-7129, D-7130 already in hooks.csv from input_lua_d3 session — confirmed drift; DEFERRED rows marked RESOLVED as confirmations.
- D-7120 was crammed onto D-7060's line in DEFERRED.md (formatting bug); split to separate line and resolved.

## Functions analyzed (11 DEFERRED targets)

| D-ID | RVA | Confidence | Pattern identified |
|---|---|---|---|
| D-7120 | 0x004b7a70 | C1 | Lua exec wrapper: FUN_004b7aa0+FUN_004b7960(reset on result==0) |
| D-7121 | 0x004ba1b0 | C1 (drift-confirm) | lua_Alloc callback (input_lua_d3) |
| D-7122 | 0x004b7be0 | C1 (drift-confirm) | luaD_rawrunprotected (input_lua_d3) |
| D-7123 | 0x004c06c0 | C1 | 23-entry Lua table registration loop (stride-8 array) |
| D-7124 | 0x004b7200 | C1 | 5-byte thunk; target writes value+uint16 tag to stack slot |
| D-7125 | 0x004b9730 | C1 | lua_settagmethod: event lookup+type validate+settability+write |
| D-7126 | 0x004b7140 | C1 | push TObject type=2 with 8-byte double value |
| D-7127 | 0x004b7250 | C1 | pop TOS + set global: strlen+intern+table-set |
| D-7128 | 0x004ba210 | C1 (drift-confirm) | lua_State init dispatcher (input_lua_d3) |
| D-7129 | 0x004b9850 | C1 (drift-confirm) | Lua GC block teardown (input_lua_d3) |
| D-7130 | 0x004b64e0 | C1 (drift-confirm) | Inline memset (input_lua_d3) |

## Depth-1 fill analyzed (10)

| RVA | Pattern |
|---|---|
| 0x004b7aa0 | Lua chunk loader: default handler + FUN_004bc440(buf)+FUN_004b79c0(exec, binary?) |
| 0x004b7960 | Protected stack reset: compute target top + luaD_rawrunprotected |
| 0x004b7ff0 | TObject capture + push type=5; allocates via FUN_004bee20 |
| 0x004b9650 | Event name→index validator; 0..14 valid; 13+type4=GC deprecated |
| 0x004b9600 | Tag range validator; [0, L+0x4c] range check |
| 0x004b9540 | Settability table: (signed char)DAT_005d8880[type+event*15]; event>5→1 |
| 0x004b7df0 | Variadic Lua error formatter: vsprintf(280-byte buf)+FUN_004b7b00 |
| 0x004b7570 | Stack grower: stack_last+=0x280 (40 slots); luaD_throw if max exceeded |
| 0x004b9aa0 | strlen-bithack+FUN_004b9950(L str len) |
| 0x004b8340 | Global table setter: probe+tag-method dispatch or direct write or insert |

## Trackers updated
- **hooks.csv**: 16 new C1 rows (6 DEFERRED targets new + 10 depth-1 fill)
- **DEFERRED.md**: D-7120..D-7130 all marked RESOLVED (11 rows)
- **STUBS cleared**: S-2400, S-2403, S-2404, S-2405, S-2406, S-2407
- **STUBS new**: S-3705..S-3713 (9 stubs from D1 callee boundaries)
- **UNCERTAINTIES new**: U-3694..U-3698 (5 new)
- **SCRIBE_QUEUE.md**: queued row filed

## Key findings

### Lua state layout additions
| Offset | Field | Evidence |
|---|---|---|
| +0x44 (param_1[0x11]) | global table pointer | FUN_004b8340 |
| +0x48 (param_1[0x12]) | tag method array base | FUN_004b9730 (confirmed d4) |
| +0x4c | tag type count upper bound | FUN_004b9600 (possible conflict with d4 "+0x4c CI slot count") |
| +0x08 | stack_last | FUN_004b7570 (param_1[2]) |
| +0x04 | stack base | FUN_004b7570 (param_1[1]) |
| +0x0c | max_stack | FUN_004b7570 (param_1[3]) |

### Binary chunk detection
FUN_004b7aa0 checks `*param_2 == '\x1b'` (ESC byte) to distinguish pre-compiled Lua from text source — standard Lua binary chunk header marker.

### Settability table at DAT_005d8880
A 2D signed-byte table [event_count × 15] at 0x005d8880 controls which (event, type) tag-method combinations can be set. Row stride = 15 (0xF). Indices > 5 always return settable=1.

## Cap status
cap_count=0 (16 new plates, under 24-RVA hard cap); all 11 DEFERRED targets consumed; session complete. No input_lua_d2_cont1-cont1 needed.
