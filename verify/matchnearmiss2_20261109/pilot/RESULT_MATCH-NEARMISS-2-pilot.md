# MATCH-NEARMISS-2-pilot: hand rewrites on 20 near-miss rows (2026-11-09)

Branch `orch/MATCH-NEARMISS-2-pilot`, base `orch/MATCH-NEARMISS-2` @ bbda49248. Toolchain: `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178), each file's own line-1 FLAGS, unchanged in every row. No `__asm`, no naked,
no pragma used to force bytes. No mashedmod/ edit, no game run, no Ghidra write.

**Verdict: 9 of 20 closed (45%), all 9 promoted C2 -> C4 by route match.**

| miss type | closed | rate | rows closed |
|---|---|---|---|
| order | 6/10 | 60% | 0x005a8390 0x005a8560 0x005ab560 0x005ac7e0 0x004e4450 0x005a7ee0 |
| regalloc | 0/4 | 0% | none |
| count | 2/4 | 50% | 0x00402b70 0x005b0df0 |
| const | 1/2 | 50% | 0x005af070 |

Per-row before/after score, differing bytes, verdict, what closed it or the residual, compile count and source
sha256: `results.tsv`. Compiles per row: 1 to 16 (median 8), all inside the 25-compile budget.

## What closed, and what did not

The closures came from two kinds of rewrite, neither of them generic statement shuffling. (1) The RenderWare
linked-list idiom (6 rows, all of the closed order rows): writing a list insert in `rwLinkListAddLLLink` order
(`link->next = list->next; link->prev = list; list->next->prev = link; list->next = link`), or a list-head init as
`next = self; prev = self`, through a `{next, prev}` link type instead of `int *` arithmetic, with the link fields
addressed from the owning object pointer (`param + off`) where the original addresses them that way. The prior
residual notes on these rows all read "one load/store scheduled one slot off", which was the int-pointer
spelling, not scheduler noise. (2) Wrong facts carried over from the decomp (2 rows): 0x005b0df0 had the two callee
arities swapped (`FUN_005ad6a0` takes 1 argument and `FUN_005ad0b0` takes 4, per their own matched sources
`re/match/s26/005ad6a0.c` / `005ad0b0.c`), and the split `add esp,4` / `add esp,0x10` is the two cdecl cleanups,
closed in 1 compile. 0x005af070 stored `param_6` to +0x54 and `param_7` to +0x50; the original stores `param_7`
(loaded at `[esp+0x24]` after the `add esp,4`) to +0x54 and `param_6` (loaded at `[esp+0x24]` with the call argument
still pushed) to +0x50. The relocation-masked score hid this as a "const" miss at 97.0%. With the mapping fixed, the
three trailing fields in ascending order match. 0x00402b70 closed with the plain index-form join loop
`strcat(out, dirs[i])`. What did not move: one-slot x87 scheduling of a parameter load (0x0057c0d0, 0x005735f0), a
store placed after a reload (0x005bf8a0, 0x005a86a0), base-register choice after copy propagation (0x004c1040,
0x004c1210, 0x00540340), SIB base/index order (0x0055ded0), an AND/OR with a known value that the DDK folds and the
original keeps (0x004967e0, 0x005a9c50), and `imul` operand order (0x005c4ad0). The regalloc type closed 0 of 4.

## Count rows: size artefact check

`verify/c4rnearmiss_20261107/size_triage.py` over the 4 count rows: only 0x00402b70 has `body_end - entry + 1`
(963) != pc_funcs `body_size` (960). It is not a pure size artefact: at 963 B it still had 6 differing bytes (a `jle`
scheduled before `lea ebp`). After the rewrite the whole symbol is 963 B BYTE-IDENTICAL and the instruction score is
99.7% (345/344) only because the scorer reads 960 B and drops the final epilogue `jmp`. `c4_reclass.py` already
accepts that case (`insn ... (Ghidra body short, 345/344)`). The other three count rows have equal sizes, so their
misses are code differences. No row is recorded as SIZE-ARTEFACT.

## Same-file and relocheck checks

All 9 changed files hold one MATCH function each, so no other identical function could regress. The 11 NOT-CLOSED
sources are unchanged (variants were compiled from temporary copies). `relocheck.py` reports OK for all 9 closed
rows (`relocheck.tsv`). 0x004e4450 was first BYTE-IDENTICAL with relocheck BAD (the two globals `DAT_007d3ff8` /
`DAT_007d7164` loaded in swapped order at +0x77/+0x7d, which the byte compare masks); writing the last statement as
`DAT_007d3ff8 + DAT_007d7164 + 8` made it OK. The base-commit source already had that BAD.

## Promotion

`py -3.12 re/tools/c4_reclass.py batch --tsv verify/matchnearmiss2_20261109/pilot/c4_batch.tsv` (dry run, report
`c4_dryrun.tsv`): 9 PASS, 0 refused. Then the same with `--apply --result
verify/matchnearmiss2_20261109/pilot/RESULT_MATCH-NEARMISS-2-pilot.md`: 9 rows C2 -> C4 route match in `hooks.csv`
plus 9 CHANGELOG lines. `re/orchestrator/tracker_lint.py --repo . --base orch/integration`: OK, hooks.csv 6155 rows, pre-existing=3, NEW=0.

## Read for the batch decision (30% per miss type)

- order 60%, count 50%, const 50% pass the 30% line on this pilot. regalloc 0% does not.
- The order yield is the list-link idiom, not the type. Before a batch, grep the rest of the pool for list inserts
  and list-head inits (`+ 4) = ` stores next to a global or `+0x..` list head) and run those first. The order rows
  without that idiom closed 0 of 4 here.
- The count and const closures were decomp errors (callee arity, parameter-to-field mapping). A cheap pre-pass over
  the pool: compare each source's callee prototypes against the callees' own matched sources in `re/match/`, and
  check for a split `add esp` around a nested call. Lead outside the pool: 0x005af690 (88.9%) has the same split
  `add esp` note as 0x005b0df0. 0x005a8060 (40.0%) uses the same `DAT_007dcad8` list as 0x005a8390 / 0x005a8560.
- Samples are small (4 regalloc, 4 count, 2 const), so the per-type rates carry wide error.

## Files

| file | sha256 |
|---|---|
| results.tsv | 6c9061fbe1f468e307133c8f842da079d78572507dcc1b4d7eac16303540dacb |
| hand_log.tsv | 478fd56d277a93c906c20079fb5f0232e21385966e010252d0335607452daeb5 |
| c4_batch.tsv | ce6f78bb70aa582ca725af8168eefca425861ab5f735153c9c0a3e7868320ec9 |
| c4_dryrun.tsv | d76e7f7a6dc383741273b0b8dc82036f633d578d9c56bfdd7ca2d0a4ed9878e8 |
| relocheck.tsv | a994d1f2a3c80b02aa9604667a8e8dfad1b3b86a4f078d2d1cd88952041efdd7 |
| build_results.py | rebuilds results.tsv from hand_log.tsv (re-scores base commit and tree) |
