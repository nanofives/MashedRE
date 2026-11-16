# RESULT MATCH-43DFD0-RAISE-R3-ARMS (run 52, round 3, worker "arms")

Row MATCH-43DFD0-RAISE, round 3. Match-only (L-168): no build, no gate, no game run. The 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-RAISE-R3-ARMS` from `orch/integration` 810042e0e. One attempt.
Source `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`, C.
Final source sha256 `7821ba51d9cd1c7a026088a7416f6ac3b51abec75b00009e7d13b6f3f4aa3bd8`.

## Verdict

GREEN as a raise, not a match. One lever kept: the arm order of the 0xff1d0000 case. No byte-identical claim, so no relocheck
and no promotion TSV.

| Measure | before (810042e0e) | after (this branch) |
|---|---|---|
| match.py whole function | 49.8% (regs 67.0%, ops 88.6%), 2811/2817 ins | **51.1%** (regs 67.4%, ops 89.1%), 2810/2817 ins |
| instructions equal (`=` rows of `--show`) | 1402 | **1440** |
| `--bytes` | 11098 B whole symbol, 6419 differing unmasked bytes, first at +0x5f | 11082 B whole symbol, 6432 differing unmasked bytes, first at +0x5f |

The differing-byte count rises by 13 while the symbol shrinks by 16 B and 38 more instructions line up. As in rounds 1 and 2, the
offsets after the change shift, so score and equal count are the measures of fit. The first differing byte stays at +0x5f: the
0/1 register roles at 0x0043e02e / 0x0043e03b (worker REGS's area).

## Kept: 0xff1d0000 arm order (source lines 532-589)

At 0x0043e563 the original calls 0x00430760, then `test eax, eax; je 0x43e59e` at 0x0043e568 / 0x0043e56a. So the `!= 0` arm comes
first, at 0x0043e56c..0x0043e599 (`mov eax, [0x67f0c0]`, `cmp eax, ebx; jne 0x43e57d`, `mov [0x7f1a1c], ebx`, `dec eax`,
`FUN_0043d2a0(6, 0)` at 0x0043e586, `call 0x431d00` at 0x0043e58e, `mov [0x67ea78], ebx` at 0x0043e593). The `== 0` arm follows at
0x0043e59e (`call 0x42b9e0`). The source had Ghidra's `if (iVar8 == 0) { ... } else { ... }`, which our compile laid out as
`jne`, with the `== 0` arm first. Change: `if (iVar8 != 0) { <old else arm> } else { <old then arm> }`, both arms moved verbatim.
Our compile now emits `call; test eax, eax; je` and the `!= 0` arm first. That arm matches the original instruction for instruction
apart from the 0 register (edi in ours, ebx in the original).

## Tried, not kept (all identical listings at 51.1% / 1440 equal on top of the kept change)

| Lever | Target in the original | Result |
|---|---|---|
| ff1d `iVar8 == 1` arm passes `iVar8` in place of the literal 1 | `push eax` at 0x0043e62d (the other three arms push the literal `1`) | identical: c2 propagates the constant and pushes its 1 register (ebx) |
| dispatch preamble test as `*(int *)((char *)&DAT_0067ed80 + DAT_0067e9f8 * 0x40) == 0` | 0x0043e4e0..0x0043e4ef | identical |
| dispatch preamble as nested `if (e7f0 == 1 \|\| e7f0 == 2) { if (ed80 == 0) store; } else store;` | 0x0043e4d1..0x0043e4f1 | identical |

## Residual in this area

- **Dispatch preamble keeps `DAT_0067e9f8` live in eax.** Ours (offsets in our symbol): `mov eax, [0x67e9f8]; mov ecx, eax; shl ecx, 6`
  at +0x50b, and on the `e7f0 != 1 && e7f0 != 2` path a second `mov eax, [0x67e9f8]` at +0x51f before the `1.0` store, plus a
  `jmp +0x524` to share the store. The value stays in eax through the uVar10 tree, and two cases use it with no reload: `default`
  (ours +0x1b9c `shl eax, 6`) and `0xff4c0000` (ours +0x19dc). The original loads straight into ecx (`mov ecx, [0x67e9f8]; shl ecx, 6`
  at 0x0043e4e0), has no load on the other path, and reloads in both cases (`mov eax, [0x67e9f8]` at 0x0043fc37 and 0x0043fa18).
  This costs ours 3 extra instructions in the preamble. Three preamble forms (above, and the three DISPATCH tried in round 2) give
  the same listing. The cause sits in the readers (the default and ff4c bodies, source lines ~1373 and ~1447), outside this worker's
  original range 0x0043e43d..0x0043e8ff. Not tried, area.
- The 0x1000 arm (0x0043e5aa..0x0043e5de) loads a local 1 (`mov esi, 1` at 0x0043e5ac) for its stores and pushes. Ours uses its
  function-wide 1 register (ebx). Same family as the 0/1 register roles (REGS).
- `mov dword ptr [0x67f19c], 1` at 0x0043e4c0 and `mov dword ptr [0x67e914], 1` at 0x0043e689 are literals in the original and
  `ebx` in ours. Same family.

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --show --bytes` before and after (table above).
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022 vcvars32, syntax only, no object, no link)
  in `mashedmod/src/mashed_re/Match`: exit 0.
- Diff: `re/match/s08/0043dfd0.c` lines 532-589 only (one hunk pair: the arm swap). Declarations, prologue, header line 2 and the five
  team-slot loops are untouched, so the branch merges with REGS and LOOPS. Header score left for the lead.
- Scratch tools (session scratchpad, not committed): `odis.py` (original listing from `original/MASHED.exe.unpatched`, read only),
  `ours.py` (our compiled symbol with relocation names), `v.py` / `m.py` (variant runner, score + equal count), `eaxuse.py`.

## [UNCERTAIN]

[UNCERTAIN] The arm swap keeps behaviour. This rests on reading the source: the condition is negated and the two arms are moved
verbatim, and both still end in the case's `break`. Test: `re/tools/match/equiv.py` on this source against 810042e0e's, or the
PROVISIONAL knob-path menu walk ON against OFF. Not run here (match-only row, L-168). One line in
`re/orchestrator/UNCERTAIN_REGISTER.md`.
