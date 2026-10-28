# PREREG2 D5-ICONCUBE-SPIN (run 19: align the first pickup tick)

Follows the run-18 RESULT (`verify/iconcube_20261028/RESULT_D5-ICONCUBE-SPIN.md`, `07b408cc6`), which was RED:
the cube draw is exact on its inputs, but the standalone's pickup spin lags the original's by 178 ticks on
mortar / oil and its mine is collected on a different schedule. Branch `orch/D5-ICONCUBE-SPIN` after merging
`orch/integration` @ `0d1285dc7` (merge `27e7d6c62`). No new original-side capture (the original does not boot:
no audio endpoint).

## 1. Facts this run starts from

- **The original ticks the pickups before GO.** `FUN_0040fc00` @ `0x0040fc00` calls `FUN_0040da50`
  (`0x0040fc75`, the cars' registry entries at `0x0040dad0`), `FUN_0045bba0` (`0x0040fcd0`) and `FUN_00485070`
  (`0x0040fd07`) with no guard (decomp `decomp_pc.py` 2026-10-28). `FUN_0045bba0` calls `FUN_00459000` with no
  guard (after `FUN_0045a190`, return address `0x0045be76`). The port runs `FUN_0040fc00` on the pre-race
  sub-states 3 / 4 / 5 (`SubStateCameraChain`, `CarParts_FrameTick` 0x0040fc75, `CopterTick` 0x0040fcb9 in the
  MASHED_SUBSTATE block of `TrackRenderer::UpdateCar`).
- **The standalone does not.** The stand-in countdown branch returns at
  `if (countdown_ > 0.f) { CopterTick(...); return; }` before `UpdateRace`, and the only default call of
  `CarsReg_FrameTick` / `Pickups_FrameTick` / `Registry_FrameTick` is in `UpdateRace` (or at the end of
  `UpdateCar` under MASHED_REG_LATE, after the same return). The MASHED_SUBSTATE block returns before them too.
- **The d1r1 Training trace is 854 pre-GO rows long.** `log/d1r1/orig_training/trace.csv` (1200 rows), column
  `ba8c`: rows 0..740 = 3, row 741 = 4, rows 742..853 = 5, rows 854..1199 = 6. Row 884 is 30 frames after GO.
- **The standalone's replay GO lands near row 179.** The stand-in countdown is `countdown_ = 3.0f`. Every
  countdown frame consumes one trace row (`RaceCamUpdate` in the countdown branch), the GO frame consumes the next
  one in `UpdateRace`. So the standalone's sim at row 884 is about 705 frames after its GO, the original's 30.
- **`MASHED_CAR_REPLAY` does not feed the registry.** It writes only the cars' render matrices (`rep_car` in
  `TrackRenderer::Render`, a `D3DMATRIX` per slot). `CarsReg_FrameTick` (`Race/CarRegistrant.cpp`) copies each
  car's own record `+0x958` (`Vehicle::VehRecord`) into its registry entry `+0x258`, so `FUN_00458920`'s pair
  test reads the standalone's simulated cars. The run-18 [UNCERTAIN] is answered: no.

## 2. Change

- **`MASHED_PICKUPS_PREGO`** (behaviour, default OFF): on every stand-in countdown frame, and on every
  MASHED_SUBSTATE pre-race frame that runs `CopterTick`, run `Race::CarsReg_FrameTick()`,
  `Gameplay::Pickups_FrameTick()` and `Race::Registry_FrameTick()` in FUN_0040fc00's order (0x0040fc75,
  0x0040fcd0, 0x0040fd07). Each function keeps its own knob gate (`MASHED_REGISTRY`, `MASHED_PICKUPS_ORIG`).
  Knob OFF: no change.
- **`MASHED_CAM_REPLAY_GO`** (diagnostic, default OFF, inert without a loaded `MASHED_CAM_REPLAY`): at the
  countdown seed, `countdown_ = (G + 0.5) / 60` with G = the first trace row whose `ba8c` is 6, so the countdown
  covers frames 0..G-1 and the GO frame consumes row G, as in the original. It moves the standalone's GO in the
  replay only. Every countdown_ consumer (HUD start-light lead, the 113-frame state-5 window of the AI race
  timer, the launch rev charge) sees the longer countdown. Not a behaviour change, never in a scored default arm.

## 3. Arms (Training replay, `run_replay3d.py`, `--shots 884,924,959 --frames 1450`, `MASHED_ICONCUBE=1`)

| arm | knobs |
|---|---|
| R1 | ICONCUBE (the run-18 ON arm, rebuilt on the merged source) |
| R2 | ICONCUBE + PICKUPS_PREGO |
| R3 | ICONCUBE + PICKUPS_PREGO + CAM_REPLAY_GO |
| R4 | ICONCUBE + CAM_REPLAY_GO (control: GO aligned, no pre-GO tick) |

Each arm also runs with `MASHED_PKCB_TRACE` so the first pickup tick and every collection are logged.

## 4. Predictions

- **P-R1**: the run-18 numbers repeat (row 884 theta mine 44, mortar / oil 345, extra mine on 924 / 959).
- **P-R2-SPIN**: mortar and oil theta at row 884 = the original's 163.000 with |c| <= 1 tick (GREEN at 0). The
  mine keeps a different angle (collected on the standalone's own schedule).
- **P-R3-SPIN** (P-SPIN of the first PREREG): every cube on every row has theta_stand - theta_orig = the same c,
  GREEN at |c| < 0.01 deg. [UNCERTAIN] One tick of offset (c = +/-1 deg) is possible from where the row is sampled
  inside the frame. It is reported as its value, not as a pass.
- **P-R3-SET** (G-DRAW-ON of the first PREREG): the cube set and order equal the original's on rows 884 / 924 /
  959. [UNCERTAIN] This needs the standalone's AI to collect the mine between rows 884 and 924 as the original
  does. The cars are the standalone's own sim (section 1), so this is a prediction about the early-race sim, not
  the draw. A miss is reported with the collection tick and car from the trace.
- **P-R4**: GO aligned without the pre-GO tick. The standalone's first tick is the GO frame (row 854), so it
  loses about 852 of the original's 883 ticks: mortar / oil c = -852 mod 360 = -132 deg, +/- 1. Attribution
  control: the pre-GO tick is what closes the spin, the GO alignment is what moves the collection.

## 5. Gates

- **G-OFF** both knobs unset: G-CTRL and B hashes equal `merge_train.json` refs (G-CTRL
  `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`), via
  `verify/iconcube_20261028/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- **G-PG-SIM** `MASHED_PICKUPS_PREGO` set (new stack `PG`): the same six hashes predicted. Before GO no car
  moves and the AI does not tick, so no entry is collected and the registry publish has no reader. A change is
  reported with its first differing row and is a finding about the knob, not a gate failure of the default.
- `MASHED_CAM_REPLAY_GO` is inert without `MASHED_CAM_REPLAY`, which no gate arm sets.

## 6. Not claimed

No C-level change (no Frida diff possible). No default flip: turning `MASHED_PICKUPS_PREGO` or `MASHED_ICONCUBE`
ON is a decision row.
