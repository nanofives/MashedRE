# RESULT MATCH-43DFD0-RAISE-R4-ROLES (round 4, register-role lead)

Row MATCH-43DFD0-RAISE, round 4. Match-only (L-168): no build, no gate, no game run. The 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-RAISE-R4-ROLES` from `orch/MATCH-43DFD0-RAISE-R3` cbd5dd521. One attempt.
Source `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`, C.
Final source sha256 `b9f1a0f89099fd1c57b73df9aa6c288db867c55a5783e2a67616f67e1f1024cb`.

## Verdict

AMBER, kept. A behaviour-preserving spelling moves the 0 / 1 roles at the top of the function to the original's
(0 in ebx, 1 in edi) and raises the score 51.54% -> 52.29%. It is not the 0xff4b0000 site the brief named: on the merged
R3 base that site no longer moves the roles. The move is partial. Inside the big switch, 0 is no longer held in a
register (immediates instead of the original's ebx), and after the switch join 0 is in edi. The register-folded score
falls 67.7% -> 64.0%, because a tail merge between the three `FUN_00431b80(1)` arms changed (cause below).

| Measure (`match.py --bytes`) | before (cbd5dd521) | after (this branch) |
|---|---|---|
| whole function | 51.5% (51.54, regs 67.7%, ops 89.5%), 2799/2817 ins | **52.3%** (52.29, regs 64.0%, ops 86.0%), 2801/2817 ins |
| instructions equal (difflib) | 1452 | **1473** |
| `--bytes` | 11066 B, 6419 differing unmasked bytes, first at +0x5f | 11070 B, **6337** differing, **first at +0xfe** |
| roles at the top | 0 edi, 1 ebx, uVar10 esi | **0 ebx, 1 edi** (original: `xor ebx,ebx` 0x0043e02e, `mov edi,1` 0x0043e03b), uVar10 esi (original edi, `mov edi,eax` 0x0043e4cf) |
| case 0xff490000 (orig 0x0043f9a9..0x0043f9d2) | 2 differing (`push edi`, `mov [0x67f1a8],edi` against ebx) | 2 differing (`push 0`, `mov [0x67f1a8],0` against ebx), not raised |

## The 0xff4b0000 lead is stale on the merged base

The R3 finding (`RESULT_MATCH-43DFD0-RAISE-R3-REGS.md`) was measured on 810042e0e. On cbd5dd521 (ARMS + LOOPS + REGS
merged) the same diagnostics do not move the roles:

| Diagnostic on cbd5dd521 (behaviour-changing, not kept) | score | roles |
|---|---|---|
| `FUN_00472640(0xfe)` | 51.15% | 0 edi / 1 ebx (unchanged) |
| call dropped / argument a global | 51.22 / 51.12% | unchanged |
| top fade 0xff -> 0xfe / tail fade 0xff -> 0xfe | 51.47 / 51.47% | unchanged |
| `return;` at the head of each of the case labels (all switches) | 46-52% | unchanged except **0xff490000 -> 0 ebx / 1 edi** (52.04%) and 0x12 (1 no longer enregistered) |

Original bytes at the brief's site, for the record (read from `original/MASHED.exe.unpatched`): `push 0xff` 0x0043f9d7,
`call 0x472640` 0x0043f9dc, `mov [0x67ecb8],1` 0x0043f9e1 (immediate 1, not a register), `mov [0x67ecb4],ebx`
0x0043f9eb, `call 0x4098b0` 0x0043f9f1, `push ebx; push 1; call 0x43d2a0` 0x0043f9f6..0x0043f9f9, one `add esp,0xc`
0x0043f9fe for both calls, `mov [0x7f0fe8],ebx` 0x0043fa01, `jmp 0x43f6d4` 0x0043fa07. On top of the kept form, a
`uchar` prototype for FUN_00472640 and three store orders in that case give 52.25-52.29% with the roles unchanged.

## Where it moves: case 0xff490000

Original 0x0043f9a9: `push ebx; push 7; mov [0x67f1a8],ebx; call 0x43d2a0; mov eax,[0x67ed6c]; add esp,8; cmp eax,2;
jne 0x43fc74; mov [0x67e9fc],3; jmp 0x43f6d4`. 0x0043fc74 is a compensation block `xor ebx,ebx; or ebp,0xffffffff;
jmp 0x43f6d4`, the same pair as 0x0043f6cf / 0x0043f6d2 ahead of the join 0x0043f6d4.

| Spelling of the case tail (each alone on cbd5dd521) | score | roles |
|---|---|---|
| base `if (ed6c == 2) { e9fc = 3; } break;` | 51.54% | 0 edi / 1 ebx |
| `else break;` added, `&&` assign, `if ... else {}`, `!(ed6c != 2)`, `2 == ed6c`, goto to a label before `break`, local for ed6c | 51.54% (identical listing) | unchanged |
| `ed6c - 2 == 0` | 50.34% | unchanged |
| **`if (ed6c != 2) break; e9fc = 3; break;` (kept), same with a braced `{ break; }`** | **52.29%** | **0 ebx / 1 edi** |
| stores / call reordered (f1a8 after the call), alone or with the kept form | 51.54 / 52.29% | unchanged / moved |
| diagnostics: call dropped, `if` dropped, store unconditional | 52.11-52.18% | 0 ebx / 1 edi |
| diagnostics: f1a8 store dropped, other constants (5, 7,5, 3, 1) | 50.37-51.51% | unchanged |

So the roles follow the edge shape of this case (a separate `break` edge out of the test), not its constants. Only the
early-break spelling gives that shape among the behaviour-preserving forms tried.

## What did not follow (residual)

- The early-break spelling applied on top of the kept form to the six other `if (C) { ... } break;` sites in switch arms
  (source lines 1287, 1306, 1636, 1681, 1869, 1914), alone and all together: identical listing, 52.29%.
- Probes on top of the kept form (a `return;` at the head of each other case, 77 runs): none puts 0 back in a register in
  case 0xff490000 except cutting case 0xff420000, which reverts the top roles too (49.06%). So no single case edit
  restores the original's ebx = 0 through the switch.
- Register-folded loss (67.7 -> 64.0): with the new colouring, c2 merges the `push 1; call FUN_00431b80; add esp,4`
  tails of the `cmp edi,5/6/7` arms (ours around +0x2296..+0x2325) and moves two ~60-instruction blocks. In the original
  those arms do not merge: each loads its own `eax` (0 / 1 / 2) and `mov esi,ebp` before `call 0x431b80`
  (0x00440352..0x004403a4), because FUN_00431b80 takes register arguments that `void FUN_00431b80(int p0)` (source
  line 226, six calls at lines 1760-2003) does not model. That is a converter-shape defect for the ARMS / LOOPS lanes,
  not a register-role lever. [UNCERTAIN] that the missing register arguments are what lets c2 merge those tails: rests on
  reading the two listings, not on a variant (out of this lead).

## Next round

- Model FUN_00431b80's register arguments (eax = 0/1/2, esi = ebp at 0x00440354..0x0044039d) at its six call sites. That
  removes the tail merge, which should give back the register-folded loss on top of this form.
- The switch region's 0 web (original ebx through every case body, re-made at 0x0043f6d2 / 0x0043fc74) is still open.
- uVar10 is in esi (original edi at 0x0043e4cf).

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes` on cbd5dd521 and on this branch (table above).
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022 vcvars32, syntax only, no object,
  no link) in `mashedmod/src/mashed_re/Match`: exit 0.
- Diff: `re/match/s08/0043dfd0.c` header line 2 (score) and the 0xff490000 case (lines 1347-1349), CRLF and encoding kept;
  `re/orchestrator/UNCERTAIN_REGISTER.md` (one line); this file. No byte-identical claim, so no relocheck and no
  promotion TSV.
- Variant count for the role lead: 46 behaviour-preserving or diagnostic edits in the 0xff4b0000 / 0xff490000 cases and
  the `if ... break` sites, plus 120 automated case-head `return;` probes (diagnostic only).
- Scratch tools (session scratchpad, not committed): `exp.py` (parallel variant runner with role readout), `v0.py`..`v6.py`,
  `odis.py` (original listing from `original/MASHED.exe.unpatched`, read only), `dours.py`, `regdiff.py`, `selfdiff.py`.
