# RESULT: D4-AUDIO-VOLTODS, 0x005aeb90 split into Audio/AudioLinearToLog.cpp, VolToDs behind MASHED_VOLTODS_ORIG (W0-2)

PREREG: `PREREG_voltods.md` (`38e5d658a`, committed alone before any arm). Branch `orch/D4-AUDIO-VOLTODS`.
Arms ran on exe `fa498bb1` / .asi `7141ab85` (code `f55e6c202`). G was re-run on the final build, exe `7b7fa3fc` /
.asi `2d32b943` (code `0e7b07b75`, a comment-only change). These builds are not byte-reproducible: an exe-only source
change also moved the .asi hash. Evidence: `sweep_out.txt`, `path1.txt`, `arms_run1.txt`, `arms_run2.txt`,
`K_UL_audio.txt`, `K_ULV_audio.txt`, `consts.txt`. Raw: `D:/MashedWT/D4-AUDIO-VOLTODS_data/`.

**Verdict: GREEN. Every prediction holds. The default stays OFF. The flip goes to Mariano as a decision row.**

## What changed

- `0x005aeb90` AudioLinearToLog moved out of the asi-only `Audio/TimerGetters_ah5.cpp` into `Audio/AudioLinearToLog.cpp`,
  which is in both rsp files. The .asi body is unchanged. In the exe the two `.rdata` operands come from private copies:
  `0x005cc990` = `0x3727c5ac` (1e-5f) and `0x005cd0b8` = `0x44fa0000` (2000.0f), read from `MASHED.exe.unpatched`.
- `AudioEngine.cpp` VolToDs: `MASHED_VOLTODS_ORIG=1` (default OFF) returns `(LONG)(int32_t)AudioLinearToLog(v * g_focusGain)`.
  `g_focusGain` (the port's focus mute) stays port-side. `MASHED_VOLTODS_LOG=1` is log only: the first 400 calls with
  both forms.
- Lint 0 NEW (8 known, allowlisted).
- PREREG correction: the old body sat at `TimerGetters_ah5.cpp:49` on `orch/integration`, not `:51`.

## S: offline sweep, all 2^32 inputs (`sweep.cpp`)

The original's `0x005ae000` page and both `.rdata` pages are mapped at their own addresses, and the original's 97 bytes
run on this CPU.

- **S1 (original bytes vs the exe body): 0 / 4,294,967,296 mismatches at PC_53, PC_24 and PC_64.** Prediction held.
- **S3 (original at PC_24 vs PC_53): 0 inputs differ.** The caller thread's x87 precision does not matter, which closes
  the PREREG's thread / precision-control [UNCERTAIN] for this function's output.
- **S2 (original vs port VolToDs), as predicted:**

  | Input range | Inputs | Differ | max abs(orig - port) |
  |---|---:|---:|---:|
  | negative finite, -0, -inf | 2,139,095,041 | 0 | 0 |
  | +0 | 1 | 0 | 0 |
  | (0, 1e-5f) | 925,353,387 | 0 | 0 |
  | [1e-5f, 1e-4f] | 27,914,604 | 27,914,597 | 2000 |
  | (1e-4f, 1) | 112,085,224 | 17,873 (0.016%) | 1 |
  | [1, FLT_MAX] | 1,073,741,824 | 0 | 0 |
  | +inf | 1 | 1 | 2^31 |
  | NaN | 16,777,214 | 16,777,214 | 2^31 - 10000 |

  - In `[1e-5f, 1e-4f]` the port returns -10000 (silence) where the original returns -10000 to -8000. That is the
    -100 dB to -80 dB band.
  - In `(1e-4f, 1)` every difference is orig - port = -1, one hundredth of a dB, where the f32 product rounds across an
    integer before the truncating FISTP.
  - For +inf and NaN the original returns `0x80000000`. The port returns 0 for +inf and -10000 for NaN.

## P: path1, the moved .asi body

`run_diff.py audio_linear_to_log`, .asi `7141ab85`: **18 / 18 GREEN, 0 mismatches.** Prediction held.

## G: gates

- **Run 1 (exe `fa498bb1`):**
  - H_DEF `bacfc747 / 8dd1c004 / 6f9932b6`
  - B_DEF_K0 / K1 / K26 `f9deb9b9 / d3601fcc / 9da9a26a`
  - Unchanged.
- **Run 2 (exe `7b7fa3fc`, final code):** H_DEF-2 `bacfc747 / 8dd1c004 / 6f9932b6`, B_DEF_K0 / K1 / K26-2 `f9deb9b9 / d3601fcc / 9da9a26a`. Unchanged.

## K: knob arms with live audio (MASHED_MUTE removed, MASHED_VOLTODS_LOG=1)

- B_UL_K0 and B_ULV_K0 step dumps are both `f9deb9b9`, equal to each other and to B_DEF_K0. Nothing in the sim reads the
  volume, and DirectSound init does not move the step dump.
- First run: the witness was lost. The root had no `log/` directory, so the audio logger's
  `fopen("log/mashed_re.log")` failed without a message. I created the directory and re-ran both arms as `-2`. Hashes
  were again `f9deb9b9` for both.
- **Witness:**
  - B_ULV_K0-2 carries `VolToDs: MASHED_VOLTODS_ORIG=1, AudioLinearToLog path`. B_UL_K0-2 does not.
  - Each arm logs 5 VolToDs calls on 3 distinct inputs: 0.42 (`3ed70a3e`), 0.60 (`3f19999a`) and 0.63 (`3f2147ae`). The
    returns are -753, -443 and -401. **port == orig on all 5**, so `ret` equals both forms in both arms.
- Report: the 1200-frame B recipe hits only the music and menu volume path, so the knob changes nothing audible there.
  The band where the forms differ (inputs from 1e-5 to 1e-4, -100 to -80 dB) was not reached in-game.

## Trackers (`apply_trackers_voltods.py`, re-classify transaction, level kept C3)

hooks.csv `005aeb90`: file and exe_file now point to `Audio/AudioLinearToLog.cpp`, plus a note. One CHANGELOG line. No
U-rows or S-rows.

## Decision for Mariano (flip)

Should `MASHED_VOLTODS_ORIG` become the default, so the exe's volume law is the original's `0x005aeb90`?
- **A: flip.** The original's law is byte-faithful (S1 0 / 2^32). Measured cost:
  - inputs from 1e-5 to 1e-4 play at -100 to -80 dB instead of silence;
  - 0.016% of inputs from 1e-4 to 1 drop 1 hundredth of a dB;
  - +inf / NaN would reach SetVolume as `0x80000000`.
- **B: keep OFF.**

Recommendation: A. The audible difference sits below -80 dB, the gates do not move, and the knob replaces a port-invented law
with the C3 original body. Evidence: this file, S2 table, K section.
