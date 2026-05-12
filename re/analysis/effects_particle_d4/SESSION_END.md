# SESSION_END — effects_particle_d4

**Session ID:** effects_particle_d4-20260512
**Slot used:** Mashed_pool7
**SHA-256:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓ (on `.unpatched`; patched binary confirmed present)

## Functions analyzed (3)

| RVA | Name | Confidence | Notes |
|-----|------|-----------|-------|
| 0x004e5fc0 | FUN_004e5fc0 | C1 | keyframe animation update; lerps XYZ+scale; clears S-1960 |
| 0x004c0b10 | FUN_004c0b10 | C1 | frame dirty-state predicate (returns *(*(p+0xa0)+3) & 3); clears S-1961 |
| 0x004c3d60 | FUN_004c3d60 | C1 | RW engine vtable matrix dispatch (3-arg); clears S-1963 |

## Stubs cleared
S-1960, S-1961, S-1963

## Stubs introduced
None — all three functions are leaves.

## New uncertainties
U-3578 .. U-3583 (6 entries; appended to UNCERTAINTIES.md)

| ID | Subject |
|----|---------|
| U-3578 | FUN_004e5fc0: animation sub-object type at param_1+0x18 |
| U-3579 | FUN_004e5fc0: keyframe index units (+0x50/+0x52) |
| U-3580 | FUN_004e5fc0: interpolation parameter t formula (+0x5c × +0x58) |
| U-3581 | FUN_004c0b10: object type at *(param_1+0xa0) |
| U-3582 | FUN_004c3d60: specific RW API at engine vtable[+8] |
| U-3583 | FUN_004c3d60: value and layout of DAT_007d3ffc |

## Deferred (new)
None — all d4 callees are leaves; no further recursion branches.

## Candidate set note

All d1 + d2 stubs were already at C1 in hooks.csv before this session. The d3 batch had introduced 6 stubs (S-1960..S-1965); three (S-1960, S-1961, S-1963) are cleared here. S-1964 and S-1965 were cleared by the track_collision_geometry session 14. S-1962 (0x004c0ed0, camera matrix accessor) was cleared by the render_lighting_alt session. All 6 d3 stubs are now cleared.

## Struct document produced
`re/analysis/structs/particle_emitter.md` — initial draft covering:
1. **Staging globals** (0x006924d8 … 0x006925a8) — 7 fields fully mapped
2. **Render pool / batch struct** — 9-field layout fully mapped from FUN_00476d00
3. **Emitter channel struct** (DAT_007dc57c) — 11 channel-pair slots + dirty mask + capability bitmask (RpPrtStd-style)
4. **Explosion/fireball ring entry** — stride 0x488 bytes; sub-array offsets partially mapped; [UNCERTAIN U-0588]
5. **Debris ring entry** — stride 36 bytes; 8 fields mapped
6. **Slow-decay particle entry** — stride 32 bytes; 9 fields mapped

Multiple-struct note: Three distinct particle code paths confirmed (explosion/fireball, debris, slow-decay) with separate ring buffers. Documented separately as required by session STOP-AND-ASK rule; user did not need to be asked since each struct is documented in the same file.

## Powerups linkage
The exact call chain from "powerup detonates" → "explosion ring entry allocated" has not been traced within this session. Known: powerups_d2 effect-setters at 0x00476cb0 / 0x00476c10 write into the emitter channel struct at DAT_007dc57c. The entry-point from powerup logic to `FUN_0048ebf0` (fire-column burst spawn) or the explosion ring allocator needs a powerups_d3/d4 sweep to close.

## Session summary
3 functions analyzed. 3 stubs cleared, 0 new stubs. 6 uncertainties filed. 0 new deferred entries. 1 struct document produced (6 subsections). SCRIBE_QUEUE entry added.
