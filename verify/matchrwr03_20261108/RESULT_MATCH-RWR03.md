# RESULT MATCH-RWR03

rows 100%: 0 / 79 (before 0 / 79). No source file changed.

Worked: 0x00549900 only (94.9%). The residue is `fisub dword [esp]` on a zero-extended stack temp for (float)b.
Tried b as `int` in the struct: 84.2%, worse (the compiler then emits fisub from the param directly in part, and keeps the extra temp stores).
Not attempted: the other 78 rows, including the 9 rows without compiling source (0x005422c0 has no file). Session stopped early on budget.

results.tsv lists every row at its round-1 pct (after = before). relocheck.tsv is empty (no row at 100%).
Follow-up: 0x00549900, 0x00540340, 0x00550580, 0x0054b630 are the nearest (>=93%).

## Continuation (second pass)
Bounded permuter (3 rounds, walk 40, 270-350 compiles each) on the four nearest rows: no improvement.
0x00549900 94.9, 0x00540340 93.7, 0x00550580 93.5, 0x0054b630 93.0 (all unchanged, before = after).
0x00550580: `return 0` on the failed-callback path and a restructured early-return form (66.7%, worse) did not remove `xor esi,esi; mov eax,esi`. Sources reverted.
Still 0/79 at 100%. Rows below 93% and the 9 no-source rows were not worked.
