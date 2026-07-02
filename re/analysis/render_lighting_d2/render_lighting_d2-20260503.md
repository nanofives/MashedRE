# render_lighting — Session render_lighting_d2-20260503
Date: 2026-05-03
Slot: Mashed_pool13 (fallback; Mashed_pool4 held by concurrent session)
Analyst: Claude Sonnet 4.6

## Objective

Depth-2 continuation of session render_lighting-20260502-2221.
Resolve D-2200 (LIGHT_SETUP_FN not identified) and D-2201 (Lights_Filename consumer not traced).

## D-2201 Resolution — Lights_Filename consumer

### Config system architecture

`FUN_0047b980` (0x0047b980) is the config registration function:
```c
void FUN_0047b980(undefined4 param_1_string_key, undefined4 param_2_handler_fn) {
  thunk_FUN_004b7fd0(DAT_006bf1e0, param_2_handler_fn, 0);
  FUN_004b7250();
}
```
Called as `PUSH <handler_fn>; PUSH <key_string>; CALL FUN_0047b980`.

The config system is **callback-based** (not storage-slot based). The second argument is a
handler function pointer, not a storage address. When the config parser encounters a key,
it calls the registered handler with the parsed value string.

### Lights_Filename handler — FUN_0047aaa0 (0x0047aaa0–0x0047aace)

Registered as handler for "Lights_Filename" config key at 0x00440d01.

```c
undefined4 FUN_0047aaa0(undefined4 param_1) {
  // param_1 = parsed value string from config file
  uVar2 = FUN_004b6fc0(param_1);              // string processing (length/trim)
  pcVar3 = (char *)FUN_004b70d0(param_1, uVar2); // get char* from string object
  pcVar4 = (char *)(DAT_006bf1c8 + 0x2640);   // dest = course_desc_block + 0x2640
  do {
    cVar1 = *pcVar3;
    *pcVar4++ = cVar1;
    pcVar3++;
  } while (cVar1 != '\0');
  return 0;
}
```

Copies the filename string to `DAT_006bf1c8 + 0x2640`. `DAT_006bf1c8` is the course
description block pointer. The Lights_Filename value lands at byte offset **0x2640**
within that block.

### File consumer — FUN_00479330 (0x00479330–0x00479e7d)

"Creating course from description\n" log string confirms this is the course loader.
Parameter `param_2` is a `char*` pointing to the course description buffer (same block
as `DAT_006bf1c8` above).

The lighting branch at 0x00479820 (inferred; `ADD EAX, 0x2640` at 0x00479823):

```c
if (param_2[0x2640] == '\0') {
    // --- No Lights_Filename: create default lights inline ---
    uVar2 = FUN_004e4dd0(2);                              // RpLightCreate(rpLIGHTDIRECTIONAL=2)
    *(undefined4 *)(param_1 + 0x105e0) = uVar2;           // course->dirLight = light
    FUN_004e4900(uVar2, &DAT_006132dc);                   // RpLightSetColor(light, &defaultDirColor)
    *(unsigned char *)(*(int *)(param_1 + 0x105e0) + 2) = 1; // enable
    FUN_004e4810(*(undefined4 *)(param_1 + 0x105d4), *(unsigned4 *)(param_1 + 0x105e0));
                                                           // RpWorldAddLight(world, dirLight)
    uVar2 = FUN_004e4dd0(1);                              // RpLightCreate(rpLIGHTAMBIENT=1)
    *(undefined4 *)(param_1 + 0x105e4) = uVar2;           // course->ambLight = light
    FUN_004e4900(uVar2, &DAT_006132ec);                   // RpLightSetColor(light, &defaultAmbColor)
    *(unsigned char *)(*(int *)(param_1 + 0x105e4) + 2) = 1; // enable
    uVar2 = FUN_004c0b30();                               // get D3D device
    FUN_004c0740(*(undefined4 *)(param_1 + 0x105e4), uVar2); // D3D device association
    FUN_004c1520(...);                                     // set D3D light params
    FUN_004e4810(*(undefined4 *)(param_1 + 0x105d4), *(undefined4 *)(param_1 + 0x105e4));
                                                           // RpWorldAddLight(world, ambLight)
} else {
    // --- Lights_Filename set: load DFF containing embedded lights ---
    uVar2 = FUN_0042a5d0(param_2 + 0x2640, 0, 0);         // RpClumpStreamRead (load DFF)
    *(undefined4 *)(param_1 + 0x105dc) = uVar2;            // course->lightClump = clump
    iVar1 = FUN_004e6650(uVar2);                           // RpClumpGetNumLights(clump)
    FUN_004b4010(*(undefined4 *)(param_1 + 0x105dc), aiStack_420); // get lights array
    for (iVar4 = 0; iVar4 < iVar1; iVar4++) {
        iVar3 = aiStack_420[iVar4];                        // RpLight*
        if (*(char *)(iVar3 + 1) == 0x01)                 // subType == rpLIGHTAMBIENT
            *(int *)(param_1 + 0x105e4) = iVar3;           // course->ambLight = light
        else if (*(char *)(iVar3 + 1) == 0x02)            // subType == rpLIGHTDIRECTIONAL
            *(int *)(param_1 + 0x105e0) = iVar3;           // course->dirLight = light
        *(unsigned char *)(iVar3 + 2) = 1;                 // enable
        FUN_004e4810(*(undefined4 *)(param_1 + 0x105d4), iVar3); // RpWorldAddLight
    }
    // Optional ambient-color override from course config params ...
}
```

The `Lights_Filename` DFF file contains embedded `RpLight` objects. Each light's type
is read from `light+1` (rwObject.subType: 0x01=ambient, 0x02=directional).

## D-2200 Resolution — LIGHT_SETUP_FN

`LIGHT_SETUP_FN` is **not a separate function**. Light setup is inline in `FUN_00479330`
(course loader) at the branch on `param_2[0x2640]`.

When no light file is specified, the game creates default lights using `FUN_004e4dd0`
with constants `1` and `2` — these ARE the rpLIGHTAMBIENT and rpLIGHTDIRECTIONAL
type constants. Evidence confirming this:
- `FUN_004e4dd0(x)` allocates a struct, sets `struct[0] = 3` (rwObject type = rpLIGHT=3),
  sets `struct[1] = param_1` (subType = light type passed in)
- `*(char *)(light + 1) == 0x01|0x02` checks in the DFF branch read the same byte

The parent session's search for a standalone "RpLightCreate" was correct in concept
but the function is statically linked under name `FUN_004e4dd0`.

## New function identifications

| RVA | Name (proposed) | Evidence |
|-----|----------------|---------|
| 0x004e4dd0 | `RpLightCreate(type)` | Allocates struct; sets type=3 at +0, subType=param at +1; RGBA=1.0f×4 at +0x18..+0x24; initialises linked-list sentinels at +0x2c/+0x30/+0x34/+0x38 |
| 0x004e4810 | `RpWorldAddLight(world, light)` | Branches on `light[1] & 0x80`; inserts into world+0x34 (localLightList) or world+0x3c (directionalLightList) via intrusive doubly-linked list |
| 0x004e4900 | `RpLightSetColor(light, &RwRGBAReal)` | Writes 4 floats to light+0x18..+0x24; sets grey-flag at light+3 if R==G==B |
| 0x004e6650 | `RpClumpGetNumLights(clump)` | Traverses list at clump+0x10; returns count |
| 0x0047aaa0 | `config_Lights_Filename_handler(val)` | Copies string to config_block+0x2640 |
| 0x00479330 | `course_loader(course*, desc_buf*, params*)` | "Creating course from description"; loads TXD, BSP, collision BSPs, DFF clumps, sky, splines, camera paths, lights |

### RpLight struct layout (confirmed at 0x004e4dd0)

| Offset | Type | Field |
|--------|------|-------|
| +0x00 | uint8 | rwObject.type = 3 (rpLIGHT) |
| +0x01 | uint8 | rwObject.subType = lightType (1=ambient, 2=directional, ≥0x80=local) |
| +0x02 | uint8 | flags |
| +0x03 | uint8 | privateFlags (0x01 = grey optimization) |
| +0x04 | ptr | object.parent (frame) |
| +0x10 | ptr | destroy callback (= FUN_004d7ff0) |
| +0x14 | ptr | (unused/next callback?) |
| +0x18 | float | color.red |
| +0x1c | float | color.green |
| +0x20 | float | color.blue |
| +0x24 | float | color.alpha |
| +0x28 | float | radius (0 for ambient/directional) |
| +0x2c | ptr | lightList.next (self on init) |
| +0x30 | ptr | lightList.prev (self on init) |
| +0x34 | ptr | worldLinkList.next |
| +0x38 | ptr | worldLinkList.prev |
| +0x3c | int16 | pluginRef count |

### RpWorld offsets (confirmed at 0x004e4810)

| Offset | Field |
|--------|-------|
| +0x34 | localLightList (RwLinkList head, for point/spot lights ≥0x80) |
| +0x3c | directionalLightList (RwLinkList head, for ambient/directional <0x80) |

## Lighting architecture summary (resolves U-0767)

Mashed supports TWO lighting modes per track, selected at course-load time:

1. **Default lights** (`Lights_Filename == ""`): two RpLights created inline (directional=2,
   ambient=1) with default colors from `DAT_006132dc`/`DAT_006132ec`. These are true
   `RpLightCreate` objects added to the RpWorld.

2. **File-based lights** (`Lights_Filename = "trackname.dff"`): a DFF clump is loaded via
   `FUN_0042a5d0`; embedded RpLight objects are extracted by type byte and added to the world.
   The `nodeD3D9SubmitNoLight.csl` pipeline node (0x00618598) is a separate render-path
   override — it disables runtime lighting for specific geometry but coexists with RpWorld lights.

RpLightCreate IS called at runtime; the parent session's FidDB miss was because RW is
statically linked and the function was renamed to `FUN_004e4dd0`.

## Config registration block (context)

The code at 0x00440bc0–0x00440d20+ (immediately after FUN_0043dfd0's switch tables) is
a config registration sequence calling `FUN_0047b980(string_ptr, handler_fn_ptr)` for
each config key. It is not within a Ghidra-recognized function (unlabeled gap).
Interesting pairs observed:
- 0x00440d01: `PUSH 0x47aaa0; PUSH "Lights_Filename"; CALL FUN_0047b980`
- 0x00440d10: `PUSH 0x47aad0; PUSH "Ambient_RGB";     CALL FUN_0047b980`

Both 0x0047aaa0 and 0x0047aad0 are handler callbacks in the 0x0047a??? region.
`DAT_006bf1c8` is the course config block; handler at 0x0047aad0 sets the ambient RGB
value at some other offset within it.

## Deferred — none

D-2200 and D-2201 both fully resolved. No new depth-3 deferrals required.

## Scribe queue

Five new function IDs ready to write back to master:
- `FUN_004e4dd0` → `RpLightCreate`
- `FUN_004e4810` → `RpWorldAddLight`
- `FUN_004e4900` → `RpLightSetColor`
- `FUN_004e6650` → `RpClumpGetNumLights` (or equivalent)
- `FUN_0047aaa0` → `Lights_Filename_ConfigHandler`
- `FUN_00479330` → `course_loader` (partial; subsystem=render/track)

Scribe separately; do NOT scribe from this session.

---

## CORRECTION 2026-07-02 (session: RW lighting research follow-up, Mashed_pool0 read-only)

The sub-type reading above (lines ~85-88, ~130: "1=AMBIENT, 2=DIRECTIONAL") is **wrong —
reversed**. Re-decompile + disassembly of `FUN_00479330` (0x00479330) shows MASHED follows
the standard RW enum: **1 = rpLIGHTDIRECTIONAL, 2 = rpLIGHTAMBIENT**. Three independent
in-function witnesses:

1. `PUSH 0x2` @ 0x00479951 → `RpLightCreate(2)` @ 0x0047996b → stored `course+0x105e0`,
   color from `DAT_006132dc` = (0.25,0.25,0.25,1.0), **frameless** → ambient.
2. `PUSH 0x1` @ 0x0047999e → `RpLightCreate(1)` @ 0x004799a0 → stored `course+0x105e4`,
   color from `DAT_006132ec` = (0.75,0.75,0.75,1.0), **frame attached** (`FUN_004c0b30` @
   0x004799c0, `FUN_004c0740` @ 0x004799cd) and rotated 60.0f (0x42700000) about (1,0,0)
   via `FUN_004c1520` @ 0x004799e8 → directional.
3. The `Ambient_RGB` override block (0x0047990b-0x0047994c) applies the course-description
   floats (`param_3+0x76..0x78`) to the `course+0x105e0` light, creating it with
   `RpLightCreate(2)` @ 0x00479917 if absent → subtype 2 receives Ambient_RGB.

Additional fact missed above: **all three creation/extraction paths force the flags byte to
0x01 (rpLIGHTLIGHTATOMICS only)** — `MOV byte [reg+2],0x1` @ 0x00479922, 0x00479987,
0x004799bc, and in the DFF-extraction loop — overriding DFF stream flags (Arctic assets
carry 0x3). Runtime lights therefore illuminate atomics only, never world geometry.

Full write-up: `re/prior_art/notes/rw_lighting_research_2026-07.md` §5.3, §8.
