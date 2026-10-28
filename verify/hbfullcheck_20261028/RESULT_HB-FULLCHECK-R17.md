# RESULT HB-FULLCHECK-R17 (2026-10-28)

Verification-only Gate-Full of orch/integration @ 3ebb203dc268429fae18123070a76130983eb426
(MASHED_SNAP_POST default ON, MASHED_TEXANIM default ON). Worktree orch/HB-FULLCHECK-R17 at the same sha,
built with MASHED_NO_DEPLOY=1 mashedmod\build.bat (rc 0, exe sha8 81cbc495). No code change. Reproduces
merge_train.ps1 Gate-Default and Gate-Full (lines 171-244) with merge_train.json refs, every runner wrapped in test-slot.
No merge_train.ps1 process was running before any D2 arm or the 13-track run.

**Verdict: RED. One gate differs: the 13-track table moved 8 rows. Everything else is GREEN.**

| Gate | Result | Ref |
|---|---|---|
| G-CTRL H_DEF | 5943c5f1 / 364350ae / 52052890 | same |
| B K0/K1/K26 | 3a03670c / 9fcdbd3a / 53778664 | same |
| (b) vsum | 3/3 on all 3 arms | 3 |
| (e) ai_speed_env --check (K0) | 3/3 PASS, launch 1425.7 / 2052.5 / 2055.0 | same |
| D2 x3 (solo, 0 reruns, 1628/1633/1630 lines) | 0.1942 / 0.2492 / 1896.74 on all three, all "in" | band ref |
| 13 tracks | DIFFER (8 rows), INSTRUMENT GATES: PASS | verify/hbtexanimrefs_20261027/tracks_score.txt |

Note: K1 v1 fails a standalone (e) check (ft_median_m0 2043.0, ref 2551.6). The gate checks K0 only, so it is not part of the verdict.

## 13-track rows moved (new vs reference, columns as in tracks_score.txt)

Old = reference (hbtexanimrefs_20261027), New = this run. Full diff: tracks_diff.txt.

| Row | Old | New |
|---|---|---|
| 03_Arctic 1051 | 153  33000  93.2% \| 34 34 0 | 155  33192  93.7% \| 33 33 0 |
| 03_Arctic 1091 | 148  32693  92.9% \| 37 37 0 | 150  32885  93.4% \| 36 36 0 |
| 04_Highway 853 | 133  21121  89.6% \| 18 18 0 | 135  21313  90.5% \| 17 17 0 |
| 06_SuperG 743 | 339  24236  91.8% \| 21 21 0 | 341  24428  92.5% \| 20 20 0 |
| 06_SuperG 783 | 339  24239  91.8% \| 22 22 0 | 341  24431  92.5% \| 21 21 0 |
| 07_Roundabout 1091 | 312  26734  83.3% \| 34 34 0 | 314  26926  83.9% \| 34 34 0 |
| 08_Storm 968 | 221  17233  94.3% \| 25 25 0 | 223  17425  95.3% \| 24 24 0 |

(The diff reports 8 changed lines: the 03_Arctic pair, 04_Highway, the 06_SuperG pair, 07_Roundabout, 08_Storm = 7 rows listed above plus the second 03_Arctic frame counted as in the table. Exact count of changed table lines in tracks_diff.txt is 8 line pairs across hunks 7-8, 10, 16-17, 19, 23, i.e. 7 rows.)
Every moved row gains 2 covered cells (192 px in the last count column) and loses 1 from the uncovered cell count. The other
rows (01, 02, 05, 09-13) are identical. All rows CITED.

Likely related to SNAP_POST / TEXANIM defaults versus a reference taken before the SNAP flip. This run does not establish the cause.

## Raw data (not committed), D:/MashedWT/HB-FULLCHECK-R17_data
arms/ sha256: H_DEF.step.csv 5943c5f1c3e2f3f2..., H_DEF.gates.csv 364350aea89110f2..., H_DEF.ptrace.log 520528908f8dc61f...,
B_DEF_K0 3a03670c3991b702..., B_DEF_K1 9fcdbd3a6901025544..., B_DEF_K26 5377866478b33a98...,
D2_DEF_1.motion_diag.log ae9532cc93bea98d..., D2_DEF_2 411c026d596825808f..., D2_DEF_3 acab2155e8831f83...
tracks_score.txt sha256 a3c14c418b1cb6ad8e3e97306bca2ebfbf733f98446b51fec1fb81231b936ae0 (copy committed here).
