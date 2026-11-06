# RESULT D4-DC-INC-SQRT-FOLD

## Flip follow-up, 2026-11-02 (L-86 answered (a)): HANDOFF

**The flip is committed, but run13 and the post-flip gates are NOT measured.** The display on this PC was asleep for the
whole window: every `diag.py probe-render` from 09:50 to 10:25 local (18 probes, private root) failed D3D9 CreateDevice
`0x8876086A` on HAL, HAL/SW and REF. That is an environment block, not a finding.

Done:
- `12efef518`: merged `orch/integration` @ `bf8abf3d0` (167 commits) with `git -c merge.autoStash=false`. One conflict in
  `verify/savelink_20261023/run_arms.py` (adjacent STACK lines), resolved by keeping both sides. Build OK.
- `7e9e9869e`: the flip. `SqrtFoldOn()` (`Math/RwLutTables.cpp`) is now true unless `MASHED_NO_SQRT_FOLD` is set to a
  value other than 0. `MASHED_SQRT_FOLD=1`, the old opt-in, is a no-op. The comments in `Math/RwLutTables.h`, `Math/RwSqrt.cpp`
  and `Math/Vec3.cpp` name the RVAs (FastSqrt 0x004c3b30, FastInvSqrt 0x004c3b90, Vec3Magnitude 0x004c3ac0) inside the tagged
  comment. A first try put them on their own `// 0x004c3b30` lines. rva-lint then bound the `FastSqrt` call-through as a second
  body next to `Math/RwLutTables.cpp:54 LutSqrt` (2 NEW DUP-IN-TARGET findings), so that try was reworded before the commit.
  `run_arms.py` gains `NSQF` = `MASHED_NO_SQRT_FOLD=1`.
- Allowlist: `re/tools/dual_copy_allowlist.txt` has **no key** for 0x004c3ac0 / 0x004c3b30 / 0x004c3b90. The batch3 rows
  removed them, and the scanner never bound these hooks (`// Original: 0x...` headers). So there is nothing to retire.
- Build at `7e9e9869e` (`MASHED_NO_DEPLOY=1`): `mashed_re.exe` `021ef8fa095cfb2de946236dabf4c1c05e07c4c026c934c78b58a80383fa6a89`,
  `mashed_re_dev.asi` `c4366804bcb0b008d29946c958f461b47f9056048f02cb13b76a4c4d18c8056c`.
  `scripts/lan_release/archive_lint.py --ref orch/D4-DC-INC-SQRT-FOLD`: OK (rva-lint 2 findings, 2 allowlisted, NEW=0, asm-lint
  1133 files PASS).

Measured offline from the knob-ON arms of the first run (build `1ab4669f`, base `96640c25a`, files byte-identical to the B ref):

| Check (merge-train `full` set) | Input | Result | Ref |
|---|---|---|---|
| (e) E launch, `re/tools/ai_speed_env.py --check` | `arms/B_SQF_K0.step.csv` (`1d5bf4f9`) | launch 1425.7 / 2052.5 / 2055.0, (e) PASS 3/3 | `e_launch` 1425.7 / 2052.5 / 2055.0: same |
| (b) `verify/savelink_20261023/vsum.py` | `arms/B_SQF_K{0,1,26}.step.csv` | 3/3 on all three arms | 3/3 x3: same |

These two checks read only the B step.csv, so they hold for any build whose B arms hash `1d5bf4f9` x3. They were not re-run on
the post-merge build.

Gate-neutral claim for the flip, from measured data only: on the pre-merge base, knob ON gave G-CTRL `8284be46 / ba407a16 /
31fe043e`, B `1d5bf4f9` x3, D2 3/3 in band, E launch same, (b) 3/3. The merge brings in 167 integration commits whose own gates
were the same refs (`merge_train.json`). The combination (merged base + fold ON) has not been run.

**Remaining runs (exact), from the worktree, once `diag.py probe-render` passes with `MASHED_ROOT=D:\MashedWT\D4-DC-INC-SQRT-FOLD_root`:**
1. Post-flip default gates, expected G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3:
   `py.exe -3.12 <orch-wt>\re\orchestrator\mashed_lock.py run --label D4-DC-INC-SQRT-FOLD -- py.exe -3.12 verify\savelink_20261023\run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`
   with `A4_EXE=<wt>\mashedmod\build\mashed_re.exe`, `A4_ROOT=D:\MashedWT\D4-DC-INC-SQRT-FOLD_root`, `A4_OUT=D:\MashedWT\D4-DC-INC-SQRT-FOLD_data\arms_flip`.
   Then `vsum.py` and `ai_speed_env.py --check` on the new B files (expect as above).
2. run13 (now the default = fold ON), table identical to `verify/run13p3_20261029/tracks_score_sub_p3.txt` and INSTRUMENT GATES PASS:
   `mashed_lock.py run --solo --ttl 14400 --label D4-DC-INC-SQRT-FOLD -- py.exe -3.12 verify\rsky_20261016\run13.py run --exe <wt>\mashedmod\build\mashed_re.exe --root D:\MashedWT\D4-DC-INC-SQRT-FOLD_root --out D:\MashedWT\D4-DC-INC-SQRT-FOLD_data\tracks13`,
   then `run13.py score --out ...\tracks13 --cite verify\rres_20261016\cites_rres.json`.
3. Optional: D2 x3 (`D2_DEFH_1..3`, `--solo`), score with `verify/d2band42_20261029/score_d2_v2.py`, at least 1600 motion_diag lines.

The merge train's `-Full` run (review as SOURCE -Full FLIP) also covers 1 to 3.

Raw files of this follow-up (`D:/MashedWT/D4-DC-INC-SQRT-FOLD_data/`, not committed):

| File | SHA-256 |
|---|---|
| e_check_sqf.txt | `dca388aae767aaf50d9cb7d39350cb495a3966b9cedbdfbee6d68719c6d49c20` |
| vsum_b_sqf.txt | `4b7b9546fe63cf7878e30860aab64e19e2d7850b2c0422924df590158de2097b` |
| build_merge.log | `90fd9b1a4953008bfcee656cb4344f10b35c019afd44d658f0d040dfd3f3d158` |
| build_flip.log (final, NEW=0) | `bbc0ba43586816c0218e7675eac2d5a14c7baba09b73134d26fdaccc9feaa4bc` |

## First run, 2026-11-01

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
