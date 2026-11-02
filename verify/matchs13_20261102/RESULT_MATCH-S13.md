# RESULT MATCH-S13, 2026-11-02

**Verdict: BLOCKED. Built on this branch, gates not run: display D3DERR_NOTAVAILABLE.** The static and port legs are
done. Per the lead's brief, no boot, gate or probe was attempted on LIVING-PC (display off). Slice 0x0047aad0..0x004840b0
(128 game functions, 0 library rows inside). **106/128** sources are byte-identical under `re/tools/match/match.py`
(XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87). Every one of the 106 has `relocheck.py` OK in its best
source file (`relocheck.tsv`). All 16 BAD lines there belong to functions below 100%, where the instruction streams do
not line up. **24** of the 106 are now the port's only body (19 wrapper TUs `Match/MatchS13_<va8>.cpp`): 9 in
`mashed_re.exe` and the .asi, 15 in the .asi only.
**C4 candidates: 0.** None of the 9 exe swaps is reachable from `WinMain` over direct edges in this build (L-68), so
every swap stays at its current level with the callerless note.

Branch: `orch/MATCH-S13`, cut from orch/integration 3c6510ee6 (MATCH-S01..S08, S08fu and T01G merged; S09..S12 not;
nothing here depends on them). Insertion anchors are S13-only. The rsp block sits after `"Collision\RwpIntegrator.cpp"`
in both rsp files (S09/S10 use `PickupPoolSpawn`, S11 `DrawQuadPrimitives`, S15 `LaunchHandshake`, S16 `VideoConfig`).
The x87 block sits after `RwpSolverMath2Quat` in `mashedmod/x87_tus.txt` (others use `WarzonePontoon`, `PathNodes`,
`CameraEntries`, `ViewportScaledRectDraw`).

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`slice_rvas.txt`, from hooks.csv by `tools/mkslice.py`, the cut_slices.py rule; = `verify/matchscope_20261031/slices.tsv` row) |
| raw converter (g2s) at 100% | 26 |
| hand-matched to 100% (helpers h1-h12, `helper_notes.txt`, `helper_assign.txt`) | 80 |
| of the 106, byte-identical by the whole-symbol rule (`by` = `bytes`, Ghidra size short of the real end) | 6 (0x0047bcc0, 0x0047c160, 0x0047c270, 0x0047c2d0, 0x00480340, 0x00482730) |
| below 100% | 22 (table below; budget spent) |

Tool-data fixes:
- `re/console/pc_funcs.csv` lacked 11 functions of this slice (0x0047ac80, 0x0047ad30, 0x0047ad40, 0x0047b250,
  0x0047b280, 0x0047b350, 0x0047b3c0, 0x0047b500, 0x0047b630, 0x0047b720, 0x0047b7d0), so `match.py` refused to score
  them. Their sizes come from the pool clone's functions (`decomp_pc.py`, slot Mashed_pool10).
- 0x0047ad30 (16 B, DATA-referenced twice from FUN_00440bc0) had no Ghidra function in the pool clone. It was created
  transiently with `decomp_pc.py --create` (slot Mashed_pool0). The master project is not changed.

Slice priority (`verify/match_topdown_20261101/gap_value.tsv`, rows marked MATCH-S13): 0x0047b720 KTC_NewCopter
**100%**, 0x0047f450 **100%**, 0x0047f840 **100%**, 0x0047eb30 VehiclePhysicsWorldStep 92.8%, 0x0047def0 85.3%,
0x0047d640 45.7%.

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x0047ba00 | 14 | 0.0 | 0.0 | b | every 4-byte-arg forward form compiles to a tail jmp. The original pushes and calls |
| 0x0047ba20 | 230 | 60.8 | 71.6 | b | dot-product term order (z first vs y,x,z) and pt/p load order not found |
| 0x0047bb10 | 370 | 79.8 | 87.4 | b | pb address form, pa/ib register swap, sum add operand order |
| 0x0047be80 | 235 | 86.4 | 88.9 | b | shrink-wrapped prologue (count test before the pushes) and one fsub slot. HELD in 0047bf70.c |
| 0x0047c0f0 | 111 | 97.3 | 97.3 | b | DAT_006c2fe8 store lands after `rep movsd`. fec/fe8 are file-static |
| 0x0047cea0 | 127 | 94.0 | 100.0 | b | last-call arg chain loads in swapped registers (edx/ecx), lea operand order |
| 0x0047cf20 | 176 | 56.5 | 58.0 | b | original repeats the bitset test (no CSE). Every spelling tried gets CSE'd |
| 0x0047d3c0 | 638 | 91.9 | 91.9 | b | x87 scale by 0x005cc9dc keeps k in a register vs fmul mem; e0 stack temp |
| 0x0047d640 | 2181 | 45.7 | 69.3 | b | structure right (event ring struct, emission, dispatch order). Per-node loop register allocation and stack slot order differ |
| 0x0047def0 | 2738 | 85.3 | 85.3 | b | row-copy loop induction base (+0x10 vs +0x14) shifts loop padding and later jump targets. One store is one slot early in the c==2 path |
| 0x0047eb30 | 1705 | 92.8 | 93.0 | b | the acos clamp `> 1` test is threaded away by 9176 (+4 B, which shifts targets and padding); small x87 scheduling |
| 0x0047f6d0 | 251 | 97.9 | 97.9 | b | temp spilled to arg5's home slot, original uses arg3's |
| 0x004809e0 | 398 | 98.4 | 98.4 | a | one fld (v.y*300) is scheduled before the FUN_0046d4a0 arg push in the original. Permuter, literal/global, operand order and an inline helper did not move it |
| 0x00480b70 | 493 | 93.1 | 93.1 | b | 4-iteration wheel loop (unrolled). x87 scheduling of vel*20 against the int copy of m->pos |
| 0x00480d60 | 2494 | 72.8 | 66.8 | b | original keeps two copies of the shared case tail (cases 6 and 7). Ours emits one. Register choice in cases 1 and 2 |
| 0x004818d0 | 339 | 98.9 | 98.9 | b | one v.z copy store is scheduled after the first fld |
| 0x00481a30 | 855 | 63.3 | 65.4 | b | register allocation (obj/atom in ebp/ebx), local slot order, query struct size unknown |
| 0x00481e00 | 550 | 86.5 | 96.5 | b | DAT_006ce818 file-static. Count cached in ecx not edx, param_2*=K operand order |
| 0x00482140 | 1405 | 76.8 | 77.4 | b | normal-sum loop unrolled x8 here vs x4 in the original. j/tl slot swap |
| 0x00482900 | 48 | 85.7 | 85.7 | b | fld param / fmul [0x005cc728] operand order. A literal 60.0f fixes the order but folds a negated constant |
| 0x00482ae0 | 301 | 99.0 | 99.0 | a | the p0 load comes one slot after the first fld t. Permuter and 6 rewrites did not move it |
| 0x00482c10 | 3616 | 83.3 | 80.0 | b | hand-written (raw did not compile). Residual x87/push scheduling in inlined quat->matrix blocks. uint->float uses the compiler's 2^32 constant vs DAT_005cc94c |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit.

Held static callees (files that define another RVA's function):
- `0047bf70.c` holds 0x0047bc90, 0x0047bcc0 (both now bodies, `MATCH_STATIC`) and 0x0047be80 (86.4%, converted to
  `// HELD:` and `#ifndef MATCH_PORT`. The port calls the original through a naked raw-address forwarder).
- `0047c270.c` holds 0x0047c230 and `0047c2d0.c` holds 0x0047c1f0 (both 100% with no other port body, so these files
  are their only body).

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, x87, port symbol, install, source blob, match and relocheck.
Its `old_body` column is post-swap, so it names the new wrapper. The removed old bodies are in the helper reports
`swap_w1.txt` .. `swap_w4.txt` (committed, small).

| targets | rows |
|---|---|
| exe + asi | 9: 0x0047c160, 0x0047c230, 0x0047c270, 0x0047c1f0, 0x0047c2d0, 0x0047d150, 0x0047ea40, 0x00482030, 0x004826d0 |
| asi only | 15: 0x0047b9e0, 0x0047bc90, 0x0047bcc0, 0x0047bf70, 0x0047c0b0, 0x0047cdc0, 0x0047ce00, 0x0047ce40, 0x0047ce70, 0x0047ce80, 0x0047d130, 0x00482730, 0x00482860, 0x00483a30, 0x00483ca0 |

- `py -3.12 scripts/lint_rva_bodies.py` -> `2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`
  (unchanged). In `bodies_post.tsv`, each of the 24 RVAs has exactly one body, its `MatchS13_` wrapper.
  `py -3.12 scripts/lint_inline_asm_abs.py` -> OK (1153 files). `re/tools/dual_copy_allowlist.txt` is unchanged: no
  swapped RVA had an entry.
- **x87:** `MatchS13_00482030` only. Its old body (`Vehicle/SubStripUV.cpp`) was a naked-asm copy of the original x87
  instructions, so x87 keeps what it computed. No other old TU was in `x87_tus.txt`, so their wrappers stay SSE, as
  S11 did. The helpers flagged 0x0047cdc0 (f*f), 0x00482730 (constant stores) and 0x004826d0 (compare against 0.0).
  None of them computes a different value under SSE.
- 0x004826d0: the matched source reads the original's 0x005d757c (0.0), where the old body read the port global
  `DAT_005d757c` (0.0, written nowhere in the port). The exe's copy of that `.rdata` slot is 0, so the value is the
  same, as S08 accepted for 0x00442cc0. The C++ `mashed_re::Collision::PhysicsCollisionBodyCreate` stays as a one-line
  forwarder with no RVA comment.
- 0x0047b9e0: the shadow A/B entry `Track_RenderQuadThunk_47b9e0` (armed only by `MASHED_SHADOW_AB`) stays in
  `Track/CourseLoadDispatch.cpp` as a thin caller of the matched `_impl` (the S11 FollowTargetField pattern). Asi only.
- Raw-address forwarders, used where the old body called the original by address and the port has no body:
  0x0047c160 -> 0x004c0ed0 / 0x004c1b40 / 0x0047bb10; 0x0047c270 -> 0x004780c0; 0x0047c2d0 -> 0x00478030; 0x0047d150 ->
  0x0057c210; 0x0047bf70 -> 0x0047be80 / 0x004c0ed0 / 0x004c3d90 / 0x004b53b0; 0x0047b9e0 -> 0x00496c10.
  Port-body binds: 0x0047bcc0 binds `FUN_0047bc90` to `Match47bc90`, 0x0047bf70 binds `FUN_0047bcc0` to
  `TriEdgePairsBuildBody`, and 0x004826d0 binds `FUN_00481e00` to `ConeCastHullBuild` (extern "C" forwarder).
- 0x0047e9c0.c gained C++-compat edits (callee arities for FUN_0047d640/FUN_0047def0, pointer casts). DDK match is
  still 100% (40/40) and relocheck is OK. It is not swapped (below).
- Old TUs now holding no code (kept in the rsp files so this branch does not touch those lines):
  `Camera/CameraPathPredicates.cpp`, `Track/PromoLoop_standins_r2.cpp`, `Vehicle/ReplayRewind_ag.cpp`,
  `Vehicle/SubStripUV.cpp`.

**3 byte-identical rows NOT swapped (NOT-SAFE).**

| rva | old body | reason |
|---|---|---|
| 0x0047d100 | Util/PromoLoop_sessionB.cpp BoundedThunk47d100 (naked, asi) | the matched source declares FUN_004b5240 with one arg and relies on the DDK tail jmp to pass the caller's 2nd arg through the reused frame. The port's `Or4b5240` takes (p, flag), and a VS2022 compile is not guaranteed to emit that tail jmp |
| 0x0047e9c0 | Collision/RwpSolverCore23.cpp FUN_0047e9c0 | TT-11 `ShadowAB::RunTracked` wrapper armed by `MASHED_SHADOW_AB` / `MASHED_PHYS_C4_SELFTEST`. `_impl` computes the same thing. Swappable once the shadow wrapper is retired |
| 0x004840b0 | Vehicle/ShapeOwnerHandlePool.cpp ShapeOwnerHandleGet | TT-11 ShadowAB wrapper (`MASHED_SHADOW_AB`) around `_impl`, which computes the same thing. Same follow-up |

**79 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68): 0x0047aad0,
0x0047ab30, 0x0047abd0, 0x0047ac80, 0x0047acd0, 0x0047ad00, 0x0047ad30, 0x0047ad40, 0x0047ade0, 0x0047ae80, 0x0047aef0,
0x0047af50, 0x0047afa0, 0x0047b000, 0x0047b0a0, 0x0047b110, 0x0047b160, 0x0047b1b0, 0x0047b200, 0x0047b230, 0x0047b250,
0x0047b280, 0x0047b350, 0x0047b3c0, 0x0047b420, 0x0047b480, 0x0047b500, 0x0047b5b0, 0x0047b630, 0x0047b6b0, 0x0047b720,
0x0047b7d0, 0x0047b860, 0x0047b880, 0x0047b8a0, 0x0047b8d0, 0x0047b980, 0x0047b9b0, 0x0047ba10, 0x0047ccb0, 0x0047cd50,
0x0047cde0, 0x0047ce20, 0x0047d080, 0x0047d180, 0x0047d240, 0x0047d330, 0x0047ea60, 0x0047f1e0, 0x0047f230, 0x0047f290,
0x0047f380, 0x0047f450, 0x0047f480, 0x0047f4c0, 0x0047f840, 0x0047f940, 0x0047fad0, 0x0047fc40, 0x0047fe00, 0x0047ff70,
0x00480100, 0x00480340, 0x00480720, 0x00481750, 0x00481780, 0x00481810, 0x00481d90, 0x004820e0, 0x00482820, 0x00482930,
0x004829d0, 0x00483a40, 0x00483a70, 0x00483d10, 0x00483ed0, 0x00483fa0, 0x00484000, 0x00484060.
0x0047b720 (KTC_NewCopter), 0x0047f450 and 0x0047f840 are gap_value rows: their sources are ready for a top-down row
that ports their caller.

**Port bugs found: none.** The swap helpers found every old body that a SAFE swap replaced computing the same thing as
the matched source. 0x00482730's old body was `void` where the original returns param_1 in EAX. No port caller reads
it, so this is not a defect. No ledger rows were filed.

## Exe callers (L-68)

Binary witness on this build: `py -3.12 verify/matchs13_20261102/tools/exe_reach.py mashedmod/build/mashed_re.exe
mashedmod/build/mashed_re.map <symbols>` walks the direct `call`/`jmp rel32` graph from the linker map, starting at
`_WinMain@16` (`exe_reach.txt`). `addr_taken=yes` is the install table, not a caller.

| rva | symbol | direct chain from WinMain |
|---|---|---|
| 0x004826d0 | PhysicsCollisionBodyCreate | source caller `Collision/PhysicsWorldBuild.cpp:68` (`VehiclePhysicsWorldCreate`, 0x0047d3c0). That function is not in the exe map (no exe caller, dropped by the linker), so there is no chain |
| 0x0047c160, 0x0047c270, 0x0047c2d0, 0x0047d150, 0x0047ea40, 0x00482030 | | 0 direct sites, not reachable |
| 0x0047c230, 0x0047c1f0 | file-static in their wrappers | called only from 0x0047c270 / 0x0047c2d0 |
| 15 asi-only rows | | none (exe-callers=0) |

## Gates

**Not run: display unavailable.** The lead's brief records that LIVING-PC's display is off (`CreateDevice` fails with
D3DERR_NOTAVAILABLE, as MATCH-S10/S11 measured), and it forbids gates, D2 arms and probes in this step, so no boot
was attempted.

Owed before review: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on this exe, compared with
`re/orchestrator/merge_train.json` (`gctrl.ref` / `b.ref`), plus the merge train's `-Full` pass (`mashedmod/src` is in
`full_set_paths`). Two exe swaps run in default code. 0x0047ea40 (`RwpSceneStepWrapper`) and 0x00482030
(`SubStripQuadUV`) have no direct exe caller on this map. 0x004826d0 has a caller only through a function the exe does
not link. A gate move from this branch would therefore point at an indirect (address-taken) path.

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

No row qualifies for C4 (no exe caller). All 24 swapped rows stay at their level, and `notes` += `match evidence:
identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`. Evidence for each: `match.py
re/match/s13/<file>.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`, `relocheck.py` OK (`relocheck.tsv`), one body
(`Match/MatchS13_<va8>.cpp`, `swaps.tsv`). The `file` cell moves to the wrapper.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`: Build OK, .asi not deployed (`original\mashed_re_dev.asi` still
dated 2026-10-24). exe sha256 `943c804125a1c2f11004df8126b95b71686a64e899e3b0454c2f998833199fa3`, asi
`a04feadc13f861d0027d1f924a82cd6f20b8ffbd8683a4edc6bdc3044a005cea`.

## Raw files (not committed; `D:/MashedWT/MATCH-S13_data`)

| file | sha256 |
|---|---|
| decomp.json (decomp_pc.py --port --callees, 128 entries, slot Mashed_pool10; 0x0047ad30 entry from decomp_ad30.json) | 9802cb94b49c1057e0fff4c3a4f0cec3fb97c4b1856f76f92e596c53ae108eb8 |
| decomp_ad30.json (decomp_pc.py --create, slot Mashed_pool0) | 4b76a5fb00fc7db5e843fa5b6a04a78acfd31b8a3618bbc29cf4da22943c881c |
| g2s/results.tsv (raw converter scores) | 7c9ab801d6d57f66d84f3d3570df68a4a40382461fcb6c292f9ba7ad647431a2 |
| g2s.log | 3815e63beca2c966a7b0ccfda74cd54439637cc9c8249c77cd7cd1fddb687866 |
| g2s_conv.log (to_s13 + match + relocheck of the 26 raw 100% files) | f136da9567c02951938525fcbe0fd8dd1ed51d4f22b199f79a1ae4764f0cae04 |
| build1.log (Build OK) | 50fc547d0bede11769372781362c9bb809b466ab91c4f3d472f0e3da8a47f502 |
| g2s/, notes/h1..h12.md, scratch/, bodies.tsv, bodies_post.tsv, filemap.txt | raw converter output, helper notes, scratch |
