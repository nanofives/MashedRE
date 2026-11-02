# RESULT MATCH-S08, 2026-11-01

**Verdict: GATES_VERDICT.** Slice 0x00432260..0x00451680 (128 game functions). **93/128** sources are
byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87), and
every one of the 93 has `relocheck.py` OK (`relocheck.tsv`; the 2 BAD lines there are the in-file copy of S07's
0x00430910 at 61.7% inside `004322c0.c` / `004323c0.c`, not a slice row). **29** of them are now the port's only
body (29 wrapper TUs `Match/MatchS08_<va8>.cpp`): 9 in `mashed_re.exe` (all also in the .asi), 20 in the .asi only.
Gates on the merged build: GATES_LINE.
**C4 candidates: 4 rows** with an exe caller reachable from `WinMain` (table below). The other 5 exe swaps have no
exe caller (L-68).

**Top-down rows (L-72):** T07's pause menu **0x0043d7c0 is byte-identical** (1844 B whole symbol, 0 differing
unmasked bytes; instruction score 99.6% only because Ghidra's size stops before the `add esp`/`ret`; relocheck OK,
165 checked, unchecked `__ftol`, `PostQuitMessage`). T11's **0x0043bf30 is 100%** (raw converter, 57/57, relocheck
OK). T11's **0x00434720 (8369 B) is 57.1%** (regs 70.5%, ops 97.1%: hand rewrite from the listing, residual is
stack-slot packing and an ebx/edi swap, class b). None of the three is swapped here: those rows belong to the
T07/T11 worker. Their sources are `re/match/s08/0043d7c0.c`, `0043bf30.c`, `00434720.c`.

Branch work: cut from orch/integration 87a4527d2, merged orch/integration 6d9d01a7f (MATCH-S07) with
`merge.autoStash=false` before the swap leg (c1b7e3c15, clean), so held callees bind to S07's bodies.

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`slice_rvas.txt`, from hooks.csv by `tools/mkslice.py`, the cut_slices.py rule) |
| raw converter (g2s) at 100% | 28 |
| hand-matched to 100% (helpers h1-h11, `helper_notes.txt`, `helper_assign.txt`) | 65 |
| byte-identical by the whole-symbol rule (`by` = `bytes`) | 3 (0x0043d7c0, 0x004425d0, 0x00448940) |
| below 100% | 35 (table below; budget spent) |

Tool-data fix: `re/console/pc_funcs.csv` lacked 6 functions of this slice (0x00448940, 0x004492b0, 0x004496a0,
0x00449710, 0x0044ca40, 0x00451680), so `match.py` refused to score them. Their sizes come from the Ghidra decomp
(`decomp_pc.py --create` for 0x00448940 and 0x00451680, which the pool clone lacked; the other four exist in the
clone and in `re/console/cache/func_bounds.csv`).

| rva | size | best % | regs % | class | residual (helper notes) |
|---|---|---|---|---|---|
| 0x00432b30 | 1327 | 74.2 | 74.2 | b,c | static eax/esi register-arg function with stand-in caller; residual cross-jumping of identical case tails (343 vs 357 ins) |
| 0x004332a0 | 835 | 97.3 | 97.3 | b | block layout: original puts the slot.v = -1 block after the epilogue, ours inline |
| 0x004335f0 | 2376 | 64.8 | 75.3 | b | structure right (ops 92%), stack layout/regalloc and pal[0] forwarding in the id==0 branch; permuter's 71.4% changes behaviour, rejected |
| 0x00433f40 | 997 | 99.3 | 100.0 | b | one add-operand register pair at the first FUN_004282a0 arg (eab4/eadc load order); switch cases kept apart by constant spellings |
| 0x00434720 | 8369 | 57.1 | 70.5 | b | hand rewrite from disasm, ops 97.1%; residual = frame slot packing (use-count + liveness) and ebx/edi swap for zero vs n, not found in budget |
| 0x004368e0 | 10415 | 25.1 | 25.0 | b | raw body plus uchar param and static b8b0/b8c0 only (60 min cap); struct-by-value draws, goto structure, raw arity artefacts owed |
| 0x00439210 | 5109 | 40.5 | 58.2 | b | structure in place (ops 83%), 60 min cap: stack layout/regalloc/x87 scheduling in the two row loops |
| 0x0043a610 | 1050 | 35.1 | 42.9 | b | structure right (ops 84%), alpha in bl where the original uses a stack byte and ebx constants (regalloc/stack layout) |
| 0x0043aa30 | 1187 | 50.5 | 58.7 | b | structure right (ops 91%), stack-slot layout/regalloc not found (locals placed by use, not declaration order) |
| 0x0043af10 | 4061 | 22.9 | 29.7 | b | raw converter body + compile fixes; Ghidra merged pushes into wrong callees, needs rewrite from the listing |
| 0x0043c5b0 | 3292 | 40.5 | 61.1 | b | structure by hand; local slot layout and loop pointer bias differ (60 min cap) |
| 0x0043d2a0 | 1303 | 54.0 | 64.7 | c | register-arg callee FUN_00432b30 (eax/esi, 1327 B) not in file; loop regalloc follows |
| 0x0043dfd0 | 10938 | 17.7 | 46.9 | b | raw body, float globals/prototypes fixed; regalloc across 10.9 KB body, out of budget |
| 0x004414b0 | 581 | 89.8 | 89.8 | b | x87 scheduling of the pos*-1 and at/right scale vectors (scalar kept to end in original) |
| 0x00441b30 | 329 | 86.7 | 86.7 | b | frame slots of the float locals (f6 in param_2's slot in the original) |
| 0x00441d40 | 1730 | 69.2 | 74.2 | b | original keeps the wrapped angle on the x87 stack through the rate formula; step-copy frame slot; permuter's 77.5% changed behaviour (rejected) |
| 0x00442440 | 391 | 73.8 | 68.9 | b | original pushes translate args after the last fadd; 9176 hoists them into the sum (esp offsets shift) |
| 0x004427c0 | 598 | 52.7 | 70.2 | b | min loop unrolled x8 here vs x4 in the original; param/table registers differ |
| 0x00442a60 | 531 | 99.4 | 99.4 | a | one load of the loop counter scheduled after fmul instead of before; memset intrinsic clear |
| 0x00442e00 | 627 | 65.1 | 65.7 | b | frame 0xc vs 8 (original packs dz/sqrt into param_1's dead slot), x87 spills in the pitch wobble |
| 0x00443440 | 2225 | 58.4 | 67.5 | b | register (esi/edi/ebx) and stack-slot packing differ; flow/calls line up (ops 94.1%); Ghidra size short of real end 0x00443d0d |
| 0x00443dc0 | 3464 | 57.5 | 59.0 | b | out of budget: frame layout, 4 duplicated y-compares in 2 draws (ours folds them), walk/retry block layout |
| 0x00444b60 | 1163 | 69.0 | 69.0 | b | all 336 ins in order (ops 98.8%); frame slot order (+2 free slots) and x87 pop of t*t differ |
| 0x00445aa0 | 2579 | 67.4 | 71.6 | b | out of budget: edi/ebp swap, 'off' kept in dead param_2 slot (ours const-propagates), active-branch block layout |
| 0x00446520 | 7412 | 94.9 | 94.9 | a | FLAGS /Od (function is unoptimized code); frame matched via name-hash local naming; residual = orig evaluates [p+x] operand before [ebp-x] (fld [p]; fsubr), 9176 never does (likely 9176 vs 9466) |
| 0x00448220 | 1248 | 97.0 | 97.0 | b | real size 1248 (tail jmp 0x00434360); tail block fld/fmul vs arg-push scheduling, statement orders + permuter no change |
| 0x0044bd50 | 273 | 89.5 | 89.5 | b | points-array induction base y vs z and loop store order; inline setters gave fld/fstp |
| 0x0044be70 | 289 | 89.7 | 89.7 | b | same shape and residual as 0x0044bd50 (points induction base, loop store order) |
| 0x0044bfa0 | 934 | 74.3 | 74.3 | b | stack layout of the six V3 locals (v placed above at/right); dot-product operand order follows it |
| 0x0044c490 | 94 | 95.5 | 95.5 | b | vector pointer materialization (add eax,0x40) vs folded first load; struct-copy form swaps eax/ecx |
| 0x0044c4f0 | 250 | 98.8 | 98.8 | a | store [ebx+0x3c] scheduled after the two pushes for FUN_0047cdc0 (pilot residual, no source move found) |
| 0x0044e0a0 | 239 | 98.4 | 98.4 | b | one fstp-vs-load swap after the FUN_004b4510 call |
| 0x0044e190 | 170 | 73.2 | 98.2 | b | static EBX=rec, register rotation from handles[i] on |
| 0x0044e2d0 | 60 | 81.0 | 95.2 | b | static EAX/EBX, one load in ecx vs eax reused after push (in 0044e190.c) |
| 0x00450b10 | 391 | 86.6 | 85.7 | b | uv/x+w/y+h locals + permuter order, x87 store scheduling residual |

Classes: (a) compiler-version codegen, (b) source shape not found in budget, (c) tool limit. 0x00446520 is
unoptimized code (`/Od` matches it to 94.9%; the residual operand order is (a)).

Held static callees: several sources define a callee in-file for the DDK's register conventions (listed with
`holds` in `helper_notes.txt`, e.g. 004322c0.c holds S07's 0x0042f6b0 / 0x00430830 / 0x00430910, 0043d2a0.c holds
0x0042ac00 / 0x0042ac50 / 0x0042ad90, 00434720.c holds 0x0042f8d0 / 0x00430b30 / 0x0042b8b0 / 0x0042b8c0). None of
those files is swapped in this branch; a swap of one needs the round-2 `HELD` conversion (SWAP_BRIEF.md).

## Swap (port leg)

`swaps.tsv` has one row per swapped RVA: wrapper, targets, x87, port symbol, install, source blob, match, relocheck.
Helper reports: `D:/MashedWT/MATCH-S08_data/swap_w1.txt` .. `swap_w4.txt` (summarised here).

| targets | rows |
|---|---|
| exe + asi | 9: 0x00432ad0, 0x004331a0, 0x00436810, 0x0043aee0, 0x004425d0, 0x00442cc0, 0x00443300, 0x00448700, 0x0044b000 |
| asi only | 20: 0x00433240, 0x0043c000, and 18 PromoLoop leaves (0x00432260, 0x00432290, 0x00442410, 0x00442420, 0x00442df0, 0x00443080, 0x00443090, 0x004430a0, 0x004430b0, 0x004495d0, 0x00449880, 0x0044c370, 0x0044d6e0, 0x0044dfe0, 0x0044dff0, 0x0044e020, 0x0044e050, 0x0044e070) |

- x87 TUs added to `mashedmod/x87_tus.txt` (11, block after `RwpSolverCore17`): MatchS08_004331a0, _0043c000,
  _00442420, _00442cc0, _00442df0, _00443300, _00449880, _0044dff0, _0044e020, _0044e050, _0044e070.
- rsp entries: one S08 block after `"Util\CameraEntryDispatch.cpp"` in both rsp files (mid-file, away from the other
  slices' blocks). `build_objs.ps1` unchanged. `re/tools/dual_copy_allowlist.txt` unchanged against orch/integration.
  `py -3.12 scripts/lint_rva_bodies.py` -> `25 finding(s): CROSS-TARGET=18, DUP-IN-TARGET=7 | allowlisted=25 NEW=0`.
- Callee binds: old raw-address calls now go to the port bodies that exist on this branch (0x0042d3a0
  `RaceTransitionBufZero`, 0x00402f80 `CupFloatInit`, 0x004248b0 `PerCarSnapshotInit`, 0x00424920
  `EndOfRoundAccumulator`, 0x004307a0 `ElapsedVsThresholdCheck`, 0x00430760 `IsMultiplayerMode`, 0x004464c0
  `CameraEntryDispatchAll`, 0x0042f7b0 `FrontendCursorUpdate`, 0x0042fa00 `MenuTeamSelectTick`, 0x0042aae0 via a
  forwarder to `MenuIm2DQuad`). Callees with no port body keep raw-address forwarders, as the old bodies had them
  (0x004332a0 and 0x0042c960 in 0x0043c000, 0x0047d150 in 0x0044b000).
- `.rdata` reads: the exe's copy of the original `.rdata` reads 0 (w3, below). Of the 9 exe swaps only 0x00442cc0
  reads `.rdata` (0x005d757c = 0.0, the same bits as the zero pad). Every other exe-swap global is `.data`/BSS that
  the old body also read at the same address.
- 0x00448700.c: one C++ cast at the call (`(undefined4)&DAT_00897fe0`), still 100% and relocheck OK.
- 0x0043d2a0.c: a nested `/* */` in the header comment stopped it compiling; fixed (no code change, 54.0%).

**10 byte-identical rows NOT swapped (NOT-SAFE).**

| rva | old body | reason |
|---|---|---|
| 0x004323c0 | MenuCursorBack / RowMoveBack (FrontendNav.cpp) | exe calls it on port-owned state (`g_challenge_row`, `g_game_state.*`, RowModeSync); matched source writes 0x0067f17c/84/a4 directly |
| 0x004325c0 | MenuAnimTickBody (MenuAnimTickTwin.cpp) | exe runs it on port `g_records`, env branch MASHED_NO_ANIM_TRUNC; file also holds 0x0042ac50, which has a body (Calc42ac50) |
| 0x00441820 | RaceCamera::NodeDir | reads port `led_[]` / `nodes_`; matched calls 0x00409790 (no port body), 0x004098a0 (asi-only) |
| 0x00441c80 | CamCarMidpoint441c80 | exe arm uses host `most_separated` in place of 0x0040e180 (no port body) and returns (0,0,0) on failure |
| 0x00442600 | CamEntriesInit442600 | exe arm reads CameraEntryTables.h (the original tables read 0 in the exe), host track id, Ai_RandRange, skips 0x00405540 |
| 0x004464c0 | CameraEntryDispatchAll | exe arm skips types 1 and 2; 0x00441d40 has no port body, 0x00442440 is asi-only with its install disabled (D-10793) |
| 0x00442c80 | RubberBandGate | matched reads 7.0 from `.rdata` 0x005cc9b8, which reads 0 in the exe; 0x0040e350 is the exe's Fi_GameMode stand-in. w3 verified a literal-constant form stays 100% + relocheck OK: follow-up row |
| 0x00442ce0 | RubberBandGrip | same `.rdata` issue (1.0/0.6/0.5/0.25/0.0); literal form 100% (81/81) + relocheck OK, then SAFE: follow-up row |
| 0x00448940 | ArcticTrackNodeSlot0 (S-5713 stub in the exe) | 17 of 22 callees have no port body; 0x0049a0c0 is passed as a callback address |
| 0x00448dc0 | PlaceMatrix (TrackRenderer.cpp) | a different function: the port's PlaceMatrix is a per-call helper; the matched code loops 5 tiles of 0x00896460 |

**54 byte-identical rows with no port body**, not swapped (a new body with no caller is not C4, L-68; they wait for a
caller or a top-down row): 0x004322c0, 0x00432450, 0x004324a0, 0x00432800, 0x004391b0, 0x0043bf30, 0x0043d7c0, 0x0043df00, 0x00441490, 0x00441700, 0x00441760, 0x004417e0, 0x00441990, 0x00442a20, 0x004430c0, 0x004430e0, 0x00443210, 0x00443d10, 0x00444ff0, 0x00448730, 0x00448770, 0x00448820, 0x00448880, 0x00448cb0, 0x00448e20, 0x00448ef0, 0x004491e0, 0x004492b0, 0x004496a0, 0x00449710, 0x00449b30, 0x00449ba0, 0x00449bb0, 0x0044aaa0, 0x0044ab20, 0x0044bbc0, 0x0044bc30, 0x0044c5f0, 0x0044c740, 0x0044c8b0, 0x0044c920, 0x0044ca40, 0x0044caa0, 0x0044cb00, 0x0044d5e0, 0x0044df80, 0x0044e240, 0x0044e310, 0x0044e680, 0x0044ecc0, 0x0044f830, 0x00450300, 0x00451060, 0x00451680. 0x0043d7c0 and 0x0043bf30 are among them and belong to T07/T11.

## Exe callers (L-68)

Binary witness on this build: `py -3.12 D:/MashedWT/MATCH-S08_data/tools/exe_reach.py mashed_re.exe mashed_re.map
<symbols>` (S07's tool: direct `call`/`jmp rel32` graph from the linker map, walked from `_WinMain@16`;
`exe_reach.txt`).

| rva | symbol | direct chain from WinMain |
|---|---|---|
| 0x004425d0 | UtilZeroTable8964c0 | Ai_BridgeLoad <- TrackRenderer::Load <- WinMain (TrackRenderer.cpp:648) |
| 0x00442cc0 | AiVehicleFloat4Get | TG_414c30 <- Ai::ModeSeven_MaybeReplay <- TrackRenderer::Load <- WinMain (naked call, AiModeSeven_naked.inc:174) |
| 0x00443300 | AiSplineEval_x87 | AL_443440 <- AL_414570 <- Ai::AiLeaf_MaybeReplay <- TrackRenderer::Load <- WinMain (11 naked call sites, AiLeafTargets_naked.inc) |
| 0x00448700 | VehicleSeedWritePair | TrackRenderer::UpdateCar <- RenderFrame <- WinMain (TrackRenderer.cpp:5275) |
| 0x00432ad0, 0x004331a0, 0x00436810, 0x0043aee0, 0x0044b000 | | none reachable (exe-callers=0) |

A direct chain shows the exe calls the body. It does not show the gate scenario executes it (no per-function
trace was taken). 0x00442cc0 and 0x00443300 sit under `*_MaybeReplay` paths.

## Gates

GATES_SECTION

The merge train's `-Full` pass (D2 x3, 13 tracks) was not run here: `mashedmod/src` is in `full_set_paths`, so the
train runs it on enqueue.

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

Common evidence: `match.py re/match/s08/<va8>.c` 100% under `flags=/O2 /GS /Ob1 tc=ddk`, `relocheck.py` OK with
`unchecked=0`, one body (`Match/MatchS08_<va8>.cpp`, both targets), exe caller chain from `WinMain` (table above),
gates unchanged on this build. The `file` cell moves to the wrapper.

| rva | name | level | match.py | relocheck | old body (removed) |
|---|---|---|---|---|---|
| 0x004425d0 | FUN_004425d0 (UtilZeroTable8964c0) | C3 -> C4 | whole symbol 45 B BYTE-IDENTICAL (`--bytes`; insn 80% only because Ghidra's size is short) | checked=3 | Util/UtilLeaves_ab6.cpp |
| 0x00442cc0 | AiVehicleFloat4Get | C3 -> C4 | 7/7 ins | checked=2 | Ai/AiVehicleFieldGetters.cpp |
| 0x00443300 | FUN_00443300 (AiSplineEval_x87) | C3 -> C4 | insn 100% (307 B) | checked=14 | Ai/AiSplineEval.cpp (naked asm; the .asi install stays on the AiSpline_Entry self-test dispatcher, which calls the matched symbol) |
| 0x00448700 | VehicleSeedWritePair | C2 -> C4 (re-classify to confirm the match route admits a jump from C2) | 13/13 ins | checked=4 | Vehicle/VehicleSeed.cpp |

CHANGELOG line per row: `2026-11-0x  0x<rva>  <name>  C3->C4  route match (c4=match), exe-callers via <chain>;
evidence: verify/matchs08_20261101/RESULT_MATCH-S08.md`.

`notes` for 0x00442cc0 / 0x00443300: the exe reaches them under `Ai::ModeSeven_MaybeReplay` / `Ai::AiLeaf_MaybeReplay`,
and these gates give no evidence those paths ran (CONFIDENCE.md, "How the Write-set leg relates").

All other swapped rows (5 exe+asi, 20 asi-only): stay at their level, `notes` += `match evidence: identical,
swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`. 0x0043c000 (TimerSlotTickDispatcher) is already C4
by another route and keeps it (asi-only swap).

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` (test slot + `mashed_lock.py run`), Build OK, .asi not deployed.
exe sha256 `66fa722898bbb146d28f447bcf556c1785bec31b60618bb15ca9492c31cce18f`, asi
`d643a0dc3aecf1e9616ebd0413e48258fa626f7479ba050b8259f52b6079997c`.

## Raw files (not committed; `D:/MashedWT/MATCH-S08_data`)

RAW_TABLE
