# RESULT D4-DC-4d20-FLIP: the grid spawn at row always comes from the shared RwMatrixRotate body

**Verdict: GREEN. Every gate hash equals the merge_train.json refs.** This is the L-19 = B change
(`log/orch/DECISIONS.md` L-19, Mariano 2026-10-26): `MASHED_GRID_ROTATE_RW` is gone, the at row is always the
shared body's, and the inline double sin / cos at-row form is deleted. The decision also asked for the at-row bit
check at a second grid angle before merge. It ran at 11 distinct angles, and every line is SAME.

No PREREG: this is the decided flip of a measured knob (evidence `verify/dc4d20grid_20261025/RESULT_D4-DC-4d20-GRID.md`),
not a new behaviour knob.

## What changed (branch `orch/D4-DC-4d20-FLIP`, base `orch/integration` @ `c2bf772f8`)
- `mashedmod/src/mashed_re/Race/RoundGrid.cpp` `GridDir_PortYaw`: deleted `GridRotateRw()` (the knob) and the inline
  at row (`atX = s`, `atZ = 1 - (float)(1 - cos r)`). The at row given to `Vehicle::BodyOrient_NoteExactBasis` is now
  always `GridRotateAt(deg)` = `RwMatrixRotate` (`Math/RwMatrixRotate.cpp`, 0x004c4d20) on a scratch matrix, axis
  (0, 1, 0), mode 0, m[8] / m[10]. The mode-0 equivalence comment (0x004705fe / 0x0047060a, 0x004c5285..) is kept.
- `MASHED_GRID_ROTATE_TRACE` stays as a diagnostic. It now prints `deg` and the `rw` at row only.
- `verify/savelink_20261023/run_arms.py`: `STACK['GR'] = []` (knob deleted, same form as the V3 / NOOL / CR rows). `GT`
  is kept.
- Not in scope and left as is: the yaw still comes from `atan2(cos r, sin r)` with double std::sin / std::cos
  (`GridDir_PortYaw`, lines `s` / `c` / `yaw`). L-19 covers the at-row form only.
- rva-lint: 1 finding (`CROSS-TARGET`), allowlisted 1, NEW=0. Allowlist unchanged (1 key).

## Second-angle at-row bit check (`gridrot_lines.txt`)
Base exe (the pre-flip tree at `c2bf772f8`, both forms present, knob OFF) with `MASHED_GRID_ROTATE_TRACE` on all 13 tracks
(`run13.py run --tracks <t>`, one trace file per track) plus the Warzone arm `W_GT`. Then the same runs on the flip exe.

| track | deg bits | deg | inline = rw (base) | flip rw = base inline |
|---|---|---|---|---|
| 01 Egypt | 42b40031 | 90.000374 | SAME | yes |
| 02 Neustein | 430a6fd6 | 138.436859 | SAME | yes |
| 03 Arctic | 3d220e94 | 0.039565 | SAME | yes |
| 04 Highway | 42b3faef | 89.990105 | SAME | yes |
| 05 Sands | 433540bd | 181.252884 | SAME | yes |
| 06 SuperG | 4333ab90 | 179.670166 | SAME | yes |
| 07 Roundabout | 43b40000 | 360.0 | SAME | yes |
| 08 Storm | 43b40000 | 360.0 | SAME | yes |
| 09 Forest | 43340000 | 180.0 | SAME | yes |
| 10 Dump | 402e1be8 | 2.720453 | SAME | yes |
| 11 Warzone | 43871fb1 | 270.247589 | SAME | yes |
| 12 City | 42b1d4a1 | 88.915291 | SAME | yes |
| 13 Training | 43340000 | 180.0 | SAME | yes |
| W_GT (Warzone arm) | 43871fb1 | 270.247589 | SAME | yes |

28 lines (2 calls per run), 11 distinct angles. In every line, the base inline row, the base RW row and the flip RW row
are equal bit for bit. H_GT on the flip exe: `43340000` -> `b3bbbd2e/bf800000`, the GRID lane's value. Coverage limit:
these are the angles the default scenarios execute. Angles that no track's start node produces were not exercised.

## Gates (refs from `re/orchestrator/merge_train.json`)
| arm | hashes | ref |
|---|---|---|
| H_DEF (G-CTRL) | 5943c5f1 / b493262b / 09551e6a | 5943c5f1 / b493262b / 09551e6a = |
| B_DEF K0 / K1 / K26 | 3a03670c / 2516d903 / 53778664 | 3a03670c / 2516d903 / 53778664 = |
| H_GT (trace on) | 5943c5f1 / b493262b / 09551e6a | = |
| W_GT step, base vs flip | d29c7145 / d29c7145 | = |

13 tracks (flip exe, `run13.py score --cite verify/rres_20261016/cites_rres.json`, `tracks_score.txt`): byte-identical
to `verify/dc4d20grid_20261025/tracks_score.txt`. The 39 track rows are identical to
`verify/rpct_20261022/tracks_flip_score.txt`. INSTRUMENT GATES PASS.

## D2 (solo, `score_d2_v2.py port`, whole-log window, d2rebase band; `score_d2.txt`)
| run | lines | slip1500 | slip2000 | drvmed | H1 | sha8 |
|---|---|---|---|---|---|---|
| D2_DEF_1 | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 | aacb6a3c |
| D2_DEF_2 | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 | aacb6a3c |
| D2_DEF_3 | 1633 | 0.1900 | 0.2550 | 1965.67 | 398/400, 1372.1 | b9d24cab |

3/3 in band, all at or above 1600 lines. D2_DEF_1 and D2_DEF_2 equal the GRID lane's D2_DEF_2 / D2_GR_3 (`aacb6a3c`)
byte for byte. D2_DEF_3 is inside the known wall-clock run-to-run spread.

## Tracker follow-up
None. 0x004c4d20 keeps its one scanner-visible body. The deleted inline form was never a tracked row.

## Hygiene
- All runs under `mashed_lock.py run --label D4-DC-4d20-FLIP` (D2 with `--solo`). Private roots
  `D:/MashedWT/D4-DC-4d20-FLIP_root` and `D:/MashedWT/D4-DC-4d20-FLIP_root13`. Only PIDs this lane spawned ran.
- Builds with `MASHED_NO_DEPLOY=1`. `original\mashed_re_dev.asi` was not deployed. Nothing ran from the main checkout.

## Raw files (not committed, `D:/MashedWT/D4-DC-4d20-FLIP_data`)
| file | sha256 |
|---|---|
| arms/H_DEF.step.csv, arms/H_GT.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms/H_DEF.gates.csv, arms/H_GT.gates.csv | b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f |
| arms/H_DEF.ptrace.log, arms/H_GT.ptrace.log | 09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1 |
| arms/B_DEF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| arms/B_DEF_K1.step.csv | 2516d9034a1774893bf2f59edd368c3783ad6cfe0ed491dc68b3d84a0afec7f6 |
| arms/B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| arms/W_GT.step.csv, arms_base/W_GT.step.csv | d29c714570caa3dbe85bf246d1e238aeb4d2518f71edf7de9b6a214353eb50f1 |
| arms/D2_DEF_1.motion_diag.log, arms/D2_DEF_2.motion_diag.log | aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314 |
| arms/D2_DEF_3.motion_diag.log | b9d24cab9ade807e784a0f400d827c9e0eea29e6a4eb52dfbfad723e92011d98 |
| arms/H_GT.gridrot.txt, arms/T9.gridrot.txt, arms/T13.gridrot.txt | c0f7502e8c3732a59970308268bb8801764a20c0468ba4603b475a3b1a979e8c |
| arms/W_GT.gridrot.txt, arms/T11.gridrot.txt | 7478b34ba54b17f2944d90fefe9ac464addd1788410860e31adf2bae0de993cf |
| arms/T1.gridrot.txt | ec3ce51ca7b06c85de8e741dba2a4ab286ca915616e289b2a85fae889e24f773 |
| arms/T2.gridrot.txt | 68603263519073482f540ee428b81c4c3e63d7f9be093d2b3a7839bba7462722 |
| arms/T3.gridrot.txt | 6948360360f0903a28756040fe588b8cc290ed909352db0a8ec5e18b1445fa14 |
| arms/T4.gridrot.txt | 86f1621f5cf21fcb29cff62b40e862733b16b88c72d26a770a89898f33164714 |
| arms/T5.gridrot.txt | c9cfaae20f41445ddc85419421a795f2e7f9f3b6d7259e2fbcbd1b716203a0e2 |
| arms/T6.gridrot.txt | 5957746563ea99fe5a4b929b7a273442de6a70ed4736e49b0a34302f6acb31ac |
| arms/T7.gridrot.txt, arms/T8.gridrot.txt | d3767998d9c65317f9846d183438662eca6416e9c3361762e5cb159967a993e8 |
| arms/T10.gridrot.txt | b849effd2279a27d18b14044348a67c7fef1341c946a7a18468fe7fb5f2460e8 |
| arms/T12.gridrot.txt | f812d8ec6967f2ca945f9b5548bb88ee4f55c73a8cda32c9fa03d0ba7a7c398b |
| arms_base/W_GT.gridrot.txt, arms_base/T11.gridrot.txt | 4a9332058fbcb01d6e81098868d140d6211b760f2fb292eae7839a9f467da91d |
| arms_base/T9.gridrot.txt, arms_base/T13.gridrot.txt | 9a47edf405949f9d8e8bef90c81de0cb8f1a32d7015e0734438efd6d697d2388 |
| arms_base/T1.gridrot.txt | 1e4215f1f0eeb4e794b825a7e4f2a547b0842ad6badcb373e73a91f5829191dd |
| arms_base/T2.gridrot.txt | 93e4173f357d601f5538cf8a394a50eb0efeb538b5327789a6a1b0cc0634aafd |
| arms_base/T3.gridrot.txt | 095e0078a8adbfcf81a59120731c01a234342d8bb42ccdf03942c739c2198905 |
| arms_base/T4.gridrot.txt | 69025efad8322732c94861ee1d08e6815ff52eb319401c89ff23e3534790b23f |
| arms_base/T5.gridrot.txt | 8500d998cf220ac30e8d5387684d008b14c98746100dffc5a025ed059428cad9 |
| arms_base/T6.gridrot.txt | 356168abef4503bd34639643f170edb00e8c7da8c4261cc3a8392a887e2a0fcb |
| arms_base/T7.gridrot.txt, arms_base/T8.gridrot.txt | 52c0debf2d725ba0fcede94d29292ff6475a6954b3ad5df55261a70b77f9d1f7 |
| arms_base/T10.gridrot.txt | 382b0d54d42d0880fe75de0023fe47ca7ed76448595421170eaf96d7c29bf903 |
| arms_base/T12.gridrot.txt | 5ac17786e1bcde2bb04d53e8d8db2a73ea4724ee79a9b049a1a7338d5dc4ce03 |
| exe_base/mashed_re.exe (pre-flip, `c2bf772f8`) | 309139e483e2c6a9c55e0f3e3a3c12f99d11b203212e6c9e3aa2ada144387484 |
| exe_flip/mashed_re.exe (this branch) | 19fe4caea671eb25bc2ef05a7ce7cba167987a8345421f76a071f3e7d0bbaf79 |

Committed here: this RESULT, `gridrot_lines.txt`, `tracks_score.txt`, `score_d2.txt`, `RUNLOG.txt` (arm hash list).
