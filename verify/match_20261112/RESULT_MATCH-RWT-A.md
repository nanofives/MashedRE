# RESULT MATCH-RWT-A: doubled RW match lane, executed RW tail (2026-11-12)

Row kind: match-only (no build, no gates, no game run). Session on claude3, Opus 5.5.

## Ranked list (for MATCH-RWT-B)

Full list: `verify/match_20261112/RANKED_MATCH-RWT.tsv` (163 rows), generator `rank_match_rwt.py`
(inputs: `log/orch/RW_CLOSURE_REVIEW_20261112.md` row C and its evidence folder `log/orch/rw_closure_review_20261112/`,
`RwCore_Bodies.tsv` from `orch/RWC-PLUGIN-ATTACH` @ e5b357042, `verify/equivcheck_20261112/measure_final.tsv`,
`re/coverage/reach.tsv`).

Set: the executed RW functions of row C, minus the 12 hand bodies and 0x004c7a70 (never measured):
114 provisional NOT-PROVEN (`measure_final.tsv`), 42 g2s NOT-PROVEN (`equiv_g2s_executed_rw_64.txt` minus 0x004c3d90,
which the review puts in class A by its 1-minute fix), 7 g2s compile failures. Every row is executed (reach.tsv).

Order: race path first (reached_race_load or reached_race_run = 1), then best_pct descending.
Columns: rank, rva, size, executed, race_path, stage, best_pct, class, source (`g2s:<dir>` = regenerate with
`decomp_pc.py --port --callees` + `g2s.py`, the review kept only the strict sources), band, equiv kind.

Bands: A95 (>= 95%), A80 (80 to 95%), B50 (50 to 80%), B<50, B-cf (compile failure or no size).

**Stale inputs, filtered (second commit).** The review's composition predates later merges: rescoring the 22
provisional band-A sources with `match.py` showed 10 already 100% under their file's `// TOOLCHAIN`, and hooks.csv on
`orch/integration` a616fcc3b has **20 of the 163 rows at C4 already** (12 in band A). Two more (0x004d1e30, 0x00534b60)
are closed at 100% on `orch/MATCH-RWC-NEAR-C`, and 0x004f1fb0 is being shaped on `orch/MATCH-RWC-NEAR-D`. Those 23 rows
carry `status = skip: ...` and rank `-` at the end of the TSV. The 140 `open` rows are ranked 1..140.
Columns added: `hooks_conf` (hooks.csv on orch/integration a616fcc3b), `status`.

| band | open, race path | open, menu only | open total | skipped |
|---|---|---|---|---|
| A95 | 0 | 1 | 1 | 3 |
| A80 | 10 | 2 | 12 | 12 |
| B50 | 34 | 18 | 52 | 6 |
| B<50 | 43 | 24 | 67 | 1 |
| B-cf | 7 | 1 | 8 | 1 |
| total | 94 | 46 | 140 | 23 |

MATCH-RWT-A took the 13 open A95 + A80 rows (ranks 1-10 and 95-97). For MATCH-RWT-B: open B50 race path is ranks
11-44, B<50 race path 45-87, B-cf race path 88-94, then the menu-only rows (B50 98-115, B<50 116-139, B-cf 140).
0x004f1fb0 (B-cf) is on `orch/MATCH-RWC-NEAR-D`. The `status` column now also marks the 9 rows closed here and the 4
parked here.

## Verdict

GREEN. **9 of 13 open band-A rows byte-identical**, relocheck OK, plain C (no inline asm in any body). 4 parked.
Promotion file `re/promotions/MATCH-RWT-A.tsv` (route match, 9 rows), not applied on the branch.

`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-A.tsv` (dry run):
```
[1/9] 0x004c2b80 match PASS byte-identical 47 B, relocheck OK (2/0)
[2/9] 0x004c2bb0 match PASS byte-identical 57 B, relocheck OK (4/0)
[3/9] 0x0052e3d0 match PASS byte-identical 185 B, relocheck OK (15/0)
[4/9] 0x005386b0 match PASS byte-identical 195 B, relocheck OK (13/2)
[5/9] 0x00549710 match PASS byte-identical 216 B, relocheck OK (17/0)
[6/9] 0x00557d90 match PASS byte-identical 181 B, relocheck OK (4/0)
[7/9] 0x004dba60 match PASS byte-identical 556 B, relocheck OK (17/0)
[8/9] 0x004e1b60 match PASS byte-identical 299 B, relocheck OK (5/0)
[9/9] 0x004c0910 match PASS byte-identical 334 B, relocheck OK (11/0)
batch: 9 pass, 0 refused, 0 skipped (already C4), 9 rows  [by route: match=9]
```

Scored with `re/tools/match/match.py --bytes` under each source's `// TOOLCHAIN` (rw9044 for all nine; every one is
also checked under ddk, see below) and `// FLAGS: /O2 /Ob1 /G5`, then `re/tools/match/relocheck.py`. Ghidra: pool slot
`Mashed_pool0`, read-only, through `re/tools/decomp_pc.py --port --callees` (run from the main checkout: the pool lives
there, a worktree run fails with "Could not find project"). No build, no game, no tracker edit.

## Per RVA

| rank | RVA | size | stage | before | after | source | what closed it |
|---|---|---|---|---|---|---|---|
| 4 | 0x004c2bb0 | 56 | race_run | 87.0 (g2s) | **100** | `re/match/rwta/004c2bb0.c` | RW standard-function pointer through a named local `fn` |
| 5 | 0x004e1b60 | 299 | race_load | 86.5 | **100** | `re/match/rwl01/004e1b60.c` | rewritten, see below |
| 6 | 0x004c2b80 | 46 | race_run | 84.2 (g2s) | **100** | `re/match/rwta/004c2b80.c` | named local `fn`, as 0x004c2bb0 |
| 3 | 0x004dba60 | 534 | race_run | 87.6 | **100** | `re/match/s21/004dba60.c` | `// TOOLCHAIN rw9044` + `ox` converted before `oy` |
| 8 | 0x004c0910 | 334 | race_load | 82.0 | **100** | `re/match/rwl01/004c0910.c` | rw9044 + rotated child loop + RW link-list unlink |
| 9 | 0x005386b0 | 194 | race_load | 81.7 (g2s) | **100** | `re/match/rwta/005386b0.c` | split floats are one `float v[3]`, `_DAT_00912498` is float |
| 10 | 0x00557d90 | 180 | race_load | 80.7 (g2s) | **100** | `re/match/rwta/00557d90.c` | `FUN_00557a60` declared as a function |
| 96 | 0x0052e3d0 | 184 | menu | 87.3 (g2s) | **100** | `re/match/rwta/0052e3d0.c` | rep-stos loop is intrinsic `memset(p, 0, 0x400f)` |
| 97 | 0x00549710 | 215 | menu | 82.1 (g2s) | **100** | `re/match/rwta/00549710.c` | four split locals are one `undefined4 a[4]` |
| 95 | 0x004db3e0 | 368 | menu | 95.2 | 95.2, parked | `re/match/rwl01/004db3e0.c` | see Remainder |
| 7 | 0x004c6750 | 1092 | race_load | 83.3 ddk / 93.5 rw9044 | 93.5, parked | `re/match/s19/004c6750.c` | see Remainder |
| 1 | 0x004cb4c0 | 320 | race_run | 90.2 | 90.2, parked | `re/match/s20/004cb4c0.c` | see Remainder |
| 2 | 0x004d40d0 | 159 | race_run | 89.1 | 89.1, parked | `re/match/s20/004d40d0.c` | see Remainder |

Six of the nine are g2s rows: `re/match/rwta/` holds the raw `g2s.py` output with the one edit named above (the header
comment of each file says which). The sources under `rwl01/` and `s21/` were edited in place.

0x004e1b60 (RW plugin extension-chunk reader). Ghidra reused `param_2` / `param_3` as the address-taken chunk
`type` / `size`. The match keeps stream and object in their params and declares `int type; int size;` **inside the loop
block**: c2 then parks them in the two dead param slots, as the original does (at function scope one went to the frame,
`sub esp, 0x14`). The last byte was the RwError order: `err.pluginID = 1` is stored before the `_rwerror` call
(`FUN_004d7ff0`), not after it, so the store is not dead.

0x004dba60 is compiled into the exe on the knob path (`mashedmod/src/mashed_re/RwCore/RwCore_004dba60.cpp`, MASHED_ORIG_TOP
+ MASHED_RWC_CORE, provisional). The edit swaps two independent int-to-float conversions, so the value is unchanged.
No other edited source is included by `mashedmod/src` (grep for the source paths).

### Levers that closed these (for MATCH-RWT-B and later RW rows)

- **Run every RW source under rw9044 before shaping.** 10 of the 22 provisional band-A rows were already 100% under
  their own `// TOOLCHAIN` and 12 were C4 in hooks.csv; the review's per-row percentages came from one toolchain.
  0x004dba60 and 0x004c0910 needed rw9044 plus one change, and both stay below 90% under ddk.
- **g2s declaration bugs are the cheap wins.** `extern undefined1 FUN_x` for a function whose address is stored,
  an `int` extern for a float global, and split struct locals (`fStack_c/8/4`, `uStack_10..4`) for an array passed by
  address. Each is a one-line fix.
- **`mov eax, [reg+off]; call eax` = a named function-pointer local** (`code *fn = ...; fn(...)`). The inline
  `(*(code *)p[i])(...)` gives `call [reg+off]`. A by-value `__forceinline` helper taking `fn` gives the same 100%.
- **Ghidra's rep-stos loop with a stosw/stosb tail = intrinsic `memset`** (declare it and `#pragma intrinsic(memset)`).
- **Rotated loops:** where the original has a guard `cmp; je` before the loop and `test; jne` at the bottom, write
  `for (; x; x = next)` and put the failure path after the success return (`goto fail`).
- **RW link-list macro** (`l->prev->next = l->next; l->next->prev = l->prev;`) gives the original load order.
- **Two x87 conversions in source order:** the first written is the first `fild` (0x004dba60).

## Remainder (parked, with what was tried)

| RVA | best | residual | tried |
|---|---|---|---|
| 0x004db3e0 | 95.2 | the two COM `Release` calls take edx then ecx (ours ecx then edx), and the original returns `uVar3` from esi in the last block where ours knows it is 0 | 19 spellings (return shapes, join variable, inverted if/else, vtable struct, fn local, pointer local), both toolchains; `match_permute.py --walk 200`: 865 compiles, 0 gain |
| 0x004c6750 | 93.5 (rw9044) | `image` lives in ebp at the final return (original esi), so the early `return image` on equal size cannot tail-merge into the shared exit | 4 shapes (block-scope source image, return palImage per branch, order of `image = palImage`); permuter 780 compiles, 0 gain |
| 0x004cb4c0 | 90.2 | the 16-dword compare loop: original uses a down counter and folds the memory operand (`cmp esi, [eax]`); pointer and counter forms give the counter but load into ebx | 23 loop shapes incl. an inlined `MemEqual(b0, b1, size)` helper (best 83.5); permuter 699 compiles, 0 gain |
| 0x004d40d0 | 89.1 | the result test is `cmp eax, edi` (edi = 0) in the original, `test eax, eax` in ours, plus an ecx/edx swap on the vtable temp | 14 spellings (zero variable, pointer return, local visitor, fn local), both toolchains. Below 90%, so no permuter |

## Raw data

None committed beyond the sources. Scratch work (variants, g2s outputs of the six g2s rows, permuter logs) stayed in
the session scratchpad and is not needed to reproduce: each committed source re-scores with `match.py --bytes`.
