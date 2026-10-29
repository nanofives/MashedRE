# RESULT D5-PREGO-CUBE-FLIP (decision L-52, run 21)

**Verdict: DECISION.** The flip is built and committed. G-CTRL, B and D2 come out exactly as L-52 expected.
The 13-track table moves, and L-52 did not expect that. It is not byte-identical to
`verify/hbfullcheck_20261028/tracks_score.txt` (`a3c14c41`) and has 9 RED rows. All 9 are IconCube draws (`mine`, `oil`).
With `MASHED_NO_ICONCUBE=1` the table is `a3c14c41` again. With `MASHED_CAM_REPLAY_GO` the 4 affected tracks have 0 RED.
So the RED rows come from the recipe's replay start, not from the cube draw. Whether to accept the moved
13-track table (and how to cite it) is the user's call.

Expected new refs if L-52 stands: **G-CTRL `5943c5f1/306186d4/52052890` (gates.csv `364350ae` -> `306186d4`),
B `3a03670c/9fcdbd3a/53778664` (unchanged), 13-track table `7770c056` (was `a3c14c41`, 9 RED rows, see 3).**

## 1. Change (commit `f30f7401d`, branch `orch/D5-PREGO-CUBE-FLIP` off `orch/integration` @ `34235b24c`)

- `D3d9Render/TrackRenderer.cpp` `PickupsPreGo()`: default ON. `MASHED_NO_PICKUPS_PREGO=1` (or `MASHED_PICKUPS_PREGO=0`)
  opts out.
- `D3d9Render/PickupIconCube.cpp` `IconCube_Enabled()`: default ON. `MASHED_NO_ICONCUBE=1` (or `MASHED_ICONCUBE=0`) opts
  out. It is still ANDed with `PickupsOrig_Enabled()` (`MASHED_PICKUPS_ORIG`, already default ON through `Core/FaithDefaults.cpp`).
- Same opt-out form as D3-CF-SNAP-FLIP's `SnapPost()`. `MASHED_RFX_EMITTERS` and `MASHED_PICKUPS_PREGO_NO_REFDIST` are not
  touched. Both TUs are standalone-only, so the `.asi` path does not change. No tracker edits.
- Tested exe `mashed_re_flip.exe` (`195e0412`), built from the worktree with `MASHED_NO_DEPLOY=1`. Private root
  `D:/MashedWT/D5-PREGO-CUBE-FLIP_root`, no knob env var set in the shell (`env | grep ^MASHED` empty).

## 2. Gates, no env var (`run_arms.py`, SHA-256 first 8 hex, `gate_hashes.txt`)

| arm set | G-CTRL step / gates / ptrace | B K0 / K1 / K26 |
|---|---|---|
| flipped default (`H_DEF`, `B_DEF_K*`) | 5943c5f1 / **306186d4** / 52052890 | 3a03670c / 9fcdbd3a / 53778664 |
| both opt-outs (`H_NOPGIC`, `B_NOPGIC_K*`) | 5943c5f1 / 364350ae / 52052890 | 3a03670c / 9fcdbd3a / 53778664 |

- G-CTRL equals the expected `5943c5f1/306186d4/52052890`. Only `gates.csv` moves, and its hash is run 20's `H_PG.gates.csv`
  (`306186d44ce7...`) byte for byte. B is unchanged. The opt-out arm gives the current `merge_train.json` refs.
- **D2 x3** (`D2_DEF_{1,2,3}`, solo lock): 0.1942 / 0.2492 / 1896.74, all "in" on all three (`score_d2.txt`). Arm 2
  logged 1530 lines (under 1600), so it was re-run as `D2_DEF_4`: 1630 lines, same scores (`score_d2_rerun.txt`).

## 3. 13 tracks (`verify/rsky_20261016/run13.py`, solo lock, private root, all exit 0, INSTRUMENT GATES: PASS)

| arm | table SHA-256 | vs `hbfullcheck_20261028` (`a3c14c41`) |
|---|---|---|
| flipped default, no knob | `7770c056` (`tracks_score_flip.txt`) | **differs**: 23 rows, 9 RED |
| `MASHED_NO_ICONCUBE` (PREGO still ON) | `a3c14c41` (`tracks_score_noic.txt`) | byte-identical |

- In the flipped table, 23 of 39 rows change (`tracks_diff_vs_hbfullcheck.txt`). The changed rows are Egypt x3,
  Highway 928, Sands x3, SuperG x3, Roundabout x3, Storm x3, Warzone x3, City 836 and Training x3. Matched rows rise by
  0 to 4 per changed row. Neustein, Arctic, Forest and Dump do not change.
- **9 RED rows, all IconCube draws** (`red_rows_flip.txt`):
  - 7 `MOVED mine` / `MOVED oil`: same texture and triangle count, AABB moved by up to 0.0641 (Sands 1084 / 1124,
    SuperG 743, Warzone 624 / 659, Training 884 `mine`, Training 959 `oil`).
  - 2 `EXTRA mine` (B-only, 12 tri): Sands 1159 and SuperG 818. The standalone draws a mine that the original's capture
    does not have on that row.
- **Diagnostic, the 4 tracks with RED rows + `MASHED_CAM_REPLAY_GO`** (run 20 R6's alignment of the sim start to the
  trace's GO, `tracks_score_go.txt`, `f206c578`): all 12 rows CITED, **0 RED**. Every row matches at least as many draws
  as in the hbfullcheck table. Part of that rise may come from the GO alignment itself, because no GO arm without the
  cube was run, so this is not claimed as a cube gain.
- Reading: the 13-track recipe starts its sim at the replay's first row, not at the trace's GO. The cube spin
  angle and pickup collection depend on the sim tick, so on those rows they are offset from the original's. With GO
  alignment the cube draw is cited everywhere. This matches run 19 R5 / run 20 R6, where the cube matched only with
  `MASHED_CAM_REPLAY_GO`. [UNCERTAIN] No per-cube angle or pickup-tick comparison was run on these 4 tracks. The only
  evidence is that the RED rows clear under GO alignment.

## 4. Decision needed (not taken here)

The flip moves the 13-track reference table. Options:
- (a) Keep L-52: accept `7770c056` as the new 13-track ref. Either cite the 9 cube rows (a new `cites_rres.json` entry
  for IconCube spin and collection offset under a non-GO replay) so the table reads CITED, or move the 13-track recipe to
  `MASHED_CAM_REPLAY_GO` and re-baseline every track.
- (b) Flip PREGO only now (13-track table unchanged, G-CTRL gates.csv still moves to `306186d4`), and flip the cube
  later together with the recipe change.

Recommendation: (a) with the recipe kept as is and a cite for the 9 rows. The cube draw itself was shown exact under
GO alignment (run 20 R6, and 0 RED here), and the recipe change would re-baseline 39 rows for a diagnostic knob.
`merge_train.json` not edited (L0 commits the refs).

## 5. Raw data (`D:/MashedWT/D5-PREGO-CUBE-FLIP_data`, not committed)

| file | SHA-256 |
|---|---|
| mashed_re_flip.exe | 195e0412278c83c361dcfb7019d31b877fc8a05067084ca249dbc07ddd51d356 |
| arms/H_DEF.gates.csv | 306186d44ce7193b93b53a12e1658b871e90bb276480c2add7d8c9f7c08e6a2e |
| arms/D2_DEF_1.motion_diag.log | c9d721d7ac717523f69558983ca147fd932ca23cd6570bb0b14cfd2edde6e37c |
| arms/D2_DEF_2.motion_diag.log | c608633ff6dacbfc2639a4cac2b2bcf2e612a1806647c14a48fc058373746047 |
| arms/D2_DEF_3.motion_diag.log | c381fdd789dfc782ac61c8b23821970b39e61fed94616c44f5537aa869d6bfa6 |
| arms/D2_DEF_4.motion_diag.log | acab2155e8831f83b223b08e59e1e65dafad8c03f454d8cf22c6192b093bbd07 |
| tracks_score.txt (dirs in tracks/) | 7770c056966221130b68dc18e5b6b7bea3f3afc2f475aedb371b3c55a54a6b4d |
| tracks_score_noic.txt (tracks_noic/) | a3c14c418b1cb6ad8e3e97306bca2ebfbf733f98446b51fec1fb81231b936ae0 |
| tracks_score_go.txt (tracks_go/) | f206c5786c560421235e51faa89065a1ca20626a0e99ee8199b4daf026fbeeb3 |
| tracks/13_Training/diff_00884.txt | a2d9b3bc7bb45808f7d99db016fe2d70ba17c51f5e5aba44da2dc5c3513fa930 |
| tracks/05_Sands/diff_01159.txt | 28310141fb262232c2a79955721b4066b6b40a1e850f07c57af6793ae0f5bb33 |
| tracks/06_SuperG/diff_00818.txt | 77c504cb6ed7e4b5ed8868408c99e450e1f13d8eb60cc97c0fa7589c01bc0176 |

The remaining per-arm hashes are in `gate_hashes.txt`. Run logs: `RUNLOG_flip.txt`, `RUNLOG_ctl.txt` and `RUNLOG_go.txt` in the data
dir, with extracts committed. Scripts: `run_flip.sh`, `run_ctl.sh`, `run_go.sh`.
