# PREREG pontoon (2026-10-16)

Lane: port the Warzone track object at `0x00684d18` (course 0x21), user decision 2026-10-16. Branch `work/pontoon` from
`race/first-frame-parity` 2cc15b2cf. Worktree `D:\MashedWT\pontoon` (moved off C: when C: reached 0 bytes free; same git repo).
Model claude-opus-5-5. Written before the first measuring run.

## 0. What the bytes show (static, unpatched image; files in this directory)

Files: `decomp_batch1.txt`, `decomp_batch2.txt` (decomp_pc `--create`, Ghidra missed most of these), `datarefs1.txt`,
`disasm_pontoon.txt` (0x00451680..0x00452180), `disasm_slots.txt` (0x00452ad0..0x00452e00), `disasm_rtanim.txt`,
`disasm_hanim_update.txt`, `dump_boat.py` (BOAT.DFF / BOATANIM.ANM structure), `bsp_probe.py` (COLLIDE.BSP heights).

- Hook record `0x005f3570` (Course_Id 0x21): slot 0 `0x004527f0`, 1 `0x00452b20`, 2 `0x00452d10`, 4 `0x0045b350`, 5 `0x00452e00`,
  6 `0x00452af0`, 7 `0x00452ad0`, 10 `0x00452da0` (memory read).
- Slot 0 (`0x004527f0`): Tower = `FUN_0047ce40(1000)` -> `DAT_005f978c`, Deck = `FUN_0047ce40(2000)` -> `DAT_005f9790`;
  `FUN_00451760(&obj, course+0x10114 (clump 1 = Boat.dff), course+0x105b0 (General_Anim 0 = BoatAnim.anm))`;
  `FUN_00451680(&obj, 0x00452170)` stores the reset callback at `obj+0xc4`; Tower / Deck body matrices copied to `0x00684de8` /
  `0x00684c58`; both RENDER bits cleared (`FUN_0047d100(..,0)` -> `FUN_004b5240`, atomic+2 bit 4).
- `FUN_00451760`: obj+0 clump, +0x4c anim, +0x44 = 0, +0x50 = hierarchy (`FUN_004b4790` -> `FUN_004b4760`: flags |= 0x3000,
  attach; SetCurrentAnim `FUN_0052de60`; update `FUN_00533ec0`), +0x48 = 0, loop callback `FUN_0052df20(interp, 0x00451690, obj)`,
  atomics by `RpClumpForAllAtomics` order into +4/+8/+0xc (`FUN_004516d0`), +0x10 = frame of +8, +0x14..+0x2c = RwBBox of +8's
  geometry vertices (`FUN_00451730` -> `FUN_004c1740`), time 0 (`FUN_0052e2c0`), update, +0x38..+0x40 = node 1 position,
  +0x94 = &matrixArray[1] (hier+8, +0x40), +0x54 = copy, +0xc4 = 0 (then set by FUN_00451680).
- Slot 1 (`0x00452b20`, units = int ticks, dt = units * 1/3000 `0x005cc948`): `FUN_00451d80(&obj, units)` at 0x00452ba2, then
  Tower `0x684cd8` and Deck `0x684c98` := M * inv(obj+0x54) * [obj+0x94], ortho-normalized (`FUN_00451a50`), set as body matrices
  (`FUN_0047d180`) and the bodies' velocity rows zeroed. The other slot 1 calls (craters, water UV, trees) are outside this lane.
- `FUN_00451d80` (6 cases, jump table 0x00452118), `obj+0x4c != 0` gate:
  - 0: SetCurrentTime(0), +0xc8 = 0, update, `FUN_00451ce0` (sample node 1: +0x38..0x40 pos, +0x94 ptr, movement +0xa8..b0 =
    (pos - (+0x84..0x8c)) * 360.0 `0x005ccac4`), +0x54 := [+0x94], `FUN_00451820` (trigger), +0xa4 = 1.0, `FUN_00451ad0`.
  - 1: +0xa4 -= units * 1/3000 (`0x005ce388`); < 0.0 -> 2. +0x54 := [+0x94], +0x44 = 0, SetCurrentTime(0), +0xc8 = 0, update,
    `FUN_00451ce0`, `FUN_00451ad0`.
  - 2: +0x54 := [+0x94]; d = units * 1/1500 (`0x005ce384`); +0xc8 += d; +0x44 = d; AddAnimTime + update (`FUN_004b47c0`);
    `FUN_00451ce0`; `FUN_00451ad0`; all four `0x0088ff50..5c` == 0 and +0xc8 > 2.0 (`0x005cc574`) -> 3.
  - 3: +0x54 := [+0x94]; d as 2; +0x44 = d; +0xc8 -= d; SubAnimTime (`FUN_0052df70`); +0xc8 <= 0 -> `FUN_00451cc0` (state 0 +
    callback 0x00452170: Tower/Deck reset to 0x684de8 / 0x684c58), +0xc8 = 0, SetCurrentTime(0); update; `FUN_00451ce0`; `FUN_00451ad0`.
  - 4: +0x54 := [+0x94]; +0x44 = units * 0.0; AddAnimTime(0) + update; `FUN_00451ce0`; `FUN_00451ad0`. (no writer of 4 found)
  - 5 (set only by the loop callback 0x00451690 when the 33.33 s anim wraps): frustum test of atomic +8's sphere; outside -> reset.
- Trigger `FUN_00451820`: per live car (`FUN_0040e340`), car ring matrix (`FUN_0046d4a0`) * inv([obj+0x94]); x in (+0x20,+0x14) and
  z in (+0x28,+0x1c) strictly -> +0x48 = 1. Also `FUN_0046be10(0x005f9788, v, 0, 0)`: car marker slot 0 colour 0xff804040, off.
- Coupling `FUN_00451ad0` (disassembled 0x00451ad0..0x00451cbd): per live car, `FUN_00451990(obj, carMat, carMat)` IN PLACE:
  local = car * inv(obj+0x54); inside the box -> car := local * [obj+0x94] (the car rides), word `0x0088ff50[v]` = 1, marker off;
  outside -> word 0, and if `obj+0xb4[v]` (inside last call) was 1: `FUN_0046bf50(obj+0xa8, v)` (record +0x14 == 0: record
  +0x9b0..b8 += movement; else the RWP body's velocity), per wheel w = 0..3 outside the box `FUN_00481750(v, (0,-75,0), &wheel.pos)`
  (RWP body impulse), `FUN_00480720(v)` (record +0x14 0 -> 1, +0x18 = 600, car matrix and velocity handed to the RWP body,
  record velocity zeroed); marker on. `obj+0xb4[v]` := inside.
- BOAT.DFF: 5 frames, HAnim PLG on frame 1 (4 nodes: (0,0,0) (1,1,0) (2,2,3) (3,3,1), keyFrameSize 36); atomics (frame 3, geom 0),
  (frame 4, geom 1: 6 verts, the box), (frame 2, geom 2: platform). BOATANIM.ANM: 74 std keyframes, duration 33.3333. Node 1 path
  (173.48,-1.43,58.45) at 0 -> (174.42,61.93) at 3.33 -> (116.39,104.12) at 33.33, y constant -1.43.
- Predicted box (geom 1 RwBBox): sup (3.7417 `0x406f7803`, 0, 3.8272 `0x4074f0d8`), inf (-4.2605 `0xc0885604`, 0, -4.1749 `0xc08598c8`).
- COLLIDE.BSP has surfaces at y -1.4 and -2.4 along the path (bsp_probe.py).

## 1. Scope and what cannot be ported here

Ported (exe, `Track/WarzonePontoon.{h,cpp}` + renderer hook-up): slot 0 object init, slot 1 object tick + Tower/Deck matrix
tracking (data only), slots 6/7 reset + callback 0x00452170, the RtAnim interpolator calls used (0x0052de60 / 0x0052e0e0 /
0x0052df70 / 0x0052e2c0, std keyframe interpolate 0x00532b80, keyframe->matrix 0x00532a60), RpHAnim update 0x00533ec0 for Boat.dff,
the trigger, the in-box carry, the exit velocity add (record +0x14 == 0 branch), the words, the markers' fields, and drawing the
three boat atomics at their animated LTMs.

NOT portable in this lane (no exe counterpart): the exit RWP hand-off `FUN_00480720` and wheel impulses `FUN_00481750` (the exe has
no per-car RWP rigid-body mode; `VehicleCouplingBridge` is inert in the exe), the Deck / Tower collision bodies (no scene-object
physics in the exe), state 5's frustum test if the camera planes are not reachable from the tick (it is reached only after 33.3 s
aboard). Each gets a U-row.

Knobs (all three default OFF while measuring): `MASHED_PONTOON_ANIM=1` (object init + tick + animated draw, words NOT written),
`MASHED_PONTOON_STATE=1` (state machine trigger + words `0x0088ff50`, `0x00684de0`, `0x00684dac` live), `MASHED_PONTOON_CARRY=1`
(in-box carry + exit velocity). STATE implies ANIM, CARRY implies STATE. Measurement-only: `MASHED_PONTOON_REPLAY=<capture csv>`,
`MASHED_PONTOON_TRACE=<abs path>`.

## 2. Arms

- **OP (original, stock MASHED, private PID):** new `--pontoon-probe` in `re/frida/scenario_launch.py`: entry+leave of
  `FUN_00451d80` with every input and output above as raw dwords (full rows for the first 8 calls, then a 30-row look-back ring
  flushed around any row with a non-zero state, word or `+0xb4` flag). Recipe of seek414f00 O3 (Warzone, rule 8, 4 cars, mode 10),
  hold 180 s. Re-run (not report) on a piz-collision crash. At least one 0->1->2->3->0 cycle needed; up to 3 attempts.
- **R (replay):** the new exe with `MASHED_PONTOON_REPLAY=OP.csv`: for each captured segment, seed the object from the segment's
  first (state 0) entry row, feed each row's entry car matrices and units, compare every output dword with the capture's leave row.
- **B / N (exe):** B = clean build of 2cc15b2cf here; N = this branch, knobs OFF; N+A, N+S, N+C = knobs ON cumulatively.

## 3. Predictions (written before any run)

OP:
- P1 units = 50 on every row; `obj+0x4c` non-zero; box dwords = the predicted geom-1 bbox above (+0x18 / +0x24 = 0).
- P2 per cycle: one 0->1 row with some word = 1; state 1 holds 61 calls (+0xa4 = 1.0 - n*50*`0x39ae9681`, leaves at the first
  negative); in state 2 +0xc8 grows by 50*`0x3a2e9681` per call; 2->3 only on a row with all four words 0 and +0xc8 > 2.0; 3->0
  on the row where +0xc8 reaches <= 0.
- P3 every row with word[v] = 1 at leave: car v's leave matrix == (enter * inv(+0x54 at leave)) * [+0x94 at leave], RW op order.
  Word 0 and +0xb4[v] = 0: car matrix unchanged by the call.
- P4 an exit row (+0xb4[v] 1 -> 0): record +0x14 0 -> 1, +0x18 = 600, record +0x9b0..+0x9c4 = 0 at leave.
- P5 (descriptive, no direction predicted): car y on the platform in states 2/3 against the BSP surface under it.

R:
- P6 bit-identical on every compared dword (state, +0xa4, +0xc8, +0x44, +0xa8..b0, +0xb4, words, +0x54, [+0x94], all 4 node
  matrices, the 3 atomic LTMs, carried car matrices, Tower/Deck). If not, the result reports the count and max ULP, no claim.

Exe:
- P7 N (knobs OFF) = B: G-CTRL TRAINING d3b90ee0 / 80f01f35 / 56d62ae7; D2 metrics (`verify/d2settle_20261016/score_d2.py`)
  identical; Warzone draw files byte-identical to B; the 9 other capfix tracks byte-identical.
- P8 N+C on TRAINING: G-CTRL and D2 identical to B (object only on course 0x21). Other tracks' draw files byte-identical (risk
  flagged: memory sim-trajectory-is-code-layout-sensitive).
- P9 N+A on Warzone capfix frames 584 / 624 / 659: the boat rows unchanged against B (state 0, t = 0 pose = the DFF frames).
- P10 N+S / N+C live Warzone (`run_exe_arms.py warzone`, 3600 frames): if a car enters the box, the trace shows a 0->1->2->3->0
  cycle with the P2 timing, the AI reader (`MASHED_SEEK_TRACE`) passes gate 1 on rows with +0xc8 in [0.5, 30] and returns 1 only
  with the word set. No direction predicted for whether a car enters it (O1 had none in 120 s).
- No-op control: N with every knob OFF and `MASHED_PONTOON_TRACE` set writes no object row.

Default rule: a stage goes default ON only if R is bit-identical for it, P7/P8 hold and the live run shows no crash. CARRY can
not be complete (exit RWP hand-off, Deck body missing): it stays OFF with a recommendation unless the user decides otherwise.
