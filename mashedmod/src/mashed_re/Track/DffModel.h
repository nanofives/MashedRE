// Mashed RE — vehicle/prop DFF model parser (R5 opener).
//
// DFFs are standard RenderWare 3.6 clump streams (ver 0x1c02000a) — format
// cracked 2026-06-10 and documented with the librw cross-reference in
// re/analysis/formats/vehicle_dff.md; this is the C++ twin of
// re/tools/dff_dump.py. Frames' parent-chain world transforms are BAKED into
// model space at parse time (the standalone renders the whole car as a rigid
// unit and moves it via D3DTS_WORLD; per-part animation is R5+ scope).
#pragma once

#include <cstdint>
#include <vector>

namespace mashed_re {
namespace Track {

struct DffMaterial {
    char         tex_name[33];
    std::uint8_t rgba[4];
    // F3: the UVAnim dict entry this material is bound to via its RW UVAnim
    // material extension (rwID_UVANIMPLUGIN 0x135), e.g. "bmp_Sea_M". Empty if
    // the material carries no UV-anim binding. See track_anim_data.md F3.
    char         uv_anim[33];
};

// One renderable batch: model-space triangles for one material.
struct DffBatch {
    std::uint32_t      material;   // index into materials
    std::int32_t       atomic;     // owning atomic index (for part splits)
    float              abox[6];    // owning atomic's model-space bbox
    std::vector<float> verts;      // x,y,z per vertex (model space)
    std::vector<float> uvs;        // u,v per vertex (zeros if untextured)
    std::vector<std::uint32_t> prelit;  // RGBA per vertex (empty if absent)
    std::vector<std::uint16_t> tris;    // v0,v1,v2 triples
};

class DffModel {
public:
    // Parse a .DFF clump blob. Validations mirror dff_dump.py (frame/geo/
    // atomic counts, index ranges); false + last_error() on violation.
    bool Parse(const std::uint8_t* d, std::size_t len);
    const char* last_error() const { return err_; }

    std::vector<DffMaterial> materials;
    std::vector<DffBatch>    batches;     // one per (atomic, material) pair
    float                    bbox[6] = {};  // model-space min xyz, max xyz
    std::uint32_t            total_tris = 0, total_verts = 0;

private:
    const char* err_ = "not parsed";
};

}  // namespace Track
}  // namespace mashed_re
