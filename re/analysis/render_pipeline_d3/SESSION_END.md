---
session: render_pipeline_d3-20260506-0505
slot: Mashed_pool10
parent: render_pipeline_d2-20260503
outcome: COMPLETE — D-4363 + D-4364 resolved; 10 new hooks.csv entries; 3 new U rows
---

# render_pipeline depth-3 — Session End Report

## Pre-flight summary

| Check | Result |
|-------|--------|
| SHA-256 MASHED.exe | `bdcae093…` ✓ matches anchor |
| Pool slot | Mashed_pool10 (no active lock; created fresh lock file) |
| MCP health_ping | ok |
| ghidra_info | Ghidra 12.0.3 ✓ |
| Open-program identity | project_name=Mashed_pool10 read_only=true ✓ |

## Parent DEFERRED rows picked up

| ID | RVA | Resolution |
|----|-----|-----------|
| D-4363 | 0x00552d10 0x00552df0 0x00552da0 0x00552e40 (quad) | RESOLVED — all 4 analyzed as C1; see notes below |
| D-4364 | 0x004c4a50 FUN_004c4a50 | RESOLVED — analyzed as C1; Rodrigues' rotation matrix worker |

## Functions analyzed

| RVA | Assigned name | Role | Disposition |
|-----|--------------|------|-------------|
| 0x004c4a50 | FUN_004c4a50 | RwMatrixRotate (axis-angle worker; Rodrigues') | NEW C1 — D-4364 resolved |
| 0x00552d10 | FUN_00552d10 | Im2D camera-matrix stack push | NEW C1 — D-4363 [1/4] + S-0445 + S-1480[1/4] |
| 0x00552da0 | FUN_00552da0 | Im2D scale setter | NEW C1 — D-4363 [2/4] + S-1480[2/4] |
| 0x00552df0 | FUN_00552df0 | Im2D offset/position setter | NEW C1 — D-4363 [3/4] + S-1480[3/4] |
| 0x00552e40 | FUN_00552e40 | Im2D view-projection matrix getter | NEW C1 — D-4363 [4/4] + S-1480[4/4] |
| 0x004c5010 | FUN_004c5010 | RwMatrixScale | NEW C1 — depth-4 callee |
| 0x004c51a0 | FUN_004c51a0 | RwMatrixTranslate | NEW C1 — depth-4 callee |
| 0x004c4600 | FUN_004c4600 | RwMatrixMultiply | NEW C1 — depth-4 callee |
| 0x004c4dc0 | FUN_004c4dc0 | RwMatrixInvert (dispatch) | NEW C1 — depth-4 callee |
| 0x004c4eb0 | FUN_004c4eb0 | RwMatrixInvert (general cofactor path) | NEW C1 — depth-5 callee |

## Stubs resolved by this session

| S-ID | Callee | Resolved by |
|------|--------|------------|
| S-0445 | 0x00552d10 FUN_00552d10 | analyzed this session |
| S-1480 [1/4] | 0x00552d10 | analyzed this session |
| S-1480 [2/4] | 0x00552da0 | analyzed this session |
| S-1480 [3/4] | 0x00552df0 | analyzed this session |
| S-1480 [4/4] | 0x00552e40 | analyzed this session |
| S-1483 | 0x004c4a50 FUN_004c4a50 | analyzed this session |

## New hooks.csv entries (scribe plate)

```
004c4a50,FUN_004c4a50,render,C1,new,re/analysis/render_pipeline_d3/004c4a50.md,render_pipeline_d3-20260506-0505,,,Rodrigues' rotation-matrix worker; params: (RwMatrix* dst, RwV3d* axis, float 1-cos, float sin, int combineOp); builds 4x4 in local; combine 0=set 1=right-mul 2=left-mul; error 0x80000003 on invalid op; resolves D-4364 S-1483
00552d10,FUN_00552d10,render,C1,new,re/analysis/render_pipeline_d3/00552d10.md,render_pipeline_d3-20260506-0505,,,Im2D camera-matrix stack push; depth counter DAT_00912b04 max 0x1f; alloc-on-demand via FUN_004c57a0; copies 16 dwords from source slot; returns false if full; resolves D-4363[1] S-0445 S-1480[1]; U-2047
00552da0,FUN_00552da0,render,C1,new,re/analysis/render_pipeline_d3/00552da0.md,render_pipeline_d3-20260506-0505,,,Im2D scale setter; builds {p1,p2,1.0f} 3-float; calls FUN_004c5010(slot,&struct,1); clears DAT_00912bd8+bec; resolves D-4363[2] S-1480[2]
00552df0,FUN_00552df0,render,C1,new,re/analysis/render_pipeline_d3/00552df0.md,render_pipeline_d3-20260506-0505,,,Im2D offset setter; builds {p1,p2,0.0f}; calls FUN_004c51a0(slot,&struct,1); clears dirty flags; resolves D-4363[3] S-1480[3]
00552e40,FUN_00552e40,render,C1,new,re/analysis/render_pipeline_d3/00552e40.md,render_pipeline_d3-20260506-0505,,,Im2D view-projection getter; lazy cache DAT_00912bd8; builds proj from stack-slot+FUN_004c4600/5010/51a0/4dc0; inverts to DAT_00912b58; always multiplies by camera LTM via FUN_004c0ed0; returns &DAT_00912b98; resolves D-4363[4] S-1480[4]; U-2048 U-2049 U-2050
004c5010,FUN_004c5010,render,C1,new,re/analysis/render_pipeline_d3/004c5010.md,render_pipeline_d3-20260506-0505,,,RwMatrixScale; 3 combine types: 0=set-diagonal 1=right-row-scale 2=col-scale; clears flags 0x20003 (identity+ortho bits); error 0x80000003 on invalid op
004c51a0,FUN_004c51a0,render,C1,new,re/analysis/render_pipeline_d3/004c51a0.md,render_pipeline_d3-20260506-0505,,,RwMatrixTranslate; 3 combine types: 0=set 1=rotate-then-add 2=direct-add; clears bit 17 (identity) only; error 0x80000003 on invalid op
004c4600,FUN_004c4600,render,C1,new,re/analysis/render_pipeline_d3/004c4600.md,render_pipeline_d3-20260506-0505,,,RwMatrixMultiply(dst,matA,matB); identity fast-path checks bit 0x20000 on each src; vtable dispatch for general case; propagates combined flags to dst[3]
004c4dc0,FUN_004c4dc0,render,C1,new,re/analysis/render_pipeline_d3/004c4dc0.md,render_pipeline_d3-20260506-0505,,,RwMatrixInvert dispatch; path1=identity(copy); path2=orthonormal(transpose+neg-translate); path3=general(FUN_004c4eb0); callee D-4363-related
004c4eb0,FUN_004c4eb0,render,C1,new,re/analysis/render_pipeline_d3/004c4eb0.md,render_pipeline_d3-20260506-0505,,,RwMatrixInvert general (cofactor expansion); computes 3x3 cofactors+det; guards div-by-zero (det=0→scale=1.0); negates rotated translation; no further callees
```

## New UNCERTAINTIES rows (scribe plate)

```
| U-2047 | semantic | render_pipeline_d3: FUN_00552d10 DAT_00912a80 vs 00912a84 | Two globals differ by 4 bytes at base; Ghidra may have split a single pointer-array; two-pointer scheme purpose unknown | reference_to 0x00912a80 + reference_to 0x00912a84 | reference_to | none |
| U-2048 | semantic | render_pipeline_d3: FUN_00552e40 DAT_00912c0c struct | Field +0x14 used as mode discriminant (==2 → override fVar7=1.0); field +0x4 used as camera-object pointer; struct type unknown | reference_to 0x00912c0c | reference_to | none |
| U-2049 | semantic | render_pipeline_d3: FUN_00552e40 DAT_00912b0c+b08 | Two floats used as viewport-dimension inputs for fov terms; who writes them and from what source is unknown | reference_to 0x00912b0c + 0x00912b08 | reference_to | none |
| U-2050 | value | render_pipeline_d3: _DAT_005cc32c at 0x005cc32c | Float constant used in Im2D projection fov scaling term; value and semantic unknown | memory_read 0x005cc32c 4 | memory_read | none |
```

## New STUBS rows (scribe plate)

None — all callees of newly-analyzed functions are either already-mapped, analyzed this session, or external RW vtable (excluded per session rule).

## New DEFERRED rows (D=6040..6099)

None — all depth-4 / depth-5 callees analyzed this session. No render_pipeline_d3-cont1 bucket needed.

## DEFERRED rows closed

| ID | Closed |
|----|--------|
| D-4363 | RESOLVED — all 4 Im2D setter/getter functions analyzed C1 |
| D-4364 | RESOLVED — FUN_004c4a50 analyzed C1 |

## RW matrix operations found — subsystem summary

This session identified the following RwMatrix API surface directly in the MASHED.exe binary (statically linked RW):

| RVA | RW API equivalent |
|-----|------------------|
| 0x004c4a50 | `RwMatrixRotate` (worker receiving 1-cos/sin) |
| 0x004c5010 | `RwMatrixScale` |
| 0x004c51a0 | `RwMatrixTranslate` |
| 0x004c4600 | `RwMatrixMultiply` |
| 0x004c4dc0 | `RwMatrixInvert` (dispatcher) |
| 0x004c4eb0 | `RwMatrixInvert` (general cofactor fallback) |

Im2D viewport management lives at 0x00912a80–0x00912c10 (stack + cached matrices + dirty flags).

## Queue / follow-up

No follow-up bucket needed for this lineage.

Outstanding uncertainties U-2047..U-2050 are resolvable via `reference_to` / `memory_read` in a future session.
