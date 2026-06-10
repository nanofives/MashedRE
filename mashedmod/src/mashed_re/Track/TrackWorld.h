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

namespace mashed_re {
namespace Track {

struct Material {
    char          tex_name[33];   // empty string = untextured (color-only)
    std::uint8_t  rgba[4];
};

struct Sector {
    std::vector<float>         verts;   // x,y,z per vertex
    std::vector<std::uint32_t> prelit;  // RGBA per vertex (empty if absent)
    std::vector<float>         uvs;     // u,v per vertex (empty if absent)
    // mat,v0,v1,v2 per triangle (mat already includes matListWindowBase)
    std::vector<std::uint16_t> tris;
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
    float                 bbox[6] = {};   // sup.xyz, inf.xyz (header order)
    std::uint32_t         total_tris  = 0;
    std::uint32_t         total_verts = 0;

private:
    const char* err_ = "not parsed";
};

}  // namespace Track
}  // namespace mashed_re
