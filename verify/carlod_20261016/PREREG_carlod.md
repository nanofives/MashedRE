# PREREG carlod: per-part car visibility (FUN_00420870) and the env gloss pass (U-9079, U-9693)

Committed before the first measuring run. Base: `race/first-frame-parity` `b26d9f3c` (rtex merged).
Branch `work/carlod`. Instrument: d1r1's matched-pose per-draw recorder and `re/tools/drawlist_diff.py`
3D mode (`verify/d1r1_20261016/RESULT_d1r1.md` sections 1 and 7) on its 9 frames: TRAINING 884/924/959,
Arctic 1051/1091/1126, City 761/801/836. The original side is d1r1's committed records
(`verify/d1r1_20261016/evidence/*_orig_row_*.draws.json`) and traces (`log/d1r1/orig_*/trace.csv`),
reused unchanged, so every row delta is a standalone-side change. Harness: copies of rtex's
`run_arm.py` / `run_replay3d.py` / `rowdelta.py` / `run_gctrl.py` in this directory, outputs under
`log/carlod/`. Base exe: `log/carlod/exe_base/mashed_re.exe`, the clean build of `b26d9f3c`.

Cited rows this lane owns (from `verify/rtex_20261016/evidence/final_diffs/`): **C02-CARLOD**, 3 rows per
frame, 27 in all (EXTRA Advantage 91-134 draws, EXTRA AdvantageSmall 12-16, EXTRA Glass 8-16) and
**C17-CAR-GLOSS**, 2 rows per frame on 7 frames (TEX1 `Advantage ... CarGloss`, MISSING `CarGloss`).

## 1. What the original does (static, then live)

All RVAs in `MASHED.exe` (anchor in CLAUDE.md). Extracts in `evidence/`.

**A. The per-part loop.** `FUN_00420870` @ `0x00420870`, called per car per frame from `FUN_0040da50`
(`FUN_00420870(slot, param_2, view matrix)`). The car record is `0x0063d9e0 + slot*0x2ac`; its dword
`part*4` (part 0..0x7f) is the part's RpAtomic, null entries skipped (`0x00420b20`). For each one a
switch (`0x00420a33`..`0x00420b16`) computes a bool from the record's **state** `S = *(rec+0x298)`
(`0x0063dc78`, values 0/1/2) and **flags** `F = *(rec+0x294)` (`0x0063dc74`) and passes it to
`FUN_004b5240` (`0x00420b16`), which sets (`or cl,4` `0x004b524f`) or clears (`and cl,0xfb`
`0x004b5256`) bit 4 of the atomic's flags byte (`mov byte [eax+2],cl`), rpATOMICRENDER. Every case then
ANDs the result with `NOT (F & 0x400)`, except parts 0x17 and 0x32 (`0x00420b98`).

| parts | visible when |
|---|---|
| 0 1 2 3 8 9 0x10 0x12 0x14 | S = 0 |
| 4 6 / 5 7 | S = 0 and F&0x10 clear / set |
| 0xa 0xc / 0xb 0xd | S = 0 and F&0x20 clear / set |
| 0x46 | S = 0 |
| 0x17 | S = 0 and F&0x400 (skips the 0x400 mask) |
| 0x21 0x22 0x23 0x24 0x29 0x2a 0x31 0x47 | S = 1 |
| 0x25 0x27 / 0x26 0x28 | S = 1 and F&0x10 clear / set |
| 0x2b 0x2d / 0x2c 0x2e | S = 1 and F&0x20 clear / set |
| 0x32 | S = 1 and F&0x400 (skips the 0x400 mask) |
| 0x64 | S = 2 and F&0x30 clear |
| 0x65 / 0x66 / 0x67 | S = 2 and (F&0x10, F&0x20) = (set, clear) / (clear, set) / (set, set) |
| 0x11 | F&0x800 set and F&0x20 clear (any S) |
| 0x15 | F&0x40 set and F&0x10 clear (any S) |
| 0x18 | F&0x80 set and F&0x20 clear (any S) |
| every other part code | never (`bVar11` stays false, `0x00420a2e`) |

**B. State S.** Zeroed at car init (`FUN_00420420` `0x00420482`); written by `FUN_0041ef60` (store at
`0x0041ef6e`) from the race camera update `FUN_00446520`: `0` for slots 0..3 at its start (`0x00446547`);
in the chase branch, per slot, `FUN_0041f120(slot, 0, &d2)` (3 stack args, `add esp,0xc` `0x0044694f`) gives
`d2` = |`rec+0x258` (part 0x13's LTM position, cached by `FUN_00420870`) - camera object frame position|^2,
then `fld [ebp-0x1ac]; fcomp [0x005ce1d0]; test ah,5; jp` (`0x00446952`..`0x00446963`): `S = 1` when
`d2 < 144.0` (`[0x005ce1d0]` = 144.0, read from the image), else `S = 2` (`0x00446965`, `0x00446975`);
then `S = 0` for slot 0 when `FUN_0042f6a0() == 2` or `DAT_007f0fd0` is 5, 9 or 10 (`0x0044698d`..`0x004469ae`),
and `0` for all slots at `0x004486d9` (`FUN_00448220`).

**C. Flags F.** Bit 3 (`0x8`, slot active, `FUN_0041eda0`). `FUN_0041eeb0` (`0x0041eeb0`, called per car per
frame at the end of `FUN_0040da50`, `FUN_0041eeb0(slot, bc44, bc44, brake)`): `0x40` and `0x80` follow
`DAT_0063bc44[slot]`, which `FUN_0040da50` sets to 1 when `DAT_00644158` (the course id) is 0, 0x1a, 0x21
or 0x24; `0x800` follows the controller's byte `*(0x007f103c + ctrl*0x4c + 1)` (the brake byte, per
`Ai/AiController.cpp` `[5] brake`); then `F&0x400` clears `0x8c0`, `F&0x10` clears `0x40`, `F&0x20` clears
`0x880`. `0x10`/`0x20` are written by `FUN_0041ede0`, `0x400` by `FUN_0041ef80`.

**D. Live witness (original, 112 car records)**, `vis_check.py` on rtex's `--matfx` captures
(`log/rtex/orig_matfx2_*`, every 100 trace rows, all game phases), `evidence/vis_model_check.txt`:
- the table in A with the right S reproduces **every** part's render bit on 111 of 112 records (71 parts
  each); the 112th is City row 900 car 2, whose flag word was read one frame after the bit it produced
  (`0x8c8` -> `0xc8`, part 0x11 still on);
- the distance rule in B picks that S on the same 111 (`d2` from `pos258` and `p29c_pos`);
- flags seen: TRAINING `0x008`, Arctic and City `0x0c8` (course ids 30 / 0 / 26; 0 and 0x1a are in C's set,
  30 is not), `0x800` on two City cars at row 900. `0x10`, `0x20`, `0x400` never set.
- visible parts at S = 1, F = `0x008`: `0x21 0x22 0x23 0x24 0x25 0x27 0x29 0x2a 0x2b 0x2d 0x31` (11 per car,
  the 44 Advantage draws of d1r1 over four cars); with F = `0x0c8` plus `0x15 0x18`. At S = 2: `0x64` only
  (plus 0x11 / 0x15 / 0x18 by flags).

**E. The standalone today** draws every non-hull atomic of the car model, so it draws the S = 0 / 1 / 2
sets together (92-134 extra Advantage draws, 12-16 AdvantageSmall, 8-16 Glass per frame, C02). With the
env pass on (`MASHED_CAR_ENV=1`) the extra Glass atomics also get the second pass (rtex rule 2 / 6 fail).

## 2. Fixes (each with an env knob; default decided by section 4)

- **F-VIS**: `Vehicle/CarPartVisible.{h,cpp}` (new, both targets, one body) transcribes A. `LibRw/RwRaceSubmit`
  gives each queued car instance a state and flag word and sets each atomic's RENDER flag from
  `CarPartVisible(S, F, part)` before drawing it (parts from `g_model_parts`; the model's build-time
  flags restored for instances without one). `TrackRenderer` supplies S from B (the part-0x13 world position
  `car_part19_` x the instance matrix against `last_eye_`, FUN_0041f120's operand order, unclamped `d2 < 144.0`)
  and F = `0x8` plus `0xc0` when `course_id_` is 0, 0x1a, 0x21 or 0x24. Knob `MASHED_CAR_VIS=0` disables.
  Stated gaps, `[UNCERTAIN]`, U-row filed: F bits `0x10`, `0x20`, `0x400` stay 0 (the standalone has no
  damage or wreck state to feed them), `0x800` stays 0 (no brake-byte binding in the standalone;
  `Vehicle::g_launchAccelByte` is the accel byte only), the slot-0 `S = 0` modes of B are not modelled,
  and S uses the current frame's matrix where the original reads last frame's cache (differs only for a
  car within one frame of the 12 m threshold).
- **F-ENV** (re-run, not new code): `MASHED_CAR_ENV=1` on top of F-VIS, judged by the same rules.
- **Follow-up 3** (48 ZERO/ONE dual materials on the Atmos car, the dual-UV-transform material on Storm):
  only if cited. All four cars on all 9 frames draw texture `Advantage` (d1r1 Pr-1), so no cited row
  involves either model. Filed, not ported, unless a cited row appears.

## 3. Rules (per fix, on all 9 frames)

1. **Targets.** F-VIS: the three C02-CARLOD rows disappear on all 9 frames (27 rows). F-ENV, measured on top
   of F-VIS: the C17-CAR-GLOSS rows (TEX1 `Advantage ... CarGloss`, MISSING `CarGloss`) disappear on every
   frame that has them (TRAINING 3, Arctic 1051 / 1091, City 761 / 801).
2. **No new rows.** No (kind, tex, detail-class) row appears that the comparison arm lacks
   (`rowdelta.py`, rtex's key), on any of the 9 frames, and every remaining row stays cited (0 RED). F-ENV
   keeps rtex amendment 2: a NEW STATE row whose differing fields are a subset of {AREF, AF, CULL} on a
   texture whose MISSING row disappeared in the same frame is that MISSING row's replacement (cited C19), not
   a failure. Any new EXTRA / MISSING / ORDER / MOVED row fails.
3. **Matched draws and matched triangles do not drop** per frame.
4. **No-op control.** `MASHED_CAR_VIS=0` reproduces the `base` arm's rows on 9/9 frames.
5. **G-CTRL** (`run_gctrl.py`, TRAINING default arm) gives `c2e4d60d / b50bd39b / ba90ed88` before (base exe)
   and after (final default exe).
6. **Pixels (report).** Mean abs RGB difference against the original BMP per frame, base arm vs each arm.
   A rise above 0.5 on any frame blocks default ON (recommendation instead).

A fix passing 1-5 ships default ON with its opt-out knob, rule 6 permitting. Failing any keeps it default OFF.

## 4. Arms

`base` (exe_base, default knobs), `vis` (new exe, defaults), `vis_off` (`MASHED_CAR_VIS=0`, rule 4),
`vis_env` (`MASHED_CAR_ENV=1`), and if F-ENV passes the final exe with its new defaults. Rule 2 for F-VIS
compares `vis` against `base`, for F-ENV `vis_env` against `vis`.

## 5. Predictions

- Pr-1: with F-VIS the standalone draws 11 Advantage-family parts per car on TRAINING and 13 on Arctic and
  City (0x15, 0x18), and every C02 row goes; no other row moves.
- Pr-2: the C03 depth-off per-car mesh row (`0x004ec3e1`, `FUN_0041faa0`) is unaffected: it is not one of
  the 11 parts.
- Pr-3: with F-ENV the env second pass runs only on the visible env parts of A's set (parts 0x25, 0x29,
  0x2b at S = 1) and the Arctic 1051 pixel rise of rtex (+0.60) is gone.
- Pr-4: `MASHED_CAR_VIS=0` is bit-identical in rows to base, so the no-op control passes.

## 6. Amendments

(Amendment 3 below was written AFTER the first `vis` arm's result was read; it is labelled so.)

### Amendment 1 (citation correction; written after the `base` arm and while the `vis` arm ran, before any
`vis` result was read; no rule changes)

Section 1 A cites `0x00420a2e` for "`bVar11` stays false". That address was not read from the binary and is
wrong. The disassembly (`evidence/dis_00420a00_switch.txt`) gives: the per-part loop body starts `0x00420a16`
(`mov ecx,[esi+ebp*4]`), the result is cleared by `xor eax,eax` at `0x00420a19`, a null atomic jumps to the
next part at `0x00420a1d` (`je 0x420b20`), parts above 0x67 take the 0 to `0x00420af8` (`cmp ebp,0x67; ja`,
`0x00420a23` / `0x00420a26`), the dispatch is `jmp [edx*4 + 0x420c3c]` at `0x00420a33` with the byte index
at `0x00420c94`, the 0x400 mask is `0x00420b02..0x00420b14`, and `push eax; push ecx; call 0x4b5240` is
`0x00420b16..0x00420b18`. Every other RVA in section 1 was checked against the same listing and the
call-site listing (`evidence/dis_state_setter_callsites.txt`). The per-handler entry addresses cited in
`LibRw/RwCarFx.cpp` come from that listing and match the table in 1 A by what each handler tests.

### Amendment 2 (harness note, before any `vis` result was read)

The first TRAINING launch of the `base` arm exited 0 and wrote no records (arctic and city, run right
after it, wrote theirs). Re-running TRAINING alone produced rows 80 / 79 / 79, matched 194 / 191 / 191, which
equal rtex's final numbers exactly. The `base` arm's TRAINING numbers are from that re-run
(`log/carlod/run_base_training.txt`); the empty first launch is not evidence of anything about the exe.

### Amendment 3 (written after the first `vis` arm was read; scope addition, rules unchanged)

The first F-VIS build (arm `log/carlod/vis1_noglass`, section 2 as written: RENDER bits only) FAILED rules 2
and 3 on TRAINING and City: the three C02 rows went on every frame it ran, but a NEW uncited MISSING `Glass`
row appeared (original 8 Glass draws, 64 triangles, now unmatched) and matched draws fell 194 -> 186 /
191 -> 183 / 191 -> 183 on TRAINING (City 351 -> 343, 339 -> 331). Cause, from the original: the Glass
atomics (parts 0xe 0xf 0x2f 0x30) have no RENDER bit in the live capture (`vis_model_check.txt`
and the part listing, `rflags` 0x1) because `FUN_00420870` hides them, and the original draws them anyway:
`FUN_00420050` @ `0x00420050` renders the car clump (`call 0x4e6680`, `0x00420162`) and then calls
`[atomic+0x48]` (the atomic's own render callback) on glass atomic A (`0x0042019d`) and B (`0x004201d1`),
chosen by S (`[esi+0x38]` part 0xe or `[esi+0xbc]` part 0x2f; `[esi+0x3c]` part 0xf or `[esi+0xc0]` part
0x30; none at S = 2), gated by F bits 0x400 (both), 0x10 / 0x1000 (A), 0x20 / 0x2000 (B)
(`evidence/dis_00420140_glass_pass.txt`). F-VIS therefore includes that pass: after the clump render,
`CarGlassPart(S, F, 0)` then `(S, F, 1)` atomics are rendered through `Atomic::render()`. The `vis` arm of
section 4 is the build with the glass pass; `vis1_noglass` stays on record as the failed first attempt.
Predictions and rules are unchanged: Pr-1 now reads "11 (TRAINING) / 13 (Arctic, City) opaque parts plus the
2 glass atomics per car".
### Amendment 4 (written after the second `vis` arm was read, before any arm that uses the input; instrument input, rules unchanged)

The second `vis` arm (the build with RENDER bits and the glass pass, `log/carlod/vis`): 0 RED, the three C02
rows and the C21 Glass ORDER row gone on all 9 frames, TRAINING 0 new rows and matched 194 / 191 / 191
(equal to base), pixels down on all 9 frames. It still fails rules 2 and 3 on Arctic 1091 and 1126 and City 836:
a NEW MISSING `Advantage` row (1, 1 and 2 original draws of 4 triangles) and matched draws 138 -> 137,
101 -> 100, 292 -> 290. Part 0x11 has 4 triangles (`evidence/car_parts.txt`) and is the only 4-triangle
visible part; it shows when F & 0x800, the controller's brake byte (section 1 C), and the standalone's default
AI cars produce no such byte. The base build matched those plates by accident, because it drew every atomic
of every car.

Instrument input, in the spirit of the car-pose transplant: `MASHED_CAR_BRAKE_ROWS=row:mask,...` (default
unset, `TrackRenderer`, measurement only) sets F & 0x800 for the slots in `mask` on the frame that consumed
trace row `row`. `brake_from_draws.py` derives the masks from the ORIGINAL's committed draw records: a
4-triangle `Advantage` draw whose box centre is within 3 m of a car's frame origin (`brake_rows.json`,
`evidence/brake_from_draws.txt`: Arctic 1091 slot 3, 1126 slot 1, City 836 slots 1 and 2, none on the other
six frames, every plate 0.51 m from its car). The derivation is circular for the 0x11 plates (it reads the
draws the arm is scored against), so the arm that uses it is NOT evidence for the 0x11 gate. That gate rests on
the disassembly (`0x00420b7d`, `FUN_0041eeb0`) and on the live witness (`vis_model_check.txt`: City row 900,
the two cars with F = 0x8c8 show part 0x11). What the arm shows is that, with that one input supplied, no
other row appears and no matched draw is lost.

Arms: `vis` (the build as is, no brake input, the failing record above), `visb` (same exe, brake input on),
`visb_off` (`MASHED_CAR_VIS=0`, brake input on: rule 4 control). Rule 2 and 3 for F-VIS read `visb` against
`base`. The default build still shows no brake lights on AI cars; that is U-9703's open half and is reported,
not closed.
### Amendment 5 (written after the `visb_env` arm was read, before any arm that uses the change; F-ENV input fix, rules unchanged)

`visb_env` (F-VIS plus `MASHED_CAR_ENV=1`, rtex's coefficient): 0 RED, matched draws and triangles up on 7
frames, rule 4 control `visb_off` = base on 9/9, rule 2 clean on 8 of 9 frames (the NEW `STATE CarGloss` rows
are rtex amendment 2's replacements of the MISSING rows that went). It fails rules 1 and 2 on City 801 only:
the original gives env draws to cars 0 and 3 there, the standalone to car 0 (`evidence/env_cars.txt`), so the
MISSING `CarGloss` and TEX1 rows only shrink and the new STATE row has no vanished MISSING row to belong to.
Car 3's raw d2 is 102.70 in the standalone (`vislog_city.csv`) and FUN_00420290 clamps d2 to 100, so the
standalone's coefficient byte is 0 (FUN_005412d0 draws plain at byte 0, `uVar8 == 0`).

Finding: the original's coefficient is computed against the PREVIOUS row's camera. `env_lag_live.py` takes
rtex's live captures (112 records, coefficient rec+0x274, shade rec+0x284, cached part-0x13 position, the
formula of FUN_00420290) and the same run's trace: with camera row K-1 the formula reproduces the live
coefficient with residual 0.00000 on every record with a nonzero coefficient; with row K it is off by up to
+0.009, with K-2 by up to -0.010 (`evidence/env_lag_live.txt`). `env_lag_test.py`: the camera row K-1 predicts
whether the original draws env for each of the 36 car-frames of the 9 cited frames with 0 errors, row K with 1
(City 801 car 3). rtex's "reproduces the live value to about 1e-4 on stationary rows" is this lag on rows where
the camera barely moves. The same test on the LOD state (`state_lag_test.py`, observed state from the far-LOD
draws) cannot discriminate: all 16 (camera row, car row) combinations predict the 36 car-frames with 0 errors,
so the state rule stays on this frame's camera and the lag question for S is left open (U-9703).

Change: `RaceSceneState::eye_prev_` (the previous Render's eye) feeds the coefficient's d2
(`TrackRenderer` car block); `MASHED_CAR_ENV_PREVCAM=0` restores this frame's camera (no-op control).
Arms: `venv` (new exe, `MASHED_CAR_ENV=1`), `venv_nolag` (`MASHED_CAR_ENV=1 MASHED_CAR_ENV_PREVCAM=0`, must equal
the `visb_env` arm). Rules 1-6 for F-ENV are unchanged and read `venv` against `visb`; the failing `visb_env`
arm stays on record.
### Amendment 6 (written after the `venv` arm was read, before the arms that use the change; reading of rule 6 and one input fix)

Rule 6 reading. Section 3 rule 6 says "base arm vs each arm". For F-VIS the base arm is the arm without the fix;
for F-ENV it is `visb` (F-VIS only), which is also how rtex's rule 6 compares ("both arms", the arm with and
the arm without the fix). The stricter, incremental reading is the one used for the default decision; the
literal one (against `base`) is reported beside it. `venv` (previous-camera coefficient, rtex's shade): rows and
matches pass (City 801 now fully like the other frames), pixels against `visb` rise by +0.57 on Arctic 1051
(10.74 -> 11.31, over 0.5) and by at most +0.37 elsewhere; against `base` the largest rise is +0.42.

Cause of the Arctic rise, measured: the standalone's env shade (rec+0x284) on Arctic is 1.0 on all four cars
(`log/carlod/vislog_arctic.csv`) while the original's live value is 0.21-0.28 (rtex captures, rows 400-1000),
so the coefficient is about 0.87 at row 1051 against about 0.61. City matches (0.2926 vs 0.293), TRAINING varies
per car in both. The sampler (`CollisionPrelitSample`, FUN_00479100 @ 0x00479100) returns 1.0 for a hit sector
without colours, and `Track::World::Parse` read 5 of Arctic's 6 collision sectors as colourless: its size-only
layout solver took COLLISIONS.BSP's 8 extra bytes per vertex as "1 UV set". The world header's `format` word
(`0x40000079`: prelit + normals, 0 UV sets, STRUCT +0x24) with RenderWare's 4-byte sector normals
(RpVertexNormal) gives 4 + 4 = 8 bytes per vertex, i.e. normals + prelit.
`evidence/bsp_layout_check.txt` walks every BSP of all 13 track pizzes: the flags' layout, where it fits the
size equation, differs from the solver on exactly 6 of 490 sectors (Arctic COLLISIONS.BSP x5, Arctic AI.BSP x1).

Change: `Track/TrackWorld.cpp` uses the flags' layout when it fits and keeps the solver otherwise. It changes
which colours a hit on those sectors returns, so the default build is affected only through the env pass
(`MASHED_CAR_ENV`), and no sector of any GRAPH / COLL BSP of TRAINING, City or the other 11 tracks parses
differently. Arms: `visb2` (new exe, defaults) must equal `visb` in rows (no-op for the default build) and
`venv2` (`MASHED_CAR_ENV=1`) is the F-ENV arm, judged by rules 1-6 against `visb2` with rule 6 read
incrementally. If it passes, F-ENV becomes default ON (`MASHED_CAR_ENV=0` opts out) in a `final` build whose
rows must equal `venv2`'s.

### Amendment 7 (rebase onto `0020d521`; written after the rebased arms were read; no rule changes)

`race/first-frame-parity` moved from `b26d9f3c` to `0020d521` while this lane ran (31 commits, among them
`librwrest`: car liveries per slot, wheel / hub / body poses through the DFF frame hierarchy
(`RaceSubmit_AddInstancePosed`, per-model draw order and frustum cull), per-car relight). The branch was
rebased and every number that decides a default was re-measured on the rebased tree. Pre-rebase results
(sections above, arms on `b26d9f3c`, `scores.txt`) stay on record.

- Merge: `RwRaceSubmit` / `TrackRenderer` keep `librwrest`'s structure. The per-part RENDER bits are set before
  its pose and draw-order block (which already honours the RENDER flag per atomic) and the glass atomics are
  drawn after the clump render inside the same light and render-state scope, only for a copy that was not
  culled; `CarVis` is carried through `AddInstancePosed`. The state and flags use the car matrix and part 0x13's
  frame origin of the player's model (`car_part19_`), as before.
- `Track/TrackWorld.cpp`: `librwrest` fixed the same sector-layout bug independently (its `hdr_format & 0x10`
  branch: 4-byte `RpVertexNormal`, prelit from the 0x08 flag, UV count solved). Its layout equals the flags'
  layout of `evidence/bsp_layout_check.txt` on every sector where the two differ from the old solver (Arctic
  COLLISIONS.BSP x5, AI.BSP x1), so carlod's change to that file was dropped in the rebase.
- New baseline: arm `base2` (clean `0020d521`, `log/carlod/exe_base2`) and G-CTRL `716cb2fd / 2a2afda1 / ba90ed88`
  (the new default, u9685). Arms on the rebased tree: `base2`, `visx` (`MASHED_CAR_ENV=0`: F-VIS only), `visoff`
  (`MASHED_CAR_VIS=0`, rule 4), `nolagx` (`MASHED_CAR_ENV_PREVCAM=0`), `final2` (all defaults). Rules read as in
  amendments 4-6: F-VIS `visx` against `base2`, F-ENV `final2` against `visx` with pixels read incrementally.
- Measurement hygiene: two builds must not run at once (a nested worktree made the base build write into this
  worktree's `build\obj`); the rebased campaign is `run_all.py`, one standalone run at a time.
