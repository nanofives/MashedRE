# PREREG addendum 1, D4-WSB-C4-SOLV-TRK1: re-run on the L-56 flipped build

Committed alone, before any re-run arm. Read-out rules of `PREREG_D4-WSB-C4-SOLV-TRK1.md` are unchanged.

- Branch merged `orch/integration` @ `4ee1823e1` (L-56 flip, `MASHED_PICKUPS_PREGO` + `MASHED_ICONCUBE` default ON)
  with `git -c merge.autoStash=false merge`, merge commit `c65466382`. `mashedmod/` changed (22 files), so the prior
  leg B evidence (tied to `.asi` `fbdef276`) no longer covers this source. All legs re-run.
- Rebuilt in the worktree, `MASHED_NO_DEPLOY=1`: `mashed_re.exe`
  `409ac992fb9b7465e87c04314c96f720f52dd675fab4aaed5a30f10566cdf371`, `mashed_re_dev.asi`
  `ee83d072235ef1aecf3863da86ef59ece224dbf5ec5d9ba38327a19f185ac5dd`. Private root
  `D:/MashedWT/D4-WSB-C4-SOLV-TRK1_root/original/mashed_re_dev.asi` replaced by `ee83d072` (hash checked after the copy).
- Tags so the prior run's raw data and summary rows stay: leg B `SHADOW_TAG=heapflip`, arm N `SHADOW_TAG=needpagesflip`,
  racediff `C4_TAG=_flip`. Leg B again runs on all 8 rows. Arm N only for a row whose `heapflip` read-out is
  CLEAN-NO-PAGES.
- Default gate references now (`re/orchestrator/merge_train.json` @ `4ee1823e1`): G-CTRL `5943c5f1/306186d4/52052890`,
  B `3a03670c/9fcdbd3a/53778664`. The PREREG's `364350ae` is superseded by the flip (`306186d4`).
