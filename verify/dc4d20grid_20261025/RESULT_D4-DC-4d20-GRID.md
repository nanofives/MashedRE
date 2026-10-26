# RESULT D4-DC-4d20-GRID: the grid spawn basis through the one RwMatrixRotate body

**Verdict: GREEN, knob default OFF. With the knob ON, every gate hash and every D2 score equals the default.** All
PREREG gates held (`PREREG_D4-DC-4d20-GRID.md`, commit `caaeaae5f`, committed alone before any arm). One decision row is
filed: flip `MASHED_GRID_ROTATE_RW` ON and delete the inline form.

## What changed (commit `b84546e2f`, base `orch/integration` @ `4ff9c8468`)
- `Race/RoundGrid.cpp` `GridDir_PortYaw`: with `MASHED_GRID_ROTATE_RW=1` (default OFF) the at row handed to
  `Vehicle::BodyOrient_NoteExactBasis` comes from the shared `RwMatrixRotate` (`Math/RwMatrixRotate.cpp`, 0x004c4d20)
  on a scratch matrix, axis (0, 1, 0), the law's degrees, mode 0, read as m[8] / m[10]. The OFF path keeps the double
  std::sin / std::cos form, now marked port-only.
- Why mode 0 matches the original's mode 1: `FUN_004704c0` rotates matrices that `FUN_004c51a0` mode 0
  (`0x004705fe` / `0x0047060a`, ebp = 0 from `0x004704fe`) just rebuilt. That replace arm (`0x004c5285..0x004c52ac`)
  writes the identity 3x3 and ORs flags `0x20003`. `FUN_004c4a50`'s mode-1 arm on an identity-flagged matrix copies R
  (`Math/RwMatrixRotateInner.cpp`, the `kIdentityMask` branch). Disassembly read from `original/MASHED.exe.unpatched`.
- `MASHED_GRID_ROTATE_TRACE=<file>` (diagnostic, changes no value): per call, the degree bits and both at rows.
- `verify/savelink_20261023/run_arms.py`: stacks `GR`, `GT`, `GRT`.
- Dual-copy scanner (build.bat rva-lint): 1 finding (`CROSS-TARGET 004177b0`), allowlisted 1, NEW=0. The allowlist did
  not grow (1 key, as at base). The inline form had no RVA header, so the scanner never counted it.

## G1: default gates (refs from `re/orchestrator/merge_train.json`)
| arm | step | gates | ptrace | ref |
|---|---|---|---|---|
| H_DEF | 5943c5f1 | b493262b | 09551e6a | 5943c5f1/b493262b/09551e6a = |

| arm | K0 | K1 | K26 | ref |
|---|---|---|---|---|
| B_DEF | 3a03670c | 2516d903 | 53778664 | 3a03670c/2516d903/53778664 = |

(b) 9/9 car regimes PASS, (e) 3/3 PASS, launch 1425.7 / 2052.5 / 2055.0 unchanged (`vsum_b.txt`, `e_check.txt`).
13 tracks (`run13.py`, `cites_rres.json`): 39 rows identical to `verify/rpct_20261022/tracks_flip_score.txt`,
INSTRUMENT GATES PASS (`tracks_score.txt`). PASS.

## G-RUN and G-BITS (`gridrot_lines.txt`)
| arm | calls | deg bits | inline at.x / at.z | rw at.x / at.z | verdict |
|---|---|---|---|---|---|
| H_GT (knob OFF) | 2 | 43340000 (180.0) | b3bbbd2e / bf800000 | b3bbbd2e / bf800000 | SAME, on=0 |
| D2_GT_1 (knob OFF) | 2 | 43340000 (180.0) | b3bbbd2e / bf800000 | b3bbbd2e / bf800000 | SAME, on=0 |
| D2_GRT_1 (knob ON) | 2 | 43340000 (180.0) | b3bbbd2e / bf800000 | b3bbbd2e / bf800000 | SAME, on=1 |

G-RUN PASS: G-CTRL and D2 both execute the grid basis, and the ON arm reads `on=1`. G-BITS: 6 of 6 lines SAME.
at.x = `0xb3bbbd2e` (-8.74227766e-08), at.z = `0xbf800000` (-1.0): the shared body and the inline form agree bit for bit
at 180.0 deg. Coverage limit: every exercised call in these scenarios is the single input 180.0 deg. Other grid
angles were not exercised here, so bit equality at other degrees is not measured.

## G2: knob ON
| arm | hashes |
|---|---|
| H_GR | 5943c5f1 / b493262b / 09551e6a |
| B_GR K0/K1/K26 | 3a03670c / 2516d903 / 53778664 |

Equal to the refs, as the PREREG requires when every G-BITS line is SAME. (b) 9/9 and (e) 3/3 with the same launch
values.

## G3: D2, solo (`score_d2_v2.py`, whole-log window, band.json; `score_d2.txt`)
| run | lines | slip1500 | slip2000 | drvmed | H1 | motion_diag sha8 |
|---|---|---|---|---|---|---|
| D2_DEF_1 | 1628 | 0.1900 | 0.2550 | 1964.25 | 398/400, 1372.1 | 9f351ebc |
| D2_DEF_2 | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 | aacb6a3c |
| D2_DEF_3 | 1630 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 | 1c1d5b1b |
| D2_GR_1 | 1628 | 0.1900 | 0.2550 | 1964.25 | 398/400, 1372.1 | 9f351ebc |
| D2_GR_2 | 1629 | 0.1900 | 0.2550 | 1964.25 | 398/400, 1372.1 | 5c755621 |
| D2_GR_3 | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 | aacb6a3c |
| D2_GT_1 | 1637 | 0.1900 | 0.2548 | 1970.58 | 398/400, 1372.1 | 0ecd557e |
| D2_GRT_1 | 1629 | 0.1900 | 0.2550 | 1964.25 | 398/400, 1372.1 | 5c755621 |

All 8 in band, all at or above 1600 lines. D2_GR_1 = D2_DEF_1 and D2_GR_3 = D2_DEF_2 byte for byte. D2_GR_2 =
D2_GRT_1. The spread across runs (drvmed 1964.25 to 1970.58) is the D2 wall-clock run-to-run spread, present inside
the DEF trio itself (3 distinct logs). PASS.

## Decision filed
`MASHED_GRID_ROTATE_RW` default ON (add `GRID_ROTATE_RW` to the FAITH set in `Core/FaithDefaults.cpp`), then delete the
double std::sin / std::cos at-row form in `GridDir_PortYaw`? Expected with it ON: G-CTRL `5943c5f1/b493262b/09551e6a`,
B `3a03670c/2516d903/53778664`, D2 in band, 13 tracks identical (all measured here). Options: A, flip ON. B, flip ON and
delete the inline form and the knob in the same change. C, keep OFF. Recommendation: B. The spawn basis then has one
source for 0x004c4d20's arithmetic, and the measured cost is nil on every gate. Caveat for the record: equality is
measured at 180.0 deg only.

## Tracker follow-up (not done here: trackers only via re-classify)
None required. 0x004c4d20 keeps its one scanner-visible body; the inline form was never a tracked row.

## Hygiene
- Runs: `mashed_lock.py run --solo --label D4-DC-4d20-GRID`, private roots `D:/MashedWT/D4-DC-4d20-GRID_root` and
  `_root13`. Only PIDs this lane spawned ran. Build with `MASHED_NO_DEPLOY=1`, `original\mashed_re_dev.asi` not deployed.
  Nothing ran from the main checkout.
- A first robocopy from Git Bash wrote one stray copy of `original\` to `D:\MashedWT$r\original` (64 entries, created
  2026-10-25 22:11:06). The removal was blocked by the session's protected-path guard. It is safe to delete by hand.
  It holds no evidence.
- `run_arms.py`'s new `GT` line first carried a single backslash before `@NAME@` (SyntaxWarning, same string value).
  Fixed to `\\` during the run.

## Raw files (not committed, `D:/MashedWT/D4-DC-4d20-GRID_data`)
| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms/H_DEF.gates.csv | b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f |
| arms/H_DEF.ptrace.log | 09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 |
| arms/H_GR.step.csv, arms/H_GT.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms/H_GR.gates.csv, arms/H_GT.gates.csv | b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f |
| arms/H_GR.ptrace.log, arms/H_GT.ptrace.log | 09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 |
| arms/B_DEF_K0.step.csv, arms/B_GR_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| arms/B_DEF_K1.step.csv, arms/B_GR_K1.step.csv | 2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6 |
| arms/B_DEF_K26.step.csv, arms/B_GR_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| arms/D2_DEF_1.motion_diag.log, arms/D2_GR_1.motion_diag.log | 9f351ebc3c62f9425fb19c0424c33fb8d961cae5dc0df2357d9eed339cd78f2c |
| arms/D2_DEF_2.motion_diag.log, arms/D2_GR_3.motion_diag.log | aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314 |
| arms/D2_DEF_3.motion_diag.log | 1c1d5b1bd9faccbbd7bf35fa851a779896ac41b0f6213debd07edbb6f0b242b6 |
| arms/D2_GR_2.motion_diag.log, arms/D2_GRT_1.motion_diag.log | 5c7556218d1c8a050e9fa529bcce3a94e96208a510b97b2b804e8e3b0f5efdef |
| arms/D2_GT_1.motion_diag.log | 0ecd557eaee1ea08f2498af67fd9dbef3ba1965a24fe944b7abe611da42faf45 |
| arms/H_GT.gridrot.txt, arms/D2_GT_1.gridrot.txt | 9a47edf405949f9d8e8bef90c81de0cb8f1a32d7015e0734438efd6d697d2388 |
| arms/D2_GRT_1.gridrot.txt | 2794a020a4620972dd2cb0721f8a827aa901341e33f6dc99abd1763f02908cb6 |
| mashed_re.exe (this branch, b84546e2f) | 41aa8cccebb0ed6f708b7667affb93eb6a438b775eb8e095e6492fa242135cb1 |

Committed here: this RESULT, the PREREG, `score_d2.txt`, `tracks_score.txt`, `vsum_b.txt`, `e_check.txt`,
`gridrot_lines.txt`, `RUNLOG.txt` (arm hash list), the driver `run_all.sh`.
