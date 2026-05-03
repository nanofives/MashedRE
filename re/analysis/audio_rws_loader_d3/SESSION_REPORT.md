---
session: audio_rws_loader_d3
session_id: audio_rws_loader_d3-20260503-KKKK
date: 2026-05-03
slot_requested: Mashed_pool6
slot_used: Mashed_pool12
reason_for_slot_change: Mashed_pool6 locked (14:35 ART today; batch-9 fanout). Pools 0–11 all locked. Fell back to Mashed_pool12 (first unlocked slot found).
anchor: MASHED.exe size=2846720 SHA-256=BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E OK
---

## Work loop summary

Processed all 18 DEFERRED rows from D-2860..D-2877 (depth-3 callees from audio_rws_loader_d2-cont1 session).
Discovered 13 additional depth-4 callees; 12 fully analyzed, 1 unresolved (FUN_005aa1e0).
No cap (cap_count=0). Full set + all resolvable depth-4 callees completed.

## Functions analyzed

### Original D-2860..D-2877

| D-ID | RVA | Short description |
|---|---|---|
| D-2860 | 0x005ade60 | circular-list forward index-search; node+8 == key; returns 0-based idx or -1 |
| D-2861 | 0x005aeca0 | endian-swap field packer; 1/2/4-byte bswap dispatch; advances output ptr |
| D-2862 | 0x005ae080 | sub-struct A pool-return; DAT_007dda28 pool; bit0 flag guard (NOT zero-init as d2 described) |
| D-2863 | 0x005ae050 | sub-struct B heap-free; FUN_004522d0; bit1 flag guard (NOT zero-init as d2 described) |
| D-2864 | 0x005aa060 | saves 16B from param_2; tree-search via FUN_005aa0c0 + LAB_005aa1e0 callback |
| D-2865 | 0x005aa0c0 | recursive tree-walk predicate search; DAT_009146fc root; child list at node+0x10 |
| D-2866 | 0x005aaac0 | circular-list walker; DAT_007dccf0 head; stop-on-zero callback |
| D-2867 | 0x005aba20 | wave_node full alloc+init; format resolution; pool or external node; 28B src+out fmt copy |
| D-2868 | 0x005ac7b0 | wave_node dual sub-struct init; FUN_005ae010 + FUN_005adfe0 sequential |
| D-2869 | 0x005ac900 | dispatcher stub; FUN_005aa0c0 + LAB_005ac930 callback; returns stack-written local_4 |
| D-2870 | 0x005ac980 | format-desc copy with optional zero-init; bit0/bit2 flag propagation |
| D-2871 | 0x005aca80 | chunk size calculator; base 0x1c/0x2c; plus ext-data size |
| D-2872 | 0x005acaa0 | bidirectional format-desc pack/unpack; raw or field-by-field bswap; ext-data support |
| D-2873 | 0x005acd10 | format-array linear search; 1 on any FUN_005ac9e0 match |
| D-2874 | 0x005ac5f0 | format-desc equality; FUN_005adf30 + 4 field checks; NOT wave-state validator |
| D-2875 | 0x005b3b30 | thin wrapper FUN_005b3a00 with arg reorder; returns output size |
| D-2876 | 0x005b3b60 | thin void wrapper FUN_005b3a00 |
| D-2877 | 0x005b3b80 | PCM format-conversion pipeline; FUN_005b3760 chain; vtable converters; temp-buf mgmt |

### Depth-4 callees (new, fully analyzed)

| RVA | Short description |
|---|---|
| 0x005acd60 | 9-entry format table scanner at PTR_DAT_00633674; FUN_005adf30 per entry |
| 0x005ac9e0 | format-entry match check; 5 conditions incl range + FUN_005adf30 |
| 0x005ae0b0 | 3-field zero-init leaf; 14 bytes |
| 0x005a7b00 | audio context insert; FUN_005a7a40 check then FUN_005addd0 |
| 0x005aea50 | vtable+0x114 alloc trampoline; jumptable not recovered |
| 0x005b3a00 | streaming size calculator; same chain as FUN_005b3b80 but no buffer I/O |
| 0x005ae650 | bitmap pool constructor; 6-param; DAT_007dda80 global list |
| 0x005ae800 | bitmap pool alloc; scan + bit-set + aligned addr |
| 0x005b36f0 | format-table lookup 51 entries PTR_PTR_00634550 |
| 0x005b3760 | conversion-chain builder + 5-entry LRU cache; DAT_007ddcac index |
| 0x005addd0 | doubly-linked list insert; DAT_009146c0 pool; counterpart to FUN_005ade10 |
| 0x005ae780 | bitmap pool destructor; drains blocks + frees header |

### Stub (unresolved)

| RVA | S-ID | Reason |
|---|---|---|
| 0x005aa1e0 | S-1720 | No Ghidra function body at LAB_005aa1e0; inline callback; D-5080 |

## Tracker changes

- **hooks.csv**: +31 rows (30 C1/MAPPED + 1 C0/STUB; subsystem=audio; file=re/analysis/audio_rws_loader_d3/*)
- **STUBS.md**: -18 rows (S-0980..S-0997 removed); +1 row (S-1720 FUN_005aa1e0)
- **DEFERRED.md**: -18 rows (D-2860..D-2877 resolved); +1 row (D-5080 FUN_005aa1e0)
- **UNCERTAINTIES.md**: +12 rows (U-1727..U-1738)

## Key structural findings

1. **D-2862/D-2863 description corrections**: d2 called FUN_005ae080/005ae050 "zero-init" but both are conditional free functions (pool-return and heap-free respectively). `param_1[2]` and `param_1+8` hold ownership-flag bytes, not just padding.

2. **Format-descriptor struct confirmed (28 bytes = 7 uint32s)**:
   - `+0x00`: field0 (sample-count or similar range value)
   - `+0x04`: format key ptr (16-byte; compared by FUN_005adf30)
   - `+0x08`: field2
   - `+0x0c byte`: channel descriptor
   - `+0x0d byte`: secondary channel byte
   - `+0x10`: ext-data ptr
   - `+0x14`: ext-data byte count
   - `+0x18 byte`: flags (bit0=fmt-A, bit1=ext-written, bit2=fmt-B)

3. **Audio format-conversion graph (PTR_PTR_00634550)**:
   - 51 registered single-step converters as `(src_fmt_entry, out_fmt_entry)` pairs
   - `FUN_005b3760` builds chains up to depth 4 with a 5-entry LRU cache (`DAT_007ddb30`)
   - `FUN_005b3760` + `FUN_005b36f0` + `FUN_005ac9e0` form the complete graph-search stack
   - This is a runtime PCM format-conversion pipeline (sample-rate, bit-depth, channel conversions)

4. **Pool allocator family confirmed**:
   - `FUN_005ae650`: constructor (creates pool with bitmap-tracked blocks)
   - `FUN_005ae800`: alloc from pool (bitmap scan + optional block alloc)
   - `FUN_005ae920`: free to pool (already C1 from d1)
   - `FUN_005ae780`: destructor (drain + free header)
   - All share the same block layout: next/prev ptrs + bitmap + aligned data

5. **Tree-walk infrastructure (FUN_005aa0c0)**:
   - Root at `DAT_009146fc`; child links at node+0x10 (= `node[4]`); `child_base = childptr - 6`
   - Callback signature: `int cb(root, node, key)` → 1 = match
   - `FUN_005aa060` and `FUN_005ac900` both use this for format/object lookup

6. **Two unresolved inline callbacks**:
   - `LAB_005aa1e0` (D-5080): tree-predicate for FUN_005aa060 (DSound context search?)
   - `LAB_005ac930` (U-1730): stack-writing callback for FUN_005ac900 (format lookup result)

## Depth-4 DEFERRED (D-5080)

1 function queued. Pickup: `audio_rws_loader_d3-cont1`.
Priority: D-5080 (005aa1e0) — requires `listing_disassemble_seed 0x005aa1e0` then `decomp_function`.
