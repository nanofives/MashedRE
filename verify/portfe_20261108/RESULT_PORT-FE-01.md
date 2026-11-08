# PORT-FE-01: lane A frontend + input swaps, 7 A-dual folds (2026-11-08)

Branch `orch/PORT-FE-01` (worktree `.worktrees/PORT-FE-01`, off `orch/integration` @ 80d933fa7). Recipe:
`verify/portplan_20261108/PLAN.md` section 4. No `original/`, master Ghidra, `.ps1`/`.sh`, CONFIDENCE or tracker-note write.

## Verdict

**GREEN.** Build OK, rva-lint NEW=0, relocheck OK on every matched body in scope (14 RVAs, 0 unchecked), port column unchanged on all 18 rows, default gates unchanged on the worktree exe (G-CTRL 8284be46/67931834/31fe043e, B 1d5bf4f9 x3).

- **1 swap** (0x0040e470 CarSlotStateGet): the hand body is gone, the byte-matched source is the one body in both
  targets. port stays `called`.
- **1 exe caller rebind** (A-dual): `Frontend/SmallLeaves_n2.cpp` ElapsedVsThresholdCheck (0x004307a0) now calls
  0x00429a70 / 0x00429a80 / 0x00429a90 by name (the matched exports) in the exe.
- **9 rows were already done** before this step (matched body is the one body, no hand copy left): 0x00408a50,
  0x00408ad0 (MATCH-S01), 0x00426cb0, 0x004298c0, 0x00429a70, 0x00429a80, 0x00429a90 (MATCH-S06), 0x0042af50,
  0x0042bde0, 0x00431b70 (MATCH-S07). See "Why the plan saw hand / dual copies".
- **7 rows NOT-SAFE** (reasons below), 0x0042d3e0 and 0x00497450 among them only for their port-side analogue: their
  matched body is already the RVA's one body.

## Why the plan saw hand / dual copies that are not there

`lanes.tsv` took its body kind from the `hooks.csv` `exe_file` column, which lists every exe TU that names the RVA,
including TUs where only a comment remains after an earlier fold ("MATCH-S06: the body of 0x00429a70 moved to ...").
`scripts/rva_body_scan.py` on this branch gives exactly one body per target for all 18 RVAs (table `rows.tsv`,
column `exe_body_after` is the `exe_file` value, not the body list). 0x00408a50 / 0x00408ad0 were flagged `handport`
because their matched source is `re/match/s01/00407a60.c` (three functions in one file), not
`re/match/s01/raw/00408a50.c`.

The memory rule "a dual copy can be a call-through" applied here too: `SmallLeaves_n2.cpp` held only
`reinterpret_cast` pointers to 0x00429a70/80/90, no bodies.

## Swapped (machine form `swaps.tsv`, `relocheck.tsv`)

| rva | port symbol | source | match.py (DDK 9176/9178, /O2 /GS /Ob1) | relocheck | wrapper (exe + asi) | old TU |
|---|---|---|---|---|---|---|
| 0x0040e470 | CarSlotStateGet | re/match/s02/0040e470.c | 100.0%, 15 B whole symbol BYTE-IDENTICAL | OK (1/0) | Match/MatchS02_0040e470.cpp | Ai/CarSlotState.cpp |

- Wrapper in the MATCH-S02 shape (`MATCH_PORT`, `#define FUN_0040e470 CarSlotStateGet`, `extern "C"` include,
  same `RH_ScopedInstall`). The one global `PTR_PTR_005f2770` binds to its original address (preamble `MGLOBAL`),
  the same address the hand body read. No callees. Added to both rsp lists and `x87_tus.txt`.
- Same read as before (`[0x005f2770] + i*4 + 0x34`), no guard either way (the hand getter had none on purpose, see
  `Ai/CarSlotState.cpp` header). Its by-name exe callers (`Ai/AiLeafTargets.cpp`, `Ai/AiTargetClosest.cpp`,
  `Match/MatchS07_0042af50.cpp`) are unchanged.

## Caller rebind (A-dual)

`Frontend/SmallLeaves_n2.cpp`: under `MASHED_STANDALONE` the three callee pointers of ElapsedVsThresholdCheck now point
at `LapFracGetBySlot` / `LapLapsGetBySlot` / `LapSecsGetBySlot` (Match/MatchS06_00429a{70,80,90}.cpp). They pointed
at 0x00429a70/80/90, which the exe does not map (only the six thunks at `exe_main.cpp:9727-9732`: 0x0042b8b0,
0x0042b8c0, 0x0042d5a0, 0x00472f40, 0x004730b0, 0x00472c60), so any exe call faulted. The exe reaches
ElapsedVsThresholdCheck by name only through `Match/MatchS08_0043aee0.cpp` (`#define FUN_004307a0`). The `.asi`
keeps the address calls, so its diffs are unchanged. The three rows stay `port=body`: the static graph from WinMain
does not reach 0x004307a0 (`backfill_port.py`: no row changed).

## NOT-SAFE (left as is)

| rva | reason |
|---|---|
| 0x0040e480 CarSlotStateSet | the exe body has a null-base guard the original lacks (`Ai/CarSlotState.cpp:52`); in the standalone `[0x005f2770]` reads 0 and the matched body would AV (also MATCH-S02 RESULT line 113) |
| 0x00430830 SplitScreenTrackAssignment | port-side rebind: the exe runs `SplitScreenTrackAssignmentFor(g_game_state.game_mode, row)` (`MenuNavSM.cpp:997`, `exe_main.cpp:7528`); the matched body reads the mode from `DAT_0067e9fc` |
| 0x004309b0 FrontendModeIndex | port-side rebind: the exe runs `FrontendModeIndexFor(launchMode)` (`exe_main.cpp:2621`); the matched body reads `DAT_0067e9fc` |
| 0x004325c0 | behaviour knob `MASHED_NO_ANIM_TRUNC` in `MenuAnimTickBody`, and the exe runs it on `g_records` with a port-side input struct (`MenuAnimTick.h`) |
| 0x00432b30 MenuPromptStrip | `.asi` entry is a naked register-ABI thunk (EAX/ESI), the exe runs only the split tag lookup `PromptStripTag` on port records with its head in the Nav caller (`PromptStripTwin.cpp:12-18`) |
| 0x0042d3e0 (hand analogue only) | the matched MenuEntryArrayInit is already the RVA's one body and the exe calls it (`exe_main.cpp:9192`, boot milestone B7). The second, port-side analogue `RecordsZero` (`MenuNavSM.cpp:404`) clears `g_records` (port storage) and on purpose more than the original (pad20, pad30, entry 30; comment at `MenuNavSM.cpp:398-402`, open parity question) |
| 0x00497450 (hand analogue only) | the matched Pred497450 is the RVA's one body. `Q_PlayerActive` (`MenuNavSM.cpp:469`) reads `g_game_state.player_active` (port storage) with an extra `player < 0` guard |

None of the NOT-SAFE rows is a "callee with no exe body" case.

## Port column before / after (`rows.tsv`)

All 18 rows: port unchanged (11 `called` stay `called`, 7 `body` stay `body`). `backfill_exe_file.py` changed one
row (0x0040e470: `Ai/CarSlotState.cpp` -> `Match/MatchS02_0040e470.cpp` in `exe_file`); `backfill_port.py`:
"hooks.csv already current", totals none=5311 body=587 called=253.

The `hooks.csv` `file` column of 0x0040e470 still names `Ai/CarSlotState.cpp`. `c4_reclass.py` has no file-column
operation (the row is already C4), so that leg is left for a re-classify pass.

## Allowlist delta

None. `re/tools/dual_copy_allowlist.txt` holds no key for any of the 18 RVAs before or after (the earlier folds
retired them). rva-lint on this branch: `CROSS-TARGET=1 allowlisted=1 NEW=0`, asm-lint OK.

Ledger note: `D4-DC-INC-00426cb0` (stage `blocked`, allowlist key `DUP-IN-TARGET:00426cb0`) is resolved on
orch/integration already: the key is gone and the body scan finds one body (Match/MatchS06_00426cb0.cpp). The lead can
close it.

## Build

`cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`: Build OK, shared .asi not deployed. Frozen copy
`D:/MashedWT/PORT-FE-01_data/exe/`: exe sha256 `1173512e16ec7097fc15e502520991d3f82c7a9fc8128766bd2d34ef1136f954`,
asi `2fde05e114e3a25bc8f6abea6dcca7005508e2a2ece129c26fa5a7f9ed63c032`.

## Gates

Run on the frozen exe `1173512e` under `test-slot` (outside) and `mashed_lock.py run --label PORT-FE-01` (inside),
private root `D:/MashedWT/PORT-FE-01_root`, `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
Refs from `re/orchestrator/merge_train.json`.

| arm | file | sha8 | ref | |
|---|---|---|---|---|
| H_DEF | step.csv (13962 lines) | 8284be46 | 8284be46 | same |
| H_DEF | gates.csv (54085 lines) | 67931834 | 67931834 | same |
| H_DEF | ptrace.log (27042 lines) | 31fe043e | 31fe043e | same |
| B_DEF_K0 | step.csv (762 lines) | 1d5bf4f9 | 1d5bf4f9 | same |
| B_DEF_K1 | step.csv (762 lines) | 1d5bf4f9 | 1d5bf4f9 | same |
| B_DEF_K26 | step.csv (762 lines) | 1d5bf4f9 | 1d5bf4f9 | same |

All arms exit=0, self-exited. No hash moved, so no bisect and no flip candidate. The first attempt (`gates1.log`) stopped
after H_DEF ran because `run_arms.py` does not create `A4_OUT` (the merge train does); the folder was created and the
set re-run whole.

## nav_coverage

`re/tools/nav_coverage.py` (static, over `MenuNavSM.cpp` tables and `exe_main.cpp`): output identical on this branch and
on orch/integration f1c50169e (34 screen ids, 33 with tables, 11 reachable from root 1, table hole [27], both). This
step does not touch `MenuNavSM.cpp` or a screen table.

## Raw data (not committed), D:/MashedWT/PORT-FE-01_data/

| file | sha256 (first 16) |
|---|---|
| exe/mashed_re.exe | 1173512e16ec7097 |
| exe/mashed_re_dev.asi | 2fde05e114e3a25b |
| arms1/H_DEF.step.csv | 8284be46b3c692f3 |
| arms1/H_DEF.gates.csv | 6793183492e0c082 |
| arms1/H_DEF.ptrace.log | 31fe043e261d2df9 |
| arms1/B_DEF_K0.step.csv | 1d5bf4f980bf7eb4 |
| arms1/B_DEF_K1.step.csv | 1d5bf4f980bf7eb4 |
| arms1/B_DEF_K26.step.csv | 1d5bf4f980bf7eb4 |
| gates2.log | 61a81a5155d075a4 |
| build1.log | 30a55b08de8c5276 |
| navcov_branch.txt, navcov_base.txt | identical |
| hooks_before.csv (hooks.csv at 80d933fa7) | (copy, for rows.tsv) |

## Follow-ups (not in this step)

- Lane B: exe TUs that still call these RVAs by original address (unreachable in the exe today, they would fault):
  0x0040e470 in `Frontend/MenuButtonDetect.cpp:59`, `Frontend/MenuNav.cpp:69`, `Frontend/MenuStateMachine.cpp:36`,
  `HUD/ScenarioWriters_sa2s2.cpp:64`; 0x0040e480 in `Frontend/MenuButtonDetect.cpp:66`; 0x00430830 in
  `Frontend/FrontendNav.cpp:23`; 0x00408ad0 in `Frontend/MenuLeaves_af4.cpp:56`. Rebind by name when their caller
  is wired.
- 0x00430830 / 0x004309b0 / 0x0042d3e0 / 0x00497450 / 0x004325c0: the exe frontend runs on port-side state
  (`g_game_state`, `g_records`). Folding these needs that state at the original addresses first (frontend
  port-state step, same shape as MATCH-PORTSTATE-RECORDS), not a body swap.
- re-classify leg: `hooks.csv` `file` of 0x0040e470 -> `Match/MatchS02_0040e470.cpp`.
