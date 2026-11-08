# RESULT MATCH-RWR03 (opus takeover, HANDOFF: interrupted by a LIVING-PC reboot)

Status: HANDOFF, partial. Match-only: no build, no game, no gates, no tracker edits, no promotion.
This replaces the two sonnet passes (0/79). Their results.tsv in this folder is still the old one: regenerate it
with `D:/MashedWT/MATCH-RWR03_data/tools/collect.py` (R01's collect.py retargeted to R03.tsv) before review.

## Rows now at 100% with relocheck OK: 6 / 79 (before 0 / 79)

| rva | size | before | after | what moved it |
|---|---|---|---|---|
| 0x0053e730 | 48 | 85.7 | 100 | returns the allocated pointer (Ghidra typed it void); `if (!p) return 0; ... return p;` |
| 0x0053e760 | 44 | 86.7 | 100 | same: returns the allocated pointer |
| 0x00550580 | 237 | 93.5 | 100 | tail as `if (r != 1) { p->f20 = 1; p = 0; } else { p->f1c = 2; } return p;` (one result variable) |
| 0x0054b630 | 150 | 93.0 | 100 | new capacity kept in the dead param_2 (`param_2 = cap + 8`), which gives the original's spill slots |
| 0x0054d4d0 | 1785 | 90.5 | 100 | slice flags /O2 /Ob1 /G5; every read of param_1 goes through `volatile float *v = param_1` (the original reloads each stored float; without volatile c2 forwards them) |
| 0x0054dbd0 | 2379 | 78.9 | 100 | same volatile read alias; global typed float and renamed DAT_005cc31c (relocheck could not resolve `_DAT_`) |

Reviewer note on the two volatile rows: the qualifier is on reads only and changes no value in a single thread, no
`__asm`, no naked function. If the lead prefers not to accept a volatile alias as the source shape, hold those two.

`relocheck.tsv` lists these six (all OK; 0x00550580 has the two kernel32 imports unchecked by design).

## Improved, not 100%

- 0x00540340: 93.7 -> 99.4. The four D3D9 device vtable calls are `__stdcall` (no `add esp` after them). Residue:
  one `mov esi,[ebx]` (original) vs ours `[eax]` after `mov ebx,eax`. Tried 8 variable-identity shapes
  (nested ifs, gotos, piVar3-routed load) and match_permute (271 compiles, no gain). relocheck OK.

## Tried, unchanged

- 0x00549900 (94.9): residue is a c2 peephole: original `fild qword;...;fisub dword [esp]`, ours keeps `mov eax,1`
  between `fild qword` and `fsubp`, which blocks the fold. 9 expression/cast/local shapes, no move.
- 0x0054f410 (75.8): original copies most floats with `fld/fstp` and only lane 0 of each row with `mov`; volatile
  source, volatile dest and `(float)(double)` copies did not reproduce it.

## Next (for the resumed session)

1. Regenerate results.tsv (all 79 rows, plus before% column) with tools/collect.py.
2. Small rows: 0x00543b90 / 0x00543da0 (`add eax,ecx` order: route the global through its own named variable per
   R01 finding 1), 0x0053d040 (load order of the two params).
3. Then 0x0054c400 87.5, 0x00544d20 86.7, 0x00538600 84.8, 0x00540160 83.8, 0x00541b50 83.6, 0x0053eaa0 79.2
   (unmerged early returns: try returning the pointer / one result variable as above).

## Raw (not committed)

- `D:/MashedWT/MATCH-RWR03_data/permute_540340.log`, `scratch/` variant specs and sources, `tools/`.
