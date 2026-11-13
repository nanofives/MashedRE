# RESULT MATCH-RWT-S1B (2026-11-13)

Row: compiler shaping of 0x004cb4c0 toward a byte match. Match-only: no build, no gates.
Model: Opus 5.5, high effort. Branch `orch/MATCH-RWT-S1B` from `orch/integration`, source taken from `orch/MATCH-RWT-S1`
(same body, longer notes).

# Attempt 2 (second Opus-high attempt): GREEN, 100.0%, byte-identical, relocheck OK

**Verdict: GREEN.** 0x004cb4c0 is byte-identical under the recorded header (`// FLAGS: /O2 /Ob1 /G5 /Oy-`, toolchain
`ddk`, unchanged), and also under `rw9044`. relocheck OK. `c4_reclass.py batch` dry run PASS. Promotion TSV:
`re/promotions/MATCH-RWT-S1B.tsv` (not applied on the branch).

## The lever: an over-aligned global

Attempt 1 showed the fold needs ebx out of the allocation. The construct that does that is a reference to a global
whose type has `__declspec(align(N))` with N > 4. Such a function takes c2's aligned-frame path: an ebp frame, and
under the Processor Pack c2 (rw9044) ebx is not allocated (the alternative aligned-frame scheme, used with `/Oy-` and
a stack local, keeps the incoming esp in ebx). With no aligned stack local left, no `and esp` is emitted. The result
is the original's prologue and loop.

Source change in `re/match/s20/004cb4c0.c` (body otherwise unchanged):
- `typedef __declspec(align(8)) struct { int m[16]; } Mat16A;` and `MGLOBAL(Mat16A, DAT_00618258, 0x00618258);`
  (was `Mat16`). The copy is `*(Mat16A *)cache = DAT_00618258;`, the call passes `(Mat16 *)&DAT_00618258`.
- The compare loop is the S1 down-counter: `const int *p = DAT_00618258.m, *q = cache; int n = 16;
  do { if (*q != *p) goto set; p++; q++; } while (--n);`.
- align(16) gives the same bytes. align(8) is kept because 0x00618258 & 0xf = 8: the original's object is
  8-aligned and not 16-aligned.

Probe evidence (`verify/match_20261113/MATCH-RWT-S1B_probes/`, listings `t26.rw9044.txt`, `t27.*.txt`):

| probe | construct | rw9044 /O2 /Ob1 /G5 | ddk /O2 /Ob1 /G5 |
|---|---|---|---|
| t26 k8 / k16 | function-scope `static __declspec(align(8 / 16)) Mat16` | frame, no ebx, folded `cmp esi, [eax]`, no `and esp` | |
| t26 k4 / kn | align(4) / none | no frame, `push ebx`, split | |
| t27 m_blk / m_file | block-scope / file-scope `extern __declspec(align(8)) Mat16` | frame, no ebx, folded | frame, ebx taken, split |
| t27 m_none | plain extern | no frame, ebx, split | no frame, ebx, split |
| t23 f6, t24 g2..g7 | aligned stack local in a function with a parameter | frame + `and esp,-16` / `-8`, no ebx, folded | |
| t25 h1..h6 | `__declspec(align(1 / 2 / 4))` | no frame, ebx | |

The t26 `k8` listing (rw9044) is the original's shape instruction for instruction: `push ebp` / `mov ebp, esp` /
`mov eax, [ebp+8]` / `push esi` / `push edi` ... `mov esi, [ecx+eax]` / `cmp esi, [eax]` / `dec edx` ... `pop edi` /
`mov eax, 1` / `pop esi` / `pop ebp` / `ret` (original: 0x004cb4c0..0x004cb4c9, loop 0x004cb565..0x004cb570,
exit 0x004cb572..0x004cb57a).

## Scores of the committed source (`v14.py` sweep, `v14.log`)

| loop | aligned type | ddk /Oy- | ddk no /Oy- | rw9044 /Oy- | rw9044 no /Oy- |
|---|---|---|---|---|---|
| down-counter (committed) | align(8) | **100.0** | 83.5 | **100.0** | **100.0** |
| down-counter | align(16) | 100.0 | 83.5 | 100.0 | 100.0 |
| down-counter, compare `*p != *q` | align(8) | 97.8 | 82.5 | 97.8 | 97.8 |
| index (old body) | align(8) | 90.2 | 90.2 | 90.2 | 90.2 |
| `while (n--)` | align(8) | 70.2 | 77.6 | 70.2 | 70.2 |

The recorded header stays `/O2 /Ob1 /G5 /Oy-` with the default `ddk` toolchain: no flag change was needed. Note for the
unit's flags: rw9044 reaches 100% with and without `/Oy-`, ddk only with `/Oy-`. The frame in the original is
explained by the aligned global under rw9044 plain `/O2 /Ob1 /G5` (the flags of every frameless neighbour in this
object). I did not change the recorded flags because the ddk + `/Oy-` result is also byte-identical.

## Checks (run on the branch)

```
py -3.12 re/tools/match/match.py --bytes re/match/s20/004cb4c0.c
  0x004cb4c0 _FUN_004cb4c0: 100.0% (regs 100.0%, ops 100.0%)  92/92 ins  flags=/O2 /Ob1 /G5 /Oy- tc=ddk
  bytes: 320 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/match.py --bytes --toolchain rw9044 re/match/s20/004cb4c0.c
  0x004cb4c0 _FUN_004cb4c0: 100.0% (regs 100.0%, ops 100.0%)  92/92 ins  flags=/O2 /Ob1 /G5 /Oy- tc=rw9044
  bytes: 320 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
py -3.12 re/tools/match/relocheck.py re/match/s20/004cb4c0.c
  0x004cb4c0	OK	checked=25	unchecked=0	004cb4c0.c
py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-RWT-S1B.tsv      (dry run, no --apply)
  [1/1] 0x004cb4c0 match PASS byte-identical 320 B, relocheck OK (25/0)
  batch: 1 pass, 0 refused, 0 skipped (already C4), 1 rows  [by route: match=1]
```
- Not a naked or asm-only body: plain C, no `__asm`.
- Port include `mashedmod/src/mashed_re/RwCore/RwCore_004cb4c0.cpp` (MATCH_PORT, C++): VS2022
  `cl /nologo /Zs /TP /W3 /DMASHED_STANDALONE /std:c++17 RwCore_004cb4c0.cpp` -> no warning, exit 0. No build run
  (match-only row). The body is listed as provisional for the RW core knob path
  (`verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv`); moving it off that list is port-axis work and is not done here.
- Source SHA-256: `30f66202be37179cbddddb340171391be59edb976b885117d8319f0e1be5de37`.

## Levers tried in attempt 2 before the hit (no change, kept for the record)

- c2 switches with an argument via `/d2` (`v8.py`, `v12.py`): `-off0..64`, `-Loop0..16` (0..2 change the code, not
  toward the original), `-vol0..255`, `-db0..255`, `-ef0..255`, `-stkfill0`, `-Gt0/100`, `-dslow#`/`-dshigh#`,
  `-inlt#`/`-inls#`, plus `-noehopt -nolock -optref -NoLUR -Zvc6 -QIWMTemu -QIftol2 -cef -ehopt -bzalign -datasort
  -EHa -EHs -Zd -Za -Zl -loopopt -nodlp -QIfpinit -QIdivrem64 -ide -cbstring` (`-isize0` and `-dlp` stop with C1001,
  `-fastcap` adds profiling calls). c2 `-G` letters (`gscan.py`): only `-G6` folds (with the /G6 scheduler),
  `-GZ` drops optimization.
- Per-function `#pragma optimize` (`v9.py`): `y` off alone and with `t`/`s`/`p`/`a`/`w`/`i`: `y` off = `/Oy-`.
- `/O` letters taken apart (`v10.py`): `/Og` with favor none / `/Ot` / `/Os`, `/Ob0..2`, `/Oi`, `/Op`: 90.2% at best.
- CPU targets (`v11.py`): `/G3 /G4 /G5 /G6 /GB` and c2-only `/d2G3..6` on both toolchains.
- VC6 c1 switches via `/d1` (`v13.py`): `-Ov -Oq -Ow -Oa -Bdlp -BnoWorkList -noexpinl -GM -int2enum -GI -Gi -Oy`.
- Frame or reservation from inline asm (`t21.c`, `t22.c`): asm in dead code removed by c1, asm in a branch only c2
  can prove dead, unreachable asm: no frame, ebx still allocated. Doubles, `__int64`, floats, struct copies (`t20.c`).
- `framescan.py` / `framescan.log`: ebp-frame functions in 0x004c0000..0x005c8000 and their ebx use. Between
  0x004c0330 and 0x004cf160 (both aligned frames with `and esp`) the only ebp-frame functions are 0x004cb330,
  0x004cb490 and 0x004cb4c0. All three reference 0x005d8b90 (0x004cb3ef, 0x004cb4af, 0x004cb5ca), and 0x004cb4c0
  also 0x00618258.

## Follow-up for the lead (not done here)

0x004cb330 (current source 64.9%): the same lever, `DAT_005d8b90` declared with an aligned type and no other change,
scores 85.1% (regs 92.1%, ops 99.1%) under ddk `/Oy-` and under rw9044 with or without `/Oy-`
(`cb330_a8.c`, `cb330_a16.c` in the probe folder, not applied to `re/match/rwl01/004cb330.c`). Its first-loop counter
spill at [ebp-4] (0x004cb353, 0x004cb363, 0x004cb36a) fits the same ebx exclusion. 0x004cb490 (already 100% under
rw9044 with `/Oy-`) references 0x005d8b90 too.

# Attempt 1 (first Opus-high attempt): RED, history

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
