# RESULT MATCH-43DFD0-RAISE-R3-REGS (round 3, worker "regs")

Row MATCH-43DFD0-RAISE, round 3. Match-only (L-168): no build, no gate, no game run. The 0x0043dfd0 body was never executed.
Branch `orch/MATCH-43DFD0-RAISE-R3-REGS` from `orch/integration` 810042e0e. One attempt.
Source `re/match/s08/0043dfd0.c`, toolchain ddk (cl 13.00.9176 / c2 13.00.9178), `// FLAGS: /O2 /GS /Ob1`, C.
Final source sha256 `2001821489d9d9971df8f2ef7051bf4330f7b3b6e4ede9cbcf0d1becb70c1371`.

## Verdict

AMBER. The 0/1 register roles (area of this worker) did not move: no source construct in the declarations or the
prologue / flag-setup region changes them, and every lever there compiles to the identical listing. One raise is kept,
**outside the assigned area**: the `LAB_00440a0c` fade tail (source lines 2060-2088, original 0x00440a0c..0x00440aa9),
which no round-3 worker owns (LOOPS = team-slot loop bodies, ARMS = the 0xff1d0000 arm). It is its own commit, so the
lead can drop it. No byte-identical claim, so no relocheck and no promotion TSV. Line 2 (header score) is not edited, to
keep the three branches mergeable.

| Measure (`match.py --bytes`) | before (810042e0e) | after (this branch) |
|---|---|---|
| whole function | 49.8% (regs 67.0%, ops 88.6%), 2811/2817 ins | **50.0%** (49.77 -> 50.02, regs 67.2%, ops 88.8%), 2800/2817 ins |
| instructions equal (difflib equal blocks) | 1402 | **1409** |
| `--bytes` | 11098 B whole symbol, 6419 differing unmasked bytes, first at +0x5f | 11082 B, 6404 differing, **first at +0x5f** |
| roles at the top | 0 in edi, 1 in ebx, uVar10 in esi | unchanged (original: 0 ebx `xor ebx,ebx` 0x0043e02e, 1 edi `mov edi,1` 0x0043e03b, uVar10 edi `mov edi,eax` 0x0043e4cf) |

## Prologue / flag-setup region: what is left is only the roles

Read from `--show` over the original 0x0043dfd0..0x0043e4cf: every instruction lines up with ours except the
register names of the 0 and 1 webs, plus one 4-byte pair in the scan pointer advance (original `inc eax` /
`lea ecx,[ecx+eax*4]` at 0x0043e3f2 / 0x0043e3f3, after the `jne 0x4409f2` at 0x0043e3ec; ours `lea ecx,[ecx+eax*4+4]`, same length).

Levers tried in the area, each alone on 810042e0e, whole-function score and roles (Z = 0 register, O = 1 register, U = uVar10):

| Lever | score | roles |
|---|---|---|
| base | 49.77% | Z edi, O ebx, U esi |
| `uVar10` declared first / last | 49.77% (identical listing) | unchanged |
| `extraout_EDX` declared first / last; typed `uint`; typed `char` | 49.77 / 49.77 / 49.73% | unchanged |
| flags zeroed with `extraout_EDX` first; chained `a = b = ... = 0` | 49.77% | unchanged |
| bVar `= true` written `= 1` | 49.77% (identical) | unchanged |
| named function-scope zero (`iZero`) for the three top stores / the 8 flag clears / the thunk arguments | 49.77% (identical) | unchanged |
| comma rvalue `(0, DAT_0067f19c) == 2`, `(0, DAT_0067eca4) == 1` | 49.77% (identical) | unchanged |
| `DAT_0067eca4` read once into a local for the 1 / 2 / 3 tests | 49.77% (identical) | unchanged |
| `eca4 != 3` goto written as a nested `if`; `f19c == 2` return through a shared label | 49.77% (identical) | unchanged |
| thunk branch swapped / tested directly / as one `if` | 49.63 / 49.77 / 49.63% | unchanged |
| `f19c = f19c - 1` for the `= 1` store; the three zero stores reversed | 49.77% | unchanged |
| edx flag tests merged into one `\|\|` | 49.13% | unchanged |
| scan as `do { } while`; pointer advance in 4 spellings (`++i; p += i`, `&p[i+1]`, `p + 1 + i`, `p += i; p++`) | 49.77% (identical) | unchanged |

## Where the roles are decided (diagnostics, semantics-changing, not kept)

- Truncating the function (a `return;` before the scan loop, after the scan, before the uVar10 switch, or before
  `FUN_004325c0`) gives Z ebx. So the top region alone colours 0 into ebx as the original does, and code further down
  moves it to edi.
- All 153 post-scan lines' `1` literals written as 7: roles unchanged (50.84%). All 13 pre-scan `1`s as 7: 1 is no longer
  enregistered, Z stays edi (52.20%). So the 1 web's extent does not decide it.
- A `return;` at the head of each of 41 case bodies (uVar10 switch, switch 1, switches 2/3): only the 0xff4b0000 case
  flips the roles, to Z ebx / O ebp (59.52%, 1682 equal instructions). Cutting before the bVar6 or bVar7 blocks also flips
  (Z ebx, O ebp).
- In the 0xff4b0000 body (original 0x0043f9d7), only the argument of `FUN_00472640(0xff)` matters: `0xfe`, a global, or
  dropping the call flips to Z ebx / O ebp (59.6%). Changing the top fade's or the tail's 0xff alone does not flip;
  changing all five other 0xff sites to 0xfe while leaving 0xff4b at 0xff flips too. So the flip happens when the
  0xff4b push and the fade 0xff constants are different constants. 36 random single-literal changes elsewhere never
  flipped (36/36 unchanged). With the 0xff4b argument at 0xfe, 42 further random literal changes all stay Z ebx / O ebp
  (59.1-61.0%); none reached the original's O edi / U edi.
- Every semantics-preserving spelling of that argument folds to the same listing: `(uchar)0xff`, `(uchar)-1` with a
  `uchar` prototype, a `ushort` prototype, `-1 & 0xff`, `255`, `0xffu`, `0xffL`, a local, a `static const`, and
  `int` / `float` return prototypes.
- [UNCERTAIN] The DDK c2 builds one register candidate per constant value across the function, and the 0xff candidate
  spanning the top fade, the 0xff4b0000 case and the tail sets the colouring order. This rests on the flips above, not on
  c2 internals. In the original the same five 0xff sites are immediates (0x0043e0c0 `cmp ecx,0xff`, 0x0043e0d9 `mov [0x67eca8],0xff`,
  0x0043f9d7 `push 0xff`, 0x00440a31, 0x00440a41) and the roles are Z ebx / O edi, so the 0xff candidate is a lever that
  moves the colouring, not by itself the difference against the original.

## Kept: the fade tail (outside the assigned area, own commit)

Original 0x00440a0c..0x00440aa9: `add ecx,eax; cmp ecx,0xff; mov [0x67eca8],ecx; jl 0x440aa3` (store before the compare,
0x00440a2f..0x00440a3d), then `cmp esi,ebx; mov [0x67eca8],0xff; je 0x440aa3` (one 0xff store scheduled between the test
and the branch, 0x00440a3f..0x00440a4b), and in the eca4 == 5 part `add ecx,eax; cmp ecx,ebx; mov [0x67eca8],ecx;
jg 0x440aa3` (0x00440a75..0x00440a7f), all exits through one epilogue at 0x00440aa3.
The source had `if (eca8 + v < 0xff) { eca8 += v; return; }`, two `= 0xff` stores, and `if (eca8 < 1) { ... }`.
Now: `eca8 = eca8 + v; if (eca8 < 0xff) return; eca8 = 0xff; if (iVar8 == 0) return; eca4 = 5;` and
`eca8 = eca8 + v; if (0 < eca8) return; ...; eca4 = 0; eca8 = 0;`.

| Tail form (each alone on 810042e0e) | score |
|---|---|
| store before compare, `if (iVar8 == 0) { = 0xff; return; } = 0xff;` (head merge), `0 < eca8` or `<= 0` | 49.91% |
| **store before compare, one `= 0xff` before the `iVar8 == 0` test, `0 < eca8` (kept)** | **50.02%** |
| same with `<= 0` and a nested block | 50.02% (identical) |
| bVar7 block above the tail as nested `if`, direct call, `!= 0`, or gotos (on top of the kept tail) | 50.02% (identical) |

After the change the tail lines up instruction for instruction with the original except the register names (ours `edi`
for 0, the original `ebx`) and one `xor edi, edi` at the tail entry: in ours the 0 web is rebuilt there, in the original
ebx still holds 0 from the top.

[UNCERTAIN] The tail rewrite keeps behaviour. Read from the source only: every path stores the same values to
0x0067eca8 / 0x0067eca4 and calls the same functions in the same order. Test: `re/tools/match/equiv.py` on this source
against 810042e0e's, or the PROVISIONAL knob-path menu walk ON against OFF. Not run here (match-only row, L-168).
One line in `re/orchestrator/UNCERTAIN_REGISTER.md`.

## Residual for the next round

- The 0/1/uVar10 colouring is decided by code outside the prologue. The two measured switches are the 0xff4b0000 case
  (source line ~1365, its `push 0xff` at 0x0043f9d7) and the bVar6 / bVar7 regions near the end. A next attempt in those
  areas can use the flip tests above as a probe: a semantics-preserving edit there that gives Z ebx, O edi, U edi.
- The scan pointer advance pair at 0x0043e3f2 (4 spellings fold to the same listing).

## Checks

- `py -3.12 re/tools/match/match.py re/match/s08/0043dfd0.c --bytes` before and after (table above); `--show` for the
  listings.
- C++ side: `cl /nologo /Zs /EHsc /std:c++17 /I.. /I..\.. ProvS08_0043dfd0.cpp` (VS2022 vcvars32, syntax only, no object,
  no link) in `mashedmod/src/mashed_re/Match`: exit 0.
- Diff: `re/match/s08/0043dfd0.c` lines 2063-2087 only (CRLF and encoding kept), `re/orchestrator/UNCERTAIN_REGISTER.md`
  (one line), this file. Declarations and lines 1-2059 are byte-identical to 810042e0e.
- Scratch tools (session scratchpad, not committed): `exp.py` / `batch.py` (parallel variant runner with role readout),
  `v1.py`..`v16.py` (variant sets), `odis.py` (original listing from `original/MASHED.exe.unpatched`, read only).
