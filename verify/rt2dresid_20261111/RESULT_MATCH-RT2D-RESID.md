# RESULT MATCH-RT2D-RESID (2026-11-11, second attempt 2026-11-12, third attempt 2026-11-12)

## Third attempt (roadmap run 40, 2026-11-12, Fable high on claude3, gate 1)

Verdict: GREEN, 2 of 3. 0x00554390 and 0x00555360 are byte-identical under rw9044 with relocheck OK. 0x00554200
stays at 99.3% with one store out of place. The source text of 0x00554200 is unchanged. Its header comment now
carries the findings below.

| RVA | run 39 (rw9044) | run 40 (rw9044) | bytes | relocheck | source |
|---|---|---|---|---|---|
| 0x00554390 | 92.5% | **100.0%** (439/439 ins) | BYTE-IDENTICAL, 1110 B | OK, checked=39 unchecked=0 | re/match/rwl05/00554390.c |
| 0x00555360 | 98.9% | **100.0%** (90/90 ins) | BYTE-IDENTICAL, 353 B | OK, checked=33 unchecked=0 | re/match/s24/00555360.c |
| 0x00554200 | 99.3% | 99.3% (141/141 ins, regs 99.3%, ops 100%) | not identical | not run | re/match/s24/00554200.c |

Commands: `py -3.12 re/tools/match/match.py <src> --bytes` and `py -3.12 re/tools/match/relocheck.py <src>`. Flags
`/O2 /Ob1 /G5`, C, toolchain rw9044 (VC6 SP5 cl 12.00.8804 + Processor Pack c2 13.00.9044, L-131) from the
`// TOOLCHAIN` line of each file. Neither matched body is naked or inline asm. `0x00553e80` in
re/match/s24/00554200.c stays 100% under rw9044 and under the DDK.

Promotions: `re/promotions/MATCH-RT2D-RESID.tsv` (2 rows, route match). Dry run
`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RT2D-RESID.tsv`: `2 pass, 0 refused, 0 skipped`,
both C2 -> C4. No `--apply`, `hooks.csv` and CHANGELOG untouched. The dry run used this branch's `c4_reclass.py`
(base bf44aae7d). The train applies with the tool at the integration head.

### 0x00554390 (Rt2d font stream read): 92.5% to 100%

One change. The outline loop indexes the glyph array with the same counter as the bitmap loops
(`&g[i].code`, `g[i].path = path`, `for (i = 0; i < font->count; i++)`) instead of walking a pointer (`g->code`,
`g++`) with a second counter. c2 strength-reduces `g[i]` to a pointer that it creates in the loop preheader, after
the guard: `mov ebp,eax` at 0x005545be follows the `jle` at 0x005545b8. Until then ebp is free and holds the
constant 0, which the original uses for `cmp [ebx],ebp` (0x00554595), the guard `cmp ecx,ebp` (0x005545b2) and
the counter store `mov [esp+0x10],ebp` (0x005545b4). The hoisted `i = 0` and the counter `j` of runs 38 and 39 are
removed. They were not needed: four spellings of the loop heads give the same code.

Kept from run 39: `char name[36]`. With `name[32]` the same source is 93.8% (frame 0x2c, every `[esp+N]` above
the name shifted by 4). [UNCERTAIN: what the original declares in the 4 bytes at [esp+0x3c]. No instruction of
0x00554390 references them. Evidence missing: the original source.] A dead store of 0 to an extra local (scalar,
one-element array, through a pointer, 9 forms) is removed by c2 together with its slot and does not give the
0x30 frame.

### 0x00555360 (Rt2d font-system init): 98.9% to 100%

One change besides the toolchain line. The index loop takes a pointer to the quad each iteration and stores
through it in index order:

```c
p = &DAT_00912700[i * 6];
p[0] = v; p[1] = v + 2; p[2] = v + 3; p[3] = v; p[4] = v + 3; p[5] = v + 1;
```

Why the direct form could not match: with `DAT_00912700[i * 6 + k] = ...` c2 9044 bases its pointer on the
address of the second store statement. All 720 statement orders were compiled and the zero-displacement store is
statement 2 in every one. The original's base is index 2 (`mov eax,0x912704` at 0x00555451, `mov [eax],cx` at
0x0055546b) and its store order is 0,3,1,2,4,5, which the direct form gives only when index 1 is statement 2
(base 0x00912702). The two exclude each other. 15 loop spellings were compiled in index order: the per-iteration
pointer with a `short v` is the only one at 100%.

Consequences outside re/match, for the lead:

- **The file now carries `// TOOLCHAIN rw9044`.** The DDK scores the new text 95.6%.
- **`re/match/ADMITTED.tsv` still has the 0x00555360 row** (DDK compile of commit 1ea4e247a,
  EQUIV-IF-CALLERS-IGNORE). It describes the old text. Not edited here: equiv.py was not re-run on the new
  text and the function leaves the admitted class once the promotion applies. Removing the row is a lead call.
- **`re/tools/match/test_equiv.py` used the live file as its L-117 reference** and expected
  EQUIV-IF-CALLERS-IGNORE. The old text is frozen at `re/tools/match/equiv_tests/refs/00555360_held.c`
  (byte copy of the blob at 05c15dd85) and the reference points there, the same handling as 0x005c4ad0.
  `py -3.12 re/tools/match/test_equiv.py`: 55/55 passed.
- **`mashedmod/src/mashed_re/Match/PortTop_00555360.cpp` on orch/integration includes this source** (NEARMATCH-555360,
  knob path only). That file is not on this branch and was not touched. The new text compiles as C++ with
  VS2022 cl 19.44 under the wrapper's five defines (`/c /W3 /O2 /TP`, exit 0, no warning, scratch TU, not
  committed). The loop computes the same 384 values, so the body's behaviour is unchanged. The wrapper's header
  still says "unedited 94.4% near-match". Rebinding it as a matched body, and the object-build check, is a
  top-down row.

### 0x00554200 (Rt2d path stream read): 99.3%, not matched

The residual is the one of run 39: the original stores obj+0x10 after the obj+0x18 and obj+8 stores
(`mov [esi+0x10],eax` at 0x005542a2, after 0x0055429c and 0x0055429f), ours right after the lea. Run 39 cited
0x005542a7 for that store. The listing gives 0x005542a2.

What c2 9044 does in this block (0x0055427d to 0x005542b4), measured on this file:

1. **hdr is aliased by every store through obj.** With `hdr.count` and `hdr.flags` read in place instead of
   through the two locals, each read is reloaded after the store before it (78.2%). No variant moved a hdr load
   across a store. So in the original's source the `hdr.extra` load (0x00554295) sits after the count and flagB
   stores and before the parent and flagA stores, and the extra store comes after them.
2. **A direct `obj->extra = hdr.extra` is always load, `lea ecx,[ecx+ecx*2]`, store**, adjacent, in all 120
   statement orders. That is why no order of the five statements reaches the original.
3. **A single-use temporary does not separate them.** With `extra = hdr.extra; ...; obj->extra = extra;` c2
   moves the store up to the load, past the parent and flagA stores, and the lea follows in eax (93.6%). Dead
   second uses of the temporary are removed before that (10 forms). A second use that stays
   (`hdr.extra = extra`) keeps load, parent, flagA, extra, at the price of one instruction (82.4%).
4. **New lever: a store through `obj->parent`.** Written right after `obj->parent = obj`, c2 forwards the
   pointer, so the store is still `[esi+0x10]`, but it stays behind the parent store.
   `extra = hdr.extra; obj->parent = obj; obj->flagA = flags & 1; obj->parent->extra = extra;` gives the
   original's order of all six memory operations: 94.3%, regs 99.3%, ops 99.3%. Left over in that form: the lea
   is `lea eax,[ecx+ecx*2]` after the extra store, not `lea ecx,[ecx+ecx*2]` at 0x00554299, and eax / ecx / edx
   are renamed from there to the call at 0x005542b4.

[UNCERTAIN: which source form makes c2 compute count * 3 between the hdr.extra load and the parent store while
the extra store stays last. Evidence missing: a variant that does it. Form 4 is the closest.]

Tried in run 40, none above 99.3%:

| Batch | Variants | Best |
|---|---|---|
| Generated bodies: store order x temporary and its position x one store through obj->parent x size from count / obj->count / hdr.count / obj->parent->count | 6,000 | 99.3% (6 variants, all the committed order) |
| Stores through `result` instead of `obj`, 5 orders x 32 masks x 2 parent values | 320 | 99.3% |
| Float `extra` in both structs x 120 orders | 120 | 99.3% |
| `/Oa` and `/Ow` x 120 orders (probe only, not the recorded flags) | 240 | 97.9% |
| Temporary for extra x 6 orders x 2 head orders x 2 size forms | 24 | 99.3% |
| `__inline` / `__forceinline` setters for parent, flagA, extra | 40 | 99.3% |
| Typed temporaries (float, cast, register, three loads first, struct copy, second use) | 8 | 93.6% |
| Dead second uses of the temporary | 10 | 93.6% |
| Store through obj->parent x temporary x flagA through parent | 12 | 98.6% |
| Size temporary at 5 positions in form 4, `obj->count * 0x18` in the call, flagA before parent | 26 | 94.3% |
| `size = obj->count * 0x18` pinned before the stores, 4 forms | 30 | 95.7% |
| Comma expressions (side effects are emitted first) | 9 | 98.6% |
| 4-byte memcpy and struct copy for extra x 12 orders | 24 | 99.3% |
| match_permute.py started from form 4 | 1,480 compiles (6 rounds, walk 150) | 99.3% (round 1 moves back to the committed order) |

Next step for whoever picks this up: start from form 4 and look for what places the multiply. The split in the
original (`lea` at 0x00554299, `shl ecx,3` at 0x005542b0) says the multiply is not pinned as a whole. A pinned
`size` temporary moves both parts up.

### Files (run 40)

- `re/match/rwl05/00554390.c`, `re/match/s24/00555360.c`: the two 100% sources.
- `re/match/s24/00554200.c`: comment only.
- `re/promotions/MATCH-RT2D-RESID.tsv`: 2 rows.
- `re/tools/match/test_equiv.py`, `re/tools/match/equiv_tests/refs/00555360_held.c`: the frozen L-117 reference.
- Variant generators and per-variant outputs stayed in the session scratchpad. They are not evidence for a
  status change. The tables above describe each batch in enough detail to rebuild it.

## Second attempt (roadmap run 39, 2026-11-12, Opus high on claude3)

Verdict: RED. 0 of 3 at 100%, nothing promoted (no `relocheck.py`, no `c4_reclass.py`, no tracker edit). Two sources
improved and are committed. 0x00555360 was not worked: it is admitted by rule (`re/match/ADMITTED.tsv`, equiv.py) and
needs no provisional row.

| RVA | run 38 (rw9044) | run 39 (rw9044) | source |
|---|---|---|---|
| 0x00554200 | 69.5% | **99.3%** (regs 99.3%, ops 100%, 141/141 ins) | re/match/s24/00554200.c |
| 0x00554390 | 90.2% | **92.5%** (regs 92.7%, ops 98.4%, 439/439 ins) | re/match/rwl05/00554390.c |
| 0x00555360 | 98.9% | 98.9% (unchanged, admitted) | re/match/s24/00555360.c |

`re/match/s24/00554200.c` now carries `// TOOLCHAIN rw9044`. `0x00553e80` in the same file stays 100% under rw9044
and under the DDK (`--toolchain ddk --only 0x00553e80`).

### 0x00554200: 69.5% to 99.3%

Each change is read off the original listing (`odis.py 0x00554200`):

1. **A second variable joined at the end of the else.** Header failure assigns `ret = 0` (the original jumps straight
   to the destroy test: `xor ebx,ebx; jmp 0x00554335` at 0x00554265), the else ends with `ret = result`
   (`mov ebx,ebp` at 0x0055432f), the tail is `if (ret == 0) Destroy(obj); return ret;`. With one variable c2
   threaded every failure past the result test and removed result. With the join, result stays in ebp, the case
   region has no zero register (`test reg,reg`, `push 0`), as in the original. 69.5 to 88.1.
2. **Failure polarity and one goto.** The 0x1a1 chunk test with the call in the then arm and the failure in the else
   arm, and the item-array read failure as `goto done` (label before `ret = result`). The chunk failure then
   tail-merges into the malloc failure block, as the original's `je 0x005542c3`. 88.1 to 98.6.
3. **`obj->buf = buf` in both arms of the flags & 4 test.** c2 hoists the common store above the branch, after the
   condition load, giving the original's `mov al,[esp+0x18]` before `mov [esi],edi` (0x005542f0). 98.6 to 99.3.

Residual: one store. The original stores obj->extra after the parent and flagA stores (`mov [esi+0x10],eax` at
0x005542a7), ours right after the `lea`. Tried, none above 99.3%: all 120 orders of the five field stores (before
and after change 3), extra through a temp (180 placements), a size temp
(210 placements), float and unsigned field types, hdr as an int array, `/Oa`, `/Ow`, `/G6`, flag test spellings,
match_permute.py from 88.1%, 98.6% and 99.3% (842, 1,323 and 1,325 compiles).

### 0x00554390: 90.2% to 92.5%

1. **Outline counter `j = i`, bitmap loop keeps i.** i is then in ebp for the first bitmap loop, the bitmap glyph
   pointer shares [esp+0x10] (0x005546bf) and oldTxd is at [esp+0x18] (0x0055469e), as in the original.
2. **`char name[36]`.** Gives the original's 0x30 frame with name at [esp+0x1c] (0x00554745) and the dword at
   [esp+0x3c] that the original never references. The read stays 0x20 bytes. With name[32] this shape has a 0x2c
   frame (86.3%).

Residual: the zero register. The original sets ebp = 0 at 0x0055458b and uses it for `cmp [ebx],ebp` (0x00554595),
both loop guards and the counter store (0x005545b4), then as the bitmap i. Ours keeps immediates. Probe: one extra
store of 0 in either branch or before it makes c2 9044 create the zero register (in edi), so c2 creates it at four
constant-0 uses in the region and the current source has three. Open: which source form gives the fourth use with no
code of its own. Tried, none above 92.5%: dead initialisers (oldTxd, g, font, version, path, a spare int, 63
variants), explicit `count > 0` guards (60), i = 0 at 7 points, two reaching definitions of i, i = 0 as the first
statement of both arms, name sizes 33 to 40, t1 / n / j scope and declaration order (240 + 168), match_permute.py
from 89.5% and 92.5% (1,059 and 1,353 compiles).

Variant files and permute logs stayed in the session scratchpad. They are not evidence for a status change.

## First attempt (run 38, 2026-11-11)

Row: byte-match the Rt2d residuals 0x00554390, 0x00554200 and 0x00555360 under toolchain rw9044 (VC6 SP5 cl
12.00.8804 + Processor Pack c2 13.00.9044, L-131 / L-133). Match-only row: no build, no gates. Branch
`orch/MATCH-RT2D-RESID` from `orch/integration` bf44aae7d. Flags `/O2 /Ob1 /G5` as C for all three.

## Verdict

RED: 0 of 3 reached 100%, so nothing is promoted (no `relocheck.py` or `c4_reclass.py` run, no tracker edit).
One source improved: 0x00554390 went from 86.1% to 90.2%, committed. The other two are unchanged.

| RVA | at start (rw9044) | now (rw9044) | source | stalled on |
|---|---|---|---|---|
| 0x00554390 | 86.1% (429/439 ins) | **90.2%** (regs 91.1%, ops 98.2%, 440/439) | re/match/rwl05/00554390.c (updated) | a zero held in ebp across the isOutline branch, and i in ebp for the first bitmap loop |
| 0x00555360 | 98.9% (90/90 ins) | 98.9% | re/match/s24/00555360.c (unchanged, DDK file) | one 16-bit store scheduled one slot late |
| 0x00554200 | 69.5% (139/141 ins) | 69.5% | re/match/s24/00554200.c (unchanged) | the original keeps `result` in ebp apart from the node, ours folds it away |

Scores are `py -3.12 re/tools/match/match.py <src>` (0x00554390, TOOLCHAIN line in the file) and the same with
`--toolchain rw9044` (0x00555360) or `--toolchain rw9044 --only 0x00554200` (0x00554200).

## 0x00554390 (Rt2d font stream read): 86.1% to 90.2%

Five changes, each read off the original listing (`py -3.12 re/tools/match/odis.py 0x00554390`):

1. The read after `FUN_00554200` uses the same temporary as the other outline reads. The original's
   `lea eax,[esp+0x48]` at 0x00554625 is the stream parameter home [esp+0x44] plus the argument still pushed for the
   0x00554200 call at 0x0055461e. The old source had a second temporary t2 in the frame.
2. Both branches end in one `return font`. The original copies `mov eax,ebx; pop edi; pop esi; pop ebp; pop ebx;
   add esp,0x30; ret` into the outline success, the outline fail block at 0x0055467e (`xor ebx,ebx; mov eax,ebx`)
   and the bitmap end. Each fail block sits right after its success code, reached by goto (`if (0) { fail: .. }`).
   This one change took the block layout to ops 98%.
3. The glyph list pointer is fetched in its own statement before memset, so the count is loaded after the call
   (`mov ecx,[ebx+0x130]` at 0x00554575).
4. `i = 0` before the isOutline test, shared by the outline loop and the first bitmap loop. This gives the
   original's 0x30 frame (every other shape tried gives 0x2c).
5. t1 (outline) and n (bitmap) are block-scoped, so both take [esp+0x44].

Residual: the original sets ebp to 0 at 0x0055458b (`xor ebp,ebp`) and uses it for the isOutline test
(`cmp [ebx],ebp` at 0x00554595), both count > 0 tests and the outline i = 0 store (`mov [esp+0x10],ebp` at
0x005545b4). i is in ebp for the first bitmap loop and in [esp+0x10] in the outline loop. Ours keeps i in
[esp+0x10] in both loops and uses immediate zeros. The same c2 does put 0 in a register elsewhere in this function
(the bitmap tail, `xor esi,esi` at 0x00554781, which ours matches), so the missing piece is in the source, not the
compiler. [UNCERTAIN: which source form makes c2 13.00.9044 keep 0 in ebp here. No variant below produced it.]

Tried, none above 90.2%:
- `i = 0` at 5 points (none, before the 0x005c4c60 call, before memset, after the list fetch, before the branch) x
  outline counter (i re-initialised, i shared, a separate j = 0, j = i) x first bitmap loop (re-initialised or not) x
  j scope: 51 variants, best 90.2% (shared i, the committed form), the rest 86.3% or lower.
- `register int i`: 90.2%. `int i = 0` in the declaration: 86.8%. `i = 0` after the list fetch: 86.1%.
- Both branches as `__inline` / `__forceinline` helpers, three call shapes, two fail forms: 12 variants, best 87.8%.
  This form does put i in ebp for the bitmap loop, but the zero register before the branch is still missing.
- Flags: `/Os`, `/O1`, `/Ox`, `/G6`, `/Oy-`, `/Op`, `/Ob2`, no `/Ob1`: all at or below 86.3% on the j-variant.
- `re/tools/match/match_permute.py` on the committed form: 1,345 compiles, 4 rounds, walk 200, no improvement.

## 0x00555360 (Rt2d font-system init): 98.9%, unchanged

rw9044 removes the DDK residual (1) of the file header (the merged `xor eax,eax; ret` returns): every check returns
through its own `jne; ret`, 90/90 instructions. One pair is left. In the index-table loop the original stores index 1
(`mov word ptr [eax-2],si`) before index 2 (`mov word ptr [eax],cx`). Ours stores it one instruction later.

Tried, none above 98.9%:
- All 720 orders of the six stores (`perm`, 720 compiles): 98.9% best, only the committed order reaches it.
- Eight orders x {short, int, unsigned short} v, a pointer walk `p[k]` with `p += 6`, v computed from i (`i * 4`),
  v as a second loop variable: 48 variants, best 98.9%.
- `*p++` stores, `for (v = 0; v < 0x100; v += 4)`, `p[k]` with a comma increment, short and unsigned short tables:
  24 variants, best 89.2%.

The file is left as it is. 0x00555360 is in `re/match/ADMITTED.tsv` (line 25) with this source compiled by the DDK
(EQUIV-IF-CALLERS-IGNORE). Adding a `// TOOLCHAIN rw9044` line would change what that admission compiled. Retagging
it is for the lead to decide together with the ADMITTED row.

## 0x00554200 (Rt2d path stream read): 69.5%, unchanged

rw9044 gives the same 69.5% as the DDK. The original keeps two pointers: the node in esi and `result` in ebp
(`mov ebp,esi` at 0x00554256). Each case failure is `xor ebp,ebp; jmp 0x0055432d` (0x005542c3), and at 0x0055432d
`xor edi,edi; mov ebx,ebp; cmp ebp,edi; jne 0x00554383`. Only the header failure is threaded past that test
(`xor ebx,ebx; jmp 0x00554335` at 0x00554265). Ours removes `result`: each success path tests the node directly and
each failure jumps to the destroy block. With ebp free, ours uses ebp as the zero register for the whole function.
The original uses edi only in the init and tail regions.

Tried, none above 69.5%: the type-1 case as an `__inline` helper returning the node or 0, three tail forms (the
original `if (result) return result`, `if (!result && obj)`, `if (!result)`): 6 variants, 68.8%. `result = obj`
before the read or inside the else branch, x switch / `do { if } while (0)` / `if` with goto, x the same three
tails: 18 variants, best 69.5% (the committed form). This adds to the 45 DDK variants of MATCH-RT2D-FONT-2.
`0x00553e80`, in the same file, stays at 100% under rw9044 as well as under the DDK.

## Files

- `re/match/rwl05/00554390.c`: the 90.2% source, with a header comment for the residual and the attempts.
- This file. Variant sources and per-variant listings stayed in the session scratchpad. They are not evidence for any
  status change, and the sweep scripts are described above in enough detail to rebuild them.

No tracker edits. `re/analysis/match_ledger.tsv` is a generated snapshot (still at the DDK scores) and was not edited.
