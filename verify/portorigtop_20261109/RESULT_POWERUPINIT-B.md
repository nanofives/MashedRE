# RESULT POWERUPINIT-B (2026-11-10): 10 of 12 rows C4 by match, 2 parked with scores

L2 worker `cmwb3bps015lwsc1bjg9b2bc5` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/POWERUPINIT-B` on
`orch/integration` `05ea6cb1e`. MATCH work only. No `mashedmod/` file changed, so no build and no gates ran. The swap
of the nine `DAT_005f9998[i].init` (+0x14) stand-ins is row POWERUPINIT-C.

## Scope and outcome

Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), C, `// FLAGS: /O2 /GS /Ob1` on every source.
Command per row: `py -3.12 re/tools/match/match.py re/match/s29/<rva>.c --bytes`, then
`py -3.12 re/tools/match/relocheck.py re/match/s29/<rva>.c`. No source has inline `__asm` or `__declspec(naked)`.

| rva | role | before | best | relocheck | now |
|---|---|---|---|---|---|
| 0x00452f70 | init, slot 8 (+0x14 at 0x005f9bac) | no row | 100.0%, 311 B byte-identical | OK 22/0 | C4 |
| 0x00454e70 | init, slot 6 (0x005f9b2c) | no row | 100.0%, 250 B | OK 22/0 | C4 |
| 0x00455400 | init, slot 2 (0x005f9a2c) | no row | **94.0%** (regs 97.0%, ops 98.5%), 131/133 ins | BAD (follows from the residual) | C1 |
| 0x004576f0 | init, slot 7 (0x005f9b6c) | no row | 100.0%, 179 B | OK 21/0 | C4 |
| 0x004583a0 | init, slot 3 (0x005f9a6c) | no row | 100.0%, 293 B | OK 32/0 | C4 |
| 0x0045a6b0 | init, slot 4 (0x005f9aac) | no row | 100.0%, 213 B | OK 16/0 | C4 |
| 0x0045b830 | init, slot 5 (0x005f9aec) | no row | 100.0%, 237 B | OK 17/0 | C4 |
| 0x00454e40 | tail target of 0x00454e70 / 0x00454f70 | no row | 100.0%, 48 B | OK 6/0 | C4 |
| 0x0045b810 | tail target of 0x0045b830 / 0x0045b920 | no row | 100.0%, 24 B | OK 3/0 | C4 |
| 0x004b3fc0 | atom collector, called by 8 of the 9 inits | C2 | 100.0%, 55 B | OK 2/0 | C4 |
| 0x004b5560 | RpClumpForAllAtomics callback of 0x004b5580 | no row | 100.0%, 23 B | OK 1/0 | C4 |
| 0x0045a3a0 | callee of 0x0045a6b0 | C2, re/match/s09/0045a3a0.c | **81.0%** (regs 90.5%), unchanged | n/a | C2 |

`c4_reclass.py batch --tsv <10 rows> --apply --result` this file: 10 PASS, 0 refused.

Slots 0 and 1 (0x00456760, 0x004548e0) were already C4 in s09. With this row all nine +0x14 inits except slot 2
(0x00455400) have a byte-identical source, and so do both tail targets and 0x004b3fc0.

## Tracker writes (re-classify)

- `re/console/pc_funcs.csv` +9 rows: the seven inits and the two tail targets. Ghidra has no function at any of them
  (`decomp_pc.py` reports "instruction (disassembled, but not inside any function)", xref DATA from the table only).
  Extents come from a control-flow walk of MASHED.exe.unpatched that stops at `ret` and at jumps out of the body
  (the two tail jumps). This is the MATCH-S19-FU-PCFUNCS precedent. Nothing was written to the master Ghidra project.
- `hooks.csv` +10 rows at C1 (the nine above plus 0x004b5560, which is in pc_funcs.csv but had no row). C1 evidence:
  the table xref or the call/jump site, plus the strings, in each row's notes. Subsystem follows the siblings:
  `vehicle` for the seven inits (0x00456760 and 0x004548e0 are filed `vehicle`), `render` for 0x00454e40 (its
  callees 0x00454bd0 / 0x00454c00) and 0x004b5560 (its callee 0x004b5260), `gameplay` for 0x0045b810 (callee
  0x0045b360). Then C1 -> C4 for nine of them and C2 -> C4 for 0x004b3fc0 by `c4_reclass.py`.
- `re/analysis/CHANGELOG.md`: 10 `(none)->C1` lines and 10 `->C4` lines.
- `scripts/backfill_exe_file.py` ran: 0 rows changed (no exe body exists for any of them, `exe_file` stays empty).
  `scripts/backfill_port.py` did not run: it needs `mashedmod/build/mashed_re.map`, and this worktree has no build. The
  `port` cell of the 10 new rows is empty. POWERUPINIT-C builds and runs it.

**Flag, not changed:** 0x004b3fc0 is filed `third-party-library[msvc-crt-main]`, status `waived-platform-crt`. That
filing is by address band only (0x004a0000..0x004b4000, "do NOT promote" band note). The function is game code: it
compiles byte-identical as plain C under the game flags `/O2 /GS /Ob1`, and it calls RpClumpForAllAtomics 0x004e66d0
with the game callback 0x004b3f20. As filed, it counts out of the P-DoD port count (L-67). `c4_reclass.py` keeps the
status by design. Refiling it to a game subsystem is a tracker decision for the lead. POWERUPINIT-C calls it, so
it decides whether the port count includes it.

## Sources (re/match/s29)

Each file holds one `// MATCH:` function. The static callees that need register passing are copied in without a
MATCH line, from the s09 file where each is C4 (0x00454bd0 ESI, 0x00454c00 EAX, 0x0045b360 ESI, 0x00456d00 EAX).

- **0x00454e40 / 0x0045b810**: the stand-in callers already written in s09 (`standin_00454e40`, `FUN_0045b810`) are
  the originals. 0x0045b810 needed the signed `(int)` pointer compare (`jl`, the s09 stand-in gave `jb`).
- **0x004b3fc0**: the count and the context {out, &count} are one 12-byte local struct, count first. With two
  separate locals cl put the count in the dead param_2 slot (33.3%).
- **0x004b5560**: first form.
- **0x004576f0**: the record loop is an index loop `DAT_0068a250[i]`. A pointer loop gave 94.7%, because the strength
  reduced pointer sat on the record base instead of +0xc. The address-of in `0x00456d00(&DAT[i])` puts it on +0xc.
- **0x0045a6b0**: `i = 4; do { i--; r = &DAT_0068bb60[i]; ... } while (i != 0)`, the same counter shape as
  0x0045a3a0. A pointer do-while got a separate down-counter from cl.
- **0x0045b830**: the {4.0f, 4.0f, 4.0f} scale is a **block-scope initializer** inside the case. Assigning the three
  fields put their stores before the argument pushes (94.8%). The part-code test is a `switch` (case 0 / case 1,2),
  because the if/else-if form gave `cmp 3; jge` instead of `cmp 2; jg`.
- **0x00454e70**: first form (switch, a {0, 0, 0.5f, 1.0f} initializer at function scope, pointer loop, tail call).
- **0x004583a0**: the dropper loop is an index loop (pointer form 96.8%), and the mine loop is a pointer loop.
- **0x00452f70**: `for (i...) { l = &DAT_00684e3c[i]; l->... }`. Plain `DAT[i].f` gave a byte-offset IV with a
  displacement (84.5%). A pointer loop gave an added down-counter (76.3%). Six statement orders made no change.

New lever (not in memory ddk-match-levers yet): when cl must keep a strength-reduced pointer on a field and
compare it against the end, write an index loop that takes `p = &ARR[i]` at the top and uses `p->` throughout.
Neither the bare index nor the bare pointer loop gives that code.

Record bases in 0x00452f70 and 0x00455400 are a source choice that makes the two arrays of each function adjacent.
The code reads only fields (the 0x00452f70 launcher fields at 0x00684e40..0x00684e50, its round fields at
0x00684eac..0x00684ec0 + i*0x110). Moving a base moves only the masked displacements.

## Parked

### 0x00455400: 94.0%, residual = IV reuse plus IV order (not a SIB base/index-order residual)

Best source `re/match/s29/00455400.c`. The other 131 instructions are identical. Two differences:
1. After the second inner loop, the original stores +0x20 / +0x48 / +0x58 of the missile record through a fresh
   `imul eax, [esp+0x14], 0x6c` from the spilled counter (0x00455581..0x00455596). Ours reuses the
   strength-reduced offset `edi` (`mov [edi+disp], ebp`), so the loop test sits before the stores, not after
   them. relocheck BAD follows from this: the three stores' relocated displacements move.
2. The two IV decrements are in the order `sub edi,0x6c; sub esi,0x2c` in the original and reversed in ours.

Tried, none moved the score: a second array symbol for the trailing stores (88.7%), `m = &ARR[i]` pointers at top /
middle / trailing only (88.7 / 88.7 / 94.0%), a launcher pointer (84.2%), char-offset expressions with and without a
named offset (94.0%), a second counter j for the missile records (cl merges it with i, 94.0%), `while (i-- != 0)`,
`while (i) { i--; }` (94.0%), `for (i = 4; i >= 0; i--)` (83.6%), trailing stores moved first (85.0%), and
`match_permute.py --rounds 3 --walk 40` (450 compiles, no improvement). Not a SIB base/index order, so the L-115 knob
path does not apply. Next try: a separate inlined helper that receives the counter by value, or a source where the
counter's address escapes before the loop.

### 0x0045a3a0: 81.0%, residual = register allocation (param_1 in EBP from entry)

Source unchanged (`re/match/s09/0045a3a0.c`, its header lists the earlier attempts). The original loads param_1 into
EBP at entry (`push ebp; mov ebp,[esp+0xc]`) and stores through it after the loop. Ours reloads it into EAX after the
loop. Tried this session: an `__inline` / `/Ob2` static helper holding the loop (9.5%, not inlined), a float-typed
param struct, a local copy, the `register` keyword, a zero local, a block-pointer outer loop (71.4%). None moved it.
Not a SIB residual. 0x0045a6b0 calls it with a pushed argument, so its being in another file does not change the
caller's match.

## Checks

- `py -3.12 scripts/lint_rva_bodies.py` after the tracker writes: `OK -- no NEW duplicate bodies` (34 known, NEW=0).
- `c4_reclass.py batch --apply --strict`: 10 pass, 0 refused, wrote 10 hooks.csv rows and 10 CHANGELOG lines.
- No `mashedmod/` change: build and default gates (G-CTRL 8284be46/67931834/31fe043e, B 1d5bf4f9 x3) not run.
