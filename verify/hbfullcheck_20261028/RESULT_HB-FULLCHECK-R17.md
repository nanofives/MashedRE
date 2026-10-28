# RESULT HB-FULLCHECK-R17 (2026-10-28)

Verification-only Gate-Full of orch/integration @ 3ebb203dc268429fae18123070a76130983eb426
(MASHED_SNAP_POST default ON, MASHED_TEXANIM default ON). Worktree orch/HB-FULLCHECK-R17 at the same sha,
built with MASHED_NO_DEPLOY=1 mashedmod\build.bat (rc 0, exe sha8 81cbc495). No code change. Reproduces
merge_train.ps1 Gate-Default and Gate-Full (lines 171-244) with merge_train.json refs, every runner wrapped in test-slot.
No merge_train.ps1 process was running before any D2 arm or the 13-track run.

**Verdict: RED. One gate differs: the 13-track table moved 7 rows. Everything else is GREEN.**

| Gate | Result | Ref |
|---|---|---|
| G-CTRL H_DEF | 5943c5f1 / 364350ae / 52052890 | same |
| B K0/K1/K26 | 3a03670c / 9fcdbd3a / 53778664 | same |
| (b) vsum | 3/3 on all 3 arms | 3 |
| (e) ai_speed_env --check (K0) | 3/3 PASS, launch 1425.7 / 2052.5 / 2055.0 | same |
| D2 x3 (solo, 0 reruns, 1628/1633/1630 lines) | 0.1942 / 0.2492 / 1896.74 on all three, all "in" | band ref |
| 13 tracks | DIFFER (7 rows), INSTRUMENT GATES: PASS | verify/hbtexanimrefs_20261027/tracks_score.txt |

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

Every moved row gains 2 in the third numeric column and 192 in the fifth, and the sixth percentage rises; the cell count after the bar drops by 1 (03_Arctic, 04_Highway, 06_SuperG, 08_Storm) or stays (07_Roundabout). The other
rows (01, 02, 05, 09-13) are identical. All rows CITED.

Likely related to SNAP_POST / TEXANIM defaults versus a reference taken before the SNAP flip. This run does not establish the cause.

## Raw data (not committed), D:/MashedWT/HB-FULLCHECK-R17_data
arms/ sha256: H_DEF.step.csv 5943c5f1c3e2f3f2..., H_DEF.gates.csv 364350aea89110f2..., H_DEF.ptrace.log 520528908f8dc61f...,
B_DEF_K0 3a03670c3991b702..., B_DEF_K1 9fcdbd3a6901025544..., B_DEF_K26 5377866478b33a98...,
D2_DEF_1.motion_diag.log ae9532cc93bea98d..., D2_DEF_2 411c026d596825808f..., D2_DEF_3 acab2155e8831f83...
tracks_score.txt sha256 a3c14c418b1cb6ad8e3e97306bca2ebfbf733f98446b51fec1fb81231b936ae0 (copy committed here).

## Attribution run 1: MASHED_NO_SNAP_POST=1 (same exe 81cbc495, same procedure as the 13-track run)

Solo lock via mashed_lock.py solo-begin, test-slot, no merge_train.ps1 live, MASHED_NO_SNAP_POST=1 exported.
Raw out dir D:/MashedWT/HB-FULLCHECK-R17_data/tracks_nosnap_nosnap (name doubled by a script substitution), scored file
tracks_score_nosnap.txt (committed here). All 13 tracks exit 0, 1 attempt each.

- `^\d\d_` rows vs verify/hbtexanimrefs_20261027/tracks_score.txt: **identical, 0 differing lines**.
- INSTRUMENT GATES: PASS.
- **NO_SNAP_POST=1 identical: yes.** Item 3 (build and run at 852369807): not run, the condition (NO_SNAP_POST=1 does not restore) did not occur.

## Static: can SNAP_POST run on the 13-track frames

Source: mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp.
- 1659-1663 `SnapPost()`: true unless MASHED_NO_SNAP_POST is set non-zero or MASHED_SNAP_POST=0 (default ON since 0de5e0171).
- 5722 `const bool phys = Vehicle::VehiclePhysics_Enabled();` 5758 `snapPost = phys && SnapPost()`; 5759 `snapKeep = snapPost && v < 16 && s_outer[v].have`.
- The gate is not stuck-car-only. `snapKeep` (any car the outer driver stepped this frame) removes the dead-state skip (5763) and the spin skip (5767) and, with snapPost, the pre-commit `stuckReloc` at 5811 (`if (!snapPost) stuckReloc();`). The post-commit `stuckReloc` at 5891 runs for every non-skipped opponent. The relocation body itself only acts on a stuck car, but the loop-head and ordering changes apply to every opponent loop iteration while physics is enabled.
- run13.py (header lines 1-10) runs one deterministic round with no opt-in knobs and none of MASHED_SNAP_POST / MASHED_NO_SNAP_POST set, so the default (ON) applies. The measured result above (rows return under NO_SNAP_POST=1) shows the path was reached on at least the 5 moved tracks. Which frame it fires on was not traced (MASHED_SNAP_LOG=1 not run).

## Provenance of the reference (verify/hbtexanimrefs_20261027/tracks_score.txt)

- Scored on orch/HB-TEXANIM-REFS flipped exe `1a4c2e82` (RESULT F2/F3), after merging orch/integration 2232ac2e5 (Arctic sky flip) as f2a4db0ec. Commit da0572742 is the flip commit on that branch.
- `git merge-base da0572742 3ebb203dc` = da0572742 itself (da0572742 is an ancestor of 3ebb203dc; 852369807 is also an ancestor).
- mashedmod/src commits in 3ebb203dc not in da0572742: f805d071c (D3-CF-SNAP: SNAP_LOG + SNAP_POST, default OFF), 78ac9d862 (merge of D3-CF-SNAP), af86770e4 (merge of HB-TEXANIM-REFS), 0de5e0171 (D3-CF-SNAP-FLIP, SNAP_POST default ON). mashedmod/src commits in da0572742 not in 3ebb203dc: none.
- Gate refs in the RESULT at that exe: G-CTRL 5943c5f1/364350ae/52052890, B 3a03670c/e85ea6fb/53778664 (pre-SNAP-FLIP K1).
- Arctic: verify/hbarcticskyflip_20261027/tracks_score.txt (the table before the TEXANIM flip) has 03_Arctic 1051 matched 153 / m.tri 33000 and 1091 148 / 32693, the same as the reference. This run's default row is 155 / 33192 and 150 / 32885, and the NO_SNAP_POST=1 run gives 153 / 33000 and 148 / 32693.
