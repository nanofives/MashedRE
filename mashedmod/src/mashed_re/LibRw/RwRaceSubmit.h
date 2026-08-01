// LibRw/RwRaceSubmit.h — the IN-LOOP librw submit path (E2'b step 3).
//
// Lane M3-E1'..E3' (gate D2: librw is the SHIPPING renderer).
// Design: re/analysis/LIBRW_SIZING_2026-08.md, "E2'b step 3 — DESIGN DECIDED".
//
// WHAT THIS IS. E2'b step 2 could build a librw scene but only draw it from
// RenderWorldProbe() — a standalone probe with an invented camera and no fog or
// lights. A parity shot taken from that would look authoritative while measuring
// nothing, which is the same trap as the three measurement failures logged in the
// brief. So this path renders IN THE FRAME LOOP instead, beside
// g_track.Render(dev, t, &ci), and CONSUMES the camera/fog/light state
// TrackRenderer already resolved (Race::RaceSceneState) rather than re-deriving
// it. Nothing here parses COURSE.LUA or LIGHTS.DFF: a second copy of a parser
// that must agree with the first is the "wrong plate propagates into ports"
// failure mode.
//
// DEVICE OWNERSHIP. Upstream librw creates its own D3D9 device. It cannot here:
// the world must land in the exe's backbuffer, inside the exe's
// BeginScene/EndScene/Present, so that ONE frame holds both renderers' output and
// the capture harness (MASHED_DBG_BBDUMP) plus the d3d9-shim frame limiter keep
// working untouched. librw is therefore handed the exe's device via a documented
// local patch — see mashedmod/deps/librw/MASHED_PATCHES.md.
//
// SCOPE, STATED UP FRONT. This submits the STATIC WORLD only. Cars, props,
// particles, pickups, sky and the HUD still come from the D3D9 path. An imgdiff
// against verify/librw_ref/ therefore shows large and LEGITIMATE deltas; they are
// scope, not parity failures, and must not be reported as a parity number.
//
// EXE-ONLY, like the rest of LibRw/: build.bat compiles this as an isolated TU
// with the librw include path. Nothing in asi_sources.rsp may reference it.
// This header deliberately does NOT include <rw.h> (same containment pattern as
// RwBridge.h / RwSceneBuild.h) so TrackRenderer.cpp and exe_main.cpp can call in.
#pragma once

#include <cstddef>
#include <windows.h>

struct IDirect3DDevice9;

namespace mashed_re {

namespace Race { struct RaceSceneState; }
namespace Track { class World; }
namespace Txd { class Dictionary; }

namespace LibRw {

// True when MASHED_RENDER_LIBRW=1. Default OFF keeps the shipping D3D9 path
// reachable at all times.
bool RaceSubmit_Requested();

// Bring librw up on the exe's already-created device+window. Call once, AFTER
// InitD3D9() (the device must exist to be adopted) and before any track load.
// Returns false and logs to log/librw_race.txt on the first failing step; the
// caller must then carry on with the D3D9 path alone.
bool RaceSubmit_Init(HWND hwnd, IDirect3DDevice9* dev, int width, int height);

// Tear down. Does NOT release the adopted device — the exe owns that.
void RaceSubmit_Shutdown();

// Build the librw scene from the SAME parsed data TrackRenderer just used.
// Called from the tail of TrackRenderer::Load, where `world` and `dicts` are
// still alive (they are locals there), so no asset is parsed twice.
// No-op unless RaceSubmit_Init succeeded.
bool RaceSubmit_OnTrackLoaded(const Track::World& world,
                              const Txd::Dictionary* dicts, std::size_t ndicts);

// Per-frame submit. Reads eye/at + the projection quartet + fog + lights out of
// `st`; issues no Clear and no Begin/EndScene (the caller owns the frame).
// No-op until a track has been built.
void RaceSubmit_Render(const Race::RaceSceneState& st);

// True once a scene is built and the submit path is live — lets the caller skip
// the D3D9 world batches so the two renderers do not overdraw each other.
bool RaceSubmit_Active();

}  // namespace LibRw
}  // namespace mashed_re
