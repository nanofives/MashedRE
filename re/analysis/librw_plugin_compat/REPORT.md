---
bucket: librw_plugin_compat
session: librw_plugin_compat-20260507-1950
slot: Mashed_pool12
session_date: 2026-05-07
subsystem: render
parent: rw_engine_init/004c32b0 (C1 mapped)
librw_path: C:/Users/maria/Desktop/Proyectos/re-references/librw/
criterion_rw37_path: C:/Users/maria/Desktop/Proyectos/re-references/criterion-rwg37/core/src/plcore/batype.h
---

# librw Plugin Compatibility Matrix

## Context

`FUN_004c32b0` (0x004c32b0, rw_engine_init session, C1) calls `FUN_004d7de0`
(RwObjectRegisterPlugin equivalent) 14 times, all with `&DAT_00617fe0` as the
first argument. Each call registers a plugin slot (size, id, ctor, dtor, copy=0)
on what is the Engine-level object type.

`FUN_004d7de0` signature (verified in this session, 0x004d7de0, 514 bytes):
```
int FUN_004d7de0(int *param_1, int param_2, int param_3,
                 code *param_4, code *param_5, code *param_6)
```
param_1=object_type*, param_2=plugin_data_size, param_3=plugin_id,
param_4=ctor, param_5=dtor, param_6=copy.

All 14 calls pass copy=0 (null); FUN_004d7ff0 is substituted as a no-op for
null ctor/dtor/copy (verified in the decomp of FUN_004d7de0 at 0x004d7de0).

## Plugin ID namespace

All 14 IDs occupy the `rwVENDORID_CRITERIONINT` (vendor=4) range:
`MAKECHUNKID(4, sub_id)` = `(4 << 8) | sub_id`.
Canonical names from Criterion RW37 source
(`criterion-rwg37/core/src/plcore/batype.h` lines 243–277).

## Compat Matrix

| slot | id    | mashed_size (hex/dec) | ctor_rva   | dtor_rva   | rw_standard_name (Criterion RW37) | librw_present | librw_size (hex/dec) | gap_action |
|------|-------|-----------------------|------------|------------|----------------------------------|--------------|----------------------|------------|
| 1    | 0x40f | 0x8 / 8               | 0x004d8430 | 0x004d8470 | rwID_ERRORMODULE                 | no           | —                    | STUB_REQUIRED: 8-byte ErrorModule globals; decompile ctor to derive layout |
| 2    | 0x401 | 0x18 / 24             | 0x004c3e90 | 0x004c3e20 | rwID_VECTORMODULE                | no           | —                    | STUB_REQUIRED: 0x18-byte VectorModule globals; decompile ctor |
| 3    | 0x40d | 0x0 / 0               | 0x004d9030 | 0x004d9040 | rwID_COLORMODULE                 | no           | —                    | STUB_REQUIRED: size-0 slot; callbacks still run; decompile ctor/dtor |
| 4    | 0x402 | 0x18 / 24             | 0x004c4470 | 0x004c4430 | rwID_MATRIXMODULE                | no           | —                    | STUB_REQUIRED: 0x18-byte MatrixModule globals; decompile ctor |
| 5    | 0x403 | 0x4 / 4               | 0x004c07b0 | 0x004c0830 | rwID_FRAMEMODULE (ID_FRAMEMODULE) | YES         | 0x0 / 0              | SIZE_MISMATCH: librw=0, Mashed=4 — [UNCERTAIN U-2888]; librw registers no data; Mashed does; decompile ctor |
| 6    | 0x404 | 0x4 / 4               | 0x004cbca0 | 0x004cbd00 | rwID_STREAMMODULE                | no           | —                    | STUB_REQUIRED: 4-byte StreamModule globals; decompile ctor |
| 7    | 0x405 | 0x4 / 4               | 0x004c1980 | 0x004c1940 | rwID_CAMERAMODULE                | no           | —                    | STUB_REQUIRED: 4-byte CameraModule globals; decompile ctor |
| 8    | 0x406 | 0x220 / 544           | 0x004cd900 | 0x004cdb60 | rwID_IMAGEMODULE (ID_IMAGEMODULE) | YES         | 0x84 / 132           | SIZE_MISMATCH: librw=132, Mashed=544 — [UNCERTAIN U-2889]; Mashed has 4× the data; decompile ctor |
| 9    | 0x407 | 0x64 / 100            | 0x004c78e0 | 0x004c78a0 | rwID_RASTERMODULE (ID_RASTERMODULE) | YES       | 0x84 / 132           | SIZE_MISMATCH: librw=132, Mashed=100 — [UNCERTAIN U-2890]; Mashed is SMALLER (fewer stack entries?); decompile ctor |
| 10   | 0x408 | 0x34 / 52             | 0x004c5f60 | 0x004c5e00 | rwID_TEXTUREMODULE (ID_TEXTUREMODULE) | YES     | 0x28 / 40            | SIZE_MISMATCH: librw=40, Mashed=52 — [UNCERTAIN U-2891]; 12 extra bytes; decompile ctor |
| 11   | 0x409 | 0x60 / 96             | 0x004d8530 | 0x004d8550 | rwID_PIPEMODULE                  | no           | —                    | STUB_REQUIRED: 0x60-byte PipeModule globals; decompile ctor |
| 12   | 0x412 | 0x4 / 4               | 0x004d8fa0 | 0x004d9000 | rwID_CHUNKGROUPMODULE            | no           | —                    | STUB_REQUIRED: 4-byte ChunkGroupModule globals; decompile ctor |
| 13   | 0x40a | 0x74 / 116            | 0x004cd850 | 0x004cd810 | rwID_IMMEDIATEMODULE             | no           | —                    | STUB_REQUIRED: 0x74-byte ImmediateModule globals; decompile ctor |
| 14   | 0x40b | 0x28 / 40             | 0x004d8a80 | 0x004d8b70 | rwID_RESOURCESMODULE             | no           | —                    | STUB_REQUIRED: 0x28-byte ResourcesModule globals; decompile ctor |

### librw size derivation (32-bit, from librw source)

**ID_FRAMEMODULE** (`frame.cpp:25`): `Engine::registerPlugin(0, ID_FRAMEMODULE, ...)` → librw size = 0.

**ID_IMAGEMODULE** (`image.cpp:1065`): `Engine::registerPlugin(sizeof(ImageGlobals), ...)`.
`ImageGlobals` = `{char*(4) + int(4) + FileAssociation[10](120) + int(4)}` = **132 bytes**.
(`FileAssociation` = `{char*(4) + fnptr(4) + fnptr(4)}` = 12 bytes; ×10 = 120.)

**ID_RASTERMODULE** (`raster.cpp:54`): `Engine::registerPlugin(sizeof(RasterGlobals), ...)`.
`RasterGlobals` = `{int32(4) + Raster*[32](128)}` = **132 bytes**.

**ID_TEXTUREMODULE** (`texture.cpp:86`): `Engine::registerPlugin(sizeof(TextureGlobals), ...)`.
`TextureGlobals` = `{ptr(4)+ptr(4)+bool32(4)+bool32(4)+bool32(4)+bool32(4)+LinkList(8)+LinkList(8)}` = **40 bytes**.
(`LinkList` contains one `LLLink` = two pointers = 8 bytes.)

### Callback address notes

All 28 callback addresses verified via `listing_code_unit_at` in this session (all non-null).
None are recognized as `FUN_` entry points by Ghidra — all appear as `LAB_` labels. [UNCERTAIN U-2887]
All 28 are confirmed valid code addresses. Decompilation of each callback deferred to
`librw_plugin_compat-cont1` (D-8560).

Observed first-instruction patterns:
- **ctor pattern A** (MOV EAX/EDX/ECX, dword ptr [ESP+0x8]): slots 0x401, 0x402, 0x403, 0x405, 0x406, 0x407, 0x408, 0x40a, 0x40b — reads the plugin data base pointer from stack arg (RW ctor convention: `(obj_base, offset, size)`).
- **ctor pattern B** (MOV ECX/EAX, global): slots 0x40f, 0x404, 0x409, 0x412 — loads a module-global first.
- **ctor 0x40d**: begins `MOV EAX,[0x007d6c94]` — reads global state pointer before any arg.
- **dtor pattern** (MOV EAX/EDX, [0x007d3ff8] or module-global): consistent across all 14; reads engine state pointer as first action.

## Decision Summary

| Metric | Count |
|--------|-------|
| Total registered plugins | 14 |
| Standard RW module IDs (Criterion RW37) | 14 of 14 |
| Present in librw | 4 of 14 (FRAMEMODULE, IMAGEMODULE, RASTERMODULE, TEXTUREMODULE) |
| Clean size match (librw == Mashed) | **0 of 4** |
| librw absent entirely | 10 of 14 |

### Recommendation: librw-as-substitute NOT viable as-is

**Rationale:**
1. All 14 module slots use `rwVENDORID_CRITERIONINT` IDs that are engine-internal
   module registrations, not toolkit plugins. These represent engine subsystem globals
   attached to the Engine object instance.
2. librw only covers 4 of 14: Frame, Image, Raster, Texture — and all 4 have different
   struct sizes from Mashed's registrations. The structs would need to be rederived from
   Mashed's ctor callbacks before any of them can be used.
3. The other 10 (Vector, Matrix, Stream, Camera, Pipe, Immediate, Resources, Color,
   Error, ChunkGroup) are absent from librw entirely and require full implementation.

### Stubs librw would need on the Mashed side

Each of the following requires a Mashed-side struct + ctor/dtor implementation before
librw can substitute for the RW engine. All are gated on `librw_plugin_compat-cont1`
(D-8560) which decompiles the callback bodies to derive struct layouts:

| Stub ID | RW module name | Mashed data size | Priority |
|---------|---------------|-----------------|----------|
| — | rwID_VECTORMODULE (0x401) | 0x18 | HIGH (basic math module) |
| — | rwID_MATRIXMODULE (0x402) | 0x18 | HIGH (basic math module) |
| — | rwID_FRAMEMODULE (0x403) | 0x4 (librw=0) | MEDIUM (size gap small) |
| — | rwID_STREAMMODULE (0x404) | 0x4 | MEDIUM |
| — | rwID_CAMERAMODULE (0x405) | 0x4 | MEDIUM |
| — | rwID_IMAGEMODULE (0x406) | 0x220 (librw=0x84) | HIGH (large struct gap) |
| — | rwID_RASTERMODULE (0x407) | 0x64 (librw=0x84) | HIGH (librw LARGER than Mashed) |
| — | rwID_TEXTUREMODULE (0x408) | 0x34 (librw=0x28) | MEDIUM |
| — | rwID_PIPEMODULE (0x409) | 0x60 | LOW (pipeline) |
| — | rwID_IMMEDIATEMODULE (0x40a) | 0x74 | LOW |
| — | rwID_RESOURCESMODULE (0x40b) | 0x28 | LOW |
| — | rwID_COLORMODULE (0x40d) | 0x0 | LOW (no data, callbacks only) |
| — | rwID_ERRORMODULE (0x40f) | 0x8 | MEDIUM |
| — | rwID_CHUNKGROUPMODULE (0x412) | 0x4 | LOW |

**Earliest unlock condition:** decompile the 28 callback bodies (D-8560, bucket
`librw_plugin_compat-cont1`) to derive all 14 module struct layouts, then compare
field-by-field against librw's 4 partial matches and the Criterion RW37 source.
Only after struct layouts are confirmed can the librw integration decision be finalized.

## Uncertainties Filed

- [UNCERTAIN U-2887] All 28 plugin callback addresses are LAB_ labels in Ghidra; none recognized as FUN_ entry points. Function boundaries unknown.
- [UNCERTAIN U-2888] 0x403 rwID_FRAMEMODULE: Mashed registers 4 bytes; librw registers 0. Extra 4 bytes may be version-specific (RW 3.5/3.6 vs 3.7) or Mashed-specific extension.
- [UNCERTAIN U-2889] 0x406 rwID_IMAGEMODULE: Mashed registers 544 bytes; librw ImageGlobals is 132 bytes. Mashed has 412 extra bytes — extent unknown without ctor decompilation.
- [UNCERTAIN U-2890] 0x407 rwID_RASTERMODULE: Mashed registers 100 bytes; librw RasterGlobals is 132 bytes. Mashed is SMALLER than librw — no 32-entry raster stack, or fewer fields.
- [UNCERTAIN U-2891] 0x408 rwID_TEXTUREMODULE: Mashed registers 52 bytes; librw TextureGlobals is 40 bytes. Mashed has 12 extra bytes.

## Deferred

- D-8560: Decompile all 28 callback functions (LAB_ addresses, 14 ctor + 14 dtor) to derive Mashed module struct layouts. Pick up as bucket `librw_plugin_compat-cont1`; same depth; no further recursion.
