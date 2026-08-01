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

// True when MASHED_RENDER_LIBRW=1. Checked in WinMain BEFORE InitD3D9, because
// the two paths must never coexist: librw owns Direct3DCreate9 + CreateDevice +
// Present itself (deps/librw/src/d3d/d3ddevice.cpp:1518, :1622, :1356), so it
// takes over the whole device, not a slice of it. Default OFF keeps the shipping
// D3D9 path reachable at all times.
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

}  // namespace LibRw
}  // namespace mashed_re
