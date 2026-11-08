# RESULT MATCH-S18, 2026-11-07 (match-only, decision L-93)

**Verdict: GREEN. 83/96 identical** (83 byte-identical of 96 attempted). Slice 0x004b3b70..0x004b8080 (96 game
functions, 49 library rows inside skipped, `slice_rvas.txt`, `tools/mkslice.py`). Every one of the 83 has
`re/tools/match/relocheck.py` OK (84 OK lines: 0x004b6940.c also holds 0x004b6710, see below), 0 BAD. The only
`unchecked` names are Win32 imports (`CreateFileA`, `GetLastError`, `CloseHandle`, `ReadFile`, `GetOverlappedResult`)
and the CRT `/GS` cookie pair in 0x004b6940. No source is naked asm: no `__asm`, `#include` or `#pragma` other than
`#pragma intrinsic` in any `re/match/s18/*.c`.

Scope per L-93: select, g2s, match, bounded permute, sources under `re/match/s18/`. **No body swap** into
`mashedmod/src`, **no tracker edits** (hooks.csv, CHANGELOG, UNCERTAINTIES, STUBS, DEFERRED untouched). Step
C4R-MATCHBATCH-2 promotes the identical rows. Gates: not run (match-only, the merge train runs them).

Compiler: XP DDK cl 13.00.9176 / c2 13.00.9178 (`tools/vc7ddk`), through `re/tools/match/match.py`. Flags per file on
line 1: 95 files `/O2 /GS /Ob1` (game flags), 1 file `/O2 /Ob1 /G5` (0x004b71c0, not matched).

## Counts

| | count |
|---|---|
| slice functions | 96 |
| raw converter (g2s.py on decomp_pc.py --port --callees) at 100%, relocheck OK | 25 |
| hand-matched to 100% (helpers h1-h5, `helper_assign.txt`, brief `BRIEF.md`) | 58 |
| **identical total** | **83** |
| of the 83, scored by the whole compiled symbol (Ghidra size short) | 1 (0x004b51d0: Ghidra size 2 instructions short, the whole 98 B symbol is byte-identical) |
| below 100% (budget spent) | 13 |

All 96 files pass the VS2022 C++ check (`cxxcheck.bat`, `/TP /DMATCH_PORT`), re-run over the whole directory after
the last edit: `checked=96 fail=0`. 13 of the 25 raw-converter files first failed it (int vs pointer arguments,
and a 2-argument FUN_004b4130 prototype in 004b4140.c); explicit casts and corrected callee prototypes fixed them,
with `match.py` 100% and relocheck OK re-run on each after the edit.

Per-function table: `results.tsv` (rva, size, subsystem, conf, exe body, raw g2s %, best %, regs %, how scored,
source, source sha256 prefix). Relocheck: `relocheck.tsv`. Helper reports: `helper_notes_h1.md` .. `helper_notes_h5.md`.

## Forms worth carrying to later slices

- 12 functions are 5-byte `jmp rel32` thunks (0x004b5350..0x004b53a0 into 0x00550xxx, 0x004b6540..0x004b65b0 into
  0x004b66xx..0x004b68f0). g2s scored them 0% because Ghidra sizes each as one instruction. Form:
  `ret FUN_x(args) { return FUN_target(args); }` with the target's arity from its own `ret N`. All 12 at 100%.
- Register-arg functions (pilot finding 1): 0x004b6710 (name in EAX), 0x004b6b00 (ECX dst, EAX value), 0x004b6b90
  (EAX, ECX) match as a `static` function followed by a stand-in caller `MatchStandIn_<va8>` that has **no**
  `// MATCH:` line and is not a matched function. Parameter order picks the registers ((dst, value) gives ECX/EAX for
  0x004b6b00). The port swap leg must drop the stand-in callers (they exist only to give 9176 the register
  convention). 0x004b6940.c holds 0x004b6710 as its real static callee (both 100% in that file).
- Callback-query families (0x004b4a80/4ad0/4b20/4b60, 0x004b4c80/4cd0/4d10, 0x004b4e20/4e70, 0x004b41b0/41f0/4230/4280,
  0x004b4010): one solved form per family carried to its siblings: a callback context struct, `result = 0` written
  first (or in the declarator), and the query record's type word stored before the sphere/line copy.

## Below 100% (13)

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x004b42c0 | 356 | 89.0 | 89.0 | b | x87 scheduling: up-cross products vs normalize arg pushes, `fstp st(0)` placement |
| 0x004b4430 | 178 | 81.9 | 63.9 | b | x87 operand order in 4 of 12 cross products, `fstp st(0)` placement |
| 0x004b4550 | 247 | 63.3 | 63.3 | a | vec3 mean: the original unrolls the add loop x4, 9176 unrolls x8 in every form and flag set (same as 0x00401690, 0x00499d90) |
| 0x004b5080 | 260 | 85.4 | 85.4 | b | x87 operand order inside the up cross product, one `fstp` placement; the permuter gained only by breaking semantics |
| 0x004b53b0 | 431 | 45.4 | 45.4 | a | max loop unrolled x4 by the original, x8 by 9176 in every form tried (as 0x00401690, 0x00475ab0) |
| 0x004b55a0 | 421 | 83.7 | 92.7 | b | RwGlobals pointer held in EAX instead of ECX through the colour block |
| 0x004b5750 | 623 | 81.6 | 94.3 | b | frame slot order of the two RwInt16 offset locals vs the int conversion temp (an eax/ecx flip follows) |
| 0x004b5af0 | 661 | 85.7 | 85.7 | b | matrix-axes Im3D draw, structure exact; y/z axis products load the axis before the scale in the original; operand order, helpers, forms, flags, permuter (455 compiles) no move |
| 0x004b5d90 | 816 | 60.1 | 60.1 | b | x87 scheduling of the 24 box-corner stores (original dups negations with `fld st(0)`, routes copies through int temps over the white-colour slot) |
| 0x004b61c0 | 701 | 95.9 | 95.9 | a | 9176 unrolls the vertex loop x8 (original x4); a hand x4 unroll matches all code but adds a 2-byte loop-top align pad |
| 0x004b6640 | 180 | 60.5 | 88.4 | b | the original keeps `p` in EDX and copies it to ESI only for the last call, 9176 puts it in ESI from the start |
| 0x004b6b20 | 110 | 71.4 | 91.4 | b | register args (EAX/EBX/ECX, stand-in caller); after the FUN_004b67e0 read the original moves the byte count to EAX and forms the end with `lea ecx,[eax+ebx]`, 9176 uses ECX + add; the permuter's 77.1% reorders two global stores against the original's order, not kept |
| 0x004b71c0 | 52 | 54.2 | 66.7 | a | `repne scasb` strlen that 9176 does not emit under any flag set tried (`/O2 /Ob1 /G5` recorded); h4 places it with the Lua 4.0 code that starts after 0x004b8080 [UNCERTAIN: TU membership not checked against the library-identity table] |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit. The x4-vs-x8 loop unroll
(0x004b4550, 0x004b53b0, 0x004b61c0) is the same residual earlier slices recorded at 0x00401690, 0x00475ab0 and
0x00499d90: 9176 unrolls these loops twice as far as the shipped compiler did.

## Raw data (not committed), `D:/MashedWT/MATCH-S18_data`

| file | sha256 |
|---|---|
| decomp.json (decomp_pc.py --port --callees, 96 entries, slot Mashed_pool0) | a121933aba33c62061794880a5503b8672789c4216b560212235e332c5c13c0c |
| g2s/results.tsv (raw converter scores) | 7c2f7f4626737f18b0a870e0869cbf2010731fa84879a2b34e119fd14488f1e8 |
| g2s.log | 2b8af6378d337dd59ba22af99089d229fdb7cba8c04be00c65fdf13e81b9cea7 |
| relocheck_raw.tsv (= relocheck.tsv without its header) | bec3294cf4721edbc5b0d81f5048b8851703823fbb93b7b5ba159579f774268b |

Also there: `g2s/` raw converter sources, `notes/h1..h5.md` (copied here as `helper_notes_h*.md`), `scratch/`.

## Next

C4R-MATCHBATCH-2 promotes the 83 identical rows from `results.tsv` + `relocheck.tsv`. A later swap leg (port axis)
must drop the `MatchStandIn_*` callers in 0x004b6710.c, 0x004b6b00.c, 0x004b6b90.c, 0x004b6b20.c.
