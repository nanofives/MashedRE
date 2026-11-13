# RESULT MATCH-RWT-B: doubled RW match lane, bands B-cf and B50, first measure of 0x004c7a70 (2026-11-12)

Row kind: match-only (no build, no gates, no game run, no game lock). Session on claude3, Opus 5.5.
Input: `orch/MATCH-RWT-A:verify/match_20261112/RANKED_MATCH-RWT.tsv` (open B-cf and B50 rows only; A95 / A80 left to
MATCH-RWT-A; 0x004d1e30, 0x00534b60, 0x004f1fb0, 0x0053dea0 not touched).

## Verdict

**11 functions byte-identical** under the recorded compilers, relocheck OK, plain C (no `__asm`, no naked), all 11 PASS
in `py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-B.tsv` (dry run, nothing written).
Promotion file: `re/promotions/MATCH-RWT-B.tsv` (route `match`). `--apply` is left to the merge train.

| rva | band | path | before | after | toolchain | bytes | relocheck | source | what moved it |
|---|---|---|---|---|---|---|---|---|---|
| 0x004d1d70 | B50 | menu | 72.1 (ddk = rw9044) | 100 | ddk and rw9044 | 188 B | OK 3/0 | re/match/s20/004d1d70.c | `ext` declared with its initializer before `next` (DAT_00911ae4 load and lea placement) |
| 0x004d3450 | B50 | race_load | 71.7 (g2s rw9044) | 100 | ddk and rw9044 | 117 B | OK 1/0 | re/match/rwtb/004d3450.c | store through param_1 (g2s had `unaff_retaddr`), COM call `__stdcall`, parent walk as `while`, signed max |
| 0x00538c30 | B50 | race_load | 57.1 | 100 | ddk and rw9044 | 66 B | OK 2/0 | re/match/rwtb/00538c30.c | the two error words as one 8-byte struct (own frame slot, not the dead param slots), flags /O2 /Ob1 /G5 |
| 0x004ce6f0 | B50 | race_load | 73.7 (ddk) | 100 | ddk and rw9044 | 145 B | OK 7/0 | re/match/rwtb/004ce6f0.c | clean rewrite: end pointer = path + strlen, `for` over the list at +0x21c, next at +0x30 |
| 0x004ce5d0 | B50 | race_load | 54.3 | 100 | ddk and rw9044 | 179 B | OK 10/0 | re/match/rwtb/004ce5d0.c | the call args Ghidra dropped (realloc 3 args, malloc 2), error with variadic FUN_004d7ff0, sum order `DAT_007d3ff8 + DAT_007d4628` |
| 0x00544e00 | B50 | race_load | 59.8 (rw9044) | 100 | rw9044 (ddk 97.9) | 261 B | OK 6/0 | re/match/rwtb/00544e00.c | field-by-field copy, block-scope float temp for the +0x54 copy, `1.0f / (0, x)` comma rvalue for the fdivr order |
| 0x00557e50 | B50 | menu | 60.0 | 100 | rw9044 | 55 B | OK 5/0 | re/match/rwtb/00557e50.c | join variable `ret` (stops the neg/sbb select) |
| 0x00533740 | B50 | menu | 57.9 | 100 | rw9044 | 172 B | OK 14/0 | re/match/rwtb/00533740.c | registration record as one 12-dword struct; Ghidra `LAB_` targets named `FUN_<rva>` so all 14 relocations are checked |
| 0x005444a0 | B50 | race_load | 53.8 (rw9044) | 100 | rw9044 | 588 B | OK 14/**1** | re/match/rwtb/005444a0.c | user-data stream read as an inline per-array reader, string branch `len > 0` first, case-2 read through its own extern name (see note) |
| 0x00543500 | B50 | race_run | 57.3 (rw9044) | 100 | rw9044 | 528 B | OK 25/0 | re/match/rwtb/00543500.c | COM calls `__stdcall`, `FUN_004d7100(1)` / `(0)` as two calls (cross-jumped), lit branch with the `param_3 == 0` arm first, `if (!ext) 0 else ext[0xc]`, ext loaded through mesh+8 before mat |
| 0x005493e0 | B50 | menu | 62.8 (rw9044) | 100 | rw9044 | 410 B | OK 7/0 | re/match/rwtb/005493e0.c | memset intrinsic, error struct, typed record steps (0x44, 0x2c, 12, 4, 8 bytes), loop guard `if (nt > 0) do..while` |

Race path: 7 of 11 (0x004d3450, 0x00538c30, 0x004ce6f0, 0x004ce5d0, 0x00544e00, 0x005444a0, 0x00543500).
The c4_reclass dry run reports C2->C4 for 0x004d1d70, 0x004ce5d0, 0x005493e0 and C1->C4 for the other eight (hooks.csv on
orch/integration a616fcc3b).

**Note on 0x005444a0 (one unchecked relocation).** The original calls 0x004cc790 from two separate sites in the format 1
and format 2 arms (listing lines 109 and 154 of `match.py --show`, both `call 0x4cc790`). Written as two identical calls,
c2 merges the arms and the body is 9 instructions short; the original source had two distinct callees that resolve to one
address. The case-2 call therefore goes through a second extern name `FUN_004cc790_site2`, which relocheck cannot resolve
to an address and lists as unchecked (14 checked, 1 unchecked, verdict OK, c4_reclass PASS). The target was checked by hand
against the original listing. If the lead wants only fully checked relocations, drop this row from the TSV.

Every new source lives in `re/match/rwtb/` (header `// FLAGS: /O2 /Ob1 /G5`, `// TOOLCHAIN rw9044`, a comment naming the
mechanical content). The g2s regeneration came from `decomp_pc.py --port --callees` (run from the main checkout: the
worktree has no `mashed_pool`) and `g2s.py` under both toolchains.

## Step 1: toolchain rescore sweep (both toolchains, every existing B50 / B-cf source)

`verify/match_20261112/MATCH-RWT-B_toolchain_sweep.tsv` (40 sources). **No free 100%.** Gains under rw9044:
0x004c4a50 77.8 -> 92.9, 0x004cff20 62.5 -> 79.7, 0x004d0290 73.4 -> 79.8, 0x0053d820 66.7 -> 72.5,
0x004cf5a0 59.4 -> 65.2, 0x004cc050 78.5 -> 80.6. Losses under rw9044: 0x0053fbb0, 0x004e6e00, 0x004cc5e0,
0x004d8810 (keep ddk). The rest score the same on both.

## Step 2: B-cf (compile failures), first measures

| rva | size | path | ddk | rw9044 | source | compile fixes |
|---|---|---|---|---|---|---|
| 0x004c7a70 `_rwDeviceSystemFn` | 2810 | race_run | 8.0 | **19.0** | re/match/rwtb/004c7a70.c | tagRECT/LPRECT typedef before the GetClientRect import, param_1 int (switch), case labels plain ints, Direct3DCreate9 returns int*, COM globals int*, one `*(undefined4 *)param_1` store |
| 0x004c9290 | 2106 | race_run | 23.3 | 34.9 | re/match/rwtb/004c9290.c | same type fixes (LPRECT, COM globals) |
| 0x004ca160 | 2481 | race_run | 10.4 | 32.7 | re/match/rwtb/004ca160.c | same, plus `extern int DAT_007d4140` |
| 0x004cac10 | 638 | race_run | 66.2 | **92.6** (shaped, see below) | re/match/rwtb/004cac10.c | COM globals int*, code* callback |
| 0x004ea9a0 | 2430 | race_run | 20.6 | 22.4 | re/match/rwtb/004ea9a0.c | stack slot local_d8 used as float split into `fd8`, one `*(int *)uStack_ac` index |
| 0x004e5850 `RpWorldCreate` | 923 | menu | 35.9 | 44.6 | re/match/rwtb/004e5850.c | the decompiler plate comment broke the C (removed) |
| 0x00536780 | 443 | race_load | 21.8 | 44.6 | re/match/rwl03/00536780.c (unchanged) | none: it compiled; it scored "compile-fail" because no `pc_funcs.csv` size row exists |
| 0x00536e90 | 330 | race_run | 99.1 | 99.1 | re/match/rwl03/00536e90.c (unchanged) | same (no size row) |
| 0x00536fe0 | 528 | race_run | 14.6 (g2s, /O2 /GS) | 15.8 | log only (g2s from `decomp_pc.py --create`) | none |

The three Ghidra-missed functions (0x00536780, 0x00536e90, 0x00536fe0) have **no row in `re/console/pc_funcs.csv`**, so
`match.py` and `c4_reclass.py` refuse them ("no Ghidra size"). Sizes from a transient `decomp_pc.py --create`: 443, 330,
528 B. They were scored with those three rows appended to `pc_funcs.csv` in the working tree only, then the file was
restored (not committed). A C4 for any of them needs those rows in `pc_funcs.csv` first.

**0x00536e90 at 99.1%**: one load (`mov ebp,[esp+0x14]`, the position stride) is scheduled 3 slots later than in the
original. Tried: 6 placements of `pos += pstride`, 6 type variants (unsigned stride, char/V3 pointer, loop-header
increment, inline `((V3 *)pos)->`, stride copy), 7 commutation variants of the last vertex, 8 flag sets, permuter 667
compiles. No change.

## Step 3: shaping of B50 and B-cf rows (sources committed, below 100)

| rva | path | before | after | toolchain | what moved it | residue |
|---|---|---|---|---|---|---|
| 0x005441f0 | race_load | 70.2 | 98.7 (regs 98.7, ops 99.1) | rw9044 | nested `RwMalloc(rwstrlen(s) + 1, 0x30002)` (Ghidra split the call and moved the hint), format switch with three separate `case` bodies (dec chain), `memcpy` intrinsic for the copy, block-scoped first loop counter | stack-slot order and one sub placement |
| 0x005518f0 | race_load | 68.8 | 96.8 (regs 100, ops 100) | rw9044 | unsigned `i < 8` with `1 << i`, create call in both lookup arms, `FUN_0052ddf0(FUN_0052ddc0(anim), (*anim)+4)` (Ghidra dropped the second arg), refcount `r = refs - 1; store; test`, `? stream : 0` | eax/ecx swap in the release block (6 forms, inline helper, permuter 657 compiles) |
| 0x004c4a50 | race_run | 77.8 | 97.2 | rw9044 (toolchain line added) | comma rvalue `(0, unitAxis->f)` on crossed and scaled products | c2 schedules the pos=0 / flags=3 stores and the combineOp load after the x87 chain; the original interleaves them (11 statement positions, permuter 1003 compiles) |
| 0x004cac10 | race_run | compile-fail | 92.6 | rw9044 | COM calls `__stdcall` (no `add esp`), bit-set branch first, swap-chain Present with 5 args, 0x6c-byte light records, `0 <= hr` | raster extension pointer: original `add eax,edx; [eax+0x1c]`, here folded into `[ecx+edx+0x1c]` (6 forms; the permuter's 96.5% rewrote a compare into a different instruction, not kept) |
| 0x004c39b0 | race_run | 69.9 | 69.9 (rw9044 ops 100) | — | not committed | eax/ecx swap for `in`; a hoisted `base` local gives 75.9 but costs ops |
| 0x004d8300 | race_run | 51.6 (regs 100, ops 100) | 51.6 | — | not committed | f held in edi, original ebp (push order); 7 loop forms tried |
| 0x004dc8a0 | race_run | 58.3 | 58.3 | — | not committed | the counter load is hoisted above the four zero stores; volatile, struct, pointer, inline-helper forms tried |
| 0x004cbbd0 | race_run | 77.4 (rw9044) | 77.4 | — | not committed | original keeps c.x/c.y/c.z live on the x87 stack and multiplies with `fmul st(i)`; all 48 term/operand orders identical, double/float temps worse |

| 0x0054f8d0 | race_load | 56.7 | 64.3 (ops 97.0) | rw9044 | error struct, variadic FUN_004d7ff0, header as two unsigned shorts | frame layout 4 bytes apart from the first spill on, shifts most stack operands |

### 0x005441f0 (98.7%)

Two residues: the `lea edx,[esi+edi+4]` (param_2 + param_3 + 4) is scheduled before `xor ebp,ebp` in the original and after
it here, and the string-copy pointer difference is formed as `mov ebp,eax; sub ebp,esi` in the original, `sub eax,esi;
mov ebp,eax` here. Tried: 3 statement orders for the loop setup, `piVar8[3] - piVar9`, index-form string loop (92.0),
two permuter passes (1224 + 1812 compiles; the first found the `iStack_8` step order, 96.4 -> 98.7).

## Levers that worked (for the next lane)

- g2s sources lose call arguments (Ghidra merges pushes across nested calls) and store through `unaff_retaddr`: a clean
  rewrite from the original instruction stream closed 7 of 8 rows. Small g2s rows (< 300 B) are the cheap end.
- COM vtable calls through Ghidra's `code` type are cdecl; the original's are `__stdcall` (no `add esp` after).
- Join variable for `return x ? p : 0` (0x00557e50), struct for an address-taken pair (0x00538c30), declaration
  initializer order (0x004d1d70), `(0, x)` comma rvalue for x87 operand order (0x00544e00, 0x004c4a50).
- Sum order of two globals decides their load order (0x004ce5d0: `DAT_007d3ff8 + DAT_007d4628`).
- Nested `RwMalloc(rwstrlen(s) + 1, hint)` and a switch with separate identical case bodies (dec chain) (0x005441f0).

## Untouched remainder (open B50 / B-cf rows not worked this session)

Race path B50: 0x004cdd60 (79.2), 0x004dcbb0 (77.5), 0x004e5fc0 (77.3), 0x0053d5a0 (75.3), 0x004d0290 (73.4; rw9044 79.8),
0x004ce970 (71.5), 0x004e4c30 (71.3), 0x0053fbb0 (68.5), 0x004e6e00 (67.9), 0x004cee90 (66.7), 0x0053d820 (66.7; rw9044 72.5),
0x004c4360 (65.2), 0x004cff20 (62.5; rw9044 79.7), 0x004cc5e0 (59.8), 0x004cf5a0 (59.4; rw9044 65.2),
0x004e8a10 (58.6), 0x004e9330 (51.5), 0x004d8810 (50.7), 0x004e8120 (50.2).
Menu B50: 0x004d5190 (79.0), 0x004cc050 (78.5; rw9044 80.6), 0x004daff0 (77.8), 0x004f3440 (67.1), 0x0053d0b0 (67.1),
0x004fa5f0 (62.5), 0x004c30b0 (61.7), 0x004d7de0 (61.5), 0x004de1b0 (56.8), 0x004e1e70 (56.5),
0x00549970 (53.6), 0x00543b30 (52.9), 0x0053fec0 (50.9), 0x004e13f0 (50.0).
B-cf: measured above, not shaped past the first measure except 0x004cac10.

## Raw data (not committed)

`D:/MashedWT/MATCH-RWT-B_data/`:
- `decomp.json` sha256 0c1f45bf8fd7f7ff0e153fd469f9abffa147f91aa226480a14f59996485545d4 (19 functions, `--port --callees`)
- `decomp_6fe0.json` sha256 7bc215cab29c86678f750e4a312edb2a347ee91f09a4eb2216454a23e4e6b5cc (`--create`)
- `create_sizes.json` sha256 114748c61a9fe1178fc8b61361bc8f13feef24dda7b8e43d62b48a6cfbd83ecc (`--create --no-decomp`)

Committed summaries: `MATCH-RWT-B_toolchain_sweep.tsv`, `MATCH-RWT-B_g2s_regen.tsv`.
