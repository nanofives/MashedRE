# RESULT MATCH-S06, 2026-11-01

**Verdict: @VERDICT@.** Slice 0x00426070..0x0042ae10 (128 game functions, `verify/matchscope_20261031/slices.tsv`).
122/128 sources are byte-identical under `re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178,
`/O2 /GS /Ob1`, C, x87), and every one of the 122 has `relocheck.py` OK. 104 of them are now the port's only body
(102 wrapper TUs `Match/MatchS06_<va8>.cpp`; 0x00426460 and 0x00429240 sit in their holder's wrapper): 85 in
`mashed_re.exe` and the .asi, 19 in the .asi only. Gates on this build: @GATES@.
**C4 candidates: 10 rows** (9 C3 -> C4, plus 0x00426cc0 already C4 by the frida route gaining the match route).
The other 75 exe swaps have no exe caller reachable from `WinMain` (L-68).

Resumed after the 2026-11-01 reboot. The pre-reboot session had finished the static leg (helpers h1-h12) and batch 1
of the swap leg (sw1, sw2: 34 RVAs, built OK); batch 2 (sw3-sw6) was briefed but had not started. This pass: WIP
commit of the 63 uncommitted files (eb05d626f), `git -c merge.autoStash=false merge orch/integration` (a1cf8e891:
three naked `.inc` conflicts and the allowlist, resolved by taking integration's side and re-applying the batch-1
renames, below), batch 2 by four helpers, two build fixes, rebuild (ab947d820), gates.

## Match (static leg)

| | count |
|---|---|
| slice functions | 128 (`D:/MashedWT/MATCH-S06_data/slice_rvas.txt`) |
| byte-identical, relocheck OK | 122 (`results.tsv`, `relocheck.tsv`; unchecked relocations are `__security_check_cookie` / `__security_cookie` / `__ftol` only) |
| below 100% | 6 (table below; permuter run on each) |

0x00426700 and 0x0042ac00 are identical by the whole-symbol byte rule (Ghidra's size stops before the real end):
`match.py --bytes` prints `BYTE-IDENTICAL (relocations masked)` (column `by` = `bytes`). Every `.c` the batch-2
helpers edited (C++ compatibility, `MATCH_STATIC`, `MATCH-HELD` copies; 26 functions in 22 files) was re-run after
the edits: 100% (0x0042ac00 bytes-identical as before), relocheck OK.

| rva | size | best % | class | residual (helper notes, `D:/MashedWT/MATCH-S06_data/notes/h*.md`) |
|---|---|---|---|---|
| 0x00426810 | 672 | 97.9 | b | 3 scheduling swaps in the `_ftol` colour lerps; dot written z + (x + y); permuter 586 compiles, no gain |
| 0x004273e0 | 234 | 90.4 | b | int/x87 interleave around the second scale; the colour-local form fixes x87 order but swaps ECX/EDX |
| 0x00428450 | 306 | 84.6 | b | h's int->float temp lands in param_2's dead slot; the original packs it into the unwritten rect.x field |
| 0x00429310 | 644 | 98.3 | b | one load scheduled after, not before, the deferred `add esp,0x48` before the last FUN_00430b00 |
| 0x00429bd0 | 575 | 99.4 | b | `push 6` scheduled before the edx load of [0x7d3ff8] in the 2nd render-state call |
| 0x0042aa00 | 160 | 76.8 | b | step/bias swap EBP<->EBX (the original loads step in the prologue) |

## Swap (port leg)

`swaps.tsv`: one row per slice RVA (wrapper, targets, kind, level, match %, source blob id, exe call sites, reach).

| | rows |
|---|---|
| swapped, exe + asi | 85 |
| swapped, asi only | 19 |
| of the 104: replaced an existing port body | 68 |
| of the 104: new body (no port body before; callee or caller of a swapped body, top-down L-72) | 36 |
| identical, not swapped: no port body and no swapped caller | 16 (0x00426630, 0x00426640, 0x004266f0, 0x00426700, 0x00426780, 0x00426ab0, 0x00426b40, 0x00426c50, 0x00426c90, 0x00426e00, 0x00427620, 0x004283a0, 0x00428400, 0x004298e0, 0x0042ad10, 0x0042add0) |
| identical, NOT-SAFE (left as is, below) | 2 |

Helper reports: `D:/MashedWT/MATCH-S06_data/notes/sw1.md` .. `sw6.md`.

- **Asi only (19).** 13 batch-1 swaps whose old body was asi-only (0x00426070, 0x004260c0, 0x00426670, 0x004266b0,
  0x00426cd0, 0x004274d0, 0x00428390, 0x00429840, 0x00429860, 0x0042a920, 0x0042a940, 0x0042a980, 0x0042a9f0) stay
  asi-only, as the old bodies were. 6 new bodies: the track-load chain 0x00426340 / 0x004264d0 (with 0x00426460) /
  0x00426e10 / 0x004270f0, and 0x004275d0. Their callees have port bodies only in asi-only TUs (S04 `TrackNode*`,
  `LowRvaSetters_o2`, `PromoLoop_sessionB`, `HUD/FontCtx.cpp`), so the exe link failed with them in. Moving those TUs
  into the exe was not done (it would pull their other bodies into the exe). None of the six has an exe caller.
- **Exe-side raw forwarders.** Where a callee's port body is asi-only, the wrapper binds it per target: the port body
  in the .asi, a raw-address forwarder `MatchS06_<va8>_Call<callee>` in the exe. The original's VA holds nothing in
  the standalone, so these paths cannot run in the exe. None of them is reachable from `WinMain` (table below).
- **0x0040e470** (0x0042ae10 `MenuReadinessCheckA`): bound to `CarSlotStateGet` (`Ai/CarSlotState.cpp`), as S05/S07.
- **0x005554d0** (0x00427680, 0x00428320): its port body `FontText_StringWidthAccumulator` (`HUD/HudBatch.cpp`) is
  under `#if 0`, so both targets use the raw forwarder (the .asi reaches the original, the exe has no caller).
- **Held static callees:** canonical bodies 004277a0.c, 00427680.c, 00427840.c, 00427880.c (`MATCH_STATIC`),
  0x00429240 in 00429290.c, 0x00426460 in 004264d0.c. Every other holder's copy is `// MATCH-HELD:` under
  `#ifndef __cplusplus`. 0x0055deb0 in 004292d0.c is MATCH-HELD, bound to `RwpWorldSolverHandle`.
- **Merge fix-ups:** integration's `Race/PathProgStep_naked.inc`, `Ai/AiLeafTargets_naked.inc`,
  `Ai/AiTargetClosest_naked.inc` carried the naked copies PP_426090/426bb0/426cb0/426cc0/426cf0/426d00, AL_426bb0,
  TG_426bb0 that batch 1 had removed; they were removed again and their calls renamed to the matched symbols. Two
  TUs new from integration still named removed copies in their exe branch: `Race/PathProgDot.cpp` (PP_426cc0 ->
  `VehicleTable4cPtrGet`) and `Gameplay/RangeTable_ah1.cpp` (PP_426090 -> `GlobalDat0066ce58Get`). Both are the
  same instructions (`mov eax, imm / ret` and `lea` of the same table).
- **Allowlist:** the seven keys of the swapped RVAs (DUP 00426090, 00426bb0, 00426cb0, 00426cf0, 00426d00, CROSS
  00426bb0, 00426cc0) are deleted; comment line 47 now names `Match/MatchS06_0042ad90.cpp`. `lint_rva_bodies.py`:
  18 known, NEW 0. `x87_tus.txt`: 38 S06 wrappers in one block after `WarzonePontoon`. `build_objs.ps1` unchanged.
- **Swaps that move the port toward the original** (gate watch, all called out by the helpers): 0x00429300 (no
  float rounding of the int at 0x008991b8), 0x00429870 (signed, unrounded lap sums), 0x00429aa0 (CRT `_ftol` in place
  of a call to the original's 0x004a2c48), 0x004295a0 (now runs the matched 0x00427e00 body). None has an exe caller.
- **Empty TUs** left in the rsp lists (header + include only): `Frontend/GlobalGetters_s5.cpp`,
  `Frontend/FrontendLeaves_ad2.cpp`, `Frontend/TextCtrlCodeRemap.cpp`, `Frontend/MenuMenusBB.cpp`,
  `Frontend/TextMeasure.cpp`, `Frontend/MenuChrome.cpp`, `Frontend/KvFind.cpp`, `Frontend/ViewportScaledRectDraw.cpp`,
  `Util/PromoLoop_round39.cpp`, `Util/TimeRecordWrite_ag.cpp`. Removing them from the rsp lists is a follow-up.

### Not swapped: NOT-SAFE (follow-ups)

- **0x004274e0** `LocalizationFileLoad` (`Frontend/SkeletonAndScatter_t6.cpp`, asi): the old body returns 0 for
  `DAT_007f0f60 == 5`; the matched source (= the original) opens `USA.dat` (0x005cd5ac). The old body is also wrapped
  in `ShadowAB::Run` (`MASHED_SHADOW_AB`). A behaviour change: default-OFF knob + PREREG, or a decision row.
- **0x004282a0** `MenuMenusBA` (`Frontend/MenuMeasure.cpp`, exe + asi): the exe side reads port-owned cells
  (`exe_main.cpp:3367-3371`: `GetMenuMessage` over `g_msg_dat`, `g_font` glyph sum, `s_menuMeasureDiv`), called by
  `MeasureMenuMessage`. Swapping would bring back the CROSS-TARGET dual copy D4-DC-82a0 removed.
- **0x0042ac90** (swapped) note: `MenuEntryWalk` in `Frontend/MenuNav.cpp` has no RVA comment and walks the port's own
  nav table; `MenuNavSM.cpp:1285`/`:1399` call it. The exe's menu path runs that walk, not the matched
  `MenuEntryGet` (0 exe call sites). Not a second body by the lint rule.

## Exe callers (L-68)

`py -3.12 verify/matchs06_20261101/tools/exe_reach.py mashed_re.exe mashed_re.map <symbols>` (S07's tool: direct
`call`/`jmp rel32` graph from the linker map, BFS from `_WinMain@16`), `exe_reach.tsv`. 32 of the exe symbols have
direct call sites, 10 reach `WinMain`:

| rva | symbol | level | exe sites | chain from WinMain |
|---|---|---|---|---|
| 0x00426090 | GlobalDat0066ce58Get | C3 | 8 | RangeTableMemberTest <- PP_408610 <- Race::PathProg_MaybeReplay <- TrackRenderer::Load |
| 0x00426bb0 | Global66d6d8Get | C3 | 5 | TG_414a70 <- Ai::TargetClosest_MaybeReplay <- TrackRenderer::Load |
| 0x00426bc0 | GetDat0066d6e0 | C3 | 1 | SnapNodeToSafeStartLines <- Race::RoundGrid_MaybeReplay <- TrackRenderer::Load |
| 0x00426bd0 | GetTableEntry0066d658 | C3 | 1 | SnapNodeToSafeStartLines <- Race::RoundGrid_MaybeReplay <- TrackRenderer::Load |
| 0x00426cb0 | SlotIndexToPtr | C3 | 7 | PP_407e20 <- PP_408610 <- Race::PathProg_MaybeReplay <- TrackRenderer::Load |
| 0x00426cc0 | VehicleTable4cPtrGet | C4 (frida) | 4 | PP_407e20 <- PP_408610 <- ... |
| 0x00426cf0 | GetDat0066d6e4 | C3 | 1 | PP_408610 <- Race::PathProg_MaybeReplay <- TrackRenderer::Load |
| 0x00426d00 | FrontendArraySlotGet | C3 | 8 | PP_407e20 <- PP_408610 <- ... |
| 0x0042a9c0 | ModeCodeLookup | C3 | 2 | PromptStripResolve <- PromptStripAppend <- Frontend::Nav_DevGoto |
| 0x0042ad90 | Find42ad90 | C3 | 9 | Frontend::Nav_DevGoto |

The other 22 symbols with call sites are called only from other callerless S06 bodies (e.g. `FUN_00427e00` 35 sites,
all in new bodies). The remaining 75 exe swaps: 0 sites or no path from `WinMain`.

## Gates (this build)

@GATETABLE@

The merge train's `-Full` pass was not run here: `mashedmod/src` is in `full_set_paths`, so the train runs it on
enqueue.

## C4 re-classify text (apply after the merge, through re-classify; not applied here)

All rows: route `match` (c4=match), toolchain ddk (cl 13.00.9176 / c2 13.00.9178), flags `/O2 /GS /Ob1`, gates
unchanged on this branch's build (@GATESHORT@), evidence `verify/matchs06_20261101/RESULT_MATCH-S06.md`. Source
blob ids (sha256/16) from `results.tsv`.

| rva | name | level | exe_file (new) | notes text |
|---|---|---|---|---|
| 0x00426090 | GlobalDat0066ce58Get | C3 -> C4 | `mashedmod/src/mashed_re/Match/MatchS06_00426090.cpp` | `match evidence: identical (re/match/s06/00426090.c 9571180ac6d961b5, 100%), relocheck OK checked=1 unchecked=0, swapped, exe-callers=8 (RangeTableMemberTest, PP_407b00..., reach WinMain via PathProg_MaybeReplay), gates unchanged (MATCH-S06)` |
| 0x00426bb0 | Global66d6d8Get (CameraPath::GetCount) | C3 -> C4 | `.../Match/MatchS06_00426bb0.cpp` | `match evidence: identical (re/match/s06/00426bb0.c 534c18317f3903fc, 100%), relocheck OK checked=1 unchecked=0, swapped (replaces naked PP_/AL_/TG_426bb0), exe-callers=5 (TG_414a70, AL_ / PP_ callers), gates unchanged (MATCH-S06)` |
| 0x00426bc0 | GetDat0066d6e0 | C3 -> C4 | `.../Match/MatchS06_00426bc0.cpp` | `match evidence: identical (re/match/s06/00426bc0.c 99e0ccb62b7f8306, 100%), relocheck OK checked=1 unchecked=0, swapped, exe-callers=1 (SnapNodeToSafeStartLines), gates unchanged (MATCH-S06)` |
| 0x00426bd0 | GetTableEntry0066d658 | C3 -> C4 | `.../Match/MatchS06_00426bd0.cpp` | `match evidence: identical (re/match/s06/00426bd0.c d40fd1269c1cbfda, 100%), relocheck OK checked=2 unchecked=0, swapped, exe-callers=1 (SnapNodeToSafeStartLines), gates unchanged (MATCH-S06)` |
| 0x00426cb0 | FUN_00426cb0 (SlotIndexToPtr) | C3 -> C4 | `.../Match/MatchS06_00426cb0.cpp` | `match evidence: identical (re/match/s06/00426cb0.c b1418ef203d3c181, 100%), relocheck OK checked=1 unchecked=0, swapped (replaces naked PP_426cb0), exe-callers=7 (PP_407e20 et al.), gates unchanged (MATCH-S06)` |
| 0x00426cc0 | VehicleTable4cPtrGet | C4 -> C4 (route match added to the frida C4) | `.../Match/MatchS06_00426cc0.cpp` | `match evidence: identical (re/match/s06/00426cc0.c ac6a326bf8033df7, 100%), relocheck OK checked=1 unchecked=0, swapped (replaces naked PP_426cc0), exe-callers=4 direct (PP_407e20) plus Dot408590 through s_fn_426cc0, gates unchanged (MATCH-S06)` |
| 0x00426cf0 | GetDat0066d6e4 | C3 -> C4 | `.../Match/MatchS06_00426cf0.cpp` | `match evidence: identical (re/match/s06/00426cf0.c da93cbc2cce54ab3, 100%), relocheck OK checked=1 unchecked=0, swapped (replaces naked PP_426cf0), exe-callers=1 (PP_408610), gates unchanged (MATCH-S06)` |
| 0x00426d00 | FrontendArraySlotGet | C3 -> C4 | `.../Match/MatchS06_00426d00.cpp` | `match evidence: identical (re/match/s06/00426d00.c 116d7d449582f588, 100%), relocheck OK checked=1 unchecked=0, swapped (replaces naked PP_426d00), exe-callers=8 (PP_407e20 et al.), gates unchanged (MATCH-S06)` |
| 0x0042a9c0 | ModeCodeLookup | C3 -> C4 | `.../Match/MatchS06_0042a9c0.cpp` | `match evidence: identical (re/match/s06/0042a9c0.c bf321af12b82ee41, 100%), relocheck OK, swapped, exe-callers=2 (PromptStripResolve <- Nav_DevGoto), gates unchanged (MATCH-S06)` |
| 0x0042ad90 | FUN_0042ad90 (Find42ad90) | C3 -> C4 | `.../Match/MatchS06_0042ad90.cpp` | `match evidence: identical (re/match/s06/0042ad90.c 8adb7c60c907c133, 100%), relocheck OK, swapped, exe-callers=9 (Frontend::Nav_DevGoto), gates unchanged (MATCH-S06)` |

CHANGELOG line per row: `2026-11-0x  0x<rva>  <name>  C3->C4  route match (c4=match), exe-callers=<n>
(Match/MatchS06_<va8>.cpp); evidence: verify/matchs06_20261101/RESULT_MATCH-S06.md`.

- **Rows already C4 by the frida route with a callerless swap** (0x00426c10, 0x00426c30, 0x00426c70, 0x00427780,
  0x004295a0, 0x0042a920, 0x0042aad0, 0x0042aae0, 0x0042ae10): stay C4 (frida route); notes gain
  `match evidence: identical, swapped, exe-callers=0` (no match route until a caller lands).
- **every other swap**: stays at its level, notes `match evidence: identical, swapped, exe-callers=0; C4 when a
  caller lands (L-68 ruling)` (or `exe-callers=<n> from callerless S06 bodies` per `exe_reach.tsv`).
- 0x0042a9c0 and 0x0042ad90 reach `WinMain` only through `Frontend::Nav_DevGoto` (the dev goto path). Under the L-68
  letter (an exe caller exists and reaches `WinMain`) they qualify. The lead may hold them below C4 if dev-only paths
  do not count.

## Build

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` (test-slot wrapped), Build OK, .asi not deployed. `[rva-lint]` 18
known, NEW 0; `[asm-lint]` OK. Two failed links on the way: build3 (exe: the four-RVA track chain's asi-only callees,
and the PathProgDot / RangeTable_ah1 references) and build4's first try (asi: `FontText_StringWidthAccumulator` under
`#if 0`). exe sha256 `09d01899b049130c2fe8800d1cb41d57eb99e6c119eb78b180425c6f6b007431`, asi
`a25648defe12a15762187a6d74c4e3c7793044aa03035c4719ba297e987886ea`.

## Raw files (not committed; `D:/MashedWT/MATCH-S06_data`)

```
8586b5b47aa06edaa04f1cb88376182ab123223059bb9887307e8e610610af72  decomp.json
6a4028d33670bb4c3b00b6b4319924c364bf4e257799be3cce106e221eb09f9d  bodies.tsv (pre-swap body scan)
072259da465dec074a1fa8ce9d15180475f5d3997091fe44ab90bc85a05dfb70  build4.log
@RAWGATES@
```
