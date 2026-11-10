# RESULT MATCH-X87-CASTLEVER-OPORDER, 2026-11-10

Follow-up to MATCH-X87-CASTLEVER-PILOT (lead MATCH-RACEPATH). Branch `orch/MATCH-X87-CASTLEVER-OPORDER` from the pilot head
60d30387c. Rows: the 6 operand-order rows the pilot left in the pool (`../pilot/RESULT_MATCH-X87-CASTLEVER-PILOT.md`, "Rest of the pool"),
smallest residue first. Yield rule from the lead: stop after 4 rows worked if fewer than 2 have closed.

## Yield: 1 / 4 worked closed, stopped by the yield rule

| rva | before B | after B | verdict | what moved it |
|---|---|---|---|---|
| 0x00557110 | 12 | 0 | CLOSED, C4 by match | lever 4: `DAT_005d757c` -> `0.0f` literal (0x005d757c is `.rdata` `00000000`) |
| 0x0041e6c0 | 15 | 15 | NOT-CLOSED | nothing (schedule: the last fstp vs 3 arg pushes) |
| 0x00453730 | 24 | 16 | NOT-CLOSED, partial committed | lever 1 on line.end.y + named float local on hit.v0.x * 0.05f |
| 0x00405ab0 | 42 | 33 | NOT-CLOSED, partial committed | lever 4 comma on t.x, lever 1 on w.y, lever 2 on d.y |
| 0x0045a190 | 64 | 64 | NOT-WORKED | yield stop (greedy alone 49 B, not committed) |
| 0x00457100 | 100 | 100 | NOT-WORKED | yield stop (greedy alone 99 B, not committed) |

Detail per row, variant counts and hashes: `results.tsv`. Variant specs: `variants/v_<rva>.py` (run with `tools/tryv.py`). Greedy
log: `dblgreedy.log`.

## What the residues were

On reading, the pilot's "operand-order look" did not hold for these rows: the first diffs listed in `pool_classified.tsv` showed
only a few lines. With the full diff, 0x0041e6c0, 0x00453730 (after the greedy) and 0x00405ab0 (after the clean levers) are
schedule residue: an x87 op sinks or rises one slot past a push, pop, lea or `add esp`. The pilot closed 0 / 5 of that class, and
this slice 0 / 3. 0x00557110 looked like schedule too, but the constant's form was the cause: once the compare constant is a
`__real@` literal instead of an extern global, cl schedules the `fld` / `fcomp` pair where the original has it.

## Findings for the levers list

1. Lever 4 (`_DAT_` floats that are `__real@` literals) also applies to `MGLOBAL(float, DAT_005d757c, ...)` / `extern const float
   DAT_005d757c`, not only to the `_DAT_005c....` externs, and it moves scheduling as well as the operand encoding. Check the
   binary first: read the 4 bytes in `original/MASHED.exe.unpatched` and confirm the section is `.rdata` (initialised). 0x00405920.c
   uses DAT_005d757c in 6 places, and the literal gave no gain on 0x00405ab0, so the effect is per function.
2. Parentheses around a call operand change cl 13's code: `t.x = (0, t.x) + (FUN_00472650(-0.2f, 0.2f));` is 3 B closer than the
   same line without the parentheses (`variants/v_00405ab0_iso.py`). Comments change nothing. Add a parenthesised-operand form to the search.
3. False gain guard held: greedy round 0 on 0x00405ab0 wrote `(double)(0.023f)`, which compiles to `fmul qword`. It was thrown out.
   The committed partial has no qword operand (checked with `match.py --show`).

Tool change (scratch copy only, `tools/dblgreedy.py`): the operand regex now also takes `*(T *)(base + off)`, the form the pilot
found missing. No row here closed through it.

## Checks

- 0x00557110: `match.py --bytes` BYTE-IDENTICAL 169 B, `relocheck.py` OK 4/0 (`relocheck.tsv`), end 0x005571b9 <= next pc_funcs entry
  0x005571c0. Single `// MATCH:` in its file. Gate: `c4_reclass.py batch --tsv c4_batch.tsv` 1 pass (`c4_dryrun.txt`), then `--apply --result` (route match).
- Files with other MATCH rows (`baseline_files.txt` before, re-run after): `s09/004538b0.c` 0x00453100 / 0x00453210 / 0x004532f0
  still byte-identical, 0x004538b0 87.4% as before. `s01/00405920.c` 0x00405f60 / 0x00406070 / 0x00406410 still byte-identical,
  0x00405920 7 B and 0x00406ae0 91.8% as before. `s04/0041e6c0.c` was not edited.
- No file under `mashedmod/src` includes any of the six sources (`grep -rln`), so the port side is unchanged.
- Semantics: the 0.0f literal is the same constant. `(0, x)` yields x. A double intermediate or a named double of a float
  expression narrows back to the same float on x87 (the original evaluates in extended precision too). The named float local of a
  float product matches the original's bytes in that region.

## Pool after this slice

The x87 pool left after the pilot is now 51 parked rows + 0x0045a190 + 0x00457100 (both not worked). Predicted yield for another
pass: under 10%. 3 of the 4 rows worked here turned out to be schedule residue once the full diff was read, and across both
slices the levers closed none of the schedule class (0 / 8). Recommendation: park the rest of MATCH-X87-CASTLEVER. If it is picked up again,
first screen the whole pool for `MGLOBAL` / `extern const float` constants that are `.rdata` literals (finding 1), a mechanical
pass that needs no hand work.
