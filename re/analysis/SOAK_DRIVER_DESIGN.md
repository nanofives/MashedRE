# SOAK-DRIVER design: an unattended race driver and the T3 soak matrix runner

Row `SOAK-DRIVER` (ROADMAP v5 T3, `ROADMAP.md` "T3, breadth", P-DoD (port) item 3). Kind: design, text only, no build,
no gates, no game launch. Date 2026-11-13. Branch `orch/SOAK-DRIVER` from `orch/integration` `ba39eeb6b`.

All paths are repo-relative to `orch/integration` unless marked otherwise. NO-GUESSING: every claim about the original
cites an RVA or a file; what nobody has measured is marked `[UNCERTAIN]` with the missing evidence. Estimates are
labelled as estimates and show their arithmetic.

## 0. Verdict

- **Recommendation: candidate A (the game's own AI on the player slot)** as the soak driver, entered through the
  existing replay harness with one new scenario field, `slots`. Candidate B (recorded human input, `RO-INPUT-RECORD`)
  stays as the oracle-reference tool for flows that need a human-typed slot, not as the soak driver.
- **No user decision is needed for A against B.** A meets "unattended" by construction, B does not (it needs a person
  for every cell, section 3.5). The one point a lead may want confirmed is an interpretation of P-DoD item 3, stated in
  section 6 as an assumption.
- **First step is a probe, not the runner** (`SOAK-P0`, section 5.1): three facts A rests on have never been measured.

## 1. What the soak matrix must show

`ROADMAP.md` P-DoD (port) item 3: "The soak matrix completes: every track in every mode, unattended, no crash, results
screen reached." T3 exit is P-DoD items 1 to 3 (`ROADMAP.md`, "T3, breadth", "**Exit:**"). The same driver is named as
what unblocks `D4-SAVE-U3560` ("a natural TimeTrial lap", `ROADMAP.md` T3 bullet "Soak matrix").

It runs on two binaries:
- the original `original\MASHED.exe` (patched per `re/analysis/BOOT_PATCHES.md`, never edited by this work), to prove
  the driver finishes every cell on the reference and to give the per-cell reference outcome;
- `mashed_re.exe` on the top-down path (`MASHED_ORIG_TOP`), which at T3 runs the original's own top level and race code
  (`ROADMAP.md` T2 / T3), so the driver must work through the same original code paths on both.

## 2. Existing harness pieces (inventory, cited)

| piece | what it gives | file |
|---|---|---|
| Menu-less race warp, original, Frida | writes selection globals `0x0063ba7c` / `0x0067f17c` track, `0x0067e9fc` mode, `0x007f0fd0` rule, `0x0067ea98` car, `0x0067ea64` team; calls `FUN_0040e480(k, ...)` for k = 0..3; RNG pins; timer pin; `DAT_00771968 := 2` | `re/frida/js/detorig.js` `warp()` (the `e480(k, k === 0 ? 1 : ...)` loop) |
| Its driver | `--clock tick`, `--boot`, `--warp-tick`, `--tick-cap` (default 3000), `--rec`, `--input-json`, `--asi`, `--crash-catch`, `--timeout` (default 600 s) | `re/frida/detorig.py` (argparse block); scenario is the hard-coded `SCENARIO` dict at `re/frida/detorig.py:26` (track 0, mode 10, rule 0, car 0, team 0, cars 4) |
| Same replay with no Frida in the process (original side) | `MASHED_REPLAY_ASI=<cfg.txt>` in `mashed_re_dev.asi`: virtual clock, tick listener at `0x00493390`, warp, pins, player-0 script at `0x00497310`; config line `scen track mode rule car team cars` | `mashedmod/src/mashed_re/Core/ReplayAsi.cpp` (header comment; slot loop `e480(k, k == 0 ? 1 : ...)` at line 254) |
| Exe-side replay knob | `MASHED_REPLAY=<replay.json>`, ORIG_TOP path only: same clock, tick, warp at `warp.after_ticks`, pins, race anchor, player-0 input | `mashedmod/src/mashed_re/Match/PortTop_Replay.cpp` (header items 1 to 6; slot loop at line 276; scenario parse around lines 159 and 370) |
| Replay format | `replay.json` `mashed-replay-1`: `scenario`, `warp`, `clock`, `rng`, `input` rows `[tick after anchor, player, control, value]`, `trace` | `verify/replayoracle_20261111/DESIGN_REPLAY-ORACLE.md` section 2 |
| Determinism of the original under the tick clock | 6 of 6 run pairs identical on 2,724 of 2,724 race ticks | `ROADMAP.md` findings table; `verify/replayoracle_20261111/RESULT_REPLAY-ORACLE-DESIGN.md`, `RESULT_RO-WARP-NORMALIZE.md` |
| Input recorder (candidate B) | `--rec` logs player-0 `(tick, control, value)` from the `0x00497310` resolver; `inputrec2replay.py convert / compare`; round trip 321 / 321 ticks identical, scripted input only | `verify/replayoracle_20261113/RESULT_RO-INPUT-RECORD.md`; `re/tools/inputrec2replay.py` |
| Older keyboard recorder | DirectInput keyboard buffer by render frame | `re/frida/record_attach.py`, `re/frida/replay_session.py` |
| Menu navigation to a race (original) | statenav Quick Battle route; the cup route with `--root <copy>` and `--reset-save` | `re/frida/statenav.py`; `re/frida/c4_hud_scenario.py` (header, `--route attract|qb|cup`) |
| Results screen detector | frontend depth `0x0067e9f8`, frontend phase `0x0067eca4`, screen type `*(0x0067ed3c + depth*0x40)`, race phase `0x00771968`, in-race UI `0x0063ba8c` | `re/frida/c4_detect_e_scenario.py:47`, `:128` |
| Match-end condition | `0x00410510` (Race::EvaluateResult): match won at score 8 (2 to 3 entrants or teams) or above 11 (4 entrants); on conclusion `DAT_0063ba8c = 0xb`, `DAT_0063b914` = winner, `DAT_0063b90c = 1` | `re/analysis/race_camera/points_system.md` section "0x00410510" |
| Crash catching | first-fault exception handler (`--crash-catch`); EIP catcher; minidump parser | `re/frida/detorig.py` (`--crash-catch`); `re/frida/poll_attach_catch_crash.py`; `scripts/parse_minidump.py` |
| Exe stand-in log | each stand-in logs `[ORIG_TOP] rva <rva>: stand-in ...` to `mashed_re.log` | `mashedmod/src/mashed_re/Match/PortTop_StandIns05.cpp:9-13` |
| Exe binding table | which RVAs are matched, provisional or stand-in on the top-down path | `verify/porttop_20261108/BINDINGS.tsv` |
| Knob boot of the exe | top-down boots, nav-demo captures | `verify/porttop_20261108/knob_boot.py` |
| Run discipline | `mashed_lock.py run` around every launch, test-slot outside, private root, own PIDs only | `re/orchestrator/WORKER_RULES.md` "Builds and game runs"; `re/orchestrator/mashed_lock.py` |
| Hand-path AI-on-player knob (not usable) | `MASHED_AI_DRIVES_PLAYER`: the hand renderer's own pure-pursuit steering, not the original's AI (the in-file audit note says the AI controller is never called) | `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:5758-5781`. Hand path, frozen until T4 (`ROADMAP.md` "Frozen until T4"), and it runs no original code, so it is not T3 evidence |

## 3. The two candidates

### 3.1 Candidate A: the game's own AI on the player slot

**Mechanism.** `FUN_0040e480(slot, state)` is a single store `*(PTR_PTR_005f2770 + slot*4 + 0x34) = state`
(`re/match/s02/0040e480.c`; `re/analysis/c0_promotion_frontend_a/0x0040e480.md`). Two configurations of it exist:

| caller | slot 0 | slot 1 | slot 2 | slot 3 | source |
|---|---|---|---|---|---|
| the replay warp (all three implementations) | 1 | 2 | 2 | 2 | `re/frida/js/detorig.js` `warp()`; `Core/ReplayAsi.cpp:254`; `Match/PortTop_Replay.cpp:276` |
| the original's attract launch `0x0043df00` (mode `0xb`, rule 6) | **2** | 2 | 2 | 0 | `re/match/s08/0043df00.c` (`FUN_0040e480(0,2)` ... `FUN_0040e480(3,0)`, after `DAT_0067e9fc = 0xb`, `DAT_007f0fd0 = 6`) |
| the race-launch action of `0x0043dfd0`, when `FUN_00430760` returns non-zero | 1 | 1 | 1 | 1 | `re/analysis/game_mode/0x0043dfd0.md` (calls at `0x0043e8ac..0x0043e8c8`) |

The attract race starts with no input source at all: `re/frida/c4_hud_scenario.py` header, route `attract` ("boot,
touch nothing, the game starts its own attract demo race", "installs no input hook"). So the original itself runs a
race in which slot 0 has state 2 and no input is supplied.

Candidate A = run the normal scenario with slot 0 at state 2: add a `slots` field to the scenario (default
`[1, 2, 2, 2]` = today's behaviour, so nothing changes when it is absent) and pass `slots[k]` to `FUN_0040e480` in the
three warp implementations.

`[UNCERTAIN]` (three gaps, all closed by `SOAK-P0`, section 5.1):
1. That state 2 on slot 0 makes the original's AI steer car 0. Evidence today: the attract race (above) and that states
   1 and 2 are written as different values by the warp. Missing: a run that shows car 0 moving with no input and the AI
   per-car state of slot 0 being stepped. The reader of `PTR_PTR_005f2770 + 0x34 + k*4` is not identified
   (`0x0040e480.md`, U-3431). Also open: `0x0043dfd0` writes state 1 to all four slots in one branch, so what makes a
   slot "human" may be state 1 together with the player-slot map `DAT_007f1a14..` (written by `0x0043df00` as
   `0, 1, 2, -1`), not state 1 alone.
2. That a match with no state-1 slot reaches the match end (`DAT_0063ba8c = 0xb`, `points_system.md`). Measured today
   only with a human-state slot 0: with no input the Quick Battle race sat 240 s with no round end; 1 Hz pulses of
   control 4 on player 0 reached Race Results in about 60 s, 10 of 10 runs (`re/frida/c4_detect_e_scenario.py`,
   2026-10-12, see its route comment near line 249).
3. That the Race Results screen (frontend type 5) appears after a **warp-entered** race. The warp writes the
   selection globals and the phase byte but pushes no frontend screens, so the post-race frontend stack the menus
   build is absent. Missing: one warp-entered run that reaches `DAT_0063ba8c = 0xb` and then the frontend.

**On the original.** Boot under `mashed_lock.py run`, private root (`WORKER_RULES.md`), detorig boot mode with the
tick clock, warp with `slots = [2, 2, 2, 2]` (or `[2, 2, 2, 0]` for three cars, the attract shape), no input rows. Run
until the results detector fires (section 4.3) or the tick cap. For the long runs use the no-Frida replay
(`--asi`, `MASHED_REPLAY_ASI`, `Core/ReplayAsi.cpp`) rather than the Frida agent: the Frida path keeps a JS agent in the
process for the whole match, and the CLAUDE.md "Frida overhead on hot paths" rule applies to anything that grows a
per-frame callback.

**On mashed_re.exe.** The same `replay.json` through `MASHED_REPLAY` (`Match/PortTop_Replay.cpp`) on the top-down
path. The AI that drives car 0 is then the original's AI code as bound on the top-down path (matched, admitted or
provisional bodies, `BINDINGS.tsv`), the same code as for slots 1 to 3, so the exe needs no driver code of its own.
Precondition: the top-down race path runs (T2: `0x004111c0` running, the run-loop pieces off their stand-ins,
`ROADMAP.md` T2). Today `0x004111c0` is a stand-in that counts and does not run
(`Match/PortTop_StandIns05.cpp`, "race tick: the hand session runs the race (counted, not run)"), so the exe side of the
soak cannot run before T2's exit.

**Reuse.** Everything in section 2 except the recorders. New code: the `slots` field (3 files), a scenario selector
on the detorig CLI (today `SCENARIO` is a constant), a match-end and results detector in the replay tick listener, and
the matrix runner (section 4).

**Determinism.** On the original, a race with no input under the tick clock is a function of the tick: the replay
oracle measured 6 of 6 run pairs identical over 2,724 ticks with scripted input (`ROADMAP.md` findings table). AI-only
input removes the last external input, so each cell needs one run, and a second run of one cell per batch is the
control. Caveat: that measurement covers 2,724 ticks of one track; a full match is longer
(estimate below). `[UNCERTAIN]` determinism past tick 2,724 and on the other 12 tracks; the matrix runner's
control re-run measures it. Without the tick clock (wall clock), round outcomes do not reproduce (three identical
240 s captures gave three different matches, `verify/d3_consumer_20261007/RESULT_E1.md` section 5), so the tick clock is
required, not optional. On the exe: `[UNCERTAIN]` until two exe runs of one cell are compared; the oracle hashes give
that compare for free when `trace` is on.

**Runtime per race (estimate, not measured).** Detorig arms of 2,724 hashed ticks ran 57 to 68 s wall each, boot,
menu and load included (`DESIGN_REPLAY-ORACLE.md` section 4, "Cost"). A match ends at score 8 or above 11
(`points_system.md`). Measured round lengths: a 240 s capture evaluated 3 to 5 rounds (`RESULT_E1.md` section 5), so
48 to 80 s per round at 60 fps. Rounds per match is `[UNCERTAIN]` (it depends on the per-round deltas, +2 / +1 / -1 /
-2, `points_system.md`); at the lower bound of 6 rounds that is 5 to 8 min per cell at 60 fps. The d3d9 shim's
limiter can be lifted in race phases only (`MASHED_FPS_CAP_RACE=0`, `mashedmod/src/d3d9_shim/d3d9_shim.cpp:366-383`,
race phases 3 and 6 of `DAT_00771968`), and under the tick clock game time advances per tick, not per real second, so an
uncapped race should run faster than real time. `[UNCERTAIN]` not measured under detorig: the shim comment's
"~360 FPS uncapped" (`d3d9_shim.cpp:342`) is a menu-era figure. `SOAK-P0` measures ticks per match and wall time,
capped and uncapped.

**Failure modes.**
- AI gets stuck (wall pin, flipped car) and the round never ends: the tick cap and a "no score change for K ticks"
  rule catch it; the cell is RED with the tick and car positions, not a crash.
- Slot 0 at state 2 changes which code runs (camera follow, results flow, unlock writes keyed on the human slot). That
  is a coverage gap, not a failure: flows that need a human slot go to section 3.4.
- A mode refuses an all-AI slot set (the MP family 6 to 9 goes through the `FUN_00430b60` gate,
  `game_mode_rules_REmap_20260616.md` step 2). Use `[2, 2, 2, 2]` with the mode's normal player count, else mark the
  cell N/A with the gate's evidence.
- Piz-collision crashes `MASHED.exe+0x1509b4` / `+0xe66db` (U-9677) are re-runs, not findings (CLAUDE.md).

**Effort (estimate).** Four rows: `SOAK-P0` probe (harness, original only, no build: the `slots` field in
`detorig.js` / `detorig.py` only), `SOAK-SLOTS` (the field in `ReplayAsi.cpp` and `PortTop_Replay.cpp`, default
unchanged, a top-down row kind per `WORKER_RULES.md`), `SOAK-RUNNER` (section 4, Python, no build), `SOAK-ORIG` (the
first full matrix on the original). The exe matrix (`SOAK-EXE`) waits for T2's exit.

### 3.2 Candidate B: recorded human input (`RO-INPUT-RECORD`)

**Mechanism.** A person plays each cell once on the original with the recorder on (`--rec`); the log is converted to
`replay.json` input rows (`inputrec2replay.py convert`); every later run replays those rows on slot 0 at state 1
(`verify/replayoracle_20261113/RESULT_RO-INPUT-RECORD.md`).

**On the original.** Same boot and warp as A, slot 0 at state 1, `--input-json <replay.json>`. Measured round trip:
321 / 321 ticks identical, 3,000 / 3,000 with `--ticks 3000`, with scripted input (same RESULT, "Round-trip").

**On mashed_re.exe.** The same `replay.json` through `MASHED_REPLAY`; `PortTop_Replay.cpp` header item 6 returns the
script's value for player 0 from the matched `0x00497310`.

**Reuse.** The recorder, converter and compare exist and are tested on scripted input. Missing: a recording with real
device input (`RESULT_RO-INPUT-RECORD.md`, "Gap": "[UNCERTAIN] untested with real device input"); a record mode in the
in-ASI replay (`Core/ReplayAsi.cpp` has none, same "Gap"); menu input if cells are entered through the menus.

**Determinism.** Exact on the original under the tick clock (above). On the exe, the input is open loop: it reproduces
the race only while the exe computes the original's state tick by tick. From the first divergence on (the oracle's
whole subject at T2 and T3, `RO-FIRST-DIVERGENCE`), car 0 receives inputs recorded for a different position and the
race can stall or never end. So B fails the soak for a reason that is not a crash or a missing screen, and the soak
result then repeats the oracle result instead of measuring robustness.

**Runtime per race.** Replay: as A. Recording: one human-played match per cell, real time, so the 118 cells of
section 4.1 at the 5 to 8 min estimate above are about 10 to 16 h of a person at the machine, plus re-takes.

**Failure modes.** Open-loop drift on the exe (above). Any change to the warp, clock or pins that shifts the race
anchor invalidates every recording (rows are keyed by tick after the anchor, `DESIGN_REPLAY-ORACLE.md` section 2).
The `dt = 0` ordering caveat (`RESULT_RO-INPUT-RECORD.md`, "Gap").

**Effort.** Small in code (the recorder is done), large in human time, and it is not unattended in the sense of P-DoD
item 3: an unattended worker cannot supply the recordings (`RESULT_RO-INPUT-RECORD.md`, "Gap").

### 3.3 Side by side

| criterion | A: AI on slot 0 | B: recorded input |
|---|---|---|
| needs a person | no | yes, one match per cell (estimate 10 to 16 h for 118 cells) |
| closed loop on the exe | yes: the AI reacts to the exe's state | no: diverges with the first state difference |
| original code exercised on the exe | the original's AI for all four cars | the original's input path for slot 0, AI for 1 to 3 |
| exercises a human-state slot | no | yes |
| determinism on the original | tick clock, one run per cell `[UNCERTAIN past 2,724 ticks]` | same |
| new code | `slots` field in 3 files, detectors, runner | detectors, runner, real-device recording, ASI record mode |
| unmeasured premises | 3 (section 3.1), one probe closes them | 1 (real device input), needs a person to close |

### 3.4 What A does not cover, and where B fits

Flows keyed to a human-state slot are not exercised by A: the T3 list "pause overlay, unlock progression, cup and
championship flow, TimeTrial ghost save and reload, points, per-car colours" (`ROADMAP.md` T3) and the natural
TimeTrial lap of `D4-SAVE-U3560`. These are per-flow checks, not 118 cells. For each one: a short scripted input
(`input` rows, slot 0 at state 1, the RO-INPUT-RECORD format) where a script reaches it, or one human recording where
it does not. That is B's use: a handful of recordings, kept as oracle references, not the soak.

## 4. Soak matrix runner spec

### 4.1 The matrix (sourced from the repo)

**Tracks: 13 course rows, k = 0..12.** The course table is 13 record pointers at `0x005f2728` and the hook table
has `kHookCount = 13` at `0x005f37a0` (`mashedmod/src/mashed_re/Track/CourseTableState.cpp`, `kCoursePtrs`,
`kHookRecs`). Row k's record dword 0 is the `Course_Id` (`Race/GameModeState.cpp` header comment); matching it to the
hook-table name gives, from those two arrays:

| k | record | Course_Id | name |
|---|---|---|---|
| 0 | `0x005f2570` | 30 | training |
| 1 | `0x005f25fc` | 3 | egypt |
| 2 | `0x005f2660` | 39 | neustein |
| 3 | `0x005f2584` | 0 | arctic |
| 4 | `0x005f264c` | 38 | highway |
| 5 | `0x005f25c0` | 11 | sands |
| 6 | `0x005f25ac` | 25 | superg |
| 7 | `0x005f2598` | 2 | rouabout |
| 8 | `0x005f2624` | 36 | storm |
| 9 | `0x005f25e8` | 34 | forest |
| 10 | `0x005f2638` | 37 | dump |
| 11 | `0x005f25d4` | 33 | warzone |
| 12 | `0x005f2610` | 26 | city |

The same order as the 13-track table's ids 1..12 plus 13 = Training (`verify/rsky_20261016/run13.py`, `TRACKS`).

**Modes.** Valid game-mode ids `{2..10}` (`re/analysis/game_mode_rules_REmap_20260616.md` step 1, `FUN_0042f6b0`) plus
`0xb` attract (`0x0043df00`). Rule per cell (same note, steps 2 and 3):

| mode | name (source) | rule (`DAT_007f0fd0`) | cells |
|---|---|---|---|
| 2 | TimeTrial (`re/analysis/game_mode/0x0043dfd0.md`, jump table `0x00440aac`) | 0 | 13 |
| 3 | Challenge Cup, Bronze (`re/analysis/negctl/reearn_20261011/c4_a1_h_RESULT.md:62`, `re/analysis/negctl/reearn_20261011/c4_a1_m_RESULT.md:60`) | per track, event table `0x005f65c8` | 13 |
| 4 | `[UNCERTAIN]` name, cup family (same table) | per track, `0x005f65c8` | 13 |
| 5 | `[UNCERTAIN]` name, cup family; tracks 3, 7, 12 are the non-elimination rule 5 | per track, `0x005f65c8` | 13 |
| 6, 7, 8, 9 | multiplayer family (`FUN_00430760` returns 0) | from game length `DAT_0067ea88`: 0 / 1 / 2 (action `0xff420000`) | 13 each = 52 (x3 lengths if lengths are in scope) |
| 10 | QuickRace / Quick Battle (`0x0043dfd0.md`; `re/analysis/negctl/reearn_20261011/c4_a1_m_RESULT.md:60`) | 0 | 13 |
| 0xb | attract (`0x0043df00`) | 6 | 1 (track 0, `FUN_0040e170(0)`) |

Total: 13 + 39 + 52 + 13 + 1 = **118 cells** (with the three MP game lengths: 222). The runner reads this list from a
committed `verify/soak_<date>/matrix.tsv` generated from the two tables above, not from a hand list in code.
`[UNCERTAIN]` which cells the shipped game offers at all (unlock state, `TrackAvailGet`); the warp sets the globals
directly and bypasses unlock gating, which is acceptable for a crash and completion soak, and the menu-entered subset
(4.2) uses an edited save in a private root (`c4_hud_scenario.py` `--reset-save`). Never the unlock patches on
`original\MASHED.exe` (CLAUDE.md).

Mode 2 is a single-car lap mode (`0x00410510` returns 0 for mode 2, handled elsewhere, `game_mode_rules_REmap`
step 4). Its end condition is not the score rule. `[UNCERTAIN]` what ends a TimeTrial run and whether the AI drives a
lone slot 0; `SOAK-P0` includes one mode-2 cell.

### 4.2 Entry route per cell

- **Warp entry** (all 118): boot to the main menu, warp at `warp.after_ticks` with the cell's track, mode, rule and
  `slots`. Breadth at the lowest cost.
- **Menu entry** (one cell per mode, 10 cells): statenav / cup route (`re/frida/statenav.py`,
  `c4_hud_scenario.py --route qb|cup`) with the clock pinned, then the AI slot set at the race-launch point. This is
  the route that proves Race Results through the real frontend stack and is the fallback if `SOAK-P0` gap 3 shows
  the warp does not reach Race Results. On the exe it needs T1 (menus with no stand-in).

### 4.3 Pass criteria, per cell

A cell is GREEN when all of these hold:
1. **Match end:** `DAT_0063ba8c == 0xb` observed, with `DAT_0063b90c == 1` and `DAT_0063b914` in 0..3
   (`points_system.md`, `0x00410510`). Mode 2: its own end event (`[UNCERTAIN]`, from `SOAK-P0`).
2. **Results screen:** after (1), `0x0067eca4 == 3` and `*(0x0067ed3c + d*0x40) == 5` with `d = *(0x0067e9f8)`
   (`re/frida/c4_detect_e_scenario.py:47`, `:128`), held for 60 ticks. On the exe the same words are read at the same
   addresses: the top-down path keeps the original's globals at their original addresses (`ROADMAP.md`, CLAUDE.md
   "Roadmap"). Read in the tick listener (native, once per tick), never a per-frame JS callback.
3. **No crash:** process alive until the runner ends it; no first-fault event (`--crash-catch` on the original, the
   exe's own unhandled-exception path on the exe); no new minidump (parse with `scripts/parse_minidump.py`);
   exit code of the runner's own stop only. A piz-collision signature (U-9677) on the original is a re-run, max 2.
4. **No hang:** the tick counter advances at least once per 10 s wall; the cell's tick cap (default: 3x the
   original's ticks to (2) for that cell once known, else 120,000 ticks = 33 min at 60 tps) not reached.
5. **Log checks (exe):** zero `[ORIG_TOP] rva ...: stand-in` lines in `mashed_re.log` after the race anchor
   (`Match/PortTop_StandIns05.cpp:13` format), or each one listed in the cell row with its `BINDINGS.tsv` entry; the
   replay log has its `warp at tick` and `race anchor at tick` lines (`PortTop_Replay.cpp:287`, `:299`).
6. **Control:** one cell per batch re-run; its ticks-to-(1) and winner equal the first run (determinism check).

RED cells carry the first failing criterion, the tick, and for (3) the faulting RVA. The original's per-cell
ticks-to-match-end and winner are the reference column; on the exe a different winner or tick is reported, not
failed (that is the oracle's job, not the soak's), unless (1) to (5) fail.

### 4.4 Runner shape

- `re/tools/soak/soak_matrix.py run --side orig|exe --matrix <tsv> --out D:/MashedWT/<step>_data [--cells ...]`
  and `score --out ... > verify/soak_<date>/SOAK_<side>.tsv`; same split as `verify/rsky_20261016/run13.py`
  (`run` / `score`).
- One fresh process per cell, private root `D:\MashedWT\<step>_root`, `MASHED_ROOT` set, every launch wrapped as
  test-slot outside and `mashed_lock.py run --label <step>-<cell>` inside, kill only the PID it spawned
  (`WORKER_RULES.md`). Original side via `detorig.py --boot --clock tick --asi` with a generated per-cell config;
  exe side via `MASHED_REPLAY=<cell>.json` and `MASHED_ORIG_TOP`, object build (L-125, `ROADMAP.md`).
- Prints `[N/M] <track>-<mode> GREEN|RED <criterion>` per cell (workspace progress-line rule).
- Commits only the scored TSV and a RESULT; raw logs stay in `D:/MashedWT/<step>_data` with SHA-256 in the RESULT
  (`WORKER_RULES.md` "Evidence").
- Wall time (estimate from 3.1): 118 cells x 5 to 8 min = 10 to 16 h per side at 60 fps; less if uncapped race phases
  hold under the tick clock (`SOAK-P0` measures). Run with `run_in_background` and a timeout covering the batch, or
  in batches of about 20 cells.

## 5. Plan of rows

### 5.1 `SOAK-P0` (first, original only, harness row, no build)
`slots` field in `re/frida/js/detorig.js` / `re/frida/detorig.py` plus a `--scen` JSON option. Runs (each under the
lock): (a) track 0 mode 10, `slots [1,2,2,2]`, no input: today's baseline; (b) same with `[2,2,2,2]`; (c) `[2,2,2,0]`;
(d) one mode-2 cell; (e) one MP-family cell; (f) control re-run of (b). Measure per run: car 0 displacement over the
first 600 ticks after the anchor, ticks to `DAT_0063ba8c == 0xb`, whether the results detector fires after a warp
entry, wall time at `MASHED_FPS_CAP_RACE` unset and `0`. Closes the three `[UNCERTAIN]` of 3.1 and the mode-2 end
question. If (b) shows car 0 not driven, A falls back to an input-level AI (feed slot 0's own AI control output into
the `0x00497310` override); that fallback is a new design row, not assumed here.

### 5.2 Then
`SOAK-SLOTS` (exe + ASI field, default unchanged), `SOAK-RUNNER`, `SOAK-ORIG` (118 cells on the original, gives the
reference column), `SOAK-EXE` after T2's exit (`RO-ORACLE-GATE`), `SOAK-HUMAN-FLOWS` (section 3.4, scripted input or
a few recordings).

## 6. Decision status

- A against B: **no user decision needed.** B cannot run unattended (it needs a person per cell) and is open loop on
  the exe; A is the ROADMAP's named candidate (`ROADMAP.md` T3) and the original itself uses the slot configuration
  A needs (`0x0043df00`).
- Assumption a lead may want confirmed (not blocking): P-DoD item 3 "every track in every mode ... results screen
  reached" is met by cells whose four cars are all AI (slot 0 at state 2), with the human-slot flows checked
  separately (3.4). If Mariano reads item 3 as requiring a human-state slot 0, the soak needs B-style input for every
  cell and stops being unattended; the options would then be (a) accept AI cells for item 3 (recommended), (b) record
  118 human matches, (c) scripted per-track inputs (not closed loop, same drift as B on the exe).

## 7. Path check

Every file path cited above was checked to exist on `orch/integration` `ba39eeb6b` (2026-11-13): 46 full paths, 0 missing. Short forms (`ReplayAsi.cpp`, `points_system.md`, ...) are repeats of a full path cited earlier in the same section. `replay.json` is the format name, not a file.
