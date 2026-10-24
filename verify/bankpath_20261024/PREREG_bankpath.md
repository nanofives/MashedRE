# PREREG: D4-AUDIO-BANKPATH, 0x004625b0 CharacterBankPath as one shared core (W0-1)

Written before any arm and before the code change. Branch `orch/D4-AUDIO-BANKPATH` off `orch/integration`
`30913130b`. User decision W0-1 (`re/orchestrator/DECISIONS.md:30`): shared core, thin wrapper, output uppercase
with backslashes, the `original/` prefix moves to the caller `RaceSession.cpp:192`, default-OFF knob + PREREG first.
Survey: `verify/audiolink_20261023/SURVEY.md:349-373`.

## State before

- `Audio/AudioCharacterBankPaths.cpp` (asi-only, `asi_sources.rsp:38`) holds the 0x004625b0 port
  `AudioCharacterBankPathsBuild` (C3, `hooks.csv:988`): all six slots of `DAT_00690158` (stride 0x80), each
  `snprintf(slot, 0x80, "toastaudio\pc\audio\pcdics\%s\%s.rws", lang, NAME)`, names uppercase
  (`AudioCharacterBankPaths.cpp:53-55, 70-79`).
- The exe twin `Audio::CharacterBankPath` (`AudioEngine.cpp:306-320`) builds one character on demand:
  lowercase name (`:301`), forward slashes, `original/` prefix (`:318`). Its only caller is `RaceSession.cpp:192`,
  which only logs the string (`player voice bank: colour=%d -> %s`). No file is opened from it.

## Change

1. **Shared core** in `Audio/AudioCharacterBankPaths.cpp`, declared in a new `Audio/AudioCharacterBankPaths.h`:
   `mashed_re::Audio::CharacterBankPathCore(int charIndex, int langCode, char* buf, int cap)` writes
   `toastaudio\pc\audio\pcdics\<lang>\<NAME>.rws` with one `snprintf(buf, cap, ...)`, the 0x004625b0 language
   switch and the uppercase `DAT_006041f0` names. Returns false on charIndex outside 0..5, null buf or cap <= 0.
2. **.asi wrapper**: `AudioCharacterBankPathsBuild(langCode)` loops the six slots and calls the core with
   cap 0x80. Same bytes as before (same format, same cap, same per-slot `snprintf`).
3. **Exe knob** `MASHED_BANKPATH_CORE=1`, **default OFF**. ON: `Audio::CharacterBankPath` returns the core's
   output (the thin wrapper), and `RaceSession.cpp:192` prefixes `original/` itself. OFF: both keep today's body
   and output byte for byte.
4. `Audio\AudioCharacterBankPaths.cpp` is added to `exe_sources.rsp`. The fixed-address `DAT_00690158` buffer
   stays in the .asi wrapper only. The exe never calls the wrapper (no caller by name), and `RH_ScopedInstall`
   resolves to `Stubs\HookSystemNoOp.cpp` in the exe.

After the flip (a decision row, not this step), the lowercase body and `CharacterBankName` in `AudioEngine.cpp`
go, `CharacterBankPath` stays the thin wrapper, and the knob is removed.

## Arms

- **P path1.** `run_diff.py audio_char_bank_paths_4625b0` (abs_ranges 0x00690158 x 0xC0 dwords, tests 0..4)
  on the rebuilt dev .asi, loaded from the worktree build (no redeploy).
- **L lint.** `py -3.12 scripts/lint_rva_bodies.py`: 0 NEW.
- **E exe knob.** Arms `B_DEF_K0` and `B_BP_K0` (`STACK['BP'] = ['MASHED_BANKPATH_CORE=1']`) from a lane copy of
  `verify/savelink_20261023/run_arms.py`. Read the `player voice bank:` line in each arm's `mashed_re.log`.
- **G.** Default gates on the new exe: `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.

## Predictions

- **P.** 5 / 5 GREEN.
- **L.** 0 NEW.
- **E.** `B_DEF_K0` logs `original/toastaudio/pc/audio/pcdics/english/<name>.rws` with a lowercase name (today's
  string). `B_BP_K0` logs `original/toastaudio\pc\audio\pcdics\english\<NAME>.rws`, uppercase, and that path
  exists on disk under the run root. `B_BP_K0`'s step hash equals `B_DEF_K0`'s (the string is only logged).
- **G.** Unchanged: G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc / 9da9a26a`.

## Decision rule

All four hold: commit, hooks.csv `exe_file` for 0x004625b0 -> `Audio/AudioCharacterBankPaths.cpp` through
re-classify (level kept at C3), ledger to review, file the flip as a decision row. P fails: report RED, no commit
of the code. G or the E step hash moves: stop and report.
