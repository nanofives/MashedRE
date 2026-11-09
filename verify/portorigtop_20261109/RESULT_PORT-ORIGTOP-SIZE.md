# RESULT PORT-ORIGTOP-SIZE (D4): how much C4-by-match code the MASHED_ORIG_TOP route can run

Branch `orch/PORT-ORIGTOP-SIZE` from orch/integration `488fa1d9f`. Read-only: no mashedmod change. Static analysis,
plus the original's own runtime census for the race point. Tool: `verify/portorigtop_20261109/origtop_size.py`
(`--runtime --race --order`). Outputs: `SIZE.tsv`, `race_point.tsv`, `race_path.tsv`, `order_full.txt` (same folder).

## The table

Reach = the original's call graph from 0x00492370 (its WinMain, matched, entered under MASHED_ORIG_TOP). It is not the
exe's default path. **2,338 functions**, of which **1,629 are C4 by match**.

| step (bound cheapest first) | functions to bind in reach | C4-by-match rows runnable | added by this step | added if bound alone |
|---|---|---|---|---|
| today's bindings only (BINDINGS.tsv bound / port / rw / shim / crt; stand-ins replaced by their matched bodies) | - | **996** (121 already run) | - | - |
| + rt2d fonts | 5 | 998 | +2 | +2 |
| + Lua API | 7 | 1,016 | +18 | +18 |
| + D3D / DirectShow glue | 24 | 1,017 | +1 | +1 |
| + RW-Physics 3.7 rows not yet matched (matching work, not a binding) | 30 | 1,049 | +32 | +32 |
| + CRT | 33 | 1,135 | +86 | +53 |
| + remaining RW core (0x004e7e30, 0x004c15c0 and the rest) | 81 | **1,207** | +72 | +37 |
| + C0..C3 game code (matching work: 357 functions) | 357 | 1,541 | +334 | +209 |

- "Cheapest" = fewest functions of that family in reach that need a binding.
- Other classes in reach, each under 20 rows released:
  - 44 functions with no hooks.csv row: +16 alone
  - 15 C4 game rows with neither a matched body nor an exe body: +2
  - 4 C4 game rows with an exe hand body only: +0
  - 1 qhull row: +0
- The only class that blocks 20 or more rows beyond the named families is **C0..C3 game code**. It needs matching
  first, not a binding.

### Dual copies (rows runnable once every binding family is bound: 1,207)

**92** of the 1,207 also have a hand body in the exe for the same address, with the exe not compiling the matched
source. Under the route they are dual copies until the flip and need an ORIG_TOP-only allowlist class.

| subsystem | rows | rvas |
|---|---|---|
| audio | 21 | 0x005a89a0 0x005a89b0 0x005a89c0 0x005a9e40 0x005abf80 0x005ac5f0 0x005ac740 0x005ac980 0x005ac9e0 0x005aca80 0x005acd60 0x005ad2e0 0x005ade60 0x005aded0 0x005adf30 0x005ae050 0x005ae0b0 0x005aea10 0x005aec00 0x005baf40 0x005baf60 |
| gameplay | 11 (7 after the GAMEPLAY merge) | 0x004075b0 0x00407b00* 0x00407b70* 0x00407d70* 0x00407db0* 0x00409330 0x0040b890 0x0040b8e0 0x0040b930 0x0041f360 0x0045c110 |
| render | 11 | 0x00476a40 0x00476c60 0x00476df0 0x00498a00 0x00498e40 0x004b4050 0x004b4650 0x004c3bf0 0x004c4600 0x004cbb60 0x004d40c0 |
| frontend | 10 | 0x00408a70 0x0040e470 0x00426cb0 0x004298c0 0x0042fab0 0x00430760 0x00430910 0x00431b70 0x004323c0 0x00432b30 |
| vehicle | 10 | 0x0040e180 0x00468b40 0x0046b1c0 0x0046b540 0x0046c7b0 0x0046cbb0 0x0046dbe0 0x00476c10 0x004770c0 0x00485340 |
| ai | 10 | 0x004148b0 0x00417cf0 0x00422b50 0x004233e0 0x00443080 0x0046c730 0x0046c750 0x0046d4a0 0x0046d780 0x0046d7f0 |
| util | 8 | 0x0040b180 0x0040dbd0 0x00422120 0x00441c80 0x0046baa0 0x004b64e0 0x004c3b30 0x0055dec0 |
| save | 3 | 0x004099a0 0x004b3b70 0x00550980 |
| hud | 3 | 0x0041a3e0 0x0041b630 0x00556cc0 |
| powerups | 2 | 0x0045baa0 0x0045bac0 |
| boot | 1 | 0x00404830 |
| world-objects | 1 | 0x00484a50 |
| RenderWare-Physics-3.7 | 1 | 0x0055bd80 |

\* swapped on `orch/PORT-BATCH-GAMEPLAY` (enqueued). Once it merges, they compile their matched source and are no
longer dual. This branch reads orch/integration's hooks.csv, which does not have them yet.

### Caveats on the 1,207 (counted, not resolved)

- **72 read RwGlobals** (`DAT_007d3ff8`). Under the default path it is 0 in the exe pad.
  [UNCERTAIN] What the ORIG_TOP path holds there after the matched RW bring-up 0x00493710 and its librw bindings is not
  measured here. The rows are listed in `SIZE.tsv`. This is the "RwGlobals producer" row the lead filed.
- **123 read nonzero initialised original data** (.data / .rdata bytes that are nonzero in MASHED.exe).
  [UNCERTAIN] They need the seed to cover them on this path. Which seed the ORIG_TOP path loads (L-99 PortStateSeed
  and the PORT-TOP .rdata seed) is not checked per row here.
- Static reach is an over-approximation. It has two edge kinds:
  - direct `E8` calls, from `re/console/pc_features.csv`
  - address-taken edges: capstone over every function finds imm32 / disp32 operands that are function entries, or that
    start a `.rdata` / `.data` table of function entries (71 tables)
  - MASHED.exe has no base relocations, so relocations cannot supply these edges.

  Direct calls alone reach 2,184 functions. The address-taken edges add 154, including the state tables that link
  0x00492d30 to the race code.
- "Runnable" means every callee in reach is matched, bound or in a bound family (greatest fixpoint). It says nothing
  about gates. Each swap is still gated. The 996 count assumes each stand-in on the path is replaced by its matched
  body when that body's subtree is runnable. That is the route's premise.

## The race point

### Where ORIG_TOP is today

It boots and walks the 12 front-end screens; nothing reaches a race:
- the 3000-frame knob-ON run ends in matched teardown: RESULT_PORT-TOP-04I.md:59-60
- the 12 screens: RESULT_PORT-TOP-04B.md:132-143

The run loop 0x00492290 is matched. Its tick pieces are not, per BINDINGS.tsv rows 20-23:
- 0x004929d0 is the port wrapper `GameModeMachine_Tick` (no-op unless MASHED_PORTSTATE_GAMEFLOW_FU=1)
- 0x00492d30 is a hand stand-in (menu nav + GameFlow_Update)
- 0x00492e90 is the hand RenderFrame

### What a race executes

The original's runtime census (`re/coverage/reach.tsv`, INT3 on every entry, 3 runs) uses the scenario warp: state 2,
track 0, mode 10, 1 player + 3 AI. So a race can be entered without porting the menus.

Functions it runs in race_load / race_run that the route reaches (from `race_point.tsv`):

| class | race_load | race_run | total |
|---|---|---|---|
| matched, runnable now | 295 | 134 | 429 |
| matched, blocked by a callee below | 134 | 51 | 185 |
| C0..C3 game code (needs matching) | 106 | 64 | **170** |
| RW-Physics 3.7, not matched | 24 | 1 | 25 |
| RW core, unbound | 13 | 0 | 13 |
| CRT, unbound | 5 | 1 | 6 |
| C4 game, no body | 4 | 0 | 4 |
| C4 game, hand body only | 1 | 0 | 1 |
| Lua / qhull | 1 / 1 | 0 | 2 |

### Missing pieces in call order

1. **0x004929d0 game-mode machine.**
   - Status: C4, a port wrapper on this path.
   - Needed: its own body, or the GAMEFLOW_FU knob on.
   - Why: per its hooks.csv note it is the session-phase dispatcher on `DAT_00771968` (7 cases) and calls the
     course load in case 2.
   - Its static subtree: 688 functions, 456 matched, 277 of them runnable with every family bound.
2. **0x00492d30 GameTickStateMachine7.**
   - Status: matched (`Match/MatchS15_00492d30.cpp`), but a stand-in on this path.
   - Its 7-case dispatch on `DAT_00771968` calls 0x0043dfd0 in menu states and 0x004111c0 in race states.
3. **0x004111c0, the race-phase state machine.**
   - C2, 13,730 bytes, 11 states, 58 callees. It runs only in race_load / race_run.
   - It is the largest single unmatched piece on the race path.
   - Its static subtree: 1,018 functions, 667 matched, 499 runnable with every family bound.
4. **Race-load chain under it.** The C0..C3 functions the original runs, in call order. First:
   - 0x0047a320 SetCopter
   - 0x004726f0
   - 0x004b42c0
   - RwMatrixCombine 0x004c52f0, RwMatrixRotate 0x004c4d20, RwMatrixRotateInner 0x004c4a50
   - 0x00401da0, 0x004744a0, 0x0045c550, 0x0045efe0, 0x00463640, 0x00464a50, 0x00487280
   - 0x00441d40, 0x00442440, 0x00445aa0 camera step
   - and on, 106 in all.

   The largest:
   - 0x00446520 (7,411 B, race-result camera)
   - 0x0046f6c0 VehicleWheelContactSolver (3,580 B)
   - 0x00443dc0 spline lookahead (3,483 B)
   - 0x00412f30, 0x0047def0, 0x004883b0, 0x00487280
   - 0x00445aa0
   - 0x00490500
   - 0x0040e590, the race-start setup coordinator
5. **Race-run tick.** 64 C0..C3 functions. First in call order:
   - 0x004568d0
   - the audio stream tick 0x005acda0 / 0x005ace70 / 0x005a86a0 / 0x005a8750 / 0x005b3760 / 0x005b3b80
   - 0x0045cd30, 0x00442e00, 0x00489500, 0x0040be50, 0x0040c690, 0x00412620

   The largest:
   - 0x00469df0 VehicleVehicleCollisionImpulse (5,062 B)
   - 0x0040eee0 (3,356 B)
   - 0x00480d60, 0x0046ef70, 0x0040be50, 0x00415220
   - 0x0041c410 HudStandingsRowUpdate
   - 0x00410d10, 0x004128f0, 0x004752f0
6. **Physics library.**
   - 25 RW-Physics 3.7 functions that run in the race are not matched yet. Examples: 0x0057c210 is C4 but not
     matched; 0x00559ee0, 0x00564310, 0x0055b650 are C3.
   - Plus 13 unbound RW core, among them 0x00538c80, 0x00535700 and 0x004c1210 (list in `race_point.tsv`).
7. **0x00492e90 frame render.**
   - Matched, a stand-in on this path.
   - Static subtree: 431 functions, 65 of them C0..C3 (the HUD / panel draws 0x004368e0, 0x00434720, 0x00439210,
     0x0043af10).
   - A race replay compares state, not pixels, so this can stay a stand-in for the first race comparison.

So the first point where the route can be compared by race replay needs:
- the three run-loop pieces (1, 2, 7) off their stand-ins
- 0x004111c0 matched
- about 170 C0..C3 race functions matched: about 106 for race load and 64 per tick
- 25 RW-Physics rows matched
- 13 RW core + 6 CRT + 1 Lua bindings

Everything else the race runs is already matched: 614 rows, 429 of them runnable as soon as the stand-ins come off.

## Evidence

| file | what |
|---|---|
| `verify/portorigtop_20261109/origtop_size.py` | the tool (inputs, classes and fixpoint in its docstring) |
| `verify/portorigtop_20261109/SIZE.tsv` | reach classes, per-family alone / cumulative counts, caveats, dual copies per subsystem |
| `verify/portorigtop_20261109/race_point.tsv` | per phase (menu / race_load / race_run) and class: the functions the original runs, in call order, with name, confidence and size |
| `verify/portorigtop_20261109/race_path.tsv` | run-loop and race-path nodes: subtree, matched, runnable, blockers |
| `verify/portorigtop_20261109/order_full.txt` | DFS call order from 0x00492370 with each node's status |
| inputs | hooks.csv and BINDINGS.tsv at `488fa1d9f`; `re/coverage/reach.tsv` (2026-10-11 census); `original/MASHED.exe.unpatched` (anchor SHA-256 BDCAE093...) |
