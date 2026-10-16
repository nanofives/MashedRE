# RESULT d2bsp: the BSP sphere walk FUN_00538c80 and the outer chunk loop of FUN_00470c70

Branch `work/d2bsp`, rebased onto `race/first-frame-parity` `10c18c59` (first onto `34ea5b71`, amendment 3, then
onto `10c18c59` after rstates and d2engage merged, amendment 4). Pre-registration `PREREG_d2bsp.md` (sections 0..4,
amendments 1..4, each committed before the runs it governs). Model claude-opus-5-5. Started on account2 (sections
0..2, amendments 1..2, the pre-rebase arms), finished on account3 (amendments 3..4, both rebases, rebased arms, this
file). **The final arms are on `10c18c59` (`rb2/`)**; `rb/` holds the `34ea5b71` record, which they reproduce.

## 1. Verdicts (section 4 rules, applied to the rebased arms)

| fix | knob | gates | default |
|---|---|---|---|
| **L** outer chunk loop `0x00470f50..0x00471151`, A4 dt = `(float)local_24` (`0x0047105a`) | opt-out `MASHED_NO_D2CHUNK=1` | all PASS | **ON** (this branch) |
| **W** walk `FUN_00538c80` case 3 (`FUN_00539ec0` + `FUN_00547450` + `FUN_004c3b90` verbatim) + collector `FUN_00468b80` with the material RGBA key | opt-in `MASHED_D2BSP=1` | **G-WHEEL-F FAIL**, every other gate PASS | OFF, stand-in kept |
| **K** `0x0046cc40` z-first summation (`0x0046ccfd..0x0046ce13`) | opt-in `MASHED_D2CLS=1` | report only (amendment 2) | OFF |

The stand-in `ProduceTerrainBatch` (`ContactProducer.cpp`) is NOT deleted: W failed a registered gate.

## 2. Contact set per wheel against the original (Training, `o_bp1.msd`, 4764 player classifier calls)

Replays on the rebased build are byte-identical to the pre-rebase ones (`R_DEF` `e8d19d3f`, `R_K` `7dee6f32`).

| measure | fix W | fix W + K | stand-in |
|---|---|---|---|
| G-WALK: same count, order, every captured dword | **4764/4764 PASS** | same | 0/4764 (same count 8.84 %) |
| triangles (vertex dwords 0..8): same set / original's inside | 100 % | 100 % | 8.84 % / 100 % |
| G-WHEEL-K (`+0x1f0` at the next solver entry) | **19052/19052 PASS** | 19052/19052 | 30/19052 |
| G-WHEEL-F (fv, key, normal; own or axle partner) | **14596/19052 = 76.61 % FAIL** | 15512/19052 = 81.42 % | 26.96 % |
| G-WHEEL-F, frames >= 1000 (amendment 2) | 10128/11044 | **11044/11044** | 2725/11044 |
| registered report (amendment 3 hold filter) | 76.63 % | 81.44 % | 26.97 % |
| post-hoc, not registered: hold pattern any key | 14596/15512 = 94.09 % | **15512/15512** | 33.12 % |

The G-WHEEL-F miss is two sets. (a) 3540 hold-tick wheel-calls: the original's next entry holds fv 0, normal
(0, 1, 0), state 1 on frames < 1000, the per-hold-tick reset between classifier and next entry that amendment 2
cites (`RESULT_d2settle.md` D0-d). Amendment 2 described its key as -1 and amendment 3's report filter copied that:
3536 of the 3540 carry key `00000000`, 4 carry `ffffffff`, so the registered filter removes 4. (b) 916 wheel-calls
with fv 2..15 ulp off from the port's x-first summation in `0x0046cc40` (`CarWorldContacts.cpp:137-157`). K removes
all 916. Whether the port reproduces (a) on its own path is not measured (U-9710).

G-KEY PASS (`P_W_1` / `P_DEF_1`, `g_key.py`): under W every live batch key is a Training collision colour
(`ffc88080` 8466, `ffb48080` 5832, `ff008000` 4538, `ff010101` 116); without W all 28870 keys are 0. The live query
radius (`self+0x4a4`, `WheelContactSolver.cpp:152`) is `3f2d93d1` on 3300/3300 and 3293/3293 queries, the
original's `o_bp1` value.

## 3. G-CTRL, no-op, determinism (rebased; base exe `3a4d8dfe` = build of `34ea5b71`, new exe `ca3729df`)

| arm | step | gates | ptrace | x2 |
|---|---|---|---|---|
| `H_BASE` | 716cb2fd | 2a2afda1 | ba90ed88 | identical |
| `H_NOL` (opt-out) | 716cb2fd | 2a2afda1 | ba90ed88 | identical, **no-op PASS** |
| `H_DEF` (= L) | 2b4d47bb | 356821c8 | f7a155e1 | identical, = pre-rebase `H_L` (prediction met) |
| `H_W` (W + L) | d3b90ee0 | 80f01f35 | 56d62ae7 | identical |

Pre-rebase (`9420dde2`): `H_DEF` = `H_BASE`, `H_L0` = `H_BASE` (G-CHUNK PASS), `H_W` / `H_L` repeat-identical.
**On `10c18c59`** (amendment 4, base exe `da7a64ce`, new `39ed4b9a`, `rb2/`) every hash in this table is reproduced
byte-for-byte x2, both replays are byte-identical again (`e8d19d3f`, `7dee6f32`), and all nine B arms are
byte-identical to the `34ea5b71` ones. Final G-CTRL of the default build: `2b4d47bb / 356821c8 / f7a155e1`.

## 4. D2 (bounds `d81a8df6` unchanged, `score_d2.py`) and recovery H1 (`a8_launch.py`), each arm alone

Final, on `10c18c59` (`rb2/SCORE_D2_rb2.txt`):

| arm | slip 1500-2000 [0.18855, 0.19635] | slip 2000-2600 [0.24488, 0.25487] | driving-median [1904.70, 1982.44] | H1 |
|---|---|---|---|---|
| BASE x3 | 0.1927 in | 0.2489 in | 1860.35, 1860.35, 1860.27 OUT | 398/400, 1362.8 |
| DEF = L x3 | 0.1905 in | 0.2521 in | 1883.91 x3 OUT (above the base min 1860.27) | 398/400, 1362.8 |
| W (+ L) x3 | 0.1890 in | 0.2476 / 0.2483 / 0.2480 in | **1912.04, 1914.84, 1912.04 in** | 398/400, 1362.7 |

`D2_DEF_3` stalled after track load (its log ends at the `R4 track load OK` line, no motion_diag) and was re-run as
`D2_DEF_4` under the amendment-3 rule. Scored runs 1630..1635 lines. The `10c18c59` motion_diag lines carry three
columns d2engage appended (`bf4`, `bf8`, `st`); the shared prefix of `D2_DEF` is identical to the `34ea5b71` run.

On `34ea5b71` (`rb/SCORE_D2_rb.txt`, 12 runs, 1621..1636 lines): BASE 1860.27 x3; L 1883.91 x3; W 0.1890 / 0.2480 /
1913.06, 1912.04, 1913.06; WK 0.1890 / 0.2480..0.2486 / 1913.06, 1906.19, 1912.04, all in. `D2_WK_1` is
byte-identical to `D2_W_1` and `D2_W_3` (`fc692a12`), `D2_WK_3` to `D2_W_2` (`b7b6083d`): K's ulp-level fv change
reaches no printed motion_diag field on this arm. K's knob is in the function the live path calls
(`CarWorldContacts.cpp:96`, read at `:148`, called at `WheelContactSolver.cpp:161`). Pre-rebase W without L had slip
1500-2000 0.1879, OUT x3 (`SCORE_D2_pre.txt`). `+0xb14` engages at d=15 on every arm of both bases, as on the original.

**driving-median (U-9180).** The default moves 1860.27 -> 1883.91 under L (+23.64, still 20.79 = 1.09 % under the
floor). On the L build, replacing the stand-in by the walk moves it to 1912.04..1914.84, inside the bound by
+7.34..+10.14, with both slips inside. This is the first arm since D2 was parked with all three metrics inside
`d81a8df6` on 3/3 runs, and it holds on both bases.

## 5. D3 (b), collateral, watch list (same on both bases)

- D3 (b) (`score_regimes.py`): A 1/3, B 2/3, C 0/3 on BASE, DEF and W. No loss.
- Collateral (`coll.py`, A = `o_p2b`, floor `o_p0a`, banded): 39 of 94 on `H_BASE`, `H_DEF`, `H_W`. No change.
- Watch list: Training wall pin signature (memory `d2-arm-wall-pin-signature`) absent on every arm (driving n
  1330..1425, slips measured). `wcs_drift`, airborne drift and the `+0x9e0` distribution are not measured: they need
  `MASHED_D2SINK` arms, none registered. No watch-list hit reported.

## 6. Residuals

- W's G-WALK is measured on Training only (one capture, 4764 queries). The other 12 tracks' worlds are built from their
  COLLI*.BSP and their stream layout checked (section 0), but their walk is not captured against the original.
- L: the original interleaves all 16 cars per chunk, the port steps one car at a time. Identical while the port's
  `remI = (int)(dt*3000) <= 0x32` (50 at the fixed step).
- The hold-tick reset (2a) on the port side: U-9710.

## 7. Tracker changes (through `re-classify`, this branch)

- `DEFERRED.md` D-11071: the chunk-loop item done (L default ON); the walk ported opt-in with G-WALK 4764/4764; W + L
  puts all three D2 metrics in bounds 3/3; W's default waits on the user decision below.
- `UNCERTAINTIES.md` U-9180: numbers of section 4. U-9181: RESOLVED, the port's `+0x4a4` is `0x3f2d93d1` on every live
  query (= the original's), fixed by d2settle's A3 LUT sqrt (`VehicleInit.cpp:28-40`). motion_diag's `susp=` still
  prints 692.302 on the same runs, so `susp=` is not the radius the walk receives. New U-9710 (section 2 (a)).
- No C-level moves (section 4: no run_diff path for the walk in this lane).

## 8. User decision needed

Wire W default ON (opt-out `MASHED_NO_D2BSP=1`, stand-in deleted), with or without K? Registered result: W fails
G-WHEEL-F (76.61 %), so it stays OFF here. Facts for the decision: the miss is (a) 3540 hold-tick wheel-calls whose
reference is reset between the classifier and the next entry, and (b) 916 ulp-level fv differences that K removes;
with K and the hold pattern left out, 15512/15512 match (post-hoc). Every other gate passes, and W + L is the first
arm with all three D2 metrics inside `d81a8df6` on 3/3. Before a default flip: a G-WALK capture on at least one more
track, and U-9710.

## 9. Evidence

Pre-rebase: `RUNLOG.txt`, `SCORE_{D2,B,COLL}_pre.txt`, `COLLATERAL_CROSS_H_{BASE,W,L}.txt`, `*.motion_diag.log`,
replays `R_*.replay.csv`, capture `o_bp1.msd.batchprobe.csv`. Rebased (`rb/`): `RUNLOG.txt`,
`SCORE_{D2,B,COLL}_rb.txt`, `G_WALK_{DEF,K}.txt`, `G_WHEEL_F_POSTHOC.txt`, `G_KEY.txt`,
`COLLATERAL_CROSS_H_{BASE,DEF,W}.txt`, `*.motion_diag.log`, `*.ptrace.log` (`34ea5b71`); `rb2/` the same set on
`10c18c59` (final), plus `SCORE_*_rb2.txt`. Scorers: `g_walk.py`, `g_key.py`,
`score_all.py`, `coll.py`; runner `run_arms.py`.
