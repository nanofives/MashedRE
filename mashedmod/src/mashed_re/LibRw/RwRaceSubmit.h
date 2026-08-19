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
#include <cstdint>
#include <windows.h>

struct IDirect3DDevice9;

namespace mashed_re {

namespace Race { struct RaceSceneState; }
namespace Track { class World; class DffModel; }
namespace Txd { class Dictionary; }

namespace LibRw {

// D1 INVERTED (2026-08-18): librw is the DEFAULT renderer. Returns true unless
// MASHED_RENDER_LIBRW=0, which reverts to the legacy D3D9 path for A/B. Unset or
// any non-"0" value keeps librw. (Was: "true when MASHED_RENDER_LIBRW=1, default
// OFF".) See RwRaceSubmit.cpp for the full rationale.
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

// ---- instanced models: props, cars, copters --------------------------------
//
// Every parsed Track::DffModel in TrackRenderer is a function LOCAL that dies as
// soon as its batches are baked (load_prop's `m`, LoadCar's `model`,
// LoadCarLiveries' per-livery `model`, LoadCopters' `model`). Nothing retains
// one. So a model must be handed over WHILE IT IS ALIVE, at load time, exactly as
// the static world is -- re-parsing it later would be the second-copy-of-a-parser
// failure mode again.
//
// Drop the previous track's registered models. MUST be called at the START of
// TrackRenderer::Load: models are registered DURING the load (from the live
// DffModel locals), so clearing them at the tail -- where the world is rebuilt --
// would destroy the ones just handed over.
// MASHED_LIBRW_INST: DEFAULT ON since 2026-08-02 (the earlier "STAGED, default
// OFF, ground goes black" caveat is superseded -- the regression was missing UV
// animation D-S3-SEA, not shading; see RwRaceSubmit.cpp). Instanced props/cars/
// copters route through librw and land at the best-measured config (gating shots
// 0.06-0.59). Before the D1 flag inversion this was unreachable on a clean env;
// after it, librw is default so instancing is LIVE by default.
// MASHED_LIBRW_INST=0 reverts to world-only.
bool RaceSubmit_InstancesEnabled();

// [D-S3-PROP] Per-model routing. Honours MASHED_LIBRW_ONLYPROP=<handle>: with it
// set, only that model's instances go to librw and every other model falls through
// to its D3D9 draw, so the untargeted models cancel against the
// MASHED_LIBRW_INST=0 baseline and one model becomes attributable by counting.
bool RaceSubmit_InstanceModelEnabled(int handle);

void RaceSubmit_BeginTrackLoad();

// Returns an opaque handle (>= 0) to keep in the owning struct, or -1 on failure.
// A -1 handle simply means "keep drawing this one through D3D9", so the port is
// incremental by construction and a single bad model cannot black out the scene.
// `ambient` = RaceSceneState::amb_world_ (0x00RRGGBB), already parsed by
// TrackRenderer before any prop loads. See BuildClump's note for why it matters.
// `uv_rates`, when non-null, is 2 floats (du/dt, dv/dt) per MATERIAL, in
// model.materials order -- the same rates the D3D9 path feeds its texture
// transform (TrackRenderer's mat_scroll / uv_rate, F3). librw's d3d9 shader pipe
// has NO texture matrix (default_VS.hlsl passes input.TexCoord straight through)
// and its UVAnim plugin is stream-only, so the scroll is applied by moving the
// texture coordinates themselves -- see RaceSubmit_SetAnimTime.
int RaceSubmit_RegisterModel(const Track::DffModel& model,
                             const Txd::Dictionary* dicts, std::size_t ndicts,
                             std::uint32_t ambient,
                             const float* uv_rates = nullptr,
                             std::size_t nmats = 0);

// Set the UV-animation clock for this frame. MUST be the same `t` the D3D9 path
// passes to TrackRenderer::Render, or the two renderers scroll to different
// phases and the surface differs even though everything else matches -- which is
// exactly what D-S3-SEA turned out to be (a static-UV librw sea read as "1.5x too
// bright" until MASHED_NO_UVSCROLL=1 reproduced it on the D3D9 path).
void RaceSubmit_SetAnimTime(float t);

// Queue one placed copy for this frame. `m44` is a D3DMATRIX in memory order --
// the SAME matrix the D3D9 path would pass to SetTransform(D3DTS_WORLD), so both
// renderers place the object identically instead of re-deriving it. Instances are
// consumed and cleared by RaceSubmit_Render.
void RaceSubmit_AddInstance(int handle, const float* m44);

// Per-frame submit. Reads eye/at + the projection quartet + fog + lights out of
// `st`; draws the static world and then every queued instance. Issues no Clear
// and no Begin/EndScene (the caller owns the frame). No-op until a track is built.
void RaceSubmit_Render(const Race::RaceSceneState& st);

// True once a scene is built and the submit path is live — lets the caller skip
// the D3D9 world batches so the two renderers do not overdraw each other.
bool RaceSubmit_Active();

}  // namespace LibRw
}  // namespace mashed_re
