# U-9185 re-baselined on the shipped default — the car<->car contact already cut the over-speed by ~a third

**MEASURED 2026-10-06.** No game run (uses the leg-3 captures committed at `523cc5d9`), no
build, no C-level moved, `original/` untouched. The point of this note: **U-9185's recorded
over-speed numbers are now STALE**, because the car<->car contact shipped default-ON on
2026-10-06 (`19e6b2e1`) changed the AI window speeds, and any further attack on the over-speed
must start from the new baseline, not U-9185's `3421.7 / 3346.2 / 3495.9`.

## The number that moved

Plain median of `rec_9e4` over the 220-call window (the `+15..31 %` / `+41..34 %` metric of
`verify/d3_noboost_20261003/RESULT.md`), against the original `o_t1`/`o1` (both
`2419.5 / 2397.0 / 2617.6`, deterministic):

| build | car 1 | car 2 | car 3 | over-speed vs original |
|---|---:|---:|---:|---|
| original (`o_t1`, `o1`) | 2419.5 | 2397.0 | 2617.6 | — |
| **OLD default** (car<->car OFF, = U-9185's record) | 3421.7 | 3346.2 | 3495.9 | **+41.4 / +39.6 / +33.6 %** |
| **NEW default** (car<->car ON, shipped `19e6b2e1`) | 3057.7 | 3056.5 | 3254.3 | **+26.4 / +27.5 / +24.3 %** |

The car<->car contact (collisions jostling and slowing the field) **removed ~13–15 percentage
points of over-speed per car** — roughly a third of it — as a side effect of the steering
improvement U-9196 adjudicated. It was never measured against the speed metric because leg 3's
gates were (e) and the band counts; this note closes that gap. The over-speed is **not closed**
(+24–28 % remains), but its magnitude is a quarter smaller than U-9185 records.

## What this does NOT change — the carrier is still COMMAND, per the standing record

The re-baseline is a magnitude update, not a re-attribution. The established finding stands and
should be read before any "physics" attack on the over-speed:

- `verify/d3_elim_20261003/RESULT_STEP2.md` (2026-10-03): **"the surviving over-speed is a
  COMMAND defect, not a force term"**; U-9185 item (c) is ANSWERED and moved to **U-9186**. The
  port's force chain reproduces the original's speed to **0.23–0.98 %** before the AI's
  commanded throttle first diverges, so drive force (`+0xb14/+0xb1c`, A4 `0x00470670`), A5 drag
  (`0x0046ddb0`), A6a's clamps incl. grip-clamp #6 (`0x00467650`), the contact solver
  (`0x0046f6c0`) and the gear/rev channel (`+0xb0c`) are **jointly exonerated** as the carrier.
- The carrier is three unported branches of `FUN_00416250` (mode 3, mode 7, two early returns):
  the AI holds full throttle where the original lifts or brakes.

**So "the only substantive physics path left on (b)" is not an accurate description of the
over-speed** — the over-speed is a command defect (U-9186), and the car<->car contact just
removed a third of it by changing the physics the AI drives through, not by changing a force
term. The genuinely-physics residual on (b) is a different, narrower thing: U-9191 item (b),
car 1's body-heading divergence at matched position (~0.89–0.98 deg).

## Re-verified on the shipped build — the command attribution survives the ship

The 2026-10-03 onset analysis (physics exonerated to <1 % pre-onset) was run on the
car<->car-**OFF** build, and the shipped contact changes the physics (velocity + angular
impulses on contact), so it had to be re-run. `ai_speed_onset.py --orig o_t1 --port L3on1`
(current default, car<->car ON), no new capture:

- **Onset is COMMAND on all three cars at all three thresholds** — car 2 `k=24`, car 3 `k=41`
  (car 1 `k≈102`), `G2B-SENS: PASS (COMMAND)` for each.
- **Pre-onset physics agreement still holds:** median `|dspeed|` **0.715 / 0.473** with max
  fraction **0.00228** over `[0, onset)` — **under 1 %**, so the force chain is still exonerated
  with the contact ON.
- The divergence is the same command one: at car 2's onset `k=24` the original brakes
  (`c5 = 255`, `mode 3`, speed `2374 → 1050`) while the port holds full throttle (`c5 = 0`,
  `mode 0`, speed `2375 → 2400`). The port pins `ai_mode = 0` and never enters the original's
  lift/brake modes.

**Conclusion: the surviving +24–28 % over-speed is a COMMAND defect, reconfirmed on the shipped
default. It is not a physics carrier — the physics is exonerated to under 1 % before the
command diverges.** Reducing it further means fixing the command (U-9186: the unported
`FUN_00416250` mode-3 / mode-7 / early-return branches), not a force term. The car<->car
contact already took a third of it out by changing the physics the AI drives through, which is
the largest single reduction any lever has produced on this metric.
