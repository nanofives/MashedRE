# Session End Report — render_frame_tree_d2-20260503-0700

**Date:** 2026-05-03  
**Slot:** Mashed_pool12 (pre-assigned pool3 was hard-locked by concurrent session; pool12 acquired; identity verified via program_list_open)  
**Parent bucket:** render_frame_tree (D-2080)  
**SHA-256 anchor:** BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

## Work completed

### D-2080 resolved — FRAME_UPDATE_FN identified

**FRAME_UPDATE_FN = `FUN_004d80d0` at `0x004d80d0`** (`_rwFrameSyncDirty`)

Trace: `FUN_004c1a00` (BeginUpdate wrapper) → `*(camera+0x18)` (function pointer) → concrete BeginUpdate body at `LAB_004c2bb0` (0x004c2bb0) → `CALL 0x004d80d0` at 0x004c2bbc.

The camera+0x18 function pointer is written in `FUN_004c1d30` (RwCameraCreate): `*(camera+0x18) = &LAB_004c2bb0`.

Prior session MM failed because it searched for RwFrame* symbols/imports. These are statically linked with no debug info. Structural trace through camera-creation → concrete BeginUpdate → frame-sync call was the successful path.

### Functions mapped — 4 new C1 hooks

| RVA | Name | Notes |
|-----|------|-------|
| `0x004d80d0` | FUN_004d80d0 | FRAME_UPDATE_FN (_rwFrameSyncDirty); dirty-list walker; rwGlobals+0xbc sentinel |
| `0x004d8280` | FUN_004d8280 | child-hierarchy sync with parent-dirty propagation (_rwHierarchySyncDirtyWithParent) |
| `0x004d8300` | FUN_004d8300 | child-hierarchy sync, no-parent path (_rwHierarchySyncDirty) |
| `0x004c0e50` | FUN_004c0e50 | rwFrameMarkDirty; inserts root into dirty list at rwGlobals+0xbc |

### Depth-3 DEFERRED filed — D-3460 (1 entry)

| D-ID | RVA | Name | Reason |
|------|-----|------|--------|
| D-3460 | `0x004c4600` | FUN_004c4600 | RwMatrixMultiply wrapper; identity optimizations; actual multiply via vtable at rwGlobals+DAT_007d4028+8; pick up as bucket matrix_math |

### Additional structural findings (not hooked — context only)

These were discovered during tracing but are not in the hook target scope of this session:

- **Concrete BeginUpdate** at `0x004c2bb0` (LAB_) — sets `*rwGlobals = camera`, calls FRAME_UPDATE_FN at `0x004c2bbc`, then dispatches `*(rwGlobals+0x4c)(0, camera, 0)`. Ghidra has no function object at `0x004c2bb0` (only LAB_ label) — needs master write session to create function.
- **Concrete EndUpdate** at `0x004c2b80` (LAB_) — dispatches `*(rwGlobals+0x70)(0, camera, 0)`, then zeroes `*rwGlobals`.
- **`FUN_004c1d30`** (`RwCameraCreate`) — allocates camera; writes function pointers at camera+0x10/+0x18/+0x1c; fills projection params; links into `DAT_00617f98` global list.
- **`FUN_004c0740`** (`RwObjectHasFrameSetFrame`) — unlinks camera from old frame's objectList at camera+8/+0xc, writes new frame at camera+4, inserts into new frame's objectList at frame+0x90, calls `FUN_004c0e50`.
- **`FUN_004670a0`** — primary camera factory: `RwCameraCreate` → `FUN_004c0b30` (frame create) → `FUN_004c0740` (set frame) → `FUN_004c77c0` ×2 (rasters); stores to `DAT_006905b0`.

### RwFrame struct layout confirmed (from FUN_004d80d0 access patterns)

| Offset | Field |
|--------|-------|
| +0x00 | RwObject.type |
| +0x03 | RwObject.privateFlags (bits: 0=in dirty list, 1=hierarchy dirty, 2=modelling dirty, 3=child hierarchy) |
| +0x04 | RwObject.parent (RwFrame*) |
| +0x08 | inDirtyList.next (RwLLLink) |
| +0x0c | inDirtyList.prev |
| +0x10 | modelling (RwMatrix, 64 bytes) |
| +0x50 | ltm (RwMatrix, 64 bytes) |
| +0x90 | objectList sentinel (RwLLLink, 8 bytes) |
| +0x98 | firstChild (RwFrame*) |
| +0x9c | nextSibling (RwFrame*) |
| +0xa0 | root pointer [UNCERTAIN U-1187] |

### rwGlobals offsets confirmed (additions to prior sessions)

| Offset | Role |
|--------|------|
| +0x00 | current camera (BeginUpdate stores, EndUpdate clears) |
| +0xbc | dirty frame list sentinel next |
| +0xc0 | dirty frame list sentinel prev |
| +0x4c | render dispatch fn ptr (BeginUpdate concrete) |
| +0x70 | render dispatch fn ptr (EndUpdate concrete) |

## Tracker mutations

- **hooks.csv:** +4 C1 rows (004d80d0, 004d8280, 004d8300, 004c0e50) + 1 C0 deferred row (004c4600)
- **DEFERRED.md:** D-2080 → Cleared (FRAME_UPDATE_FN found); D-3460 added (FUN_004c4600, matrix_math bucket)
- **UNCERTAINTIES.md:** U-1187, U-1188, U-1189 added (see below)
- **STUBS.md:** no new entries
- **CHANGELOG.md:** one batch entry

## New UNCERTAINTIES

| ID | Type | Statement | Path |
|----|------|-----------|------|
| U-1187 | structural | `frame+0xa0` interpreted as root pointer from `FUN_004c0e50` access pattern; not confirmed via explicit struct type annotation | Cross-check with librw `RwFrameGetRoot` return to see if it reads from frame+0xa0 or a different offset |
| U-1188 | structural | Concrete BeginUpdate at `0x004c2bb0` has no Ghidra function object (LAB_ only); referenced via data pointer in FUN_004c1d30; bounds and calling convention unconfirmed | Master write session: `listing_disassemble_seed + function_create` at 0x004c2bb0 |
| U-1189 | structural | `DAT_007d4028` in rwGlobals — used as plugin-data sub-offset in `FUN_004c4600` (matrix flags table at rwGlobals+DAT_007d4028+4 and vtable at +8); full role unknown | Decomp `FUN_004c4600`'s vtable target once matrix_math bucket opens |

## Notes

- Pool3 was hard-locked at session start; pool12 used instead. Identity confirmed via `program_list_open` → project_name = Mashed_pool12. ✓
- `FUN_004c4600` (matrix multiply) — NOT recursed per session rules ("RwMatrix utility callees → DEFERRED, no recurse").
- The object sync-callback dispatch in `FUN_004d80d0` (`(*(code*)puVar6[2])(puVar6-2)`) is fully dynamic; concrete callbacks per object type are handled in their own subsystem sessions (e.g., camera sync at `LAB_004c1de0`).
- `FUN_004d8280` and `FUN_004d8300` are self-recursive (child traversal); no additional depth-3 DEFERRED rows needed beyond D-3460.
