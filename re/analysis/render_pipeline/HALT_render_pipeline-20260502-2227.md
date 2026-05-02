---
session: render_pipeline-20260502-2227
slot: Mashed_pool5
date: 2026-05-02
outcome: HALT — specified anchors not found
---

## Halt reason

All six specified RW pipeline anchors (search_defined_strings + external_imports_list) returned 0 results:

- `RxPipeline` — 0
- `RpAtomicSetPipeline` — 0
- `RpMaterialSetTexture` — 0
- `RpMaterialCreate` — 0
- `rpATOMIC` — 0
- `RpMeshHeader` — 0

No FidDB-matched symbols: symbol_by_name for Pipeline, RpAtomic, RpMaterial, RxD3D9 all returned 0.

RenderWare is **statically linked** (zero Rw*/Rp* entries in the 177-symbol import table; only D3D9.DLL::Direct3DCreate9, DINPUT8, KERNEL32, USER32, WINMM, DSOUND, GDI32, ADVAPI32, OLE32).

## Binary observation

Defined strings ARE present and search_defined_strings IS functional. Found game-code RW strings:

| Address    | Value |
|------------|-------|
| 0x005cf7d0 | `Calling RwEngineStart\n` |
| 0x005cf808 | `Calling RwEngineOpen\n` |
| 0x005cf820 | `Calling RenderwareAttachPlugins\n` |
| 0x005cf884 | `Calling RwEngineInit\n` |

These addresses are REPORTED but NOT VERIFIED via function_at in this session (session halted before work loop). Do NOT cite these addresses in hooks.csv without calling function_at in a follow-up session.

## Alternative anchor path (see D-2140)

`0x005cf820` ("Calling RenderwareAttachPlugins\n") is a promising anchor for a re-run.
`reference_to(0x005cf820)` should yield the function that calls `RpAtomicSetPipeline` or equivalent custom pipeline setup, if any exists.

## Conclusion

Stock RW paths are used for the majority of the binary. Custom pipeline setup (if any) would be inside the plugin-registration function referenced by the "RenderwareAttachPlugins" string. A re-run session should start from that reference rather than from pipeline API strings (which are stripped in this release build).
