# MATCH-NEARMISS-2-B: slice B result, 2026-11-09

Branch `orch/MATCH-NEARMISS-2-B` from `orch/MATCH-NEARMISS-2` @ 55d7eb08a. 23 rows from
`verify/matchnearmiss2_20261109/slice_B.tsv` (95.0% to below 100%, miss types order / count / const).
Compiler and flags unchanged: XP DDK cl 13.00.9176 / c2 13.00.9178, each file's line-1 FLAGS. No `__asm`, no naked,
no byte-forcing pragmas. No mashedmod edits, no game launches, no Ghidra writes, no tracker edits.

**Port check (lead addendum):** none of the 23 sources is `#include`d by `mashedmod/`. The two grep hits
(`00498510.c` in `Match/PortTop_InputPort.cpp:125`, `004d7480.c` in `Render/D3d9StateCache.cpp:153/243`) are comments
only. Every row was worked in place. No PORT-INCLUDED rows.

## Verdict: 2 CLOSED, 21 NOT-CLOSED, 0 SIZE-ARTEFACT

| type | rows | CLOSED | rows closed |
|---|---|---|---|
| order | 17 | 1 | 0x0047c0f0 |
| count | 4 | 1 | 0x00550da0 |
| const (const+other+regalloc) | 2 | 0 | |

Per-row detail (before/after percent and bad bytes, what closed it or the residual, sha256 of the source after):
`results.tsv`. The `pct` column copies `after_pct` so `c4_reclass.py make-tsv` reads the file.

### Closed

- **0x00550da0** (`re/match/s24/00550da0.c`, count, 96.4% / 41 B -> BYTE-IDENTICAL 369 B, relocheck OK). File read
  (RtFS handler). The h4 source stored state 2 inside both position branches, and 9176 hoisted the common store above
  the compare. Written as one `*(int *)(param_1 + 0x20) = 2;` after the in-range / overflow if-else, 9176 sinks it
  into both arms as the original does. One compile.
- **0x0047c0f0** (`re/match/s13/0047c0f0.c`, order, 97.3% / 4 B -> BYTE-IDENTICAL 111 B, relocheck OK 10/0). The count
  at 0x006c2fe8 is typed as a member of the object at 0x006c1228: `struct { SectSet set; char pad[0x840]; int count; }`
  (0x006c1228 + 0x1dc0 = 0x006c2fe8). With that, the count store lands before the `rep movsd` as in the original.
  The pad content is `[UNCERTAIN]`: this function does not touch the 0x840 bytes, so the reconstruction cites only the
  offset. The h3 author had tried all 16 static/extern combinations of the four separate globals. The variant that
  closed keeps 0x006c2fec a file static (pilot finding 2). With 0x006c2fec extern it stays at 97.3%.

### Improved, not closed

- **0x00484de0** (order, 14 -> 8 bad bytes, 99.2%). The zero test reads `obj->ncells`, then `n = obj->ncells`. The
  spill of n now follows the `je` as in the original. `lea ecx,[edi+eax*4+0x24]` is still scheduled after the spill
  (original: before). 16 forms in total.

### Not closed (residual classes)

- **One int/x87 instruction one slot off** (14 rows: 0x004726f0, 0x004744a0, 0x00477e60, 0x00477f00, 0x004785e0,
  0x004809e0, 0x004818d0, 0x00482ae0, 0x00491340, 0x004c3040, 0x004d4a20, 0x004f8730, 0x00557110, 0x00556e90): an
  integer load, store, push or pop scheduled one slot from an x87 instruction or a neighbouring store. Each was in
  the 11-07 permute. Rewrites tried here: callee prototypes checked against the callees' own matched sources
  (0x004c39b0 `const RwV3d *in`, 0x0046d4a0 `unsigned int`, 0x004c3df0 pointer return, 0x004c2c90 `int *out`):
  none changed a byte. Typed containing structs (RwDevice member of RwGlobals for 0x004c3040, a pool struct with
  count/head for 0x004f8730, a typed V3 dir for 0x00491340) and expression forms (`-=`, `*=`, dot splits, temp
  structs for the Rt2d brush) also changed nothing.
- **0x00498510** (count): the pc_funcs size (653) is short of the contiguous body (665). At the contiguous size the
  insn score is 99.4% (`size_triage.py`), but the whole-symbol compare still has 11 bad bytes, all from
  `mov ecx,0x80` placed first in the restore struct copy (original: after `lea esi`/`mov edi,ebx`). So this is not a
  SIZE-ARTEFACT. 9 forms (pointer destination, goto, memcpy intrinsic in both copies, double instead of `__int64`
  stand-in) leave the slot unchanged.
- **0x004b61c0** (count, 398 B): 9176 unrolls the natural vertex loop x8 (70.2%). The original's loop is x4. The
  hand-unrolled x4 form reproduces the body but 9176 puts a 2-byte `mov edi,edi` alignment pad before its loop top,
  and that shifts every later byte. Pointer-walk loops (49-52%), a `for (j = 3; ...)` loop (77%), a break form
  (same pad) and `i < n - 3` (77%) do not help. Same compiler-behaviour finding as S01 0x00401690 and S16 0x00499d90.
- **0x00555f20** (count): `inc ecx; mov ebx,ecx` against the original's `lea ebx,[ecx+1]`, with the RwEngine load
  after the sub. 13 forms (spellings of n, RwGlobals typed as struct / char* / void*, memset) leave it unchanged.
- **0x004c9cd0** (const): `push esi` is shrink-wrapped to the light-loop block. The original saves esi at the top of
  the restore block. 10 forms (function-scope locals, goto, an `__inline` helper for the whole block or the light
  part) leave it unchanged.
- **0x004d7480** (const): case 0x10 / 0x11 load and store order in the RW D3D9 render-state switch. A typed
  `RenderStateCache[]` `{value, changed}` array at 0x007d57f8 compiles identically: it keeps 0x004d5480
  BYTE-IDENTICAL and leaves 0x004d7480 at 82 B. The macro compare form, `const` conversion tables and an int cache
  store in case 0x11 (92.4%) do not help. The file was reverted to its original text.
- **0x004d4f00** (order): moving the elems load after the param_3 load, which is the original's order, makes 9176 swap
  esi/edi (74.2%) in all 6 forms tried.

## Which rewrite kinds worked

1. **Common store after an if/else, not duplicated in both arms** (0x00550da0). The h4 source had duplicated it, and
   9176 then hoisted it above the compare.
2. **A global typed as a member of the object it sits after** (0x0047c0f0). This is the same family as the pilot's
   typed link structs: the store is addressed from the containing object's base. The same lever did not move
   0x004f8730 or 0x004d7480.
3. Source errors (callee arity / types, swapped parameter fields, string literals): every callee with a matched
   source was checked and none disagreed in a way that changed bytes.

Every row here except 0x00550da0 was in the 11-07 bounded permute (0 closed). Closure was 2 of 23 (9%), against 9 of
20 in the pilot. Most residuals are single-slot int/x87 scheduling, and nothing found here moves that class.

## Gate dry run

`c4_reclass.py make-tsv --results results.tsv --out c4_batch.tsv` then `batch --tsv c4_batch.tsv` (dry run, nothing
written). Output `c4_dryrun_out.txt`, verdicts `c4_dryrun.tsv`:

- 0x0047c0f0: **PASS** (byte-identical 111 B, relocheck OK 10/0).
- 0x00550da0: **REFUSED, no hooks.csv row**. The match criteria hold (BYTE-IDENTICAL 369 B, relocheck OK). The
  function has no `hooks.csv` row (`slice_B.tsv` lists it as "(no row)"). The lead must add a C1 row in the
  promotion transaction (the gate says "add it first: C1 by the identity") before it can pass. This worker does not
  edit trackers.

Gate dry run: 1 of 2 CLOSED rows pass. The other is blocked only by the missing tracker row.

## Files

- `re/match/s24/00550da0.c`, `re/match/s13/0047c0f0.c` (closed), `re/match/s14/00484de0.c` (improved). Each has a
  dated `MATCH-NEARMISS-2-B` note under its FLAGS line. Each file holds one function, and the whole-file re-run
  shows no other function affected.
- `results.tsv`, `c4_batch.tsv`, `c4_dryrun.tsv`, `c4_dryrun_out.txt`, this file.
