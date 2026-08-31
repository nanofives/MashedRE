# Class-scoped ambient fold — prototype + result (2026-08-31, branch race/arctic-cap)

**Result: the class-scoped fold works. It keeps TRAINING's win AND fixes the Arctic sea.**
Recommend the parent apply `../geomlight_scoped_fold.patch` to `race/geomlight`, make the
scoped mode the default, and merge.

## The fix

`race/geomlight` removes the manual prelit ambient fold for ALL non-lit prelit geometry.
That's right for the road (`ROAD.DFF` = `0x2008b`) but wrong for the water
(`LAKE/WATER0x.DFF` = `0x1000f`), which the original renders bright (see
`RESULT.md`). The scope key is `numTexCoordSets` (RW format bits 16-23): road = 2,
water = 1. Prototype (`geomlight_scoped_fold.patch`, 3 files):

- `Track/DffModel.{h,cpp}` — carry the raw geometry format dword through to `DffBatch`
  (`geo_flags`), so the fold site can see the class.
- `LibRw/RwSceneBuild.cpp` — new `MASHED_LIBRW_AMBFOLD_SEA=1` folds ambient into the
  prelit ONLY for water-class batches (`((geo_flags>>16)&0xFF) <= 1`), leaving the road
  unfolded. The two existing modes are untouched: no env = no fold (geomlight default);
  `MASHED_LIBRW_AMBFOLD=1` = fold all (old baseline).

Built in the geomlight worktree; the edit is left uncommitted there for the parent to
review/commit. `mashed_re.exe` was rebuilt with it.

## Arctic sea — luma over the `0x1000f` fold mask (sea-dominant frames)

| frame | sea mask | original | geomON (fold off) | geomOFF (fold all) | **geomSEA (scoped)** |
|---|---|---|---|---|---|
| s8 | 68.7% | 27.9 | 9.1 (Δ18.8) | 30.1 (Δ2.2) | **30.1 (Δ2.2)** |
| s14 | 56.5% | 32.5 | 9.8 (Δ22.7) | 27.7 (Δ4.8) | **27.7 (Δ4.8)** |

The scoped fold restores the sea **identically to fold-all** (matches the original).
Visual: `../sea_search/s8_3way_orig_geomON_geomSEA.png` (orig | geomON | geomSEA) — the
scoped sea is the wet blue-grey dock, matching the original; geomON is near-black.

## TRAINING — whole-frame 8x6 imgdiff vs `verify/parity_race_20260830/orig_race.bmp`

| build | mean abs |
|---|---|
| trON (geomlight, fold off) | 15.45 |
| trOFF (fold all) | 18.47 |
| **trSEA (scoped)** | **15.45** |

`geomON vs geomSEA` on TRAINING = **0.000 mean abs** (byte-identical) — the scoped fold
does NOT touch the road (`0x2008b`), so the 18.47→15.45 win is fully preserved.
(`geomON vs geomOFF` = 4.238, i.e. fold-all is what regressed the road.)

## Verdict

The class-scoped fold is the correct fix: TRAINING road stays at 15.45 (win kept, byte
-identical to geomlight), Arctic sea matches the original (Δ2-5, same as fold-all). Apply
the patch, default `MASHED_LIBRW_AMBFOLD_SEA` on, re-run both diffs to confirm on a clean
build, then merge `race/geomlight`.

## [UNCERTAIN] / scope of validation

- Validated on TRAINING (road) and Arctic (sea). The scope key `numTexCoordSets<=1` was
  verified to select water and exclude road on THESE two tracks. Other tracks with non-lit
  prelit props at 1 texcoord set would also be folded; not yet checked. Re-run the parity
  harness on the other cup tracks (EGYPT/NEUSTEIN + the rest) before shipping broadly.
- Same sub-frame roll-drift / HUD-artifact caveats as `RESULT.md`; the class-masked luma
  and the byte-identical TRAINING check are robust to them.
