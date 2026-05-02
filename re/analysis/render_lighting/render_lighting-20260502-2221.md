# render_lighting — Session render_lighting-20260502-2221
Date: 2026-05-02  
Slot: Mashed_pool6  
Analyst: Claude Sonnet 4.6

## Objective

Identify LIGHT_SETUP_FN — function that creates the game world's ambient + directional lights
(heuristic: calls RpLightCreate 2+ times with different rpLIGHT* type constants).

## Halt condition triggered

**RpLight anchors not found. Lighting may be baked or use a different system.**

## Evidence

### Import table
`external_imports_list` → 177 imports. Zero RpLight* entries. RenderWare 3.x is statically linked
into MASHED.exe; the FidDB (Function ID Database) produced no RpLight* function matches during
Ghidra auto-analysis.

### String search
- `search_defined_strings("RpLight")` → 0 results
- `search_defined_strings("ambient")` → "Ambient_RGB" at 0x005cde64 (config key only)
- `search_defined_strings("Light")` → "Lights_Filename" (config key), "8headlight", "startlights.dff",
  "Headlight", "nodeD3D9SubmitNoLight.csl"

### Key findings

#### `nodeD3D9SubmitNoLight.csl` at 0x00618598
Pipeline node string. A custom D3D9 rendering pipeline that explicitly bypasses standard
RenderWare world lighting. Referenced only as a data pointer; no function in this session's
explored callers uses it as a code branch. This is the strongest indicator that Mashed does NOT
use RpLightCreate/RpWorldAddLight — it routes geometry through a no-light D3D9 pipeline instead.

#### "Lights_Filename" config key
Registered by the large switch-case config registration function at ~0x0043dfd0. The config system
maps string keys to global pointer slots (e.g., "Ambient_RGB" → 0x0047aad0, "Lights_Filename" →
0x0047aaa0). The function that actually reads the Lights_Filename pointer and loads the file was
not traced in this session. The file-based system (if it exists) uses a custom format, not
RpLightCreate API calls directly.

#### "startlights.dff" → FUN_0041d8b0 (0x0041d8b0–0x0041d90f)
```c
void FUN_0041d8b0(void) {
  iVar1 = FUN_0042a5d0("startlights.dff", 0, 0);  // load DFF clump
  FUN_004b6520(&DAT_0063d55c, 0x30);               // zero 0x30 bytes
  DAT_0063d580 = *(undefined4 *)(iVar1 + 4);
  DAT_0063d57c = iVar1;
  FUN_004b51d0(iVar1, &DAT_0063d55c, 8, 0, 0);    // extract atomics/frames
  puVar2 = &DAT_0063d55c;
  do {
    FUN_004b52f0(*puVar2, 0x40, 1);                // process each element
    puVar2 = puVar2 + 1;
  } while ((int)puVar2 < 0x63d574);               // 6 iterations
}
```
Loads `startlights.dff` as a RenderWare DFF geometry clump (the start-line light prop model),
extracts 6 atomics/frames into `DAT_0063d55c`. NOT RpLightCreate — this is 3D geometry for the
start/finish line lamp post prop, not a world light source.

#### RW engine init chain explored
- FUN_00493710 → RwEngineInit, RtFSManagerOpen, RwEngineOpen, RwEngineStart
- FUN_00492370 → top-level app init
- FUN_00402750 → AppInitialiseOnBootup (PIZ setup, cameras, HUD, subsystem inits)
- FUN_00426e10 → track loader (PIZ + Lua scripts, no RpLightCreate)
- FUN_004671a0 → camera accessor (returns DAT_006905b0)
- 37 callers of FUN_004671a0 explored; none identified as LIGHT_SETUP_FN

#### D3D9 vtable dispatch — FUN_004c1bb0 (0x004c1bb0)
```c
uint FUN_004c1bb0(uint param_1, undefined4 param_2, undefined4 param_3) {
  iVar1 = (**(code **)(DAT_007d3ff8 + 0x9c))(param_1, param_2, param_3);
  return -(uint)(iVar1 != 0) & param_1;
}
```
D3D9 vtable method dispatch (buffer clear/set operations). NOT RpWorldAddLight.

## Architectural interpretation [UNCERTAIN U-0767]

Evidence is consistent with one of these lighting architectures:
1. **Vertex-colored / prelit geometry** — RW supports per-vertex pre-baked lighting; RpLightCreate
   never called at runtime. Most likely given `nodeD3D9SubmitNoLight.csl`.
2. **Custom light file format** — "Lights_Filename" config key → file loaded per-track, parsed
   into custom light descriptors, applied via D3D9 directly or via custom RW plugin. Would still
   not use RpLightCreate if the custom pipeline bypasses RW's world-light system.
3. **Ambient_RGB config only** — track ambient set via a color constant, no directional lights.
   Consistent with the fast-paced top-down/isometric view where directional lighting matters less.

## Gaps (deferred)

- D-2200: LIGHT_SETUP_FN not identified
- D-2201: "Lights_Filename" config key consumer not traced

## Functions incidentally noted

| RVA | Name | Observation |
|-----|------|-------------|
| 0x0041d8b0 | FUN_0041d8b0 | Loads startlights.dff DFF clump; extracts 6 atomics into DAT_0063d55c |
| 0x004671a0 | FUN_004671a0 | Camera accessor; returns DAT_006905b0 (camera ptr) unless state==3 |
| 0x004c1bb0 | FUN_004c1bb0 | D3D9 vtable dispatch at DAT_007d3ff8+0x9c; buffer ops |
| 0x00493710 | FUN_00493710 | RW engine init: RwEngineInit→RwEngineOpen→RwEngineStart |
| 0x00402750 | FUN_00402750 | AppInitialiseOnBootup — calls FUN_0041d8b0 for startlights.dff |

## Scribe notes

No master writes queued. LIGHT_SETUP_FN not identified; no C1+ RVAs to scribe back.
