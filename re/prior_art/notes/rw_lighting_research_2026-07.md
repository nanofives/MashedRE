# RenderWare 3.x lighting & material pipeline — research notes (2026-07-02)

Purpose: reference notes for implementing **vehicle lighting** in `mashed_re` (D3D9 RW-subset
port). Compiled from a deep-research web sweep (librw / re3 / gta-reversed / leaked RW 3.7.0.2
source / MSDN) cross-checked against local repo evidence (vendored headers, MASHED RE notes,
`verify/ws_e_lighting/`).

**Verification legend** (every claim below carries one):
- `[LOCAL]` — verified directly against a file checked into this repo (path:line cited).
- `[WEB-CONFIRMED n-0]` — adversarially verified by the deep-research workflow (n refuter votes, none refuted), with quote.
- `[WEB-UNVERIFIED]` — extracted from a fetched source but the verify pass was killed by a
  session-limit outage (60/101 verifier agents failed 2026-07-01); treat as candidate-grade
  unless also marked `[LOCAL]`.
- `[WEB-REFUTED 0-3]` — claim extracted then killed by 3/3 refuters; recorded as a correction.
- `[MASHED-RE]` — from this repo's own Ghidra RE notes (RVA cited); MASHED.exe ground truth.

Provenance caveat: the deep-research run hit a session-limit wall mid-verify; its synthesis
step failed, so this document is the synthesis. Raw run: workflow `wf_7e1055ed-84d`
(93 claims extracted, 25 reached verification, 4 confirmed, 2 refuted, 19 unverified).

---

## 1. RpLight — types, flags, struct

### 1.1 Type enum

Identical across all four independent witnesses:

```c
rpNALIGHTTYPE     = 0
rpLIGHTDIRECTIONAL = 1
rpLIGHTAMBIENT     = 2
rpLIGHTPOINT       = 0x80   // rpLIGHTPOSITIONINGSTART — positioned types begin here
rpLIGHTSPOT        = 0x81
rpLIGHTSPOTSOFT    = 0x82
```

- `[LOCAL]` gta-reversed vendored header: `re/prior_art/renderware/gta-reversed-modern/source/game_sa/RenderWare/rw/rpworld.h:1205-1219` (`rpLIGHTPOSITIONINGSTART 0x80` at :1196).
- `[LOCAL]` librw local clone (commit 1252b90, 2026-04-28): `re/prior_art/renderware/librw/src/rwobjects.h:708-714` (`DIRECTIONAL = 1, AMBIENT, POINT = 0x80, SPOT, SOFTSPOT`).
- `[WEB-UNVERIFIED]` leaked RW 3.7.0.2 source `world/balight.h` — same values (github.com/sigmaco/rwsrc-v3.7.0.2).
- `[WEB-UNVERIFIED]` re3 fakerw `rpworld.h` — same values (gitlab.eientei.org mirror, lcs branch).

Distinct from the rwObject *type* ID: `#define rpLIGHT 3` (`rpworld.h:1193` `[LOCAL]`;
librw `enum { ID = 3 }` `rwobjects.h:673` `[LOCAL]`). MASHED's own RpLight has `3` at
byte +0x00 and the sub-type at +0x01 — see §1.3.

### 1.2 Light flags

```c
rpLIGHTLIGHTATOMICS = 0x01   // illuminates atomics (cars, props)
rpLIGHTLIGHTWORLD   = 0x02   // illuminates static world geometry
```
- `[LOCAL]` `rpworld.h:1248-1254`; librw `rwobjects.h:715-718` (`LIGHTATOMICS = 1, LIGHTWORLD = 2`).
- Private flag `rpLIGHTPRIVATENOCHROMA = 0x01` (grey-light optimization) — `rpworld.h:1259-1264` `[LOCAL]`.
- `[WEB-UNVERIFIED]` re3 (`src/rw/Lights.cpp`) creates exactly one ambient + one directional
  world light and enables them with `rpLIGHTLIGHTATOMICS` **only** — atomics, not world
  geometry, are lit by RpLights in GTA3. Mashed's *assets* differ: its Arctic LIGHTS.DFF
  lights carry stream flags `0x3` (both ATOMICS and WORLD) — asset-verified comment at
  `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp:344-387` `[LOCAL]`. But at *runtime*
  Mashed matches the re3 convention: `FUN_00479330` overwrites every light's flags byte to
  `0x01` (LIGHTATOMICS only) after creation/extraction — `MOV byte [reg+2],0x1` at
  0x00479922, 0x00479987, 0x004799bc, and in the DFF-extraction loop `[MASHED-RE, decompiled
  + disassembled 2026-07-02 in Mashed_pool0]`. This is the static explanation for the s4
  empirical result (world must NOT get runtime ambient re-added; see §6/§7).

### 1.3 Struct layout

RW 3.7 SDK shape (`[LOCAL]` `rpworld.h:1277-1288`, `[WEB-UNVERIFIED]` identical in leaked 3.7.0.2):

```c
struct RpLight {
    RwObjectHasFrame object;              // frame-bearing header
    RwReal           radius;
    RwRGBAReal       color;               // 4 floats
    RwReal           minusCosAngle;       // spot cone
    RwLinkList       WorldSectorsInLight;
    RwLLLink         inWorld;
    RwUInt16         lightFrame, pad;
};
```

**MASHED.exe binary layout** `[MASHED-RE]` — confirmed from the constructor `FUN_004e4dd0`
(= RpLightCreate, hooks.csv C1) in `re/analysis/render_lighting_d2/render_lighting_d2-20260503.md:125-146`:

| offset | field |
|---|---|
| +0x00 | type = 3 (rpLIGHT object ID) |
| +0x01 | subType (light type byte) |
| +0x02 | flags |
| +0x03 | privateFlags (0x01 = grey) |
| +0x04 | frame ptr |
| +0x10 | destroy callback |
| +0x18..+0x24 | color RGBA floats |
| +0x28 | radius |
| +0x2c/+0x30 | lightList links |
| +0x34/+0x38 | worldLink links |
| +0x3c | pluginRef |

RpWorld keeps two light lists `[MASHED-RE]` (same note :147-152): world+0x34 = localLightList
(positioned types ≥0x80), world+0x3c = directionalLightList (ambient/directional <0x80).

**Stream form** (what LIGHTS.DFF contains) — `RpLightChunkInfo`, 24-byte STRUCT body
(`[LOCAL]` `rpworld.h:1181-1189`; exactly what the port's parser decodes,
`TrackRenderer.cpp:344-387`):

```c
{ RwReal radius; RwReal red, green, blue; RwReal minusCosAngle;
  RwUInt32 typeAndFlags; }   // type = >>16, flags = &0xffff
```

librw's own `Light` differs (adds clump extension, drops sector list) — reading aid only,
not binary-compatible: `librw/src/rwobjects.h:670-684` `[LOCAL]`. Notably it stores **only
radius + minusCosAngle + RGBAf color — no attenuation coefficient fields**; distance
attenuation is the driver's job `[LOCAL struct / WEB-UNVERIFIED interpretation]`.

---

## 2. Attenuation (point/spot — LOW priority for Mashed, see §5)

- `[WEB-UNVERIFIED]` RW 3.7.0.2 docs describe point/spot/soft-spot intensity as **linear
  fall-off reaching zero at the light's radius** (sigmaco/rwsrc-v3.7.0.2).
- `[WEB-UNVERIFIED]` RW's D3D9 world pipeline (`world/pipe/p2/d3d9/D3D9lights.c`, leaked
  3.7.0.2) maps local lights to fixed-function `D3DLIGHT9` as:
  `Range = radius`, `Attenuation0 = constant`, `Attenuation1 = linear/radius`,
  `Attenuation2 = quadratic/(radius*radius)`, with plugin defaults
  `constant=1.0, linear=0.0, quadratic=5.0`. Both `rpLIGHTSPOT` and `rpLIGHTSPOTSOFT` map to
  `D3DLIGHT_SPOT` (hard: Theta≈Phi=2·coneAngle; soft: Theta=0, Phi=2·coneAngle).
  **Ambient RpLights are accumulated by color summation, not as D3D light objects.**
- `[LOCAL]` The gta-reversed header exposes the same knob:
  `RpD3D9AttenuationParams { constant; linear; quadratic; }` +
  `RpD3D9LightSetAttenuationParams` — `rpworld.h:1390-1395` (GTA:SA RVA 0x755D20; **not** a
  MASHED address).
- `[WEB-UNVERIFIED]` D3D9 semantics (MSDN light-types): directional lights ignore
  attenuation/range entirely; point lights require valid attenuation + Range.

---

## 3. Geometry flags

Identical across all witnesses — `[LOCAL]` `rpworld.h:716-750`, `[LOCAL]` librw
`rwobjects.h:558-574`, `[WEB-UNVERIFIED]` RW 3.7.0.2 `world/bageomet.h` and plugin-sdk
`rwcore.h` (same values):

```c
rpGEOMETRYTRISTRIP               = 0x00000001
rpGEOMETRYPOSITIONS              = 0x00000002
rpGEOMETRYTEXTURED               = 0x00000004
rpGEOMETRYPRELIT                 = 0x00000008   // per-vertex prelight colors present
rpGEOMETRYNORMALS                = 0x00000010   // vertex normals present
rpGEOMETRYLIGHT                  = 0x00000020   // geometry participates in runtime lighting
rpGEOMETRYMODULATEMATERIALCOLOR  = 0x00000040   // multiply by material RGBA
rpGEOMETRYTEXTURED2              = 0x00000080
rpGEOMETRYNATIVE                 = 0x01000000
rpGEOMETRYNATIVEINSTANCE         = 0x02000000
rpGEOMETRYFLAGSMASK              = 0x000000FF
rpGEOMETRYNATIVEFLAGSMASK        = 0x0F000000
```

- `[WEB-UNVERIFIED]` Modulate semantics per RW-3.6-era header docs: material color is
  multiplied with vertex colors in **both** the prelit and the lit paths (plugin-sdk `rwcore.h`).
- `[LOCAL]` Port already parses these: `mashedmod/src/mashed_re/Track/DffModel.cpp:186-187`
  (`lit = flags & 0x20`, `modmat = flags & 0x40`); prelit extraction :190-191, :360-361.
- `[LOCAL]` Mashed world BSP format word `0x4001004d` = TRISTRIP|TEXTURED|PRELIT|
  MODULATEMATERIALCOLOR with **no NORMALS and no LIGHT** — so the static world can never be
  runtime-lit per-vertex; it renders from baked prelight (documented in
  `TrackRenderer.cpp:344-387` comment block). Vehicles/props are the LIGHT|NORMALS atomics.

---

## 4. Surface properties & material → D3DMATERIAL9

### 4.1 Structs

`[LOCAL]` `RwSurfaceProperties` — `rwplcore.h:1492-1497`; identical field order in librw
`rwobjects.h:421-426`:

```c
struct RwSurfaceProperties {
    RwReal ambient;    // +0x00
    RwReal specular;   // +0x04   <-- SPECULAR IS THE MIDDLE FIELD (porting trap)
    RwReal diffuse;    // +0x08
};
```
SDK doc comment (`rwplcore.h:1486-1489`): coefficients 0.0..1.0; "currently the specular
element is not used."

`[LOCAL]` `RpMaterial` — `rpworld.h:196-204`: `texture +0x00, RwRGBA color +0x04,
pipeline +0x08, surfaceProps +0x0C (amb/spec/diff), refCount +0x18, pad +0x1A`.
Stream form `RpMaterialChunkInfo` — `rpworld.h:171-179`:
`{ RwInt32 flags; RwRGBA color; RwInt32 unused; RwBool textured; RwSurfaceProperties surfaceProps; }`.

### 4.2 librw fixed-function mapping (reference implementation)

`[WEB-CONFIRMED 2-0]` librw `setMaterial_fix` (`src/d3d/d3ddevice.cpp`) builds the
D3DMATERIAL9 by **modulating the RW material RGBA with the ambient and diffuse coefficients**
(colors are 0-255, normalized by /255.0f); **Specular and Emissive are forced to black,
Power = 0** — RW's specular surface property is NOT forwarded to fixed-function specular:

```c
float ambmult = surfProps.ambient/255.0f;
float diffmult = surfProps.diffuse/255.0f;
mat9.Ambient.r = color.red*ambmult;   /* .g .b likewise; .a = color.alpha */
mat9.Diffuse.r = color.red*diffmult;  /* ... */
mat9.Power = 0.0f; mat9.Emissive = black; mat9.Specular = black;
```

`[WEB-REFUTED 0-3]` — correction: an earlier extracted claim said the FF material is built
from constant white {255,255,255,255} with the real material color applied later via
texture-stage modulate. 3/3 refuters killed it; the confirmed `setMaterial_fix` quote above
is the correct account. A second claim (unconditional TSS-stage-1 MODULATE against a
D3DTA_CONSTANT holding material color) was likewise refuted 0-3.

`[WEB-CONFIRMED 2-0]` librw shader path uploads SurfaceProperties as one 4-float VS constant
in order `[0]=ambient, [1]=specular, [2]=diffuse, [3]=extraSurfProp`
(`d3ddevice.cpp`) — specular-before-diffuse again; second instance of the same trap.

`[WEB-UNVERIFIED]` The stock-RW D3D9 bridge is `RwD3D9SetSurfaceProperties(surfProps,
color, flags)` — takes exactly surface properties + material RGBA + geometry flags and
converts an RpMaterial to a D3DMATERIAL9 (plugin-sdk `rwcore.h`; `[LOCAL]` prototype also in
vendored `rwcore.h:3973` and thunk `rwcore.cpp:752`).

### 4.3 Prelight routing (librw FF path)

`[WEB-CONFIRMED 3-0]` Prelit vertex colors ride the **emissive** channel:
`D3DRS_EMISSIVEMATERIALSOURCE = D3DMCS_COLOR1` iff `Geometry::PRELIT`, else `D3DMCS_MATERIAL`;
`D3DRS_DIFFUSEMATERIALSOURCE = D3DMCS_COLOR1` only when the instanced mesh has vertex alpha,
else `D3DMCS_MATERIAL` (librw `src/d3d/d3d9render.cpp`).

---

## 5. Default D3D9 atomic pipeline — how lights are applied

### 5.1 librw (reference)

`[WEB-CONFIRMED 3-0]` In librw's FF default atomic pipeline (`defaultRenderCB_Fix`,
`src/d3d/d3d9render.cpp`), per-atomic lighting derives **solely from the geometry LIGHT
flag**:

```c
int lighting = !!(geo->flags & rw::Geometry::LIGHT);
if(lighting)
    d3d::lightingCB_Fix(atomic);          // uploads RpLights to the device
d3d::setRenderState(D3DRS_LIGHTING, lighting);
```
Caveat from the same verification: in current librw master this whole FF callback sits
inside a `/* */` block (lines 76-142); the active path is `defaultRenderCB_Shader`.

`[WEB-UNVERIFIED]` Stock RW manages FF lights by index via `RwD3D9SetLight` /
`RwD3D9EnableLight` (D3DLIGHT9-shaped descriptor + per-index enable) — i.e. the default
pipeline fills D3D light slots and toggles them (plugin-sdk `rwcore.h`).
`[LOCAL]` matching prototypes in vendored `rpworld.h:2482-2489`:
`_rwD3D9LightsGlobalEnable(flags)`, `_rwD3D9LightDirectionalEnable(light)`,
`_rwD3D9LightLocalEnable(light)`, `_rwD3D9LightsEnable(enable, type)` (GTA:SA RVAs, not MASHED).
Vertex-shader-path lighting constants (`[LOCAL]` `rpworld.h:45,104-108,2365-2371`):
WVP at const 0, `RWD3D9VSCONST_AMBIENT_OFFSET 4`, first light at const 5;
`_rpD3D9VertexShaderUpdateLightsColors(ptr, desc, ambientCoef, diffuseCoef)`.

### 5.2 The RW lighting equation (as reproduced by the port)

`[LOCAL]` Design comment `TrackRenderer.cpp:148-163` restates the `_rpWorldLight` atomic
model the port bakes on CPU:

```
LIGHT(0x20)+NORMALS:  colour = prelight + ambient + sunColour * max(0, N·L)
MODULATEMATERIALCOLOR(0x40):  colour *= materialRGBA / 255
PRELIT without LIGHT (glass/sea):  colour = prelight + ambient fill
```
Surface ambient/diffuse coefficients are 1.0 for the shipped Mashed materials, so they drop
out of the port's bake (same comment). If a vehicle material ever ships non-1.0
coefficients, the full form is `prelight + matColor·ambCoef·ambient +
matColor·diffCoef·Σ(light_i·max(0,N·L_i))` (§4.2 confirmed librw mapping).

### 5.3 MASHED.exe ground truth `[MASHED-RE]`

This is the decisive section for the port — **Mashed does NOT use D3D9 fixed-function
lights at all**:

- `D3DRS_LIGHTING = 0` always: `FUN_004d5480(0x89, 0)` called from render-state cold-init
  `FUN_004d5bc0` (0x004d5bc0); `D3DRS_AMBIENT` set to `0xffffffff` there
  (`re/analysis/render_6_c1_to_c2_s1/004d5bc0.md:41-45`,
  `re/analysis/render_lighting_alt/render_lighting_alt-20260503.md:16-24`).
  No `SetLight`/`LightEnable`/`D3DLIGHT9` documented anywhere in the RE notes.
- **RpLights exist but feed the RW pipeline's vertex-color path, not D3D lights.** Default
  lights are created inline in the course loader `FUN_00479330` (0x00479330), branch on
  `course_block[0x2640] == '\0'`. Re-decompiled + disassembled 2026-07-02 (Mashed_pool0,
  read-only) — instruction-level detail:
  - **No `Lights_Filename` (default branch, LAB_00479951):**
    - `PUSH 0x2` @ 0x00479951 → `RpLightCreate(2)` @ 0x0047996b; light stored at
      `course+0x105e0`; color set from `DAT_006132dc` (`PUSH 0x6132dc` @ 0x00479970,
      `FUN_004e4900` @ 0x0047997c); flags byte := 0x01 @ 0x00479987; `RpWorldAddLight`
      @ 0x00479999. **Frameless.**
    - `PUSH 0x1` @ 0x0047999e → `RpLightCreate(1)` @ 0x004799a0; light stored at
      `course+0x105e4`; color from `DAT_006132ec` (`PUSH 0x6132ec` @ 0x004799a5); flags
      := 0x01 @ 0x004799bc; **gets a frame**: `FUN_004c0b30()` @ 0x004799c0 (C1 note: RW
      alloc, object type 0x3000e), attached via `FUN_004c0740(light, frame)` @ 0x004799cd,
      then `FUN_004c1520(light->frame /*+0x04*/, &vec(1.0f,0,0), 0x42700000 /*60.0f*/, 0)`
      @ 0x004799e8 — call shape consistent with `RwFrameRotate(frame, axis, angle,
      combineOp)`; function unnamed in hooks.csv `[UNCERTAIN naming, mechanics literal]`.
      Then `RpWorldAddLight`.
  - **`Lights_Filename` set (DFF branch):** `FUN_0042a5d0` (RpClumpStreamRead) loads the
    DFF, `FUN_004e6650` (RpClumpGetNumLights) + `FUN_004b4010` collects them; per light:
    subtype byte (`light+0x01`) == 0x01 → stored at `course+0x105e4`; == 0x02 → stored at
    `course+0x105e0`; flags byte := 0x01; `RpWorldAddLight`.
  - **`Ambient_RGB` override (else-branch tail, 0x0047990b-0x0047994c):** if any of the
    three course-description floats at `param_3+0x76..0x78` exceeds `DAT_005d757c`, the
    light at `course+0x105e0` (creating it via `PUSH 0x2` @ 0x00479915 →
    `RpLightCreate(2)` @ 0x00479917 if absent) gets its color set from those floats
    (`FUN_004e4900` @ 0x00479944). **The Ambient_RGB config value targets the subtype-2
    light** — second, independent in-function confirmation that subtype 2 = AMBIENT.
- Config keys: `"Lights_Filename"` → handler `FUN_0047aaa0` (0x0047aaa0), copies to
  `course_block+0x2640` (table entry 0x00440d01); `"Ambient_RGB"` → 0x47aad0 (0x00440d10)
  (`render_lighting_d2-20260503.md:28-49,176-177`).
- **Specular is an env-map texture-stage path, not lights**: `FUN_00541b50` (0x00541b50)
  sets per-material camera-space reflection (`D3DTCI_CAMERASPACEREFLECTIONVECTOR`,
  0x30000, COUNT3) or normal (0x10000, COUNT2) texcoord generation; reflectivity comes from
  a per-material nibble read by `FUN_004cff00` (0x004cff00) —
  `*(byte*)(DAT_00911ae4+9+idx) & 0xf` — already C3 (diff GREEN 24/24, hooks.csv row 751)
  and ported as `Render/RpMaterialNibble_wf1.cpp`
  (`render_lighting_alt-20260503.md:25-107`).
- Custom no-light pipeline node string `nodeD3D9SubmitNoLight.csl` at 0x00618598 — geometry
  routed there bypasses runtime lighting (`render_lighting/render_lighting-20260502-2221.md:30-34`).
- Material push: `FUN_004d55b0` (0x004d55b0) sets emissive/diffuse floats, queues
  `D3DRS_AMBIENT/COLORVERTEX/SPECULARENABLE/LOCALVIEWER`, calls device `SetMaterial`
  (vtable+0xc4) — C2, hooks.csv row 1951.
- hooks.csv RpLight rows (all C1 `third-party-library[renderware]`): RpLightCreate
  0x004e4dd0, RpLightDestroy 0x004e4ec0, RpLightStreamRead 0x004e4c30, RpLightSetConeAngle
  0x004e4b90, RpWorldAddLight 0x004e4810, RpWorldRemoveLight 0x004e4860,
  RpClumpForAllLights 0x004e6760.

**Consequence:** Mashed's runtime lighting reaches pixels as (a) baked prelight in world
BSPs, (b) RpLight-driven per-vertex lighting of LIGHT|NORMALS atomics inside the RW
pipeline, (c) the specular env-map TSS path. §2's attenuation math is therefore
**not needed** unless point/spot lights ever show up in a LIGHTS.DFF — only ambient +
directional are documented in the assets and the default path.

---

## 6. Current port status (mashed_re) `[LOCAL]`

Lighting is fully implemented **CPU-side** (baked into vertex diffuse at batch build); no
stubs. `D3DRS_LIGHTING, FALSE` at every draw path (`ParticleSystem.cpp:285`,
`RwIm2DBridge.cpp:196`, `QuadRenderer.cpp:286`, `PickupField.cpp:223`,
`TrackRenderer.cpp:2942`) — matching MASHED's own FF-off behavior. No
SetLight/LightEnable/D3DMATERIAL9 anywhere in `mashedmod/src/mashed_re/`.

- `AtomicLight` struct + `MakeAtomicLight(ambient, sun_color, sun_dir)` —
  `TrackRenderer.cpp:164-188` (`L = normalize(-sun_dir)`).
- `LightAtomicVertex(...)` — `TrackRenderer.cpp:192-236`: one-sided N·L
  (`if (ndl<0) ndl=0`), optional `*= mat/255` for MODULATE.
- `BuildDffBatches(..., const AtomicLight*)` — `TrackRenderer.cpp:293-342`.
- LIGHTS.DFF parsers `ParseLightsDffAmbient` / `ParseLightsDffDirectional` —
  `TrackRenderer.cpp:344-387, 401-460+`; driven from `Load()` via `COURSE*.LUA`
  `Lights_Filename` (`TrackRenderer.cpp:674-725`). Arctic asset values (comment-documented):
  AMBIENT (0.2,0.3,0.3) + DIRECTIONAL (0.6,0.7,0.7), both flags=0x3; extracted dir
  colour (153,178,178), dir (0.577,-0.577,-0.577).
- State: `amb_world_`, `sun_color_`, `sun_dir_[3]` — `TrackRenderer.h:253-267`.
- Cars/copters are lit atomics through this path — `TrackRenderer.cpp:1328-1329, 1563,
  1589-1602, 1795`.
- s4 fix: world BSP prelit colours rendered AS-IS; track ambient deliberately NOT re-added
  at frame time (earlier double-count dragged the frame teal) — `TrackRenderer.cpp:743-763`,
  decisive evidence: original Arctic frame channel order B 29.8 < R 34.7 < G 34.8.

Not ported: the specular env-map TSS path (FUN_00541b50) — reflectivity nibble reader is
C3/ported, the stage setup itself is not wired in the standalone.

## 7. Cross-check vs `verify/ws_e_lighting/`

Five screenshots (BEFORE/AFTER × chase/standings + ORIGINAL_reference). Read 2026-07-01:

- BEFORE_chase: world crushed to near-black (unlit void, only emissive billboards visible).
  AFTER_chase: coherent night exposure, surface shading on tanks/buildings — the fix
  restored baked prelit rendering / ambient fill.
- BEFORE_standings: harsh teal cast + near-black road. AFTER_standings: teal reduced,
  mid-tones lifted — consistent with the s4 double-counted-ambient fix
  (`TrackRenderer.cpp:743-763`).
- ORIGINAL_reference is a **different scene** (desert daylight cockpit view): valid as a
  tone/exposure reference only, not a pixel-comparison target. AFTER frames match its key
  property (fully exposed environment, no crushed blacks).

These screenshots are consistent with, and do not contradict, any claim in §§1-6.

## 8. Open items / [UNCERTAIN]

1. **RESOLVED 2026-07-02 — sub-type numbering: MASHED agrees with the RW standard
   (1=DIRECTIONAL, 2=AMBIENT).** Re-decompiled + disassembled `FUN_00479330` in
   Mashed_pool0 (read-only). Three independent in-function witnesses:
   (a) the subtype-2 light (`RpLightCreate(2)` @ 0x0047996b) is **frameless** while the
   subtype-1 light (`RpLightCreate(1)` @ 0x004799a0) **gets a frame + a 60.0f rotation
   about (1,0,0)** — only a directional light needs orientation;
   (b) the `Ambient_RGB` course value is applied to the subtype-2 light
   (0x0047990b-0x0047994c);
   (c) the DFF branch stores subtype-1 lights in the same course slot (+0x105e4) as the
   framed default light and subtype-2 in the frameless one (+0x105e0).
   `render_lighting_d2-20260503.md:85-88,130` (1=AMBIENT/2=DIRECTIONAL) is **refuted**;
   correction appended to that note. The port's DFF parser mapping was already correct.
   Slot summary: `course+0x105e0` = ambient light, `course+0x105e4` = directional light.
2. **RESOLVED 2026-07-02 — default light colors dumped** (static image bytes at
   0x006132dc/0x006132ec, undefined data, read via memory_read):
   - `DAT_006132dc` (ambient, subtype 2): `3e800000 3e800000 3e800000 3f800000` =
     RGBA (0.25, 0.25, 0.25, 1.0)
   - `DAT_006132ec` (directional, subtype 1): `3f400000 3f400000 3f400000 3f800000` =
     RGBA (0.75, 0.75, 0.75, 1.0)
   Only used when a course has no `Lights_Filename`; default directional orientation =
   frame rotated 60.0f (0x42700000) about axis (1,0,0), combine op 0 (0x004799db-0x004799e8).
3. **Vehicle-specific surface properties** — §5.2 assumes coefficients 1.0 per the
   TrackRenderer comment (verified for track materials). Not yet confirmed for vehicle DFF
   materials; check the RpMaterialChunkInfo surfaceProps of a car DFF before assuming they
   drop out.
4. **Specular env-map path unported** — if vehicle visual parity ever fails on paint
   highlights, FUN_00541b50's TSS setup is the missing piece, not RpLights.
5. **NEW 2026-07-02 — latent DFF-flag-filter divergence in the port.** The port's LIGHTS.DFF
   parsers gate on stream flags: ambient requires `flags & 0x2`
   (`TrackRenderer.cpp:377`), directional requires `flags & 0x1` (`TrackRenderer.cpp:451`).
   The original's loader branch applies **no stream-flag filter** — it takes every clump
   light, keys on the subtype byte alone, and overwrites the runtime flags with 0x01
   (`FUN_00479330` DFF branch; disassembly 2026-07-02). [UNCERTAIN] whether the collector
   `FUN_004b4010` (C2) filters internally — not re-checked this session. No behavioral
   difference on shipped assets (Arctic lights carry flags 0x3, both filters pass), but a
   LIGHTS.DFF with other flag bits would diverge. Fix is trivial (drop the flag tests) if
   parity ever demands it.
6. 19 web claims remain formally unverified (session-limit outage). The load-bearing ones
   are all `[LOCAL]`-corroborated above; the only material claims resting solely on
   unverified web sources are the §2 attenuation formulas (irrelevant while Mashed uses
   only ambient+directional) and the re3 LIGHTATOMICS-only convention (informational).

## 9. Sources

Web (deep-research run wf_7e1055ed-84d, 2026-07-01):
- github.com/aap/librw — `src/d3d/d3d9render.cpp`, `src/d3d/d3ddevice.cpp`,
  `src/rwobjects.h`, `ARCHITECTURE.MD` (primary; also vendored locally at
  `re/prior_art/renderware/librw/`, commit 1252b90)
- github.com/sigmaco/rwsrc-v3.7.0.2 — leaked RW 3.7.0.2 source (`world/balight.h`,
  `world/bageomet.h`, `world/pipe/p2/d3d9/D3D9lights.c`) (primary, unverified tier)
- gitlab.eientei.org/mirrors/re3 — `src/fakerw/rpworld.h` (lcs branch);
  github.com/Jai-JAP/re-GTA — `src/rw/Lights.cpp` (primary)
- github.com/DK22Pac/plugin-sdk — `plugin_sa/game_sa/rw/rwcore.h` (primary)
- github.com/gta-reversed/gta-reversed-modern — `Vehicle.cpp`, `Renderer.cpp` (primary)
- learn.microsoft.com — D3D9 Light Types, D3DMATERIAL9 (primary)
- gtamods.com wiki — Geometry (RW Section), RpGeometry, Rendering with RenderWare (secondary)
- archive.org/details/renderwaregraphics3.7sdkandstudio2.01 — RW 3.7 SDK + docs (primary)

Local:
- `re/prior_art/renderware/gta-reversed-modern/source/game_sa/RenderWare/rw/{rpworld.h, rwplcore.h, rwcore.h}`
- `re/prior_art/renderware/librw/src/{rwobjects.h, d3d/d3d9render.cpp, d3d/d3ddevice.cpp}`
- `re/analysis/render_lighting/render_lighting-20260502-2221.md`
- `re/analysis/render_lighting_d2/render_lighting_d2-20260503.md`
- `re/analysis/render_lighting_alt/render_lighting_alt-20260503.md`
- `re/analysis/render_6_c1_to_c2_s1/004d5bc0.md`
- `mashedmod/src/mashed_re/D3d9Render/TrackRenderer.{cpp,h}`, `Track/DffModel.{cpp,h}`
- `verify/ws_e_lighting/*.png`
- `hooks.csv` rows 130, 746-748, 751, 753, 1951, 2277-2285, 2312
