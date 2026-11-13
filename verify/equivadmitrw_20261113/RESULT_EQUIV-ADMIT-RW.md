# RESULT EQUIV-ADMIT-RW (D4, run 41, 2026-11-13): 5 of 64 admitted

Row: L-143 / L-144, admit the executed RW EQUIV sources through the EQUIV-ADMIT-1 gate. Branch `orch/EQUIV-ADMIT-RW` off
`orch/integration`. Text row: `re/match/ADMITTED.tsv` (+5 rows) and 21 g2s sources copied to `re/match/g2seq/`.
No build, no gates, no game run, no hooks.csv edit, no promotion. Never C4 (L-128).

## Count correction
The input list `equiv_g2s_executed_rw_64.txt` holds 21 EQUIV lines (5 plain, 16 conditional) from the g2s census. The other
43 are the executed-RW rows of `measure_final.tsv` (39 EQUIV + 4 conditional; `in.tsv` has all 64). So the "38 + 5" is 39 + 21.

## Re-run (tool commit eee2fc626, `run_equiv.sh`, `equiv64.log`, `verdicts.tsv`)
45 EQUIV, 19 EQUIV-IF-CALLERS-IGNORE, 0 NOT-PROVEN. Verdicts equal the census. g2s sources (non-100% DDK/rw9044 output of
RWC-G2S-HARVEST) are copied unchanged into `re/match/g2seq/<rva>.c` so each row points at a committed file.

## Admitted (5), cross-check `dyn_{default,seeded}.tsv`, 16 runs each, 0 differences, 8+ conclusive in one mode
0x0052e5c0 (16/16), 0x00533900 (10/16), 0x00544100 (5/10), 0x005516c0 (16/16), 0x00493640 (16/16). No `--relax-icall`,
no x87 order, no conditional verdict. ADMITTED.tsv: 39 data rows, all 15 columns. No compiled caller on the path (bind later).

## Excluded
- **Superseded, 38 rows:** the source is BYTE-IDENTICAL and hooks.csv already has C4 (precedent EQUIV-ADMIT-A6, 0x004cba80).
  Includes 0x004d1640 (C4 new, ENC,VALUE-FLOW). RVAs: every `C4` row in `verdicts.tsv`.
- **Conditional, 0 callers, 16 rows:** 0x004e3b30 0x004e3e40 0x004e6610 0x004e6630 0x0052e560 0x0052e580 0x0052e5a0 0x00533820
  0x00538800 0x0053d1c0 0x0053f9d0 0x00544de0 0x00549350 0x00549390 0x00549fe0 0x005516e0 (and 0x00543e50 is plain EQUIV,
  byte-identical, C4). Reached only through pointers, "scan says ignore 0/0" is vacuous: no caller was scanned as ignoring
  (L-143: per function, no blanket rule). 0x0053d1c0 is BYTE-IDENTICAL but still conditional on ECX.
- **Conditional with a reading or unknown caller, 3 rows** (`callers_conditional.txt`): 0x004cd500 (0/2 ignore, both unknown),
  0x004d4910 (1/3, two unknown), 0x004d7100 (9/15, reads and unknown present).
- **Cross-check inconclusive, 1 row:** 0x00534990 (EQUIV [LAYOUT,VALUE-FLOW]) 0 conclusive runs in both modes, under the 8-run gate.

## Decision note (not run)
To admit the 16 pointer-only rows, a caller-side tool is needed (L-143 says none exists). Lead may ask: enumerate the plugin
and module callback tables that hold these RVAs and scan the indirect call sites. Seed builder for 0x00534990 would lift it.

Files: `in.tsv`, `run_equiv.sh`, `equiv64.log`, `verdicts.tsv`, `admit_in.tsv`, `rerun_dyn.sh`, `dyn_*`, `callers_conditional.txt`.
