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

#include "MatFxExt.h"

namespace mashed_re {
namespace Track {

struct DffMaterial {
    char         tex_name[33];
    std::uint8_t rgba[4];
    // F3: the UVAnim dict entry this material is bound to via its RW UVAnim
    // material extension (rwID_UVANIMPLUGIN 0x135), e.g. "bmp_Sea_M". Empty if
    // the material carries no UV-anim binding. See track_anim_data.md F3.
    char         uv_anim[33];
    // [rtex] MatFX material extension (0x120): TRAINING ROAD.DFF and the vehicle
    // DFFs carry DUAL src 5 / dst 6 (MatFxExt.h).
    MatFx        fx;
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
    // WS-E s2: per-vertex world-space NORMALS, frame-rotation-baked like verts
    // (empty when the owning geometry carries no rpGEOMETRYNORMALS 0x10). Sized
    // to verts (x,y,z per vertex) when present — feeds the directional N·L term.
    std::vector<float> normals;
    // WS-E s2: owning geometry's RW flags that govern lighting at render:
    //   lit          = rpGEOMETRYLIGHT (0x20) AND normals present -> RW applies
    //                  ambient + directional N·L to this batch (else prelit only).
    //   modulate_mat = rpGEOMETRYMODULATEMATERIALCOLOR (0x40) -> the lit colour
    //                  is multiplied by the material's own RGBA (e.g. car livery
    //                  panels, flags 0x73).
    bool lit          = false;
    bool modulate_mat = false;
    // [geomlight class-scope 2026-08-31] owning geometry's raw RW format dword, so
    // the ambient fold can be scoped by class: ROAD.DFF=0x2008b (numTexCoordSets 2),
    // water LAKE/WATER0x.DFF=0x1000f (numTexCoordSets 1). numTexCoordSets = bits
    // 16-23. Lets RwSceneBuild fold ambient into the water prelit only.
    std::uint32_t geo_flags = 0;
    // [rtex] second texcoord set, parallel to uvs; empty unless the geometry
    // declares two sets (ROAD.DFF 0x2008b, vehicle dual parts 0x200b3).
    std::vector<float> uvs2;
    // [rtex] the owning geometry's part code: rpUSERDATA (0x11F) array 0 element 0
    // ("0.tv_part_id" on vehicles), which FUN_004b5190 @ 0x004b5190 reads off
    // atomic+0x18 (the geometry). -1 when the geometry has no int user data.
    std::int32_t part_id = -1;
    // [rtex] the owning atomic's frame origin in model space (the frame chain applied to
    // (0,0,0)). FUN_00420870 @ 0x00420870 keeps part 0x13's frame LTM position at car record
    // +0x258, the point the env coefficient's distance fade is measured from.
    float frame_pos[3] = {0.f, 0.f, 0.f};
    // [librwrest] the owning atomic's frame (index into DffModel::frames). verts/normals
    // stay baked through that frame's chain; RwSceneBuild's hierarchy mode undoes the bake
    // so the atomic hangs off its own frame, as in the RW clump.
    std::int32_t frame = -1;
    // [librwrest] the owning geometry's streamed bounding sphere (morph target 0: center
    // xyz in the geometry's own space, radius), what RpAtomicGetWorldBoundingSphere
    // (0x004e6100) transforms by the atomic's LTM.
    float sphere[4] = {0.f, 0.f, 0.f, 0.f};
};

// [librwrest] one FRAMELIST entry (rwID_FRAMELIST struct, 0x38 bytes): RW frame matrix
// rows right = rot[0..2], up = rot[3..5], at = rot[6..8], then pos, then the parent index
// (-1 for the root). Parents precede children (Parse rejects otherwise).
struct DffFrame {
    float        rot[9];
    float        pos[3];
    std::int32_t parent;
};

class DffModel {
public:
    // Parse a .DFF clump blob. Validations mirror dff_dump.py (frame/geo/
    // atomic counts, index ranges); false + last_error() on violation.
    // [rprops U-9695/U-9696] bake_frames=false leaves every atomic's vertices and normals in its own
    // geometry space (the frame parent chain is NOT applied). It is for clumps whose frames the game
    // itself overwrites at run time: Clump_Mts_Pair clumps (the atomic frame := the Mts record, the
    // clump frame translation := 0, 0x00479330) and the sky clumps (frame := identity, 0x004491e0 /
    // 0x00426d20). Baking would keep a root-frame offset the original discards (City Standard.dff
    // root y -0.957, Arctic SKY.DFF root y -0.155).
    bool Parse(const std::uint8_t* d, std::size_t len, bool bake_frames = true);
    const char* last_error() const { return err_; }

    // [geomlight water-scope 2026-08-31] The asset name this model was parsed
    // from (e.g. "SEA.DFF"), as passed to TrackRenderer's load_prop. Parse()
    // cannot know it -- the caller fills it in. Empty when the caller did not
    // set one (the world path and the car model), which reads as "not water".
    // RwSceneBuild::BuildClump uses it as the ambient-fold discriminator:
    // the RW geometry flags alone cannot separate a water surface from an
    // awning / lamppost / skydome (all are numTexCoordSets<=1 non-lit prelit),
    // which is what over-brightened City and Dump. See
    // verify/geomlight_broadcheck/RESULT.md.
    char                     source_name[64] = {};

    std::vector<DffMaterial> materials;
    std::vector<DffBatch>    batches;     // one per (atomic, material) pair
    std::vector<DffFrame>    frames;      // [librwrest] the clump's frame list, as streamed
    // [rstates] Per atomic (index = DffBatch::atomic, stream order): morph target 0's
    // bounding sphere, which RpAtomicSetGeometry copies to atomic+0x1c/+0x28. `c` is the
    // centre moved into model space through the same frame chain the vertices are baked
    // with; `r_world` is the radius scaled by that chain's largest axis length (what
    // RpAtomicGetWorldBoundingSphere 0x004e6100 does for a non-orthonormal LTM);
    // `r_obj` is the stream radius itself (FUN_004721e0 reads atomic+0x28 unscaled).
    struct AtomicSphere { float c[3]; float r_world; float r_obj; };
    std::vector<AtomicSphere> atomic_spheres;
    float                    bbox[6] = {};  // model-space min xyz, max xyz
    std::uint32_t            total_tris = 0, total_verts = 0;

private:
    const char* err_ = "not parsed";
};

}  // namespace Track
}  // namespace mashed_re
