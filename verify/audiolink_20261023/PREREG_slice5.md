# PREREG: audiolink slice 5, the four TimerGetters_ah5 field getters split into Audio/AudioFieldGetters.cpp

Written before any arm. Branch `work/audiolink` on `9fa8636bc`. Build: exe `439495bc`, dev .asi `8467b4aa`
(slice 4: exe `4004ebb1`, .asi `2f5b419d`). Lint 8 findings, all allowlisted, 0 NEW.
Bytes: `disasm_slice5.txt` (`disasm_slice1.py 005a89a0 005a89b0 005a89c0 005ab370`).

## Change

- VoiceFieldD6cGet 0x005a89a0, VoiceFieldD70Get 0x005a89b0, VoiceFieldD74Get 0x005a89c0 and ChannelField10Get
  0x005ab370 move unchanged from Audio/TimerGetters_ah5.cpp into the new Audio/AudioFieldGetters.cpp, in both rsp files.
- TimerGetters_ah5.cpp keeps AudioRootGet 0x005aad40 and AudioTimerRateGet 0x005ab070 (global reads with no exe
  writer), plus AudioLinearToLog 0x005aeb90 (the job twin of AudioEngine VolToDs, waiting on Mariano). It stays
  asi-only.
- Step 1: each getter is `mov eax,[esp+4] / mov eax,[eax+off] / ret` (off 0xd6c / 0xd70 / 0xd74 / 0x10). Identical,
  no body change. The registry's `thiscall_field_get` passes the pointer as a __cdecl stack argument (ARG_TYPES.md:60),
  which matches the bytes.

## Arms (sandbox disabled; explicit PIDs only)

- **P path1.** `run_diff.py` voice_field_d6c_get, voice_field_d70_get, voice_field_d74_get, channel_field_10_get
  (12 each) with .asi `8467b4aa`.
- **G gates.** Exe `439495bc`, `run_arms.py H_DEF-5 B_DEF_K0 B_DEF_K1 B_DEF_K26`, root D:/MashedWT/savelink_root.

## Predictions

- **P.** 4 / 4 GREEN, 0 mismatches.
- **G.** G-CTRL bacfc747 / 8dd1c004 / 6f9932b6, B f9deb9b9 / d3601fcc / 9da9a26a, unchanged.

No control arm: no body changed.

## Decision rule

P and G hold: commit. hooks.csv: file + exe_file -> Audio/AudioFieldGetters.cpp for the four rows, levels kept, via
re-classify. P fails: report, no commit. G fails: stop for Mariano.
