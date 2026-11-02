# RESULT D4-DC-INC-SQRT-FOLD

Verdict: **DECISION**. Knob OFF leaves every gate on its ref. Knob ON leaves G-CTRL and B unchanged, and D2 scores 3/3 in band.
Flipping the default is filed as a roadmap decision.

Branch `orch/D4-DC-INC-SQRT-FOLD` off `orch/integration` @ `96640c25a`:
- `4f53d9923`: PREREG alone (`PREREG_D4-DC-INC-SQRT-FOLD.md`).
- `67688e49e`: `MASHED_SQRT_FOLD` knob (default OFF) in `Math/RwSqrt.cpp`, `Math/Vec3.cpp`, `Math/RwLutTables.{h,cpp}`, plus the
  `SQF` / `SQFH` stacks in `verify/savelink_20261023/run_arms.py`.

Build (`MASHED_NO_DEPLOY=1`): `mashed_re.exe` sha256 `1ab4669f7c38bc9fc81c27f644156f5a074c71d5301b559fd374503ee7f70057`,
`mashed_re_dev.asi` `cbdfade73625ee72163ba7bf083a0fb786c9e299328008b08f23f1d85dcba9c1`.
`scripts/lan_release/archive_lint.py --ref orch/D4-DC-INC-SQRT-FOLD` (67688e49e): PASS (include closure 0 unresolved, rva-lint 3
findings, all allowlisted, NEW=0, asm-lint PASS).

Private root `D:/MashedWT/D4-DC-INC-SQRT-FOLD_root`. Each arm ran in its own `mashed_lock.py run --label D4-DC-INC-SQRT-FOLD`
window (D2 arms `--solo`), and only spawned PIDs were touched.

## Predictions

| # | Prediction | Measured | Holds |
|---|---|---|---|
| 1 | G0 (knob OFF) = refs: G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3 | `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9` | yes |
| 2 | G1 log has no `FastInvSqrt` line (no exe caller) | no `FastInvSqrt` line in any of the 7 knob-ON logs (H, B x3, D2 x3) | yes |
| 3 | G1 hashes and D2 scores: not predicted, reported | below | n/a |

## G1 (knob ON) hashes

| Arm | step.csv | gates.csv | ptrace.log |
|---|---|---|---|
| H_SQF | `8284be46` | `ba407a16` | `31fe043e` |
| B_SQF_K0 / K1 / K26 | `1d5bf4f9` / `1d5bf4f9` / `1d5bf4f9` | | |

These equal G0 and the refs. The knob changes returned values (fold log below), but on these scenarios no changed value reaches a
hashed field. This run does not identify which call sites produce the differing values.

## Fold log (knob ON, last power-of-two line per hook)

| Arm | FastSqrt calls / differ | Vec3Magnitude calls / differ | FastInvSqrt |
|---|---|---|---|
| H_SQF | 512 / 511 | 65536 / 41768 | none |
| B_SQF_K0, K1, K26 (identical files) | 512 / 511 | 32768 / 24575 | none |
| D2_SQFH_1, 2, 3 (identical files) | 512 / 510 | 8192 / 2297 | none |

`differ` counts calls whose folded bits differ from the pre-fold form, which in the exe is the `std::sqrt` fallback.

## D2 (knob ON, `SQFH` = `SQF` + `DEFH`, scored with `verify/d2band42_20261029/score_d2_v2.py`)

Band (`verify/d2band42_20261029/band.json`): slip1500 [0.17665, 0.20605], slip2000 [0.24493, 0.25602], drvmed [1886.07, 2042.76].

| Arm | motion_diag lines | slip1500 | slip2000 | drvmed | n | H1 |
|---|---|---|---|---|---|---|
| D2_SQFH_1 (re-run) | 1644 | 0.1930 in | 0.2514 in | 1920.25 in | 385 / 649 / 1436 | 398/400, median 1333.9 |
| D2_SQFH_2 | 1639 | 0.1933 in | 0.2515 in | 1917.10 in | 382 / 647 / 1431 | 398/400, median 1333.9 |
| D2_SQFH_3 | 1638 | 0.1933 in | 0.2515 in | 1919.62 in | 381 / 647 / 1430 | 398/400, median 1333.9 |

3/3 in band. The first D2_SQFH_1 run (02:19, 112 s) wrote 807 motion_diag lines, under the 1600 minimum, so it was re-run as the
PREREG requires (one re-run used of 2). Its files are kept as `short1_D2_SQFH_1.*` and were not scored.

## Expected values for the flip (MASHED_SQRT_FOLD default ON)

G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3 (unchanged from the refs). D2 in band, scores as in the table above.

## Not measured (residual)

- E launch and run13 (the rest of the merge-train `full` set) were not run with the knob ON.
- In the .asi, the claim that form 1 (the live RW table) equals the rebuilt table comes from the `RwLutTables.cpp` header fit
  (4096/4096). It was not re-measured here. No .asi arm was run.
- Which exe call sites (PREREG reach list) produce the differing values on each scenario is not established.

## Raw data (`D:/MashedWT/D4-DC-INC-SQRT-FOLD_data/`, not committed)

Gate arms: SHA-256 of all 25 files in `sha256_gates.txt`. Key files:

| File | SHA-256 |
|---|---|
| arms/H_DEF.step.csv, arms/H_SQF.step.csv | `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198` |
| arms/H_DEF.gates.csv, arms/H_SQF.gates.csv | `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb` |
| arms/H_DEF.ptrace.log, arms/H_SQF.ptrace.log | `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3` |
| arms/B_{DEF,SQF}_K{0,1,26}.step.csv | `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` |
| arms/H_SQF.sqf.txt | `a633be35275852048f6d5feb33665a5ff7b2e7cc6bd75c69e63081ffd39ea5e3` |
| arms/B_SQF_K{0,1,26}.sqf.txt | `203de059f845bf67e58258258034ea977d222ab1a48dba9dea1e7670773cd970` |
| arms/D2_SQFH_1.motion_diag.log | `c3624e494158359ff61eafb10bfde300c15921aa5ee58d20049f6011249ce284` |
| arms/D2_SQFH_2.motion_diag.log | `e1029fa7ee9a07e8036f2762b9b24e69bc31635c636fe6aef85cceac268e907a` |
| arms/D2_SQFH_3.motion_diag.log | `fe38081b72afd4ea1ad2c44aa451677598a7c7368bffedb4b6099b13c7042d84` |
| arms/D2_SQFH_{1,2,3}.sqf.txt | `f7ce5e31d98912a1f51af80ca50e88353f0b5ab5084a36df39ebcd2be8639753` |
| arms/D2_SQFH_1.log | `bf98eda203651371c8222e60b2c75709d1565ad2609829f64297f8068c70fced` |
| arms/D2_SQFH_2.log | `b5b6d56c338d4c42a02a2653960e1c4985e76a5313defd74d8ac63cfc393cff8` |
| arms/D2_SQFH_3.log | `69099c9389cbe463aa2ca098501b1bb63b0fc1841f75443bb71671774ca90072` |
| arms/short1_D2_SQFH_1.motion_diag.log (807 lines, not scored) | `1c6288feedcef978a72047dcc8fc8abaa95a132633e929f64a5d60209206bd3f` |
| arms/RUNLOG.txt | `d000e113f0bb06336979823638734412133f4b32ce296eaa97fcc1b38279ccba` |
| d2_score.txt | scorer output for the three arms above |
