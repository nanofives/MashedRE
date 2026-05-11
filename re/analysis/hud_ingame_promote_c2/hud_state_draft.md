# HUD State Draft — for struct-extract session (Session 6)

**Source:** hud_ingame_promote_c2 (2026-05-11, pool3)  
**Confidence of this draft:** evidence-only; all semantic labels are [UNCERTAIN]  
**Key finding:** No common pointer observed — all HUD state is accessed as direct static globals in the 0x0063xxxx range. There is no `DAT_007xxxxx` pointer used as a struct base by ≥3 functions; the mission's "common pointer" assumption does not hold. The data IS cohesive (see layout below) but static, not heap/dynamic.

---

## Cluster A — HUD state block (0x0063xxxx range)

Approximately 0x0063ba8c..0x0063d7e4 (span ≈ 7512 bytes). Accessed directly, not through a pointer.

### Known field addresses (with evidence)

| Address | Size | Type | Observed role | Evidence (RVA) |
|---------|------|------|---------------|----------------|
| 0x0063ba8c | 4 | int32_t | race-state gate: must be 5, 6, or 7 for HUD draw | 0x0040dfc0 |
| 0x0063c628 | 4 | int32_t | enable-flag: game-mode 10 HUD draw path | 0x0041a3e0 |
| 0x0063c8d0 | 4×0x74=0x1d0 | struct[4] | Array_A: 4 entries × 0x74 bytes each | 0x0041b630 |
| 0x0063c8d0+n×0x74+0x6c | 4 | int32_t | Array_A per-entry enable flag | 0x0041b630 |
| 0x0063cab8 | 2×0x16c=0x2d8 | struct[2] | Array_B: 2 entries × 0x16c bytes each | 0x0041c0c0 |
| 0x0063cdbc | 4 | int32_t | enable-flag: game-mode 5 HUD draw path | 0x0041c300 |
| 0x0063ce20 | 4×0x114=0x450 | struct[4] | Array_C: 4 entries × 0x114 bytes each | 0x0041ccc0 |
| 0x0063ce20+n×0x114+0x110 | 4 | int32_t | Array_C per-entry enable flag | 0x0041ccc0 |
| 0x0063d298 | 2×0x160=0x2c0 | struct[2] | Array_D: 2 entries × 0x160 bytes each | 0x0041d870 |
| 0x0063d55c | 4 | int32_t ptr | HUD element object pointer 1 (vtable+0x48 dispatch) | 0x0041db80 |
| 0x0063d560 | 4 | int32_t ptr | HUD element object pointer 2 | 0x0041db80 |
| 0x0063d564 | 4 | int32_t ptr | HUD element object pointer 3 | 0x0041db80 |
| 0x0063d568 | 4 | int32_t ptr | HUD element object pointer 4 | 0x0041db80 |
| 0x0063d56c | 4 | int32_t ptr | HUD element object pointer 5 | 0x0041db80 |
| 0x0063d570 | 4 | int32_t ptr | HUD element object pointer 6 | 0x0041db80 |
| 0x0063d588 | 4 | undefined4 | counter/timer for threshold dispatch in FUN_0041db80; [UNCERTAIN] int32_t or float32 | 0x0041db80 |
| 0x0063d5e8 | 4 | int32_t | enable-flag: game-modes 4/7/8/9 HUD draw path | 0x0041ded0 |
| 0x0063d7e0 | 4 | int32_t | enable-flag: sub-mode 2 HUD draw path | 0x0041e850 |

### Gaps in the layout (fields not yet accessed by scribed functions)

| Range | Size | Status |
|-------|------|--------|
| 0x0063ba90..0x0063c628 | 0xd98 | unobserved between race_state and mode10_enable |
| 0x0063caa0..0x0063cab8 | 0x18 | gap between Array_A and Array_B |
| 0x0063cd90..0x0063cdbc | 0x2c | gap between Array_B and mode5_enable |
| 0x0063cdc0..0x0063ce20 | 0x60 | gap between mode5_enable and Array_C |
| 0x0063d270..0x0063d298 | 0x28 | gap between Array_C and Array_D |
| 0x0063d558..0x0063d55c | 0x04 | 4 bytes between Array_D end and object ptr 1 |
| 0x0063d574..0x0063d588 | 0x14 | gap between object ptr 6 and timer |
| 0x0063d58c..0x0063d5e8 | 0x5c | gap between timer and modes4789_enable |
| 0x0063d5ec..0x0063d7e0 | 0x1f4 | gap between modes4789_enable and submode2_enable |

---

## Cluster B — Game-state globals (separate from HUD block)

These are accessed by getter functions (FUN_00426ba0, FUN_0042f6a0, FUN_0042f500) and read directly in FUN_0040dfc0. They are **not** in the 0x0063xxxx range and are not part of the HUD state block.

| Address | Size | Type | Observed role | Evidence (RVA) |
|---------|------|------|---------------|----------------|
| 0x0066d704 | 4 | undefined4 | HUD draw-enable flag (FUN_00426ba0 getter) | 0x00426ba0 |
| 0x0067e9fc | 4 | undefined4 | game sub-mode (FUN_0042f6a0 getter) | 0x0042f6a0 |
| 0x0067ea64 | 4 | undefined4 | HUD path discriminator (FUN_0042f500 getter) | 0x0042f500 |
| 0x007f0fd0 | 4 | int32_t | game-mode inner switch discriminant | 0x0040dfc0 |

---

## Cluster C — Render context object (accessed through pointer)

| Pointer address | Offset | Size | Observed role | Evidence |
|-----------------|--------|------|---------------|----------|
| DAT_007d3ff8 | +0x18 | undefined4 | z value for Im2D vertices | 0x00450b10 |
| DAT_007d3ff8 | +0x20 | code ptr | vtable: render-state setter / texture binder | 0x00450b10, 0x00403160 |
| DAT_007d3ff8 | +0x30 | code ptr | vtable: Im2D primitive draw | 0x00450b10 |

---

## Cluster D — Misc HUD globals

| Address | Size | Type | Observed role | Evidence |
|---------|------|------|---------------|----------|
| 0x0067d974 | 4 | float32 | spin angle accumulator (coin animator) | 0x00428450 |
| 0x00771960 | 4 | undefined4 * | pointer to texture handle for coin | 0x00428450 |
| 0x00771964 | 4 | int32_t | flag/handle for FUN_00428760 call | 0x00403160 |

---

## Notes for session 6 (struct-extract)

1. **Array types need session 6 analysis:** Array_A (0x74/entry), Array_B (0x16c/entry), Array_C (0x114/entry), Array_D (0x160/entry) — only the enable-flag offsets are known at C2. All other fields are unobserved.
2. **HUD element objects at 0x0063d55c..0x0063d570** — 6 vtable pointers; vtable slot +0x48 is a draw method. Object identities unknown.
3. **DAT_0063d588 type** — must be resolved before Array_D and the threshold system can be fully typed; the 5 threshold constants in .rdata (0x005cc35c etc.) need type checks (float32 vs int32_t) in Ghidra.
4. **Gaps** — the large gap 0x0063ba90..0x0063c628 (0xd98 = 3480 bytes) suggests the race-state field at 0x0063ba8c belongs to a different sub-struct (possibly a race/game state struct rather than HUD state).
5. **No pointer found** — the "shared HUD struct" as accessed through a 0x007xxxxx pointer does not exist in these 16 functions. The HUD state is a large BSS block with direct static access.
