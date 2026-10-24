# PREREG: D4-AUDIO-VOLTODS, 0x005aeb90 split into its own shared TU, then VolToDs behind a default-OFF knob (W0-2)

Written before any arm. Branch `orch/D4-AUDIO-VOLTODS` on `f55e6c202` (from `orch/integration` `57d939099`).
Build (`MASHED_NO_DEPLOY=1`): exe `fa498bb1`, dev .asi `7141ab85`. Lint 8 findings, all allowlisted, 0 NEW.
Decision row: `re/orchestrator/DECISIONS.md:25` (W0-2: file split first, then VolToDs -> port behind a default-OFF knob +
PREREG, flip decided after measurement). Survey: `verify/audiolink_20261023/SURVEY.md:585-599`.

## Change (three commits, no default changes)

1. `7250e49e5` AudioLinearToLog 0x005aeb90 moves out of the asi-only `Audio/TimerGetters_ah5.cpp` into the new
   `Audio/AudioLinearToLog.cpp`, in both rsp files. The .asi body is byte-for-byte the old one. Under
   `MASHED_STANDALONE` the two operands read private copies: `0x005cc990` = `0x3727c5ac` (1e-5f) and `0x005cd0b8` =
   `0x44fa0000` (2000.0f), both `.rdata` (characteristics `0x40000040`), read from `MASHED.exe.unpatched` by
   `extract_pages.py` (`consts.txt`). `AudioEngine.cpp` VolToDs calls it only under **`MASHED_VOLTODS_ORIG=1`**
   (default OFF): `v *= g_focusGain` stays port-side, then `(LONG)(int32_t)AudioLinearToLog(v)`.
2. `b96fd48ce` one log line when the knob is on (arm witness).
3. `f55e6c202` **`MASHED_VOLTODS_LOG=1`** (log only, the returned value is unchanged): the first 400 VolToDs calls with
   input bits, the port form, the original form and the returned value.

Name: W0-2 says `AudioLinearToLog`. The src name is the same (`hooks.csv:3509`, C3, `log/diff_audio_linear_to_log.csv`).
The orchestrator note's [UNCERTAIN] on the name is closed: one src hit, `Audio/TimerGetters_ah5.cpp:51`, exported as
`AudioLinearToLog`.

## What the original does with the value (static, report only)

Direct callers (E8 scan of `MASHED.exe.unpatched`): `0x005b8a1b`, `0x005b8f62`, `0x005b955f`, `0x005b96b7`.
At `0x005b8f62` EAX is pushed into `call [edx+0x3c]` on the object at `[esi+0xdc]` (`0x005b8f74`), vtable slot 15 =
`IDirectSoundBuffer::SetVolume` [UNCERTAIN: the object type is read from the slot index only]. At `0x005b955f` and
`0x005b96b7` the value is stored to `[esi+0x40]` / `[ecx+0xcc]`. Which thread runs these calls, and its x87 precision
control, is [UNCERTAIN] (arm S3 measures whether the precision changes the result).

## Arms (sandbox disabled; explicit PIDs only; game runs under `mashed_lock.py run --label D4-AUDIO-VOLTODS`)

- **S offline sweep** (`sweep.cpp`, `build_sweep.bat`, `extract_pages.py`). The original's `0x005ae000` page and the two
  `.rdata` pages are mapped at their own addresses and the original's 97 bytes run on this CPU. All 2^32 input bit
  patterns.
  - **S1** original bytes vs the exe body (`/DMASHED_STANDALONE`), at PC_53, PC_24 and PC_64.
  - **S2** original vs the port VolToDs (verbatim copy, `g_focusGain` = 1), per input range, plus the delta histogram
    over `[+0, 1.0f]`.
  - **S3** original at PC_24 vs original at PC_53.
- **P path1.** `run_diff.py audio_linear_to_log` (18 vectors) with .asi `7141ab85`: the moved .asi body against the
  original.
- **G gates.** Exe `fa498bb1`, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, root `D:/MashedWT/D4-AUDIO-VOLTODS_root`.
- **K knob, live audio.** `run_arms.py B_UL_K0 B_ULV_K0` (1200 frames each, `MASHED_MUTE` removed so DirectSound
  initialises, `MASHED_VOLTODS_LOG=1`; ULV adds `MASHED_VOLTODS_ORIG=1`).

## Predictions

- **S1.** 0 mismatches at each of PC_53, PC_24, PC_64 (same instructions, operands equal to the image's).
- **S2.**
  - negative finite, -0, -inf, +0, `(0, 1e-5f)`: 0 differ (both -10000).
  - `[1e-5f, 1e-4f]`: the port returns -10000 (`v <= 0.0001f`), the original `trunc(f32(2000*log10 v))` in
    [-10000, -8000]. More than 99% of the range differs, max |d| = 2000.
  - `(1e-4f, 1)`: differences only where the f32 rounding of the product crosses an integer (the original rounds to f32
    before the truncating FISTP, the port truncates a double). |d| <= 1, fewer than 1% of the range.
  - `[1, FLT_MAX]`: 0 differ (both 0).
  - `+inf` and NaN: differ. The original's FISTP of inf / NaN gives `0x80000000` and its `SETG/DEC/AND` keeps it; the
    port returns 0 (+inf) and -10000 (NaN).
- **S3.** Report only, no prediction.
- **P.** 18 / 18 GREEN, 0 mismatches.
- **G.** G-CTRL `bacfc747 / 8dd1c004 / 6f9932b6`, B `f9deb9b9 / d3601fcc / 9da9a26a`, unchanged.
- **K.**
  - B_UL_K0 and B_ULV_K0 give the same step-dump sha (no sim reader of the volume). Whether they equal B_DEF_K0
    (`f9deb9b9`) is report only (unmuting adds the DirectSound init).
  - B_ULV_K0's log carries the knob line and every `VolToDs` line has `ret == orig`. B_UL_K0's has `ret == port` and no
    knob line.
  - Report only: the distinct inputs seen in-game and how many have `port != orig`.

## Decision rule

- S1, P, G and the K witness lines hold: commit the RESULT, ledger `review`. The default stays OFF. The flip is a
  decision row for Mariano with the S2 / K numbers, never this step's call.
- S1 or P fails: the moved body is wrong, report RED, no flip row.
- G fails: stop, report, no commit of further code.
- K hashes differ between UL and ULV: report it (a sim reader of the volume would be a finding), no flip row.
