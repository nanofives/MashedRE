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

## Continuation (third pass, takeover worker)
Still 0/79 at 100% (before 0/79). No source file changed, no row improved.
Source attempts (scratch in D:/MashedWT/MATCH-RWR03_data, all scored with match.py, DDK, /O2 /Ob1 /G5):
- 0x0053e730 (85.7%): 6 shapes (if(p){}, if(p!=0){..return;}return;, if/else, typed struct {int;short;short}, int return, p==0 early return). All 85.7%. The compiler normalizes each to `je end`; the original has `jne L; ret; L:` (separate ret). Source shape does not reach it.
- 0x00543b90 (60.0%): `DAT + a`, `a - (-DAT)`: both 60.0%. Operand order stays `add ecx,eax` (original `add eax,ecx`).
- 0x0053e760 (86.7%): same early-return class as 0x0053e730, not retried separately.
Not attempted: the other rows (session stopped far short of the 30-row quota; budget spent on diagnosis).
Follow-up: the early-return class (0x0053e730, 0x0053e760, 0x00538600, 0x0053eaa0, 0x00544d20) shares one cause: original keeps `jne; ret` / unmerged exits. A permuter over declaration order or a `goto`-based form may be the only lever.
