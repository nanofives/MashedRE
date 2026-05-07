# SESSION_END — audio_sfx_dispatch_d2

**Status**: COMPLETE — 14 C1, 0 deferred
**Session ID**: audio_sfx_dispatch_d2-20260507
**Slot**: Mashed_pool13 (stale lock cleared, released cleanly)
**SHA-256**: bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e ✓

## Functions mapped (14)

| U-ID | RVA | Name |
|------|-----|------|
| U-2427 | 0x0045efe0 | 2-ch sinusoidal ambient + 3-ch group + 2 conditional one-shots |
| U-2428 | 0x0045f5f0 | 4-ch sinusoidal vol ambient + 2 conditional one-shots |
| U-2429 | 0x0045faa0 | 8-ch ambient; speed-scaled + static 3D groups + dynamic player pos |
| U-2430 | 0x0045ff50 | 3-ch ambient + rand-pos conditional one-shot |
| U-2431 | 0x00460350 | 13-ch track ambient; 10 static 3D pos; FUN_004495d0 dynamic pos |
| U-2432 | 0x00460df0 | 11-ch track ambient; 3 collinear channels; rand one-shot |
| U-2433 | 0x00461650 | In-world item proximity SFX; 8-slot pool; special-item secondary |
| U-2434 | 0x00463640 | Per-vehicle engine RPM state machine (5 states + gear tables) |
| U-2435 | 0x00463c80 | Per-vehicle surface collision SFX; sinusoidal speed-vol |
| U-2436 | 0x00463f40 | Per-entity 2-wheel tire/surface SFX; surface material switch |
| U-2437 | 0x00464e10 | Crowd/spectator ambient; path-positioned; periodic rand one-shots |
| U-2438 | 0x00465a30 | 4-slot resequenced SFX; descending pitch; first-free-slot |
| U-2439 | 0x00465b20 | 2-slot entity SFX; player-index pitch; first-free-slot |
| U-2440 | 0x004661f0 | Track-triggered ambient; camera-pos ch; X==116.41855 trigger; 9 rand one-shots |

## D-range

No deferments — D=7180..7239 range **unused**. All 14 resolved to C1 in single pass.

## Notable findings

- **0x00463640**: Engine RPM state machine with 5 states (normal/upshift-hold/downshift/coast/airborne), per-vehicle gear tables at DAT_006130xx, sinusoidal vol modulation vs RPM freq. Most complex function in this batch.
- **0x004661f0**: Hardcoded track trigger at `pfVar4[0] == 116.41855` (exact float match) — this is a specific track position coordinate, not a parameterized distance check.
- **0x00465a30 / 0x00465b20**: The two "variant of 00465940" stubs were both clearly distinct — a30 is descending-pitch 4-slot player, b20 is ascending-pitch 2-slot player-indexed variant. Both called by 0x004661f0.
- All 14 follow the `DAT_0068f4c8 == 0 || DAT_008aa254 < 2` guard pattern confirming shared audio subsystem entry condition.
