# RESULT: D4-AUDIO-BANKPATH, 0x004625b0 CharacterBankPath as one shared core (W0-1)

**Verdict: GREEN.** Every PREREG prediction holds. One arm was re-routed (below, E), and its prediction is unchanged.
Branch `orch/D4-AUDIO-BANKPATH`. PREREG `c65435a98` (committed alone, before the code). Build: exe `fda8366c`,
dev .asi `9dae1700` (worktree build, `MASHED_NO_DEPLOY=1`, shared `original\mashed_re_dev.asi` untouched).

## What changed

| File | Change |
|---|---|
| `Audio/AudioCharacterBankPaths.h` (new) | Declares `CharacterBankPathCore` and `BankPathCoreOn` |
| `Audio/AudioCharacterBankPaths.cpp` | One slot core `CharacterBankPathCore(charIndex, langCode, buf, cap)`: a single `snprintf(buf, cap, "toastaudio\pc\audio\pcdics\%s\%s.rws")`, uppercase `DAT_006041f0` names, the 0x004625b0 language switch. The .asi export `AudioCharacterBankPathsBuild` (0x004625b0) now loops the six `DAT_00690158` slots over the core (cap 0x80, as before) |
| `Audio/AudioEngine.cpp:306` | `CharacterBankPath`: under `MASHED_BANKPATH_CORE=1` returns the core (the thin wrapper). OFF: today's body |
| `Race/RaceSession.cpp:192` | Under the knob, the caller adds the `original/` root prefix. OFF: same log string as before |
| `exe_sources.rsp` | `Audio\AudioCharacterBankPaths.cpp` added. The fixed-address wrapper is exe dead code (no caller by name), `RH_ScopedInstall` resolves to `Stubs\HookSystemNoOp.cpp` |

## Arms

| Arm | Prediction | Measured | Evidence |
|---|---|---|---|
| P path1 0x004625b0, tests 0..4 | 5 / 5 GREEN | **GREEN, 5 cases, 0 mismatches** | `path1.txt`, `path1_cases.txt` |
| L lint | 0 NEW | **0 NEW** (8 known, all allowlisted) | `lint.txt` |
| E knob OFF | lowercase, forward slashes, `original/` | `original/toastaudio/pc/audio/pcdics/english/red.rws` | `RD_DEF.race.txt` |
| E knob ON | uppercase, backslashes, `original/` from the caller, file exists | `original/toastaudio\pc\audio\pcdics\english\RED.rws`, file present (1,841,152 bytes) in the run root | `RD_BP.race.txt` |
| E step hash, `B_BP_K0` vs `B_DEF_K0` | equal | **f9deb9b9 = f9deb9b9** (and the `-e` repeats) | `RUNLOG.txt` |
| G-CTRL `H_DEF` | bacfc747 / 8dd1c004 / 6f9932b6 | **bacfc747 / 8dd1c004 / 6f9932b6** | `RUNLOG.txt` |
| B `B_DEF_K0 / K1 / K26` | f9deb9b9 / d3601fcc / 9da9a26a | **f9deb9b9 / d3601fcc / 9da9a26a** | `RUNLOG.txt` |

path1 ran on `re/frida/early_window_leaf_diff.py` with `MASHED_EW_ASI` set to the worktree build: `run_diff.py`
refuses `abs_ranges_setter` (no handler in `diff_template.js`), and this is the harness that produced the C3 evidence.
The harness asserts the original's first byte is not `0xE9` (`b0=0x53`).

## Deviation from the PREREG (E arm route)

The `B_` recipe never reaches `RaceSession::Begin`: it runs `MASHED_MUTE=1` and has no frontend race launch, so its
race log has no `[race]` line at all (`B_DEF_K0-e` / `B_BP_K0-e`). The `player voice bank:` read moved to two
`RD_` arms (`navrun.py`): `MASHED_RACE_DEMO=1` + `MASHED_GOTO=6`, audio on, which drive the real
`MenuLaunchRace -> GameFlow_RequestRace -> RaceSession::Begin` (`[gameflow] RequestRace -> LoadingRace` in both logs).
Private root `D:\MashedWT\D4-AUDIO-BANKPATH_root`, its own `log\` folder, explicit PIDs only, every boot under
`mashed_lock.py`. The step-hash half of E stayed on `B_BP_K0`. The prediction text is unchanged.

## Not done here

- The flip (knob default ON, delete the lowercase body and `CharacterBankName`, delete the knob) is a decision row,
  filed to `re/orchestrator/DECISIONS.md` through the ledger.
- Level kept at C3. No C4 claim: the .asi hook was not run installed in a canonical scenario.

## Tracker transaction

`apply_trackers.py fda8366c` (re-classify, level kept): hooks.csv 0x004625b0 `exe_file` ->
`mashedmod/src/mashed_re/Audio/AudioCharacterBankPaths.cpp` plus one note, one CHANGELOG line.

## Flip (L-3 = A, Mariano, 2026-10-24)

**Verdict: GREEN.** Same branch. Build: exe `63ae41b4`, dev .asi `a6741403` (worktree build, no deploy).

- `MASHED_BANKPATH_CORE` is removed. `Audio::CharacterBankPath` (`AudioEngine.cpp`) is now only
  `return CharacterBankPathCore(...)`. The lowercase body and `CharacterBankName` are deleted (no other caller).
- `RaceSession.cpp:192` always adds the `original/` root prefix.
- One body is left for this job: `CharacterBankPathCore` in `Audio/AudioCharacterBankPaths.cpp`.

| Arm | Prediction | Measured | Evidence |
|---|---|---|---|
| P path1 0x004625b0, tests 0..4 | 5 / 5 GREEN | **GREEN, 5 cases, 0 mismatches** (`b0=0x53`) | `path1_flip.txt`, `path1_flip_cases.txt` |
| L lint | 0 NEW | **0 NEW** (8 known, allowlisted) | `lint_flip.txt` |
| E default race launch, no knob | uppercase core path with the `original/` prefix | `original/toastaudio\pc\audio\pcdics\english\RED.rws` | `RD_FLIP.race.txt` |
| G-CTRL `H_DEF-f` | bacfc747 / 8dd1c004 / 6f9932b6 | **bacfc747 / 8dd1c004 / 6f9932b6** | `RUNLOG.txt` |
| B `B_DEF_K0-f / K1-f / K26-f` | f9deb9b9 / d3601fcc / 9da9a26a | **f9deb9b9 / d3601fcc / 9da9a26a** | `RUNLOG.txt` |

Tracker transaction: `apply_trackers_flip.py 63ae41b4` (re-classify, level kept C3): one hooks.csv note on 0x004625b0,
one CHANGELOG line. The `Not done here` item about the flip above is closed by this section.
