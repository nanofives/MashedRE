// LibRw/RwRasterBridge.h — Txd::Texture -> rw::Raster / rw::Texture.
//
// Lane M3-E2'a (gate D2). Impedance mismatch I3 in
// re/analysis/LIBRW_SIZING_2026-08.md. This is the first real piece of the
// librw path: it takes what OUR decoder produced and hands librw a finished
// raster. librw's own TXD reader is never involved -- Mashed's TXD is a
// proprietary chunk-id 0x23 container it cannot read anyway.
//
// Like RwBridge.h this header does NOT include <rw.h>; rw objects cross the
// boundary as void*. Only RwRasterBridge.cpp and RwBridge.cpp see librw.
// EXE-ONLY -- must not appear in asi_sources.rsp.
#pragma once

#include <cstdint>

#include "../Txd/TxdDecoder.h"

namespace mashed_re {
namespace LibRw {

// Build an rw::Raster (C8888 | TEXTURE) from a decoded TXD texture's BASE MIP.
// Returns an rw::Raster* as void*, or nullptr on failure.
//
// PAL4 and PAL8 sources are CPU-EXPANDED to C8888 here rather than handed to
// librw as paletted rasters. librw maps both onto D3DFMT_P8
// (deps/librw/src/d3d/d3d.cpp:397-400), and D3DFMT_P8 texture support is dead on
// modern GPUs -- the same reason the existing D3D9 path expands on the CPU
// (D3d9Render/QuadRenderer.cpp:151). Expanding keeps the two paths comparable
// for the E3' diff instead of introducing a second variable.
//
// BASE MIP ONLY, deliberately. The shipping D3D9 path also uploads one level
// (TrackRenderer.cpp MakeTexture -> CreateTexture(..., 1, ...)), and the E3'
// reference set in verify/librw_ref was captured that way. Uploading the full
// mip chain would very likely look better, but changing the renderer AND the mip
// policy at once would make every E3' delta ambiguous. Revisit after E3' is green.
void* RasterFromTxdTexture(const Txd::Texture& tex);

// As above, wrapped in an rw::Texture with the TXD's name and filter/addressing
// applied. Returns an rw::Texture* as void*, or nullptr.
void* TextureFromTxdTexture(const Txd::Texture& tex);

// FNV-1a 32 of the raster's pixels READ BACK through rw::Raster::lock, emitted
// in canonical R,G,B,A order (so the value is independent of the backend's
// internal BGRA layout). Reading back rather than hashing the source is the
// point: it exercises create + expand + lock + the real D3D memory layout.
//
// re/tools/txd_format_census.py --rgba-hash computes the identical value in
// Python from the raw TXD bytes. Two independent implementations agreeing is
// the acceptance evidence for this bridge; a hash computed only on our side
// would just be our own code agreeing with itself.
bool RasterRgbaHash(void* raster, std::uint32_t* out_hash);

// Self-test: decode named TXDs out of real .piz archives, build a raster per
// texture, and log name/format/dimensions/hash to log/librw_raster.txt for
// comparison against the Python side. Returns the number of textures hashed,
// or -1 on failure. Requires a live librw engine (call after Engine::start).
int RasterBridge_SelfTest();

}  // namespace LibRw
}  // namespace mashed_re
