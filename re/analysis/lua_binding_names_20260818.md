# Original developer names recovered from the Lua binding tables — 2026-08-18

**70 names across 69 distinct functions.** Method is deterministic and reproducible;
nothing here is inferred from behaviour.

> **CORRECTION 2026-08-18.** The first version of this note claimed all 70 were absent from
> `hooks.csv`. That was WRONG, caused by a lookup bug: the comparison normalised keys to
> `0x0047a180` while `hooks.csv` stores RVAs unprefixed (`0047a180`), so nothing ever
> matched. The harvest itself is unaffected. True figures: **42 of the 69 RVAs were already
> tracked, all at C2; 27 are genuinely new.** Of the 42, three already carried *inferred*
> names, and the harvest confirms all three (`SkyFilename_Handler`/`Sky_Filename`,
> `SetupFog_Handler`/`Setup_Fog`, `ModifyFog_Handler`/`Modify_Fog`) — 3 for 3, which is
> stronger corroboration than the single case cited below. Net value is **39 renames of
> `FUN_*` rows + 27 new discoveries**, not 69 new functions.

## Why this exists

The prior naming lane was the log printer `FUN_004963e0`, which emits `"Calling <Name>
"`
before its target (commit `e62783ab`, 14 names). That lane is **exhausted**: `strings.txt`
holds exactly 14 `Calling ` strings and all 14 were already harvested.

This is a different and much larger vein: the engine registers its script-callable
entry points by pushing a function pointer and a name literal, so the binary itself
pairs every name with its implementation.

## Method

Two registrars, found by frequency rather than assumed:

| registrar | entries | what it registers |
|---|---:|---|
| `0x0047b980` | 68 | COURSE.LUA / powerup / physics-scenario script bindings |
| `0x004714f0` | 2 | per-track scripted event callbacks |

The call shape, read off a **confirmed** case (`Setup_Fog`, site `0x00440d1f`):

```
push 0x47ab30      ; function pointer  -- FIRST
push 0x5cde58      ; name literal
call 0x47b980      ; registrar
```

Harvest is a byte-pattern scan for `68 <imm32> 68 <imm32> E8 <rel32>` over `.text`,
keeping triples whose call target is a registrar and whose second immediate is a known
`.rdata` string. A linear disassembly sweep does **not** work here — it desyncs on
padding and inline data and finds zero call sites. `.rdata` maps as `VA = file offset
+ 0x400000` (cross-checked: `GameSaveBuffer.cpp` cites `0x005cc320`).

Reproduce: `re/analysis/lua_binding_names_20260818.json` holds every row with its
registration site, so each pairing is independently checkable at a named address.

## Corroboration and limits

- `0x0047ab30` = `Setup_Fog` **independently confirms** a prior hand-inference: Ghidra
  already carried `[C2 2026-06-02] COURSE.LUA Setup_Fog handler (5 args)` on that
  function. Two methods, same answer.
- **One genuine collision**: `0x0047ad30` is registered under BOTH
  `Vehicle_Body_Dirt_Texture` and `Vehicle_Wheel_Dirt_Texture`. Deterministic, not a
  pairing artifact — one handler serves two script names. [UNCERTAIN] how it
  discriminates; that needs the body read, not guessed.
- **Some targets have no Ghidra function at all** (`0x0047ad30`, `0x004917a0` probed
  `exact_entry=None`). These are undiscovered code, so this harvest yields new
  function starts as well as names.
- A first pass using nearest-neighbour pairing produced 3 wrong rows before the exact
  call shape was derived. Recorded because the failure is instructive: the heuristic
  looked plausible at 69/69 and was still wrong.

## What the names reveal structurally

The `*_Filename` / `*_Bsp_Filename` cluster is the COURSE.LUA asset loader; `Lap_Line*`,
`Split_Sector` and `Safe_Start_Lines` are lap/sector geometry; `RWP_Object_*` are the
RenderWare-Physics body builders; `Physics_Scenario_*` is a trigger/sequence system;
`Set_Current_*` plus `Place_Powerup` is powerup placement; `Rain*` is weather.

## Names

### Registrar `0x004714f0` — 2 entries

| Lua name | function | registration site |
|---|---|---|
| `EgyptPushColumn1` | `0x004496a0` | `0x00449780` |
| `RoundaboutStartCrane` | `0x0044ca40` | `0x0044ca70` |

### Registrar `0x0047b980` — 68 entries

| Lua name | function | registration site |
|---|---|---|
| `Course_Id` | `0x0047a180` | `0x00440bc0` |
| `Occluder_Filename` | `0x0047a1b0` | `0x00440c3b` |
| `Sky_Filename` | `0x0047a1e0` | `0x00440c4a` |
| `Mts_Filename` | `0x0047a280` | `0x00440c59` |
| `SetCopter` | `0x0047a320` | `0x00440fb6` |
| `Clump_Filename` | `0x0047a3a0` | `0x00440c68` |
| `UVA_Filename` | `0x0047a440` | `0x00440c77` |
| `Spline_Filename` | `0x0047a4a0` | `0x00440c86` |
| `AI_Bsp_Filename` | `0x0047a540` | `0x00440bcf` |
| `AI_Data_Filename` | `0x0047a580` | `0x00440bde` |
| `World_Bsp_Filename` | `0x0047a5b0` | `0x00440bed` |
| `Hoppy_Bsp_Filename` | `0x0047a5e0` | `0x00440bfc` |
| `Object_Bsp_Filename` | `0x0047a610` | `0x00440c0b` |
| `Collision_Bsp_Filename` | `0x0047a6b0` | `0x00440c1a` |
| `Texture_Dictionary_Filename` | `0x0047a6f0` | `0x00440c29` |
| `Lap_Variations` | `0x0047a720` | `0x00440d9a` |
| `Split_Sector` | `0x0047a750` | `0x00440de8` |
| `Lap_Line` | `0x0047a790` | `0x00440dac` |
| `Lap_Line_End` | `0x0047a7d0` | `0x00440dbb` |
| `Lap_Line_Change` | `0x0047a7f0` | `0x00440dca` |
| `Safe_Start_Lines` | `0x0047a810` | `0x00440dd9` |
| `Camera_Anim_Filename` | `0x0047a880` | `0x00440c95` |
| `General_Anim_Filename` | `0x0047a8b0` | `0x00440ca4` |
| `Clump_Mts_Pair` | `0x0047a950` | `0x00440cb6` |
| `Physics_Object` | `0x0047a9b0` | `0x00440cc5` |
| `Clump_Exclude_From_World` | `0x0047aa20` | `0x00440ce3` |
| `Powerup_Filename` | `0x0047aa50` | `0x00440cf2` |
| `Lights_Filename` | `0x0047aaa0` | `0x00440d01` |
| `Ambient_RGB` | `0x0047aad0` | `0x00440d10` |
| `Setup_Fog` | `0x0047ab30` | `0x00440d1f` |
| `Modify_Fog` | `0x0047abd0` | `0x00440d31` |
| `Vehicle_Shininess_Range` | `0x0047ac80` | `0x00440d40` |
| `Shine_Scale` | `0x0047acd0` | `0x00440d4f` |
| `Shadow_Scale` | `0x0047ad00` | `0x00440d5e` |
| `Vehicle_Body_Dirt_Texture` | `0x0047ad30` | `0x00440d6d` |
| `Vehicle_Wheel_Dirt_Texture` | `0x0047ad30` | `0x00440d7c` |
| `SetClumpAnim` | `0x0047ad40` | `0x00440d8b` |
| `RWP_Object` | `0x0047ade0` | `0x00440df7` |
| `RWP_Object_Box` | `0x0047ae80` | `0x00440e06` |
| `RWP_Object_Cylinder` | `0x0047aef0` | `0x00440e15` |
| `RWP_Object_Sphere` | `0x0047af50` | `0x00440e27` |
| `RWP_Object_Capsule` | `0x0047afa0` | `0x00440e36` |
| `RWP_Object_Properties` | `0x0047b000` | `0x00440e45` |
| `RWP_Object_CentreOfMassOffset` | `0x0047b0a0` | `0x00440e54` |
| `RWP_Object_BaseLabel` | `0x0047b110` | `0x00440e63` |
| `RWP_Object_ShatterVel` | `0x0047b160` | `0x00440e72` |
| `Set_Current_Position` | `0x0047b1b0` | `0x00440efc` |
| `Set_Current_Type` | `0x0047b200` | `0x00440f0b` |
| `Set_Current_Respawn_Time` | `0x0047b230` | `0x00440f1d` |
| `Place_Powerup` | `0x0047b250` | `0x00440f2c` |
| `Set_Track_Powerups` | `0x0047b280` | `0x00440f3b` |
| `Physics_Scenario_Create` | `0x0047b350` | `0x00440e81` |
| `Physics_Scenario_Set_Sequence` | `0x0047b3c0` | `0x00440e90` |
| `Physics_Scenario_Add_Objects` | `0x0047b420` | `0x00440ea2` |
| `Physics_Scenario_Add_Animated_Object` | `0x0047b480` | `0x00440eb1` |
| `Physics_Scenario_Create_Trigger` | `0x0047b500` | `0x00440ec0` |
| `Physics_Scenario_Trigger_Add_Objects` | `0x0047b5b0` | `0x00440ecf` |
| `Physics_Scenario_Get_Function` | `0x0047b630` | `0x00440ede` |
| `Physics_Scenario_Trigger_Set_AI_Poly` | `0x0047b6b0` | `0x00440eed` |
| `KTC_NewCopter` | `0x0047b720` | `0x00440fc5` |
| `KTC_AddPickUp` | `0x0047b7d0` | `0x00440fd4` |
| `RainEnable` | `0x004917a0` | `0x00440f4a` |
| `RainSetHeadColour` | `0x004917c0` | `0x00440f59` |
| `RainSetTailColour` | `0x00491860` | `0x00440f68` |
| `RainSetPosition` | `0x00491900` | `0x00440f77` |
| `RainSetScale` | `0x004919b0` | `0x00440f86` |
| `RainSetCameraScale` | `0x00491a10` | `0x00440f98` |
| `RainSetDirection` | `0x00491a70` | `0x00440fa7` |

## Follow-up (not done here)

These 69 functions are **not** added to `hooks.csv` by this note. Tracker rows are
mutated only through the `re-classify` skill, and 69 new rows with authoritative names
is a transaction for that skill, not a hand-edit. The registrars themselves
(`0x0047b980`, `0x004714f0`) are also unnamed and want rows.
