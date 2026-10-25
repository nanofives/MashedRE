# RESULT dcmath4d20: one body for 0x004c4d20 (D4-DC-MATH-4d20, W0-9)

**Verdict: GREEN, knob default OFF. The 0x004c4d20 allowlist key is gone. One decision row is filed (flip
`MASHED_CAM_ROTATE_RW` ON).** Every PREREG gate held (`PREREG_dcmath4d20.md`, commit `d86299246`, committed alone
before any arm).

## What changed (commit `8d6513d39`, base `aed0e1bc7`)
- `Race/RaceCamera.cpp`: `MASHED_CAM_ROTATE_RW=1` (default OFF) makes `RotateAboutAxis` call the shared
  `RwMatrixRotate(m, axis, deg, 0)` (0x004c4d20) and then `Math::RwV3dTransformVectorsCPU(v, v, 1, m)` (the device
  slot +0x14 stand-in that `FUN_004c3df0` dispatches). The axis goes in unnormalized, as the original passes it. The
  old fused std::cos / std::sin Rodrigues stays for the OFF path, with no RVA header (port-only arithmetic).
- `re/tools/dual_copy_allowlist.txt`: key `DUP-IN-TARGET:004c4d20` removed. rva-lint now reports 1 finding
  (`004177b0`, CROSS-TARGET), NEW=0. The row's "RwMatrixRotate.cpp:52" line question is moot: the key is deleted.
- `verify/savelink_20261023/run_arms.py` gains stack `CR`.

## Where the cost lands: the camera, not D2
S1: `MASHED_ROTATE_LUT` and `MASHED_ROTATE_PC24` are already ON in the exe default. `Core/FaithDefaults.cpp:30-31`
seeds both (faithflip 2026-10-20). The row's "default OFF" is stale, as 4680's was. The shared body already runs the LUT
inverse sqrt and the PC_24 scope for all of its callers.

S2: the original's four camera sites are each a `FUN_004c4d20(m, axis, deg, 0)` + `FUN_004c3df0(dst, src, 1, m)` pair:
`0x00441892`/`0x004418ab`, `0x004418cc`/`0x004418da`, `0x00441968`/`0x00441976` (`FUN_00441820`) and
`0x00447557`/`0x00447570`.

S3 (`disasm_extract.txt`, exe sha256 `ff990ccf...7c6709`, image base `0x10000`): `RotateAboutAxis` (`0x000E87F0`) is
called from `RaceCamera::NodeDir` (`0x000E874E`, `0x000E87CF`) and `RaceCamera::Update` (`0x000E92C2`), and it calls
`_RwMatrixRotate` (`0x000E885B`) and `RwV3dTransformVectorsCPU` (`0x000E8868`). Camera code only. Measured: with the
knob ON, G-CTRL, B and D2 are unchanged (G2, G3), and the camera dump moves on 2769 of 2999 rows (G-CAM).

S5, reported and not touched: a headerless inline 0x004c4d20 form that does feed D2. `Race/RoundGrid.cpp`
`GridDir_PortYaw` computes `sin r` and `1 - cos r` in double for `BodyOrient_NoteExactBasis`, read by `BodyOrient_Init`
under `MASHED_SPAWN_BASIS_RW` (seeded ON, `FaithDefaults.cpp:28`). The original builds them with
`FUN_004c4d20(m, (0,1,0), deg, 1)` at `0x0047061c` / `0x0047062e`. The scanner does not see it (no RVA header).
Swapping it for the shared body would move the spawn basis and so the D2 gates. That needs its own step.

## G1: default gates (refs from `re/orchestrator/merge_train.json`)
| arm | step | gates | ptrace | ref |
|---|---|---|---|---|
| H_DEF | ef5f2e10 | 57f7ff83 | b4f4b55a | ef5f2e10/57f7ff83/b4f4b55a = |

| arm | K0 | K1 | K26 | ref |
|---|---|---|---|---|
| B_DEF | c37d4c0c | 0a8f9806 | 7013671d | c37d4c0c/0a8f9806/7013671d = |

PASS.

## G2: knob ON (feeds the decision)
| arm | hashes |
|---|---|
| H_CR | ef5f2e10 / 57f7ff83 / b4f4b55a |
| B_CR K0/K1/K26 | c37d4c0c / 0a8f9806 / 7013671d |

Equal to the refs, as predicted (S4). These are the expected hashes with the flip.

## G3: D2, solo (`score_d2_v2.py`, whole-log window, band.json; `score_d2.txt`)
| run | lines | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| D2_DEF_1 | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 |
| D2_DEF_2 | 1633 | 0.1900 | 0.2550 | 1965.67 | 398/400, 1372.1 |
| D2_DEF_3 | 1632 | 0.1900 | 0.2550 | 1965.67 | 398/400, 1372.1 |
| D2_CR_1 | 1632 | 0.1900 | 0.2550 | 1965.67 | 398/400, 1372.1 |
| D2_CR_2 | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 |
| D2_CR_3 | 1631 | 0.1900 | 0.2550 | 1965.35 | 398/400, 1372.1 |

All 6 in band. The CR logs are byte-identical to DEF logs: D2_CR_1 = D2_DEF_3 (`54a7c537`), D2_CR_2 = D2_CR_3 =
D2_DEF_1 (`aacb6a3c`). PASS.

## G-CAM: d1r2's row-indexed camera replay (`cam_scores.txt`)
Replay of `verify/d1r2_20261016/orig2_trace_v3.csv`, 3000 frames, private root, `run_replay.py` (copy of d1r2's with
only the root changed). Scored with `verify/d1r2_20261016/score_d1r2.py pose` (175 joined racing rows).

| arm | cam_dump sha8 | pos median | pos p95 | pos max | at median deg | at p95 deg | at max deg | right median deg | G-POSE |
|---|---|---|---|---|---|---|---|---|---|
| CAM_DEF | 4cb520ac | 7.97135e-05 | 7.87615e-04 | 1.75759e-03 | 3.97552e-04 | 2.31678e-03 | 6.90082e-03 | 2.09131e-06 | PASS |
| CAM_CR | 7467ebf4 | 7.95543e-05 | 7.87615e-04 | 1.75851e-03 | 3.97552e-04 | 2.31646e-03 | 6.90082e-03 | 2.09131e-06 | PASS |
| CAM_DEF2 | 4cb520ac | = CAM_DEF | | | | | | | PASS |

- G-CAM-RUN PASS: `CAM_CR` differs from `CAM_DEF` on 2769 of 2999 rows (largest field delta: `pitch` 1.19e-3,
  `yaw` 6.2e-5). The replay executes the rotate sites.
- G-REP PASS: `CAM_DEF2` is byte-identical to `CAM_DEF` (cam and librw dumps), so the CR difference is the knob.
- G-CAM-POSE PASS: both pass d1r2's thresholds. CR is equal on pos p95 and lower on at p95 (2.31646e-3 against
  2.31678e-3 deg) and on pos median (7.95543e-5 against 7.97135e-5). pos max is 9.2e-7 higher. The change against the
  original is below 2e-6 in every column: the camera's residual pose error is set by other arithmetic, as dualcopy4
  recorded (`verify/dualcopy4_20261016/RESULT_dualcopy4.md`, "the camera it would replace is 1e-4-accurate").

## Decision filed
`MASHED_CAM_ROTATE_RW` default ON (add `CAM_ROTATE_RW` to the FAITH set in `Core/FaithDefaults.cpp`), then delete the
port-only stand-in? Expected G-CTRL / B with it ON: `ef5f2e10/57f7ff83/b4f4b55a`, `c37d4c0c/0a8f9806/7013671d` (G2,
unchanged). D2 unchanged (G3). Camera replay pose: equal or lower on 6 of 7 columns, pos max +9.2e-7 (G-CAM). Options:
A, flip ON (the camera runs the original's call pair through the one shared body). B, flip ON and delete the stand-in
and the knob in the same change. C, keep OFF. Recommendation: B. The stand-in has no faithfulness claim and no other
caller, and the measured cost of removing it is nil on every gate.

## Tracker follow-up (not done here: trackers only via re-classify)
0x004c4d20 now has one body in the exe that the scanner can see. The RoundGrid inline form (S5) is a candidate row for
the dual-copy burn-down. It is invisible to rva-lint.

## Hygiene
The first driver pass lost its first H_DEF run: `D:/MashedWT/D4-DC-MATH-4d20_data/arms` did not exist and
`run_arms.py` raised on the copy (no RUNLOG line was written). The directory was created, and all G1 / G2 arms were
re-run under a second solo lock (`run_hb.sh`). Only PIDs this lane spawned ran. `original\mashed_re_dev.asi` was not
deployed (`MASHED_NO_DEPLOY=1`).

## Raw files (not committed, `D:/MashedWT/D4-DC-MATH-4d20_data`)
| file | sha256 |
|---|---|
| arms/H_DEF.step.csv | ef5f2e105f649cdbf1a900340eb128ac8317fce986d36538248e3b686d1178ae |
| arms/H_DEF.gates.csv | 57f7ff835cd9b7d774b939f08e091cf4b5faeca58cdcbd7fad0b41563fe134c6 |
| arms/H_DEF.ptrace.log | b4f4b55a20e57e3a37b1643703bfa8e06d6973561e943bdaf369112aaefeb28f |
| arms/B_DEF_K0.step.csv | c37d4c0cfbdd5843f946feec67c0d3559ac17613d2520cf86ac53f6f79935f84 |
| arms/B_DEF_K1.step.csv | 0a8f9806be171e483c1626fc0f985716a6412dd803e33587f408024e3870f1ec |
| arms/B_DEF_K26.step.csv | 7013671dfea95c72b49f484ddae75cefeed87cb0f95d34495a3ba59aec6e6dc0 |
| arms/H_CR.step.csv | ef5f2e105f649cdbf1a900340eb128ac8317fce986d36538248e3b686d1178ae |
| arms/H_CR.gates.csv | 57f7ff835cd9b7d774b939f08e091cf4b5faeca58cdcbd7fad0b41563fe134c6 |
| arms/H_CR.ptrace.log | b4f4b55a20e57e3a37b1643703bfa8e06d6973561e943bdaf369112aaefeb28f |
| arms/B_CR_K0.step.csv | c37d4c0cfbdd5843f946feec67c0d3559ac17613d2520cf86ac53f6f79935f84 |
| arms/B_CR_K1.step.csv | 0a8f9806be171e483c1626fc0f985716a6412dd803e33587f408024e3870f1ec |
| arms/B_CR_K26.step.csv | 7013671dfea95c72b49f484ddae75cefeed87cb0f95d34495a3ba59aec6e6dc0 |
| arms/D2_DEF_1.motion_diag.log | aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314 |
| arms/D2_DEF_2.motion_diag.log | b9d24cab9ade807e784a0f400d827c9e0eea29e6a4eb52dfbfad723e92011d98 |
| arms/D2_DEF_3.motion_diag.log | 54a7c537f8bc1f4b6260a771c4aba182d1bb44002313e208d532ad31b1d20d14 |
| arms/D2_CR_1.motion_diag.log | 54a7c537f8bc1f4b6260a771c4aba182d1bb44002313e208d532ad31b1d20d14 |
| arms/D2_CR_2.motion_diag.log | aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314 |
| arms/D2_CR_3.motion_diag.log | aacb6a3c1e1d0a308d9a0619547a9a6ebf28c6ab64f387f9c74a21d506786314 |
| cam/CAM_DEF/cam_dump.csv | 4cb520accfaee786e3dfe8d5eb9d3572608eca85497327ae8c7da0adba9f91b5 |
| cam/CAM_DEF/librw_dump.csv | 229b47f22deb61f79ad6a0415d6a4e11e53d44f152eebbbcd29e54aac649c0e3 |
| cam/CAM_CR/cam_dump.csv | 7467ebf471621dd5d8f8b8a5e147975c1d9a3dcacd18973b18fe920ddf448c5e |
| cam/CAM_CR/librw_dump.csv | 6d26f8915c7b114a1621f0752521338a7d36e8106fb433e7d1ae933e303d0244 |
| cam/CAM_DEF2/cam_dump.csv | 4cb520accfaee786e3dfe8d5eb9d3572608eca85497327ae8c7da0adba9f91b5 |
| cam/CAM_DEF2/librw_dump.csv | 229b47f22deb61f79ad6a0415d6a4e11e53d44f152eebbbcd29e54aac649c0e3 |
| exe_disasm.txt (dumpbin /disasm) | 1486e1353a6705dabe205d1f4ec833d611cb2647e21d02d075e77ec45f191907 |
| mashed_re.map | 06e074c1846b8fd375541af68b62b971cf37e41850377045ed45869696b687e0 |
| mashed_re.exe | ff990ccf9537e254a53c3ab33ee71c3d0fdf8f1deae5a27e6b61a45cbf7c6709 |

Committed here: this RESULT, the PREREG, `score_d2.txt`, `cam_scores.txt`, `RUNLOG.txt` (hash list),
`disasm_extract.txt`, and the drivers `run_all.sh`, `run_hb.sh`, `run_replay.py`.
Runs: `mashed_lock.py run --solo --label D4-DC-MATH-4d20`, private root `D:/MashedWT/D4-DC-MATH-4d20_root`.
