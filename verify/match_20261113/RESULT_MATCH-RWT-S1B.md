# RESULT MATCH-RWT-S1B (2026-11-13)

Row: compiler shaping of 0x004cb4c0 toward a byte match. Match-only: no build, no gates.
Model: Opus 5.5, high effort. Branch `orch/MATCH-RWT-S1B` from `orch/integration`, source taken from `orch/MATCH-RWT-S1`
(same body, longer notes).

**Verdict: RED, still 90.2%.** No source or flag change reached a better score. The attempt did find out why the
original's compare is folded: the register allocator. Under `/G5`, c2 splits the compare whenever a register is
free, so the fold only appears when no register is left. In the original, ebx is never used in this function or in
its neighbour 0x004cb330, even where a register is short. I found no switch or source shape that leaves ebx out on
our compilers.

## Score again under the header (first step)

Header: `// FLAGS: /O2 /Ob1 /G5 /Oy-`, no `// TOOLCHAIN` line (so `ddk`). This is an RW object, so the game-code
set `/O2 /GS /Ob1` does not apply (RESULT_MATCH-RWT-S1.md measured it lower, 70.7% to 83.7%).

```
py -3.12 re/tools/match/match.py --bytes re/match/s20/004cb4c0.c
  0x004cb4c0 _FUN_004cb4c0: 90.2% (regs 90.2%, ops 96.7%)  92/92 ins  flags=/O2 /Ob1 /G5 /Oy- tc=ddk
  bytes: 321 B whole symbol, 105 differing unmasked bytes (first at +0xc)
py -3.12 re/tools/match/match.py --bytes --toolchain rw9044 re/match/s20/004cb4c0.c
  0x004cb4c0 _FUN_004cb4c0: 90.2% (regs 90.2%, ops 96.7%)  92/92 ins  flags=/O2 /Ob1 /G5 /Oy- tc=rw9044
  bytes: 321 B whole symbol, 105 differing unmasked bytes (first at +0xc)
```
The recorded score holds. Both toolchains give the same score on every variant below, so this function cannot tell
which one built it. Its `/Oy-` neighbour 0x004cb490 is byte-identical under rw9044.

## Residual (unchanged)

Original loop at 0x004cb557..0x004cb570: `mov eax, 0x618258` / `mov ecx, edi` / `mov edx, 0x10` / `sub ecx, eax` /
`mov esi, [ecx+eax]` (0x004cb565) / `cmp esi, [eax]` (0x004cb568) / `jne` / `add eax, 4` / `dec edx` / `jne 0x4cb565`.
No ebx reference anywhere in 0x004cb4c0..0x004cb5bd. The prologue saves esi and edi only.

## The mechanism (probes, `MATCH-RWT-S1B_probes/`)

`asm.py` compiles a probe with either c2 and prints the listing. Every probe below ran under both `ddk` and `rw9044`
with `/O2 /Ob1 /G5 /Oy-` unless the text says otherwise, and both gave the same code.

1. **Compare of two loads is split under /G5 on its own.** `t2.c` b1 (`*a != *b`, no loop): `mov ecx,[eax]` /
   `mov esi,[edx]` / `cmp ecx,esi`, and esi gets pushed just for this. `t1.c` a1..a5, `t5.c` e1..e9, `t11.c` n1..n7
   (types `void*`, `long`, `unsigned long`, enum, a one-field struct, `^`, `>`/`<`) and `t3.c` c1..c6 (an inline
   helper, a real local with two uses, `register`) all split.
2. **The register side folds when it is not loaded in the same block.** `t4.c` d1 (`v != *q`, v a parameter):
   `cmp edx, [eax]`. Also b2/b3 in `t2.c`.
3. **Under /G5 the fold appears when no register is free.** `t7.c` g1: the same loop plus one value live across it
   (`t = H * 3`, used after the loop). All six registers are busy, and the code is `mov ebx, [edx+ecx]` /
   `cmp ebx, [ecx]`, which is the original's form. Without that extra value (`t6.c`, `t8.c`, `t1.c` a5) the
   second load gets ebx and ebx is pushed. That is the 97/92 down-counter result S1 reported.
4. **The original does not use ebx where a register is short.** 0x004cb330 (also an ebp frame with `/Oy-`) keeps its
   first loop's counter in a stack slot: `push ecx` (0x004cb333), `mov dword ptr [ebp-4], 0x10` (0x004cb353),
   `mov edi, [ebp-4]` / `dec edi` / `mov [ebp-4], edi` (0x004cb363..0x004cb36a). eax, ecx, edx, esi and edi are busy
   in that loop, and 0x004cb330..0x004cb48f has no ebx reference. The loop compare there is folded too,
   `cmp edi, [eax]` at 0x004cb35f. Our compiles of the current 0x004cb330 source start with `push ebx`.
5. Under `/G6`, under `/O1`, and with `/Os` added, c2 folds even when ebx is free (`t1.c` a2, flag probe). That is why S1
   saw the right loop under /G6. In this function those flags change the prologue, the store order or the stores
   themselves (/G6 87.0%, /Os family 13.4% to 35.9%).

So reaching 100% needs ebx left out of the allocation for this function, with no added instruction.

## What was tried for that (no change)

- **Switches** (`v7.py`, the full function, dn and index loop forms, both toolchains, each added to
  `/O2 /Ob1 /G5 /Oy-`): `/Op /Oa /Ow /GX /GR /Zi /Z7 /Gh /Ge /Gf /GF /Gi /QIfdiv /QI0f /QIfist /J /Za /Oi- /Ob0
  /Ob2 /Gm /GT /Gs /GA /GD /Gy- /Zp1 /Zp2 /Zl /TP /Gs0 /Ox` and the c2 switches `/d2nodlp /d2loopopt /d2datasort
  /d2bzalign`. None removed the split. `/Oa` and `/Ow` drop the score to 58.8% to 64.1%, and `/Gh` adds a `_penter` call.
  `/GZ` is refused with `/O2`, `/Og` and `/Ox` (D2016). `/ZI` is refused with `/O2`. The non-/Og sets (`/Ot /Oi /Ob1
  /G5`, `/Oity`) score 29.7%.
- **c2 switch table.** Strings in both `c2.dll` list `-u5 -dlp -nodlp -loopopt -Zvc6 -NoLUR -optref ...`. Every
  switch c2 accepted left ebx in use. The others stop with C1007 (unrecognized flag).
- **Inline asm** (`t12.c`, `t13.c`): `__asm` in the function (nop, `mov eax, x`, `mov ebx, ebx`) and a function
  with `__asm` placed before it in the same file. ebx is still allocated. An `__asm` body that uses a parameter frames
  only its own function.
- **memcmp intrinsic** (`t14.c`): `repe cmpsd`, not this loop.
- **Loop and source shapes on the full function** (`v1.py`, `v2.py`, `v4.py`, `v5.py`): index (90.2), reversed
  index (89.1), down-counter in both orders (83.5 / 82.5 under /G5, 87.0 / 84.8 under /G6), `while (n--)` (77.6),
  counter + index (83.5), volatile on either side by cast or pointer (no change), inline helpers in two loop forms
  and both argument orders, plus the `s_cmp` helper copied from the byte-identical 0x004cb8a0 (`while (n != 0)`, uint,
  83.5 / 80.4). 0x004cb8a0 has the same `mov ebp, [ebx+eax]` / `cmp ebp, [eax]` loop (0x004cb8ed). There every
  register is busy, which agrees with point 3.
- **Goto loops** (`t10.c`, including a jump into the middle of the loop): c2 rotates them into the same natural loop
  and splits.
- **Rich header** (`rich.py`): the C/C++ back ends are prodid 0x30 build 9044 (349), 0x1c/0x1d build 9178 (8/49),
  and 0x1c/0x1d build 9466 (373/18). Build 9466 is the retail VS.NET 2002 compiler and is not in `tools/`.
  [UNCERTAIN: whether this object is 9466; no object-to-address map exists.]

## Open lever for a next attempt

What keeps ebx out of the original's allocation in 0x004cb330 and 0x004cb4c0. The same answer should also fix the
0x004cb330 residual (64.9%, and its first ours-vs-original difference is `push ebx` against `push ecx`). Candidates I
did not test: a c2 build other than 9044 and 9178 (retail 9466 is not available), and hidden c2 switches that take an
argument (`-off#`, `-vol#`, `-dslow#` / `-dshigh#`, `-stkfill#`). Fable is the next tier under L-136, after two Opus-high
failures. This is the first Opus-high attempt.

## Checks

- `match.py --bytes` under the header, ddk and rw9044: output above.
- `relocheck.py`: not run (below 100%).
- `mashedmod/src/mashed_re/RwCore/RwCore_004cb4c0.cpp` includes the source (MATCH_PORT). VS2022 syntax check with
  `cl /nologo /Zs /TP /DMASHED_STANDALONE /std:c++17 RwCore_004cb4c0.cpp` returned `ZS_EXIT=0`. Only the comment changed.
- No promotion TSV (not byte-identical).

## Files

- `re/match/s20/004cb4c0.c`: body unchanged, notes extended with this mechanism (MATCH-RWT-S1B paragraph).
- `verify/match_20261113/MATCH-RWT-S1B_probes/`: probe sources and scripts (60 KB): `t*.c` probes, `asm.py`
  (listing helper), `gen.py` + `v*.py` (full-function variant sweeps), `scan.py` (load-then-`cmp reg,[mem]` scan of
  0x004c0000..0x005c8000 against `re/match` sources), `rich.py` (Rich header).
