# PREREG D4-WSB-C4-SOLV-RGN

Committed alone, before any arm. Branch `orch/D4-WSB-C4-SOLV-RGN` off `orch/integration` @ `0d1285dc7`. No source
change on this branch. Build and private root are shared with `D4-WSB-STACKONLY-C3` (same commit, no source change on
either branch): `mashed_re.exe` `779bc9c3...c447`, `mashed_re_dev.asi` `c6a264a3...3566`, root
`D:/MashedWT/D4-WSB-STACKONLY-C3_root`. Raw data of this step: `D:/MashedWT/D4-WSB-C4-SOLV-RGN_data`.

## Rows (scope: `verify/wsbc4scope_20261027/RESULT_D4-WSB-C4-SCOPE.md`, group SOLV-RGN)

| row | wrapper | out-memory leg |
|---|---|---|
| `0x0056bce0` | RunRegion param_1, 0x10 | R: region shadow + SCOPE region-coverage check (every pointer store in region, slot spills have no reader) |
| `0x0056cf90` | RunRegion param_1, 0x60 | R |
| `0x0056ed60` | RunRegion param_1, 0x2c | R |
| `0x0056fad0` | RunRegion param_1, 0x3c | R |
| `0x0056bb30` | Run | A: no store outside its frame (SCOPE scan), only the call to `0x0056ba30` |

## Arms (one hook installed alone per boot, under `mashed_lock.py` and the test slot)

1. **Leg R (4 region rows)**: `run_shadow6.py` (copy of SOLV-WS `run_shadow4.py`, data path / label changed), census
   (track 0, mode 10, 4 cars, `HOLD=60`), `MASHED_SHADOW_AB=1 MASHED_SHADOW_PRIVATE=1 MASHED_SHADOW_STW=1`,
   `SAMPLES_DONE=24`.
2. **Racediff (5 rows)**: `verify/wsbsolvws_20261027/c4_racediff_v7.py` unchanged, via `run_bvh6.py` (copy of
   `run_bvh4.py`, data path / label changed), Quick Battle window A4 300..1200, OFF `__none__` / ON the row alone. A row
   Quick Battle does not observe (0 calls both sides) gets one census run (`C4_SCENARIO=census`, `C4_HOLD=400`,
   `C4_WINDOW="1 3000"`).

Pre-launch audio check and U-9677 re-run rule as in the standing rules.

## Read-out per row (fixed now)

- Leg R PASS: n >= 1 counted samples (target 24), ndiff 0 on every sample, `PATCHBYTE ... installed=e9 ... A/B-IS-REAL`,
  no NOISY / OTHER-EXC line for the row.
- Racediff PASS: `C4-CLEAN` (installed `0xE9`, same signature set both sides). A HOLD whose only difference is a
  pointer label that moves with heap placement (the SOLV-WS section 3 LAYOUT case) is reported as such and the row
  stays C3. It is not re-labelled clean.
- **C3 -> C4** through re-classify when its out-memory leg (R or A) and its racediff both PASS. Otherwise stays C3.

Default gates on this build: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, must match
`re/orchestrator/merge_train.json` refs (one run serves both steps: same exe).
