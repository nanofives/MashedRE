# Session End: ai_update_d6-20260513

## Session summary

Drained all 9 DEFERRED rows tagged `ai_update_d6` (D-10560..D-10571), plus 16 depth-1 fill functions. Pool slot Mashed_pool2.

## DEFERRED resolution

| D-ID | RVA | Status | Notes |
|------|-----|--------|-------|
| D-10560 | 0x0041ea80 | New C1 | Lap-line gate check; 8b |
| D-10561 | 0x0041e960 | New C1 | Lap-line vtable dispatch; 8b; U-3703 |
| D-10562 | 0x00422b50 | New C1 | Vehicle damage delta accumulator; 74b |
| D-10566 | 0x0046cbe0 | Drift-clear | Already C2 in util_c0_promote-20260512 |
| D-10567 | 0x0044e0a0 | Drift-clear | Already C1 in random_rng_d2-20260513 |
| D-10568 | 0x004219f0 | Drift-clear | Already C1 in random_rng_d2-20260513 |
| D-10569 | 0x0047cea0 | New C1 | Explosion radius physics activator; 126b; U-3706 |
| D-10570 | 0x00415860 | New C1 | Player interaction cooldown setter; 17b |
| D-10571 | 0x00426cc0 | New C1 | Vehicle state matrix base getter; 12b |

## Depth-1 fill (11 new + 2 drift)

| RVA | Size | Subsystem | Notes |
|-----|------|-----------|-------|
| 0x0048aa20 | 43b | render | Physics body spawn thunk (+1.0f to FUN_0048a850) |
| 0x004219a0 | 19b | ai | Powerup slot null-guard (EAX-based) |
| 0x00487020 | 283b | render | Surface constraint applicator (Bezier+FUN_004b5080); U-3707 |
| 0x0057c210 | 13b | render | Physics world body pointer lookup: *(DAT_007dc8d8+p1) |
| 0x0055ac50 | 196b | render | Physics joint constraint toggle (bitfield + vtable callback) |
| 0x0055ac00 | 77b | render | Physics contact shape flag setter |
| 0x0055ad30 | 160b | render | Physics body activator (vtable + DAT_007dc8c0) |
| 0x0055b650 | 250b | render | Physics force+torque at world point (cross product) |
| 0x00481750 | 39b | vehicle | Vehicle physics impulse wrapper via DAT_006c9a78 |
| 0x00480d60 | 2493b | vehicle | Multi-case vehicle impulse dispatcher (9 cases); U-3708 |
| 0x004b4550 | — | — | DRIFT: already C1 in random_rng_d2-20260513 |

## Tracker changes

- hooks.csv: +15 rows (new C1; excludes 3 drifts)
- DEFERRED.md: 9 rows marked RESOLVED (6 new + 3 drift)
- UNCERTAINTIES.md: +6 rows (U-3703..U-3708)
- SCRIBE_QUEUE.md: +1 row

## Key structural findings

- **Track-descriptor object** `DAT_0063d7e4+0x40`: the same memory location serves as both a gate flag (`FUN_0041ea80` reads it as int, caller tests !=0) and a virtual dispatch target (`FUN_0041e960` calls through it as code**). This is a virtual-null-guarded dispatch pattern on the lap-line slot of the track descriptor object.
- **Physics activation sequence** for explosion events: `FUN_0055ac50` (joint enable) → `FUN_0055ac00` (contact shape flag) → `FUN_0055ad30` (body activate in world) → `FUN_0055b650` (apply force). Global overflow flag `DAT_007dc8c0`.
- **DAT_006c71d8** (entity object table, ≥200 entries) vs **DAT_006c9a78** (vehicle body index table): two distinct lookup tables for physics bodies; `FUN_0047cea0` uses the entity table, `FUN_00481750`/`FUN_00480d60` use the vehicle table.
- **Powerup slot table** at `DAT_0063fb90`: 4 outer × 2 middle × 10 inner = 80 slots (stride 0x208/0x100/0x18). Deactivation path for type-9 events clears a bit in the middle-entry bitfield and applies surface constraint.
- **Vehicle impulse dispatch** `FUN_00480d60` uses the same stride-0x341 entity array (`DAT_00881ec8`) for velocity reads; velocity fields at +0x74/+0x78/+0x7c and +0x68/+0x6c/+0x70 (relative to entry base) are the force source vectors.

## Pool slot

Mashed_pool2 — released after close.
