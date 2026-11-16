# RESULT MATCH-43DFD0-DISPATCH (round 2, worker "dispatch")

Row MATCH-43DFD0-RAISE, round 2. Match-only (L-168): no build, no gate, no game run. The 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-DISPATCH` from `orch/MATCH-43DFD0-FRAME` 0048e45cb. One attempt.
Source `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`, C.

## Verdict

GREEN as a raise, not a match. Area (b) raised the score. Area (a) did not: no lever moved the 0/1 register roles, so none was
kept. No byte-identical claim, so no relocheck and no promotion TSV.

| Measure | before (0048e45cb) | after (this branch) |
|---|---|---|
| match.py whole function | 45.9% (regs 57.9%, ops 81.1%), 2811/2817 ins | **48.6%** (regs 66.5%, ops 87.9%), 2800/2817 ins |
| instructions equal (difflib matching blocks, the `=` rows of `--show`) | 1294 | **1368** |
| `--bytes` | 11046 B whole symbol, 6421 differing unmasked bytes, first at +0x5f | 11030 B whole symbol, 6402 differing unmasked bytes, **first at +0x5f** |

The first differing byte stays at +0x5f: that is area (a), `xor edi, edi` in ours against `xor ebx, ebx` at 0x0043e02e.

## (b) The uVar10 dispatch is a `switch` (kept)

At 0x0043e4fb the original tests `cmp edi, ebp; je 0x43f6d4` (uVar10 == -1, ebp = -1 from 0x0043e4a0), then runs a binary decision
tree from 0x0043e503: `cmp edi, 0xff400000; ja 0x43f4e0; je 0x43f468`, and so on. Every subtree is one `cmp imm; ja; je` chunk,
and each chunk's last leaf is `cmp imm; jne 0x43fc37` falling into that case's body. That is the DDK `switch` tree. The source had
Ghidra's nested `if (uVar10 < 0xff400001) { if (uVar10 == 0xff400000)`, which compiles to `cmp 0xff400001; jae; cmp; je`.

Case to body address, read from the tree chunks (0x0043e503, 0x0043e6b3, 0x0043e833, 0x0043f18c, 0x0043f274, 0x0043f286,
0x0043f2e0, 0x0043f308, 0x0043f45c, 0x0043f4e0, 0x0043f4f2, 0x0043f504, 0x0043f90a, 0x0043f948, 0x0043fa0c, 0x0043fb04,
0x0043fb16, 0x0043fb64, 0x0043fbad, 0x0043fc2f):

| case | body | case | body | case | body |
|---|---|---|---|---|---|
| 0xff150000 | 0x0043e67e | 0xff310000 | 0x0043f2a4 | 0xff480000 | 0x0043f938 |
| 0xff1d0000 | 0x0043e563 | 0xff360000 | 0x0043f2d0 | 0xff490000 | 0x0043f9a9 |
| 0xff1e0000 | 0x0043e698 | 0xff370000, 0xff390000 | 0x0043f6d4 (break) | 0xff4a0000 | 0x0043f96e |
| 0xff1f0000 | 0x0043e6de | 0xff380000 | 0x0043f2f8 | 0xff4b0000 | 0x0043f9d7 |
| 0xff200000 | 0x0043e6c7 | 0xff3a0000 | 0x0043f36b | 0xff4c0000 | 0x0043fa18 |
| 0xff240000 | 0x0043e6f9 | 0xff3b0000 | 0x0043f32e | 0xff4d0000 | 0x0043fa9e |
| 0xff260000 | 0x0043e85e | 0xff3c0000 | 0x0043f396 | 0xff4e0000 | 0x0043fb39 |
| 0xff2c0000, 0xff2e0000 | 0x0043f198 | 0xff3d0000, 0xff400000 | 0x0043f468 | 0xff4f0000 | 0x0043fb34 (`jmp 0x43f6d4`) |
| 0xff710000 | 0x0043f1fd (tail of the ff2c/ff2e body) | 0xff420000 | 0x0043f53a | 0xff500000 | 0x0043fb54 |
| 0xff2d0000, 0xff2f0000 | 0x0043f21d | 0xff430000 | 0x0043f52a | 0xff720000 | 0x0043fb7c |
| 0xff300000 | 0x0043f2c4 (`jmp 0x43f2ae`, the ff31 loop) | 0xff440000 | 0x0043f900 | 0xff730000 | 0x0043fb9d |
| 0xff800000 | 0x0043fbe9 | 0xff450000 | 0x0043f928 | 0xff820000 | 0x0043fc07 |
| 0xff810000 | 0x0043fbcb | 0xff470000 | 0x0043f91e | 0xff830000 | 0x0043fc64 |
| default | 0x0043fc37 | | | | |

Change: base lines 522-1497 (`if (uVar10 != 0xffffffff) { if (uVar10 < 0xff400001) ...`) became
`if (uVar10 != 0xffffffff) { switch (uVar10) { ... } }`, one case per row above, with the bodies moved verbatim from the base lines.
Mechanical rules: every `goto LAB_0043fc37` is the `default:` (that label had no other source), `goto LAB_0043f468`,
`goto LAB_0043f21d` and `goto LAB_0043f198` are shared case labels, the ff2c/ff2e body falls into `case 0xff710000:` (it ended
`goto LAB_0043f1fd`, which was the ff71 body), ff30 and ff31 each set `DAT_0067ecdc`, run the `0x67eaf0` loop and keep
`goto LAB_0043f203`, and ff37 / ff39 / ff4f are `break`. `goto LAB_0043f371`, `goto LAB_0043f6d4` and the labels inside the ff26 and
ff42 bodies are unchanged. The ff42 body (stores worker's 0x0043f63e region) was moved, not edited.

Measured (whole function):

| Variant | score | equal |
|---|---|---|
| base (nested if) | 45.94% | 1294 |
| switch, cases in ascending value order (kept) | **48.56%** | **1368** |
| switch, cases in the original's body-address order (ff1d, ff15, ff1e, ff20, ff1f, ...) | 48.56%, identical listing | 1368 |
| switch, ff30 / ff31 / ff71 each with their own `FUN_0043d2a0(4,0); DAT_0067ea68 = 1;` tail, no goto | 48.56%, identical listing | 1368 |

Both case orders give the same listing, so for this tree the DDK places the case bodies itself (the chunk's fall-through leaf right
after the chunk, the rest by value). It does not depend on source order, unlike the jump-table switches RAISE-3 measured.
Our tree now emits the original's chunk sequence: `cmp esi, 0xff400000; ja; je; cmp esi, 0xff2f0000; ja; je; ...`. The only
difference is the register (esi against edi, area (a)).

Tried in the dispatch preamble (base line 518, the `DAT_0067e7f0` / `DAT_0067ed80` test before the tree), none changed the listing,
all 48.56%: the `!((a == 1 || a == 2) && b != 0)` form, `iVar8 = DAT_0067e7f0` hoisted, and `if (...) {} else`. Ours still has one
extra `jmp` and one reload there against 0x0043e4d1..0x0043e4f1.

## (a) 0 in ebx / 1 in edi (nothing kept)

Original: `xor ebx, ebx` at 0x0043e02e, `mov edi, 1` at 0x0043e03b, `mov esi, 2` at 0x0043e073, `push ebp; mov ebp, 4` at
0x0043e138, `or ebp, 0xffffffff` at 0x0043e4a0. uVar10 then takes edi (`mov edi, eax` at 0x0043e4cf), and after the scan every 1 is
a literal (`mov dword ptr [0x67f19c], 1` at 0x0043e4c0, `cmp eax, 1` at 0x0043e4d6, `mov eax, 1` at 0x0043e973). Ours: 0 in edi,
1 in ebx, 2 in esi, ebp the same. uVar10 takes esi, and ebx keeps 1 through the dispatch (`mov dword ptr [0x67f19c], ebx`, `cmp eax, ebx`,
`mov ecx, ebx` in the ff26 case 3-5 index). The original splits its 0 out of ebx at 0x0043e994 (inner `&DAT_005f65c8` word in ebx) and
rebuilds 0 locally (`xor esi, esi` at 0x0043e9b8 / 0x0043ea3a / 0x0043eabc, `xor ebx, ebx` at 0x0043f12d / 0x0043f185).

Levers measured on the switch source (each alone, whole function):

| Lever | top register roles | score |
|---|---|---|
| the two post-scan 1 uses written as 7 (diagnostic, changes behaviour) | unchanged (0 edi, 1 ebx, uVar10 esi) | 48.78% |
| 1, 2, 4, 8, 16 extra `= 0` stores at source line 324 (diagnostic) | unchanged | 45.97 / 46.25 / 45.97 / 47.71 / 46.40% |
| 40 extra `= 0` stores | unchanged | 44.79% |
| 100 extra `= 0` stores | 0 moves to ebp, 1 stays in ebx | 34.61% |
| the 9 loop-set 1s of the key scan written as 7 (diagnostic) | 1 no longer enregistered at the top, 0 stays in edi, ebx not pushed at the top | 47.92% |
| inner `&DAT_005f65c8` word in its own variable (`uVar22`) in the ff26 body | unchanged, identical listing | 48.56% |
| a named variable set to 1 at entry and used for the 10 top-region 1s | unchanged, identical listing | 48.56% |

So 0's register is not set by its use count: 100 extra uses never put 0 in ebx, and with no enregistered 1 at all, 0 still chooses
edi. Variable identity does not matter either (c2 works on webs after constant propagation). The choice follows from how c2 colours
and splits the function-wide webs (the 1 web reaching into the dispatch in ours, the 0 web split at 0x0043e994 in the original). No
local source construct for it was found in this attempt.

## Residual

- (a) as above: the first differing byte +0x5f and the register names through the dispatch (esi against edi for uVar10).
- In the ff26 team-slot loops our ebp/esi roles are swapped against the original (`mov ebp, 0x7f1a2c` and `lea esi, [eax + 1]` at
  0x0043eb4a / 0x0043eb56; ours builds `0 - 4` in esi and puts `eax + 1` in ebp). Same family as (a), RAISE-2's residual.
- ff1d body (0x0043e563): the original lays out the `FUN_00430760() != 0` arm first (`test eax, eax; je 0x43e59e` at
  0x0043e568). Not tried, budget.

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes` before and after (table above).
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022 vcvars32, syntax only, no object, no link) in
  `mashedmod/src/mashed_re/Match`: exit 0.
- Diff: `re/match/s08/0043dfd0.c` (base lines 522-1497 plus the header score on line 2), `re/orchestrator/UNCERTAIN_REGISTER.md`
  (one line), this file. The stores branch also edits line 2 and the switch at line ~1510. The lead merges.
- Scratch tools (session scratchpad, not committed): `gen.py` (builds the switch from the base line ranges, both case orders),
  `m.py` (score + equal-instruction count + listings), `odis.py` (original listing with addresses from `original/MASHED.exe.unpatched`,
  read only).

## [UNCERTAIN]

[UNCERTAIN] The switch rewrite keeps behaviour. This rests on reading the source: each case value reaches the body it reached through
the if-tree, `default` is the only former target of `goto LAB_0043fc37`, and the ff2c/ff2e fall-through replaces `goto LAB_0043f1fd`.
Test: `re/tools/match/equiv.py` on this source against 0048e45cb's, or the PROVISIONAL knob-path menu walk ON against OFF. Not run
here (match-only row, L-168). One line in `re/orchestrator/UNCERTAIN_REGISTER.md`.
