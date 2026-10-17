# RESULT pontoon (2026-10-16)

Branch `work/pontoon` from `race/first-frame-parity` 2cc15b2cf, worktree `D:\MashedWT\pontoon` (moved to D: when C: hit 0 bytes free,
same repo). Model claude-opus-5-5. PREREG committed before the first run (`PREREG_pontoon.md`, cc6b6383f).
Exes (sha256 first 16, `MASHED_NO_DEPLOY=1`): B = clean 2cc15b2cf `8BA05074A25126AE`; N1 `05A4B2B7649619B1`; N2
`562B0A1344559B86` (+ Tower/Deck hide, LutInvSqrt moved); **N3 = final `F9E4BCB0A5CAD47A`** (+ the split's sphere fix). Every run used a private `MASHED_ROOT` (`D:\MashedWT\pontoon_root`, a copy of `original\`).

## 0. Verdict

The Warzone track object at `0x00684d18` is ported to the exe end to end except the parts that need an RWP world the exe does
not have. **The original's per-call capture replays bit-identically through the port: 1073 of 1073 calls on a full
0 -> 1 -> 2 -> 3 -> 0 cycle (OP3, 602 carried car matrices), 38 of 38 on three quiet captures.** Every compared dword matches:
state, `+0xa4`, `+0xc8`, `+0x44`, movement, the per-car in-box flags, the words `0x0088ff50..5c`, `+0x54`, `[+0x94]`, all four
RpHAnim node matrices, the three boat atomics' LTMs, the interpolator time and next key, the carried car matrices, the car
markers and the tracked Tower / Deck matrices.

Default build: unchanged everywhere except one faithful render fix on Warzone (the Tower and Deck RWP objects are no longer drawn,
as slot 0 hides them). The object itself is behind three knobs, **default OFF**, because its exit path hands the car to an RWP
rigid body the exe does not model (U-9738). G-CTRL is the default on every arm; D2 stays in bounds.

## 1. What the bytes show (decode, static; PREREG section 0 has the full list)

- Hook record `0x005f3570` (Course_Id 0x21): slot 0 `0x004527f0` builds the object (`FUN_00451760`), slot 1 `0x00452b20` ticks it
  (`FUN_00451d80` at `0x00452ba2`, then `FUN_00451a50` for the Tower `0x684cd8` and Deck `0x684c98`), slot 2 `0x00452d10` draws the
  boat (`FUN_00452130`), slots 6 `0x00452af0` / 7 `0x00452ad0` reset it (`FUN_00451cc0`); both are called from `FUN_004111c0`
  (`0x00410195` / `0x0040fefe`). Slot 1 runs in `FUN_00426ab0` (`0x00426ae3`) after the frame's physics (`0x0040fc6c`).
- The object is an RpHAnim hierarchy (Boat.dff: 4 nodes, flags 0x3000) driven by BoatAnim.anm (74 std keyframes, 33.33 s) through
  RenderWare's RtAnim interpolator (`0x0052de60` / `0x0052e0e0` / `0x0052df70` / `0x0052e2c0`, interpolate `0x00532b80`, update
  `0x00533ec0`). Node 1 is the platform; `+0x94` points at its world matrix. Its 33 s path runs from (173.5, -1.43, 58.4) to
  (116.4, -1.43, 104.1).
- Trigger `FUN_00451820`, carry `FUN_00451990` / `FUN_00451ad0`: a car inside the box (stream atomic 1's geometry bbox, x in
  (-4.2605, 3.7417), z in (-4.1749, 3.8272), node-1 frame) is moved IN PLACE by the platform's delta (car * inv(old) * new).
- `FUN_0046be10(0xff804040, v, 0, on)` writes car marker slot 0; its one reader `FUN_0047d640` is the RWP contact filter: outside
  the box a car's RWP contacts against the platform's surface colour are dropped (U-9739).

## 2. The original, measured (`--pontoon-probe`, stock MASHED, Warzone rule 8 / 4, 4 cars, mode 10)

| arm | calls | cycles | rows kept |
|---|---:|---:|---:|
| OP1 (rule 8, 180 s) | 10176 | 0 | 38 |
| OP2 (rule 8, 240 s) | 13905 | 0 | 38 |
| OP3 (rule 4, 300 s) | 17771 | 1 (car 3) | 1073 |
| OP4 (rule 8, 300 s) | 17956 | 0 | 38 |

Predictions (`OP3_predictions.txt`): **P1 held** (units 50; box = the predicted geom-1 bbox bit for bit). **P2 held** (61 calls in
state 1, every `+0xa4` and `+0xc8` step exact in float32; 2 -> 3 on a row with all words 0 and c8 = 15.18 > 2.0; 3 -> 0 at c8 0).
**P3 held** (602 carried car-rows match car * inv(M54) * M94 within 9.2e-5 in float64; 3688 not-in-box car-rows unchanged).
**P4 failed**: car 3 was already in RWP mode (record `+0x14` = 1) at both exits, so the original added the movement to the RWP
body (`FUN_00481780`), not the record, and `FUN_00480720` did nothing. `+0x14` toggles on all four cars during a Warzone battle
(600-unit timers). **P5 answered**: a car aboard sits at y = -1.4307 (median of 541 rows; platform node y -1.4298), on COLLIDE.BSP's
-1.43 surface that runs under the whole path above the -2.445 river bed (`OP3_bsp_under_car.txt`, 37 of 55 sampled rows on that
surface, the other 18 while the car was lifted in RWP mode). So a normal-mode car rides on the BSP the exe already has; the Deck body
matters for RWP-mode cars.

## 3. Port (exe)

`Track/WarzonePontoon.{h,cpp}` (x87 TU, `Pontoon_Tick` at PC_24): the object at the original's address, RtAnim, RpHAnim update,
`FUN_00451760`, `FUN_00451d80` (all 6 cases), `FUN_00451820`, `FUN_00451ad0` + `FUN_00451990`, `FUN_00451ce0`, `FUN_00451cc0` +
`0x00452170`, `FUN_00451a50`, `0x004c4680` from the listing. Shared bodies: `0x004c4600` (Math/RwMatrixMultiplyCpu.cpp),
`0x004c52f0` (Collision/RwpSolverMath2.cpp) and `0x004c4dc0` (Render/RwMatrixInvert.cpp), the last two given an exe arm (the RW
device table is not mapped in the exe); `LutInvSqrt` (`0x004c3b90` with the real LUT) added to Math/RwLutTables.
TrackRenderer: the boat as three per-atomic props at the hierarchy LTMs (DFF atomics 2, 1, 0 = the clump list order, captured), the
tick before `ClumpAnimTick` (the original's `0x00426ae3` before `0x00426700`), reset at `StartRound`. VehiclePhysicsRun: `RecordPtr`,
`CarryTo` (the carried matrix goes to the ring's current half, the integrated basis and the position adapter).

Knobs (default OFF): `MASHED_PONTOON_ANIM` (object + animated draw, no words), `MASHED_PONTOON_STATE` (+ trigger and words),
`MASHED_PONTOON_CARRY` (+ in-box carry, exit velocity on the record branch). Default ON with opt-out: `MASHED_NO_PONTOON_HIDE=1`
(the Tower / Deck hide). Measurement: `MASHED_PONTOON_REPLAY` / `_REPLAY_OUT`, `MASHED_PONTOON_TRACE`.

## 4. Gates

| gate | result | file |
|---|---|---|
| replay OP3 (N1, N2, N3) | **1073 / 1073 calls, 0 differing dwords** | `arms/R2_replay.csv`, `R3_replay.csv`, `R4_replay.csv` |
| replay OP1 / OP2 / OP4 | 38 / 38 each | `arms/R2_OP*_replay.csv` |
| Tower rest matrix | the original's body matrix differs from TOWER.MTS by 3 dwords (1 after `0x004c4680`); Deck = DECK.MTS. The replay seeds the original's | `R2_replay.csv` header |
| G-CTRL TRAINING | B, N1 off, N1 all knobs, N2 default, N3 default, N3 all knobs: all **d3b90ee0 / 80f01f35 / 56d62ae7** | `arms/batch1_*.txt`, `batch2_N2.txt`, `batch3_N3.txt` |
| D2 (score_d2.py, bounds d81a8df6) | B 0.1890 / 0.2476 / 1912.01; N1 off x3 0.1890 / 0.2480-0.2483 / 1913.06-1914.88; N1 CARRY 0.1890 / 0.2483 / 1914.88: **all in** | `arms/SCORE_D2.txt` |
| Warzone live, knobs off | N1, N2, N3 default step dump = B (`ea4b8425`), seek trace = B (`e0fc7578`); ANIM only too (`W_PA1`) | `arms/batch*` |
| Warzone live, determinism | STATE `872cf716` on N1 and N3; CARRY `f84119ef` / `a88dbf57` / `49972586` on N1, N2, N3 | `arms/batch*` |
| no-op control | knobs off: no `pontoon` trace line, no word written (W_DEF2 / W_DEF3) | `arms/W_summary.txt` |

### Warzone live (3600 frames, rule 8; `analyze_w.py`)

| arm | object | AI reader `0x00414f00` |
|---|---|---|
| B / N off | inactive | 4201 calls, gate 1 passes 0, ret 1: 0 |
| N1 STATE | car 2 enters at call 1839; 61 calls state 1; 116 state 2 (c8 max 3.86, the platform leaves the uncarried car); 116 state 3; back to 0 at 2132 | gate 1: 201, **ret 1: 45, all with the word set** |
| N1 / N2 CARRY | car 2 enters at 1839; carried 970 calls in state 2 (c8 32.30 of 33.33); state 3 to the end | gate 1: 1380, **ret 1: 885, all with the word set** |

Original reference (seek414f00): rule 8 ret 1 on 29 / 20000 calls, car 3 set the word on 182 (rule 8) / 321 (rule 4) frames; OP3
here 456 calls of state 2. The exe's car stays aboard far longer under CARRY: that is the exe AI driving toward the platform (the
reader returns the platform's position) on a different trajectory, not the object, whose behaviour per call is the replay's.

### Draw rows and collateral (capfix replays, `run_tracks.py` private-root copy; `score_tracks.py` + `cites_merged.json`)

| arm | result |
|---|---|
| N3 default vs B, all 10 tracks | **30 of 30 draw files byte-identical**; 822 rows / 0 RED on 30 frames, both |
| N3 hide opted out (`MASHED_NO_PONTOON_HIDE=1`, on N2) vs B, Warzone | 3 of 3 byte-identical |
| N3 knobs on (ANIM, CARRY), Warzone | draw files differ (the boat is now three draws at the hierarchy LTMs instead of one baked model); **rows 25 / 22 / 32, matched 318 / 318 / 275 = B's**; the three boat atomics (475, 4, 52 triangles, texture `battle_platform3`) at AABBs equal to B's and the original's to 0.000 (`arms/BOAT_ROWS.txt`) |
| Tower / Deck | no draw inside the platform area in the original, B or N3 on the three frames: the hide is invisible there (the default draw files are byte-identical to B); it is the original's slot-0 rule (`FUN_0047d100(.., 0)`), so frames that do see the platform no longer draw the two objects the original never draws |
| instrument gates | FAIL on B and N3 alike (the known original-side I-NAME on Neustein 1169, 73 / 77); `gates_sc.txt` hash-identical; PASS on the Warzone-only scorings |

Found on the way: the original draws each boat atomic twice per frame (2 x 475, 2 x 4, 2 x 52), the exe once; no row is raised
for it (the boat is fully matched). A first knobs-on build dropped the 52-triangle atomic: `DffModel` keeps an atomic's sphere in
clump space even unbaked, so the librw walk's `WorldSphere` x instance sent it to (355, ...) and `DistanceKeeps` culled it. Fixed
(`AtomicSphere::c_geo`, used only by the split path); every number above is from the fixed build N3 `F9E4BCB0A5CAD47A`.

### Rebased onto d328adb19 (txdscope + d3b + rres): authoritative

The branch was rebuilt on `d328adb19` (cherry-picks; conflict in VehiclePhysicsRun.{h,cpp} resolved keep-both with d3b's
`VehiclePhysics_BodyBasis`; tracker files taken from upstream and the transaction re-applied with the U-ids renumbered to
U-9738..U-9742). B2 = clean `d328adb19` `2744E47ED9FC4447`, N4 = this branch `84695779B3105189`, both `MASHED_NO_DEPLOY=1`.

| gate | B2 | N4 |
|---|---|---|
| replay OP3 | - | **1073 / 1073** (`arms/R5_replay.csv`) |
| G-CTRL TRAINING | d3b90ee0 / 80f01f35 / 56d62ae7 | default and all knobs: **d3b90ee0 / 80f01f35 / 56d62ae7** |
| D2 | 0.1890 / 0.2482 / 1913.06 | default x2 0.1890 / 0.2480-0.2483 / 1912.04-1914.88; CARRY 0.1890 / 0.2476 / 1912.01; all in (`arms/SCORE_D2_rebased.txt`) |
| 13-track replay (`verify/rsky_20261016/run13.py`, `cites_rres.json`) | 867 rows, 0 RED, 39 frames; Warzone 24 / 21 / 27 = 72 | **39 of 39 draw files byte-identical to B2**, 867 rows, 0 RED; gate files hash-identical (the pre-existing FAIL) |
| Warzone with CARRY (N4) | - | **24 / 21 / 27 = 72 rows, matched 326 / 326 / 296 = B2's**; the three boat atomics at B2's AABBs |
| Warzone live | W_DEF20 `ea4b8425` | default `ea4b8425`; STATE and CARRY hashes equal the pre-rebase ones |

## 5. Not ported (filed)

- **U-9738** exit hand-off (`FUN_00480720`, `FUN_00481780`, `FUN_00481750` x4): no per-car RWP rigid-body mode in the exe.
- **U-9739** Tower / Deck RWP bodies and the per-car contact filter (`FUN_0047d640`): no scene-object physics in the exe.
- **U-9740** state 5's frustum test (only after 33.33 s aboard): the tick has no camera; the port resets at once.
- **U-9741** two exe bodies each for `0x004c4680` (D2's is not a transcription) and `0x00532b80` (Dump train's sums the dot in the
  reverse order); allowlisted, consolidation moves closed gates.
- **U-9742** `FUN_00452140`'s six callers and an original draw capture with the platform moving.

## 6. User decisions

1. **Turn the platform on by default (`MASHED_PONTOON_CARRY`, which implies STATE and ANIM)?** For it: the object is bit-identical
   to the original per call, normal-mode cars ride it on the BSP surface the exe already has, TRAINING (G-CTRL, D2) and the other
   nine tracks do not move, and the AI reader starts firing on Warzone exactly when its inputs say so. Against: at the exit the
   original hands the car to an RWP rigid body for 600 units (U-9738), the exe keeps it in normal mode, and RWP-mode cars ride the
   Deck body the exe lacks (U-9739). Recommendation: ON once the user accepts the exit approximation for Warzone; until then OFF.
2. **Unify the two dual bodies (U-9741)?** Each moves a closed gate (D2 for `0x004c4680`, the Dump capfix rows for `0x00532b80`).

## 7. Hygiene

C: went to 0 bytes during setup; the C: worktree (mine, empty) was removed with `diag.py wt-remove` and recreated on D:; the parent was
told. Only PIDs this lane spawned were stopped. MB-sized step dumps / traces were deleted before the commits (sha8 lines in the
batch logs). Base worktree `D:\MashedWT\pontoon_base` (detached at 2cc15b2cf) holds the B build.
