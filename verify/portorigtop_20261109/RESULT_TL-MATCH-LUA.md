# RESULT TL-MATCH-LUA (2026-11-10): 0 of 9 at C4. 0x00478660 raised from 22.6% to 79.1%, the other 8 recorded with best score and residual

L2 worker `cmwb1iurk0zp2sc1bkybcr0jm` for lead `cmw7xu1960f37p41557tefpnd`. Branch `orch/TL-MATCH-LUA` from
orch/integration `58d07d683`. Scope: the direct C2 callees of the track loader 0x00426e10 plus the deeper rows named in
RESULT_PORT-OT-TRACKLOAD.md section "Proposed split" item 2. Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178
(`tools/vc7ddk`), `/O2 /GS /Ob1`, scored by `re/tools/match/match.py`.

## Lua range check (L-114)

None of the 9 RVAs is in the Lua library ranges (0x004b7330.., 0x004c0510.., stream layer 0x005507b0..). All 9 sit in
0x00462950..0x0047f6d0. Nothing was dropped.

Two rows touch Lua from the game side, and the lead may want to reread them against L-114:
- 0x0047a320 is `SetCopter` in hooks.csv, a Lua C handler that the binding table registers. Its body calls the Lua
  API (0x004b6fc0, 0x004b70d0, 0x004b7090) but is game code, so it stays in scope here.
- 0x0047a020 runs COURSE.LUA through 0x0047b9b0 with the C table at 0x00440bc0. It calls into Lua and is not part of it.

## Per row

| RVA | B | before | best now | residual class | source |
|---|---|---|---|---|---|
| 0x004785e0 | 121 | 97.8% | 97.8% | (a) x87 schedule: one `fld st(0)` (dup of b[2]) placed after `mov eax,[esp+0x10]` / `test`, the original places it before | re/match/s12/004785e0.c |
| 0x0047f6d0 | 251 | 97.9% (ops 100%) | 97.9% | (b) spill slot: the bit-index temp across the 0x00561ea0 call goes to arg 5's home ([esp+0x28]), the original uses arg 3's ([esp+0x20]) | re/match/s13/0047f6d0.c |
| 0x0047a320 | 121 | 95.2% | 95.2% | (b) `add esi,0x218` hoisted into the loop preheader (ours) against after the call pushes (original). The loop pad changes with it (`nop` / `lea esp,[esp]`) | re/match/s12/0047a320.c |
| 0x0047a020 | 199 (Ghidra 196) | 92.2% insn, 98.0% over 199 B | same | (b) store schedule in the 32-entry loop: the `ff4` 0.5f store (`[eax+0x10]`) comes after the three fe8 zeros in ours, second in the original. The Ghidra body is 3 B short because it leaves out the `lea ecx,[ecx]` pad | re/match/s12/0047a020.c |
| 0x004783f0 | 482 | 90.8% | 90.8% | (b) node-base `lea esi/eax,[..+ebx]` placement around the first distance `fld`. With v[2]=0 at its natural place (next to the normalize) the lea order comes out right, but the fsubs use SIB addressing and the score is 87.9%. Not a pure SIB base/index swap | re/match/s12/004783f0.c |
| 0x00462950 | 1122 | 88.8% | 88.8% | (b) register choice around DAT_0088e670 (eax/ecx flip in the third loop), from MATCH-S10. Not reworked this session | re/match/s10/00462950.c |
| 0x0047be80 | 235 | 86.4% | 86.4% | (b) shrink-wrapped prologue: the original tests the count before `push ebp/esi/edi` and keeps ECX until after it, ours saves ESI first. One fsub is one instruction later. From MATCH-S13 (HELD copy in the 0x0047bf70 file, which needs it for the ECX/EDX/EBX register convention). Not reworked this session | re/match/s13/0047bf70.c (HELD) |
| **0x00478660** | 1606 | 22.6% (raw g2s) | **79.1%** (regs 79.1%, ops 93.9%, 444/446 ins) | (b) see below | re/match/s12/00478660.c (rewritten) |
| 0x00479330 | 2888 | 35.6% (raw g2s) | 35.6% | not reworked. 832 instructions, parked on size per the brief | re/match/s12/00479330.c |

L-115: none of the residuals is purely SIB base/index order, so no row is a knob-path candidate under that rule.

## 0x00478660, by hand

Raw g2s output replaced by a structured source. The original shape, from the Ghidra decomp and the binary:
- per AI polygon i: RwRGBA key {i, i>>8, 0, 0xff} into 0x004b5030 (two 0x30-byte triangles).
- The quad is triangle 0's three vertices, plus the first vertex of triangle 1 that 0x004b46b0 finds equal to none of them.
- The normal comes from the register-arg 0x004785e0 (a HELD static copy in the file gives the EAX/ECX convention) and the
  centre from 0x004b4550.
- A local RwMatrix {right = (0,1,0) x at, up = at x right, at, pos} goes through 0x004c45f0, then 0x004c4dc0 (inverse),
  then 0x004c3d90 (4 points).
- Each corner is classified by quadrant into the record's c[0..3]. The record is 0x4c B at course +0xc1c0 and the count
  is at +0x16240.

What scored:

| move | score |
|---|---|
| hand source, classify as an `__inline` function (stayed a call) | 18.6% |
| classify as a macro | 41.0% |
| retests written commuted (`0.0f > x`), so cl stops threading them | 47.1% |
| `p = &polys[i]` assigned just before its first use (ebp keeps &c[0], esi = ebp-0x18, as in the original) | **79.1%** |

The residual has three parts:
1. The original re-tests `x` before the `y` test of c1 and c3. Plain spellings let cl thread that test away (404 ins).
   The only spelling found that keeps the retest with `fld x` first is `x < 0.0`, at 82.7%. It emits `fcomp qword`,
   where the original has `fcomp dword [0x005d757c]`, so it is not kept. The file keeps the commuted retest
   (`fld K / fcomp x`).
2. x87 scheduling of the right and up cross products.
3. `lea edi` is taken from ebp, where the original takes it from esi.

Tried with no gain:
- the declaration order of all 8 locals (no effect on the frame)
- a `p++` pointer walk, and a corner-pointer IV
- a global float for the 0.0, an `__inline` cross, and a local y-axis vector
- separate tx/ty temps
- `0`, `0.f`, `(float)0.0` spellings, and a pointer-alias retest

The frame layout now matches the original (key 0x10, quad 0x14, cand 0x44, transformed 0x50, matrix 0x80, count 0xc0,
tri-2 copy 0xc4, inverse 0xe8, triangles 0x128). relocheck is BAD only from the offset shift past the first mismatch,
which is expected below 100%.

## Variants tried on the near rows (this session)

| row | what was tried | compiles |
|---|---|---|
| 0x004785e0 | eq1, scalar bz, block scope, operand orders of all six products, negation, double intermediate, return value, norm temp, unsigned, caller constant, early return, comma rvalue, vector a, three vertex pointers | 19 |
| 0x0047a320 | no piVar7, inline store, `++`, strcpy intrinsic (two forms), dword index, struct globals (two forms) | 9 |
| 0x0047a320 | `match_permute.py --rounds 3 --walk 60`: no improvement | 315 |
| 0x0047a020 | 8 store orders, chained `=` for the two 0.5f stores (two forms) | 10 |
| 0x0047a020 | permuter: no improvement (92.2%) | 552 |
| 0x004783f0 | v[2] placements, node pointers, two `__inline` distance helpers, array temp | 11 |

## Acceptance

| check | result |
|---|---|
| rows at C4 | 0 of 9. No promotion, so no c4_reclass / re-classify run and no tracker change |
| best score + residual recorded | 9 of 9 (table above) |
| rva-lint (`scripts/lint_rva_bodies.py`) | OK: 29 findings, all allowlisted, NEW=0 |
| mashedmod | unchanged (`git diff 58d07d683 -- mashedmod` empty). The match sources are not in the port build, so no build and no G-CTRL / B gates |
| Ghidra | pool slot 0, read-only, released |

## Next

- 0x00479330 (2888 B, raw 35.6%) is the largest open row. The 0x00478660 rewrite went from raw 22.6% to 79.1%, so a
  structured rewrite of 0x00479330 could gain as much. It needs its own session.
- 0x00478660 needs a way to keep the c1/c3 `x` retest with a float compare. Candidates not yet tried: the classify
  written as nested ifs with explicit `goto`, and the corner tests inside a `for` loop that cl unrolls.
- The 97-98% rows (0x004785e0, 0x0047f6d0, 0x0047a320, 0x0047a020) have now resisted the permuter and the hand levers in
  two sessions. They fit the parked x87 / near-miss pool (MATCH-X87-CASTLEVER) better than this slice.
