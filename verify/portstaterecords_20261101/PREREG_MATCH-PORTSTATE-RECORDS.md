# PREREG MATCH-PORTSTATE-RECORDS (2026-11-01)

L-78 (c), port-state migration 2/3 (Mariano 2026-11-01): `mashed_re.exe` keeps the original car record block
alive behind a default-OFF knob. Committed alone, before any arm runs. Base: `orch/MATCH-PORTSTATE-COURSE`
@ 622335487 (migration 1/3, knob `MASHED_PORTSTATE_COURSE`).

## What the original holds and what the exe holds today

| range | original | exe today |
|---|---|---|
| 0x008815a0 .. +16*0xd04 | vehicle record array, stride 0xd04 (`imul eax,eax,0xd04` at 0x0046d78e). 0x00881e80 (base of the a2 source of 0x0046d510) = +0x8e0 of record 0, 0x008815a8 (read by 0x0046dbe0) = +0x8 | the same 0xd04 layout in the exe-only static `g_records[16*0xd04]` (`Vehicle/VehiclePhysicsRun.cpp:128`), zeroed and bound to `g_vehicleArrayBase` in `VehiclePhysics_Init` (:310, :320). The image pad at 0x008815a0 (`exe_main.cpp` B17) stays 0 |
| 0x0063d9e0 .. +4*0x2ac | per-car visual record (RpAtomic* objs[0x80] at +0, clump at +0x26c, flags +0x294; built by 0x00420xxx, readers 0x0041ec0f..0x0041f30a, 0x00420061..0x0042089b) | exe-only `s_rec[4][0x2ac]` (`Race/CarRegistrant.cpp:37`) carrying only +0x258..+0x263 (= `VehRecord(v)+0x958`, 12 B), +0x264 (radius bits 0x3f088c2d, U-9218) and +0x290 (= v), handed to the matched 0x0041f710 |
| 0x008995bc + v*0x138 | int counter (+0xfc of the 4 x 0x138 block at 0x008994c0), added to by 0x00422b50 (callers 0x0046d7b5 / 0x0046d7df in 0x0046d780), read at 0x00424900 / 0x00424e93 | no exe producer: both 0x00422b50 calls in `Vehicle/LaunchRevCharge.cpp:219-229` are `#ifndef MASHED_STANDALONE`, and the exe body `VehicleDamageAccum` calls the raw RVAs 0x0040e470 / 0x00472650 / 0x004a2c48 |

Exe code that already addresses the 0x008815a0 range absolutely (capstone scan of the COURSE exe `a7f4913f`,
`absref_scan.py`, 50 sites). Sites whose function has an exe caller on the gate path:

- `TrackRenderer::U9186GateDump` (armed by the H arms, `MASHED_U9186_GATES`): prints `I32(0x008815a4+vb)` and
  `I32(0x00881f90+vb)` next to the port record's `+0x004` / `+0x9f0` (`TrackRenderer.cpp:6718-6723`).
- `AiPreTickRubberBand` (`Ai/AiStandalone.cpp:1642` `GearConstSet`): writes record `+0x154` of cars 0..3.
  The exe has no reader of record `+0x154` other than `VehicleInit`'s initial write (grep `kGearTorque1` / `0x154`
  over `Vehicle/`).
- `AiLeaf_46d570` / the naked `AL_46d570`: rebased by `g_al_recdelta` (`Ai/AiLeafTargets.cpp:287`) =
  record base - 0x008815a0, so it reads the port record in both knob states.
- `ElimTraceFrame` (`MASHED_ELIM_TRACE`, diagnostic, not armed by any gate arm).

The other sites (`VehicleCouplingBridge_impl`, `EntityFieldSet`, `VehicleSlotFieldSet`, `VehicleSlotInit`,
`UtilTableRead46b4f0`, `VehicleDestructionStateGetter`, `VehicleEntitySlotRead`, `VehicleVec3At9C8Get`,
`VehicleVelocityWorldGet`, `VehicleDampVec3`, `VehicleCarStateSet`, `VehicleFloatFieldAsInt`, `PK_46d4a0`) have no
exe caller by symbol, or are hook bodies that are inert in the exe. The three `PickupTick` sites `lea [edx+0x888cb0]`
are an index-scaled base, not a record field [UNCERTAIN: which array the effective address lands in was not
traced; P4 measures the outcome].

## The change (one knob)

`MASHED_PORTSTATE_RECORDS` (default OFF; exe only; read once at static init).

1. **Vehicle records.** `g_records` becomes a pointer: knob OFF it points at the exe's own static store (as now),
   knob ON at 0x008815a0. `VehiclePhysics_Init` zeroes 16*0xd04 bytes there and binds `g_vehicleArrayBase` to it,
   so every port read and write of a record field is the original address. No copy, no mirror: the port record IS
   the original block.
2. **Car visual records.** `s_rec` likewise: knob ON the four 0x2ac records are 0x0063d9e0 + v*0x2ac. The exe
   writes the same three fields as now. Every other field stays 0 (the exe builds no RW car object).
3. **Witness** (knob ON only): one `[portstate-records]` line in `mashed_re.log` every 600 race frames, cumulative
   counts over every race frame for cars 0..3: record base; matched-read equalities for the T13 rows
   (0x0046dbe0 read `I32(0x008815a8+v*0xd04)` vs `VehRecord(v)+8`; 0x0041f030 read of 16 B at
   `0x0063d9e0+v*0x2ac+0x258` vs `VehRecord(v)+0x958` + radius bits; 0x0046d510's result `at` row of the +0x928
   matrix vs record +0x9d4, bitwise); and the values of `0x008995bc+v*0x138` and `0x0088fc88+v*0xb4`.

Knob OFF the exe runs the same computations on the same store. Code layout changes: `rec()` and the two inits load
a pointer instead of an array address.

Not in this step: the 0x008995bc counter (no exe state exists to keep there; its producer is the 0x00422b50 call
in 0x0046d780 with its random-subtract arm for non-player cars, a behaviour change with an RNG draw, and the exe body
calls unbound RVAs). The held word 0x0088fc88 and 0x0063c8d0 (T13 0x00414490 / 0x0041b520) are other state.

## Predictions

- **P1 (default unchanged).** Knob OFF: G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3
  (`re/orchestrator/merge_train.json` `gctrl.ref` / `b.ref`; arms `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`).
- **P2 (storage).** Knob ON: the witness reports base 0x008815a0 for the records and 0x0063d9e0 for `s_rec`.
- **P3 (live reads).** Knob ON, H arm: in `H_PSR.gates.csv` the absolute columns (`0x008815a4+vb`, `0x00881f90+vb`)
  equal the port record columns (`+0x004`, `+0x9f0`) on every row. Knob OFF (H_DEF) the absolute columns read 0.
  Witness: the 0x0046dbe0 and 0x0041f030 equalities hold on every sampled car-frame (mismatch count 0).
- **P4 (behaviour with the knob).** Knob ON: `H_PSR.step.csv` = 8284be46, `H_PSR.ptrace.log` = 31fe043e,
  `B_PSR_K0/K1/K26` = 1d5bf4f9 x3. `H_PSR.gates.csv` differs from ba407a16 only in the two absolute columns: with
  those two columns replaced by the port columns in both files, H_PSR equals H_DEF row for row
  (`gates_mask.py`). Reason: no exe reader on the gate path consumes the relocated fields differently (list above),
  and the rubber band's `+0x154` write has no port reader.

**If P4 fails** (a step / ptrace / B hash moves, or the masked gates differ): no flip, the knob stays default OFF,
and the move is filed as its own decided-flip decision with the moved hashes and the first divergent row.
**If P2 or P3 fails**, the relocation is wrong: fix before the re-check.

## Re-check of the T13 rows (after the arms)

For each T13 row left on the record block (`RESULT_MATCH-TOPDOWN-T13.md`): 0x0046dbe0, 0x0046d510, 0x0041f030,
0x0041f1e0, 0x00422b50, and 0x00414490 (via 0x0046d510), record with the knob ON whether the matched read is now the
port's value, which blocker remains, and whether a swap is sound (L-72: every global the matched source reads is live
in the exe, and an exe caller exists). No re-classify is prepared unless a row becomes a sound swap with an exe
caller in the default build.
