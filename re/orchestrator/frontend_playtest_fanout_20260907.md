# Frontend playtest fan-out — orchestrator ledger (2026-09-07)

Orchestrator: this interactive session (account2). Merger: same session (applies
child diffs onto HEAD, rebuilds, relaunches). Base commit at fan-out: `a6fd2d46ffd8c5d89fbc4cef9e21fb052db8fabf`.
Uncommitted at fan-out: GameModeCarSelect.cpp (crash fix), playtest_feedback (checklist
correction), this ledger, untracked videocfg.bin.

Trigger: manual playtest of `mashed_re.exe` on a real desktop. Colour-select CRASH
already fixed inline this session (GameModeCarSelect.cpp `CallCursorMover` guarded
under `MASHED_STANDALONE`; the absolute `call 0x00431b80` is injected-only).

## Children

| id | item | mode | account | deliverable | status |
|----|------|------|---------|-------------|--------|
| A | #5 all-players-red — livery/badge reads the picked colour | execution→SCOPING | account2 `[local]` worktree | Ghidra-blocked; spec below | done (no diff) |
| B | row-icon residuals — pulse period / Team-Play category / col-3 arms | execution | account2 `[local]` worktree | R3 col-3 arms MERGED+built; R1/R2 needs-Ghidra | MERGED |
| C | #4 points miscount — finish-order resolver `FUN_0040d590` | scoping-only | account2 worker (read-only) | spec: `re/analysis/points_scoring_spec_20260907.md` | DONE |
| D | #3 full pause-menu chrome — `0xff210000` / `FUN_0043d7c0` | scoping-only | account2 worker (read-only) | spec: `re/analysis/pause_menu_chrome_spec_20260907.md` | DONE |

C and D cannot be *executed* on account2 (Ghidra MCP hard-blocked). They produce a
ready-to-implement spec that account3 (or a later Ghidra-capable session) executes.

## Merge protocol (this session, on child return)

1. Read each child's returned summary (NOT its transcript).
2. A/B: apply the returned diff onto current HEAD by hand (do not merge the stale
   worktree branch — memory `agent-worktree-forks-stale`). Resolve exe_main.cpp
   line drift manually; A and B both touch exe_main.cpp so apply sequentially.
3. Rebuild `mashedmod\build.bat`, relaunch on primary monitor, hand to user to verify.
4. C/D: file the spec under re/analysis/, add a DEFERRED row via re-classify with the
   account3 re-pickup condition.
5. Update the status column above.

## #1b — Colour Select "can't select colours" (desktop playtest 2026-09-07)

Crash fixed, but the screen shows no selection feedback. ROOT-CAUSED live:
- Input + cursor WORK (temp `MASHED_DBG_COLOUR` probe: L/R cycle `DAT_0067ea98`
  0→5, `g_csel_p1_car` mirrors, active=1, S_OK). Probe since removed.
- Defect: screen-4 renderer computes `selCol` then discards it (`(void)selCol;`
  exe_main.cpp ~5123); the moving cursor was removed 2026-08-28. No visible selection.
- Ghidra-gated: screen-4 render notes CONTRADICT (6 static tiles+cursor vs 1 cycled
  livery). Queued for account3 → `re/analysis/colour_select_visual_spec_20260907.md`
  (also folds in the ea98 0..6 vs 6-tile wrap bug). Pairs with §A (#5, same data flow).

Trap logged (memory candidate): `kLogPath` is RELATIVE ("mashed_re.log"), so the
standalone's debug logs land in `./mashed_re.log` (process CWD = repo root), NOT
`./log/mashed_re.log`. Reading the wrong file made 3 working probes look like 0 hits
and nearly produced a false "keyboard device is dead" conclusion. Read `./mashed_re.log`
for exe_main `std::fopen(kLogPath,...)` output.

## MECHANISM DEFECT (found via child A, 2026-09-07)

`Agent(isolation:"worktree")` forked the child from `main` @ `350ac4ca`, **269 commits
behind** the active branch `race/first-frame-parity` @ `a6fd2d46`. `CarSelectCycleColour`
and the whole colour-cycle work don't exist in that checkout, so any execution child is
useless. Confirms + worsens memory `agent-worktree-forks-stale`. Do NOT use worktree
isolation for execution children on this branch. Alternatives: (i) `[local]` subagent with
NO worktree isolation (edits live tree — serialize to avoid collision), or (ii) do the
executable slices in-session.

## A — #5 all-players-red SPEC (Ghidra-blocked here → account3)

- CONFIRMED leg: `CarSlotAssign` (0x0042b9e0, MenuButtonDetect.cpp:374 `*piVar2 = iVar4-1`)
  writes `SlotColour` 0x007f1a1c[player*16] = (car-choice from 0x0067eaf0) − 1. Note
  `re/analysis/frontend_promote_menus_a/0x0042b9e0.md` step 5 corroborates.
- MISSING leg (needs Ghidra): the writer that copies the 0x0067ea98 colour cursor into
  the 0x0067eaf0 choice array on confirm, and its transform. `0x0067eaf0` entry semantics
  beyond [0]=choice id are `[UNCERTAIN U-1652]`.
- account3 fix: decompile the original car-select confirm handler that populates
  0x0067eaf0; then the port's confirm block (exe_main.cpp ~5160-5190 on HEAD) pokes
  0x0067eaf0[p0] with the cursor-derived value before CarSlotAssign — one-site edit.
- `choice → 0x007f1a1c = choice−1` already correct; no change there.
