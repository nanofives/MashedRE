// LibRw/RwSceneBuild.h — Track::World / Track::DffModel -> librw scene objects.
//
// Lane M3-E2'b (gate D2). Impedance items I1 (world), I2 (props/cars) and I5
// (geometry flags) in re/analysis/LIBRW_SIZING_2026-08.md.
//
// This is the step that makes librw's "BSP is not supported at all" irrelevant:
// our own TrackWorld parser already turned GRAPH*.BSP into Track::World, so we
// CONSTRUCT rw::Geometry/rw::Atomic in memory rather than asking librw to read a
// file it cannot read. Same for DFF via Track::DffModel.
//
// No rw types cross this header (rw objects are void*), and it is EXE-ONLY —
// must not appear in asi_sources.rsp. Same containment as RwBridge/RwRasterBridge.
#pragma once

#include <cstdint>
#include <vector>

#include "../Track/DffModel.h"
#include "../Track/TrackWorld.h"
#include "../Txd/TxdDecoder.h"

namespace mashed_re {
namespace LibRw {

// A set of decoded TXDs to resolve material texture names against, searched in
// order. Mashed material names are matched case-insensitively (the TXD and the
// BSP material list do not agree on case).
struct TextureSource {
    const Txd::Dictionary* dicts;
    int                    count;
};

// Track::World -> rw::World* (returned as void*), one rw::Atomic per sector.
//
// Triangle layout note: Track::Sector::tris is (mat, v0, v1, v2) per triangle
// with `mat` already including matListWindowBase (TrackWorld.h:30), which maps
// 1:1 onto rw::Triangle{v[3], matId}. Every sector geometry gets the FULL world
// material list appended, because matId is a world-global index.
//
// The static world carries no vertex normals (confirmed by the parser's output
// and by TrackRenderer's lighting notes), so NORMALS/LIGHT are not set — the
// baked prelight is the lighting, exactly as on the D3D9 path.
void* BuildWorld(const Track::World& world, const TextureSource& tex);

// Track::DffModel -> rw::Clump* (returned as void*), one rw::Atomic per batch.
// DffBatch verts are model-space and frame-baked (DffModel.h), so every atomic
// gets an identity frame parented to the clump frame.
// `ambient` is the track's RpLight ambient as 0x00RRGGBB (RaceSceneState::amb_world_).
// It is ADDED into the prelit colours of batches that are prelit-but-not-LIGHT,
// mirroring what the D3D9 path bakes in via BuildDffBatches' AtomicLight. Those
// batches carry no normals, so librw's lighting cannot reach them: lightingCB_Shader
// sets ambient to BLACK for non-LIGHT geometry, and without this they render several
// times too dark (measured: Arctic sea prelit (12,14,11) vs baked (63,91,88)).
// LIGHT batches are left alone -- they DO receive the rw::Light ambient, and baking
// it in as well would double-count.
// `out_atomic_mat`, when non-null, receives one entry per atomic BuildClump
// creates, in creation order, holding that atomic's material index. The caller
// needs it to bind per-material state (UV-animation rates) to atomics: the
// mapping is not batch index -> atomic index, because batches with no vertices
// or no triangles are skipped and produce no atomic. Deriving it by re-walking
// model.batches would silently drift the moment that skip rule changes.
void* BuildClump(const Track::DffModel& model, const TextureSource& tex,
                 std::uint32_t ambient = 0,
                 std::vector<std::uint32_t>* out_atomic_mat = nullptr);

// E2'b step 2: build the Arctic world, render it once through librw from a
// DETERMINISTIC overview camera derived from the world bbox, and dump the
// backbuffer to `out_bmp`. Returns 0 on success.
//
// SCOPE, stated so the output is not over-read: this draws the STATIC WORLD ONLY.
// No car, props, copters, particles, pickups, HUD or menu. It therefore cannot be
// imgdiff'd against verify/librw_ref -- those are full-game frames, and the
// comparison would be apples-to-oranges, not parity. The gate at this stage is
// "does the world draw, with the right geometry and textures". Real E3' parity
// waits until the submit path covers everything the D3D9 path draws.
int RenderWorldProbe(int width, int height, const char* out_bmp);

// Self-test: load a real track's GRAPH*.BSP + TXD, build the scene, and verify
// the built geometry against the parser's own totals. Writes log/librw_scene.txt.
// Returns 0 on success, non-zero on the first failing check. Needs a live engine.
int SceneBuild_SelfTest();

}  // namespace LibRw
}  // namespace mashed_re
