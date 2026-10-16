# Collision-FX re-measure on the default build (ROADMAP "Open decisions" follow-up #2), 2026-10-16

Recipe (verify/fe_residuals_20261016/run_fx.py): the G-CTRL race (Training, car 1, round rule 4, deterministic, 14400 frames) +
MASHED_FX_DEBUG=1, private gameroot copy. `fx_stats.py` reads the per-slot debug lines (1200 samples/slot, one per 12 calls).
Build base 017fc698 + this branch. G-CTRL after the change: 991a5fff / be0d47ea / bc75b405 (unchanged).

## Baseline compared against
- ORIGINAL-SIDE: only the spark path has an original counterpart. FUN_004864f0 (burst) / FUN_00486370 (per particle) / FUN_00486610
  (4-particle wrapper), entry-only counters via orig_fx_count.py on a scenario_launch race (Training, 4 cars, player accel pulsed, 85 s
  attached): 24 bursts, 115 particles in the ~45 s before phase 3 ended (verify/.../fx/orig_fx_count_t0.json). The original has no
  skid particle path counted here (skid-smoke has no original counterpart: EmitCarFx is a [SCAFFOLD]).
- 2026-08-15 STANDALONE baseline for skids / pool: per-slot skids 772/358/241/378, pool peak 175 / median 130 of 1,200, R=55.4,
  max speed 16-21 (verify/fx_verify/fx_debug_perslot_occupancy.txt, ROADMAP "Over-firing" paragraph). Different race length and track.

## Numbers (240 s race window, R=27.5)
| arm | skids/slot 0..3 | sparks/slot 0..3 | max spd/slot | pool peak / median |
|---|---|---|---|---|
| default, before (MASHED_FX_RAWSPEED=1) | 1027 / 2519 / 12668 / 16959 | 392 / 302 / 1498 / 1728 | 3243 / 4478 / 4477 / 5712 | 1200 / 188 |
| default, world-unit speed (new default) | 62 / 815 / 5139 / 5489 | 2 / 4 / 15 / 32 | 8.6 / 12.2 / 12.2 / 12.2 | 598 / 110 |
| 2026-08-15 baseline | 772 / 358 / 241 / 378 | (3/1/1/0 in 51 samples) | 16-21 (R 55.4) | 175 / 130 |

## Finding
Thresholds did NOT carry over. Under the ported physics `EmitCarFx` receives the chain's internal speed/vel (player to 3243,
AI cur_speed ~3200) while R and pos are world units: |dpos|/dt / sp = 0.00257 median over 3449 moving player frames (player_trace.log of
F_NEW). The R-scaled gates fired on every moving frame (slot 3: 790 of 817 moving samples had skidI>1) and the pool peaked at 1200 of 1200.
Fix (TrackRenderer.cpp EmitCarFx): derive speed/vel from the world position step (teleports > R/2 skipped). Sparks fall to the original's
order (2..32 bursts per car vs the original's 24 total / 45 s), pool peak 598. Residual, NOT fixed: AI slots 2/3 still emit 5139/5489 skids
(skidI>1 on 312/578 and 326/745 moving samples) against 241/378 in 2026-08-15, a 14-22x excess on a longer race; the skid gates are
scaffold calibrations, not original-derived. Thresholds untouched (decision below).
