# PREREG rprops2: rprops follow-ups U-9699, U-9700, U-9701 and U-9696's remainder

Committed before the first post-fix run. Base: `race/first-frame-parity` `04dee0ed`.
Branch `work/rprops2`, worktree `.worktrees/rprops2`. Evidence dir `verify/rprops2_20261016/`, bulky
runs under `log/rprops2/` (gitignored). One commit per item, each scored on its own. Assets come from a
private copy of `original/` (scratchpad `gameroot`, anchor `MASHED.exe.unpatched` SHA-256 `BDCAE093...3C0E`
checked), never the shared main root.

## 0. Instrument and baseline (measured, unchanged base)

d1r1's matched-pose per-draw recorder and `re/tools/drawlist_diff.py` 3D mode on the nine frames
(TRAINING 884/924/959, Arctic 1051/1091/1126, City 761/801/836), harness copied from
`verify/rprops_20261016/` with the paths re-pointed (`run_arms.py`, `run_replay3d.py`, `score_rprops2.py`,
`score_pix.py`, `run_gctrl.py`, `rnoop.py`). Base build `04dee0ed` unchanged, arm `base4`
(`scores_base4.txt`): matched draws 198/195/195, 145/144/105, 474/462/415, rows 77/76/76, 78/77/77,
126/124/122, 0 RED on 9/9. Those row counts equal `RESULT_rfx.md` section 9 "rb3, default" exactly, so
the instrument reproduces the orchestrator's baseline on this machine (I-N0). Original-side records are
d1r1's (`verify/d1r1_20261016/evidence/*_orig_row_*.draws.json`), read only.

## 1. Decoded before any post-fix run (bytes: `disasm_range.py` over `MASHED.exe.unpatched`; decomp: `decomp_pc.py --create`)

**U-9700 (ship tick gate).** `0x0041e9e0` is `return *(DAT_0063d7e4 + 0x18)`. `DAT_0063d7e4` is written
only by `FUN_0041e870` (`0x0041e878` clear, `0x0041e895` store): it scans the hook table at `0x005f33f8`
(stride 0x48, count `0x005f37a0`) and keeps the record whose `+0x10` (Course_Id) equals its argument.
Record `+0x14` is slot 0, `+0x18` is slot 1. `0x0041e8c0` is `jmp [DAT_0063d7e4 + 0x18]`. So
`0x00426ab0` (the course update) calls slot 1 when the pointer is non-null. `0x0041e9b0(id)` returns
`*(DAT_0063d7e4 + 0x10) == id`, a value `0x00426ab0` discards. There is no frame counter and no
two-update delay in the exe. `0x00448cb0` poses the freighters BEFORE it advances the accumulators, so the
K-th call of the course update poses with K-1 advances. The port's "skip the first two Render ticks"
(`UpdatePropAnims`) is therefore an offset between the standalone's Render count and the replay row index
(`RaceCamUpdate` consumes row `R.next++`), not an exe behaviour.

**U-9701 `SkySecondaryDispatch` / `[DAT_007030ac + 0x48]`.** `DAT_007030ac` is written once, at
`0x004880e2` in `FUN_004880a0`: a sprite mesh made by `FUN_00534b60(0x2000, 0x10000087, 0)` with source
blend 5, destination blend 2 (additive), `+4 = 0x200`. `FUN_004883b0` refills it every frame from the
56-byte pool at `0x0086ae4c` (16 vertices per live entry, size and alpha per vertex, fed through the
`FUN_00535700` streams 1, 4, 2, 0x80). `+0x48` of a mesh made by `FUN_00534d00` is its draw callback
(`FUN_00535cb0`, `DrawIndexedPrimitive(TRIANGLELIST)` with `NumVertices = 4n`, `PrimCount = 2n` when
`_DAT_007dc598 == 0`). `FUN_004862d0` initialises the same pool family (`exp_cloud2`, `scorch`). The
City leaf sprites (`FUN_0047c330`, `DAT_006c301c = FUN_00534b60(0x800, 0x87, 0)`) are the same mesh class,
and the recorded original draws them as TRIANGLELIST 432 vertices / 216 primitives twice per frame.

**U-9699 (camera-man yaw).** `0x00441d40` (1730 bytes) is `Step(entry, host)`: `dt = (float)DAT_007f1008 *
1/3000`, target = `0x00441c80` midpoint, PD-style rewrite of entry `+0xb0` (yaw, degrees) / `+0xbc` (rate)
and `+0xac` / `+0xb8` (pitch), guarded by `entry+8 != 1`, constants read from the image:
`0x005cc948`=1/3000, `0x005cc9fc`=1000, `0x005ccacc`=45, `0x005cc9b8`=7, `0x005cc9e0`=25, `0x005cc750`=16,
`0x005cd9fc`=7.5, `0x005cc35c`=4, `0x005ccac4`=360, `0x005cd09c`=180, `0x005ccad0`=90, `0x005cd324`=270,
qword `0x005ccae0`=57.2957795. The recorded original draw carries the camera-man world matrix: TRAINING
884 `w` = (0.99706, 0, 0.07668 / -0.07668, 0, 0.99706), i.e. yaw atan2(0.07668, 0.99706) = 4.398 deg,
which is the direct witness. The entry array exists in the standalone only under `MASHED_SUBSTATE` (default
off), and `CameraEntryDispatchAll` does not call types 1 and 2 there, so the default build has no
camera-man entry state to step.

## 2. Fixes under test (one commit each)

- **F-YAW** (item 1) port `0x00441d40` as `CamEntryType1Step441d40` in `Race/CameraEntries.cpp` (both
  targets, instruction-order x87 transcription, ST values `double`, `fstp dword` rounds). The default build
  keeps a small private entry array for the camera-men (built by `CamEntryBuild441b30` from the track's
  table rows 1..N) and steps it once per `RaceCamUpdate` from the same car rows the camera reads
  (midpoint of the most-separated pair). `TrackRenderer` re-poses the camera-men instances from entry
  `+0xb0` each frame. Opt-out `MASHED_NO_CAMMANYAW=1`.
- **F-TICK** (item 2) move the ship tick to the course-update analogue (once per `RaceCamUpdate`) and delete
  the skip constant if the phase then measures exactly K-1 with no tuning. Otherwise keep the constant and
  only correct its documentation and U-9700.
- **F-LEAF / F-PASS** (item 3) and **F-TEX** (item 4): predictions are added as amendments below before
  their first post-fix run.

## 3. Gates

| gate | pass condition |
|---|---|
| **R-RULE1** | every row a fix claims to turn GREEN matches on every frame where it occurs |
| **R-RULE2** | no row that matched at `base4` stops matching on any of the nine frames (matched draws and triangles do not fall) |
| **R-RULE3** | no new RED row: every unmatched row hits a citation |
| **G-CTRL** | default build `716cb2fd / 2a2afda1 / ba90ed88` (`run_gctrl.py`, step / gates / ptrace sha8) against a baseline built from this base (`base4_default`) |
| **R-NOOP** | the opt-out arm reproduces the base draw record and BMP byte for byte |
| **R-PIX** | mean absolute pixel difference against the original does not rise by more than 0.5 on any frame |

A fix is default ON with its opt-out only if every gate passes. Otherwise it stays default OFF with a
recommendation in `RESULT_rprops2.md`.

## 4. Predictions (falsifiable, fixed before the run)

- **Pr-1a** (F-YAW) the standalone camera-man world matrix yaw equals the recorded one within 0.5 deg on
  every nine-frame row where the original records a camera-man (TRAINING `CamManHot` 884 / 924 / 959,
  Arctic `CamManCold` where on screen). If the residual is larger, it is reported with the arm and the
  assumption it points to (initial rate state, or the step-to-row phase).
- **Pr-1b** the CamMan AABB rows stay matched (0.01) and matched draws do not fall.
- **Pr-1c** G-CTRL unchanged (the step writes only the private entries, no value the simulation reads).
- **Pr-2a** (F-TICK) the freighter y phase measures K-1 at the camera-update site without a skip constant:
  Arctic ship rows match at 0.01 on every frame where they are on screen, exactly as at `base4`. If the
  count at the camera-update site is not K-1, the constant stays and Pr-2a is refuted.
- **Pr-2b** no other row moves; G-CTRL unchanged.
