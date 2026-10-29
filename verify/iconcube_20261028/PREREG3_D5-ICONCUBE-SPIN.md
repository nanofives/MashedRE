# PREREG3 D5-ICONCUBE-SPIN (run 20, 2026-10-28): the pre-GO refdist producer under MASHED_PICKUPS_PREGO

Committed alone, before the code change and before any arm. Standalone only: the original is not booted this run
(audio endpoint absent). Original behaviour comes from static reading and captures already on disk.

## 1. Static finding (cited, read before this PREREG)

Run 19 (G-PG-SIM FAIL): with `MASHED_PICKUPS_PREGO`, AI cars 2 and 3 enter mode 7 on the first racing tick, look =
pool entry 4 (the mine, (1.88176906, -3.09718895)). The original's captures show mode 0 there.

- **The original registers the pickups before GO too.** `verify/d3_mode7_m4_20261010/o_c30a.msd.c30.tsv` and
  `o_c30b.msd.c30.tsv` (original FUN_00414c30 probe, Training, 4 cars, game mode 10) list the registry slots per call.
  On the first `ba8c == 6` tick (rows n 2209..2211 and 2416..2418) the registry holds 5 slots of type 7, slot 5 / 6 at
  (1.8818, 0.6694, -3.0972) r 0.5, the mine. Decoder: `verify/iconcube_20261028/c30_slots.py`. So narrowing the
  registration would not match the original.
- **The gate is FUN_00442cc0 (refdist) in FUN_00414c30's type-7 path.** From the verbatim body
  (`mashedmod/src/mashed_re/Ai/AiModeSeven_naked.inc`): `0x00414e19 cmp [ebx+4],7`, `0x00414e2f call FUN_00442cc0(v)`,
  `0x00414e34 fcomp [0x005d757c]` (0.0), `0x00414e42 jp 0x00414d5a` (next slot when refdist != 0.0). FUN_00414490
  (`0x00414e69`, the only call that can give return 2 = mode 7) is reached only when `0x008989b0[v] == 0.0`.
- **The original's refdist is nonzero for every AI car on every pre-GO tick and on the first racing tick.** Same
  captures, column `refdist`: on the first `ba8c == 6` tick v1 / v2 / v3 = `0x3fc6d696` / `0x3feaa0bd` / `0x402ff88e`
  (1.5534 / 1.8330 / 2.7495), the same words as the `ba8c == 3` and `ba8c == 4` ticks before it. `sub` is empty on
  those rows (FUN_00414490 not called), `ret` 0.
- **The producer runs pre-GO in the original.** FUN_0040fc00 (`verify/rfx2_20261016/evidence/decomp/headlight_disasm_40fc00.txt`):
  the only branch before the registry chain is `0x0040fc69 je 0x0040fc74`, which skips FUN_00425a40 (`0x0040fc6c`)
  alone. `0x0040fc75 call FUN_0040da50` and `0x0040fc7c call FUN_0040d470` follow with no branch. FUN_0040d470 ->
  `0x0040d4f3` FUN_00448220 -> `0x004483c2` FUN_00446520 -> `0x004469b6` FUN_00442a60, the producer of `0x008989b0`
  (`verify/d3_u9683_20261016/xrefs_order.txt`).
- **The port never runs that producer before GO.** `RaceComputeDistancesTick` is called only in UpdateCar after the AI
  block (U-9683 default) or in UpdateRace. The stand-in countdown returns before both. So the port's first racing AI
  tick reads `0x008989b0[v] == 0.0` for every car (it was never written), and with the pre-GO registry the type-7 gate
  opens.

## 2. Change (default OFF, only under MASHED_PICKUPS_PREGO)

`PreGoRegistryTick` (`D3d9Render/TrackRenderer.cpp`) runs FUN_0040fc00's chain in the original's order:
`CarsReg_FrameTick` (0x0040fc75), **`RaceComputeDistancesTick(2)` (0x0040fc7c, FUN_0040d470 -> FUN_00442a60)**,
`Pickups_FrameTick` (0x0040fcd0), `Registry_FrameTick` (0x0040fd07). `MASHED_PICKUPS_PREGO_NO_REFDIST` (diagnostic,
default OFF) drops the new call, which restores run 19's PREGO. The site value 2 only labels `MASHED_REFDIST_TRACE` rows.
Knob OFF: no new call, no code path changes.

## 3. Predicates

| id | arms | prediction |
|---|---|---|
| G-OFF | `H_DEF`, `B_DEF_K0/K1/K26` | equal to `merge_train.json` refs (G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/9fcdbd3a/53778664`) |
| G-NR | `H_PGNR`, `B_PGNR_K*` (PREGO + NO_REFDIST) | equal to run 19's PREGO hashes (`67763abd/5250011e/6457cb73`, `b9f43d6b/74c23a4b/079a708f`) |
| P-M7 | `H_PG` step dump, first racing tick | cars 2 and 3 `ai_mode` 0, look equal to `H_DEF`'s first tick (car 2 (-1.41216886, -10.0070553)) |
| P-RD | `H_PGRD` (PREGO + `MASHED_REFDIST_TRACE`), first `T` row | refdist nonzero for cars 1..3. Report-only: the words vs the original's `0x3fc6d696 / 0x3feaa0bd / 0x402ff88e` |
| G-PG-SIM | `H_PG`, `B_PG_K*` | equal to the refs above (hypothesis: the mode-7 opening is the only first-tick effect of the pre-GO publish) |

If G-PG-SIM holds: 13-track table (`run13.py`, ref `verify/hbtexanimrefs_20261027/tracks_score.txt` as used by
`verify/hbfullcheck_20261028/tracks_score.txt`) and D2 x3 in band (0.1942 / 0.2492 / 1896.74), both with
`MASHED_PICKUPS_PREGO=1`. Even with every gate held the knob stays default OFF and the flip is a decision row.

If G-PG-SIM fails: report the first differing step row and its fields, and whether refdist's other first-tick readers
(FUN_00415220 / FUN_00415200 fire gates, AiLeaderTimer) account for it. No further narrowing is pre-registered.
