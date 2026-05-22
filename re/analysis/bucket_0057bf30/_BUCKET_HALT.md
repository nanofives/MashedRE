---
bucket: 0x0057bf30..0x00583330
session: batch-v-s5
session_date: 2026-05-18
pool_slot: Mashed_pool4  # acquired via ghidra_pool.sh; pre-assignment was Mashed_pool5 but acquire returned pool4
status: PARTIAL-HALT
reason: bucket is bimodal — top 8 RVAs are game/RW code (plated), remaining 72 RVAs are statically-linked qhull-2002.1 library residue (NOT plated)
---

# Bucket 0x0057bf30 — partial-HALT report (batch-v-s5)

## Decision

The 80-RVA candidate table for this session is **bimodal**:

- **Top of bucket (RVAs #1..#8, addresses 0x0057bf30..0x0057c550, 8 functions)**: RenderWare-style game code — a single object family using the RW memory-vtable at `DAT_007d3ff8` (slots `+0x108` alloc, `+0x10c` free), plus two standalone geometry primitives. **These were plated** as eight per-RVA C1 markdown files in this bucket directory.
- **Remainder of bucket (RVAs #9..#80, addresses 0x0057c5b0..0x00583330, 72 functions)**: statically-linked **qhull-2002.1** computational-geometry library — the upstream source half of the cluster that batch-t-s5 already plated as `bucket_00583f10/` and which the orchestrator's batch_v header explicitly flags as known library residue. **These were NOT plated**, per the STOP-AND-ASK library-residue clause in batch_v:

> "your bucket address span (0x0057bf30..0x0058xxxx) is **adjacent to** the already-tagged qhull-2002.1 library-residue cluster (0x00583f10..0x005913c0). If your candidates fall into that or any other library-residue cluster, **halt per STOP-AND-ASK library-residue clause**: emit one `_BUCKET_HALT.md` + a HALT-row to SCRIBE_QUEUE.md modeled after `batch-t-s4` / `batch-t-s5`. Do NOT plate library residue."

## Evidence the qhull half is library

### Anchor function: FUN_0057c5b0 = qh_fprintf

- Decompiles to a 3-way dispatch on `param_1` (FILE descriptor index): 0 → return 0; 1 → `_fwrite` to `&DAT_00616110`; 2 → `_fwrite` to `&DAT_00616130`; else → `FUN_004cbe80(param_1, buf, len)`.
- Calls `FUN_004a42c5` (cited at 0x0057c5d3) which is the internal `vsprintf`-shaped formatter; the `&stack0x0000000c` argument is the C va_args tail.
- **220 callers**, virtually all in 0x00583f10..0x005a5902 — i.e. the qhull cluster batch-t-s5 plated. Many callers have FidDB names like `qh_printcenter` (0x005a0e00), `qh_printend` (0x005a1300), `qh_printincidences` (0x005a1720), `qh_printextremes_2d` (0x005a1810), `qh_printextremes` (0x005a1990), `qh_printfacet2math` (0x005a2500), `qh_printfacetNvertex_nonsimplicial` (0x005a2a40), `qh_printfacetridges` (0x005a34e0), `qh_printhelp_singularity` (0x005a3ea0), `qh_printhelp_degenerate` (0x005a3f80), `qh_printpoints_out` (0x005a48d0), `qh_printridge` (0x005a4c60), `qh_printvdiagram` (0x005a4d40), `qh_printvertex` (0x005a4ed0).

The function is the qhull `qh_fprintf` printf-wrapper. 0x0057c650 is its `(buf, file)` adapter (one caller chain swaps the argument order before delegating to 0057c5b0).

### Anchor function: FUN_0057cb30 = qh_check_bestdist

Contains literal format-string anchors (cited as bare addresses):

- `s_qh_check_bestdist__check_points_b_006242f0`
- `s_qh_check_bestdist__check_that_al_006242a0`
- `s_qhull_output_completed__Verifyin_00624238`
- `s_qhull_precision_error__point_p_d_006241d8`
- `s__d_points_were_well_inside_the_h_00624130`
- `s_qhull_precision_error__qh_check__006240a8`
- `s_qh_check_bestdist__max_distance_o_00624078`

Reads qh_qh globals (`DAT_00913e78`, `DAT_00914588`, `DAT_0091459c`, `DAT_00913ef8`, `DAT_00913ebc`, `_DAT_00913e90`, `_DAT_00914464`, `_DAT_0091449c`) and calls FUN_005834a0 (qhull-internal `qh_setsize`), FUN_00581320 (qh-internal), FUN_00595f10 (`qh_distplane` per batch-t-s5), FUN_005964e0, FUN_00593540, FUN_00590900 (`qh_errexit2` per batch-t-s5), FUN_005836a0. The `qh_check_bestdist__` prefix in the format strings is the qhull source-file/function tag.

### Anchor function: FUN_0057ca30 = qh_new_qhull adapter

Contains `s_qhull_s_Pp_0062406c` (the `qhull s Pp` default options string used by qhull's `qh_new_qhull` entry point) and calls `FUN_0058f520` (which batch-t-s5 identified as `qh_new_qhull`), `FUN_00589dc0` (qh_freeqhull), `FUN_0058f0a0` (qh_memfreeshort).

### Anchor function: FUN_0057c670 = qhull triangulation/output formatter

Calls FUN_00585ee0 (which batch-t-s5 mapped to `qh_makeridges`), iterates `DAT_009145c4`/`DAT_0091459c` (qh_qh facet/vertex lists), and produces 12-byte triangle records — i.e. the qhull-to-game triangle exporter.

### qhmem allocator residue (RVAs #25..#80)

Functions in the range 0x0057ee00..0x00583330 reference:

- `DAT_007dc92c`, `DAT_007dc8f8`, `DAT_007dc900`, `DAT_007dc910`, `DAT_007dc940`, `DAT_007dc944` — qhmem freelist counters / freelist heads / size-class table (matches the qhmem layout in batch-t-s5's struct map).
- `DAT_009145a8`, `DAT_009145ac`, `DAT_009145c0`, `DAT_009145cc`, `DAT_009145d0`, `DAT_009145d4`, `DAT_00914618` — qh_qh globals.
- Calls `FUN_0058ee50` = qh_memalloc (per batch-t-s5), `FUN_0058f010` = qh_memfree (per batch-t-s5).

This is `qh_setlarger`, `qh_setdel*`, `qh_setunique*` and friends — the qhull setT primitives — co-located with the merge-core because qhull's source files are linked by compilation order, putting `poly.c` / `poly2.c` / `set.c` ahead of `merge.c` (which starts at 0x00583f10 in batch-t-s5).

### Caller-set match

Of my 80 candidate RVAs, **42** appear directly in the 220-element caller-list of `FUN_0057c5b0` (qh_fprintf): 0x0057cb30, 0x0057ce90, 0x0057d370, 0x0057d580, 0x0057d990, 0x0057de60, 0x0057e750, 0x0057e820, 0x0057ec70, 0x0057ed80, 0x0057ee00, 0x0057f0c0, 0x0057f200, 0x0057f4e0, 0x0057f750, 0x0057f860, 0x0057f940, 0x0057f970, 0x00580090, 0x005803a0, 0x005808a0, 0x00580ac0, 0x00580fc0, 0x005811e0, 0x00581290, 0x00581320, 0x00581540, 0x005815f0, 0x00581680, 0x005817f0, 0x00581a40, 0x00581ff0, 0x00582220, 0x005822f0, 0x005824a0, 0x00582550, 0x00582840, 0x00582930, 0x00582b10, 0x00582bf0, 0x00583150, 0x00583330.

The remaining ~30 candidates between 0x0057c670 and 0x00583070 either:

- Are qhmem set primitives (no need to print anything) — verified spot-checks at 0x00582ac0 (`qh_setdellast`), 0x00582ce0 (`qh_setdel`), 0x00582f70 (`qh_setlarger`), 0x005830a0 (qh_setalloc helper), 0x00581170 (otherfacet walker), 0x00581930 (qh_qh stats reset).
- Are called only from inside the qhull cluster.

Either way: every single RVA from 0x0057c5b0 onward is qhull-2002.1 source.

## Bucket boundary (observed)

- **0x0057bf30 .. ~0x0057c570** — game/RW-style code (the 8 plated RVAs; see § "Plated functions" below). Two unreferenced geometry primitives co-located at the top.
- **0x0057c5b0 .. 0x00583330** — qhull-2002.1 source residue (72 candidates skipped); continues into batch-t-s5's bucket 0x00583f10..0x005913c0 without a real boundary at 0x00583f10 — the 0x005833b0..0x00583f0b range (the 12 unmapped functions just past my candidate list end) is also qhull (e.g., 0x005834a0 has 56 callers from inside the qhull merge cluster, 0x005836a0 is called from qh_check_bestdist).

The address-space boundary "0x0057c5b0" should be treated as the **true** start of the qhull library cluster inside MASHED.exe, not 0x00583f10. The orchestrator's call-out that "bucket span is adjacent to the qhull cluster" was correct, but the boundary lies inside my bucket, not at its end.

## Plated functions (the 8 game-code RVAs)

Eight per-RVA C1 plates are present in this bucket directory. They share a clear shape:

| RVA | Plate | Role |
|-----|-------|------|
| 0x0057bf30 | `0x0057bf30.md` | unreferenced 3D line-segment vs triangle intersection; calls FUN_005667c0 + FUN_00566ea0 + FUN_00566d50 (RW vector primitives) |
| 0x0057c0d0 | `0x0057c0d0.md` | unreferenced same-side-of-edge test (signed cross-product comparison) |
| 0x0057c1b0 | `0x0057c1b0.md` | RW object init: calls FUN_0055c380 with `&DAT_005e5e50` |
| 0x0057c2b0 | `0x0057c2b0.md` | RW object destructor 1: calls FUN_0057c440 + dispatches `+0x30` chain through FUN_004c0e50 |
| 0x0057c370 | `0x0057c370.md` | RW object destructor 2: calls FUN_0055df90 + FUN_0057c550 + RW free-vtable @ DAT_007d3ff8+0x10c |
| 0x0057c420 | `0x0057c420.md` | RW object accessor: dereferences `[+0x10]` then `[+0x10][0]`/`[+0x10][2]` and calls FUN_0055bd80 |
| 0x0057c440 | `0x0057c440.md` | RW object copy: copies 0x40 bytes (16 dwords) from a vertex slot into `[+0x10][+0x8]`, calls FUN_004c51a0 + FUN_004c45f0 (look like vector ops) |
| 0x0057c550 | `0x0057c550.md` | RW object refcount-dec: decrements `[+0x50]` and frees when zero (calls FUN_0055b930 + FUN_00564190 + free-vtable @ DAT_007d3ff8+0x10c) |

All eight share the RenderWare memory-vtable pattern (`DAT_007d3ff8`, slots `+0x108` alloc and `+0x10c` free). FUN_0057c500 (size=0x54 constructor) is **not** in the candidate list but is the missing peer of 0057c2b0/0057c370 (see callees on plates).

The class is **not** identifiable from strings or RTTI in the open program — no `Rw*` / `Rp*` / `Rt*` string anchors, no FidDB tags, no const-data vtable label other than the bare `&DAT_0062403c` byte. The shape (0x54-byte struct, 0x4c-offset child object with its own 0x50-offset refcount, ref-tracked via `[+0x50]`) is *consistent with* an RW core-object subclass but cannot be named without further xref work — flagged `[UNCERTAIN]` in the plates.

## Why brute-force C0->C1 plates would be wrong for the qhull half

Same reasoning as batch-t-s4's halt of the libpng/zlib bucket:

1. **Wasted effort** — qhull is upstream public-domain library code (Brad Barber, qhull-2002.1, 2002-08-20 per the version anchor `s_2002_1_2002_8_20` at 0x00625e04 documented in batch-t-s5).
2. **No greenfield value** — `mashed_re.exe` (greenfield target) can either link qhull as a dep or just exclude it (Mashed only seems to use qhull for the geometry export path through FUN_0057c670; whether that path is actually hit at runtime is `[UNCERTAIN]`).
3. **Tracker pollution** — adding 72 C1 rows for "qhull library code" muddies the real game-code RVA inventory.
4. **Wrong methodology** — qhull-2002.1 source is publicly available; identification is by string-anchor + source cross-reference, not by hand-plating decomp.
5. **Duplicate with batch-t-s5** — that session already plated the merge/init/qhmem/output halves of qhull (0x00583f10..0x005913c0); plating my half would duplicate the same library's plates across two bucket directories.

## Recommended action (for the orchestrator / sweep)

1. Tag the range **0x0057c5b0..0x00583330** in `hooks.csv` (or `library_residue.csv`) with `status=library`, `library=qhull-2002.1`.
2. Optionally merge with batch-t-s5's adjacent range 0x00583f10..0x005913c0 into a single contiguous qhull tag spanning 0x0057c5b0..0x005913c0 (and likely beyond — qhull functions extend through at least 0x005a5820 per the FidDB-named `qh_print*` cluster).
3. Add to `DEFERRED.md` with re-pickup condition: "if a qhull function is on a hot path or a hook target, plate it individually then; otherwise leave library-tagged."
4. Apply Ghidra's FidDb signatures to bulk-name qhull-2002.1 across the binary. The batch-t-s5 plates already enumerate the qh_* names that need backfilling here.
5. Do **not** re-issue the qhull half in a future batch_X sweep.

## What this session did produce

- 8 per-RVA C1 plates for the game-code top of the bucket (0x0057bf30..0x0057c550).
- This `_BUCKET_HALT.md` for the qhull tail (0x0057c5b0..0x00583330).
- One queue row in `re/SCRIBE_QUEUE.md` (Queued section) flagging the partial halt.

## What this session did NOT produce

- No per-RVA plates for the 72 qhull-residue RVAs (#9..#80 of the candidate table).
- No mutations to `hooks.csv` / `STUBS.md` / `UNCERTAINTIES.md` / `DEFERRED.md` (those belong to the sweep, per parallel-safety rules).
- No git commits.

## Pool acquisition

- Pre-assignment: `Mashed_pool5`.
- `bash scripts/ghidra_pool.sh acquire` returned `Mashed_pool4` (pool5 unavailable / held by sibling session).
- Marker file written to `.pool_slot_batch_v_s5` records the actual slot used.
- Opened read-only via `mcp__ghidra__project_program_open_existing` on `Mashed_pool4`, `health_ping` OK.

## Queue-row note

The queue row appended to `re/SCRIBE_QUEUE.md` flags the partial-halt; the sweep will read both this `_BUCKET_HALT.md` and the 8 game-code plates rather than expecting 80 plate files.
