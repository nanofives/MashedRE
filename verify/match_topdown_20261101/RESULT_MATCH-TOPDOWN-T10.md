# RESULT MATCH-TOPDOWN-T10 (2026-11-01)

Verdict: **GREEN, TEXT-ONLY.** 0 of the 8 unlock RVAs can be swapped soundly into `mashed_re.exe` (6 new bodies,
2 already swapped with 0 exe callers); each has a one-line reason below. No build input changed. T10 shares the
branch `orch/MATCH-TOPDOWN-T05` with T05 (same game-mode machine 0x004929d0, same Course::Finish 0x0040d270);
T05's table is `verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-T05.md`.

L-72 (Mariano 2026-11-01): swap matched callers in, top-down. T10 = the matched direct children of the game-mode
machine 0x004929d0 (`RESULT_MATCH-TOPDOWN-MAP.md` unit 10): 0x0040b430, 0x0040b6e0, 0x0040b700, 0x0040d440,
0x0040e340, 0x0040e450, 0x0040e460, 0x00432080. 0 blocking gaps on the map, deep gap Course::Finish 0x0040d270.

## Why nothing swaps: the exe's game-mode machine is port code over port state

0x004929d0 is a 7-case machine on `DAT_00771968` (`re/analysis/game_state/0x004929d0.md`): case 2 loads the
course (0x0040d440), case 3 runs the round end (0x00432080 end check, 0x0040e450 / 0x0040e460 round flag,
0x0040b6e0 / 0x0040b430 / 0x0040b700 score counters). The port counterpart is **not** `Boot/GameStateCluster.cpp`
(that TU holds six leaves only: 0x00426c00, 0x0042b8e0, 0x0042b910, 0x0042b940, 0x0042c1c0, 0x0042c1d0). The exe
runs the game flow through `Race::GameFlow_*` / `RaceSession` (`Race/GameFlow.h`; pause 3 <-> 7 at
`exe_main.cpp:2229-2262`, match-winner -> Results via `GameFlow_RequestResults`), and keeps the round state in
port objects. The globals the matched children read or write are dead in the exe:

| global | used by | exe status |
|---|---|---|
| `0x008a94f0[4]` | 0x0040b430 (+=) | no exe writer; exe reader `FrontendArrayGet` (0x0040b6c0, `Match/MatchS02_0040b6c0.cpp`) has 0 exe callers; `Frontend/TimerReset.cpp` zeroes it |
| `0x0063b8ec` | 0x0040b6e0 (+=) | no exe writer; exe reader `HotkeyStringBaseGet` (0x0040b7a0) has 0 exe callers |
| `0x008a9530/40/50/60[4]` | 0x0040b700 (++) | no exe writer; exe readers `ModeScoreGetBySlot` (0x0040b6b0), `PlayerHotkeyTableGet` (0x0040b7b0), `SlotSortByModeScore` (0x0040b620) have 0 exe callers |
| `0x008a94d0` | 0x0040e340 (return), 0x0040b700 via 0x0040e340 | no exe writer: the exe uses the port stand-in `g_participantCount = 4` (`Vehicle/ForceIntegratorStubs.cpp:162-171`), so 0x0040e340 returns 0 in the exe |
| `0x0067ea64` / `0x0067e9fc` | 0x0040b700 via 0x0042f500, 0x0040b6e0 via 0x0042f6a0 | 0 in the exe (`Race/SubStateMachine.cpp:25-34`) |
| `0x0063b908` | 0x0040e450 (get), 0x0040e460 (set) | referenced only by asi-only TUs (`Util/PromoLoop_round26.cpp`, `Util/PromoLoop_sessionB.cpp`) |
| `0x005f2770`, `0x0063ba7c` -> 0x0040d270 | 0x0040d440 | course-handler table not filled in the exe (T05 RESULT); 0x0040d270 unmatched, no exe body |
| `0x007f1a14`, `0x007f1041`, `0x007f1501`, `0x0067eca4`, ... | 0x00432080 | partly live (frontend TUs write some), but the return value decides the round end (`DAT_00771968 = 5`), which the exe decides in `GameFlow` |

A new exe call site for any of these would either run on dead data (the T13 0x0041b520 precedent: "the body runs,
the inputs are not the original's", not proposed for C4) or move round-end / scoring behaviour (a behaviour
change: default-OFF knob + PREREG + decision, not a swap).

## Per-RVA table

`match.py` = `re/tools/match/match.py <src> --toolchain ddk --bytes` (cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`),
re-run on this branch (log `D:/MashedWT/MATCH-TOPDOWN-T05_data/match_rerun.log`). `exe callers` = E8/E9 rel32
sites to the symbol in this branch's baseline `mashed_re.exe` (orch/integration `3a3186c4a`), scanned against its
`/MAP`; the one absolute reference each swapped body has is its `s_AutoReg_*` registry entry, not a call.

| rva | outcome | matched source | match.py | relocheck | exe body | exe callers | reason |
|---|---|---|---|---|---|---|---|
| 0x0040b430 | left (already swapped, S02) | `re/match/s02/0040b430.c` | 100.0% 12/12, 47 B identical | OK checked=3 unchecked=0 | `UtilFieldAdd8a94f0`, `Match/MatchS02_0040b430.cpp` (both) | 0 | only original caller is case 3 of 0x004929d0 (map call graph); the exe round end is `GameFlow` and `0x008a94f0` has no exe reader that runs. Stays C3 (L-68, exe-callers=0) |
| 0x0040b6e0 | left | `re/match/s02/0040b6e0.c` | 100.0% 8/8, 29 B identical | OK checked=3 unchecked=0 | none (asi `Util/PromoLoop_sessionB.cpp` Add40b6e0) | 0 | `0x0063b8ec` dead in the exe (table above); gate 0x0042f6a0 reads `0x0067e9fc` = 0 in the exe |
| 0x0040b700 | left | `re/match/s02/0040b700.c` | 100.0% 40/40, 148 B identical | OK checked=7 unchecked=0 | none | 0 | mode from 0x0040e340 / 0x0042f500, both 0 in the exe, so every call would take no case; counters `0x008a9530..60` dead in the exe |
| 0x0040d440 | left | `re/match/s02/0040d440.c` | 100.0% 9/9, 34 B identical | OK checked=5 unchecked=0 | none (asi `Track/CourseLoadDispatch.cpp` Course_LoadCurrent_40d440) | 0 | calls unmatched 0x0040d270 (no exe body) with the zero table `0x005f2770`; the exe course load is port code (T05) |
| 0x0040e340 | left (already swapped, S02) | `re/match/s02/0040e340.c` | 100.0% 2/2, 6 B identical | OK checked=1 unchecked=0 | `GetLiveCarCount`, `Match/MatchS02_0040e340.cpp` (both) | 0 | `0x008a94d0` has no exe writer; the exe's participant count is `g_participantCount = 4` (`Fi_ParticipantCount`), so rebinding those readers to this body would turn 4 into 0. Keeps its C4 from the earlier route; hooks.csv already carries the L-68 note (match route waits for a caller) |
| 0x0040e450 | left | `re/match/s02/0040e450.c` | 100.0% 2/2, 6 B identical | OK checked=1 unchecked=0 | none (asi `Util/PromoLoop_round26.cpp` Flag63b908Get) | 0 | `0x0063b908` has no exe reference. Original callers: 0x004929d0 (case 3) and 0x0045c7f0 (asi-only body `Sub45c7f0`, no exe body) |
| 0x0040e460 | left | `re/match/s02/0040e460.c` | 100.0% 3/3, 10 B identical | OK checked=1 unchecked=0 | none (asi `Util/PromoLoop_round26.cpp` Flag63b908Set) | 0 | same global as 0x0040e450. Original callers: 0x004929d0 (case 3, clears it) and 0x00410860, whose exe port `TrackRenderer::RoundEnd7ExitTick` (RVA-less, `D3d9Render/TrackRenderer.cpp:8001`, MASHED_R7_EXIT) returns its gate to the port flow and never writes `0x0063b908`; a call there would write a flag no exe code reads |
| 0x00432080 | left | `re/match/pilot/a1_00432080.c` | 100.0% 110/110, 417 B identical | OK checked=2 unchecked=42 | none (asi `Vehicle/ReplayCluster.cpp` RaceEndCheckFinish) | 0 | its return value ends the round (`DAT_00771968 = 5`) in the original; the exe ends the round in `GameFlow` from port state, so a call is a behaviour change. Callee 0x0040e470 would bind to `CarSlotStateGet`; 0x0042d3a0 has no exe body |

The map lists 0x0040e340 under T10 through 0x0040b700; it has the many other original callers of a getter, and
none of their exe counterparts reads `0x008a94d0` through it (`Vehicle/Integrate2.cpp:161` calls the raw address
0x0040e340 in the asi arm only; the exe arm uses `Fi_ParticipantCount`).

Original callers above are from the map's call graph (`D:/MashedWT/MATCH-TOPDOWN-MAP_data/cg.json`, `orig_edges`):
0x0040b430 / 0x0040b6e0 / 0x0040b700 / 0x0040d440 / 0x00432080 have 0x004929d0 as their only caller.

## What T10 needs before any swap

An exe game-mode machine that keeps the original's round state at the original addresses (`DAT_00771968`,
`0x008a94d0`, `0x0063b908`, the score counters) instead of `RaceSession`, or the matched children rebound to the
port's state. Either is an architecture-level change to `Race/GameFlow` (CLAUDE.md "Stop and ask"), not an L-72
swap. 0x004929d0 itself is unmatched (S15 per the map's gap list).

## Gates

Not run: no build input changed. The baseline build (T05 RESULT "Gates") was used only for the `/MAP` counts.

## Re-classify

None. 0x0040b430 / 0x0040e340 keep their level and their existing L-68 note (`exe-callers=0`).

## Raw data (D:/MashedWT/MATCH-TOPDOWN-T05_data, not committed)

Same files as the T05 RESULT:

```
81e39d800314d4ad10c36b0c9421ac850093169fbf1fc3550cda7a59ebcd8609  mashed_re.exe (baseline build, orch/integration 3a3186c4a)
a7780b0705f0b0b17c44254522bdb2c5ad1ce939ddc61f538941e0c1e75bd1e7  mashed_re.map
2ac3848f7d1f2e88a72fa8df1e193550654a2588dc7a7e598e3c200e2b65cfd8  match_rerun.log
```
