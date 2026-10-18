# RESULT u9755: the spawn y via FUN_0046d400 (2026-10-18)

Branch `work/u9755` from `race/first-frame-parity` `79c7e1bf1` (default G-CTRL `b1f49e7a / b1be0054 / d3a20f4c`). Model
claude-opus-5-5, account2. PREREG `PREREG_u9755.md` (`a2326b095` with the knob), amendments 1 (`f0d69430b`) and 2
(`03a7f8127`), each committed before the arms it governs. Scored exe `388fd4b2` (`MASHED_NO_DEPLOY=1`), private root
`D:\MashedWT\u9755_root`. Original references: o_wobj2, new captures `o_sdrop1` (`--spawn-drop-probe`).

## 0. Verdict

1. **The spawn pose is now the original's for all four cars, bit for bit** (`MASHED_SPAWN_Y_LINE`, default OFF):
   - `FUN_0046d400` is ported over the collision replica: the line walk `FUN_00538d60` generated verbatim (`BW_538d60`,
     `re/tools/naked_specs/collworld_line_u9755.json`, dispatched as `FUN_00538c80` case 1 under PC_24) and the callback
     `FUN_0046d3a0`. New original-side probe `scenario_launch.py --spawn-drop-probe` (`o_sdrop1`): caller `0x00470517`,
     param_3 1.17499995 for every car, best `0x406f3466 / 0x406d02b4 / 0x406acd3a / 0x40689b88`; the port's
     `MASHED_SPAWN_Y_LOG` gives the same inputs and the same four best bits.
   - The grid point is `FUN_00408b00`'s, x / y / z in its operation order (`RoundGrid_GridPoint`). `place()` had combined
     the lateral terms into one multiplier, which put car 2's z 1 ulp off (0.202357292 against 0.202357411).
   - The exact y reaches the record and the physics without the caller convention (`y + car_ground_off_` could not carry
     0.471709967 back out; it came back as 0.471709996): `VehiclePhysics_SeedSpawnPose(..., physY)` writes it and seeds the
     d2posmodel `g_physPos` / `g_outPos` pair.
2. **U-9755's premise is refuted.** The call-0 y gap (2.09e-7) is not the spawn y: the port's spawn settle returns
   0.506262839 (physics 0.468762839) for car 1 from the old and the exact spawn y alike (`MASHED_SETTLE_DIAG`). The
   original's y after `FUN_004704c0`'s solver pass `FUN_0046f6c0(0)` is 0.468762636 (frame 2), held through the countdown,
   0.468502462 after the first racing step. The gap is made in that spawn solver pass: new U-9756. Car 1's drift is unchanged
   (1.2e-4 at call 21).
3. **Default-ON candidate.** `SY` alone passes every default-ON gate: (b) the default's pass set, D2 3/3, 13 tracks 0 RED. It
   moves G-CTRL to `b7534619 / fb690fb1 / d3a20f4c` (player trace unchanged). Recommendation: default ON (user decision).

## 1. Gates

| gate | measured | verdict |
|---|---|---|
| G-NOOP | `B_DEF_K0/K1/K26` `e332af0c / 01ad5975 / 772f9fb7` on every exe | PASS |
| G-SPAWNY | `B_TPPGSY_K1` countdown pose cars 0..3 = o_sdrop1 (car 1 y 0.471709967, car 2 z 0.202357411) | PASS |
| G-Y0 | `SPLSBKNSYR1` car 1 `+0x95c` at call 0 0.468502671 against 0.468502462 | FAIL (spawn solver pass, U-9756) |
| G-DRIFT | `SPLSBKNSY` car 1 1.2e-4 at call 21 | FAIL (unchanged) |
| G-B | `SY` A 2/3 (cars 2, 3), B 2/3 (cars 1, 2), C 0/3; `SPLSBKNSY` 0/3 x3 | PASS for `SY` |
| G-D2 | `D2_SY_1..3`: slip 0.1941 / 0.1944 / 0.1941, 0.2545 x3, driving-median 1962.62 / 1963.42 / 1962.62, H1 398/400 | PASS 3/3 |
| G-13 | 39 frames, 869 rows, 0 RED, instrument gates PASS (`D:\MashedWT\u9755_data\tracks_sy`) | PASS |
| G-CTRL | `H_SY` `b7534619 / fb690fb1 / d3a20f4c` | moves (AI rows only) |

## 2. Code

- `Collision/CollisionWorldLine_naked.inc` (generated), `Collision/CollisionWorld.cpp/.h`: case-1 dispatch, `CollWorld_SpawnY`
  (`FUN_0046d400` + `FUN_0046d3a0`), `MASHED_SPAWN_Y_LOG`.
- `Race/RoundGrid.cpp/.h`: `RoundGrid_GridPoint`.
- `D3d9Render/TrackRenderer.cpp`: `place()` under the knob; per-car exact y to the countdown `SeedSpawnPose`.
- `Vehicle/VehiclePhysicsRun.cpp/.h`: `SeedSpawnPose(..., physY)`.
- `Race/PathProgStep.cpp`: the gate log prints positions at `%.9g`.
- `re/frida/scenario_launch.py`: `--spawn-drop-probe`.

## 3. Tracker transaction (re-classify, no C-level change; `apply_trackers.py`)

U-9755 RESOLVED (spawn y ported, premise refuted); new U-9756 (the spawn solver pass y); hooks.csv notes on `0046d400`,
`00538d60`, `00408b00`, `004704c0`; one CHANGELOG line.

## 4. User decision needed

`MASHED_SPAWN_Y_LINE` default ON (opt-out at the flip)? Faithful spawn pose for every car; (b), D2 and the 13 tracks hold;
the G-CTRL trio moves to `b7534619 / fb690fb1 / d3a20f4c`.

### 5. Flipped (user decision 2026-10-18)

Default ON; `MASHED_NO_SPAWN_Y_LINE=1` opts out. Exe `0e773d28` (`MASHED_NO_DEPLOY=1`), `flip/RUNLOG.txt`: G-CTRL default
`b7534619 / fb690fb1 / d3a20f4c` x2 (= `H_SY`); default B dumps K0 / K1 / K26 `c7b40cf2 / 5b9905d8 / f680a529` (= `B_SY_*`);
opt-out `b1f49e7a / b1be0054 / d3a20f4c` and `e332af0c / 01ad5975 / 772f9fb7` (the previous default). The default now equals
the `SY` arm, so section 1's (b), D2 (`D2_SY_1..3`, 3/3) and 13-track (869 rows, 0 RED) numbers stand for it.
