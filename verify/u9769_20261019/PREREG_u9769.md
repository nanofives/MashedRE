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
