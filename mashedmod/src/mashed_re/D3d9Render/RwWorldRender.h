// Mashed RE — WS-E1: RenderWare world-render path (generic traversal layer).
//
// Verbatim port of the GENERIC RW world render — the §4 functions of
// re/analysis/render_world_path_E1_2026-06-16.md — that replace the D3D9 "spike"
// world draw in TrackRenderer once the RW engine substrate exists. B-FULL is
// ratified (the full RW D3D9 driver is in-scope), but THIS file is the phase-1
// slice: the sector/atomic traversal + frustum cull + the RpAtomic render-callback
// CONTRACT. The RW device (DAT_007d3ff8), the RxPipeline node graph, the BSP->RpWorld
// loader and the per-atomic submit (-> IDirect3DDevice9::DrawIndexedPrimitive) are
// the DEFERRED prerequisites (E2 / B-full); see RwWorldRender.cpp TODOs.
//
// STATUS: PENDING parity (RW world render). Standalone has no RwEngineOpen/RpWorld
// yet, so the entry no-ops (guarded) until RwWorldRender_SetEngine() is fed a real
// RW globals + world. The traversal/cull logic is a faithful, diff-checkable port.
//
// Anchor: MASHED.exe SHA-256
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// Ghidra pool6, read-only, 2026-06-16. Every RVA cited inline.
#pragma once

#include <cstdint>

namespace mashed_re {
namespace D3d9Render {

// Bind the RW engine substrate the traversal reads (null in the standalone until
// the RW engine init + BSP->RpWorld loader are ported). globals = DAT_007d3ff8
// (RwGlobals); objExtBase = DAT_007d716c (per-object extension base).
void RwWorldRender_SetEngine(void* globals, void* objExtBase);

// True when MASHED_RW_RENDER is set AND a real RW engine+world is bound. When false,
// TrackRenderer keeps the spike world draw (the shipping path).
bool RwWorldRender_Enabled();

// Top-level world render for `cam` against the bound world (mirrors the shipping
// FUN_004270f0 -> FUN_004844a0 -> RwEngineForAllPlugins dispatch). No-op + log if
// the RW substrate is not bound. Returns the number of sectors traversed (0 if inert).
int RwWorldRender_Render(void* world, void* cam);

} // namespace D3d9Render
} // namespace mashed_re
