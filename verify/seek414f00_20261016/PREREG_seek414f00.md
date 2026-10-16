# PREREG seek414f00 (2026-10-16)

Written before the first measuring run. Base `race/first-frame-parity` 9b024812. Model claude-opus-5-5.

## 0. Static facts this rests on (read-only pool slot, decomp_pc.py, raw-image scans of `MASHED.exe.unpatched`)

Files: `decomp_414f00.txt`, `datarefs.txt`, `decomp_451ad0.txt`, `decomp_warzone_obj.txt`, `datarefs_684d18.txt`.

- **The task premise is inverted.** `FUN_00414f00` is a READER of both words, not their producer: `FUN_00452eb0` (`fld [0x00684de0]`,
  `0x00452eb0`) and `FUN_00452ea0` (`[0x0088ff50 + i*4]`, `0x00452ea4`). It writes only `*param_3` / `param_3[1]` (`0x00414f00` body).
- The only writer of `0x0088ff50[v]` is `FUN_00451ad0` (`0x00451b47` = 1, `0x00451b5b` = 0), value = `FUN_00451990(obj, car matrix)`,
  an XZ box test of the car in the object's frame (`obj+0x14/+0x20` x bounds, `+0x1c/+0x28` z bounds).
- `FUN_00451ad0`'s six callers are the six cases of the state machine `FUN_00451d80(obj, dt)` (jump table `0x00452118`, state
  `[obj+0x48]` 0..5, gated on `[obj+0x4c] != 0` at `0x00451d8e`). `FUN_00451d80` has ONE caller, `0x00452ba2`, inside `0x00452b20`,
  which pushes `obj = 0x00684d18`. `0x00452b20` is referenced only from `0x005f3578`, slot 1 of the per-track hook record whose
  Course_Id is `0x21` (`0x005f3570`). So `0x0088ff50` can be non-zero only on Warzone.
- `0x00684de0` = `obj+0xc8` and `0x00684dac` = `obj+0x94` for `obj = 0x00684d18` (no absolute-address writer exists for either:
  every `.text` immediate in `0x00684c00..0x00684e40` was listed). `obj+0xc8` is written in states 0..5 of `FUN_00451d80`
  (`+= dt*0.000666` in state 2, `-=` in state 3, `= 0` elsewhere); `obj+0x94` = `FUN_00550bc0(obj+0x50)+0x40` in `FUN_00451ce0`.
- `FUN_00414f00` callers: `0x00416478` (in `FUN_00416250`) and `0x00417fc8` (in `FUN_00417da0`), both behind `FUN_00426c00() == 0x21`.
  `FUN_00416a30` (rules 4 and 9) has no mode-10 block.
- Constants (unpatched `.rdata`): `0x005cc32c` = 0.5, `0x005cc72c` = 30.0, `0x005cc348` = 1.5, `0x005cc564` = 0.25.
- The exe's `ControlStep` (`Ai/AiStandalone.cpp`) has no `LAB_00416462` block, and no exe TU writes `0x0088ff50` or `0x00684de0`.
- D3 (b) runs on TRAINING (Course_Id 30, rule 4, `run_fk.ps1`), which dispatches to `FUN_00416a30`.

## 1. Capture (original, stock, no hooks): `scenario_launch.py --seek-probe`

New read-only probe: entry+leave on `FUN_00451d80` (one row per frame) and `FUN_00414f00` (every input the body reads plus EAX and
`cand`). Arms, 4 cars, mode 10, hold 120 s, `--statediff-out` (needed to arm the probe):

| arm | track | rule | predicted obj rows | predicted seek calls |
|---|---|---|---|---|
| O1 | 11 Warzone | 0 | one per race frame | > 0 (AI cars 1..3, `FUN_00416250`) |
| O2 | 11 Warzone | 4 | one per race frame | **0** (`FUN_00416a30` has no mode-10 block) |
| O3 | 11 Warzone | 8 | one per race frame | > 0 (`FUN_00417da0`, call `0x00417fc8`) |
| OC | 0 Training (control) | 0 | **0** | **0** |

No prediction is registered for whether `0x0088ff50` goes non-zero or whether `FUN_00414f00` returns 1. That is the question the
parent asked to have measured, and dualcopy3's `wrote=0` (0x00417640 never wrote on Warzone) is consistent with both answers
(the brake skips when the committed mode is 10). The answer decides the replay's coverage, not whether the port is written.

## 2. Port

`Ai/AiPowerupSeek.cpp`, one body in both rsp lists (the AiPostStepBrake.cpp shape), export `AiPowerupSeek`, `RH_ScopedInstall` at
`0x00414f00` in the .asi. Exe: the mode-10 block at the `LAB_00416462` point of `ControlStep` (and `ControlStepM8`), behind
`MASHED_NO_SEEK414F00` (default ON, opt-out) if every gate below passes, else default OFF with a recommendation.
The two discarded calls (`FUN_0046d6d0`, `FUN_0046d570`, U-7574) are kept in the .asi (they are the original's callees) and checked
for side effects before being dropped from the exe.

## 3. Gates and predictions (port)

- **G-BODY** (the body): exe-side replay of every captured `FUN_00414f00` row (inputs -> EAX, `cand`) through the port, bit-exact.
  If the natural capture never reaches the `ret = 1` branches, add contrived rows (the replay sets the inputs directly) and say the
  branch coverage is contrived. Plus .asi path1/path2 if a registry arg_type fits without a new handler.
- **G-CTRL** (exe, TRAINING): `2b4d47bb / 356821c8 / f7a155e1` on base and new. Prediction: identical (the block is behind
  `track_index() == 0x21`).
- **D3 (b)** per regime (K = 0,1,2,4,26,27,54,57): prediction **byte-identical step dumps, A 1/3, B 2/3, C 0/3 unchanged** (rule 4 on
  TRAINING runs `ControlStepM49`, which has no mode-10 block, and the track gate fails anyway).
- **Warzone exe** (3600 frames, step + gates): prediction **identical to base** (`b726286f / b6360eef` per dualcopy3): in the exe
  `0x00684de0` is never written, so `0.5 <= 0.0` fails at gate 1 and the port returns 0 on every call.
- **Mode-7 entries**: unchanged (prediction from the same argument).
- **N0** (no-op control): the new build with `MASHED_NO_SEEK414F00=1` equals base on every arm above.
- **Knob-took witness**: a counter of port calls / returns logged at exit; Warzone must show calls > 0 with the knob on and 0 with it off.
