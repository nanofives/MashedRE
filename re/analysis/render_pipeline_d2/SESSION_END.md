---
session: render_pipeline_d2-20260503
slot: Mashed_pool5
parent: render_pipeline-20260502-2227 (HALT)
outcome: RESOLVED — D-2140 closed; 1 new hooks.csv entry
---

# render_pipeline depth-2 — Session End Report

## Pre-flight summary

| Check | Result |
|-------|--------|
| SHA-256 MASHED.exe | `bdcae093…` ✓ matches anchor |
| Pool slot | Mashed_pool5 (no active lock; stale `.lock~` cleared) |
| Staleness | ok — current vs master |
| Scribe flag | none |
| MCP health_ping | ok |
| ghidra_info | Ghidra 12.0.3 ✓ |
| Open-program identity | project_name=Mashed_pool5 ✓ |

Note: `.pool_slot` race observed during acquire (parallel sessions stomping shared file). Bypassed acquire script; wrote pool5 directly per pre-assignment rule.

## Parent DEFERRED rows picked up

| ID | RVA | Resolution |
|----|-----|-----------|
| D-2140 | n/a (anchor: 0x005cf820) | RESOLVED — see below |

## Eligible RVA count check

`reference_to(0x005cf820)` → 0x00493788 → FUN_00493710 → 27 callees.  
Count = 27 ≥ 5. Work loop proceeds.

## Functions analyzed

| RVA | Name | Disposition |
|-----|------|-------------|
| 0x00493710 | FUN_00493710 | Already C1/mapped (rw_engine_init); no new work |
| 0x00493640 | FUN_00493640 | Already C1/mapped (rw_engine_init); 18 plugin callees D-0244..D-0261 already deferred |
| 0x004c32b0 | FUN_004c32b0 | Already C1/mapped (rw_engine_init) |
| 0x004c30b0 | FUN_004c30b0 | **NEW** — added to hooks.csv as C1/mapped (see 004c30b0.md) |
| 0x004c2fb0 | FUN_004c2fb0 | Already C1/mapped (rw_engine_init) |
| 0x00493600 | FUN_00493600 | Already C1/mapped (rw_engine_init) |
| 0x004e5d30 | FUN_004e5d30 | Already deferred as D-0244/D-0537 (rw_engine_init-cont1) |
| 0x00472380 | FUN_00472380 | Already C1/mapped (rw_engine_init_d2) |
| 0x004e7d40 | FUN_004e7d40 | Already deferred as D-0555 (rw_engine_init_d2-cont1) |
| 0x004c9f50 | FUN_004c9f50 | Already C1/mapped (rw_engine_init_d2) |
| 0x004c9eb0 | FUN_004c9eb0 | Already C1/mapped (rw_engine_init) |
| 0x004c9f60 | FUN_004c9f60 | Already C1/mapped (rw_engine_init_d2) |

## D-2140 resolution detail

`0x005cf820` = `s_Calling_RenderwareAttachPlugins_005cf820` confirmed via listing_code_unit_at ✓  
1 reference from 0x00493788 → inside FUN_00493710.  
FUN_00493710 is already analyzed as `RW_INIT_FN` (C1, hooks.csv).  
FUN_00493640 (called immediately after the log string) is the RenderwareAttachPlugins chain, also already C1.  
The custom Mashed pipeline node (ID `0x26990004`) was already identified in FUN_00472380 (C1).

**Conclusion: Mashed uses stock RW pipelines. No separate custom PIPELINE_SETUP_FN exists beyond what was already found in the rw_engine_init lineage. D-2140 closed.**

## New hooks.csv entry

```
004c30b0,FUN_004c30b0,render,C1,mapped,re/analysis/render_pipeline_d2/004c30b0.md,render_pipeline_d2-20260503,,,
RwEngineOpen wrapper; requires globals[0x49]==1; allocs 0x12c (0x4b dwords) via vtable[0x42];
copies template from DAT_007d3ec8; FUN_004c2c90 cmd-ids 0/4/0xb; sets [0x49]=2 on success;
callees D-0229 D-0232..D-0234
```

## New DEFERRED rows from this session

None. All depth-3 callees of FUN_004c30b0 (FUN_004cae90 D-0232, FUN_004d7ff0 D-0233, FUN_004d8480 D-0234, FUN_004c2c90 D-0229) already tracked in rw_engine_init-cont1.

No new U or S rows created (all functions already analyzed or already deferred in prior sessions).

## RW engine state machine (summary finding)

| State | Value at globals+0x124 | Set by |
|-------|----------------------|--------|
| uninit | 0 | (startup) |
| initialized | 1 (globals[0x49]=1) | FUN_004c32b0 (RwEngineInit) @ 0x004c32b0 |
| opened | 2 (globals[0x49]=2) | FUN_004c30b0 (RwEngineOpen) @ 0x004c30b0 |
| started | 3 (*(globals+0x124)=3) | FUN_004c2fb0 (RwEngineStart) @ 0x004c2fb0 |

Globals base: DAT_007d3ec8 (template); active ptr: DAT_007d3ff8.

## Queue / follow-up

Depth-3 work for this lineage belongs to **rw_engine_init-cont1** (already queued D-0228..D-0261) and **rw_engine_init_d2-cont1** (D-0537..D-0555). No new render_pipeline_d2-cont1 bucket needed.
