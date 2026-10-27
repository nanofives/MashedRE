# RESULT D4-FX-REBASE: original FX references re-baselined on the port's recipe (2026-10-27)

**Verdict: GREEN.** No port change. On the matched recipe (`--poke-ctrl-slots --statediff-out`, no pulses) the port's FX
rate is 0.83x to 1.18x of the original's 3-rep mean on all four cars (5-rep mean: 0.83x to 1.15x). The old 0.64x (slot 1)
and 0.26x (slot 2) ratios came from the unpoked, pulsed recipe and are retired. The gate inputs pair almost frame for
frame. The car 1 state-7 FX UNCERTAIN in `RESULT_D4-FX-SLOT2` is narrowed: the original's FX gate has no speed test
when `rec+0xcfc != 0`, and fA's car 1 matches that branch's p 0.7 to 4 digits (0.7008). What stays open is why car 1's
`+0xcfc` stayed nonzero in that one rep (section 4).

Branch `orch/D4-FX-REBASE` off `orch/integration` @ `db104ccf6` (`git merge-base HEAD orch/integration` = `db104ccf6`).
Private root `D:\MashedWT\D4-FX-REBASE_root`. Every game run (3 original reps, 3 port arms) went through
`proyectos_test_slot.py` and `mashed_lock.py run --label D4-FX-REBASE`, and the launchers killed only the PIDs they
spawned. No U-9677 crash signature in any run. Exe built with `MASHED_NO_DEPLOY=1` (sha8 `21ee1d92`), nothing deployed.

## 1. Capture

`orig_fx_rebase.py` runs the three committed probes in one original run. Their CModule and JS are loaded verbatim from
the committed scripts via `ast`, so the counters are the same code:
A = `verify/fxslot2_20261027/orig_fx_gates_count.py` (FX calls / emits, alive split, the `fx_score.txt` inputs),
B = `verify/fxinputs_20261026/orig_fx_inputs.py` (gate input census, the `score_orig*` inputs),
C = `verify/fxslot2_20261027/orig_alive_series.py` (per-frame state / alive / slot / AI calls, kill events).
A new probe D writes one row per `FUN_0040ca00` call with, per car, `|rec+0x9b0|^2`, `rec+0x958` / `+0x960`,
`rec+0x9e0`, `rec+0xcfc` and ctrl bytes 0..7 of the slot's block. Launch: `scenario_launch.py --track 0 --mode 10
--cars 4 --car 1 --rule 0 --hold 240 --poke-ctrl-slots --statediff-out <msd>`. Every rep logs
`[ctrl-slots] ctrl slot table was [0,0,0,0] -> [0,1,2,3]`. Under `--statediff-out` the launcher sends no pulses
(`scenario_launch.py:5455`). The "pulsing control 4" banner at `scenario_launch.py:5400` prints either way.

Port: `run_arms.py H_FXD` = today's default (orig gates, dead coast, kill damage ON) + `MASHED_FX_DEBUG`. The H_FXD
`fx_debug.txt` is byte-identical to the D4-FX-INPUTS `H_Y1.fx_debug.txt` (SHA-256 `7476bd32...` on both), so
today's exe gives the same FX as the arm SLOT2 scored.

## 2. Re-baselined FX rates (`fx_rebase_score.txt`, `fx_rebase_score_5rep.txt`)

Emits per 1000 active frames (active = non-skid calls + skid calls / 3, the D4-FX-GATES denominator). r1..r3 are this
step's reps. fA2 / fA3 are D4-FX-SLOT2's matched-recipe reps, same recipe and same probe A.

| car | port | r1 | r2 | r3 | fA2 | fA3 | band r1..r3 (mean) | band 5 reps (mean) | port / mean 3 / 5 |
|---|---|---|---|---|---|---|---|---|---|
| 0 | 55.2 (308) | 42.3 (307) | 42.3 (307) | 55.4 (307) | 71.7 | 52.1 | 42.3..55.4 (46.7) | 42.3..71.7 (52.8) | 1.18 / 1.05 |
| 1 | 106.6 (394) | 130.4 (391) | 117.6 (391) | 138.1 (391) | 130.4 | 125.3 | 117.6..138.1 (128.7) | 117.6..138.1 (128.4) | 0.83 / 0.83 |
| 2 | 55.3 (416) | 33.9 (413) | 43.5 (414) | 60.5 (413) | 41.2 | 60.7 | 33.9..60.5 (46.0) | 33.9..60.7 (48.0) | 1.20 / 1.15 |
| 3 | 215.5 (232) | 219.8 (232) | 232.8 (232) | 181.8 (231) | 258.6 | 258.6 | 181.8..232.8 (211.5) | 181.8..258.6 (230.3) | 1.02 / 0.94 |

- Active frames agree to 1..3 frames on every car in every rep. Skid share of calls: port 0.000 / 0.274 / 0.113 / 0.651,
  original 0.000 / 0.260..0.265 / 0.108..0.114 / 0.649..0.651.
- Cars 0, 2, 3 are inside the original's 5-rep band. Car 1 (106.6) is 0.91x of the band's lowest rep (117.6). Its skid
  share and active frames match, so the gap is in emits per call (port 0.087, original 0.097..0.114). That is 42 port
  emits against 46..54 per rep in the original.
- Alive / dead split (3 reps): alive port / mean 1.62 / 0.86 / 0.95 / 1.02. Dead 0.63 / 0.70 / 1.86 / -. These cells
  have 12 or fewer emits per rep. The original's own spread across 3 reps (for example car 2 dead 15.5..36.1) is the same
  size as the gaps.
- Replaces: `verify/fxgates_20261026/fx_score.txt` original column (266.9 / 166.1 / 215.3 / 191.0, unpoked + pulsed,
  1 rep). Port per 1000 active frames was 59.0 / 52.4 / 49.1 / 215.5 (F1, pre-dead-coast) and is now 55.2 / 106.6 /
  55.3 / 215.5. The old file stays as history and should not be read as a port comparison.

## 3. Re-baselined gate inputs (`inputs_r1.txt` with the port, `inputs_r2.txt`, `inputs_r3.txt`)

`fx_inputs_score.py` on probe B. Replaces the original columns of `verify/fxinputs_20261026/score_orig1.txt`,
`score_orig2.txt`, `score_killdamage_Y.txt` and `score_deadcoast_X.txt` (slots `[0,1,-1,-1]` there, the shared-block
recipe).

| car | outer pass (cfc) orig r1 / r2 / r3 | port | skid frames orig r1 / r2 / r3 | port | legA b0c>100 orig / port | legB rng&brk orig / port |
|---|---|---|---|---|---|---|
| 0 | 307 (0) / 307 / 307 | 308 (0) | 0 / 0 / 0 | 0 | 0 / 0 | 0 / 0 |
| 1 | 380 (16) / 381 (16) / 380 (16) | 381 (16) | 42 / 41 / 42 | 44 | 0 / 0 | 41..42 / 31 |
| 2 | 409 (5) / 409 (5) / 409 (5) | 411 (5) | 16 / 17 / 16 | 17 | 16..17 / 12 | 8..9 / 4 |
| 3 | 231 (1) / 231 (1) / 231 (1) | 231 (1) | 89 / 89 / 88 | 89 | 15 / 15 | 83..84 / 83 |

The port's `9e0<=1` class has 180 frames, the original's 0. Outer pass and active frames still agree to 1..2 frames. Probe B's frozen-velocity counts (20 / 0 / 29 / 8) are equal on both sides.

Slot 1's old 0.64x: on the shared block, every car's brake byte was block 0's (`0x0040cb37`), so car 1's legB came from
another car's input. On the matched recipe car 1's legB is 41..42 frames and the port's is 31, while the skid totals are
42 / 41 / 42 against 44. [UNCERTAIN] how the port's 44 splits between legB and the `+0xcfc` roll: the port's census
line has no per-leg split of its `+0xcfc` frames.

## 4. The car 1 state-7 UNCERTAIN (RESULT_D4-FX-SLOT2 section 3): narrowed

**State 7 on the matched recipe (probe D, `series_r123.txt`).** All 3 reps take the same path. State 6 lasts 220 frames
with identical per-car counts in every rep. Car 3 is the only alive car in state 7 (11,792 / 13,039 / 13,114 frames).
- The AI step zeroes throttle and brake outside states 5 / 6 / 9 / 10 / 11. `FUN_00416250` reads
  `FUN_0040e350()` (= `DAT_0063ba8c`, body `0x0040e350`) into `[esp+0x24]`, compares it with 6 / 5 / 9 / 0xa / 0xb
  (`0x00416a03..0x00416a1e`), then `mov byte [ebx+4], 0` at `0x00416a20` and `mov byte [ebx+5], 0` at `0x00416a24`.
  `FUN_00418560` zeroes ctrl bytes 4 / 5 / 0 / 1 / 7 / 6 before the step (`0x0041857d..0x0041858b`). Measured: car 3's
  ctrl byte 4 is 0 on every state-7 frame in all 3 reps.
- Car 3's position (+0x958, +0x960) never changes in state 7 (6.07, -22.09). |v| decays from 1712.01 by about 0.84x per
  frame (1712.01, 1435.80, 1204.43, ...), so it passes the 62,500 gate (`0x0040cae0`, [0x005ccd20]) on 12 frames.
  U-9781 records the same freeze and the 0.85 velocity scale (`FUN_0046c570`, pc `0x0046c591`).
- Dead cars 1 and 2 keep a stale ctrl byte 4 = 0xff on every state-7 frame (`FUN_00418860` calls `FUN_00418560` only
  when `FUN_0046c7b0(v) == 1`). Their `+0xcfc` is 0 on every state-7 frame, because byte 5 is 0.

**The speed gate is not the gate for fA's car 1.** `FUN_0040ca00` tests `rec+0xcfc` before the speed test:
`cmp dword [ebp], 0` at `0x0040cabe`, `jne 0x0040cb42`. On that branch there is no speed test: `FUN_00472650` rolls
(`0x0040cb49`), `fcomp [0x005cc99c]` (0.3) at `0x0040cb4e`, no call if roll < 0.3, else skid (`mov edi, 1` at
`0x0040cb62`, then three `FUN_0040c690` calls). `rec+0xcfc` is written by `FUN_0046e9e0`: 0 at `0x0046eaf2`, then 1 at
`0x0046eb0c` when the input block's byte 4 (`0x0046eafc`) and byte 5 (`0x0046eb03`) are both nonzero.
fA's car 1 after t=20 s (`fa_cfc_check.txt`): 26,325 FX calls, **all skid** (0 non-skid), 8,775 skid frames in 12,521
frames = **0.7008** of frames. The `+0xcfc != 0` branch predicts 1 - 0.3 = 0.7 with no speed dependence. The
`+0xcfc == 0` path would need |v| >= 250 on 70% of frames while ctrl byte 4 is 0. So no throttle is needed:
car 1's `+0xcfc` held nonzero for the rest of fA.

**Still open.** [UNCERTAIN] why car 1's `+0xcfc` stayed nonzero through fA's frozen phase. Candidates in the code:
`FUN_00470c70` passes A4 (`FUN_00470670`, call at `0x00471076`) the block `0x007f19b8` instead of the slot's when
`FUN_0040e350()` is not 6 / 0xb / 0xa (`0x00470fdb..0x00470ff9`), and skips the A4 call on its record gate at
`0x00471004..0x00471019`. If `FUN_0046e9e0` is reached only through A4, a skipped call leaves `+0xcfc` at its last
value. Not measured: whether `FUN_0046e9e0` sits under A4, and fA's per-frame `+0xcfc` / alive / ctrl for car 1.
fA had probe A only, and fA's race diverged from frame 0 (car 0 561 active frames against 307 in all 5 matched reps).
Probe D logs `+0xcfc` per car per frame, so a matched-recipe rep that again ends with car 1 gated would answer it.
0 of 5 matched reps reproduced fA. It does not touch the port comparison: the port's run takes the 5-rep path
(car 3 survivor, `+0xcfc` 0 in state 7).

## 5. Change, gates, trackers

- No source change. Files added in this folder: `orig_fx_rebase.py`, `run_reps.sh` (rep driver),
  `run_arms.py` (copy of `verify/fxinputs_20261026/run_arms.py` plus an `FXD` stack), `rebase_score.py`,
  `series_score.py`, `fa_cfc_check.py`, and the summaries `fx_rebase_score.txt`, `fx_rebase_score_5rep.txt`,
  `inputs_r1.txt`, `inputs_r2.txt`, `inputs_r3.txt`, `series_r123.txt`, `fa_cfc_check.txt`.
- Gates on this exe (21ee1d92): H_DEF G-CTRL `5943c5f1 / 364350ae / 52052890` = `merge_train.json` `gctrl.ref`. H_FXD
  gives the same three. B arms not run: no source change on this branch, so the exe is the integration tip's.
- No PREREG and no knob: there is no port fix.
- Trackers: no U-row covers the FX re-baseline or the section 4 residue (SLOT2 filed none). The [UNCERTAIN] in
  section 4 needs a row via `re-classify` if the lead wants it tracked.

## 6. Next

1. FX gate parity is closed at this resolution: all four cars within 0.83x..1.18x of the original mean, with gate inputs
   paired. Car 1's emits per call (port 0.087, original 0.097..0.114) is the one cell outside the original's band. The
   next lever is the wheel-colour mix behind `FUN_0040c690`'s roll threshold (D4-FX-GATES section 1 UNCERTAIN: the port
   has no colour histogram).
2. Section 4 residue: an entry probe on `FUN_0046e9e0` / `FUN_00470670` per car in state 7, and more matched reps until
   one ends with car 1 gated.

## Raw data (D:/MashedWT/D4-FX-REBASE_data, not committed), SHA-256

```
acf3eb8cb8323cacaec7fa1dbf6b8af389a12206e67bc462264367d63e932d9c r1.A.json
51b35492db917f26d4a619988b437111a19c0297f909d3ab78ef97c3eecf56c9 r1.B.json
9801ed16ea4f6babb7375c0ffc5930aadf3d1c34f3b0cf79c4ba3031580f908b r1.C.json
8bf4f4b96fadd6944ad2b63930bea72bb174c0a42c3dec402d1e38f7c569b604 r1.C.rows.bin
e573354d8a265c9dd0e0ab23594857df356be2ee4587ee65f7a7fb138737e59b r1.C.events.json
7d87f3b60172b6c6af422969b87a8f3d2115b678e4b0cf77b4bbcc84b77323e1 r1.D.rows.bin
589cdf9ae09cb94fcef1d480b19e286f99a79c4d071322e9278e27698010a799 r1.msd
ed1343db197306d76015df92a1dc47e5ca0f38b8228997e3d6009f2d09f43be1 r2.A.json
be55a04c22bb81b0fdb9538fc7294704080b693dc650192757b605bf2e28841c r2.B.json
632cdbc40995f9bbb57104095af511012ad96ea68dcfcdd79afeb0d1797efebc r2.C.json
47451ee309db0fcc0eef95e701e49b13c0c4aa3164248fe5a2c9fd1f8bd783b2 r2.C.rows.bin
935be869725a81cb44e5d1296ef40322f970961e840a6838d2fc5d13d653188f r2.D.rows.bin
2c49c3efd469138e11d5c2e780824fb0668930c808b22e97de0b5ab27c1156fd r2.msd
ff2c1107d288e9b5ec6802c4ffa7148125abaf09d811d8a31531cb0448ec5466 r3.A.json
3484db7284dba33592264fbe12f99b8face05a05fe33cd3492db91695e753838 r3.B.json
b68a5b1e9e5bdbba2a38503242c50ad25115a4b815d9620e2f64a11eb565e562 r3.C.json
69620c3529447b821cecbafa8a392f9cea877c69a0d7edd3862ee0bf6ea35651 r3.C.rows.bin
e553b8f0ecf958d6f42b393b3f0d7bd56c980c04717935ee70f6c796cfaf8582 r3.D.rows.bin
963f3cdedda0e8d7f6757f3ee39249935b92f600a3a9a99a45facd15c6b72479 r3.msd
87c89d531a16fe7f4843e217677d0e745f478760d691b6d0a505883392d7d00e r1_run.txt
f5ee8b596706610f892acf7e5018aa0fef64e6ba71f9f6d44ec7c527bad30a4c r2_run.txt
f80edbe00a62d9f4db924ace2c604f37cdabfbb6ba3a1b57a24adee261570851 r3_run.txt
7476bd32e1c8ff6f8c3cbf0136f291a35375fee619e4ee227c3f259d0a88c609 arms/H_FXD.fx_debug.txt
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms/H_FXD.step.csv
5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 arms/H_DEF.step.csv
364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 arms/H_DEF.gates.csv
520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 arms/H_DEF.ptrace.log
c30f06901c302d4dcd3df188eca521839675cf68f57591be79ed2c6820426178 arms_run.txt
2c47e065881333b89ea212830042ffb978473c02af3949b7da305ff208a09b70 arms_run2.txt
88baba6e972c145579e83d42b661373c7b8ff9ce40d68e135909450c2b865622 decomp1.txt   (decomp_pc.py 0x00416250 0x00418560 0x00410860 --callers)
5c63b4e744b94385dcfc1c7f0c51c329f28f1148c9eecdea41e1f03ae5479fdd decomp2.txt   (decomp_pc.py 0x00418860 0x0040e350 0x00417640 0x0040e4a0 --callers)
4ac57c5918271a30c80147ed473e962e8b7442b0486927ef5e135dcb30dcf0f4 dis_416250_tail.txt
fb99a5ddac0b9fa0e7a3a8b0f374a2fdb158e760cb77a0b898d2957c69d81c0f dis_40ca00_gate.txt
f05d9843385b95809b80b4dc849fd2dcbf47305a1cd1a983f9bcd6d5c9b48d1e dis_46e9e0.txt
57916adf7372293746f06a56a9b4dd78899a286edb14c3fd0f14e5b17ce588f5 dis_470c70.txt
b5ece79b6f3850d71781c1af57ea23e6d7ba38b3f67de0ab8d006123496062f3 D:/MashedWT/D4-FX-SLOT2_data/fA.json
cd72d3f1bdbae26d7799e37649c12eeb86c89ba96def14b8a5f07a4653c04532 D:/MashedWT/D4-FX-SLOT2_data/fA2.json
a9587ada64e2e5a6e41171bf35938446e94e89050fd0b0b7f662c3467d7c91cb D:/MashedWT/D4-FX-SLOT2_data/fA3.json
bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e D:/MashedWT/D4-FX-REBASE_root/original/MASHED.exe.unpatched
```
Disassembly: `verify/fxslot2_20261027/capdis.py` (capstone) on the unpatched image above. Decomp: `re/tools/decomp_pc.py`
from the main checkout (read-only pool slot `Mashed_pool10`).
