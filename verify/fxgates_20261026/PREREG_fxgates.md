# PREREG D4-FX-GATES: the original's skid-emission gates behind `MASHED_FX_ORIG_GATES` (2026-10-26)

Committed alone, before any arm runs. Ledger step D4-FX-GATES (W0-13), branch `orch/D4-FX-GATES`.

## 1. What the original does (static, anchored binary, SHA-256 BDCAE093...3C0E)

The original has no "skid smoke" emitter of its own. Its per-car wheel effects come from one per-frame driver,
`FUN_0040ca00` (0x0040ca00..0x0040cd8b, called from the per-frame dispatcher `FUN_0040ce00`), which calls the
surface-keyed spawner `FUN_0040c690` (0x0040c690..0x0040c9f9), which spawns one record with `FUN_0048cf90`
(0x0048cf90, per-id 256-slot ring). Gates, with the instruction that applies each one:

| gate | site | constant |
|---|---|---|
| car slot word `[0x007f1a14 + v*0x10] >= 0` | 0x0040ca31 `cmp ecx,-1 / jle` | -1 |
| `(int)rec+0x9e0 > 1` (`FUN_0046cc10` = `__ftol` of +0x9e0) | 0x0040ca74 `cmp eax,1 / jle` | 1 |
| branch on `rec+0xcfc` | 0x0040cabe `cmp [ebp],0` (ebp = 0x008856ac - 0xd04*(4-v) = rec+0xcfc) | 0 |
| `rec+0xcfc == 0`: `|rec+0x9b0..+0x9b8|^2 >= 62500` (`FUN_0046cb30`) | 0x0040cae0 `fcomp [0x005ccd20]`, `jnp` skip | 62500.0f (0x47742400) |
| skid if `rec+0xb0c > 100` (`FUN_0046d6a0`) | 0x0040caf5 `fcomp [0x005cc568]`, `test ah,0x41` | 100.0f (0x42c80000) |
| or skid if range `< 90` (`FUN_0046d570`) and brake byte `> 0x40` | 0x0040cb21 `fcomp [0x005ccad0]`; 0x0040cb37 `cmp byte [idx*0x4c+0x007f103d],0x40 / jbe` | 90.0f (0x42b40000); 0x40 |
| `rec+0xcfc != 0`: skid and emit iff `rand(0,1) >= 0.3`, else no call | 0x0040cb49 `FUN_00472650`, 0x0040cb4e `fcomp [0x005cc99c]` | 0.3f (0x3e99999a) |
| one `FUN_0040c690` call for wheel 0 (`[0x007f101c]` even) or wheel 2 (odd); two more (wheels 1, 3) when skid | 0x0040cb67..0x0040cbcd; 0x0040cbd5 `test edi,edi` | bones 0x1d / 0x1e, 0x20 / 0x1f |
| `FUN_0040c690`: return if `[0x00636adc] != 0` | 0x0040c6bd | (copy of `[0x007733b0]`, `FUN_00499400`) |
| roll threshold p, colour = `rec+0x1f0 + w*0xc4` (`FUN_0046d320`): `0xff968080` on track 0x1e (`FUN_00426c00`): skid 0.3 / else 0.1; `0xffc88080`: skid 0.1 / else 0.05; any other colour: skid 0.7 / else 0.4 | 0x0040c6e6..0x0040c734 | 0x005cc99c 0.3, 0x005cc56c 0.1, 0x005cc9a0 0.05, 0x005cc340 0.7, 0x005ccac0 0.4 |
| emit iff `roll <= p` | 0x0040c73e `fcomp st(1)`, `test ah,0x41 / je return` | |
| colour `0xff968080` (id 6) emits only if skid or `[0x007f0a04 + v*0xc] != 0` | 0x0040c8e8 | |

`FUN_0046d570` returns 0 only when v >= 16 or all three of +0x9b0..+0x9b8 are 0.0 (0x0046d578..0x0046d5d4), which the
62500 gate excludes, so its return test (0x0040cb19) never skips under the gate. Record fields are the port's
0xd04 records (`Vehicle::VehiclePhysics_RecordF32/I32`): +0x9e0 grounded-wheel count, +0x9b0 velocity, +0xb0c slide
measure (`VehicleControl.cpp:213`), +0xcfc both-pedals flag (`BodyOrientationIntegrate.cpp:254`).

## 2. The knob

`MASHED_FX_ORIG_GATES=1` (default OFF): in `TrackRenderer::EmitCarFx` the scaffold skid branch (yaw-rate x speed /
slip vs `R*0.12` / `R*0.02`) is replaced by the gate tree above, one smoke particle per passing roll, same visual.
Not modelled, by design:
- the rolls use a private generator; the original draws them from the shared PRNG `FUN_00472650`. A knob-ON run
  therefore cannot move any simulation value, and counts compare in distribution, not draw for draw;
- `[0x00636adc]` taken as 0; `[0x007f0a04 + v*0xc]` (the launch counter, set at the countdown end from rec+0xbf4 by
  `FUN_004103a0`, decremented by 0x18 per frame in `FUN_0040ca00`) taken as 0; `[0x007f101c]` parity replaced by a
  per-slot frame counter's parity; the per-id ring cap of `FUN_0048cf90` not modelled (counts are spawner calls).
- the port's surface key `rec+0x1f0` is 0 (`VehiclePhysicsRun.cpp:282`, collision material colour not retained), so
  every port wheel takes the "any other colour" row. `MASHED_FX_ORIG_SURFACE=<hex>` (diagnostic only, never a flip
  candidate) stands in a constant key for a 0 record key.

Impact sparks are untouched. With `MASHED_FX_DEBUG` set, the existing per-slot line in `log/fx_debug.txt` carries
the knob's cumulative counters (frames, spawner calls split by skid flag, emits split by skid flag); with the knob
OFF they stay 0 and the line is otherwise unchanged.

## 3. Arms

1. Gates, knob OFF on this branch's exe: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`,
   under `mashed_lock.py run --label D4-FX-GATES`.
2. Port FX counts, G-CTRL recipe (H_DEF env, 14400 frames) + `MASHED_FX_DEBUG`: F0 knob OFF, F1 knob ON,
   F2 knob ON + `MASHED_FX_ORIG_SURFACE` = the modal wheel colour of the original capture (arm 3), pooled over cars.
3. Original capture: `re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 1 --rule 0` (Training),
   private root, plus an entry counter (CModule): `FUN_0040ca00` entries (frames), `FUN_0040c690` entries by
   (car, skid flag, colour), `FUN_0048cf90` entries whose return address lies in `FUN_0040c690`, attributed to the
   preceding `FUN_0040c690` call. Piz-collision crashes are re-runs.

## 4. Predictions (scored in the RESULT)

- P1: arm 1 reproduces `re/orchestrator/merge_train.json` refs exactly (G-CTRL `gctrl.ref`, B `b.ref`). A knob-OFF
  exe is required to be byte-identical in behaviour.
- P2: knob OFF (F0) per-slot skid counts equal the current default's (`fx_skids_` unchanged code path).
- P3: in the original capture, each AI car's (cars 1..3) emits per `FUN_0040c690` call are below 0.4 (the lowest
  "any other colour" rate), i.e. Training's wheel colours sit mostly in the lower rows. Refuted if any AI car's
  emit/call ratio is 0.4 or more.
- P4: F1 (port surface 0) emits per 1000 frames for AI slots 1..3 exceed the original's by more than 2x.
- P5: F2 (surface stand-in) brings each AI slot's emits per 1000 frames within [0.5x, 2x] of the original's.

A default flip is not part of this step. If P5 holds, the decision row names wiring the real surface key
(collision material colour) as the precondition for a flip.
