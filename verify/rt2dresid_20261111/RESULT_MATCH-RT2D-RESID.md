# RESULT MATCH-RT2D-RESID (2026-11-11)

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
