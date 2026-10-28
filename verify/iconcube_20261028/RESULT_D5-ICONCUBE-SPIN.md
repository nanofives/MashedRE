# RESULT D5-ICONCUBE-SPIN (D-11076 cube half, L-45)

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
