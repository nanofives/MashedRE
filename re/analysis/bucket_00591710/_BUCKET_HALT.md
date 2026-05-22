---
bucket: 0x00591710..0x0059b3e0
session: batch-z-s1
session_date: 2026-05-19
pool_slot: Mashed_pool6  # pre-assigned Mashed_pool1, but acquire returned pool6 (pool1 stale-locked; pool0+pool4 also stale-locked)
status: FULL-HALT
reason: 100% of bucket is statically-linked qhull-2002.1 library residue — fills the gap between batch-t-s5's qhull cluster (0x00583f10..0x005913c0) and batch-y-s6's qhull-print tail (0x005a5020..0x005a5820)
---

# Bucket 0x00591710 — full-HALT report (batch-z-s1)

## Decision

All 80 candidate RVAs in this session are statically-linked **qhull-2002.1**
(Brad Barber, Quickhull computational-geometry library; version anchor
`s_2002_1_2002_8_20_00625e04` documented by batch-t-s5). The bucket span
0x00591710..0x0059b3e0 sits between two previously HALT-tagged qhull regions:

- batch-t-s5's qhull cluster ends at 0x005913c0 (qh_partitioncoplanar)
- my bucket starts at 0x00591710 (qh_partitionpoint — see anchor #1 below)
- batch-y-s6's qhull-print tail starts at 0x005a5020 (after 4 RVAs of
  qhull-print residue and ~5 generic helpers, batch-y-s6's bucket pivots
  into audio code)

These three bands are **contiguous qhull**. The "post-qhull-segment" label
in the batch_z header was a calibration miss — qhull extends through at
least 0x0059b454. Per the STOP-AND-ASK library-residue clause, **no
per-RVA plates were written for any of the 80 candidates.**

This matches both predictions in earlier sessions:
- batch-v-s5: "qhull functions extend through at least 0x005a5820 per the
  FidDB-named qh_print* cluster"
- batch_z header: "ELEVATED RISK ... If first 20 plates show ANY repeating
  qhull-like strings (q_<>, qh_*, s_qhull_*) -> halt and report - qhull
  main band may extend forward past current calibrated boundary."

The "first 20" check tripped at function #1 — every sampled function shows
qhull-source-file string anchors (see § Evidence below).

## Library identification: qhull-2002.1

Same library as batch-t-s5's `bucket_00583f10` halt. Brad Barber's qhull
release 2002.1 (2002-08-20). The bucket span 0x00591710..0x0059b3e0 covers
the qhull source files immediately after `poly.c`/`merge.c`/`io.c`:
geometry primitives, statistics registration, voronoi diagrams, and the
final `user.c`/`global.c` epilogue.

### Sampled qh_* functions in this bucket

Identified by direct string-anchor evidence in the decomp (literal
`s_qh_<name>__...` constant pointers passed to the qh_fprintf sink
FUN_0057c5b0):

| Bucket RVA | qhull function (string-anchor evidence) | Anchor address |
|------------|-----------------------------------------|----------------|
| 0x00591710 | `qh_partitionpoint` | s_qh_partitionpoint__point_p_d_is_i_0062b8c8 / s_qh_partitionpoint__point_p_d_is_c_0062b868 / s_qh_partitionpoint__point_p_d_is_o_0062b918 + s_qh_precision__qhull_restart_beca_0062b4ac |
| 0x00591a80 | `qh_partitionvisible` | s_qh_partitionvisible__partitioned_0062b988 + s_qhull_precision_error__qh_partit_0062b9e8 |
| 0x00593540 | `qh_findgooddist` | s_qh_findgooddist__p_d_is__2_2g_ab_0062c6dc + s_qh_findgooddist__no_good_facet_f_0062c6a8 |
| 0x00595a80 | `qh_voronoi_center` | s_qh_voronoi_center__det__2_2g_fac_0062d20c + s_qhull_internal_error__qh_voronoi_0062d260 + s_p_d_dist___2g__0062d1f4 |
| 0x00596d40 | `qh_gausselim` | s_qh_gausselim__0_pivot_at_column___0062d738 + s_Matrix__0062d730 |
| 0x005981c0 | `qh_allstatistics` part 1 (precision-statistics block) | s_precision_statistics_0062dd08 + 12 `s_*_distance_of_*` / `s_*_facets_*` / `s_zero_divisors_*` strings ranging 0x0062da64..0x0062dd08 |
| 0x00599f60 | `qh_allstatistics` part 2 (renaming + memory-usage block) | s_renamed_vertex_statistics_0062f3b4 + 16 `s_renamed_*` / `s_duplicate_*` / `s_dropped_*` / `s_memory_usage_*` strings ranging 0x0062f010..0x0062f3b4 |
| 0x0059a740 | qhull facet/vertex statistics walker (no fprintf string of its own; walks DAT_009145ac facet list updating DAT_009134xx stat counters) | (caller-side) |
| 0x0059b3e0 | `qh_appendvertex` | s_qh_appendvertex__append_v_d_to_v_0062fa64 |

Each of these calls into:

- `FUN_0057c5b0` = qhull's `qh_fprintf` sink (identified by batch-v-s5; 220 callers in the qhull cluster).
- `FUN_005913c0` = `qh_partitioncoplanar` (batch-t-s5).
- `FUN_0057f940`, `FUN_0058f6a0` (qh_errexit), `FUN_0058ee50` (qh_memalloc), `FUN_005834a0` (qh_setsize), `FUN_005836a0`, `FUN_00583070`, `FUN_00582680`, `FUN_005827f0`, `FUN_00582550`, `FUN_00583560` — all qhull-cluster utilities already identified by batch-t-s5/batch-v-s5.
- `FUN_0059cd70` and `FUN_0059cde0` — qhull pointid + visit-mark helpers (outside my bucket; called from within it).

### qh_qh global struct accesses

Every sampled function in my bucket reads from the qh_qh global struct at
DAT_00913xxx..DAT_00914xxx (the same 2196-byte struct identified by
batch-t-s5 at qh_init_A). Specific globals touched include:

- `DAT_00914588` (= qh_qh.ferr file pointer, the destination for qh_fprintf)
- `DAT_009145a8`, `DAT_009145ac`, `DAT_009145c4`, `DAT_009145c8`, `DAT_009145cc`, `DAT_009145d0`, `DAT_009145d4`, `DAT_009145d8`, `DAT_009145e0`, `DAT_009145f0` (facet/vertex list heads, counters)
- `DAT_00913e78` (TRACElevel — every qh_fprintf is gated on `< n < DAT_00913e78`)
- `DAT_00913e80`, `DAT_00913e84`, `DAT_00913e90`, `DAT_00913e94`, `DAT_00913ecc`, `DAT_00913e50` (DELAUNAY / VORONOI / KEEPcoplanar / KEEPinside flags)
- `DAT_00913e2c`, `DAT_00914018`, `DAT_00914024`, `DAT_00914464`, `DAT_00914490`, `DAT_00914494`, `DAT_009146a0`, `DAT_00914604`, `DAT_00914610`, `DAT_00914614`, `DAT_00914618`, `DAT_00914648`, `DAT_00914668`, `DAT_00914670` (qh_qh option flags + temp counters)
- `DAT_009134xx` family (statistics — `qh_zinc`/`qh_zadd`/`qh_zmax` operands; the entire stats table is populated by 0x005981c0 + 0x00599f60)
- `DAT_0091360c` + `DAT_00913df0` (the stats string-pointer array at qh_qh.zsiz + the active-stat counter; populated by the qh_allstatistics calls)
- `DAT_005cc320`, `DAT_005cc32c`, `DAT_005d1008`, `DAT_005d757c` (qhull constants — REALmax, ZEROvolume, etc., in .rdata)

These globals are entirely consistent with the qhull qh_qh struct described
by batch-t-s5. No Mashed game globals (DAT_006xxxxx / DAT_007xxxxxxx) are
touched anywhere in the bucket.

### Bucket boundary (observed)

- **0x00591710 .. 0x0059b454** — all qhull. The qh_fprintf string-anchor
  density is uniformly high (every function with TRACE output references
  `s_qh_*` strings).
- **Adjacent on the low side** (0x005913c0 = qh_partitioncoplanar, last
  batch-t-s5 RVA) is qhull → contiguous.
- **Adjacent on the high side** my candidate list ends at 0x0059b3e0
  (qh_appendvertex, 117 bytes). The next bucket starts at 0x0059b454
  inside the next ~9KB before batch-y-s6's bucket at 0x005a5020.
  Spot-check at 0x0059cd70 (called from this bucket, outside it) shows
  the qhull family continues — `qh_pointid` is a known qhull function.
  The 0x0059b454..0x005a5020 gap is almost certainly still qhull
  (qh_pointid + qh_visit_id helpers + qh_print* prefix) but is not in my
  candidate list.

The correct merged qhull library range based on this session + batch-t-s5
+ batch-v-s5 + batch-y-s6 is approximately:

  **0x0057c5b0 .. 0x005a5820** — one contiguous qhull-2002.1 library island.

Within it, the previously plated game/RW peripheral code is only at the
0x0057bf30..0x0057c550 head (batch-v-s5's 8 plates) and the
0x005a5910..0x005a5f60 generic-helper block (batch-y-s6's middle 5 plates).
The remainder — including this entire bucket — is qhull source.

## Why brute-force C0->C1 plates would be wrong for this bucket

Same reasoning as batch-t-s4 / batch-v-s4 / batch-v-s5's library halts:

1. **Wasted effort** — qhull-2002.1 is upstream public-domain library code; identification is by string-anchor + source cross-reference, not by hand-plating decomp.
2. **No greenfield value** — `mashed_re.exe` either links qhull as a dep or excludes it entirely. The qhull usage path through Mashed is `[UNCERTAIN]` and is the same usage path already noted by batch-v-s5 (FUN_0057c670 triangulation/output formatter).
3. **Tracker pollution** — adding 80 C1 rows for "qhull library code" muddies the real game-code RVA inventory.
4. **Wrong methodology** — the canonical action for library residue is FidDB bulk-rename, not 80 hand-written plates.
5. **Duplicate with batch-t-s5 / batch-v-s5 / batch-y-s6** — those sessions plated parts of qhull and noted that the rest should be tagged as library-residue.

## Recommended action (for the orchestrator / sweep)

1. Extend the qhull-2002.1 `library_residue` tag from batch-v-s5's
   recommended range (0x0057c5b0..0x005913c0) to **0x0057c5b0..0x005a5820**
   — covering this bucket plus the small 0x0059b454..0x005a5020 gap which
   is also qhull per spot-check.
2. Optionally apply Ghidra's FidDb signatures to bulk-name qhull-2002.1
   across the binary — the per-function names in the table above are a
   working seed list:
   - 0x00591710 qh_partitionpoint
   - 0x00591a80 qh_partitionvisible
   - 0x00593540 qh_findgooddist
   - 0x00595a80 qh_voronoi_center
   - 0x00596d40 qh_gausselim
   - 0x005981c0 qh_allstatistics (part 1)
   - 0x00599f60 qh_allstatistics (part 2) — likely a single C function compiled into two contiguous code blocks, or qh_allstatistics + qh_allstat extension
   - 0x0059a740 qh_collectstatistics (or qh_printstatistics-internal walker)
   - 0x0059b3e0 qh_appendvertex
3. Add to `DEFERRED.md` with re-pickup condition: "if a qhull function is
   on a hot path or a hook target, plate it individually then; otherwise
   leave library-tagged." (Same condition as batch-t-s5.)
4. Do **not** re-issue this bucket range in a future batch_X sweep.

## What this session did produce

- This single `_BUCKET_HALT.md` (the bucket directory contains nothing else).
- One queue row in `re/SCRIBE_QUEUE.md` (Queued section) flagging the
  full halt.

## What this session did NOT produce

- No per-RVA plates for any of the 80 candidate RVAs (all 80 are library).
- No mutations to `hooks.csv` / `STUBS.md` / `UNCERTAINTIES.md` / `DEFERRED.md`.
- No git commits.

## Pool acquisition

- Pre-assignment: `Mashed_pool1`.
- `bash scripts/ghidra_pool.sh acquire` returned `Mashed_pool0` (pool1
  stale-locked). pool0 also had a busy `.lock~`. Released; reacquired
  Mashed_pool4 — also busy. Released; reacquired **Mashed_pool6** which
  opened successfully.
- Marker file `.pool_slot_batch_z_s1` records `Mashed_pool6`.
- Opened read-only via `mcp__ghidra__project_program_open_existing` on
  Mashed_pool6; `health_ping` OK.

## Queue-row note

The queue row appended to `re/SCRIBE_QUEUE.md` flags the FULL halt; the
sweep will find only `_BUCKET_HALT.md` in the bucket dir (no per-RVA
plates).
