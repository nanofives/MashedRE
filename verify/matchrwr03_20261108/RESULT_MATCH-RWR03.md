# RESULT MATCH-RWR03

rows 100%: 0 / 79 (before 0 / 79). No source file changed.

Worked: 0x00549900 only (94.9%). The residue is `fisub dword [esp]` on a zero-extended stack temp for (float)b.
Tried b as `int` in the struct: 84.2%, worse (the compiler then emits fisub from the param directly in part, and keeps the extra temp stores).
Not attempted: the other 78 rows, including the 9 rows without compiling source (0x005422c0 has no file). Session stopped early on budget.

results.tsv lists every row at its round-1 pct (after = before). relocheck.tsv is empty (no row at 100%).
Follow-up: 0x00549900, 0x00540340, 0x00550580, 0x0054b630 are the nearest (>=93%).
