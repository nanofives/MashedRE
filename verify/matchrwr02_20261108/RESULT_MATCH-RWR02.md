# RESULT MATCH-RWR02 (2026-11-08)

Slice R02 (0x004e5fc0..0x00538310, 79 rows). Match-only, static.

**Verdict: HONEST PARTIAL, 0 of 79 rows reach 100% (before 0/79).** No source changed, nothing to promote.

What ran:
- Bounded permuter (`match_permute.py`, 2 rounds, walk 30) on the 17 rows at >=87%. Raw log `D:/MashedWT/MATCH-RWR02_data/perm.log`.
- Hand try on 0x004e67b0 (move the +0x68 store after `*p1 = p1`): 95.6%, worse than 97.1%, reverted.
- Permuter found no 100%. Higher non-semantic-checked variants (not committed): 0x00538310 96.4, 0x004e8750 95.5, 0x004f2e70 93.0. Variants in `log/permute/` of the worktree, unreviewed for semantics.

Per-row before/after: `results.tsv`. Rows below 87% and the 9 no-source rows (0x0052df20, 0x00536780, 0x00536e90, 0x00536fe0, 0x00537b20, 0x00537d20, 0x00537d70, 0x00537e60, 0x005422c0) were not attempted: no budget.

Residuals unchanged from round 1 (scheduling slot, push order, callee-saved choice). 0x004e67b0: orig pushes `0x618664` early and stores +0x68 after `[eax],eax`.
Follow-up: hand work on the 0x004e67b0 / 0x0052fdb0 / 0x004f3be0 / 0x00538310 push/lea schedule rows, and writing sources for the 9 no-file rows.
