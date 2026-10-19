# PREREG u9769: U-9769, the player's state at calls 22..39 (2026-10-19)

Branch `work/u9769` from `race/first-frame-parity` `e9d9665cb`. Model claude-opus-5-5, account2. Exe `MASHED_NO_DEPLOY=1`,
private root `D:\MashedWT\d3bc_root`. Written before any knob arm. Stack `EXL7AFSM7` (d3bc's full stack + the M4 mode-7 set).

## 0. Measured before this PREREG

1. G-NOOP on the new exe (`72230970`): `6efb49ed / 3e0724ac / 395fb8b7`.
2. New original capture `arms/o_c0` (regime-A argv, `--statediff-car 0`; it rolled regime B, identical to A / C through row
   60). The player's record (slot 0) on `EXL7AFSM7` K1 equals it through window call 21 (`recslot0.py`, car 1's window as the
   frame reference, the fields that also differ on the exact car 1 excluded) and splits at call 22: velocity `+0x9b0..+0x9b8`
   o `(-426.4006, -31.65092, -1161.688)` p `(-418.3935, -31.68187, -1150.349)`, from rest. Car 2's record also equals the
   original through call 21 (d3bc `o_c2`) and splits at 22.
3. New `scenario_launch.py --carcar-probe N` (entry + leave of VehicleCarCarContact `0x00469df0`, both records). o_cc1: the
   first contact is window call 22, self = slot 0 (ECX), other = slot 2 (EAX), pass 0, return 1; the player's in-call
   velocity change equals its whole-frame change, car 2's in-call change is `(362.32, -57.409, 1305.526)`.

## 1. Plan

Diagnostic `MASHED_CC_REPLAY` (default OFF, no game state): the port's `VehicleCarCarContact(other, self, pass)` on the
original's entry records, every dword diffed against the original's leave records. Arm `CR_DEF`. A fix found from it goes
behind a new default-OFF knob, written into an amendment here before its arms. Gates as d3bc (G-NOOP, the replay itself as
G-CC: 0 differing dwords on the contact rows, (b) report, default only on Mariano's decision with D2 / 13 tracks / G-CTRL).

## 2. Amendment 1 (written before any MASHED_CC_MATRIX_INVERT arm)

Measured after section 1: `CR_DEF` (exe `6a85044d`; G-NOOP `B_DEF_K0` `6efb49ed`) replays 64 o_cc1 rows; 63 are exact. The
contact row (window call 22, self 0 / other 2, pass 0) reproduces the original's linear impulse to about 1e-6 relative
(player `+0x9b0` -426.400635 vs -426.399963), but the other car's `+0x148` reads 0.176054001 against -0.0239460096: a
difference of 0.2 = 2 x 200 x 0.0005, the spin impulse `local_164 * _DAT_005ce268` with the opposite sign. The spin sign
comes from the arm transformed by `FUN_004c3df0` (vectors slot `+0x14`) with the matrix from `FUN_004c4dc0` (decomp of
`0x00469df0`, the tail after the bounce timers). The port's `Rw_MatrixDerive` (`Collision/ContactStubs.cpp:161`) is an empty
stub, so that 64-byte local is uninitialised. FUN_004c4dc0 is `RwMatrixInvertEntry` (C3, `Render/RwMatrixInvert.cpp`, exe arm
live since pontoon).

H1: with `Rw_MatrixDerive` calling `RwMatrixInvertEntry`, the replay's contact row has no spin difference. Knob
`MASHED_CC_MATRIX_INVERT=1` (default OFF). Arms: `CR_MI` (G-CC: contact row's spin dwords equal; report the residual linear
dwords), G-NOOP, then the in-run question (section 0 item 2: the player's in-run entry state at call 22) on `EXL7AFSM7` and
`EXL7AFSM7` + the knob, and (b) report.

## 3. Amendment 2 (written before the arm; existing knob only)

Measured after amendment 1 (exe `24e6d3f6`, G-NOOP `6efb49ed / 3e0724ac / 395fb8b7`; an intermediate build that wrapped
VehicleCarCarContact in a new frame gave `ddddf961` with the knob unset, which is the uninitialised-local dependence of
section 2 showing in the default; the trace now runs in noinline helpers and keeps the frame):
- `CR_MI`: the contact row's spin difference is gone; 6 dwords remain, the linear velocities to about 1e-5 relative.
- In-run (`MASHED_CC_TRACE`, `EXL7AFSM7` with and without the knob): the player's entry record equals the original's; car 2's
  entry velocity is `(-300.85376, , -2330.15259)` against the original's `(-316.573639, , -2353.27734)`. The port's value is
  the original's `+0xae0` (the previous velocity copy): car 2 enters the contact before its own A4 / A6a of the frame.
- FUN_00470c70 runs FUN_00470670 (A4, A6a inside) for all 16 records in one loop (call `0x00471076`, loop end
  `0x0047108c`), then the refresh loop, then FUN_004709a0 (substeps and the pair loop, call `0x00470bcd`).

Report arm: `EXL7AFSM7MIA` = `EXL7AFSM7` + `MASHED_CC_MATRIX_INVERT` + `MASHED_A4_ORDER` (U-9746's pre-phase), traced, with the
slot-0 record, K1. Question: does car 2 enter the call-22 contact with the original's velocity, and does the player's
record then hold past call 22. No gate, no default.
