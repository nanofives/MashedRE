# RW Engine State & Plugin Registry — `DAT_007d3ff8` and `DAT_00617fe0`

**Produced by:** Session 39 (render_promote_c2_rw_plugin), 2026-05-12
**Pool slot:** Mashed_pool2
**Source plates:** `re/analysis/render_promote_c2_rw_plugin/0x004c2{c90,d70,d90,de0,e10,e40,e70,ea0,ed0,f00,f30,f60,fb0}.md`

This doc complements [rwim2d_vertex_buffer.md](rwim2d_vertex_buffer.md) (which covers `DAT_007d3ff8` offsets +0x18 / +0x20 / +0x2c / +0x30 used by Im2D draws). The plates promoted in Session 39 expose three further offsets used by the driver-system dispatcher and the engine state machine, plus the engine-level plugin-registry global.

---

## `DAT_007d3ff8` — engine state pointer (extended layout)

Confirmed offsets seen in the 13 RW-plugin/engine-init wrappers:

| Offset | Type | First read / write RVA | Notes |
|--------|------|------------------------|-------|
| +0x00 (deref) | undefined4 | 0x004c2fb0 `*(DAT_007d3ff8+0x10)` first-word load | First word of the dispatcher object — passed to `FUN_004cf160` as a context handle (Mashed init "pipe wire-up" step). |
| +0x10 | pointer to dispatcher object | 0x004c2c90 (caller computes `DAT_007d3ff8+0x10` as `param_1`); all 12 wrapper calls cite this offset | Address of the device-driver dispatcher. The object pointed to has a vtable at `*(dispatcher+0x4)`. |
| +0x18 | float | 0x00472c60 (rwim2d_vertex_buffer.md) | `g_rwDevice.zRecip` constant. |
| +0x20 | code* | 0x00472c60 (rwim2d_vertex_buffer.md) | render-state setter `(stateIndex, value)`. |
| +0x2c | code* | rwim2d_vertex_buffer.md | DrawIndexed (textured quad). |
| +0x30 | code* | rwim2d_vertex_buffer.md | `RwIm2DRenderPrimitive(primType, verts, count)`. |
| +0xCC | code* | 0x004c2c90 case 0xe error path | Error/log callback. Invoked as `(*(code**)(DAT_007d3ff8+0xCC))(out_ptr, msg_string)` when caller passes subsystem index 0 to GetSubSystemInfo. |
| +0x124 | undefined4 | 0x004c2f60 (write `2`), 0x004c2fb0 (write `3`) | RW engine state machine word. Values observed: **2** = stopped (after `FUN_004c2f60` engine-stop), **3** = started (after `FUN_004c2fb0` engine-start with FinalizeStart success). |

Pattern: `DAT_007d3ff8` is a single global pointer to a struct ≥ 0x128 bytes in size. The structure is the RW engine context (canonical RW name `RwGlobals` / `rwGlobals.engine`).

### Engine state machine values

State transitions observed:

```
(after engine-init success)  state := 3   # FUN_004c2fb0 writes 3 after cmd 0x11 FinalizeStart
(after engine-stop  success) state := 2   # FUN_004c2f60 writes 2 after cmd 0x03 Close
```

Values 0 and 1 not yet observed in this session — likely correspond to "uninit" and "init-but-not-open" per canonical RW state monotonic (`rwINITCOUNT` enum in RW 3.x). [UNCERTAIN] — leaving without U-ID pending Session 39 scribe sweep, where the existing U-0090 may subsume it.

---

## `DAT_00617fe0` — engine-level plugin registry

Single 4-byte global at 0x00617fe0. Treated as the **address-of** the registry object, never deref'd by the 13 functions in this session; passed unmodified as the first argument to:

- `FUN_004d7de0(&DAT_00617fe0, size, id, ctor, dtor, copy)` — add plugin slot. Wrapped by `FUN_004c2d90`.
- `FUN_004d8000(&DAT_00617fe0, DAT_007d3ff8)` — open / activate the registry (called inside `FUN_004c2fb0`).
- `FUN_004d8060(&DAT_00617fe0, DAT_007d3ff8)` — close / walk the registry (called inside `FUN_004c2f60`).

Sister globals (per `re/analysis/librw_plugin_compat/REPORT.md`):
- `DAT_00618180` — wrapped by `FUN_004c7690` (different RW object type's registry).
- `DAT_00618664` — wrapped by `FUN_004e7d40` (third RW object type's registry).

Registry layout itself is documented in `re/analysis/librw_plugin_compat/REPORT.md` (14 plugin slots filed there for the engine-level registry). librw analog: `librw::Engine::s_plglist`.

---

## Driver-system command dispatch (`FUN_004c2c90`)

The 12 thin wrappers in Session 39's bucket all dispatch through `FUN_004c2c90(DAT_007d3ff8+0x10, cmd, out, in1, in2)`. `FUN_004c2c90` itself:
1. Reads `*(dispatcher+0x4)` and indirect-calls it with `(cmd, out, in1, in2)`.
2. On non-zero return → propagate.
3. On zero return → switch on `cmd`:
   - 0x0d/0x0f: write a default to `*out` and return 1.
   - 0x0e: if `in2 == 0`, call the error callback at `DAT_007d3ff8+0xCC` with the string `"Only rendering sub system"`.
   - 0x10: success-fallback iff `in2 == 0`.
   - 0x11 / 0x12: return 1 unconditionally (no-op for "FinalizeStart" / "InitiateStop").
   - everything else: route to the error sink (`FUN_004d7ff0`/`FUN_004d8480`).

| cmd  | Wrapper RVA | Sketched role | Dispatcher fallback |
|------|-------------|---------------|---------------------|
| 0x02 | inline in 0x004c2fb0 | Open | none (driver-only) |
| 0x03 | inline in 0x004c2f60, 0x004c2fb0 | Close | none |
| 0x05 | 0x004c2ea0 | GetNumModes | none |
| 0x06 | 0x004c2ed0 | GetModeInfo | none |
| 0x07 | 0x004c2f30 | UseMode | none |
| 0x0a | 0x004c2f00 | GetMode (current) | none |
| 0x0d | 0x004c2de0 | GetNumSubSystems | writes `*out=1`, returns 1 |
| 0x0e | 0x004c2e10 | GetSubSystemInfo | error callback on idx==0 |
| 0x0f | 0x004c2e40 | GetCurrentSubSystem | writes `*out=0`, returns 1 |
| 0x10 | 0x004c2e70 | SetSubSystem | success if idx==0 |
| 0x11 | inline in 0x004c2fb0 | FinalizeStart | returns 1 unconditionally |
| 0x12 | inline in 0x004c2f60 | InitiateStop | returns 1 unconditionally |

Note: command numbering matches the RW 3.x `rwDEVICESYSTEMFN` enumeration ([UNCERTAIN] — keep as candidate-rename for scribe sweep; do **not** rename in master Ghidra from this session).

---

## Open candidate renames (queued for scribe sweep, **not** applied here)

This session produced strong evidence that the following Ghidra symbols could be renamed in master to their RenderWare-canonical names. Renames are deferred to the scribe sweep — they require a master-Ghidra write transaction.

| RVA       | Current name | Candidate rename | Evidence |
|-----------|--------------|------------------|----------|
| 0x004c2c90 | `FUN_004c2c90` | `_rwDeviceSystemRequest` (or `RwDeviceSystemDefault`) | central dispatcher; 12-call fan-in; case-table maps 1:1 to `rwDEVICESYSTEMFN` |
| 0x004c2d70 | `FUN_004c2d70` | `_rwPluginRegistryGetStateGuard` (returns DAT_007d3ff4) | leaf; single caller is `FUN_004d7de0` (RwPluginRegistryAddPlugin) which uses the value as registration-freeze gate |
| 0x004c2d90 | `FUN_004c2d90` | `RwEngineRegisterPlugin` | 4-arg wrapper around `FUN_004d7de0(&DAT_00617fe0, ...)`; engine-level RW plugin registration thin shim |
| 0x004c2de0 | `FUN_004c2de0` | `RwEngineGetNumSubSystems` | cmd 0x0d wrapper |
| 0x004c2e10 | `FUN_004c2e10` | `RwEngineGetSubSystemInfo` | cmd 0x0e wrapper |
| 0x004c2e40 | `FUN_004c2e40` | `RwEngineGetCurrentSubSystem` | cmd 0x0f wrapper |
| 0x004c2e70 | `FUN_004c2e70` | `RwEngineSetSubSystem` | cmd 0x10 wrapper |
| 0x004c2ea0 | `FUN_004c2ea0` | `RwEngineGetNumVideoModes` | cmd 0x05 wrapper |
| 0x004c2ed0 | `FUN_004c2ed0` | `RwEngineGetVideoModeInfo` | cmd 0x06 wrapper |
| 0x004c2f00 | `FUN_004c2f00` | `RwEngineGetCurrentVideoMode` | cmd 0x0a wrapper |
| 0x004c2f30 | `FUN_004c2f30` | `RwEngineSetVideoMode` | cmd 0x07 wrapper |
| 0x004c2f60 | `FUN_004c2f60` | `RwEngineStop` | cmds 0x12 + 0x03 + state := 2 |
| 0x004c2fb0 | `FUN_004c2fb0` | `RwEngineStart` | cmds 0x02 + 0x11 + state := 3 |
| 0x007d3ff8 | `DAT_007d3ff8` | (already labeled `g_rwDevice` in rwim2d_vertex_buffer.md) — could be promoted to `RWSRCGLOBAL` / `rwGlobals` | matches RW canonical engine-globals struct (>= 0x128 bytes) |
| 0x007d3ff4 | `DAT_007d3ff4` | `g_rwPluginRegistryFrozen` (or `rwPluginStateClosed`) | leaf-getter at 0x004c2d70; sole consumer freezes registration |
| 0x00617fe0 | `DAT_00617fe0` | `g_rwEnginePlugins` (engine-level RwPluginRegistry instance) | passed to RwPluginRegistry{Add,Open,Close} equivalents |

---

## Cross-references

- [[rwim2d_vertex_buffer]] — covers `DAT_007d3ff8` offsets +0x18 / +0x20 / +0x2c / +0x30 used by the Im2D draw path.
- [`librw_plugin_compat/REPORT.md`](../librw_plugin_compat/REPORT.md) — 14 engine-plugin slots registered against `&DAT_00617fe0`, compared field-by-field with librw module globals.
