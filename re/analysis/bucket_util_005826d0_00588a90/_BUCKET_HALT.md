---
bucket: 0x005826d0..0x00588a90
session: batch-ag-s6
session_date: 2026-06-01
pool_slot: Mashed_pool14  # pre-assigned; opened read-only, no ghidra_pool.sh acquire; .pool_slot_ag_s6 marker written
mcp_session_id: b6c822177f5a4e0ab60dfd29ae0f992c
status: FULL-HALT
reason: all 60 candidate RVAs are statically-linked qhull-2002.1 computational-geometry library residue (vendored through RenderWare Physics 3.7); per project STOP-AND-ASK library-residue clause and established precedent (batch-t-s4 libpng/zlib, batch-v-s4/s5 qhull, batch-y-s6, batch-z-s1) library residue is NOT hand-plated — it is library-tagged + FidDB-bulk-named.
plated: 0   # zero per-RVA plates produced (correct for library residue)
---

# Bucket 0x005826d0..0x00588a90 — FULL-HALT report (batch-ag-s6)

## Decision

All **60** candidate RVAs of batch_ag Session 6 (0x005826d0 .. 0x00588a90) fall
entirely inside the statically-linked **qhull-2002.1** library island in
MASHED.exe. They are NOT game/engine code. Per the STOP-AND-ASK library-residue
clause and five prior halt precedents, **no per-RVA C2 plates were authored**;
this halt report + a HALT-row in `re/SCRIBE_QUEUE_ag_s6.md` are the deliverables.

The user was consulted before halting (the literal batch_ag instruction was
"author 60 C2 plates") and confirmed the halt-report path over hand-plating.

### Why this bucket is 100% library residue

batch_ag was generated 2026-06-01 from "util C1 pool, first 360 rows by address"
and excluded only **1 CRT-band row** — it did **not** filter out the qhull
cluster. Sessions 5 and 6 of batch_ag therefore drew their candidates straight
out of the known qhull island. This bucket splits cleanly into two halves, both
already characterised by earlier sessions:

| Sub-range | Count | Prior status |
|-----------|-------|--------------|
| 0x005826d0 .. 0x00583330 | 22 | the exact RVAs **batch-v-s5 already HALTED** on (see `re/analysis/bucket_0057bf30/_BUCKET_HALT.md`, qhmem/setT primitives, RVAs #9..#80 of that 80-RVA bucket) — never individually plated |
| 0x00583f10 .. 0x00588a90 | 38 | **already individually plated** by batch-t-s5 in `re/analysis/bucket_00583f10/` (qhull merge/poly/output half) |

Re-plating either half would (a) duplicate the existing batch-t-s5 plates for the
38, and (b) re-issue exactly the RVAs batch-v-s5's halt report told the
orchestrator "do NOT re-issue in a future batch_X sweep."

## Evidence the bucket is qhull-2002.1 (verified live in MCP session b6c822177f5a4e0ab60dfd29ae0f992c, Mashed_pool14, 2026-06-01)

All facts below were read this session from the open program — not carried over
from prior notes — per NO-GUESSING.

### Library provenance anchor (defined string)

- `0x005e5f58` (`/string`, len 68):
  `@@(#)$Id: //Physics/Rwp37Active/src/qhull/src/RwpQHullWrapper.c#1 $`
  — qhull is vendored into MASHED.exe via the **RenderWare Physics 3.7**
  (`Rwp37`) SDK (`RwpQHullWrapper.c`), confirming both the RenderWare-3.x engine
  identification and the third-party nature of this code.
- `0x0062406c` (`/string`, len 11): `qhull s Pp` — the default option string
  passed to qhull's `qh_new_qhull` entry point.
- 247 total defined strings match `qhull` (precision-error / output / warning
  format strings), e.g. `0x006240a8` "qhull precision error (qh_check_bestdist)…",
  `0x006241d8`, `0x00624238`, `0x006243e0`, `0x00624560`, `0x006245d0`,
  `0x00624654`, `0x00624790`, `0x00624830`.
- Library version anchor `s_2002_1_2002_8_20` at 0x00625e04 (qhull-2002.1,
  released 2002-08-20) documented by batch-t-s5; consistent with the above.

### Anchor function — first RVA of the bucket: FUN_005826d0 (qhull setT append + qhmem)

`void FUN_005826d0(int *param_1,int *param_2)` — appends one qhull `setT`
(`param_2`) onto another (`*param_1`), growing the destination via the qhmem
allocator. Operates on the setT layout (`set[0]` = maxsize slot, `set[maxsize+1]`
= element count). qhmem freelist residue cited at the open program:
- `DAT_007dc8f0` — qhmem LASTsize threshold (free path branch).
- `DAT_007dc8f8` — qhmem freelist heads array base.
- `DAT_007dc900` — qhmem size-class index table.
- `DAT_007dc92c` — qhmem freelist hit counter (`= DAT_007dc92c + 1`).
Calls `FUN_005830a0` (qh_setalloc) and `FUN_0058f010` (qh_memfree).
Sole caller `FUN_0059bda0` is inside the qhull cluster (>0x00583f10).

### Anchor function — FUN_00582ac0 (qh_setdellast)

`int FUN_00582ac0(int *param_1)` — pops the last element of a `setT`: reads the
maxsize slot `*param_1`, the count `param_1[*param_1+1]`, returns and clears the
last element, decrements the count. Pure setT primitive (qhull set.c).

### Anchor function — FUN_00585ee0 (qh_makeridges, poly2.c)

`void FUN_00585ee0(int param_1)` — makes ridges for a facet. Literal format-string
anchor `s_qh_makeridges__make_ridges_for_f_006276f8` passed to the qh_fprintf sink
`FUN_0057c5b0`. Reads qh_qh globals `DAT_00913e78` (IStracing level),
`DAT_00914588` (ferr stream), `DAT_0091401c` (hull_dim). Calls `FUN_005834a0`
(qh_setsize), `FUN_00582680` (qh_setappend), `FUN_0059cca0`, `FUN_00583150`.
Carries a `[C1 2026-05-18]` master comment from prior batch-t-s5 work.

### Anchor function — last RVA of the bucket: FUN_00588a90 (qh_rename_sharedvertex, merge.c)

`int FUN_00588a90(int param_1,int param_2)` — renames a shared vertex during
facet merge. Literal anchors:
- `s_qhull_internal_error__qh_rename__00628650`
- `s_qh_rename_sharedvertex__p_d__v_d_006286a0`
- `s_ERRONEOUS_00624554`
Reads qh_qh globals `DAT_00913e78`, `DAT_0091401c`, `DAT_00914588`,
`DAT_00914598`, `DAT_00914648` (visit_id), `DAT_00913440`. Calls the qh_fprintf
sink `FUN_0057c5b0`, the qh_errexit family `FUN_0058f6a0`/`FUN_0058f8e0`, and the
setT primitives `FUN_00582680`/`FUN_00582a60`/`FUN_00582420`/`FUN_005836a0`/
`FUN_00583560`/`FUN_00582f00`. Carries a `[C1 2026-05-18]` master comment.

Both ends of the bucket (0x005826d0, 0x00588a90), plus interior spot-checks
(0x00582ac0, 0x00585ee0) and the caller-set, are qhull. Combined with the
provenance string and 247 qhull format strings, the bucket is qhull-2002.1 in
full.

## The complete qhull island (per prior halts + this session)

Earlier halt/plate sessions bracket this bucket on both sides:
- 0x0057c5b0 .. 0x00583330 — batch-v-s5 `_BUCKET_HALT.md` (qhmem/setT; the true
  start of the qhull island is 0x0057c5b0, not 0x00583f10).
- 0x00583f10 .. 0x005913c0 — batch-t-s5 plated (qhull merge/poly/output).
- 0x00591710 .. 0x0059b454 — batch-z-s1 FULL-HALT.
- ~0x005a5020 .. 0x005a5820 — batch-y-s6 qhull-print residue tail.

Together: the qhull-2002.1 island spans ~**0x0057c5b0 .. 0x005a5820 (~165 KB)**.
This bucket (0x005826d0..0x00588a90) is interior to it.

## Why hand-plating qhull would be wrong (same reasoning as the prior halts)

1. **Wasted effort** — qhull is upstream public-domain library code
   (Brad Barber, qhull-2002.1), vendored via RenderWare Physics 3.7.
2. **No greenfield value** — `mashed_re.exe` can link qhull as a dependency or
   exclude it; Mashed only reaches it through the RW-Physics convex-hull path.
3. **Tracker pollution** — 60 hand-plated "qhull library" C1->C2 rows muddy the
   real game-code RVA inventory.
4. **Wrong methodology** — qhull-2002.1 source is public; identification is by
   string-anchor + source cross-reference and FidDB, not by hand-plating decomp.
5. **Duplicate work** — 38 of the 60 are already plated in `bucket_00583f10/`.

## Recommended action (for the orchestrator / sweep — NOT done by this worker)

1. Tag 0x005826d0..0x00588a90 in `hooks.csv` (or a `library_residue.csv`) with
   `status=library`, `library=qhull-2002.1`, `vendored_via=RenderWare-Physics-3.7`.
2. Prefer merging this with the adjacent qhull halts/plates into one contiguous
   `library=qhull-2002.1` tag spanning ~0x0057c5b0..0x005a5820.
3. Apply Ghidra FidDB signatures to bulk-name qhull-2002.1 across the binary; the
   batch-t-s5 plates already enumerate the `qh_*` names to backfill.
4. Add a `DEFERRED.md` row: "if a qhull function lands on a hot path or becomes a
   hook target, plate it individually then; otherwise leave library-tagged."
5. Do **not** re-issue the qhull half in a future batch_X sweep — and update the
   batch generator's util-pool filter to exclude the 0x0057c5b0..0x005a5820 island
   so batch_ah and later batches don't redraw it.

## What this session produced / did NOT produce

Produced:
- This `_BUCKET_HALT.md`.
- One HALT-row in `re/SCRIBE_QUEUE_ag_s6.md`.

Did NOT produce:
- No per-RVA plates (correct for library residue).
- No mutations to `hooks.csv` / `STUBS.md` / `UNCERTAINTIES.md` / `DEFERRED.md`
  (AUTHOR-ONLY worker; tracker writes belong to the central finalize / sweep).
- No renames, no git commits (files left untracked per batch_ag worker rules).

## Subsystem reclassification

`subsystem_observed = third-party-library [qhull-2002.1, vendored via
RenderWare-Physics-3.7]` for all 60 RVAs (hooks.csv currently labels them
`util`). Reclassification is the sweep's job; recorded here + in the queue row.
