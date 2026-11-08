# C4R-NEARMISS per-row notes (2026-11-07)

Compiler for every row: XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), each file's line-1 `// FLAGS:`.
None of the closed rows uses `__asm` or `naked` (grep count 0 in each source).

## 1. 0x00402750 (re/match/s01/00402750.c), CLOSED by a source edit

Ledger: 100% insn, 0 bad bytes, relocheck BAD on four string constants.
Cause: five literals were written with single backslashes, so `\t` became TAB and `\c`, `\p`, `\P`, `\T` dropped
the backslash. Original bytes (read from `original/MASHED.exe.unpatched`):

| va | original string |
|---|---|
| 0x005cc41c | `d:\toastart\common\font36.piz` |
| 0x005cc3b4 | `d:\toastart\common\powerups\powerups.piz` |
| 0x005cc390 | `d:\toastart\common\Panel\Panel.piz` |
| 0x005cc374 | `d:\ToastArt\common\perm.piz` |
| (pc.txd literal) | `toastart\pc\pc.txd` |

Edit: every backslash in those five literals doubled (lines 113, 140, 144, 160, 164). The pc.txd literal was not
flagged by relocheck because its constant compare reads only 8 bytes (`relocheck.py`, `csec[k:k + 8]`) and the
first 8 bytes (`toastart`) were equal. It was wrong as well (`toastartpcpc.txd`) and is fixed with the rest.
After: 100.0% (187/187), 739 B byte-identical, relocheck OK (checked 85, unchecked 8: plain `DAT_` globals
with no MGLOBAL address, the same class the PASS rows carry).

## 2. The ten 1-byte rows plus 0x00553e80: no source change, a match.py false positive

Every one of the eleven has exactly one "differing" byte, and it is the first byte of the rel32 of a `call`
whose original target is the function's own entry (a self-recursive call):

| rva | site | ours | original |
|---|---|---|---|
| 0x004c0ad0 | +0x1c | `e8 00000000` REL32 -> _FUN_004c0ad0 | `e8 dfffffff` -> +0x0 |
| 0x004d4bf0 | +0x1c2 | REL32 -> _FUN_004d4bf0 | `e8 39feffff` -> +0x0 |
| 0x004d8280 | +0x63 | REL32 -> _FUN_004d8280 | `e8 98ffffff` -> +0x0 |
| 0x004d83d0 | +0x42 | REL32 -> _FUN_004d83d0 | `e8 b9ffffff` -> +0x0 |
| 0x004da430 | +0x25 | REL32 -> _FUN_004da430 | `e8 d6ffffff` -> +0x0 |
| 0x004da480 | +0x2d | REL32 -> _FUN_004da480 | `e8 ceffffff` -> +0x0 |
| 0x004dab70 | +0x26 | REL32 -> _FUN_004dab70 | `e8 d5ffffff` -> +0x0 |
| 0x00553e80 | +0x56 | REL32 -> _FUN_00553e80 | -> +0x0 |
| 0x00554010 | +0x2b | REL32 -> _FUN_00554010 | `e8 d0ffffff` -> +0x0 |
| 0x00554150 | +0x41 | REL32 -> _FUN_00554150 | `e8 baffffff` -> +0x0 |
| 0x0055c4f0 | +0x29 | REL32 -> _FUN_0055c4f0 | `e8 d2ffffff` -> +0x0 |

Cause in the tool: `match.py` `function_from_obj()` builds `local` (relocations whose target is in the function's
own COMDAT section) without the relocation type, and `byte_compare()` checks each one as an absolute address
(`theirs = dword - addr`). That is right for the DIR32 jump-table entries it was written for. For a REL32 site the
original holds a displacement, so `theirs` is garbage and the site is always reported bad. With /Gy each function
has its own COMDAT, so the only REL32 site that lands in `local` is a call to the function itself.

Check: `selfcall_bytecheck.py` repeats `byte_compare()` with REL32 resolved as `site + 4 + rel32`. All eleven:
`typed_bad=0`, the self-call target is +0x0 on both sides. relocheck.py OK on all eleven (it skips same-section
labels by design). Instruction score was already 100.0% on all eleven.

0x00553e80 was not on the step list. It is the first MATCH in `re/match/s24/00554200.c` and the ledger refuses it
for the same single byte. The file's other row, 0x00554200, is relocheck BAD and is not touched here.

0x004c0ad0's line 1 is `/O2 /GS /Ob1 /G5`. Under the brief's RW-core set `/O2 /Ob1 /G5` it gives the same bytes
(same single self-call site, nothing else). Left as is.

Tool fix (not applied, outside this step's write scope): `match_py_rel32.patch`, 11 lines in `byte_compare()`.
Until it lands, the ledger keeps refusing these rows. C4R-MATCHBATCH-2 should apply it (or accept this
step's `selfcall_bytecheck.py` output) before promoting them.

## 3. 0x0040ac80 (re/match/s02/0040ac80.c): no source change, a Ghidra size artefact

The step brief read "12 of 22 instructions" as a missing part of the body. It is not. `re/console/pc_funcs.csv`
has `0x0040ac80,0x0040acb5,228`: `body_end` gives 0x36 contiguous bytes (entry .. ret at 0x0040acb4), and
`body_size` 228 is Ghidra's address set with the four tail-jumped case handlers folded in
(31 + 31 + 62 + 50 = 174 B at 0x0040abc0..0x0040ac72, plus the dispatcher). `match.py` reads `body_size`
contiguous bytes from the entry, so the original side runs past the `8b ff` pad and the 6-entry jump table at
0x0040acb8 into FUN_0040acd0. The 10 extra "instructions" are table bytes and the next function.

The source already holds the whole thing: the dispatcher plus the four handlers as `// HIDDEN:` statics.
`hidden_check.py` (match.py's own `score_function` / `byte_compare`, explicit size):

| va | size | insn | whole-symbol bytes |
|---|---|---|---|
| 0x0040ac80 | 0x36 (body_end - entry + 1) | 100.0% (12/12) | 80 B (code + pad + jump table), 0 bad |
| 0x0040abc0 | 31 | 100.0% (9/9) | 31 B, 0 bad |
| 0x0040abe0 | 31 | 100.0% (8/8) | 31 B, 0 bad |
| 0x0040ac00 | 62 | 100.0% (13/13) | 62 B, 0 bad |
| 0x0040ac40 | 50 | 100.0% (7/7) | 50 B, 0 bad |

relocheck OK on all five (HIDDEN rows read as annotations). The four handlers have no pc_funcs.csv row.
[UNCERTAIN] whether hooks.csv carries rows for them: not checked here (no tracker reads in scope).
`body_end - entry + 1 != body_size` holds for 225 pc_funcs rows; the PCFUNCS-REGEN step is where that belongs.

## 4. Bounded permute

See RESULT_C4R-NEARMISS.md and permute_summary.tsv.
