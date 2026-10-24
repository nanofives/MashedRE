# PREREG: D4-AUDIO-VOLTODS-FLIP, VolToDs = the original's AudioLinearToLog 0x005aeb90, knob and port body deleted

Decision: L-6 = A (Mariano, 2026-10-24, `log/orch/DECISIONS.md` row L-6). Flip `MASHED_VOLTODS_ORIG` default ON,
re-gate, then delete the old body and the knob when one body is left. Prior evidence: `verify/audiovoltods_20261024/RESULT_voltods.md`
(S1 0 / 2^32 at PC_53/24/64, S2 table, K witness). Branch `orch/D4-AUDIO-VOLTODS-FLIP` off `orch/integration` `1889fb02d`.

## Change

`mashedmod/src/mashed_re/Audio/AudioEngine.cpp` VolToDs returns `(LONG)(int32_t)AudioLinearToLog(v * g_focusGain)`
unconditionally. Deleted: `VolToDsOrig()` (`MASHED_VOLTODS_ORIG`), `VolToDsPort()` (the port's `2000*log10` body with the
`<= 0.0001f` floor), and the `MASHED_VOLTODS_LOG` log-only knob (it compared the two bodies, and one is gone). Both
knob-ON and the new code compute the same expression, so the deletion is one body, not two. Comment-only edit in
`Audio/AudioLinearToLog.cpp`.

## Predictions

- **G (gates, default env):** `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the new exe gives G-CTRL
  `bacfc747 / 8dd1c004 / 6f9932b6` and B `f9deb9b9 / d3601fcc / 9da9a26a`. Basis: RESULT_voltods K, the B step dump did
  not move with the knob ON and live audio, and nothing in the sim reads the volume.
- **X (static witness on the built exe):** the strings `MASHED_VOLTODS_ORIG` and `MASHED_VOLTODS_LOG` occur 0 times in
  `mashed_re.exe`, and the string `VolToDs v=` occurs 0 times. The .asi body of `0x005aeb90` is not touched (same source).
- **L (lint):** rva-lint 0 NEW.

## Arms

Boot gate: no MASHED boot until `roadmap.ps1 get HB-LOCKPRIO` shows stage done. Private root
`D:\MashedWT\D4-AUDIO-VOLTODS-FLIP_root`, every boot under `mashed_lock.py run --label D4-AUDIO-VOLTODS-FLIP`, sandbox
disabled, explicit PIDs only. A piz-collision crash (U-9677) is a re-run.

## Verdict rule

GREEN only if G holds on all six hashes and X holds. Any moved gate hash is RED (report the arm, do not re-gate).
