# RESULT D3-CF-U9779-RECON: the 8 RED rows are the merge train's cites file, not the flip (2026-10-25)

Verdict: **the U9779 flip is clean on the 13 tracks.** All 8 rows are rows that `cites_rres.json` cites and
`cites_merged.json` does not. They are RED on the **pre-flip default** too when scored with `cites_merged.json`. The merge
train scores with `cites_merged.json` but compares against a reference table that was scored with `cites_rres.json`, so every
full-set train bounces on these 8 rows, whatever branch it carries. No game run, no build, no default changed. Offline
re-scores only (`run13.py score`, the scorer is identical on `orch/integration` and `orch/D3-CF-U9779`).

Branch `orch/D3-CF-U9779-RECON` from `orch/integration` `b5c84688e`. Evidence under `verify/d3cf_recon_20261025/data/`.
Re-score helper `rescore.py` (copies a `run13.py run` out dir without BMPs to a scratch dir, then runs `run13.py score`).

## 1. What differs between the two tables

The merge train's `tracks_score.txt` (train `20261024_220701`, merged exe `703ce7c8`) and the worker's
`verify/u9779_20261024/data/t13_flip/scores_diff.txt` (flipped exe `5a51764d`) differ **only** in the `cited` / `red` / verdict
columns of 8 rows. Every measured column (orig draws, matched, orig tri, matched tri, rows) is equal on all 39 rows. On each of
the 8 rows exactly one row moves from cited to RED.

Cites files: `verify/rres_20261016/cites_rres.json` (44 cites) against `verify/rskyverify_20261016/cites_merged.json` (42):

| only in cites_rres | added by | covers |
|---|---|---|
| `C31-SPLIT-PAIR` | rres `3bab54f09` (2026-10-16) | Egypt SPLIT 6 tri, A 3 draws / B 1 draw (instrument pairing artifact) |
| `C30-TRAIN-ROOF-LEAD` | txdscope -> rres `269ec0a94` (2026-10-16) | Dump train roof MOVED by up to 0.0571 (replaces the old EXTRA `trainroof` row) |
| `C27b-CAR-WHEEL-ROT` | wheelcite `e9313f080` (2026-10-20) | 96-tri `Advantage` wheel draws MOVED, positions within 0.001, local rotation only |

`cites_merged.json` has `C30-TXD-SCOPE` (EXTRA `trainroof`), which rres replaced by `C30-TRAIN-ROOF-LEAD` once the roof drew.
`C10-RUNTIME-TEX` differs only in its `note` text. 15 lane RESULTs since 2026-10-16 score with `cites_rres.json`, among them
dcmix (`verify/dcmix_20261022/RESULT_dcmix.md:53`), whose table rpct's `tracks_flip_score.txt` equals. U9779 section 7.1 / 7.3
also used `cites_rres.json`. `cites_merged.json` was last used by rskyverify / pontoon / txdscope (2026-10-16).

The config: `re/orchestrator/merge_train.json` `full.run13_cites` = `cites_merged.json`, `full.run13_reference` =
`verify/rpct_20261022/tracks_flip_score.txt` (DECISIONS P0-4 A, picked from "the newest lane that ran it", marked unconfirmed
in `_run13_uncertain`).

## 2. Cross re-score (offline, same scorer, `data/rs_*.txt`)

| run (exe) | cites | RED rows | equal to the reference by the train's test (`^\d\d_` lines) |
|---|---|---|---|
| merge train `20261024_220701` (merged `703ce7c8`, flip ON) | cites_merged | 8 | no (as reported) |
| same run | **cites_rres** | **0** | **identical** |
| worker `D3-CF-U9779_data\t13_flip` (`5a51764d`, flip ON) | cites_rres | 0 | identical (U9779 7.3) |
| same run | cites_merged | **8** (same 8 rows) | no, and equal to the train's table line for line |
| dcmix `t13_final` (**pre-flip default**) | cites_merged | **8** (same 8 rows) | no |
| same run | cites_rres | 0 | identical |

The measured columns (`cut -d'|' -f1`) of all four re-scores hash to the same md5 `1edf88b6a45a10446805d25bdedc3c38`: the
pre-flip default, the worker's flipped exe and the merged exe draw the same 39 frames.

## 3. Per row

| row | RED row under cites_merged | cite in cites_rres | on the pre-flip default with cites_merged | verdict |
|---|---|---|---|---|
| 01_Egypt 740 | SPLIT 6 tri, A 3 / B 1 | C31-SPLIT-PAIR | RED | not a flip effect, not a defect: cites file |
| 01_Egypt 780 | same | C31-SPLIT-PAIR | RED | same |
| 01_Egypt 815 | same | C31-SPLIT-PAIR | RED | same |
| 03_Arctic 1051 | MOVED `Advantage` 2 draws, up to 0.0229 | C27b-CAR-WHEEL-ROT | RED | same |
| 10_Dump 735 | MOVED untextured 1 draw, up to 0.0571 | C30-TRAIN-ROOF-LEAD | RED | same |
| 10_Dump 775 | same | C30-TRAIN-ROOF-LEAD | RED | same |
| 10_Dump 810 | same | C30-TRAIN-ROOF-LEAD | RED | same |
| 13_Training 924 | MOVED `Advantage` 2 draws, up to 0.0256 | C27b-CAR-WHEEL-ROT | RED | same |

Row lines from both scorings: `data/red_rows.txt`.

## 4. Why the worker saw 0 and the train saw 8

The worker scored with `cites_rres.json`, the file the reference table was built with. The train scored with
`cites_merged.json`, an older file that lacks the 3 cites above. Memory `sim-trajectory-is-code-layout-sensitive` does **not**
apply here: the measured columns are bit-equal across three exes (pre-flip default, `5a51764d`, `703ce7c8`), so no frame moved
with code layout.

Side observation (not a gate input): the free-text `first k.. pt.. tri=.. bb=[..]` detail on some A-only rows (for example
`C08-STARTDISP OrangeDisplay` on Neustein 1094) changes between two scorings of the **same** run directory (`tri=10` in the
train's file, `tri=42` in this re-score). The scorer picks that example draw non-deterministically. The table columns and the
cite verdicts do not depend on it.

## 5. What the U9779 flip must reproduce on the merged tree

Already reproduced by train `20261024_220701` (merged exe `703ce7c8`), equal to U9779 section 7.3:

| gate | expected |
|---|---|
| G-CTRL `H_DEF` | `ef5f2e10 / 57f7ff83 / b4f4b55a` |
| B `B_DEF_K0 / K1 / K26` | `c37d4c0c / 0a8f9806 / 7013671d` |
| (b) / (e) | 3/3 / 3/3, launch 1425.7 / 2052.5 / 2055.0 |
| 13 tracks | with `cites_rres.json`: table identical to `verify/rpct_20261022/tracks_flip_score.txt`, INSTRUMENT GATES PASS (shown on the train's own run, section 2) |
| D2 x3 solo | in band. Train arm `D2_DEF_2` had 1485 motion_diag lines (slip2000 0.2588 OUT on that arm): re-run under the rerun rule, not RED. `D2_DEF_1` / `_3` in band (drvmed 1970.58 / 1966.66) |

## 6. Decision (filed on this step)

Change `re/orchestrator/merge_train.json` `full.run13_cites` to `verify/rres_20261016/cites_rres.json` (re-opens P0-4 A), then
re-run the train on `orch/D3-CF-U9779` with the flipped G-CTRL / B refs. Expected: 13 tracks identical, all other gates as
section 5, D2 3/3 once the short arm is re-run. Alternative: keep `cites_merged.json` and rebuild the reference table with it
(8 RED rows in the reference, so every lane's cites would disagree with the train). Recommendation: the first, it matches the
15 lanes and the reference table already in use. One-line patch:

```
-    "run13_cites": "verify/rskyverify_20261016/cites_merged.json",
+    "run13_cites": "verify/rres_20261016/cites_rres.json",
```
