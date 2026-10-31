# RESULT MATCH-S02 (run 27): C4 by byte match, slice 0x004099a0..0x00412100

Branch `orch/MATCH-S02` off `orch/MATCH-MERGE` (pilot tools, LIB-IDENTITY). Worktree `.worktrees/MATCH-S02`.
**No tracker was edited** (MATCH-RUBRIC is not merged into orch/integration yet): this file carries everything
re-classify needs. Raw data: `D:/MashedWT/MATCH-S02_data` (SHA-256 list at the end).

## Verdict

**GREEN.** 110 of 129 functions matched byte for byte; 19 matched sources are now the only body of their RVA in
both targets; the default gates are unchanged on that exe.

- **Matched: 110 of 129** slice functions are byte-identical under `re/tools/match/match.py` with the XP DDK
  compiler and `/O2 /GS /Ob1` (C, x87), and every one of them passes the new relocation-target check
  (`relocheck.py`, below). Raw converter alone: 52 of 129. 4 more are 82.8-97.9%, 15 were not attempted.
- **Swapped: 19 RVAs** now get their body in `mashed_re.exe` AND the .asi from the matched source (one body per
  RVA, lint clean). 1 more (0x0040cf40) is swapped in the .asi only, because the exe has no body for it.
- **Not swapped: 14 matched RVAs with a scanner-visible exe body**, each for a stated reason (the port body
  computes from port state, has extra exe-only logic, or diverges from the original). See "Not swapped".
- **Gates on the 19-swap exe: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9/1d5bf4f9/1d5bf4f9`, equal to
  `merge_train.json` gctrl.ref / b.ref.**

## What "the port uses the matched source" means here (needs the rubric owner's eye)

`mashedmod/build_objs.ps1` rejects any non-`.cpp` entry in the rsp lists (line 56), so a `.c` file cannot be a
TU. Each swapped RVA therefore gets a wrapper `mashedmod/src/mashed_re/Match/MatchS02_<rva>.cpp` that does

```
#define MATCH_PORT 1
#define FUN_<rva> <PortSymbol>          // keeps the port's symbol, so no caller changes
#pragma comment(linker, "/EXPORT:<PortSymbol>=_<PortSymbol>")   // the .asi's Frida export name
extern "C" {
#include "../../../../re/match/s02/<rva>.c"
}
RH_ScopedInstall(<PortSymbol>, 0x<rva>);   // only where the old body had an active install
```

- The included text is the exact file match.py scores. It is compiled as **C by the DDK** (the match) and as
  **C++ by VS2022** (the port). Every matched source was written in the common subset and checked with a C++
  compile (`cxxcheck`). Under C++ the preamble's `MGLOBAL(T, name, addr)` becomes `static T &name = *(T *)addr`
  (the exe maps the original data range, as every existing port body assumes); under C it is `extern T name`.
  The function bodies are not touched by any macro.
- **needs Mariano**: letting `build_objs.ps1` accept `.c` entries would compile the matched source as C in the
  port too, and the wrappers could shrink to an rsp line. Not done (no `.ps1` edits from a worker).
- Callees bind to the port's existing implementation (`--bind FUN_x=Symbol`). Where the OLD body called a callee
  through its raw original address (no port implementation), the wrapper has a small `extern "C"` forwarder with
  no RVA comment that makes that same raw-address call: 0x0040bb70 / 0x0040bb90 (0x004c5c00), 0x0040e3a0
  (0x004a332b), 0x0040dfc0 (13 callees). Behaviour is identical to the old body; in the exe those addresses are not
  code, exactly as before (no exe caller reaches these functions).
- `scripts/rva_body_scan.py` gained a third pass: a `#include "...re/match/....c"` line makes every
  `// MATCH: 0x<rva>` of the included file a body of the including TU, so `lint_rva_bodies.py` still enforces one
  body per RVA. Lint after all swaps: `16 known, NEW=0` (unchanged).
- No swapped function uses float, so `x87_tus.txt` is unchanged.

## A gap in match.py, and the check that closes it

`match.py` masks every relocation, so 100% does not show that each global, callee or constant is the right one.
Helper h4 found two statement orders of 0x0040b180 that score 100% while storing to swapped arrays.
`re/tools/match/relocheck.py` (new) compiles the source like match.py and resolves every relocation of each
`// MATCH:` function: `MGLOBAL` names to their addresses (+ addend), `FUN_<rva>` to the RVA, `thunk_FUN_<rva>` to
a `jmp` thunk that lands there, own constants/strings to the bytes the original operand points at; imports and
CRT helpers are listed as unchecked by name. Negative control: moving one MGLOBAL address in 0x0040b7b0 keeps
match.py at 100% and makes relocheck BAD.
On the first full run it found **2 real errors in sources that scored 100%**: 0x0040b250 and 0x0040b810 stored to
swapped arrays. Both were fixed (statement order, still 100%, now OK). Final: every 100% source is relocheck OK
(`relocheck.tsv`; the 3 BAD rows are the unfinished 82.8-94.9% sources, whose sites do not line up).
**Recommendation for MATCH-RUBRIC / re-classify:** require relocheck OK next to match.py 100% for the `match`
route. S01 and later slices should run it.

## Swapped rows (C4-by-match candidates once MATCH-RUBRIC is merged)

Full table with blob SHAs: `swaps.tsv`. Common to every row:
- tool: `py -3.12 re/tools/match/match.py re/match/s02/<rva>.c` -> 100.0% (and `relocheck.py` OK)
- compiler: XP DDK `cl` 13.00.9176 / `c2` 13.00.9178 (`tools/vc7ddk`, toolchain `ddk`), flags `/O2 /GS /Ob1`
  (file `// FLAGS:` line; match.py adds `/Gy`), language C
- port body: `mashedmod/src/mashed_re/Match/MatchS02_<rva>.cpp`, in `exe_sources.rsp` and `asi_sources.rsp`

| rva | port symbol | old body (removed from) | callees bound | exe callers |
|---|---|---|---|---|
| 0x0040ad20 | FrontendGlobalGet | Frontend/FrontendAccessors.cpp | - | none |
| 0x0040ad30 | UtilSet8a95ac | Util/UtilLeaves_ac.cpp | - | none |
| 0x0040b250 | UtilReset8a9500 | Util/UtilLeaves_ac.cpp | - | none |
| 0x0040b430 | UtilFieldAdd8a94f0 | Util/UtilLeaves_ac.cpp | - | none |
| 0x0040b620 | SlotSortByModeScore | Frontend/MenuScoreGetters.cpp | - | none |
| 0x0040b6b0 | ModeScoreGetBySlot | Frontend/MenuScoreGetters.cpp | - | none |
| 0x0040b6c0 | FrontendArrayGet | Frontend/FrontendAccessors.cpp | - | none |
| 0x0040b6d0 | UtilArrayGet8a94e0 | Util/UtilLeaves_ac.cpp | - | none |
| 0x0040b7a0 | HotkeyStringBaseGet | Frontend/MenuScoreGetters.cpp | - | none |
| 0x0040b7b0 | PlayerHotkeyTableGet | Frontend/MenuScoreGetters.cpp | - | none |
| 0x0040b810 | TimerGlobalsReset | Frontend/TimerReset.cpp | - | none |
| 0x0040bb70 | SpriteLookupTableA | Frontend/SpriteGate.cpp | 0x004c5c00 raw forwarder | none |
| 0x0040bb90 | SpriteLookupTableB | Frontend/SpriteGate.cpp | 0x004c5c00 raw forwarder | none |
| 0x0040d250 | DoubleDerefIndexedGetter | Frontend/SmallLeaves_t1.cpp | - | none |
| 0x0040dfc0 | HudIngameDispatch | HUD/HudDispatch.cpp | 13 raw forwarders | none |
| 0x0040e340 | GetLiveCarCount | Util/UtilLeaves.cpp | - | Thresholds_ah4.cpp (asi entries) |
| 0x0040e370 | IsCarSlotActive | Util/UtilLeaves.cpp | - | TrackRenderer, CarRegistrant, HeadingAssist, PathProgStep |
| 0x0040e3a0 | PlayerColorTableGet | Frontend/MenuScoreSort.cpp | 0x004a332b raw forwarder (noreturn) | none |
| 0x00411d60 | ReplayCheckTimer | Vehicle/SmallLeaves_q4.cpp | FUN_0042f6a0 = GetRaceSubMode | none |

asi only, NOT a C4 candidate (no exe body: the old body was under `#ifndef MASHED_STANDALONE`): 0x0040cf40
(`ReleaseResourcePair_impl`, wrapper in `asi_sources.rsp` only; the `ReleaseResourcePair` ShadowAB entry and its
install stay in Thresholds_ah4.cpp as a thin caller; callee 0x004c5a60 through a raw forwarder, as before).

Witness note for re-classify (rubric (a)-(c) of "How the Write-set leg relates"): 17 of the 19 have no caller in
the exe call graph, so the unchanged gates say nothing about them beyond "the build is unchanged in behaviour".
0x0040e370 and 0x0040e340 are called by exe code the gates run.

## Not swapped (matched, exe body exists)

| rva | reason (helper SA/SB/SC finding, each checked against the old body) |
|---|---|
| 0x004099e0 | exe body writes a file-static `g_saveStatus` (GameSave.cpp:97-99), not 0x008a95a0; exe self-test reads it (:375). asi copy = matched source |
| 0x004099a0 | exe body reads `GameFlow_SaveCounterPtr()` (C++), writes `s_autosave[]`, prints with `std::printf` |
| 0x0040e480 | exe body has a null-base guard the original lacks; without it the exe AVs (its 0x005f2770 reads 0) |
| 0x004114c0 | **old body diverges from the original**: original calls `[[0x007d3ff8]+0x10c]`, BootLowRvaCluster.cpp:247 adds one dereference. No install, no callers. Swapping would fix it: a behaviour change, so left for a decision |
| 0x00411350 | **old body diverges from the original**: it computes `(double)p*k`, the original (matched) `(float)p*k`; they differ on 1,787,798 of 1,800,000 inputs (e.g. p=3000). No exe callers; the asi installs the old body |
| 0x0040b890, 0x0040b8e0 | exe body is a 3-arg core fed port state (exe_main.cpp:4324/4326, RwRaceFx.cpp:1473/1476) |
| 0x004103a0, 0x0040dbd0 | exe arms skip callees, use port host objects, carry knobs (SubStateMachine.cpp) |
| 0x0040d590 | one body over a `Rank3Callees` struct fed TrackRenderer members |
| 0x00411ce0 | callees 0x0041a960 / 0x00483a70 have no port implementation (old body raw-calls them): swappable with raw forwarders like 0x0040dfc0; not done this run |
| 0x00410510 | C++ core over a virtual `EvalResultInputs` interface fed the port snapshot (RuleEngine.cpp) |
| 0x0040e180 | `RaceCamera::MostSeparatedPair` over a port-owned `RaceCamCar[4]`, exact sqrt instead of 0x004c3ac0 |
| 0x0040b180 | `TrackRenderer::SeedMatchScores`: member over port state (`scores_`, `team_play_`, `rule_`), logs |
| 0x0040b540 | no exe body (exe_main.cpp:4240-4268 is an inline sort, not this function) |
| 0x0040e350, 0x0040bb50, 0x0040b930, 0x0040e470 | hooks.csv `exe_file` names an exe TU (ForceIntegratorStubs / exe_main / MenuStateMachine) but `rva_body_scan` finds no exe body (asi body only): not investigated this run |

Exe-bodied but not matched: 0x0040b290 (86.6%), 0x00410d10 (94.9%), 0x0040eee0 (not attempted, 3,337 B); all
three exe bodies are TrackRenderer / RuleEngine members over port state, so a match would not swap as is.

The two divergences (0x004114c0, 0x00411350) are port bugs against the original found by this slice. Neither is
reached by the default exe (no exe caller), so no gate can see them.

## Matching results (all 129)

`results.tsv` (rva, size, subsystem, confidence, exe body TU, raw g2s %, best %, regs %, by insn|bytes, source,
source sha256/16). Summary: 110 at 100% (6 of them by the whole-symbol byte rule: Ghidra's size is short),
4 below (0x0040ad90 97.9 b, 0x00410d10 94.9 b, 0x0040b290 86.6 b, 0x0040cea0 82.8 b), 15 not attempted (all over
300 B, only 2 with an exe body: 0x0040eee0 3,337 B and 0x00410d10 above).
Per-function notes from the 7 matching helpers: `notes/h1..h7.md`.

Caveats for re-classify (pilot rule for stand-in callers still undecided):
- **stand-in callers / callees** (the target's bytes are identical, the file is not the real TU): 0x004103a0
  (stand-in caller), 0x004102f0 (stand-in caller for 0x004111c0), 0x0040c5b0 (stand-in static callee 0x0040be50).
- 0x0040ac80 / 0x0040ab40 also hold 8 functions Ghidra has no entry for (`// HIDDEN:` markers, byte-identical by
  match.py's own byte compare at their size): 0x0040abc0, 0x0040abe0, 0x0040ac00, 0x0040ac40, 0x0040a9a0,
  0x0040aa30, 0x0040aa60, 0x0040aad0. They have no hooks.csv rows.

## Commands (reproduce)

```
cd C:\Users\maria\Desktop\Proyectos\Mashed && py -3.12 re/tools/decomp_pc.py --file <slice_rvas.txt> --port --callees --json -o decomp.json
py -3.12 re/tools/match/g2s.py --decomp decomp.json --out g2s
py -3.12 verify/matchs02_20261031/tools/to_s02.py <rva8> ...      # raw 100% outputs -> s02 form
py -3.12 re/tools/match/match.py re/match/s02/<rva8>.c
py -3.12 re/tools/match/relocheck.py re/match/s02/*.c
py -3.12 re/tools/match/match_permute.py re/match/s02/<rva8>.c 0x<rva> --rounds 6 --walk 40 --jobs 2 --time 300
py -3.12 verify/matchs02_20261031/tools/excise.py <old TU> <rva8>  # remove old body
py -3.12 verify/matchs02_20261031/tools/mkwrap.py <rva8> <Symbol> <install> [--bind ...] --old <TU>
cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"
A4_EXE=<exe copy> A4_ROOT=D:\MashedWT\MATCH-S02_root A4_OUT=<arms dir> mashed_lock.py run --label MATCH-S02 -- py -3.12 verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26
```
Helper briefs: `BRIEF.md` (matching), `SWAP_BRIEF.md` (body swap). Sources are LF in git (autocrlf); the compiler
output does not depend on line endings.

## Gates

Runner `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, A4_ROOT `D:/MashedWT/MATCH-S02_root`
(robocopy of `original/`), wrapped in `mashed_lock.py run --label MATCH-S02`, exe copied out of the build dir first.

| run | build | exe sha256/16 | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs ref |
|---|---|---|---|---|---|
| 1 | 3 swaps (0x0040b6b0, 0x0040b7a0, 0x0040b7b0) | `8fc89bfb5277044f` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |
| 2 | all 19 swaps + 0x0040cf40 asi, commit c9134b0f7 | `f4cbd84185c06525` | 8284be46 / ba407a16 / 31fe043e | 1d5bf4f9 x3 | equal |

No gate hash moved, so no DECISION is needed for any swap.

## Raw data (D:/MashedWT/MATCH-S02_data, not committed)

```
9eb70ad87b72ffc2216b0fe78b6dfb06697153e02b115a66d6a39a2cab6a4ae1  decomp.json (decomp_pc.py --port --callees, 129 functions)
8fc89bfb5277044f886c87c94d1fc9efda5f0c37b30a816d5e0f2a47a9f12f6a  exe_g1/mashed_re.exe
f4cbd84185c06525ded669d42ef1df7f3a8b6381119d797105ead085c4e7ef06  exe_g2/mashed_re.exe
8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198  arms_g1/H_DEF.step.csv  (= arms_g2)
ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb  arms_g1/H_DEF.gates.csv (= arms_g2)
31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3  arms_g1/H_DEF.ptrace.log (= arms_g2)
1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457  arms_g{1,2}/B_DEF_K{0,1,26}.step.csv
7dbfca248f656b06e110757895ece5833aa8c2c8a58d3190d0f9abce6777feed  arms_g2/H_DEF.log
```
Also there: `g2s/` (raw converter output for all 129), `build1.log`, `build2.log`, `relocheck_{1,2,3}.tsv`.

## HANDOFF / next

- Re-classify the 19 swapped rows to C4 (`match` route) once MATCH-RUBRIC is merged and this branch has passed
  the train; record relocheck OK and the exe-caller witness note per row.
- 0x00411ce0: swap with raw forwarders (same pattern as 0x0040dfc0).
- Decide 0x004114c0 and 0x00411350 (port bodies that diverge from the original; swapping the matched source in
  fixes both).
- 15 unattempted + 4 residual functions (no C4 value until they get an exe body, except 0x0040eee0 / 0x00410d10).
