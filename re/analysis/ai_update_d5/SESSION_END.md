# Session End: ai_update_d5-20260512

## Session summary

Option A pivot: rather than extending the d4 callee tree (which was empty — 0 unmapped callees), traced callers of FUN_00418560 upward through the game-loop architecture. Discovered and analyzed 11 unmapped functions from `FUN_004103a0` (race tick, game state 5) and `FUN_00425a40` (physics/AI per-frame step).

Pool slot: Mashed_pool0. SHA-256 anchor verified.

## Call chain discovered

```
FUN_0042c960 / FUN_00492d30          [callers of game state machine — not analyzed this session]
  └─ FUN_004111c0   (C1, timer_d2)   game state machine: switch(DAT_0063ba8c 0-0xb)
       ├─ FUN_004103a0 (C1, leaderboard_d2) — race tick, state 5
       │    [11 new unmapped callees analyzed here]
       └─ FUN_0040fc00 (C1, Race::Tick) — per-frame physics tick
            └─ FUN_00425a40 (C1, leaderboard_d3) — physics/AI step
                 └─ FUN_00418860 (C1, ai_path_following) — AI tick dispatcher
                      └─ FUN_00418560 (C1, ai_update) — per-vehicle AI update
```

## Functions analyzed

| RVA | Name | Notes |
|-----|------|-------|
| 0x00429860 | FUN_00429860 | Race-state flag getter (DAT_008991bc); 5 bytes; U-3587 |
| 0x00429840 | FUN_00429840 | Race-state latch setter (write-once-or-clear); 23 bytes |
| 0x0041da90 | FUN_0041da90 | Delta-time output getter (DAT_0063d588 → *param_1); 16 bytes; U-3585 |
| 0x00426c90 | FUN_00426c90 | Track lap-line conditional (FUN_0041ea80 gate → FUN_0041e960); 22 bytes; U-3588 |
| 0x0046c750 | FUN_0046c750 | Entity velocity counter getter (DAT_00882194 stride 0xd04); 24 bytes; U-3586 |
| 0x0046c730 | FUN_0046c730 | Entity damage-state getter (DAT_00882198 stride 0xd04); 24 bytes; U-3586 |
| 0x0046d7f0 | FUN_0046d7f0 | Velocity counter updater (speed-gated accumulate + drain; clamp 0-3000); 143 bytes; U-3584 U-3586 |
| 0x0046d780 | FUN_0046d780 | Damage-threshold processor (counter → state 0/1/2; FUN_00422b50 apply); 108 bytes; U-3586 |
| 0x00423b00 | FUN_00423b00 | Replay pre-step (DAT_007f1a50 gate; calls 4 replay subs); 29 bytes; U-3589 |
| 0x00422ba0 | FUN_00422ba0 | Collision event queue processor (256-entry ring buffer; types 3/4/6/8/9/10); 1070 bytes; U-3590 U-3591 |
| 0x004252c0 | FUN_004252c0 | Vehicle proximity/interaction detector (event queue writer; scores alongside/head-on); 1905 bytes; U-3592 U-3593 |

## Tracker changes

- hooks.csv: +11 rows (ai, C1, new)
- DEFERRED.md: added D-10560..D-10571 (12 rows)
- UNCERTAINTIES.md: added U-3584..U-3593 (10 rows)
- SCRIBE_QUEUE.md: +1 row

## Key structural findings

- **Entity velocity/damage struct** at base `0x882194`, stride `0xd04`:
  - Field +0 (0x882194): velocity/exposure counter (int, 0–3000)
  - Field +4 (0x882198): damage state (int, 0=none/1=light/2=heavy)
  - Note: `0x882194 = 0x881f90 + 0x204` — this is field +0x204 inside the stride-0xd04 entity block whose alive/spinout flags live at 0x881f90.
- **Race-state latch** `DAT_008991bc`: set via `FUN_00429840` (write-once unless clearing to 0); read as gate by race tick. Value 0xb = lap-complete.
- **Collision event ring buffer** at `DAT_007e9de4/ea1e4/ea5e4` (256 entries, write-ptr `DAT_007e9de0`): `FUN_004252c0` writes proximity/heading events; `FUN_00422ba0` drains and applies them same frame. Types 0x3ff01–0x3ff04 are "interaction quality" codes; types 3–10 are physics/damage codes.
- **Player-slot map** `DAT_007f1a14` (4 × int, stride 4): confirmed as index translation from 0-based player index to internal vehicle slot. Used by `FUN_0046d7f0` for speed-byte lookup.
- `DAT_0063d588` = time accumulator read by `FUN_0041da90`, compared to `_DAT_005ccdf4` for lap-complete threshold in FUN_004103a0. Distinct from `DAT_005f29b8` (elapsed-time decremented counter).

## Depth-5 deferred (D-10560..D-10571)

| D-ID | RVA | Caller | Description |
|------|-----|--------|-------------|
| D-10560 | 0x0041ea80 | FUN_00426c90 | Lap-line gate check |
| D-10561 | 0x0041e960 | FUN_00426c90 | Lap-line update action |
| D-10562 | 0x00422b50 | FUN_0046d780, FUN_00422ba0 | Delta applier for entity damage/velocity |
| D-10563 | 0x00423040 | FUN_00423b00 | Replay pre-step sub-1 |
| D-10564 | 0x00423270 | FUN_00423b00 | Replay pre-step sub-2 |
| D-10565 | 0x00423320 | FUN_00423b00 | Replay pre-step sub-3 |
| D-10566 | 0x0046cbe0 | FUN_00422ba0 | Spinout/state setter |
| D-10567 | 0x0044e0a0 | FUN_00422ba0 | Physics impulse applier |
| D-10568 | 0x004219f0 | FUN_00422ba0 | Type-9 event handler |
| D-10569 | 0x0047cea0 | FUN_00422ba0 | Explosion radius effect |
| D-10570 | 0x00415860 | FUN_004252c0 | Player interaction callback |
| D-10571 | 0x00426cc0 | FUN_004252c0 | Vehicle orientation/matrix getter |

## Pool slot

Mashed_pool0 — released after close.
