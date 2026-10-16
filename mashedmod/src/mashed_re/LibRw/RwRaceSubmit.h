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
// SCOPE, STATED UP FRONT. This submits the static world and the instanced props
// and cars, and (librwrest) the copters. Particles, pickups, sky and the HUD still come from the D3D9
// path in TrackRenderer::Render. There is no D3D9 world/prop/car draw to fall back
// to: it was deleted in D1-residue R3 (2026-10-16).
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

// Bring librw up on the exe's already-created device+window. Call once, AFTER
// InitD3D9() (the device must exist to be adopted) and before any track load.
// Returns false and logs to log/librw_race.txt on the first failing step; the
// caller treats that as fatal (there is no D3D9 world to fall back to).
bool RaceSubmit_Init(HWND hwnd, IDirect3DDevice9* dev, int width, int height);

// Tear down. Does NOT release the adopted device — the exe owns that.
void RaceSubmit_Shutdown();

// Build the librw scene from the SAME parsed data TrackRenderer just used.
// Called from the tail of TrackRenderer::Load, where `world` and `dicts` are
// still alive (they are locals there), so no asset is parsed twice.
// Returns false if RaceSubmit_Init did not succeed or the scene build fails;
// TrackRenderer::Load then fails the track load.
bool RaceSubmit_OnTrackLoaded(const Track::World& world,
                              const Txd::Dictionary* dicts, std::size_t ndicts);

// ---- instanced models: props and cars --------------------------------------
//
// Every parsed Track::DffModel in TrackRenderer is a function LOCAL that dies as
// soon as it is handed over (load_prop's `m`, LoadCar's `model`). Nothing retains
// one. So a model must be handed over WHILE IT IS ALIVE, at load time, exactly as
// the static world is -- re-parsing it later would be the second-copy-of-a-parser
// failure mode again. (Copters are registered here too since librwrest, by
// TrackRenderer::LoadCopters.)
//
// Drop the previous track's registered models. MUST be called at the START of
// TrackRenderer::Load: models are registered DURING the load (from the live
// DffModel locals), so clearing them at the tail -- where the world is rebuilt --
// would destroy the ones just handed over.
void RaceSubmit_BeginTrackLoad();

// Returns an opaque handle (>= 0) to keep in the owning struct, or -1 on failure.
// A -1 handle means the model is NOT drawn (logged to log/librw_race.txt): there is
// no D3D9 draw to fall back to, so one bad model drops out of the scene rather than
// failing the track load.
// `ambient` = RaceSceneState::amb_world_ (0x00RRGGBB), already parsed by
// TrackRenderer before any prop loads. See BuildClump's note for why it matters.
// `uv_rates`, when non-null, is 2 floats (du/dt, dv/dt) per MATERIAL, in
// model.materials order -- the same rates the D3D9 path feeds its texture
// transform (TrackRenderer's mat_scroll / uv_rate, F3). librw's d3d9 shader pipe
// has NO texture matrix (default_VS.hlsl passes input.TexCoord straight through)
// and its UVAnim plugin is stream-only, so the scroll is applied by moving the
// texture coordinates themselves -- see RaceSubmit_SetAnimTime.
// [librwrest] `hierarchy`: build the clump with the DFF's own frame list (BuildClump), so
// instances can pose individual frames (RaceSubmit_AddInstancePosed).
int RaceSubmit_RegisterModel(const Track::DffModel& model,
                             const Txd::Dictionary* dicts, std::size_t ndicts,
                             std::uint32_t ambient,
                             const float* uv_rates = nullptr,
                             std::size_t nmats = 0,
                             bool hierarchy = false);

// [librwrest] Per hierarchy model, how a copy is drawn (FUN_00425e40 @ 0x00425e40 for the
// copters): skip the whole copy when the world bounding sphere of the atomic on FRAMELIST frame
// `cull_frame` lies outside the camera frustum (FUN_004c1b40 @ 0x004c1b40 on
// RpAtomicGetWorldBoundingSphere @ 0x004e6100; librw Camera::frustumTestSphere ==
// SPHEREOUTSIDE), otherwise render the atomics on `frames[0..n)` in that order, then any other
// atomic in clump order. cull_frame -1 = no test; n 0 = clump order.
void RaceSubmit_SetModelDrawOrder(int handle, const std::int32_t* frames, std::size_t n,
                                  std::int32_t cull_frame);

// [librwrest] The model draws with the RenderWare D3D9 driver's states (the d1r1 F1 rule,
// verify/d1r1_20261016/RESULT_d1r1.md section 4): each of its rasters' alpha flag from its texels
// (FUN_004d2980 / the texel rule RwRasterBridge records), ALPHAFUNC GREATER / ALPHAREF 0 (device
// init FUN_004d5bc0) and CULLMODE 2 while it draws, restored after. F1 stays opt-in for the
// world (U-9689 / U-9690); this applies it to one model only.
void RaceSubmit_SetModelRwStates(int handle);

// [librwrest] One frame's modelling matrix for one instance: FRAMELIST index `frame` of a
// hierarchy model, rows right/up/at/pos (RwMatrix order, 12 floats). Frames not posed keep
// the streamed (bind) matrix.
struct FramePose { std::int32_t frame; float m[12]; };

// [librwrest] Queue a copy of a hierarchy model with per-frame poses (FUN_0041f3d0 @
// 0x0041f3d0 / FUN_0041f4f0 @ 0x0041f4f0 / FUN_00425ca0 @ 0x00425ca0 pose wheel, body and
// rotor frames per object). `sun_scale` >= 0 scales the directional light's colour while
// this copy draws, restored after (FUN_00420050 @ 0x00420050: RpLightSetColor on
// DAT_0063d850 times rec+0x284 around the car's RpClumpRender); < 0 leaves it alone.
// `env_m44` non-null adds the car env coefficient/frame as RaceSubmit_AddInstanceEnv does.
// [rfx 2026-10-16] queue the point where the car shadow / headlight beam pass runs (LibRw/RwRaceFx.h):
// FUN_00410b30 calls FUN_0041ebb0 right after the car loop (0x00410bb4) and before the copters
// (FUN_00425e40, 0x00410c08). Without a marker the pass runs after every instance.
void RaceSubmit_AddCarFxMarker();

void RaceSubmit_AddInstancePosed(int handle, const float* m44, const FramePose* poses,
                                 std::size_t npose, float sun_scale, float env_coef = 0.f,
                                 const float* env_m44 = nullptr);

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

// [rtex 2026-10-16] Car MatFX passes (U-9693, LibRw/RwCarFx.h). BindCarDirt makes the two
// dirt.piz images (top-down BGRA) the DUAL texture of every effect-4 material by part code
// (FUN_0041ecc0 @ 0x0041ecc0); SetupCarEnv turns the env parts' materials into ENVMAP with
// the vehicle TXD's `env_tex_name` (FUN_00420420 @ 0x00420420). Both return the number of
// materials set. AddInstanceEnv queues a copy with its own env coefficient and env frame
// matrix (D3DMATRIX layout, like m44), which FUN_00420340 @ 0x00420340 refreshes per car.
int  RaceSubmit_BindCarDirt(int handle,
                            const std::uint8_t* body_bgra, std::uint32_t bw, std::uint32_t bh,
                            const std::uint8_t* wheel_bgra, std::uint32_t ww, std::uint32_t wh,
                            std::uint32_t filter_addressing);
int  RaceSubmit_SetupCarEnv(int handle, const Txd::Dictionary* dicts, std::size_t ndicts,
                            const char* env_tex_name);
void RaceSubmit_AddInstanceEnv(int handle, const float* m44, float env_coef,
                               const float* env_m44);

// [rprops U-9696] Queue one sky copy for this frame (same matrix convention as AddInstance). Sky copies
// are drawn before the world with depth test, depth write and fog off, then the states are restored.
void RaceSubmit_AddSky(int handle, const float* m44);

// Per-frame submit. Reads eye/at + the projection quartet + fog + lights out of
// `st`; draws the static world and then every queued instance. Issues no Clear
// and no Begin/EndScene (the caller owns the frame). No-op until a track is built.
void RaceSubmit_Render(const Race::RaceSceneState& st);

// True once a scene is built and the submit path is live — lets the caller skip
// the D3D9 world batches so the two renderers do not overdraw each other.
bool RaceSubmit_Active();

}  // namespace LibRw
}  // namespace mashed_re
