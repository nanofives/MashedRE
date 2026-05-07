# SESSION_END — rw_engine_init_d3

**Session short ID:** rw_engine_init_d3-20260503  
**Slot used:** Mashed_pool15 (pre-assigned Mashed_pool3 was locked by another session)  
**Date:** 2026-05-03  
**Parent session:** rw_engine_init_d2 (session L, batch 2)

---

## Slot note
Mashed_pool3 (pre-assigned) had a stale `.lock~` held by a live process — LockException on open. Fell back to Mashed_pool15 (completely clean). Preflight passed on pool15.

---

## Functions covered — 19 total

| RVA | Name | Size | Depth | Notes |
|---|---|---|---|---|
| 0x004c2d90 | FUN_004c2d90 | 35 | d3 | Plugin registry wrapper → FUN_004d7de0; DAT_00617fe0; S-0200 |
| 0x004e7d40 | FUN_004e7d40 | 38 | d3 | Plugin registry wrapper → FUN_004d7de0; DAT_00618664; S-0201 |
| 0x004a42c5 | FUN_004a42c5 | 86 | d3 | vsprintf(buf, fmt, va_list) via stack-FILE; S-0260 S-0820 |
| 0x004a2b60 | FUN_004a2b60 | 87 | d3 | sprintf(buf, fmt, ...) variadic via stack-FILE |
| 0x004ad1e0 | FUN_004ad1e0 | 8  | d3 | Fatal abort: __amsg_exit(2) |
| 0x00498a00 | FUN_00498a00 | 348 | d3 | Display mode name formatter; U-1667; S-0204 S-0822 |
| 0x00498c00 | FUN_00498c00 | 275 | d3 | D3D subsystem+mode enumeration init; 3 alloc arrays; S-0205 S-0821 |
| 0x00498e40 | FUN_00498e40 | 85 | d3 | Default mode finder (first ≥640×480, flags≠0); S-0206 S-0823 |
| 0x00498ea0 | FUN_00498ea0 | 182 | d3 | Snapshot renderer selection to globals; S-0207 S-0824 |
| 0x004c2e70 | FUN_004c2e70 | 35 | d3 | SetSubSystem: FUN_004c2c90 cmd 0x10; S-0208 S-0825 |
| 0x004cfa00 | FUN_004cfa00 | 1990 | d3 | RW/D3D pixel-format cap table init; plugin ID 0x40c; U-1669; S-0215 |
| 0x004c7690 | FUN_004c7690 | 38 | d3 | Plugin registry wrapper → FUN_004d7de0; DAT_00618180; S-0216 |
| 0x004d7de0 | FUN_004d7de0 | 258 | d4 inline | RwPluginRegistryAddPlugin core; U-1668 |
| 0x004c2d70 | FUN_004c2d70 | 5  | d4 inline | Getter DAT_007d3ff4 (RW plugin state guard) |
| 0x004c2de0 | FUN_004c2de0 | 42 | d4 inline | GetNumSubSystems: cmd 0xd |
| 0x004c2e10 | FUN_004c2e10 | 42 | d4 inline | GetSubSystemInfo: cmd 0xe; fills 0x50-byte struct |
| 0x004c2e40 | FUN_004c2e40 | 44 | d4 inline | GetCurrentSubSystem: cmd 0xf |
| 0x004c2ea0 | FUN_004c2ea0 | 44 | d4 inline | GetNumVideoModes: cmd 5 |
| 0x004c2f30 | FUN_004c2f30 | 35 | d4 inline | UseVideoMode: cmd 7 |

---

## D3D vtable command map (assembled from this + d2 sessions)

All go through FUN_004c2c90(DAT_007d3ff8+0x10, cmd, ...):

| Command | Function | Role |
|---|---|---|
| 5  | FUN_004c2ea0 | GetNumVideoModes |
| 6  | FUN_004c2f00 (d2) | GetVideoMode (returns mode index short) |
| 7  | FUN_004c2f30 | UseVideoMode |
| 0xd | FUN_004c2de0 | GetNumSubSystems |
| 0xe | FUN_004c2e10 | GetSubSystemInfo |
| 0xf | FUN_004c2e40 | GetCurrentSubSystem |
| 0x10 | FUN_004c2e70 | SetSubSystem |

---

## Tracker counts
- **hooks.csv**: +19 rows (lines 832–850)
- **STUBS.md**: removed S-0200, S-0201, S-0203, S-0204, S-0205, S-0206, S-0207, S-0208, S-0215, S-0216, S-0260, S-0820(004a42c5), S-0821, S-0822, S-0823, S-0824, S-0825 — 17 rows removed
- **UNCERTAINTIES.md**: +3 entries (U-1667, U-1668, U-1669)
- **DEFERRED.md**: 0 new entries (all depth-4 absorbed inline; D=4900..4959 range unused)

---

## IDs used
- U: 1667, 1668, 1669 (of allocated range U=1667..1686)
- S: none new (of allocated range S=1660..1679)
- D: none (of allocated range D=4900..4959)
- cap_count: 0

---

## MCP failures
None.

---

## Observations
1. Three plugin registry wrapper functions (004c2d90, 004e7d40, 004c7690) all delegate to FUN_004d7de0 with different global addresses (DAT_00617fe0, DAT_00618664, DAT_00618180). These are three distinct RW plugin registries.
2. FUN_004d7de0 is RwPluginRegistryAddPlugin — a core RW plugin system function. Plugin node is 15 DWORDs with doubly-linked list and back-ptr to registry.
3. FUN_004cfa00 fills a 512-byte static table at 0x00911b00 with RW raster format metadata. Format tokens match RW 3.x rwRasterFormat enum values exactly.
4. The D3D vtable command space at FUN_004c2c90 now has 7 known command IDs (5, 6, 7, 0xd, 0xe, 0xf, 0x10).
5. FUN_00498c00 is the video-mode enumeration initializer: allocates three parallel arrays for subsystem-info (0x50 bytes each), mode-counts (4 bytes each), and mode-structs (0x18 bytes each).
6. S-0203 (for 004989b0) was already in hooks.csv as CONFIG_SAVE_FN (subsystem=save) — cleared from STUBS without re-analyzing (already done).
7. ID collision: two S-0820 entries exist in STUBS.md (004a42c5 and 004c7730) from parallel sessions. The 004a42c5 one has been removed; the 004c7730 one remains. Tracker repair needed.

---

## Scribe queue
Not scribing directly. Add to `re/SCRIBE_QUEUE.md` when ready.
