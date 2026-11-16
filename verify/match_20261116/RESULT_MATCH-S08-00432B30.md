# RESULT MATCH-S08-00432B30 (run 51, 2026-11-16)

Row kind: match-only (no builds, no gates, no runtime; 0x0043dfd0 not run, L-168).
Source: `re/match/s08/00432b30.c` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`).

## Verdict: RED (raised, not byte-identical)

| | before | after |
|---|---|---|
| match.py 0x00432b30 | 74.2% (regs 74.2%, ops 89.1%), 343/357 ins | **93.6%** (regs 93.6%, ops 97.5%), 358/357 ins |
| `--bytes` | 1580 B symbol, 955 differing unmasked bytes | 1648 B symbol, 682 differing unmasked bytes (first at +0x21) |
| relocheck 0x00432b30 | BAD | BAD, checked=72 (every site after +0x48 is shifted by 2 bytes, residual (a)) |
| 0x0042ad10 / 0x0042add0 | 100% / 100% | 100% / 100%, both `--bytes` BYTE-IDENTICAL, relocheck OK (checked=9 / 2) |

## Levers that moved it

1. **Callee parameter order.** `FUN_0042ad10(int type, int *cnt, int x, int y, int z)` in place of `(cnt, type, ...)`.
   The caller now loads `mov eax, esi` before `mov edx, type`, as the original does at 0x00432bab / 0x00432bad.
   The callee keeps EAX = cnt, EDX = type and stays 100% byte-identical. 85.2% -> 87.2%.
2. **Case order = the original's layout order, inner and outer.** Outer cases 1,2,4,8,5,6,10 (block order of the
   targets in the outer table at 0x00433060) and each inner switch in the same order with its self case first.
   The old note's 41.1% for that outer order came from changing the outer order alone. 74.2% -> 85.2%.
3. **Local search over inner case orders** (one label moved per step, scored with match.py): in4 = 4,2,1,8,5,6,10
   and in10 = 10,2,4,8,5,6,1. 87.2% -> 93.6%. The users of each jump-table target now group exactly as in the
   original's eight tables at 0x00433060..0x004331a0 (same 33 shared/unshared block sets).

## Residual (two items)

- (a) One extra `xor edi, edi` on the mode 0 path (ours, before the `mov ebx, 0xff080000` that the original has at
  0x00432b78). It shifts every later address by 2 bytes, which is most of the 682 bytes and the relocheck BAD.
  Passing `r`, `0` or `mode` as the idx argument at each of the three FUN_0042add0 call sites (0x00432b49,
  0x00432b58, 0x00432b7d), an initialiser, a ternary form, a nested `else { if }` and a second zero variable all
  left it unchanged.
- (b) The blocks shared by the inner switches of sel 1 and sel 6 for cases 2 and 4 (s2 = 0x43 at 0x00432c22,
  s2 = 0x225 at 0x00432c36) sit in region 1 in the original and after HEAD6 in ours. All 5040 orders of the sel 6
  inner switch, and all 5040 of the sel 1 inner switch (each with the other fixed), left them in place.
  A joint search of the two was started and stopped at the lead's instruction.

## Drop rule / provisional

`re/match/PROVISIONAL.tsv` has no row for 0x00432b30 (`grep -i 432b30` empty), and no TU in
`mashedmod/exe_sources.rsp` includes `re/match/s08/00432b30.c`. The function body is copied into
`re/match/s08/0043d2a0.c` (RO-MENU-DIV-43DFD0); that copy was NOT updated here, since 0043d2a0.c is an exe-path unit
and this row is match-only. `py -3.12 re/tools/match/prov_drop.py` (dry run, this branch): PROVISIONAL.tsv 930 rows,
DROP 0; PROVISIONAL_RWC.tsv 990 rows, DROP 103 (all other rows, none touched here). 0x00432b30 is not listed. The
row tied to this work is `KEEP not-byte-identical 0x0043dfd0 provisional re/match/s08/0043dfd0.c` (19.2%, 7009 of
11106 bytes differ), so a byte match of 0x00432b30 alone would not drop it. Not byte-identical, so no
`re/promotions/` TSV. No [UNCERTAIN] claims (every number above is a match.py / relocheck / prov_drop output).

## Next

Close (a) first (it is a length change at +0x48 and fixes most bytes), then a joint in1 x in6 order search for (b).
Scratch tools used (not committed): a source generator over outer / inner case orders and a jump-table block
sequence comparer.
