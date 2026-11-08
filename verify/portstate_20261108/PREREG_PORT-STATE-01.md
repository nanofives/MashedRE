# PREREG PORT-STATE-01 (2026-11-08): frontend game-mode word at DAT_0067e9fc + the 0x00430830 / 0x004309b0 swap

Committed alone, before any arm of this step runs. Branch `orch/PORT-STATE-01` off `orch/integration` @ `ea666edab`.

## Change under test (next commit)

1. **Knob `MASHED_PORTSTATE_FRONTEND`** (default OFF, exe only, read once; `Frontend/PortStateFrontend.{h,cpp}`).
   ON: the exe frontend's game-mode word is DAT_0067e9fc itself. `MenuGameState::game_mode` becomes `game_mode_off`
   (the store used with the knob OFF) and every read and write goes through `Nav_GameModeWord()`
   (`Frontend/MenuNavSM.cpp`, 15 sites, and `exe_main.cpp`, 5 sites). OFF: the word stays in port storage, as before.
   Precedent: L-78 (c) / L-88, MASHED_PORTSTATE_RECORDS (`verify/portstaterecords_20261101/`).
2. **Swap** of 0x00430830 SplitScreenTrackAssignment and 0x004309b0 FrontendModeIndex to their byte-matched sources
   (`re/match/s07/00430830.c`, `re/match/s07/004309b0.c`, both match.py 100% with the DDK and relocheck OK on this
   branch) in both targets (`Match/MatchS07_00430830.cpp`, `Match/MatchS07_004309b0.cpp`). The hand bodies in
   `Frontend/MenuScoreSort.cpp` and `Frontend/FrontendMode.cpp` (TU deleted) are gone. Both were NOT-SAFE in PORT-FE-01
   for blocker (2): the exe called `SplitScreenTrackAssignmentFor(g_game_state.game_mode, row)` and
   `FrontendModeIndexFor(launchMode)`, while the matched bodies read DAT_0067e9fc.
3. **Bridge** (`ModeWordScope`): the three exe call sites (`MenuNavSM.cpp` RowAvail, `exe_main.cpp` s24 arrow test and
   challenge launch gate) put the value the hand core was passed into DAT_0067e9fc for the length of one call to the
   matched leaf and restore the previous value after. Both bodies are leaves (no callees), so the matched body reads
   exactly the value the hand core received, and DAT_0067e9fc holds its previous value before and after every call.

## Predictions

| id | arm / check | prediction |
|---|---|---|
| P1 | default (knob OFF) `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (the `merge_train.json` refs). Reason: the dev-race route the gates use never runs the frontend; the bridge leaves DAT_0067e9fc unchanged outside the call |
| P2 | knob ON `H_PSF B_PSF_K0 B_PSF_K1 B_PSF_K26` | the same hashes as P1. Reason: the gate route writes DAT_0067e9fc itself (exe_main MASHED_GAME_MODE route) and no race code reads the port store `game_mode_off` |
| P3 | knob ON, scripted menu walk (`MASHED_NAV_DEMO=1`) | `[portstate-frontend]` lines in mashed_re.log; at the 0x00430830 sites `bridged=0` on every line (with the knob ON the word passed is DAT_0067e9fc itself). If the walk reaches no 0x00430830 / 0x004309b0 site, P3 is "not reached", not a failure |
| P4 | menu walk frames, knob OFF vs knob ON, same exe | identical walk BMP set sha8 |
| P5 | static | rva-lint NEW=0 with no allowlist line added; one body per target for 0x00430830 and 0x004309b0 (`scripts/rva_body_scan.py`); `backfill_port.py`: both rows stay `called` |

A P1 failure blocks the step (bisect the commit). A P2 / P4 failure is recorded as knob-ON evidence and does not
block (the knob is default OFF), but it is explained in the RESULT before the step goes to review.

## Runner

`verify/savelink_20261023/run_arms.py` with `STACK['PSF'] = ['MASHED_PORTSTATE_FRONTEND']` added (this commit),
`A4_ROOT=D:/MashedWT/PORT-STATE-01_root` (robocopy of `original/`), exe copied to `D:/MashedWT/PORT-STATE-01_data/exe/`,
wrapped in `test-slot` (outside) and `mashed_lock.py run --label PORT-STATE-01` (inside). Menu walk: a copy of
`verify/dualcopy4_20261016/run_nav.py` in this folder that takes the root and extra env, same lock and slot.
