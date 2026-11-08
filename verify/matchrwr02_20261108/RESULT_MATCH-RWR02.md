# RESULT MATCH-RWR02 (MATCH-RWLIB-2 slice R02), HANDOFF

Status: **HANDOFF** (LIVING-PC reboot pending for the Windows token leak; the coordinator asked every session to stop).
Match-only: no build, no game, no gates, no tracker edits, no promotion.

This file replaces the earlier sonnet RESULT (permuter-only pass, 0/79). `results.tsv` in this folder is still that
earlier worker's table and has to be regenerated on resume (`collect.py`, see below). `relocheck.tsv` holds the rows
at 100% so far.

## Counts so far

- Rows at 100% byte match with relocheck OK: **2 / 79** (before 0 / 79).
- Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 via `re/tools/match/match.py`, file flags `/O2 /Ob1 /G5`.
- Both files: plain C, no `__asm`, no naked function.

| rva | size | before | after | what moved it |
|---|---|---|---|---|
| 0x004e67b0 | 201 | 97.1 | 100 | the +0x64 list head init (`[+0x64] = [+0x68] = head`) written after the +0x6c store, so `push 0x618664` is scheduled before the stores as in the original |
| 0x004e8940 | 82 | 92.3 | 100 | one `return param_1` at the end with if/else stores; the compiler duplicates the tail and keeps param_1 in ebx from entry, as the original does |

## Rows worked and left below 100% (do not repeat)

- **Shrink-wrap class (callee-saved push at entry in the original, ours sinks it into the if region):**
  0x004e6920, 0x004e7cc0, 0x004e6470 (round 1 already named them), and the same root in 0x004f3be0 (edi) and
  0x0052fdb0 (esi). On 0x004e6920, none of these moved it: dead `= 0` init at top, block-scoped locals, `&&` chain,
  three goto shapes breaking the if region, `do { } while (0)` around the region or the whole body. Flag sweep
  (path conversion off, `MSYS_NO_PATHCONV=1`): `/G6`, `/GB`, `/Ox`, `/Ob2`, `/Op`, `/Oa`, `/Ow`, `/Ge`, `/Og /Ot /Oy`
  all give 90.9% or lower. **Git Bash gotcha:** `--flags "/O2 ..."` without `MSYS_NO_PATHCONV=1` is mangled into
  paths and every score reads 28.8%.
- 0x0052fdb0 (96.2): `xor esi,esi` placement. Tried 14 shapes (counter init inside/outside the null test, for vs
  if+do, goto, pointer base before/after). Best equal to before.
- 0x004f3be0 (95.6): `lea edi` before the `jle`. Moving the size computation into the slow path sinks the push edi
  (88.2%); the RW `_rpMaterialListAppendMaterial` shape with one shared tail (18 variants) is 55-63%; if/else orders 75-88%.
- 0x004e7e50 / 0x004e98d0 / 0x004e9910 (88.0 each, same function shape): the original returns the callee's eax on
  each failed stream write with its own `pop esi; ret` and no `xor eax,eax`; ours folds `r == 0` to a constant and
  shares one return block. 14 return shapes tried. Same class as R01's 0x004e5d30.

## Next on resume

1. Regenerate `results.tsv` + `relocheck.tsv` over all 79 rows (adapt `D:/MashedWT/MATCH-RWR02_data/tools/collect.py`,
   a copy of R01's with paths changed; its NEW dict needs the two rows above).
2. Continue in R02.tsv order from 0x00538310 (95.5), 0x004e8750 (91.1), 0x004ebc30, 0x004f2e70, 0x0052fe90,
   0x004f2f90, 0x005327d0, 0x004ebb00, 0x004f0ae0 (ops 100%, esi/edi naming only), then the 9 no-source rows.
3. Tool: `D:/MashedWT/MATCH-RWR02_data/tools/vt.py <src> <0xaddr> <spec.py> [--show NAME | --apply NAME]`; spec holds
   `VARIANTS = {name: [(old, new), ...]}` and the tool runs relocheck automatically on any 100% variant.

## Raw (not committed)

D:/MashedWT/MATCH-RWR02_data/scratch/ (variant specs and sources), D:/MashedWT/MATCH-RWR02_data/tools/.

## Files

- `re/match/rwl02/004e67b0.c` sha256 ba8f83d95bc965b2b6aefb0b12a4c1de3d155bac63c311a944b6da493e5c0444
- `re/match/rwl02/004e8940.c` sha256 f2eefa66758c38daa226eddaa08cf74e399256e29025cac5982c1fcec01e7ae8
