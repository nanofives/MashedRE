# RESULT C4R-NEARMISS (roadmap run 35, L-93 M5), 2026-11-07

**Verdict: GREEN. Identical near-miss rows 0 -> 12** (0x00402750, 0x0040ac80, and 10+1 self-call rows), plus the four
`HIDDEN` case handlers of 0x0040ac80 (0x0040abc0, 0x0040abe0, 0x0040ac00, 0x0040ac40) shown 100% / byte-identical /
relocheck OK at their own sizes. Bounded permute over the other 98 rows at >=95%: 0 new identical, 1 improved
(0x00495870 98.4% / 401 bad bytes -> 98.9% / 70 bad bytes, applied).

Base: `orch/C4R-BATCH-M2` @ de39c047b. Branch `orch/C4R-NEARMISS`. Compiler for every score: XP DDK cl 13.00.9176 /
c2 13.00.9178 (`tools/vc7ddk`), the file's line-1 `// FLAGS:`. No `__asm` / `naked` in any closed source.
No body swap, no hooks.csv / CHANGELOG / match_ledger.tsv / tracker edits, no Ghidra slot used.

## Ten of the twelve need a tool fix before the ledger can show them

Only **0x00402750** reaches identity under the unmodified `match.py` + `relocheck.py`. The other eleven rows plus
0x0040ac80 are identical in fact, but the stock tool still scores them as before:

| rows | why stock match.py refuses | evidence here | what has to change |
|---|---|---|---|
| 0x004c0ad0 0x004d4bf0 0x004d8280 0x004d83d0 0x004da430 0x004da480 0x004dab70 0x00553e80 0x00554010 0x00554150 0x0055c4f0 | `byte_compare()` checks every same-section relocation as an absolute (DIR32) address. The one "bad" byte in each is the REL32 of a self-recursive `call` (original target +0x0 on both sides) | `selfcall_bytecheck.py` (typed_bad=0 for all 11), `notes.md` §2 | `match_py_rel32.patch` (11 lines in `byte_compare()`, not applied: outside this step's write scope) |
| 0x0040ac80 | `score_function()` sizes the original from pc_funcs `body_size` = 228, Ghidra's address set with the four tail-jumped handlers folded in. The scorer reads past the jump table into FUN_0040acd0 | `hidden_check.py`: 100.0% (12/12) at `body_end - entry + 1` = 0x36, 80 B byte-identical incl. the table, `notes.md` §3 | PCFUNCS-REGEN (contiguous size), or a per-row size override |

Patch check (temporary apply, reverted, `D:/MashedWT/C4R-NEARMISS_data/patch_check.log`): with the patch, stock
`match.py --bytes` gives BYTE-IDENTICAL on all eleven self-call rows and on 0x0040ac80 (DIR32 jump-table path
unchanged), and 30 of 30 random ledger PASS rows stay BYTE-IDENTICAL.

**Decision for C4R-MATCHBATCH-2 (lead's call, not filed as a user decision):** apply `match_py_rel32.patch` to
`re/tools/match/match.py` before the sweep. It is a tool bug fix with no scoring change outside REL32 self-call sites.
0x0040ac80 needs a correct size from PCFUNCS-REGEN. `body_end - entry + 1 != body_size` holds for 225 pc_funcs rows.

## Per item

1. **0x00402750** (`re/match/s01/00402750.c`, edited): five path literals had single backslashes (`\t` -> TAB, other
   escapes dropped the backslash). Doubled them. The fifth (`toastart\pc\pc.txd`) was wrong too but relocheck did
   not flag it, because its constant compare reads 8 bytes and `toastart` matched. After: 100.0% (187/187), 739 B
   byte-identical, relocheck OK (checked 85, unchecked 8 plain `DAT_` globals).
2. **The ten 1-byte rows** (+ 0x00553e80, the first MATCH in `re/match/s24/00554200.c`, refused for the same byte):
   no source shape closes the byte, because nothing in the source is wrong. See the table above. Sources unchanged.
   relocheck OK on all eleven. 0x004c0ad0's `/O2 /GS /Ob1 /G5` line gives the same bytes as the brief's
   `/O2 /Ob1 /G5`. 0x00554200 (same file) stays relocheck BAD and is untouched.
3. **0x0040ac80**: nothing is missing. The "10 extra instructions" are the `8b ff` pad, the 6-entry jump table at
   0x0040acb8 and the start of FUN_0040acd0. The source already reconstructs the dispatcher and the four handlers.
   Source unchanged. The four handlers have no pc_funcs row.
4. **Bounded permute**, 98 rows (110 ledger rows at >=95% not PASS, minus the 12 above). Fixed budget per row:
   `match_permute.py --rounds 6 --walk 40 --time 90 --jobs 4`, hard kill at 300 s, 3 lanes (`permute_driver.py`).
   29,930 compiles in total, no timeouts. Result: 97 rows no gain. **0x00495870** (`re/match/s16/00495fe0.c`): one
   statement swap (`d->stick[0].y = y;` moved after `mag = V2LenSq(...)`) gives 98.9%, 380/380 instructions,
   70 bad bytes (was 401). Applied. 0x00495fe0 in the same file stays 100% / byte-identical. The residual is x87
   store order (`fst [esi+0x2cc]` + `fxch` in the original against `fld st(1)` + `fstp` in ours), a class the
   flags brief lists as not moved by rewrites. No helpers were spawned: the lanes only ran a script.
   `size_triage.py`: 9 of the 98 also have a pc_funcs size mismatch. None of the 9 is identical at the contiguous
   size either (all still have bad bytes in the whole-symbol compare).

## Files

| file | what |
|---|---|
| `results.tsv` | 111 rows: rva, source, before/after %, before/after bad bytes, verdict, how, sha256, flags |
| `relocheck.tsv` | relocheck.py on the 12 closed rows + the 4 HIDDEN handlers: all OK |
| `notes.md` | per-row notes: what closed it or why it did not |
| `permute_summary.tsv` | per-row permute start / best / status |
| `selfcall_bytecheck.py` | byte_compare with typed same-section relocations |
| `hidden_check.py` | match.py score at an explicit size + HIDDEN rows + relocheck over them |
| `size_triage.py` | pc_funcs body_size vs body_end scoring |
| `permute_driver.py` | the bounded permute driver |
| `match_py_rel32.patch` | proposed match.py fix (not applied) |

Sources changed: `re/match/s01/00402750.c`, `re/match/s16/00495fe0.c`. Every other cited source is unchanged
(sha256 in `results.tsv`).

## Raw data (not committed), `D:/MashedWT/C4R-NEARMISS_data/`

| file | sha256 |
|---|---|
| permute_run.log | bc410ecdf3f83274d7a3a942ee7b97fe257722f482574b077afe3b409e647281 |
| permute_rows.tsv | e9a90b75c80612438dc255b9c87917c6f6a1beee1261a2bc57e5fc2d6208d692 |
| permute/summary.tsv | 10b138c8cbffac2ae662a3e7462c6968b6ad14d6a4d86e553e062bfb35ac394d |
| permute/0x00495870_best.c | eff30939819df957f976b0839538e03f32f1a24e5fd79414fd7bfb9a3966918f |
| size_triage.tsv | 64c142c48650793803decafb7e2ce27ecf90fb78fe730af0fd8ff7c5252e559a |
| patch_check.log | 14044a38480513e9b8e92f10863c3b1a566d42b921308296faed6a4d8723c626 |

Per-row permute logs and best variants: `D:/MashedWT/C4R-NEARMISS_data/permute/<rva>.log`, `<rva>_best.c` (822 KB).
