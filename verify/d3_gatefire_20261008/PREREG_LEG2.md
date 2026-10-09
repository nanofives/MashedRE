# PRE-REGISTRATION: D-11073 leg 2 (phase 3, the camera hold), behind default-OFF `MASHED_SUBSTATE`

Date 2026-10-09. Brief: `re/BRIEF_D11073.md` §1 "Phase 3, fully mapped" + §2 "Leg 2". Decision (A) stands:
the start-boost seed stays, the knob stays default OFF. This file is committed BEFORE any leg-2 code.
§4 (the release-frame prediction) is appended after the measurement step and committed BEFORE the
gate arm.

## 1. The machine, from the bytes (step 0, `leg2_decomp.txt`, `leg2_445aa0.asm`, `leg2_bytes.py`)

Per phase-3 frame the original runs `FUN_004111c0` case 3 = `FUN_004102f0` (`0x00411288`), then
`FUN_0040fc00` (units in ESI) -> `FUN_0040d470(0)` -> `FUN_00448220` -> `FUN_00446520(&DAT_00897fe0, p3)`.
`DAT_00897fe0` IS the race-camera struct; its dword 0 is the hold flag.

- `FUN_004102f0` (units in **EBX**, `unaff_EBX`): if `DAT_005f29b8 == 100000` (stored once, `0x004100d3`):
  `FUN_00448700(0,0)` (100 x `FUN_004464c0` = no-op, count 0 here; then `0x00897ffc`/`0x00898000` = 0),
  `FUN_004430a0(1)` (flag = 1), `FUN_0040e590()`, `FUN_0040d470(1)` (a full camera chain). Then hold:
  `FUN_0046baa0(v)` per alive car. `DAT_005f29b8 -= (u32)(units*0x44c)/0x3c`. Release:
  `FUN_00405460()==0 && FUN_004430b0()==0` -> `DAT_0063ba8c = 4`, `DAT_005f29b8 = 12000`.
- `FUN_00446520`: `FUN_00442600()` (entry init, once per race, guard `DAT_0089898c`, zeroed by
  `FUN_004425d0` from `0x0040ff31` at race setup); sub-mode != 6 -> `FUN_004464c0(cam)` (entries step,
  reading cam `+4/+8/+0xc` from the PREVIOUS camera update); then the camera main block if `cam[7]==0`;
  then the copy `cam+0x40..0x48 -> cam+4..0xc` (`0x004481c9`).
  **First phase-3 frame steps the entries twice** (via `FUN_0040d470(1)` and again via `FUN_0040fc00`).
- `FUN_00442600`: table by `FUN_00426c00()`; count `_ftol2(table[0])` (`0x00442695`); per row
  `FUN_00441b30(&DAT_008964c0 + k*0xd8, row)`; then `DAT_00897fc0 = _ftol2(FUN_00472650(0, 7.0)) << 8`
  (`0x00442744`): **one ring draw**.
- `FUN_00441b30` (bytes): entry `[1] = _ftol2(row[7])` (type), `[2] = _ftol2(row[8])`,
  `[0x33] = _ftol2(row[9])`, identity matrix at `+0xc`, `RwMatrixTranslate(+0xc, row[0..2], 0)`,
  `+0xa4 = 12.0`, `+0xac/+0xb0/+0xb4 = row[3..5]`, `+0xc4 = row[6]`, `+0xd0/+0xd4 = row[10..11]`.
- `FUN_00445aa0` (type 0), position path (`0x00445aa0..0x00445ea6`). With the flag set: target =
  `cam[1], cam[3]` (RAW dword copies, `0x00445ad5`/`0x00445adb`) each shifted by `min(|p-t|,4)/3`;
  `+0xa4 = cam[2]`; height spring on `+0x90`; XZ velocity toward the target, gain
  `clamp(d,0.75..5) * dt * 6 * 0.2`; damping `1-dt` (or `1-2dt` under 10); `RwMatrixTranslate(+0xc, v*dt, 2)`.
  **Release, site (a):** `d < 0.02f` (`_DAT_005ce18c` = `0x3ca3d70a`) -> `FUN_004430a0(0)`.
  **Site (b):** any of 8 bytes `0x007f1042 + k*0x4c` non-zero -> `FUN_004430a0(0)`.
  The rest (`+0xac..+0xc8`, the `+0x4c` matrix, `fsin`, `fpatan` x **double** 57.295799 at `0x005ccae0`)
  is orientation; nothing on the release path reads it.
- Constants: `0x005cc948` 1/3000, `0x005cc35c` 4.0, `0x005ccac8` 1/3, `0x005cc9a0` 0.05, `0x005cc31c` 3.0,
  `0x005cd0a0` 6.0, `0x005cc950` 0.75, `0x005cc358` 5.0, `0x005cc9c0` 0.2, `0x005cc55c` 10.0
  (`leg2_bytes.py f32`, all listed in the step-0 log).

### 1.1 Step-0 result: the position model reproduces the ORIGINAL (`leg2_model.py orig`)

A float32 transcription of the position path, fed the original's recorded target
(`o_cadence_nopress.csv` `tgt_fe4/fe8/fec`), gives entry 0 within 0.13 (early) and 0.005 (late) of the
recorded `e0_x/e0_z` at f = 60..600, and **releases at model frame 650 vs the original's last
phase-3 sample at frame 651** (652 frames). So the hold length is fully set by entry 0's dynamics and the
camera target's trajectory. This is what makes §4's prediction possible.

## 2. Static predictions (port inputs)

| # | prediction | basis |
|---|---|---|
| S1 | track id = 30 (`0x1e`) via `aib_track_index()` -> table `0x005f8d50`, count **7**, types **0,1,1,1,2,2,3** | `leg2_bytes.py`; the exe's `DAT_00644158` reads 0 (leg 0), so the switch is routed |
| S2 | entry 0 init: pos (1.5, 9.7, 34.5), `+0xa4` 12.0, `+0xac/+0xb0/+0xb4` = 5, 180, -15, v = 0 | row 0 |
| S3 | `FUN_00442600`'s draw is ring draw index **4** (after `FUN_00414180`'s 0..3; no AI tick runs in phase 3 in the port) -> u = `0x0fde8fa7`, 7·u·2^-31 = 0.867 -> **`DAT_00897fc0` = 0** | `p414_cdpred.py` ring |
| S4 | every later ring consumer shifts by +1 draw in round 1 -> round 2's cd = the ring at P414's offset + 1 | P414 round 2 at 319 -> now 320 |
| S5 | rounds 2+: **no phase 3** (tail sees `DAT_005f29b8` = 0 after `0x004104c0` -> state 4, 12000) | `FUN_004111c0` state-2 tail |
| S6 | release attributed to **site (a)**, all 8 input bytes 0 (the run has no input), entry 0 the only type-0 entry | brief + S1 |

## 3. What changes (scope)

Bodies at their RVAs, one TU in both targets, x87 (`x87_tus.txt`): `FUN_00441b30`, `FUN_00442600`,
`FUN_00441c80`, `FUN_00445aa0` (full body; the exe arm skips nothing in it). `CameraEntryDispatch.cpp`
(0x004464c0) moves to both targets; its exe arm steps type 0 through the port and skips types 1/2
(visual only, neither writes the struct; named) and re-reads `[esi+4]` before each test as the bytes do.
Exe bindings: `FUN_00426c00` -> `aib_track_index()`; `FUN_00405540` -> skipped (returns 0 when
`DAT_00639d78 == 0`, live 656/656; the exe has no clip module); `FUN_00472650` -> `Ai_RandRange`;
`FUN_0040e180` -> a host callback over `RaceCamera::MostSeparatedPair`; `FUN_0046d4a0` -> `PtrCompute881ec8`.

`FUN_004102f0` is EMULATED in the knob block (EBX register arg, no run_diff arg_type), stays C2, as the
state-2 tail is. Its `FUN_0040e590` call is transcribed only as the clock store `0x007f0ff8 = 0`
(`0x0040e5a2`); the rest of that 2,360-byte coordinator is named PARTIAL. Under the knob:
- race setup seeds `DAT_005f29b8 = 100000` (`0x004100d3`) and calls `UtilZeroTable8964c0` (`0x0040ff31`);
- the tail goes to state 3 on 100000;
- **the race camera is advanced during phases 3, 4, 5** (it is frozen there today), followed by the copy
  of its position into `cam+4..+0xc`. [UNCERTAIN] the original's `FUN_00442a20` (main block, flag != 0)
  is not in `RaceCamera`; named.

## 4. Release-frame prediction (appended after the measurement step; EMPTY at this commit)

**Appended 2026-10-09, BEFORE the gate arm `L2` ran.** Inputs: `L2_MEAS` (release held 800 frames),
`L2_MEAS.trace.csv`. G-CTRL already PASSES on this build (`L2_CTRL`: `c741a4c5` / `642af0ac` / `05984e30`).
Measured inputs the model takes:
- the port camera target starts at (0.030, 3.304, 4.838) and is at (0.453, 3.046, 4.526) by p3f 800
  (original: (-0.149, 3.542, 4.455) at f60 -> (-0.077, 2.986, 5.243) at release);
- the first two chain calls run with `DAT_007f1008` = 0 (dt = 0: the tail frame does not advance the
  clock, a leg-1b choice), then 50; the first call reads cam +4 = (0,0,0).

`leg2_model.py port L2_MEAS.trace.csv`:
- **G-FRAME prediction: the flag clears on chain call p3f 763 -> the log reads "phase 3 released after
  764 frames"** (gate: ±2). The original's 652 is NOT expected: the target differs.
- **G-CURVE prediction (raw distance to the unshifted target, both sides):**

| f | predicted port | original | ratio | 10 % gate |
|---|---|---|---|---|
| 60 | 27.413 | 27.875 | 0.983 | pass |
| 120 | 22.768 | 23.261 | 0.979 | pass |
| 180 | 17.282 | 17.823 | 0.970 | pass |
| 240 | 11.536 | 12.079 | 0.955 | pass |
| 300 | 7.360 | 7.625 | 0.965 | pass |
| 360 | 4.564 | 4.643 | 0.983 | pass |
| 420 | 2.942 | 2.816 | 1.045 | pass |
| 480 | 2.050 | 1.784 | **1.149** | **FAIL** |
| 540 | 1.445 | 1.079 | **1.340** | **FAIL** |

**G-CURVE is predicted to FAIL at 2 of 9 points.** The cause is predicted to be the camera target
(RaceCamera's pre-race framing vs the original's), not the entry dynamics, which G-TRANSCRIBE checks
separately. The gate is not loosened. Static: S1, S2 confirmed by `L2_MEAS.log`; S3 confirmed
(`draws_before=314` = 310 open discards + 4, `fc0=0`); S4 FAILS as worded (round-2 cd at 323 not 320:
round 1's own AI-tick draw count changes with the longer pre-race; S4 was report-only).


Measurement: knob arm with `MASHED_P3_MEASURE=800` (the release check ignored for 800 phase-3 frames;
entries do not feed the camera except through the flag, which stays 1 until release in both runs),
logging the port's `cam+4..+0xc` per phase-3 frame. That trace goes through `leg2_model.py port` ->
predicted release frame and curve, written here BEFORE the gate arm.

## 5. Gates

| gate | criterion |
|---|---|
| **G-CTRL** | knob OFF: step `c741a4c5`, gates `642af0ac`, ptrace `05984e30` |
| **G-STATIC** | log witnesses S1, S2, S3 exactly |
| **G-RULE** | round 1: phase 3 ends on the frame the flag clears; the clear is site (a) with all 8 input bytes 0; the logged `d` at the clear is < 0.02f and every earlier phase-3 `d` is >= 0.02f |
| **G-FRAME** | release frame = §4's model prediction ±2 |
| **G-TRANSCRIBE** | port entry-0 `(x,z)` per frame = `leg2_model.py` on the port's own logged target, within 1e-3 |
| **G-CURVE (cross-side)** | port `d` at f = 60, 120, ..., 540 within **10%** of the original's (`o_cadence_nopress.csv`) |
| **G-AFTER** | then 1 x state 4, 112 x state 5, exit read `0x3feeeee2`; rounds 2+ have no phase 3 (S5) |
| **G-PATH** | each new body: `run_diff` GREEN 5/5 non-degenerate + `run_verify_hook` PASS. RNG and non-repeatable limits named per body |

Report-only: (b), bias374, D2 watch, per-round cd (S4). Every timeline moves by about 650 frames in
round 1 (BRIEF §3); no scorer is re-anchored here.
