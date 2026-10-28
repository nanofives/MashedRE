# RESULT D5-ICONCUBE-SPIN (D-11076 cube half, L-45)

## Run 19 (2026-10-28): pre-GO pickup tick and replay GO alignment

PREREG `verify/iconcube_20261028/PREREG2_D5-ICONCUBE-SPIN.md` (`17d522868`, committed alone), amendment A
(`f1c1c795d`, committed alone before R5). Code `3fc3bda01` + `744ffe379` on top of the merge of `orch/integration`
@ `0d1285dc7` (`27e7d6c62`). R1..R4 ran on `mashed_re_r19.exe` (`5c9b5104`), R5 and the final gates on
`mashed_re_r19b.exe` (`403c37d5`, the committed source). Full cube table and pickup events:
`verify/iconcube_20261028/cube_cmp_rows_r19.txt`.

**Verdict: RED.** The draw side closes: with the pre-GO tick and the replay GO aligned (R5), every cube on rows
884 / 924 / 959 has the original's texture, set, order, translation (<= 4.04e-8) and angle (dtheta 0.0000 on all
6 cubes). But the behaviour knob that gets there, `MASHED_PICKUPS_PREGO`, changes the AI's first racing tick in a
way the original's captures do not show (G-PG-SIM, section R.3). So the knob is not flip-ready, and the cube's
spin stays 178 ticks behind the original on the default build. Default gates unchanged on both exes.

### R.1 Cause of run 18's offsets (static and trace)

- The original ticks the pickups before GO. `FUN_0040fc00` calls `FUN_0040da50` (`0x0040fc75`), `FUN_0045bba0`
  (`0x0040fcd0`) and `FUN_00485070` (`0x0040fd07`) with no guard, and `FUN_0045bba0` calls `FUN_00459000` with no
  guard (return `0x0045be76`). The stand-in countdown returns before `UpdateRace`, the only default call site.
- The d1r1 trace has 854 pre-GO rows (`ba8c` 3 on rows 0..740, 4 on 741, 5 on 742..853, 6 from 854). The
  standalone's 3 s stand-in countdown puts its GO at row 179. So the replay's standalone sim at row 884 was about 705
  frames after GO and the original's 30. That is why car 2 collected the mine at pickup tick 52 standalone (row 231)
  and nothing in the original before row 884.
- `ff4` (DAT_007f0ff4, `+= units` per FUN_0040fc00 call) reads 0 on rows 0 and 1, then +50 on every row: 883
  calls by row 884, 923 by 924, 958 by 959. The original's angles are exactly those counts mod 360
  (163 / 203 / 238 deg).
- **`MASHED_CAR_REPLAY` does not feed `FUN_00458920`** (run 18's [UNCERTAIN], answered). It writes only the
  render matrices (`rep_car` in `TrackRenderer::Render`). `CarsReg_FrameTick` (`Race/CarRegistrant.cpp`) copies each
  car's own record `+0x958` into registry `+0x258`, so the pair test reads the standalone's simulated cars.

### R.2 Replay arms (Training, `--shots 884,924,959 --frames 1450`, all with `MASHED_ICONCUBE=1`)

| arm | knobs | dtheta mine / mortar / oil (row 884) | set 884 / 924 / 959 | mine collected (pickup tick, car) |
|---|---|---|---|---|
| R1 | - | -119 / -178 / -178 | same / extra mine / extra mine | 52, car 2 |
| R2 | PREGO | +61 / +2 / +2 | same / extra mine / extra mine | 232, car 2 |
| R3 | PREGO + REPLAY_GO | +2 / +2 / +2 (every row) | same / same / same | 906, car 2 |
| R4 | REPLAY_GO | -132 / -132 / -132 (every row) | same / same / same | 52, car 2 |
| R5 | PREGO + REPLAY_GO + amendment A | **0 / 0 / 0 (every row)** | **same / same / same** | 904, car 2 |

- P-R1 PASS (run 18 repeats to the degree). P-R2 MISS by one tick (c = +2, predicted |c| <= 1). P-R3-SET PASS.
  P-R3-SPIN FAIL strict (c = +2 constant). P-R4 PASS (c = -132, predicted -132 +/- 1). **P-R5 PASS** (c = 0.0000).
- R5 checks the `ff4` attribution: skipping the pre-GO tick on the two rows without a FUN_0040fc00 add takes c from
  +2 to 0 on all 6 cube draws. R3 vs R4 splits the two causes: the pre-GO tick closes the spin, and the GO alignment
  moves the mine's collection to between rows 884 and 924, as in the original.
- Unchanged from run 18: `rs` element 12 (D3DRS_LIGHTING) reads 1 on every standalone draw (librw vertex shader,
  scene-wide). The other 11 elements are equal.

### R.3 Gates

`verify/iconcube_20261028/run_arms.py` (new stack `PG` = `MASHED_PICKUPS_PREGO`), SHA-256 first 8 hex, refs from
`re/orchestrator/merge_train.json` (G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`).
`RUNLOG_gates_r19b.txt` is the final exe's log. The r19 exe gave the same 16 hashes.

| arm set | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs ref |
|---|---|---|---|
| knob OFF (`H_DEF`, `B_DEF_K*`) | 5943c5f1 / 364350ae / 52052890 | 3a03670c / 9fcdbd3a / 53778664 | equal |
| `MASHED_PICKUPS_PREGO` (`H_PG`, `B_PG_K*`) | 67763abd / 5250011e / 6457cb73 | b9f43d6b / 74c23a4b / 079a708f | differ |

- **G-OFF PASS** on both exes.
- **G-PG-SIM FAIL** (the PREREG predicted no change). First difference: step row 1, frame 0, the first racing AI
  tick. With the knob, cars 2 and 3 read `ai_mode` 7, `c4` 64, look (1.88176906, -3.09718895), which is pool entry 4,
  the mine. Knob OFF: mode 0, `c4` 255, look (-1.41216886, -10.0070553) for car 2. Before GO no car moves and no AI
  ticks, but the registry published during the countdown is still there at the first AI tick, so it has a reader. The
  step dumps gain rows (+62 on H and K0, +206 on K1, +16 on K26).
- **The original does not do this.** `verify/d3bB2_20261017/o_c2r1.msd.aistep.csv` and `o_pw1.msd.aistep.csv`
  (original, same Training grid, car 2 own (1.1367228, 0.20235741)) read car 2 mode 0, look (-1.41216886,
  -10.0070553) on the first tick, the knob-OFF value. Across both captures, cars 2 and 3 never enter mode 7. Car 1
  enters it at relative frame 913 / 944, on the oil (-1.00933, -20.2196). The standalone default does the same at
  frame 100. [UNCERTAIN] (no U-row filed: tracker rows go through re-classify, candidate for the lead) The pre-GO publish runs in the original too (decomp above), but its AI does not take
  the mine on the first tick. The evidence still missing is which gate keeps it out: the original AI's pre-GO ticks,
  which the standalone does not run, or the mode-7 held-word gate.

### R.4 Raw data (`D:/MashedWT/D5-ICONCUBE-SPIN_data`, not committed)

| file | SHA-256 |
|---|---|
| mashed_re_r19.exe | 5c9b5104e7ebb658395f6fe88ced0e07b05916a5198a20d444d4e1b081c8b35f |
| mashed_re_r19b.exe | 403c37d537d830a0316b05b0e6be8b4c513c6c073f4ad616abf138907e8faa62 |
| r19/R5_pkcb.tsv | 660e86b3ef4be85b8ae670d91e680357883c138faaaff5451bbe0796251da62a |
| r19/R5/d1r2/stand_row_00884.draws.json | 0956b2ed05e7788bc3d683cfb64085383c72e727bfc6bc77ccbc48384fe66472 |
| r19/R5/d1r2/stand_row_00924.draws.json | fcda03dd26890ec0094cf105a99c4d1a7850b11dbf603b6e7e4b07a6be08c8cf |
| r19/R5/d1r2/stand_row_00959.draws.json | e6170cb1750448ea53b029ae6f3d52166bb9f673911648b128178429a8801828 |
| arms19b/H_DEF.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms19b/H_DEF.gates.csv | 364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 |
| arms19b/H_DEF.ptrace.log | 520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 |
| arms19b/B_DEF_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| arms19b/B_DEF_K1.step.csv | 9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f |
| arms19b/B_DEF_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |
| arms19b/H_PG.step.csv | 67763abdf9bc6c12ade8a356946e78b41f70f001b2dfeeb380bc3dc7f1c1f16e |
| arms19b/H_PG.gates.csv | 5250011e30e3b7cf0bfd00bc3909fb2d3a8aa52b90b527c5b95533259c0ef47f |
| arms19b/H_PG.ptrace.log | 6457cb73f45c4f383aea0631727274fd009a2a7769e2c337a6b8389de225270b |
| arms19b/B_PG_K0.step.csv | b9f43d6b36644bd934fa58adf7f20effd1ccb36e2309c5c1e888d383346711c9 |
| arms19b/B_PG_K1.step.csv | 74c23a4b66fdaaad8d5a4f875553dbd210aec54acfb6f85d6e34ff2d9f8c2f94 |
| arms19b/B_PG_K26.step.csv | 079a708f09f4c613b53a2a89ed3330fd4e23d6fac6d7be906d9ab938b3d36ff7 |

R1..R4 draws and pickup traces sit next to R5 (`r19/R{1..4}`). Run scripts: `run19.sh`, `run19b.sh` (copies committed).

### R.5 Next

- Find why the original's AI does not take a pre-GO-registered pickup at its first racing tick. Then make
  `MASHED_PICKUPS_PREGO` leave G-CTRL / B on the original's first-tick values, or narrow it. Inputs: the original's
  AI on pre-GO ticks (749 probe calls before the window on `o_reg_r13`), and the mode-7 held-word gate.
- Once that holds, `MASHED_ICONCUBE` + `MASHED_PICKUPS_PREGO` ON is a decision row (draw exact per R5).
- `MASHED_CAM_REPLAY_GO` stays a diagnostic. It makes the d1r1 replay's sim start at the trace's GO, which any later
  sim-dependent draw comparison on this trace needs.

---

## Run 18

Branch `orch/D5-ICONCUBE-SPIN`. PREREG `verify/iconcube_20261028/PREREG_D5-ICONCUBE-SPIN.md` (`5a83d6987`,
committed alone). Code `14bb50f2d`. Tested exe `mashed_re.exe` built from `14bb50f2d` source,
SHA-256 `0da0a1a885c63bdce1ec8c1dd1b024e44c9defd70835278aa6f48ea7467f0ab3`.

**Verdict: RED against the PREREG's strict predicates.** The draw port is exact on its own inputs, but two
pool-history differences upstream of the draw fail G-DRAW-ON (2 of 3 rows) and P-SPIN.
Both are sim-side (pickup collection and pickup-tick start), not the cube draw. `MASHED_ICONCUBE` is default
OFF, and with the knob OFF the draw list has no cube rows and no change. Gates: section 3.

## 1. What changed

- `D3d9Render/PickupIconCube.{h,cpp}` (exe-only, in `exe_sources.rsp`). `IconCube_OnTrackLoad`
  writes FUN_004587a0's `+0x08` = 1.0 and `+0x0c..+0x14` = (0, 1, 0) on the 25 pool entries. It registers
  `ICONCUBE.DFF` (Common/POWERUPS/Powerups.piz, one geometry, one material, flags 0x0f: prelit, no
  normals, no rpGEOMETRYLIGHT) once per FUN_00458630 name, plus an untextured copy, with
  `POWERUPICONS.TXD`. `IconCube_Submit` is FUN_00458b10's loop: i = count-1 down to 0, `+0x20 == 1`,
  `dot(d, fwd) > 0`, `|d|^2 < 800.0` (`[0x005ce5d4]`, read from the binary). The pose is identity +
  `+0x38` translation, then `RwMatrixRotate(+0x0c, +0x04, PRECONCAT)` (`0x004c4d20` port), as FUN_00458a40
  leaves the frame (`0x00458a8e`, `0x00458aac`). It queues with ZWRITE 1.
- `LibRw/RwRaceSubmit`: new kind `kInstAfterFx` (4), drawn in a second pass after
  `RaceFx_DrawWorldFx`. The after-world loop now runs twice. Pass 0 skips kind 4 and pass 1 draws only
  kind 4, so with nothing of kind 4 queued the sequence is the old one.
- `D3d9Render/TrackRenderer.cpp`: the two calls (after `LoadCopters`, after the slot-10 queue).
- It makes no RNG call and reads the pool only. Its writes are FUN_004587a0's own `+0x08..+0x14`, which
  feed only `+0x04` (read at `0x0045920d` into a sprite no-op) and `PK_FrameRotate` (counted only).
- Static facts used (decomp `decomp_pc.py`, constants from `MASHED.exe.unpatched`): `0x005ce594` = "mine",
  `0x005ce4fc` = "oil", `[0x005d757c]` = 0.0, `[0x005cc9dc]` = 0.95. FUN_00459000 calls FUN_00458a40
  only for state 1 (`0x0045904a` `dec eax / je 0x0045912b`, call at `0x004591e8`), so `+0x04` counts that
  entry's live ticks in degrees.

## 2. Draw comparison (Training, d1r1 trace, existing original records)

Tool `verify/iconcube_20261028/cube_cmp.py`, full table `cube_cmp_rows.txt`. Standalone replay
`run_replay3d.py` (copy of the d1r1 runner with the private root), `--shots 884,924,959 --frames 1450`.

| row | original cubes | knob OFF | knob ON | ON vs original |
|---|---|---|---|---|
| 884 | mine, mortar, oil | none | mine, mortar, oil | same set and order, dpos <= 4.0e-8, rs 11/12 |
| 924 | oil | none | mine, oil | extra mine |
| 959 | mortar, oil | none | mine, mortar, oil | extra mine |

- **G-DRAW-OFF PASS**: no cube draw on any row with the knob OFF.
- **Texture and placement**: every ON cube carries its entry's FUN_00458630 texture. Translation equals the
  original's to 4.0e-8 on every paired cube, and the matrix is a pure Y rotation (off-axis terms 0).
  FVF 322 is the same. The cubes draw after `RWObjShad` (row 884), as in the original (after RWObjShad /
  smoke / exp_cloud2 / PUGlow).
- **rs**: 11 of 12 equal. Element 12 (D3DRS_LIGHTING) reads 1 on the standalone. Every standalone draw
  in these records reads LIGHTING 1 with `vs` 1 (librw draws through its vertex shader, which ignores that
  fixed-function state; RWObjShad shows the same). The cube geometry has no rpGEOMETRYLIGHT, so BuildClump
  sets no `Geometry::LIGHT` and the shader uploads the prelit unmodified. This is scene-wide, not
  cube-specific. Texture format `:21` vs `:23` is scene-wide for the same reason.
- **G-DRAW-ON FAIL on rows 924 / 959**: the extra draw is pool entry 4 (mine). Mortar is missing on 924
  on both sides with the same camera, so that is the cull. Entry 4's state differs (next point).
- **P-SPIN FAIL vs original, law exact**. Original: all three cubes at theta = 163.000 (row 884 frame 0),
  164.000 (frame 1), and oil 203 / 204 at row 924. That is 883 live ticks at row 884 on every entry.
  Standalone: mine 44, mortar / oil 345 at row 884. From 884 to 924 to 959 every visible standalone cube
  advances exactly 1 deg per row (mine 44 / 84 / 119, oil 345 / 25 / 60, mortar 345 / - / 60, so mortar
  was live through 924 and culled). So the offset is not constant: mine -119, mortar / oil -178.

## 3. Gates

G-OFF and G-ON-SIM: `verify/iconcube_20261028/run_arms.py` (copy of the merge train's runner
`verify/savelink_20261023/run_arms.py`, plus stack `IC` = `MASHED_ICONCUBE`), SHA-256 first 8 hex as
merge_train.ps1 `Sha8`. Refs from `re/orchestrator/merge_train.json`: G-CTRL `5943c5f1/364350ae/52052890`,
B `3a03670c/9fcdbd3a/53778664`.

| arm set | G-CTRL step / gates / ptrace | B K0 / K1 / K26 | vs ref |
|---|---|---|---|
| knob OFF (`H_DEF`, `B_DEF_K*`) | 5943c5f1 / 364350ae / 52052890 | 3a03670c / 9fcdbd3a / 53778664 | equal |
| knob ON (`H_IC`, `B_IC_K*`) | 5943c5f1 / 364350ae / 52052890 | 3a03670c / 9fcdbd3a / 53778664 | equal |

- **G-OFF PASS** and **G-ON-SIM PASS**. The six knob-ON files are byte-identical to the knob-OFF ones on the
  full SHA-256 (section 5), so the cube draws and FUN_004587a0's pool writes feed nothing back into the sim
  and draw no random numbers on the G-CTRL (14400 frames) and B recipes.

## 4. Attribution of the spin and set differences (pool trace)

One more knob-ON replay with `MASHED_PKCB_TRACE` (`--shots 884 --frames 960`); its row-884 cubes repeat the
first ON run exactly (theta 44 / -15 / -15, same translations). Extract `pkcb_extract.txt`:

- SEED at pickup tick 1: 5 entries, types 19, 7, 19, 9, 12, each respawn time `0x40a00000` (5.0).
- PK at tick 52 and ST at tick 53: entry 4 (type 12, mine) goes 1 -> 2 (collected by car 2). ST at tick 353:
  back to 1 after 300 ticks (summed timer 5.000000).
- Mine therefore has 300 fewer live ticks than mortar / oil. 300 = -60 mod 360, which is the observed
  44 - 345 = +59 deg (one tick at the state boundary). Mortar and oil were never collected, so their
  angle 345 (mod 360) is the standalone's pickup-tick count at row 884: 345 or 705. The trace's CNT row at
  tick 600 inside a 960-frame run rules out 345 (it would reach 421 by frame 960), so 705 against the
  original's 883. The standalone's FUN_00459000 starts 178 frames later than the original's in the replay's
  frame index.
- The original shows no collection of entry 4 before row 884 (theta equal on all three). Between 884 and
  924 its mine disappears where the standalone's stays, with the same camera. So the original's entry 4 is
  not live at 924 / 959. The standalone collects on its own schedule (car 2 at tick 52, nothing near 884).
  [UNCERTAIN] Whether `MASHED_CAR_REPLAY` feeds the car positions the registry's pickup callback
  FUN_00458920 reads was not checked; that is the evidence needed to say why the schedules differ.

Both causes sit upstream of this step: when FUN_00459000 first runs (race-start frame alignment) and which
car collects which pickup when (the sim, not the replay). The cube draw reproduces the pool it is given.

## 5. Raw data (not committed, `D:/MashedWT/D5-ICONCUBE-SPIN_data`)

| file | SHA-256 |
|---|---|
| mashed_re_iconcube.exe | 0da0a1a885c63bdce1ec8c1dd1b024e44c9defd70835278aa6f48ea7467f0ab3 |
| rep_off/d1r2/stand_row_00884.draws.json | af3697d2ffe92b390a672c82a8c5b9a976bf4943ed9e5c88862cf9b1038e3f4e |
| rep_off/d1r2/stand_row_00924.draws.json | dc1d5bf7ea6b443432833e64bd02339f8014b4930f42c94e8fff208761537be0 |
| rep_off/d1r2/stand_row_00959.draws.json | c40e818961c1fd302d402f26c03e1d15efa64cf8b52bfd4b836f298ee90cea3a |
| rep_on_r1/d1r2/stand_row_00884.draws.json | 01f5ad46b7f2a101e81f2f25e4f499a0cbc57e1adf287505b7207a646da99298 |
| rep_on_r1/d1r2/stand_row_00924.draws.json | a8cb2db0e4e511a5d1ed570b5995f0bcbd97e8b0d76135095effeebe64ecec24 |
| rep_on_r1/d1r2/stand_row_00959.draws.json | 4b01bd3fb7976795d48e6292930c0b7ba6a571fff277b8449e029d881a55873b |
| rep_trace_pkcb.tsv | 201f7157b7ef2a94553d094372965f9f8cab61d1cadecfb421360c8738fc0930 |
| arms/H_DEF.step.csv = arms/H_IC.step.csv | 5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415 |
| arms/H_DEF.gates.csv = arms/H_IC.gates.csv | 364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4 |
| arms/H_DEF.ptrace.log = arms/H_IC.ptrace.log | 520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757 |
| arms/B_DEF_K0.step.csv = arms/B_IC_K0.step.csv | 3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f |
| arms/B_DEF_K1.step.csv = arms/B_IC_K1.step.csv | 9fcdbd3a6901025544e377132c575910aa5cd001faba802c1e7e8a9ac0d2886f |
| arms/B_DEF_K26.step.csv = arms/B_IC_K26.step.csv | 5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d |

The original side is `verify/d1r1_20261016/evidence/training_orig_row_{00884,00924,00959}.draws.json`
(committed) and `log/d1r1/orig_training/trace.csv` (main checkout, 3,863,100 bytes). No new original capture
was made or needed.

## 6. Not claimed, follow-ups

- No C-level change: the original does not boot (audio endpoint), so no Frida diff.
- No default flip. Flipping `MASHED_ICONCUBE` ON is a decision row. The cube draws faithfully the pool it
  is given. A matched spin and set against the original needs the pickup tick to start on the original's
  frame and the collections to match, both sim-side.
- The PickupField stand-in orbs still draw with the knob ON. They also stand in for the glow, which is
  D5-ICONCUBE-GLOW (blocked on rfx emitters). Retiring them belongs with that step.
- Candidate follow-up step: align FUN_00459000's first tick with the original's (178 frames in the d1r1
  Training replay) and check the replay feeds the registry's car positions, then re-run `cube_cmp.py`.
