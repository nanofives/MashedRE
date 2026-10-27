# HB-ARCTIC-FOLDRM RESULT (2026-10-27): dead librw ambient folds deleted, gates unchanged

**Verdict: GREEN.** Pure dead-code delete. Both knobs were default OFF, so the default build
executes the same path. All seven gate hashes equal `re/orchestrator/merge_train.json`.

Branch `orch/HB-ARCTIC-FOLDRM` from `orch/integration` `2d75d00d1`. Source of the decision:
`verify/hbarctic_20261027/RESULT_HB-ARCTIC.md` section 6.

## Gates (default exe, private root)

Exe `mashedmod/build/mashed_re.exe` (build `MASHED_NO_DEPLOY=1`, OK), SHA-256
`0cab24ba10eb3b20073958713c5b7607d837728c6580b4e468228925165e8bdc`. Runner
`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/HB-ARCTIC-FOLDRM_root`
(copy of `D:/MashedWT/HB-ARCTIC_root`, `original/MASHED.exe` SHA-256 equal to the main install's),
wrapped in test-slot and `mashed_lock.py`. All four arms exited 0, `self_exited=True`.

| gate | file | lines | sha8 | ref | match |
|---|---|---|---|---|---|
| G-CTRL | H_DEF.step.csv | 14659 | 5943c5f1 | 5943c5f1 | yes |
| G-CTRL | H_DEF.gates.csv | 56873 | 364350ae | 364350ae | yes |
| G-CTRL | H_DEF.ptrace.log | 28436 | 52052890 | 52052890 | yes |
| B | B_DEF_K0.step.csv | 1459 | 3a03670c | 3a03670c | yes |
| B | B_DEF_K1.step.csv | 1825 | e85ea6fb | e85ea6fb | yes |
| B | B_DEF_K26.step.csv | 1411 | 53778664 | 53778664 | yes |

The gates are physics/AI step dumps, so they cannot see a render change. They confirm the
delete left the sim untouched. Render neutrality rests on the code being unreachable by default
(section 2), not on these hashes.

## What was deleted (`mashedmod/src/mashed_re/LibRw/RwSceneBuild.cpp`)

| item | where it was |
|---|---|
| `AmbientFoldEnabled()` reading `MASHED_LIBRW_AMBFOLD` | `:353-359` |
| `AmbientFoldScopedSea()` reading `MASHED_LIBRW_AMBFOLD_SEA` | `:394-400` |
| `BatchIsWaterClass()` | `:404-406` |
| `ModelIsWaterAsset()` | `:425-441` |
| the stale comment blocks above them (incl. "ARCTIC SEA SHIPS TOO DARK") | `:339-393`, `:402-424` |
| `water_asset` local in `BuildClump` and its `water_asset=%d` field in the `clump[%d]` SLog line | `:457-459`, `:471-476` |
| `prelit_amb`, `fold_this`, the fold gate and the channel-order bake loop | `:613-648` |

Replaced by one 15-line comment above `BuildClump` (no fold exists, the original lights no atomic
lacking rpGEOMETRYLIGHT, lighting callback `0x004ea4e0` gate `0x004ea4e9..0x004ea4f2`, the old
dark figure was measured over a region that was road in the original) and a 2-line note at the
upload site. `prelit_src` still points at `b.prelit`; `FillVertexData` and the `UPLOAD` probe are
unchanged. The `ambient` parameter of `BuildClump` is kept (still printed by the UPLOAD probe).

Line counts: `git diff --shortstat` 1 file changed, 20 insertions, 144 deletions.
`RwSceneBuild.cpp` 960 -> 836 lines.

## 2. Reader sweep

`grep AMBFOLD` over `mashedmod`, `scripts`, `re/tools`, `re/frida`, `re/*.md`, `*.md`, `*.py`,
`*.bat`, `*.json`, `verify/savelink_20261023`: the only code readers were the two functions above.
Other hits left as they are because they are history, not readers:

- `re/HARNESS_BACKLOG.md:167,179` narrate the 2026-08-31 measurements and the scoped-fold prototype.
- `verify/*` scripts and results from August (`verify/city_blackroad/run_water_interaction.ps1`,
  `verify/geomlight_waterfold/run_exposure_probe.ps1`) set `MASHED_LIBRW_AMBFOLD*` and grep the
  `water_asset=` log field. They are `.ps1` (not edited per the task) and are dead harness for a
  deleted knob: on the new exe the env is ignored and the field is absent. `verify/arctic_ref/geomlight_scoped_fold.patch`
  is a stored patch, not a reader.
- U-9064 in `UNCERTAINTIES.md` still cites the folds. Tracker rows change only through
  `re-classify` (already filed as a follow-up in HB-ARCTIC section 6).

## 3. Raw data (not committed), SHA-256

Root `D:/MashedWT/HB-ARCTIC-FOLDRM_data/`, build log `D:/MashedWT/HB-ARCTIC-FOLDRM_build.log`.

```
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 *arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 *arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 *arms/H_DEF.ptrace.log
3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f *arms/B_DEF_K0.step.csv
e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8 *arms/B_DEF_K1.step.csv
5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d *arms/B_DEF_K26.step.csv
3ef998f098581eefeafb9b65684475cf4a6fbcb5b3d3bc1e037eff5aa9dbe152 *run_arms.log
```
