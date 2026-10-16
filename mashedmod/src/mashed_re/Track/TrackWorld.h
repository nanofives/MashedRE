// Mashed RE — track world geometry parser (R4 opener).
//
// GRAPH*.BSP inside TRACKS/<track>.piz is a standard RenderWare 3.6 world
// stream (rwID_WORLD 0x0B, ver 0x1c02000a). Format cracked 2026-06-10 and
// validated on 13/13 tracks by re/tools/track_dump.py; byte-offset tables in
// re/analysis/formats/track_world_bsp.md. This is the C++ twin of that
// parser: WORLD header -> MATLIST (texture names) -> plane-sector tree ->
// atomic sectors (vertices / prelight / uv / triangles). Triangle u16 order
// is (v0,v1,v2,mat) — determined empirically across all 13 tracks.
#pragma once

#include <cstdint>
#include <vector>

#include "MatFxExt.h"

namespace mashed_re {
namespace Track {

struct Material {
    char          tex_name[33];   // empty string = untextured (color-only)
    std::uint8_t  rgba[4];
    // F3: UVAnim dict entry bound via the RW UVAnim material extension
    // (rwID_UVANIMPLUGIN 0x135); empty if none. See track_anim_data.md F3.
    char          uv_anim[33];
    // [rtex] MatFX material extension (0x120); TRAINING material 0 is DUAL.
    MatFx         fx;
};

struct Sector {
    std::vector<float>         verts;   // x,y,z per vertex
    std::vector<std::uint32_t> prelit;  // RGBA per vertex (empty if absent)
    std::vector<float>         uvs;     // u,v per vertex (empty if absent)
    // [rtex] second texcoord set (u,v per vertex) when the world format declares
    // two (TRAINING 0x400200c9); empty otherwise. Stage 1 of MatFX DUAL reads it.
    std::vector<float>         uvs2;
    // mat,v0,v1,v2 per triangle (mat already includes matListWindowBase)
    std::vector<std::uint16_t> tris;
    // [rstates] the atomic-sector STRUCT's own box (+0x0C, 6 floats, stream order).
    // RW keeps it as the tight box (sector+0x60/+0x6c, 0x004e9eee..0x004e9f18); the
    // frustum walk tests the BSP CELL box instead (World::planes, FUN_004e51e0).
    float                      box[6] = {};
    // [rsky] the sector EXTENSION's BinMesh PLG (chunk 0x50E: u32 flags, numMeshes, totalIndices,
    // then per mesh u32 numIndices, u32 matIndex, numIndices u32 indices): the matIndex of each
    // mesh in the order RpClumpRender-style sector drawing walks them. Empty if the sector has no
    // BinMesh chunk, or its mesh materials are not exactly the materials its triangles use.
    // The index is the same value as tris[i*4] (the world material list index).
    std::vector<std::uint16_t> mesh_order;
};

// [rstates] One PLANESECTOR (0x0A) of the world BSP, STRUCT order {type, value,
// leftIsAtomic, rightIsAtomic, leftValue, rightValue}. `type` is the RW byte offset of
// the split axis (0/4/8 = x/y/z). Children are encoded: >= 0 indexes `planes`, < 0 is
// the atomic sector -1 - child (index into `sectors`, which are numbered in the stream's
// depth-first, left-first order -- the same order Parse() fills `sectors`).
struct PlaneNode {
    std::int32_t type = 0;
    float        value = 0.f, left_value = 0.f, right_value = 0.f;
    std::int32_t left = 0, right = 0;
};

class World {
public:
    // Parse a GRAPH*.BSP blob. On failure returns false and last_error()
    // names the violated invariant (all the track_dump validations are
    // enforced here too: sector sums == header totals, indices in range).
    bool Parse(const std::uint8_t* d, std::size_t len);

    const char* last_error() const { return err_; }

    std::vector<Material> materials;
    std::vector<Sector>   sectors;
    std::vector<PlaneNode> planes;        // [rstates] BSP plane sectors
    std::int32_t          root = -1;      // [rstates] encoded like PlaneNode children
    float                 bbox[6] = {};   // sup.xyz, inf.xyz (header order)
    std::uint32_t         total_tris  = 0;
    std::uint32_t         total_verts = 0;

private:
    const char* err_ = "not parsed";
};

}  // namespace Track
}  // namespace mashed_re
