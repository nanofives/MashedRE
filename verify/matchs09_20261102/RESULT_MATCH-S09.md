# RESULT MATCH-S09, 2026-11-02

**Verdict: @VERDICT@.** Slice 0x00451690..0x0045b990 (129 game functions). **107/129** sources are byte-identical
under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87), and every one of
the 107 has `relocheck.py` OK in its best source file (`relocheck.tsv`; the 19 BAD lines there all belong to
functions below 100%, where the instruction streams do not line up, so the offsets compare the wrong operands).
**19** of them are now the port's only body (19 wrapper TUs `Match/MatchS09_<va8>.cpp`): 5 in `mashed_re.exe`
(2 of them also in the .asi), 14 in the .asi only.
Gates on this build: @GATES@.
**C4 candidates: 5 rows**, each with an exe caller chain from `WinMain` (table below). The 14 asi-only swaps have
no exe caller (L-68).

Branch: cut from orch/integration 035dc5bff. MATCH-S08 is not merged there (merge-queued); nothing here depends on
it. The S09 blocks in `exe_sources.rsp` / `asi_sources.rsp` sit after `"Gameplay\PickupPoolSpawn.cpp"` and the
`x87_tus.txt` block after `WarzonePontoon`, away from S08's anchors (`Util\CameraEntryDispatch.cpp`,
`RwpSolverCore17`), so the two merges do not touch the same lines. No old TU edited here is edited by S08.

## Match (static leg)

| | count |
|---|---|
| slice functions | 129 (`slice_rvas.txt`, from hooks.csv by `tools/mkslice.py`, the cut_slices.py rule) |
| raw converter (g2s) at 100% | 22 |
| hand-matched to 100% (helpers h1-h10, `helper_notes.txt`, `helper_assign.txt`) | 85 |
| of the 107, byte-identical by the whole-symbol rule (`by` = `bytes`) | 7 (0x00451d80, 0x00452f30, 0x00453f30, 0x004548e0, 0x00457650, 0x00459290, 0x0045b930) |
| below 100% | 22 (table below; budget spent) |

Tool-data fix: `re/console/pc_funcs.csv` lacked 11 functions of this slice (0x00451690, 0x00451760, 0x00451a50,
0x00451d80, 0x00452130, 0x00452170, 0x004527f0, 0x00452ad0, 0x00452af0, 0x00452b20, 0x00458920). Their sizes come
from the Ghidra decomp: `decomp_pc.py --create` for the first ten (the pool clone lacked them), the clone's function
for 0x00458920.

Sizes that the csv gets wrong, as the helpers found them (not changed in the csv): 0x00451d80 is 944 B with its
jump table (csv 61), 0x00452ad0 is 18 B (csv 89 runs past the tail `jmp`), 0x004556f0 is 541 B (csv 98).

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x00453730 | 377 | 95.3 | 95.3 | b | static EAX-arg, held in 004538b0.c; &hit push and one out.x fstp scheduled one slot off |
| 0x004538b0 | 717 | 87.4 | 87.4 | b | static EAX-arg (stand-in caller for 0x00453b80); x87 keeps off+tgt sums live (fsubr st(2)), store interleave |
| 0x00454350 | 994 | 98.2 | 98.2 | b | two x87 scheduling spots (fstp before an arg push, fld before a V3 store), permuter 415 compiles no gain |
| 0x00455150 | 521 | 99.4 | 99.4 | a | one fstp scheduled one push early |
| 0x00455610 | 211 | 90.1 | 90.1 | b | ESI-passed (stand-in); x87 keeps +0x30 live through the velocity products, ours consumes it |
| 0x004556f0 | 98 | 19.4 | 19.4 | b | ESI-passed (stand-in); Ghidra size 98 B short, scored at real 541 B; vx multiply operand order |
| 0x004568d0 | 856 | 76.1 | 76.9 | b | induction-pointer bias +0x24 vs +0x2c (all displacements off by 8); rest matches |
| 0x00457100 | 1285 | 97.7 | 97.7 | a/b | atS block operand order vs push interleave (pilot residual) |
| 0x00457c10 | 512 | 73.4 | 92.8 | b | EAX static + stand-in caller; structure/ops right, register rotation from the t load on through the argument setup |
| 0x004584e0 | 138 | 87.8 | 87.8 | b | EAX/ESI static + stand-in caller; frame-pos copy scheduling and x87 pop order (fstp st(2)/st(1)) differ |
| 0x00458570 | 185 | 90.0 | 90.0 | b | stack rec + EAX idx + ESI out static with stand-in caller; dy spill to param slot in squared length differs |
| 0x00458a40 | 207 | 87.8 | 87.8 | b | record in EDI via static + stand-in caller; x87 scheduling (compare reload order, push 1 vs fadd, last fmul) |
| 0x00458b10 | 219 | 69.1 | 92.6 | b | count/frame in EBX/EDI swapped vs original; dot product loads the frame field first in the original |
| 0x00458e00 | 274 | 99.0 | 99.0 | b | one fld scheduled after dec esi in the slot-distance loop |
| 0x00459000 | 655 | 92.7 | 92.7 | b | clamp compares (constant loaded vs memory operand) + fsub/add ebx interleave in state 1 |
| 0x00459620 | 2727 | 63.7 | 72.8 | b | structure+frame 0x110 matched (union at 0x70, reused V3/V4 buffers, literals); register allocation of loops 1-2 (param_2 in EDI, n in memory) and scalar slot packing not reproduced, out of budget |
| 0x0045a190 | 515 | 93.0 | 93.0 | b | add edi,-0x58 vs sub edi,0x58 from the loop-walker copy; x87/push scheduling in 3 spots |
| 0x0045a3a0 | 115 | 81.0 | 90.5 | b | loop body exact; original keeps param_1 in EBP from entry, ours loads it after the loop |
| 0x0045a950 | 594 | 92.3 | 92.9 | b | memset zeroing via xor ecx (original immediates); last-block push/x87 scheduling |
| 0x0045ac40 | 295 | 61.5 | 66.1 | b | EAX/ECX static callee; inner-loop start not strength-reduced as its own induction, j not in EBP, x87 product order |
| 0x0045ae80 | 886 | 80.7 | 80.7 | b | record loops become down-counters with EBX biased +0x10 (original: unbiased EBX + spilled copy, pointer tests) |
| 0x0045b390 | 839 | 96.9 | 96.9 | b | x87/push scheduling in the v block and dir2.y load (0x0045bfe0 is 0-arg, start/end Line struct) |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit. 0x004556f0 reads
19.4% only because the csv size is 98 B; helper h4 scored it at 98.8% over the real 541 B (not counted as matched).

Held static callees: many sources define a callee in-file for the DDK's register conventions (pilot finding 1,
`holds` lines in `helper_notes.txt`). Several files hold each other's callees with full `// MATCH:` lines (e.g.
00454170.c / 00454790.c). None of those files is swapped in this branch. A swap of one needs the round-2 `HELD`
conversion (SWAP_BRIEF.md) for every other `// MATCH:` line in it.

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, x87, port symbol, install, source blob, match, relocheck,
old body. Helper reports: `D:/MashedWT/MATCH-S09_data/swap_w1.txt` .. `swap_w3.txt` (summarised here).

| targets | rows |
|---|---|
| exe only | 3: 0x00451990 (WarzonePontoonBoxCarry), 0x00451a50 (WarzonePontoonTrackBody), 0x00451cc0 (WarzonePontoonDeactivate) |
| exe + asi | 2: 0x004576b0 (AnyActive4576b0), 0x00458fa0 (TtlSet68b1b0) |
| asi only | 14: 0x00452160, 0x00452ea0, 0x00452eb0, 0x00453f30, 0x00454a30, 0x00455b40, 0x00459540, 0x0045a0d0, 0x0045a0f0, 0x0045a110 (PromoLoop leaves), 0x00458f20, 0x00458f80 (PromoLoop_sessionB), 0x004522d0 (Audio/MixedC3Sweep.cpp), 0x0045b350 (Input/DirectInput.cpp) |

- x87 TUs added to `mashedmod/x87_tus.txt` (4, block after `WarzonePontoon`): MatchS09_00451990, _00451a50,
  _00452eb0, _00458fa0. 0x00451a50 only copies matrices, it is listed because its old body sat in the x87 pontoon TU.
- `re/tools/dual_copy_allowlist.txt` unchanged. `py -3.12 scripts/lint_rva_bodies.py` ->
  `2 finding(s): CROSS-TARGET=1, DUP-IN-TARGET=1 | allowlisted=2 NEW=0`.
- Pontoon swaps keep `BoxCarry` / `TrackBody` / `Deactivate` as one-line C++ forwarders (no RVA comment) to the
  matched symbols, so no pontoon caller changed. Binds: `FUN_004c4dc0` -> `RwMatrixInvertEntry`, `FUN_004c4670`
  (a `jmp 0x004c4680`) -> a forwarder to `Math::RwMatrixOrthoNormalizeLut`, the callee the old TrackBody used.
- **0x00451990: one input-domain difference.** The old BoxCarry wrote the box test negated
  (`box0<=x || x<=box3 || ...`), so a NaN x or z counted as inside. The matched source, like the original, counts it
  as outside. Finite inputs give the same result. The gates (below) are the check that this does not move.
- 0x00458f80.c also defined 0x00458f20, which has its own 00458f20.c: the copy in 00458f80.c is now `HELD`, and
  `FUN_00458f20` binds to `Set458f20` (the 00458f20.c wrapper). 0x00458f80's old naked body called the original
  0x00458f20 by raw address.
- 0x004522d0: the old "body" was Audio/MixedC3Sweep.cpp's static raw-address trampoline `Orig_Free`; it is now a
  forwarder with no RVA comment that calls the matched `RwGlobalsFree4522d0`. Same one-argument call through
  `[0x007d3ff8]+0x10c`.
- C++-compat edits (DDK score unchanged, match.py 100%, relocheck OK after each): 00451cc0.c (`code` typedef with
  arity under `__cplusplus`), 004576b0.c (one pointer cast), 00458f80.c (HELD copy), 00458920.c (its 0x00458570 copy
  is HELD; file not swapped).

**7 byte-identical rows NOT swapped (NOT-SAFE).**

| rva | old body | reason |
|---|---|---|
| 0x00451690 | WarzonePontoon.cpp LoopCb | matched reads the interpolator at `*(*(obj+0x50)+0x20)` (RW RpHAnimHierarchy). Port `+0x50` is the TU's own Hier struct |
| 0x00451820 | WarzonePontoon.cpp Trigger | car count / matrices come from PontoonHost callbacks, not 0x0040e340 / 0x0046d4a0; 0x0046be10 inlined (no port body) |
| 0x00451ad0 | WarzonePontoon.cpp Coupling | old body leaves out printf, 0x0046bf50, the four wheel blocks and 0x00480720 (no port bodies, U-9738); adds host car_carried |
| 0x00451ce0 | WarzonePontoon.cpp Sample | matched calls 0x00550bc0 on the RW hierarchy; port reads its own Hier struct |
| 0x00451d80 | WarzonePontoon.cpp StateMachine | drives the port's own anim structs through C++; state 5 uses host sphere_visible (U-9740) |
| 0x00452170 | WarzonePontoon.cpp ResetCb | old body omits 0x0047d180 x2, printf, 0x004518f0 (no port bodies); signature differs |
| 0x00458920 | PickupCollect_naked.inc PK_458920 | matched reads 1.0f from `.rdata` 0x005ce548, which reads 0 in the exe (zero arena pad); the port reads `g_pk_c_5ce548`. Also needs an EAX/ESI forwarder to the naked PK_458570. Follow-up row: rebind or seed the constant |

Pontoon sources with no RVA body anchor in the port (report only): 0x00451760 (inline block in InitFrom),
0x004527f0 (Pontoon_Init), 0x00452b20 (Pontoon_Tick), 0x00452ad0 / 0x00452af0 (Pontoon_Reset). The port versions
leave out trees, water, craters and RWP parts, so none is the same function.

**81 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68; they wait for
a caller or a top-down row): 0x004516d0, 0x00451730, 0x00451760, 0x00452130, 0x00452140, 0x004523f0, 0x004527f0, 0x00452ad0, 0x00452af0, 0x00452b20, 0x00452ec0, 0x00452f00, 0x00452f30, 0x00453100, 0x00453210, 0x004532f0, 0x00453eb0, 0x004540c0, 0x00454130, 0x00454170, 0x004541e0, 0x00454790, 0x004547c0, 0x00454820, 0x004548a0, 0x004548e0, 0x00454a20, 0x00454bd0, 0x00454c00, 0x00454c10, 0x00454c60, 0x00454cd0, 0x00454f80, 0x00454fa0, 0x00454ff0, 0x00455030, 0x00455060, 0x00455100, 0x00455910, 0x00455b50, 0x00455fe0, 0x00456040, 0x00456140, 0x00456760, 0x004568a0, 0x00456c30, 0x00456c70, 0x00456c90, 0x00456ce0, 0x00456d00, 0x00456eb0, 0x00456f70, 0x004570a0, 0x00457610, 0x00457650, 0x004577f0, 0x00457ba0, 0x00458020, 0x00458080, 0x00458360, 0x00458630, 0x004587a0, 0x00458880, 0x004588c0, 0x00458bf0, 0x00458c70, 0x00458d00, 0x00458dd0, 0x00458fd0, 0x00459290, 0x004593b0, 0x00459400, 0x00459480, 0x00459560, 0x004595c0, 0x0045a130, 0x0045a530, 0x0045a590, 0x0045b360, 0x0045b930, 0x0045b990.

## Exe callers (L-68)

Binary witness on this build: `py -3.12 D:/MashedWT/MATCH-S09_data/tools/exe_reach.py mashed_re.exe mashed_re.map
<symbols>` (S07/S08's tool: direct `call`/`jmp rel32` graph from the linker map, walked from `_WinMain@16`;
`exe_reach.tsv`).

| rva | symbol | direct chain from WinMain |
|---|---|---|
| 0x004576b0 | AnyActive4576b0 | PK_459000 <- Gameplay::Pickups_MaybeReplay <- TrackRenderer::Load <- WinMain (naked call, PickupTick_naked.inc:82) |
| 0x00458fa0 | TtlSet68b1b0 | PickupField::InitReal <- TrackRenderer::InitPickups <- WinMain (D3d9Render/PickupField.cpp:209) |
| 0x00451990 | WarzonePontoonBoxCarry | Coupling <- StateMachine <- RunReplay <- Pontoon_Init <- TrackRenderer::Load <- WinMain (shortest; also under Pontoon_Tick) |
| 0x00451a50 | WarzonePontoonTrackBody | RunReplay <- Pontoon_Init <- TrackRenderer::Load <- WinMain (shortest; 4 sites incl. Pontoon_Tick WarzonePontoon.cpp:767/768) |
| 0x00451cc0 | WarzonePontoonDeactivate | Pontoon_Init <- TrackRenderer::Load <- WinMain (4 sites: StateMachine states 3/5, Pontoon_Reset) |
| 14 asi-only rows | | none (exe-callers=0) |

A direct chain shows the exe calls the body. It does not show the gate scenario executes it (no per-function trace
was taken). The pontoon functions run only on Warzone (course 0x21); the gate scenario does not load that track, so
the gates give no evidence those paths ran. 0x004576b0 sits under `Pickups_MaybeReplay`.

## Gates

@GATETABLE@

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here: `mashedmod/src` is in `full_set_paths`, so the
train runs it on enqueue. Its 13-track set is the run that would execute the pontoon swaps (Warzone).

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

Common evidence: `match.py re/match/s09/<va8>.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`, `relocheck.py` OK with
`unchecked=0`, one body (`Match/MatchS09_<va8>.cpp`), exe caller chain from `WinMain` (table above), gates unchanged
on this build. The `file` cell moves to the wrapper.

| rva | name | level | match.py | relocheck | old body (removed) |
|---|---|---|---|---|---|
| 0x004576b0 | AnyActive4576b0 | C3 -> C4 | 20/20 ins | checked=2 | Gameplay/PickupPredicates.cpp |
| 0x00458fa0 | TtlSet68b1b0 | C3 -> C4 | 100% (38 B) | OK | Gameplay/PickupPoolSpawn.cpp |
| 0x00451990 | FUN_00451990 (WarzonePontoonBoxCarry) | C2 -> C4 (re-classify to confirm the match route admits a jump from C2) | 100% (191 B) | OK | Track/WarzonePontoon.cpp BoxCarry |
| 0x00451a50 | FUN_00451a50 (WarzonePontoonTrackBody) | C1 -> C4 (same question, from C1) | 100% (128 B) | OK | Track/WarzonePontoon.cpp TrackBody |
| 0x00451cc0 | FUN_00451cc0 (WarzonePontoonDeactivate) | C2 -> C4 (same question) | 10/10 ins | checked=0 | Track/WarzonePontoon.cpp Deactivate |

CHANGELOG line per row: `2026-11-0x  0x<rva>  <name>  C<n>->C4  route match (c4=match), exe-callers via <chain>;
evidence: verify/matchs09_20261102/RESULT_MATCH-S09.md`.

`notes` for the three pontoon rows: Warzone-only, the default gates do not load Warzone (the merge train's 13-track
pass does). `notes` for 0x00451990: the swap changed the NaN case of the box test to the original's (old port body
counted NaN as inside). `notes` for 0x004576b0: reached under `Gameplay::Pickups_MaybeReplay`.

All 14 asi-only swaps: stay at their level, `notes` += `match evidence: identical, swapped, exe-callers=0; C4 when
a caller lands (L-68 ruling)`.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat`, Build OK, .asi not deployed (`original\mashed_re_dev.asi` still
dated 2026-10-24). @BUILDHASH@

## Raw files (not committed; `D:/MashedWT/MATCH-S09_data`)

@RAW@
