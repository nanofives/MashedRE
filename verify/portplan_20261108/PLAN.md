# Port plan, C4 rows into mashed_re.exe (PORT lead, 2026-11-08)

Source: `hooks.csv` at race/first-frame-parity `09df4709a` (orch/integration has moved to `80d933fa7` since, by
C4R tracker commits only, so the counts below can be off by a few rows). Call graph: `log/xref_mashed.json`
(direct calls only, Ghidra facts of 2026-10-16). Scripts in this folder regenerate every table:
`survey.py` -> `survey.tsv`, `survey2.py` -> `survey_game.tsv`, `closure.py`, `wire.py`, `batches.py` -> `lanes.tsv`.

## 1. Counts

C4 rows 4,164: game 2,547, library 1,617.

Game C4 rows by `port`: none 1,973, body 387, called 186 (+1 row with a malformed port cell, `0x...` "MASHED_STANDALONE only").
2,438 of the 2,547 have a matched source under `re/match/**`.

What the exe holds for each game C4 row with a matched source (body kind read from the `exe_file` TUs: `matched` =
the TU `#include`s the `re/match` file, `handport` = a hand-written body, `dual` = both):

| port | exe body | rows | meaning |
|---|---|---|---|
| called | matched | 100 | done: the game runs the certified body |
| called | handport | 74 | **lane A**: swap the hand port for the matched body (port stays `called`) |
| called | dual | 7 | lane A, plus a dual copy to fold |
| body | matched | 230 | matched body compiled, no exe path reaches it |
| body | handport | 135 | hand body, no exe path reaches it |
| body | dual | 7 | lane A-dual (fold) |
| none | - | 1,883 | no exe body |

Game C4 rows by subsystem (all port values): render 692, audio 366, gameplay 308, util 294, frontend 230, boot 150,
vehicle 119, hud 82, particle 76, ai 55, track 52, save 30, input 27, other 66.

## 2. Reachability: what can become `called`

`called` needs a path from `WinMain` in the linked exe (`scripts/backfill_port.py`). The exe's call graph is its own
(hand-written frame loop, librw renderer), so a matched body nobody calls stays `body` (L-72 said the same for S01-S04).
The frontier is the original's call edges out of rows the exe already calls:

- **Depth 1 (lane B): 170 rows** have an original caller whose exe body is `called` (106 port none, 64 port body).
  How that caller's exe body reaches the RVA today (`wire.py`): by name 74, by absolute address 63, through a matched
  body's `#define FUN_x` 30, not at all (inlined or dropped) 75 (counts are caller edges, a row can have several).
- **Closure** (swap a caller in, its callees become frontier): 281 rows in total, depth 1: 170, 2: 57, 3: 45, 4+: 9.
  By subsystem: frontend 78, render 62, util 41, gameplay 20, vehicle 16, hud 16, ai 13, audio 12, save 7, rest 16.
- **Gated below a non-C4 row: 727 rows.** The closure stops at 27 C2 + 6 C3 game rows and 14 C4 rows without a
  matched source. Two C2 dispatchers gate most of it: `0x004111c0` (448 rows below it) and `0x0040d270`
  Course::Finish (325). These are RE-axis work (match them first), not port work.
- **Under no exe-called root at all: 1,352 rows.** 663 of them hang only off the original CRT `entry` `0x004a4bb7`
  through the original's own top-level loop, the rest off roots reached only by indirect calls (state-handler tables,
  vtables, callbacks the static graph does not see). Porting these means replacing the exe's hand-written top level
  (frame loop, frontend state machine, renderer) with the matched originals: an architecture-level change
  (CLAUDE.md "Stop and ask"), see decision Q1 below.

So the port axis can move by about 280 rows to `called` plus 81 lane-A fidelity swaps without touching the exe's
architecture. Everything else needs Q1 or RE-axis work.

## 3. Library rows

| library | C4 rows | port decision |
|---|---|---|
| msvc-crt-main / -tail / crt | 316 | out of the port count (L-67 / L-70). The exe links the MSVC 2022 CRT |
| libpng 1.0.12, zlib 1.1.3 | 55 | out of the port count (L-67 / L-70) |
| renderware (core) | 320 | replaced by librw in the standalone. No decision row found that says so explicitly |
| RenderWare-Physics-3.7 | 172 (73 body) | hand ports exist (`Collision/RwpIntegrator.cpp` and others). No decision found |
| d3dx9-psgp / shader-compiler / backward-extension | 316 | platform glue (L-94 is the RE side only) |
| directshow-baseclasses | 222 (2 body) | platform glue, the exe's movie path is its own |
| lua 4.0 | 94 | none. The exe's track/powerup script path has no Lua body. No decision found |
| qhull 2002.1 (+continuation) | 122 | none. L-73 (HB-QHULL-DDK) waits for a ported caller of `0x0047d3c0` |

No library row is in this plan's batches. Questions Q2 / Q3 below.

## 4. Swap recipe (lane A, copy of MATCH-S01 / MATCH-S11)

Per row, in a worktree `orch/PORT-<sub>-NN` off orch/integration:
1. Find the exe body (`hooks.csv` `exe_file`, `scripts/rva_body_scan.py`). Read the matched source and the hand port.
   NOT-SAFE (leave the row, say why in the RESULT): the hand port reads/writes through a port-side rebind (rebased
   vehicle record, null-base guards), carries a behaviour knob or a LUT/sqrt-fold knob, is naked x87 asm, is a ShadowAB
   or self-test harness, or the matched body calls an RVA that has no exe body and cannot be bound by name.
2. Wrapper TU `mashedmod/src/mashed_re/Match/Port<Sub>_NN.cpp` (or extend an existing `Match/*.cpp`), MATCH-S01
   shape: `#define MASHED_RE_PORT 1`, `extern "C"`, `#define FUN_<rva> <export name>` for the defined functions,
   `#define DAT_<addr> (*(T*)0x<addr>u)` for globals (the exe maps those pages), and every callee bound to **an exe
   function by name**. An absolute-address callee is only allowed where the replaced hand port already called that
   address in the exe (only 6 RVA thunks exist, `exe_main.cpp:9727`; any other `.text` address faults in the exe).
   Add the wrapper to `x87_tus.txt` (matched code is x87 code). Keep `RH_ScopedInstall` names unchanged.
3. Delete the hand body (and any dual copy). Shrink `re/tools/dual_copy_allowlist.txt` if a key goes.
4. Build both targets (`mashedmod\build.bat`), rva-lint (no new duplicate bodies), `scripts/backfill_exe_file.py`
   then `scripts/backfill_port.py` (rows keep or gain `called`; a row that drops to `body` is a regression).
5. Default gates on the worktree exe through `test-slot` (G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3,
   values in `re/orchestrator/merge_train.json`). Changed hash = flip: bisect to the swap that moves it, drop that
   swap from the batch (record it as a flip candidate with its hashes), and re-gate the rest.
6. Tracker legs only through re-classify / `c4_reclass.py` (the C-level does not move, only notes/file columns).
   RESULT at `verify/port<sub>_20261108/RESULT_PORT-<SUB>-NN.md`, then `roadmap.ps1 set <id> review`.

Lane B (frontier) adds one step between 2 and 3: wire the exe caller. If the caller's exe body is a matched body,
change its `#define FUN_<callee>` to the new export name. If it is a hand port that inlines the callee, either swap the
caller too (it is usually in lane A) or replace the inlined block with the call, and cite the original call site
(`py -3.12 re/tools/callsites.py 0x<callee>`).

## 5. Batches, gates and predicted hash risk

Rows in the existing ledger rows MATCH-S11-FU-VEHBASE (9 vehicle-record leaves, rebind decision) and
MATCH-S19-FU-MATHSAFE (math knobs) are excluded from the batches below and stay with those rows.

| batch | rows | lane | gate | hash risk |
|---|---|---|---|---|
| PORT-SAVE-01 | 9: `0x00404f50 0x00404f80 0x004963e0 0x00496400 0x00498910 0x00498950 0x004989b0 0x00550bc0 0x00410510` | A | build, lint, port, gates | low (save/config path, not the sim). `0x00410510` Race::EvaluateResult is the exception: medium, gate it alone if it moves |
| PORT-FE-01 | 18: frontend lane A 10 + input `0x00497450` + 7 A-dual folds | A | build, lint, port, gates; nav_coverage for touched screens | low |
| PORT-RENDER-01 | 9: `0x00448940 0x00448dc0 0x004756e0 0x004769f0 0x00476df0 0x00498a00 0x00498e40 0x004b4b60 0x004c3c60` | A | build, lint, port, gates, drawlist diff on one track | low-medium (tick frustum reads the last render's planes) |
| PORT-UTIL-01 | 10 (util lane A minus `0x004c3b30`, `0x0046baa0`) | A | gates | medium-high: camera path and Bezier feed the race |
| PORT-GAMEPLAY-01 | 14 | A | gates | high: rank order, thresholds, pontoon bodies, powerup contact are race-sim |
| PORT-VEH-01 | 8 (vehicle lane A minus VEHBASE rows) | A | gates, per-swap | high: wheel drivetrain, aero, integrator. One swap per commit so a flip bisects cheaply |
| PORT-AI-01 | 7 (ai lane A minus VEHBASE rows) | A | gates, per-swap | high |
| PORT-FRONT-FE-01.. | frontier frontend 39 + hud 9 | B | gates, nav_coverage | low |
| PORT-FRONT-RENDER-01.. | frontier render 37 | B | gates, drawlist diff | medium |
| PORT-FRONT-UTIL/GP/VEH/AI | 27 / 15 / 13 / 12 | B | gates, per-swap | high |
| waves 2-3 | about 111 more as the frontier moves | B | as above | as above |

Order: SAVE, FE, RENDER first (low risk, proves the recipe and the callee-binding rule), then the high-risk lane A
batches one at a time, then lane B by subsystem. At most 3 workers live.

"Hash risk" is about behaviour. TRAINING G-CTRL has been stable across code layouts
(memory sim-trajectory-is-code-layout-sensitive), so a moved G-CTRL hash means a behaviour difference between the
hand port and the original. That is a parity fix in principle, but it is still a flip and goes to Mariano with the
bisected swap and both hash sets.

## 6. Decisions needed (not blocking the first batches)

- **Q1** (architecture): the 1,352 rows under no exe-called root and most of the 727 below the C2 dispatchers can
  only become `called` if the exe's hand-written top level (frame loop, frontend state machine, renderer front)
  is replaced by the matched originals, top down (MATCH-S15-FU-MAINLOOP is the entry). Options: (a) keep the
  current top level and port only the frontier (about 360 rows), (b) plan a top-down swap starting at the main
  loop. Recommendation: (a) now, revisit (b) after lane B closes.
- **Q2**: Lua 4.0 (94) and qhull (122) are library code the exe has no body for. In scope for the port count or not?
- **Q3**: RenderWare core (320) is replaced by librw and RW-Physics (172) by hand ports. Record both as decisions
  (out of the port count, like CRT) or not?
