# TrackNodeRecord — 0x48-byte track descriptor record

**Sourced from:** render_promote_c2_track_node-20260512 (Session 38)  
**Global pointer:** `DAT_0063d7e4` (0x0063d7e4) — stores pointer to the currently active record  
**Record array base:** `s_training_005f33f8` (0x005f33f8)  
**Record count:** `DAT_005f37a0` (0x005f37a0)  
**Record stride:** 0x48 (72 bytes)

## Layout

| Offset | Size | Type | Name (candidate) | Evidence |
|--------|------|------|------------------|----------|
| +0x00 | 4 | `char*` | `name` (track name string) | FUN_0041e980 return value used as string base by FUN_00426e10 caller; `"toastart/tracks/"` prefix appended at call site; [UNCERTAIN U-3213] |
| +0x04 | 4 | ? | unknown | no access observed |
| +0x08 | 4 | ? | unknown | no access observed |
| +0x0C | 4 | ? | unknown | no access observed |
| +0x10 | 4 | `int` | `id` | FUN_0041e870 @ 0x0041e870; FUN_0041e980 @ 0x0041e980; sub_0041e9b0 @ 0x0041e9ba; compared against `param_1` in all three |
| +0x14 | 4 | `void(*)(void)` | `fn_dispatch_a` (nullable fn-ptr) | FUN_0041e9d0 reads, FUN_0041e8b0 calls; non-null checked before dispatch; [UNCERTAIN U-3214] |
| +0x18 | 4 | `void(*)(void)` | `fn_dispatch_b` (nullable fn-ptr) | sub_0041e9e0 reads, sub_0041e8c0 calls; non-null checked before dispatch; [UNCERTAIN U-0554] |
| +0x1C | 4 | ? | unknown | no access observed |
| +0x20 | 4 | ? | unknown | no access observed |
| +0x24 | 4 | `void(*)(void)` | `fn_dispatch_c` | FUN_0041e8f0 calls; no null guard observed; also readable via FUN_0041ea10 (getter, C2); [UNCERTAIN U-3128] |
| +0x28 | 4 | ? | unknown | no access observed |
| +0x2C | 4 | ? | unknown | no access observed |
| +0x30 | 4 | ? | unknown | no access observed |
| +0x34 | 4 | ? | unknown | no access observed |
| +0x38 | 4 | ? | unknown | no access observed |
| +0x3C | 4 | ? | unknown | no access observed |
| +0x40 | 4 | ? | unknown | no access observed |
| +0x44 | 4 | `void(*)(void)` | `fn_dispatch_d` (nullable fn-ptr) | FUN_0041ea90 reads, FUN_0041e970 calls; non-null checked before dispatch; [UNCERTAIN U-3214] |
| +0x48 | — | — | (end, stride boundary) | — |

## Notes

- All confirmed fn-ptr fields (+0x14, +0x18, +0x24, +0x44) follow the same pattern: a companion getter reads the field for a non-null guard, then a dispatcher calls through it. The +0x24 field may not have a null guard (FUN_0041e8f0 calls directly).
- The `DAT_0063d7e4` global is written by FUN_0041e870 (scan-all, stores last match) and read/dispatched by all other functions in the cluster.
- Confirmed offsets leave 9 unknown 4-byte fields (+0x04, +0x08, +0x0C, +0x1C, +0x20, +0x28, +0x2C, +0x30, +0x34, +0x38, +0x3C, +0x40). Resolution: xref `s_training_005f33f8` writers (see U-3213).

## Open uncertainties

| ID | Field | Blocker |
|----|-------|---------|
| U-3213 | +0x00 (name inferred), all unknown fields | No write-side access observed yet |
| U-3214 | +0x14, +0x44 dispatch targets | Runtime-only; needs Frida trace or memory read |
| U-3128 | +0x24 dispatch target | Runtime-only; needs Frida hook at 0x0041e8f6 |
| U-0554 | +0x18 dual-role | Full struct layout depends on U-3213 resolution |
