# RESULT D3-CF-SNAP-FLIP: D2 3/3 in band with MASHED_SNAP_POST=1 (identical to the default). Flipped default ON: G-CTRL unchanged, B K1 e85ea6fb -> 9fcdbd3a only (2026-10-28)

Decision L-43 step 2 (`log/orch/DECISIONS.md`, Mariano 2026-10-27). Branch `orch/D3-CF-SNAP-FLIP` off `orch/integration`
`852369807` (ancestry checked). PREREG `PREREG_D3-CF-SNAP-FLIP.md` (`5f4ac5c4c`), committed alone before any arm; flip + runner
`0de5e0171`. Private root `D:/MashedWT/D3-CF-SNAP-FLIP_root` (robocopy of `original\`), raw data `D:/MashedWT/D3-CF-SNAP-FLIP_data`.
Builds with `MASHED_NO_DEPLOY=1` (nothing redeployed to `original\`). Standalone only: no original, no Frida, no audio-endpoint crash.
Runner `run_arms.py` = `verify/snap_20261027/run_arms.py` with paths changed and the `NOSP` / `NOSPSL` stacks added.

## 1. D2 x3 with MASHED_SNAP_POST=1 (knob tree, exe `d538d8d0`, `852369807` + PREREG)

Each arm alone under `mashed_lock.py run --solo`, wrapped in `test-slot`, with no `merge_train.ps1` process live (checked
before each arm). All three over 1600 `motion_diag` lines, no re-run needed. Scorer `verify/d2rebase_20261019/score_d2_v2.py port`,
band `verify/d2rebase_20261019/band.json`.

| arm | lines | slip1500 | slip2000 | drvmed | H1 |
|---|---|---|---|---|---|
| band | | [0.18396, 0.20586] | [0.24555, 0.25557] | [1893.63, 1997.46] | |
| D2 default (today) | | 0.1942 | 0.2492 | 1896.74 | |
| `D2_SP_1` | 1633 | 0.1942 in | 0.2492 in | 1896.74 in | 398/400, median 1333.9 |
| `D2_SP_2` | 1629 | 0.1942 in | 0.2492 in | 1896.74 in | 398/400, median 1333.9 |
| `D2_SP_3` | 1634 | 0.1942 in | 0.2492 in | 1896.74 in | 398/400, median 1333.9 |
| `D2_DEF_1` (flip exe `eb1fe5b8`, no knob) | 1628 | 0.1942 in | 0.2492 in | 1896.74 in | 398/400, median 1333.9 |

P1 holds: 3/3 in band, each metric equal to the current default to the printed digit (n = 334 / 529 / 1208 on every arm). This is
the structural expectation from `verify/snap_20261027/RESULT_D3-CF-SNAP.md` section 3: D2 arms set `MASHED_MEASURE_SOLO=1`, so
`ai_cars_` is empty and the opponent loop the knob changes has no iteration. drvmed stays 3.11 over the band floor, as before.

## 2. The flip (`D3d9Render/TrackRenderer.cpp`, `SnapPost()`)

`MASHED_SNAP_POST` is default ON at its own site (the `MASHED_COLL_LUT` / `MASHED_A5_RACE_TIMER_RUN` convention, not seeded in
`Core/FaithDefaults.cpp`). `MASHED_NO_SNAP_POST=1`, or `MASHED_SNAP_POST=0`, opts out to the pre-flip code path (restore +
pre-commit stuck relocation). `MASHED_SNAP_LOG` stays default OFF. `OuterSnapshot` / `OuterRestore` are not retired here.

## 3. Gates on the flipped tree (exe `eb1fe5b8`)

| arm | step | gates | ptrace |
|---|---|---|---|
| ref (`merge_train.json`) G-CTRL | `5943c5f1` | `364350ae` | `52052890` |
| `H_DEF` (new default) | `5943c5f1` | `364350ae` | `52052890` |
| `H_SL` (default + `SNAP_LOG`) | `5943c5f1` | `364350ae` | `52052890` |
| `H_NOSP` (opt-out) | `5943c5f1` | `364350ae` | `52052890` |

| arm | K0 | K1 | K26 |
|---|---|---|---|
| ref B (`merge_train.json`, pre-flip) | `3a03670c` | `e85ea6fb` | `53778664` |
| `B_DEF` (new default) | `3a03670c` | **`9fcdbd3a`** | `53778664` |
| `B_SL` (default + `SNAP_LOG`) | `3a03670c` | **`9fcdbd3a`** | `53778664` |
| `B_NOSP` (opt-out) | `3a03670c` | `e85ea6fb` | `53778664` |

- P2 holds: only B K1 moves. Full SHA-256 of `B_DEF_K1.step.csv` = `9fcdbd3a6901025544e3...` equals the D3-CF-SNAP lane's
  `B_SPSL_K1.step.csv`, byte for byte. No other hash moves.
- P4 holds: the opt-out reproduces the pre-flip refs on all six hashes.
- (b) per-regime envelopes on the new default (`verify/savelink_20261023/vsum.py`, `vsum_b_flip.txt`): K0 A 3/3, K1 B 3/3,
  K26 C 3/3 = **9/9**. (e) `re/tools/ai_speed_env.py --check H_DEF.step.csv` (`e_check_flip.txt`): 3/3 PASS, launch
  `1425.7 / 2052.5 / 2055.0` unchanged.

## 4. OuterSnapshot / OuterRestore mismatch count on the flipped tree (`snap_counts.txt`)

P3 holds. On all twelve flipped-tree arms: 0 `[snap] dead`, 0 `spin`, 0 `reloc`, 0 `reloc-any`, 0 `mismatch` restore lines and
**0 `[substep] outer` mismatch lines**. The only `[snap]` lines are `reloc-post` on `B_SL_K1` at frame 331 slot 1 and frame 395
slot 2 (the same trigger frames as the D3-CF-SNAP lane). So with the default ON, `OuterRestore` is reached 0 times on G-CTRL and B.
(The `DEF` / `NOSP` arms carry no `SNAP_LOG`, so their `[snap]` columns are 0 by construction; the `[substep] outer` line is
unconditional and is 0 on all twelve.)

## 5. Verdict (PREREG section Verdicts): GREEN

Expected refs for the decided-flip merge (MERGE_TRAIN.md "Decided default flips"): **G-CTRL `5943c5f1 / 364350ae / 52052890`
(unchanged), B `3a03670c / 9fcdbd3a / 53778664`** (only K1 moves, `e85ea6fb -> 9fcdbd3a`). The flip merges alone, with
`merge_train.json` `b.ref` updated on `orch/integration` first (cite L-43 and this RESULT in `_doc`, keep the old ref there).
13 tracks not run in this lane: `D3d9Render/` is outside `full_set_paths`, and the knob's code path only runs with opponent cars.
Next step under L-43: retire `OuterSnapshot` / `OuterRestore` once the mismatch net has stayed at 0 (0 here on all twelve arms).

## 6. Raw data (`D:/MashedWT/D3-CF-SNAP-FLIP_data`, not committed; full list `raw_sha256.txt`)

| file | SHA-256 |
|---|---|
| `exe_knob.exe` | `d538d8d0acab3b221cd867a8baebd87c6e1c2bb7a2050538f196970845d698e3` |
| `exe_flip.exe` | `eb1fe5b88dfc368ce2e1cb21d582bce2b7cc4562bb0028d22a784f7bac423825` |
| `arms/D2_SP_1.motion_diag.log` | `411c026d596825808f0f68399b280d5b97a5a2bb9969662b91def4eb8e44c36f` |
| `arms/D2_SP_2.motion_diag.log` | `c381fdd789dfc782ac61c8b23821970b39e61fed94616c44f5537aa869d6bfa6` |
| `arms/D2_SP_3.motion_diag.log` | `681a27d3c5650e8c6b71a9afcce38a6b260062f6db6971e9a9f05b86aefea1ce` |
| `arms/D2_DEF_1.motion_diag.log` | `ae9532cc93bea98d335c0e31aac5df66dd39a4b3e87d3b4b849fc00f7f9fcfef` |
| `arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `arms/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms/B_DEF_K1.step.csv` | `9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f` |
| `arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `arms/B_NOSP_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `arms/B_SL_K1.log` | `c42efab5edea30656e4e43e40faa358a87fad7299c2038f54aecbb99672a7f32` |
| `arms/RUNLOG.txt` | `4f60b065750fcead2f37a0fd3773df12d383a61077d8404a9296ca6c3590eb13` |

Re-run (from the worktree): D2 `A4_EXE=<exe> py -3.12 <orch-wt>/re/orchestrator/mashed_lock.py run --solo --label D3-CF-SNAP-FLIP --
py -3.12 verify/snapflip_20261028/run_arms.py D2_SP_1` (one arm per solo window); gates `... run --label D3-CF-SNAP-FLIP -- py -3.12
verify/snapflip_20261028/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_SL B_SL_K0 B_SL_K1 B_SL_K26 H_NOSP B_NOSP_K0 B_NOSP_K1 B_NOSP_K26`.

```
REPORT D3-CF-SNAP-FLIP
verdict: GREEN
branch: orch/D3-CF-SNAP-FLIP @ <see ledger / final report>
evidence: verify/snapflip_20261028/RESULT_D3-CF-SNAP-FLIP.md
gates: G-CTRL 5943c5f1/364350ae/52052890  B 3a03670c/9fcdbd3a/53778664   (expected new refs; only B K1 moves e85ea6fb->9fcdbd3a)
decision: none (L-43 decided); merge alone under the decided-flip protocol, b.ref K1 -> 9fcdbd3a on orch/integration first
next: merge-train the flip alone; then the L-43 step 3 lane retires OuterSnapshot/OuterRestore (mismatch net 0 on 12/12 arms)
```
