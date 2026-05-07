# SESSION_END — render_lighting_d2-20260503

## Status: COMPLETE — both deferred rows resolved

## Deferred rows closed

| ID | Resolution |
|----|-----------|
| D-2200 | RESOLVED: LIGHT_SETUP_FN is inline in FUN_00479330 (course loader). Default-light branch at param_2[0x2640]=='\0' calls FUN_004e4dd0(2) then FUN_004e4dd0(1) — rpLIGHTDIRECTIONAL and rpLIGHTAMBIENT. No separate standalone function. |
| D-2201 | RESOLVED: FUN_0047aaa0 (0x0047aaa0) is the Lights_Filename config callback; copies string to DAT_006bf1c8+0x2640. FUN_00479330 (course loader) reads param_2+0x2640 and calls FUN_0042a5d0 to load the light DFF. |

## Uncertainty closed

| ID | Resolution |
|----|-----------|
| U-0767 | CLOSED: Two lighting modes confirmed. (1) Default: inline RpLightCreate calls in course loader. (2) File-based: DFF loaded, lights extracted by subType byte. nodeD3D9SubmitNoLight.csl is a separate geometry render-path override, coexists with RpWorld lights. |

## New symbols for scribe

| RVA | Proposed Name | Subsystem | Evidence Summary |
|-----|--------------|-----------|-----------------|
| 0x004e4dd0 | RpLightCreate | render | struct[0]=3(rpLIGHT), struct[1]=lightType; RGBA at +0x18; linked-list init at +0x2c/+0x30 |
| 0x004e4810 | RpWorldAddLight | render | branch on light[1]&0x80; inserts into world+0x34 (local) or world+0x3c (dir) list |
| 0x004e4900 | RpLightSetColor | render | writes 4 floats to light+0x18..+0x24; grey-flag at light+3 |
| 0x004e6650 | RpClumpGetNumLights | render | traverses list at clump+0x10; returns count |
| 0x0047aaa0 | Lights_Filename_ConfigHandler | render | copies config value string to config_block+0x2640 |
| 0x00479330 | course_loader | render/track | "Creating course from description"; loads all track assets including lights |

## Depth-3 continuation

None required. D-2200 and D-2201 both fully resolved with no new unknowns spawned.

## Session notes

Full analysis: re/analysis/render_lighting_d2/render_lighting_d2-20260503.md
