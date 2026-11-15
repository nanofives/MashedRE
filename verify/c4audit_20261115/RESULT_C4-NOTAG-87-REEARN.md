# RESULT C4-NOTAG-87-REEARN (run 49, L-164 (a), L-166)

Match-only / tracker row. No build, no gates, no `--apply` on the branch. Base `orch/integration` 17202f6c3 (the merged
demote op of C4RECLASS-DEMOTE-OP). The train applies the two TSVs below with `batch --apply --strict` and moves them to
`re/promotions/applied/`.

## Input

The 87 weak untagged C4 rows of `verify/c4audit_20261115/c4_notag_audit.tsv`: the rows whose class is not "diff with
the hook installed" (61 install manifest plus survival, 19 diff with the hook bypassed, 6 file missing, 1 other).
Selection script `reearn87_select.py` gives `reearn87_rows.tsv`: 87 rows, 0 in `re/match/ADMITTED.tsv`, 5 in
`re/match/PROVISIONAL.tsv` (`0x0043c5b0`, `0x004c5010`, `0x004cbe80`, `0x004cc6e0`, `0x00473ee0`), and every one of the 87
has at least one `re/match` source carrying `// MATCH: 0x<rva>` (some carry several, 102 row/source pairs).

## Method

1. **Match gate.** `reearn87_probe.py` writes one probe batch per candidate-source round (a batch refuses a duplicate
   rva). Each is run as `c4_reclass.py batch --tsv <probe> --report <out>` with `reearn=1`, as a dry run. The gate
   recompiles the source now (`match.compare --bytes`, recorded compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 or
   rw9044, the source's `// FLAGS:`), runs `relocheck.check`, and refuses a naked or asm-only body (SimGolf G26) and a
   STUB in the body. Log: `reearn87_probe.txt`, reports `reearn87_probe<k>_report.tsv`.
   A row is re-earned when some candidate source PASSes. The chosen source is the passing one whose file stem is the
   rva, otherwise the first passing round.
2. **frida_diff cells.** The 5 rows whose cell is the bare word `green` take the RESULT named by their promotion commit:
   `fadf5eb63` (A6b `0x00468980`, A5 `0x0046ddb0`, A4 `0x00470670`) gives `verify/dcphys2_20261022/RESULT_dcphys2.md`,
   section 2 and lines 142-143. `22e9bc96d` (A3 `0x0046b540`) and `2d9cd2b2f` (A6a `0x00467650`) give
   `verify/dcphys_20261021/RESULT_dcphys.md`, section 1 and lines 78-80 / 111.
3. **Demote.** Every row that did not pass goes in one demote batch. Evidence is the audit TSV, which names every rva,
   so the op's "evidence names the RVA" check holds. The reason gives the audit class and the match refusal.

`reearn87_tsvs.py` builds both TSVs from the probe reports.

## Tool form (as merged), one deviation from the brief

The brief named the re-earn file `re/promotions/C4-NOTAG-87-REEARN-tag.tsv` with header `rva route evidence result`.
The merged train (`re/orchestrator/merge_train.ps1` `Apply-Promotions`) hands every added `*-tag.tsv` to
`c4_reclass.py tag`, which needs a `tag` column and does not recompile, so that file would be refused and set the branch
aside. Also, `batch` SKIPs a row that is already C4 unless the row has `reearn=1` (merged column). So the re-earn file is
a batch file, **`re/promotions/C4-NOTAG-87-REEARN-match.tsv`**, header `rva route evidence reearn result`. The train
applies it through `batch --apply --strict`. The demote file is **`re/promotions/C4-NOTAG-87-REEARN.tsv`**, header
`rva route evidence result reason frida_diff`, route `demote`. The two files share no rva, so the order they are applied
in does not matter.

## Counts (sum 87)

| bucket | rows | audit classes |
|---|---|---|
| re-earned `c4=match` (byte-identical + relocheck OK, recorded compiler) | **73** | 52 install manifest plus survival, 19 diff with the hook bypassed, 1 other (`0x0042b8c0`), 1 file missing (`0x0042b900`) |
| demoted C4 -> C3 | **13** | 8 install manifest plus survival, 5 file missing (the `green` rows) |
| skipped (already C3, no write) | **1** | `0x0043c5b0` (provisional, insn 40.5%) |
| frida_diff cell fixed (inside the demoted 13) | **5** | `0x0046ddb0`, `0x00467650`, `0x00468980`, `0x00470670`, `0x0046b540` |
| refused by the tool | **0** | |

By route: byte-matched `c4=match` 73, `c4=libid` 0, replay-verified `c4=frida-replay` 0. The 73 are byte-match
evidence for the RE. They make no claim about the port (L-93).

All 5 provisional rows failed the match gate. `0x0043c5b0` was already C3. The other 4 are demoted.

## The 5 `green` rows: classification from the dcphys RESULTs

None of the 5 is byte-identical (dry run: `0x0046ddb0` insn 81.7%, `0x00467650` 56.1%, `0x00468980` 90.1%,
`0x00470670` 98.0%, `0x0046b540` 59.2%), so route match does not hold.

What the cited RESULTs show: an in-process A/B on the original, dev `.asi`, hook installed live, canonical recipe. Each
sampled call compares the full 0xd04 record (A3 also compares `0x00613108/14/30/3c` and the return), with a no-op
control `noo` of 0.
- `RESULT_dcphys.md` table section 1: A6a 1440/1440 on f1, A3 12/12.
- `RESULT_dcphys2.md` lines 142-143: final `.asi` 402 / 3214 / 5403 calls, `noo` 0.

Neither RESULT cites the Write-set leg as `re/CONFIDENCE.md` writes it (L-38 / L-40 / L-54, 2026-10-27..29, after
both RESULTs): no heap-tracked shadow leg with more than 0 pages, and no statement that heap tracking was on during the
record compare. `grep -i "shadow|heap"` finds no such line in either file. The route as now written is therefore not
complete. They are demoted to C3 with `frida_diff` set to the RESULT, and the reason names the missing leg.

[UNCERTAIN] Whether the full-record compare covers every store these five bodies make outside their own frame. That is
read from code, not tested. Re-pickup: a write-set leg run (L-38 form) with the hook live, after which they can go back
to C4 by the frida route. Or match the bodies.

## Dry run on the head (17202f6c3), the lines the train will apply

```
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/C4-NOTAG-87-REEARN-match.tsv --result verify/c4audit_20261115/RESULT_C4-NOTAG-87-REEARN.md --jobs 6
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/C4-NOTAG-87-REEARN.tsv --result verify/c4audit_20261115/RESULT_C4-NOTAG-87-REEARN.md
```

Full output, including the CHANGELOG lines `--apply` would write: `reearn87_dryrun.txt`. Per-row reports:
`reearn87_match_dryrun.tsv`, `reearn87_demote_dryrun.tsv`. Verdict lines:

```
[1/73] 0x004938c0 match PASS byte-identical 25 B, relocheck OK (5/0)
[2/73] 0x004c2f00 match PASS byte-identical 45 B, relocheck OK (2/0)
[3/73] 0x00498bf0 match PASS byte-identical 6 B, relocheck OK (1/0)
[4/73] 0x004b3b70 match PASS byte-identical 61 B, relocheck OK (3/0)
[5/73] 0x004099e0 match PASS byte-identical 10 B, relocheck OK (1/0)
[6/73] 0x00407a40 match PASS byte-identical 17 B, relocheck OK (1/0)
[7/73] 0x0042b930 match PASS byte-identical 6 B, relocheck OK (1/0)
[8/73] 0x0042b8d0 match PASS byte-identical 14 B, relocheck OK (1/0)
[9/73] 0x00493f70 match PASS byte-identical 6 B, relocheck OK (1/0)
[10/73] 0x004963e0 match PASS byte-identical 24 B, relocheck OK (1/1)
[11/73] 0x00496400 match PASS byte-identical 104 B, relocheck OK (3/3)
[12/73] 0x00498910 match PASS byte-identical 50 B, relocheck OK (9/0)
[13/73] 0x00498950 match PASS byte-identical 90 B, relocheck OK (10/0)
[14/73] 0x004989b0 match PASS byte-identical 77 B, relocheck OK (8/0)
[15/73] 0x0042c2f0 match PASS byte-identical 10 B, relocheck OK (1/0)
[16/73] 0x0042b8c0 match PASS byte-identical 7 B, relocheck OK (1/0)
[17/73] 0x00427780 match PASS byte-identical 17 B, relocheck OK (2/0)
[18/73] 0x00430a10 match PASS byte-identical 80 B, relocheck OK (1/0)
[19/73] 0x004c3ac0 match PASS byte-identical 112 B, relocheck OK (2/0)
[20/73] 0x004099a0 match PASS byte-identical 53 B, relocheck OK (5/1)
[21/73] 0x0042a920 match PASS byte-identical 20 B, relocheck OK (2/0)
[22/73] 0x0046d700 match PASS byte-identical 54 B, relocheck OK (3/0)
[23/73] 0x00409930 match PASS byte-identical 31 B, relocheck OK (3/0)
[24/73] 0x00409900 match PASS byte-identical 44 B, relocheck OK (4/1)
[25/73] 0x004098b0 match PASS byte-identical 28 B, relocheck OK (4/0)
[26/73] 0x0042b900 match PASS byte-identical 6 B, relocheck OK (1/0)
[27/73] 0x004c2d70 match PASS byte-identical 6 B, relocheck OK (1/0)
[28/73] 0x0040dd60 match PASS byte-identical 24 B, relocheck OK (2/0)
[29/73] 0x00552df0 match PASS byte-identical 73 B, relocheck OK (5/0)
[30/73] 0x0040dc90 match PASS byte-identical 24 B, relocheck OK (2/0)
[31/73] 0x00550b00 match PASS byte-identical 187 B, relocheck OK (6/1)
[32/73] 0x0042e590 match PASS byte-identical 29 B, relocheck OK (3/0)
[33/73] 0x004c3b90 match PASS byte-identical 87 B, relocheck OK (2/0)
[34/73] 0x004cd060 match PASS byte-identical 11 B, relocheck OK (1/0)
[35/73] 0x004c3b30 match PASS byte-identical 84 B, relocheck OK (2/0)
[36/73] 0x00494ef0 match PASS byte-identical 5 B, relocheck OK (1/0)
[37/73] 0x0042fe50 match PASS byte-identical 21 B, relocheck OK (2/0)
[38/73] 0x00550bc0 match PASS byte-identical 8 B, relocheck OK (0/0)
[39/73] 0x00442c80 match PASS byte-identical 63 B, relocheck OK (4/0)
[40/73] 0x00426c10 match PASS byte-identical 28 B, relocheck OK (4/0)
[41/73] 0x00426c30 match PASS byte-identical 23 B, relocheck OK (3/0)
[42/73] 0x00426c70 match PASS byte-identical 23 B, relocheck OK (3/0)
[43/73] 0x0040de00 match PASS byte-identical 5 B, relocheck OK (1/0)
[44/73] 0x004c5820 match PASS byte-identical 16 B, relocheck OK (2/0)
[45/73] 0x00475a60 match PASS byte-identical 74 B, relocheck OK (6/0)
[46/73] 0x004c3bf0 match PASS byte-identical 101 B, relocheck OK (2/0)
[47/73] 0x00498bc0 match PASS byte-identical 6 B, relocheck OK (1/0)
[48/73] 0x00498bd0 match PASS byte-identical 6 B, relocheck OK (1/0)
[49/73] 0x00407db0 match PASS byte-identical 111 B, relocheck OK (1/0)
[50/73] 0x004098a0 match PASS byte-identical 6 B, relocheck OK (1/0)
[51/73] 0x0040b420 match PASS byte-identical 12 B, relocheck OK (1/0)
[52/73] 0x0040dc70 match PASS byte-identical 6 B, relocheck OK (1/0)
[53/73] 0x0041e9f0 match PASS byte-identical 9 B, relocheck OK (1/0)
[54/73] 0x0041ea00 match PASS byte-identical 9 B, relocheck OK (1/0)
[55/73] 0x0041ea70 match PASS byte-identical 9 B, relocheck OK (1/0)
[56/73] 0x0041efe0 match PASS byte-identical 24 B, relocheck OK (1/0)
[57/73] 0x00420da0 match PASS byte-identical 30 B, relocheck OK (1/0)
[58/73] 0x00420dc0 match PASS byte-identical 31 B, relocheck OK (1/0)
[59/73] 0x00454a30 match PASS byte-identical 15 B, relocheck OK (1/0)
[60/73] 0x004cc770 match PASS byte-identical 28 B, relocheck OK (1/0)
[61/73] 0x0045caf0 match PASS byte-identical 48 B, relocheck OK (3/0)
[62/73] 0x0045dd50 match PASS byte-identical 12 B, relocheck OK (1/0)
[63/73] 0x0046bce0 match PASS byte-identical 54 B, relocheck OK (3/0)
[64/73] 0x0046bd20 match PASS byte-identical 54 B, relocheck OK (3/0)
[65/73] 0x0046bda0 match PASS byte-identical 104 B, relocheck OK (3/0)
[66/73] 0x0046dd80 match PASS byte-identical 7 B, relocheck OK (1/0)
[67/73] 0x00423cc0 match PASS byte-identical 140 B, relocheck OK (13/0)
[68/73] 0x00423d50 match PASS byte-identical 127 B, relocheck OK (12/0)
[69/73] 0x00423dd0 match PASS byte-identical 140 B, relocheck OK (13/0)
[70/73] 0x00423e60 match PASS byte-identical 127 B, relocheck OK (12/0)
[71/73] 0x00424100 match PASS byte-identical 172 B, relocheck OK (16/0)
[72/73] 0x004241c0 match PASS byte-identical 172 B, relocheck OK (16/0)
[73/73] 0x004894f0 match PASS byte-identical 16 B, relocheck OK (0/0)
batch: 73 pass, 0 refused, 0 skipped (already C4), 73 rows  [by route: match=73]
rc=0
[1/14] 0x0043c5b0 demote SKIP row is C3, not C4 (no write)
[2/14] 0x005aee20 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 23 differing unmasked bytes of 31 (insn 78.6%)
[3/14] 0x004c5010 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 36 differing unmasked bytes of 398 (insn 84.7%)
[4/14] 0x004cbe80 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 155 differing unmasked bytes of 452 (insn 78.6%)
[5/14] 0x0046ddb0 demote PASS C4->C3: weak C4 evidence, audit class 'file missing' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 1998 differing unmasked bytes of 3099 (insn 81.7%); frida_diff was the bare word green, now verify/dcphys2_20261022/RESULT_dcphys2.md: hook-live in-process A/B, canonical recipe, no write-set leg cited (L-38 / L-40 / L-54)
[6/14] 0x00467650 demote PASS C4->C3: weak C4 evidence, audit class 'file missing' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 3885 differing unmasked bytes of 4803 (insn 56.1%); frida_diff was the bare word green, now verify/dcphys_20261021/RESULT_dcphys.md: hook-live in-process A/B, canonical recipe, no write-set leg cited (L-38 / L-40 / L-54)
[7/14] 0x00468980 demote PASS C4->C3: weak C4 evidence, audit class 'file missing' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 30 differing unmasked bytes of 437 (insn 90.1%); frida_diff was the bare word green, now verify/dcphys2_20261022/RESULT_dcphys2.md: hook-live in-process A/B, canonical recipe, no write-set leg cited (L-38 / L-40 / L-54)
[8/14] 0x00470670 demote PASS C4->C3: weak C4 evidence, audit class 'file missing' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 35 differing unmasked bytes of 802 (insn 98.0%); frida_diff was the bare word green, now verify/dcphys2_20261022/RESULT_dcphys2.md: hook-live in-process A/B, canonical recipe, no write-set leg cited (L-38 / L-40 / L-54)
[9/14] 0x0046b540 demote PASS C4->C3: weak C4 evidence, audit class 'file missing' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 906 differing unmasked bytes of 1356 (insn 59.2%); frida_diff was the bare word green, now verify/dcphys_20261021/RESULT_dcphys.md: hook-live in-process A/B, canonical recipe, no write-set leg cited (L-38 / L-40 / L-54)
[10/14] 0x004c3c60 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 6 differing unmasked bytes of 250 (insn 97.3%)
[11/14] 0x004cc6e0 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 128 differing unmasked bytes of 132 (insn 45.9%)
[12/14] 0x00482900 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 6 differing unmasked bytes of 48 (insn 85.7%)
[13/14] 0x00473ee0 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 833 differing unmasked bytes of 1461 (insn 66.3%)
[14/14] 0x00432b30 demote PASS C4->C3: weak C4 evidence, audit class 'install manifest plus survival' (C4-NOTAG-AUDIT, L-164 (a)); match.py --bytes: not byte-identical: 955 differing unmasked bytes of 1580 (insn 74.2%)
batch: 13 pass, 0 refused, 1 skipped (already C4), 14 rows  [by route: demote=13]
rc=0
```

## test_c4_reclass.py on this branch

```
[test-slot] acquired slot 2/8 after 600s
collecting ... collected 29 items
re/tools/tests/test_c4_reclass.py::test_dry_run_writes_nothing PASSED    [  3%]
re/tools/tests/test_c4_reclass.py::test_apply_touches_only_promoted_lines PASSED [  6%]
re/tools/tests/test_c4_reclass.py::test_apply_needs_result PASSED        [ 10%]
re/tools/tests/test_c4_reclass.py::test_strict_writes_nothing_on_refusal PASSED [ 13%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev0-not byte-identical] PASSED [ 17%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev1-relocheck BAD] PASSED [ 20%]
re/tools/tests/test_c4_reclass.py::test_match_refusals[ev2-shorter than the original body] PASSED [ 24%]
re/tools/tests/test_c4_reclass.py::test_toolchain_must_be_ddk PASSED     [ 27%]
re/tools/tests/test_c4_reclass.py::test_second_apply_is_a_noop PASSED    [ 31%]
re/tools/tests/test_c4_reclass.py::test_tag_dry_run_writes_nothing PASSED [ 34%]
re/tools/tests/test_c4_reclass.py::test_tag_apply_is_notes_only_and_idempotent PASSED [ 37%]
re/tools/tests/test_c4_reclass.py::test_tag_refuses_missing_row_and_strict PASSED [ 41%]
re/tools/tests/test_c4_reclass.py::test_batch_lever_column_tags_promoted_row PASSED [ 44%]
re/tools/tests/test_c4_reclass.py::test_refresh_changes_only_the_sha PASSED [ 48%]
re/tools/tests/test_c4_reclass.py::test_refresh_idempotent_when_current PASSED [ 51%]
re/tools/tests/test_c4_reclass.py::test_refresh_dry_run_writes_nothing PASSED [ 55%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_below_100 PASSED [ 58%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_relocheck_not_ok PASSED [ 62%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_row_not_at_c4 PASSED [ 65%]
re/tools/tests/test_c4_reclass.py::test_refresh_refuses_other_source_and_missing_token PASSED [ 68%]
re/tools/tests/test_c4_reclass.py::test_refresh_strict_writes_nothing_on_one_refusal PASSED [ 72%]
re/tools/tests/test_c4_reclass.py::test_demote_dry_run_verdicts_write_nothing PASSED [ 75%]
re/tools/tests/test_c4_reclass.py::test_demote_apply_touches_only_that_line_and_is_idempotent PASSED [ 79%]
re/tools/tests/test_c4_reclass.py::test_demote_refusals PASSED           [ 82%]
re/tools/tests/test_c4_reclass.py::test_demote_needs_reason_column PASSED [ 86%]
re/tools/tests/test_c4_reclass.py::test_batch_route_demote_is_the_merge_train_path PASSED [ 89%]
re/tools/tests/test_c4_reclass.py::test_reearn_column_gates_an_untagged_c4_row PASSED [ 93%]
re/tools/tests/test_c4_reclass.py::test_reearn_column_refusal_leaves_row PASSED [ 96%]
re/tools/tests/test_c4_reclass.py::test_frida_diff_column_replaces_the_cell_on_write PASSED [100%]
============================= 29 passed in 1.41s ==============================
```

## Files

`re/promotions/C4-NOTAG-87-REEARN-match.tsv`, `re/promotions/C4-NOTAG-87-REEARN.tsv`, and under
`verify/c4audit_20261115/`: `reearn87_select.py`, `reearn87_rows.tsv`, `reearn87_probe.py`, `reearn87_probe<0..5>.tsv`,
`reearn87_probe<0..5>_report.tsv`, `reearn87_probe.txt`, `reearn87_tsvs.py`, `reearn87_dryrun.txt`,
`reearn87_match_dryrun.tsv`, `reearn87_demote_dryrun.tsv`, `reearn87_pytest.txt`, this RESULT. No raw off-git data.
