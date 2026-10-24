# RESULT: D4-AUDIO-VOLTODS-FLIP, VolToDs = the original's AudioLinearToLog 0x005aeb90, knob and port body deleted

Decision: L-6 = A (Mariano, 2026-10-24, `log/orch/DECISIONS.md` row L-6). PREREG `PREREG_voltodsflip.md` (`8da3c4710`,
committed alone before any arm). Code `c1272dd37` on `orch/D4-AUDIO-VOLTODS-FLIP` off `orch/integration` `1889fb02d`.
Build exe `58ff75a6`, .asi `88b4d380` (`MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not redeployed).
Evidence: `G_arms.txt`, `X_static.txt`, `L_lint.txt`. Raw arm output: `D:/MashedWT/D4-AUDIO-VOLTODS-FLIP_data/arms`.

**Verdict: GREEN. Every prediction holds. One body is left, so the old body and the knob are deleted (L-6 second clause).**

## What changed

- `Audio/AudioEngine.cpp` VolToDs: `v *= g_focusGain; return (LONG)(int32_t)AudioLinearToLog(v);`. The focus mute stays
  port-side, as in the knob-ON form measured by D4-AUDIO-VOLTODS.
- Deleted: `VolToDsOrig()` (`MASHED_VOLTODS_ORIG`), `VolToDsPort()` (the port's `2000*log10` body with the `<= 0.0001f`
  floor), and `MASHED_VOLTODS_LOG` (it logged both forms side by side, and one form is gone).
- `Audio/AudioLinearToLog.cpp`: comment only (VolToDs calls it unconditionally). The .asi body of `0x005aeb90` is unchanged.
- The default behaviour change is the one measured in `verify/audiovoltods_20261024/RESULT_voltods.md` S2: inputs from
  1e-5 to 1e-4 now play at -100 to -80 dB instead of silence, 0.016% of inputs from 1e-4 to 1 drop 1 hundredth of a dB,
  +inf / NaN reach SetVolume as `0x80000000`.

## G: gates on exe `58ff75a6` (default env, `mashed_lock.py run --label D4-AUDIO-VOLTODS-FLIP`, private root)

| Arm | Hash | Expected |
|---|---|---|
| H_DEF step / gates / ptrace | `bacfc747 / 8dd1c004 / 6f9932b6` | same |
| B_DEF_K0 | `f9deb9b9` | same |
| B_DEF_K1 | `d3601fcc` | same |
| B_DEF_K26 | `9da9a26a` | same |

All four arms exit 0, self-exited. No piz collision.

## X: static witness on the build

`MASHED_VOLTODS_ORIG`, `MASHED_VOLTODS_LOG`, `VolToDs v=` and `AudioLinearToLog path` occur 0 times in both `mashed_re.exe`
and `mashed_re_dev.asi`. Prediction held.

## L: lint

`scripts/lint_rva_bodies.py`: 8 findings, all allowlisted, NEW = 0.

## Not done

- No live-audio arm. D4-AUDIO-VOLTODS's K arms already showed the knob-ON form (the same expression) on live audio, and the
  B recipe does not reach the differing band.
- No tracker change: `0x005aeb90` stays C3 and its file mapping is unchanged.
