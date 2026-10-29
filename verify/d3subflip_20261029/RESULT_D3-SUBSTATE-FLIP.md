# RESULT: D3-SUBSTATE-FLIP, run 22 (D-11073, L-53 / L-55 / L-57)

Date 2026-10-29, LIVING-PC. Branch `orch/D3-SUBSTATE-FLIP`: run 20's branch, merged with `orch/integration` f6033d936
(conflicts in TrackRenderer.cpp, which took integration's amendment-1 comment, and run_arms.py, which kept both stack
blocks) and with `orch/D2-BAND-42S` 5df890780 (merge-queued, carries the O42 band and stack `DEFH`). PREREG
`PREREG_D3-SUBSTATE-FLIP.md` was committed alone at c64b2bfac, before the seed and before any arm. The seed is 4c5c28a4e,
and exe `34b53a28` was built from it with `MASHED_NO_DEPLOY=1`. Standalone only: no MASHED.exe launched, no .asi deployed,
no .ps1 edited, no tracker touched.

## 0. Verdict first

> **DECISION. The flip is measured, the opt-out gives back the current refs exactly, and D2 is in the O42 band 3 of 3.
> But the 13-track table moves on 24 of 39 rows, 19 of them RED.** Every RED row is one of two kinds of free-running
> animation: the pickup IconCube (weapon textures MOVED 0.011..0.074, same triangle count) and the helicopter
> (JetRanger / Apache + Blads present on one side only). The cause is the instrument, not the render. The original's
> trace starts in phase 3 (ba8c = 3, GO at row 554..1064). Under the flip, the port runs its own phase 3 and only then
> starts consuming trace rows, so at shot row K its sim runs a whole pre-race ahead of the original's.

| leg | result |
|---|---|
| G-CTRL, flip (`H_DEF` x2) | **8284be46 / ba407a16 / 31fe043e**, 2 of 2 identical |
| B, flip (`B_DEF_K0 / K1 / K26`, K0 x2) | **1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9** |
| opt-out (`H_NOSUB`, `B_NOSUB_K*`, same exe) | 5943c5f1 / 306186d4 / 52052890, 3a03670c / 9fcdbd3a / 53778664 = merge_train.json refs (P3 PASS) |
| D2, 42 s hold, O42 band (`D2_DEFH`) | 3 of 3 in band on a quiet machine (triple 2, 1633-1646 lines). 8 of 8 arms in band overall (section 3) |
| 13 tracks vs `pregocubeflip_20261029/tracks_score.txt` | 24 rows move, 19 RED, instrument gates PASS (section 4) |
| 13 tracks, opt-out at the same +800 budget | identical to the reference, file sha cf3307d2 = cf3307d2 |

## 1. P1 / P2: the expected refs, and why gates.csv is not ROW315's

- `H_DEF` step 8284be46 (13962 rows) and ptrace 31fe043e (27042 lines) equal ROW315's H_SUB. gates.csv is **ba407a16**,
  not ROW315's 2b19135b. P1 is therefore only partly met.
- Attribution: `H_NOPGIC` (this exe plus `MASHED_NO_PICKUPS_PREGO=1 MASHED_NO_ICONCUBE=1`, the L-56 opt-outs) gives
  **8284be46 / 2b19135b / 31fe043e**, ROW315's three hashes exactly. So the gates.csv move comes entirely from the L-56
  flip that landed after ROW315's exe. The delta is 4 rows, all on frame 0 (rows 1-4): `ref_idx` 0 -> 3 and `exp_dist` /
  `refdist`. That is the same shape as L-56's own default move (364350ae -> 306186d4: frame-0 rows 1-4, `other_idx` 0 -> 3).
- B: 1d5bf4f9 on K0, K1 and K26 (K0 twice). This is P2 PASS, and it is run 20's flip-2 value. The three ring phases still
  collapse to one trajectory (run 20 section 4), so B loses its K1 / K26 power under the flip.

## 2. P3: the opt-out

`H_NOSUB` and `B_NOSUB_K0 / K1 / K26` (`MASHED_NO_SUBSTATE=1 MASHED_NO_P3_RESET_PAIR=1`) on exe 34b53a28 give the current
refs, G-CTRL 5943c5f1 / 306186d4 / 52052890 and B 3a03670c / 9fcdbd3a / 53778664. The 13-track opt-out run (section 4)
equals the reference table byte for byte.

## 3. P4: D2, 42 s hold, O42 band (`verify/d2band42_20261029/band.json`), `SCORE_D2.txt`

| arm | lines | slip1500 | slip2000 | drvmed | n driving | band |
|---|---:|---:|---:|---:|---:|---|
| D2_DEFH_1 | 1546 | 0.1966 | 0.2517 | 1920.48 | 1338 | in / in / in |
| D2_DEFH_2 | 1531 | 0.1966 | 0.2514 | 1916.02 | 1323 | in / in / in |
| D2_DEFH_3 | 1646 | 0.1930 | 0.2514 | 1920.48 | 1438 | in / in / in |
| D2_DEFH_1-2 (rerun) | 1501 | 0.1970 | 0.2528 | 1894.80 | 1293 | in / in / in |
| D2_DEFH_2-2 (rerun) | 1510 | 0.1966 | 0.2523 | 1901.56 | 1302 | in / in / in |
| **D2_DEFH_1-3** | **1633** | 0.1934 | 0.2515 | 1920.48 | 1425 | in / in / in |
| **D2_DEFH_2-3** | **1645** | 0.1930 | 0.2514 | 1920.25 | 1437 | in / in / in |
| **D2_DEFH_3-3** | **1646** | 0.1930 | 0.2514 | 1920.48 | 1438 | in / in / in |

- 8 of 8 arms are in band. On full-length arms the values equal D2_SUBH (0.1934 / 0.2515 / 1919.62, 1429 driving).
  D2_DEFH_3-3 is byte-identical to D2_DEFH_3 (25850da6).
- 4 arms came out short (1501-1546 lines). All of them ran while 5 of the 6 test slots were held by other sessions
  (SimGolf pytest x3, Simcity, D4-WSB-PORT-BROAD-1), and each one finished its hold normally (`race-demo done`, phase 3
  released after 776 frames). The 42 s hold is wall-clock, so a loaded machine logs fewer frames. The short arms sit
  nearer the drvmed floor: 1894.80 is 8.73 above O42's 1886.07.
- **Train risk:** under the flip, a full-length arm has 33-46 lines of margin over `d2_min_lines` 1600. The 2 reruns the
  train allows did not recover the loaded arms here. Triple 2 ran in a quieter window and was 3 of 3 full length. Its
  solo window opened while this step's own 13-track control still held the lock (`SOLO OPEN ... draining the current
  holder` followed by `SOLO HELD`). The control is frame-budgeted, and it reproduced the reference byte for byte.

## 4. P5: the 13-track table

Recipe: `run13.py run` on exe 34b53a28 with private root `D:/MashedWT/D3-SUBSTATE-FLIP_root13`, then `score --cite
verify/rres_20261016/cites_rres.json`. Instrument gates PASS.

**Frame budget (runner change).** The stock budget is `max shot + 350` (+150 per attempt). On the first try Egypt failed
all 3 attempts: shot 740 was recorded only on attempt 3, and 815 never was. The replay consumes its first trace row only
after GO, because the pre-race camera chain calls `race_cam_.Update` directly (`TrackRenderer.cpp:8281`). Only
`RaceCamUpdate` (8512) consumes rows (`cam_replay_row_ = R.next++`, 8571). The flip's pre-race (Egypt: phase 3 598 frames
+ 112 state-5 frames) did not fit. `run13.py` now adds `RUN13_PRE_FRAMES` (default 800) to every track's budget. With it,
all 13 tracks pass on attempt 1. **Control:** with the opt-out at the same +800 budget, the table equals the reference
byte for byte (`tracks_score_nosub800.txt` cf3307d2 = `pregocubeflip_20261029/tracks_score.txt` cf3307d2). The budget
change is neutral, and every move below comes from the flip.

**Rows that move** (`tracks_score_flip.txt` 580ab2ce, every uncited row in `red_rows_flip.txt`):

| track | row | ref | flip | RED rows (kind tex) |
|---|---:|---|---|---|
| 01_Egypt | 740 | CITED 17/17 | **RED** 15/20 | MISSING Blads x2, MISSING JetRanger, MOVED mine / mortar / oil (0.018) |
| 03_Arctic | 1051 | CITED 33/33 | **RED** 34/36 | EXTRA Apache, EXTRA Blads x2 |
| 03_Arctic | 1091 | CITED 36/36 | CITED 38/38 | (more cited rows, matched 150 -> 146) |
| 04_Highway | 928 | CITED | **RED** 19/20 | MOVED mine 0.011 |
| 05_Sands | 1084 / 1124 | CITED | CITED | matched 304 -> 286 / 287 -> 286 |
| 05_Sands | 1159 | CITED | **RED** 23/24 | MOVED shotgun 0.045 |
| 06_SuperG | 743 | CITED | **RED** 19/21 | MISSING JetRanger, EXTRA Apache |
| 06_SuperG | 783 | CITED | **RED** 21/23 | EXTRA Apache, EXTRA Blads x2 |
| 06_SuperG | 818 | CITED | **RED** 33/34 | MOVED machinegun 0.040 |
| 07_Roundabout | 1091 | CITED | **RED** 28/31 | MOVED depthcharge 0.014, machinegun 0.014, mine 0.029 |
| 07_Roundabout | 1131 / 1166 | CITED | **RED** 22/23, 25/26 | MOVED mine 0.034 / 0.064 |
| 08_Storm | 968 / 1003 | CITED | **RED** 23/24 x2 | MOVED mortar 0.035 / 0.040 |
| 09_Forest | 820 | CITED | CITED 17/17 | m.tri 17916 -> 17332 |
| 10_Dump | 735 | CITED | **RED** 16/18 | MISSING Blads x2, MISSING JetRanger |
| 10_Dump | 775 / 810 | CITED | CITED | row counts 17 -> 16, 19 -> 18 |
| 11_Warzone | 584 / 624 / 659 | CITED | **RED** 22/24, 19/21, 25/27 | MOVED machinegun + mortar (0.074 / 0.058 / 0.023) |
| 12_City | 836 | CITED | **RED** 18/21 | MISSING Apache, MISSING Blads x2, MOVED missile 0.012 |
| 13_Training | 884 | CITED | **RED** 19/20 | MOVED mortar 0.032 |
| 13_Training | 924 / 959 | CITED | CITED | m.tri 17560 -> 16822, 17051 -> 16817 |

RED totals by (kind, tex): MOVED mortar 7, MOVED mine 5, MOVED machinegun 5, MISSING Blads 3, MISSING JetRanger 3,
EXTRA Apache 3, EXTRA Blads 2, MOVED oil / shotgun / depthcharge / missile / MISSING Apache 1 each. Nothing outside those
two families goes RED. The existing cites do not reach these rows: C14-ANIM covers the helicopter as `MOVED` only, and
C32-ICONCUBE-NONGO covers `mine|oil` on Sands / SuperG / Warzone / Training only.

**Why (measured on the traces).** Every capfix trace starts in phase 3 and reaches GO late. Egypt: 597 rows ba8c=3,
1 row ba8c=4, 112 rows ba8c=5, GO at row 710. The GO rows of the other tracks are Neustein 1064, Highway 823, Sands 1054,
SuperG 713, Roundabout 1061, Storm 898, Forest 790, Dump 705 and Warzone 554. Every shot lands 30..105 rows after GO.
Under the flip, the port runs its own phase 3 and state 5 before the race camera consumes trace row 0. At row K its sim
is therefore about one pre-race (Egypt 710 frames) further along than the original's was. The camera and car poses are
replayed, so they still match. The helicopter path (`CopterTick`) and the cube spin / pickup state run free, so they do
not. Egypt's port phase 3 is 598 frames against the trace's 597 rows. **[UNCERTAIN]** whether consuming trace rows from
the port's phase-3 entry clears every RED row. Evidence missing: a table run with that alignment. `MASHED_CAM_REPLAY_GO`
(C32's alignment) sets the stand-in `countdown_`, which the sub-state path does not use, so it was not run here.

## 5. What `merge_train.json` needs for the flip pass (not applied, and no .ps1 change needed)

| key | now | flip |
|---|---|---|
| `gctrl.ref` | 5943c5f1 / 306186d4 / 52052890 | **8284be46 / ba407a16 / 31fe043e** |
| `b.ref` | 3a03670c / 9fcdbd3a / 53778664 | **1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9** |
| `full.d2_arms` | D2_DEF_1..3 | **D2_DEFH_1..3** (stack DEFH = `MASHED_DRIVE_HOLD_MS=42000`) |
| `full.d2_score` | verify/d2rebase_20261019/score_d2_v2.py | **verify/d2band42_20261029/score_d2_v2.py** (reads its own O42 band.json) |
| `full.run13_reference` | verify/pregocubeflip_20261029/tracks_score.txt | moves (580ab2ce on this exe), but only under the decision in section 6 |

**merge_train.ps1 does not need a change.** The hold reaches the arms through the arm name (stack `DEFH` in
`run_arms.py`). `Run-Arms` sets no wall, and `run_arms.py` now gives any D2 stack with `MASHED_DRIVE_HOLD_MS` above 30000
a 110 s wall when `A4_D2_WALL` is unset. The 13-track budget allowance lives in `run13.py` (default 800). The pass must run
with `-Full`. The default lives in `Core/FaithDefaults.cpp`, outside `full_set_paths`. Keep the old refs in `_doc` (the
protocol in MERGE_TRAIN.md "Decided default flips"). All values are in `expected_refs.json`.

## 6. Options (DECISION)

- **(a) recommended: a harness step before the flip lands.** Under `MASHED_CAM_REPLAY`, consume trace rows from the
  port's phase-3 entry, i.e. on the pre-race camera chain (`TrackRenderer.cpp:8281`) as well as in `RaceCamUpdate`, with
  the change knob-gated and diagnostic. Re-score the flip table. Then land the flip with that table as `run13_reference`.
  Expected: the copter and cube rows cite again or match. [UNCERTAIN] until run.
- (b) Flip now. Add a cite for the two families (helicopter MISSING / EXTRA and IconCube MOVED on these tracks, as
  instrument timing, evidence this RESULT) and set `run13_reference` to the flip table re-scored with it. This is faster,
  but it widens two cites on an instrument that a small fix may repair.
- (c) Keep MASHED_SUBSTATE default OFF (D-11073 stays open).

Train notes for whichever option lands: B collapses to one trajectory (1d5bf4f9 x3), and D2 has a 33-46 line margin over
`d2_min_lines` on a quiet machine but falls short under load (section 3).

## 7. Branch content

- `mashedmod/src/mashed_re/Core/FaithDefaults.cpp`: `SUBSTATE` and `P3_RESET_PAIR` seeded (opt-outs as above).
- `verify/savelink_20261023/run_arms.py`: a 110 s D2 wall for hold stacks, and stack `NOPGIC` (report only).
- `verify/rsky_20261016/run13.py`: `RUN13_PRE_FRAMES` (default 800).
- `verify/d3subflip_20261029/`: PREREG, this RESULT, `expected_refs.json`, `tracks_score_flip.txt`,
  `tracks_score_nosub800.txt`, `red_rows_flip.txt`, `SCORE_D2.txt`, `RUNLOG_gates.txt`, `RUNLOG_d2.txt`, `raw_sha256.txt`.
- Run 20's carried content: amendment 1 (on integration via PREGO-ALIVE), `MASHED_SUBFLIP_DIAG` (default OFF), runner
  stacks NOSUB / NOP3R / NCC / L697 / SFD. The D-11073 tracker rewrite is owed via re-classify once a flip lands.

## 8. Raw data (`D:/MashedWT/D3-SUBSTATE-FLIP_data/`, sha256 in `raw_sha256.txt`)

Exe `mashed_re_run22_34b53a28.exe`. G-CTRL / B arms are in `run22/arms/`. `H_NOPGIC` and every D2 arm are in
`run22/arms_b/`. 13-track flip output is in `run22/tracks/`, the opt-out control in `run22/tracks_nosub/`, and the first
(stock-budget) attempt in `run22/tracks_try1/`. Wrapper logs: `run22/*_run.log`, `run22/d2_rerun*.log`.

## 9. Process

Every run went through `test-slot` and `mashed_lock.py run` (D2 with `--solo`). Every mashed_re PID was spawned by this
step's runners. Two kills, both of this step's own PIDs: run13's python (15328) after Egypt had failed on the stock
budget (its game child had already exited), and each D2 arm at its 110 s wall (the recipe). No MASHED.exe was launched,
no audio endpoint check was needed, `original\` was not touched, and nothing was deployed.
