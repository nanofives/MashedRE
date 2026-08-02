// LibRw/RwBridge.h — the standalone's seam onto vendored librw (MIT, aap).
//
// Lane M3-E1' (gate D2, user-decided 2026-07-31: librw is the SHIPPING renderer).
// Sizing rationale + the full E1'-E4' plan: re/analysis/LIBRW_SIZING_2026-08.md
//
// This header deliberately does NOT include <rw.h>. librw's headers put a large
// `namespace rw` into scope and are compiled with their own include path; keeping
// them out of exe_main.cpp is what lets RwBridge.cpp be the single isolated TU
// (same containment pattern as Collision/QhullBridge.cpp for vendored qhull —
// see mashedmod/build.bat:30-39).
//
// EXE-ONLY. librw is not linked into mashed_re_dev.asi: that target runs inside
// MASHED.exe, which has its own RenderWare engine and its own D3D9 device, and a
// second engine there would collide for no benefit (LIBRW_SIZING_2026-08.md R9).
// Nothing in asi_sources.rsp may reference this file.
#pragma once

#include <windows.h>

namespace mashed_re {
namespace LibRw {

// True when MASHED_LIBRW_SMOKE=1 — the standalone E1' probe, which DOES let
// librw create and own its own device (Direct3DCreate9 + CreateDevice + Present,
// deps/librw/src/d3d/d3ddevice.cpp:1518, :1622, :1356) and so must still be
// checked in WinMain BEFORE InitD3D9. It runs and exits the process.
//
// NOTE: this used to be MASHED_RENDER_LIBRW. E2'b step 3 took that name for the
// in-loop render path (LibRw/RwRaceSubmit.h), which instead ADOPTS the exe's
// device. Default OFF for both keeps the shipping D3D9 path reachable.
bool SmokeRequested();

// E1' acceptance probe. Brings librw up on an already-created HWND
// (EngineOpenParams{HWND}, deps/librw/src/d3d/rwd3d.h), clears to a known colour
// for `frames` presented frames, then tears down. Returns 0 on success, non-zero
// on the first failing step. Writes log/librw_smoke.txt with per-step results and
// the measured present rate (the latter is how we confirm the d3d9 shim's frame
// limiter still sees librw's Present — LIBRW_SIZING_2026-08.md I8).
//
// This is a probe, not the renderer. It draws no Mashed content; feeding it from
// our loaders is E2'.
int RunSmoke(HWND hwnd, int width, int height, int frames);

// E2'b step 3 — bring the engine up ADOPTING the exe's existing D3D9 device
// instead of letting librw create its own (deps/librw/MASHED_PATCHES.md, P1).
// Lives here, next to RunSmoke, so the plugin-registration list is written once:
// a second copy that drifted from this one would change how streams parse.
// Call after InitD3D9(). Returns false on the first failing step.
// `dev` is IDirect3DDevice9*, kept void* so this header need not include <d3d9.h>.
bool EngineStartAdopted(HWND hwnd, void* dev, int width, int height);

// Matching teardown. Does NOT release the adopted device.
void EngineStop();

}  // namespace LibRw
}  // namespace mashed_re
