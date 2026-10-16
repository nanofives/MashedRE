// LibRw/RwSceneBuild.cpp — build librw scene objects from our parsed track data.
// Lane M3-E2'b (gate D2). See RwSceneBuild.h.

#include "RwSceneBuild.h"

#include <windows.h>
#define WITH_D3D
#include <rw.h>

#include <cstdarg>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <vector>

#include "RwRasterBridge.h"
#include "../Piz/PizReader.h"

namespace mashed_re {
namespace LibRw {
namespace {

// Resolve a material texture name against the supplied dictionaries.
// Case-insensitive: the BSP material list and the TXD disagree on case.
rw::Texture* ResolveTexture(const char* name, const TextureSource& src) {
    if (!name || !name[0]) return nullptr;
    for (int d = 0; d < src.count; ++d) {
        const Txd::Dictionary& dict = src.dicts[d];
        for (std::uint32_t i = 0; i < dict.count(); ++i) {
            const Txd::Texture& t = dict.texture(i);
            if (_stricmp(t.name, name) == 0)
                return static_cast<rw::Texture*>(TextureFromTxdTexture(t));
        }
    }
    return nullptr;
}

}  // namespace

// [rtex 2026-10-16] MatFX DUAL (U-9692). The original renders a material whose MatFX
// effect is DUAL through FUN_005405c0 @ 0x005405c0 (dispatched by the MatFX atomic
// callback FUN_00543500 @ 0x00543500 on effect 4): stage-1 dual texture,
// BLENDTEXTUREALPHA, then MODULATE by diffuse -- librw's d3d9matfx.cpp matfxRender_Dual
// (MASHED LOCAL PATCH P9). MASHED_LIBRW_DUAL=0 builds every material without the effect,
// i.e. the pre-rtex single-stage draw.
bool DualEnabled() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_LIBRW_DUAL");
        return !(e && e[0] == '0' && e[1] == '\0');
    }();
    return on;
}

namespace {

// Does this parsed material carry a MatFX DUAL slot the port renders? Effect 4 only:
// 6 (DUALUVTRANSFORM) adds the UV-transform matrices FUN_005405c0 uploads at 0x00540701 /
// 0x00540cbb, which are not ported [UNCERTAIN U-9697]; one shipped material uses it (Storm
// WATER.DFF, verify/rtex_20261016/evidence/matfx_census.txt), which keeps its single-stage draw.
template <typename MatT>
bool IsDualMaterial(const MatT& m) {
    return DualEnabled() && m.fx.has_dual && m.fx.type == rw::MatFX::DUAL;
}

// Build one rw::Material per parsed material, binding textures where named.
// `resolved`/`named` are counted so the self-test can assert the bindings
// actually happened rather than silently rendering untextured.
template <typename MatT>
void BuildMaterials(const std::vector<MatT>& src, const TextureSource& tex,
                    std::vector<rw::Material*>& out,
                    int* named = nullptr, int* resolved = nullptr) {
    out.reserve(src.size());
    for (const MatT& m : src) {
        rw::Material* mat = rw::Material::create();
        if (!mat) { out.push_back(nullptr); continue; }
        mat->color.red   = m.rgba[0];
        mat->color.green = m.rgba[1];
        mat->color.blue  = m.rgba[2];
        mat->color.alpha = m.rgba[3];
        if (m.tex_name[0]) {
            if (named) ++*named;
            if (rw::Texture* t = ResolveTexture(m.tex_name, tex)) {
                mat->setTexture(t);
                if (resolved) ++*resolved;
            }
        }
        // [rtex] MatFX DUAL, as streamed (MatFxExt.h). A dual texture the dictionaries
        // do not hold stays null, as RW's MatFX reader leaves it when the texture is not
        // found; FUN_005405c0 then draws the plain path (`*param_5 == 0` at 0x005405d0).
        // The vehicles' CarDirt / WheelDirt are such names: the game binds dirt.piz
        // textures later (FUN_0041ecc0 @ 0x0041ecc0 -> FUN_005401d0).
        if (IsDualMaterial(m)) {
            rw::MatFX::setEffects(mat, m.fx.type);
            if (rw::MatFX* fx = rw::MatFX::get(mat)) {
                fx->setDualTexture(ResolveTexture(m.fx.dual_tex, tex));
                fx->setDualSrcBlend(m.fx.dual_src);
                fx->setDualDestBlend(m.fx.dual_dst);
            }
        }
        out.push_back(mat);
    }
}

// Fill texcoord set 1 (stage-1 coordinates of MatFX DUAL).
void FillTexCoords1(rw::Geometry* geo, std::int32_t nv, const std::vector<float>& uvs2) {
    if (geo->numTexCoordSets < 2 || !geo->texCoords[1] || uvs2.empty()) return;
    for (std::int32_t i = 0; i < nv; ++i) {
        geo->texCoords[1][i].u = uvs2[i * 2 + 0];
        geo->texCoords[1][i].v = uvs2[i * 2 + 1];
    }
}

// Geometry format flags for a mesh with `nsets` texcoord sets: RW writes rpGEOMETRY
// TEXTURED2 (0x80) without TEXTURED for two sets (ROAD.DFF 0x2008b), and the matfx
// callback keys the stage-1 TEXCOORDINDEX on that bit (0x00540c4c).
std::uint32_t TexFlags(std::size_t nsets) {
    if (nsets >= 2) return rw::Geometry::TEXTURED2 | (2u << 16);
    return nsets ? static_cast<std::uint32_t>(rw::Geometry::TEXTURED) : 0u;
}

void AppendMaterials(rw::Geometry* geo, const std::vector<rw::Material*>& mats) {
    for (rw::Material* m : mats)
        if (m) geo->matList.appendMaterial(m);
}

// Fill the shared per-vertex attributes. `prelit` may be empty; `uvs` may be
// empty; `normals` may be empty. Counts are the caller's responsibility.
void FillVertexData(rw::Geometry* geo, std::int32_t nv,
                    const std::vector<float>& verts,
                    const std::vector<float>& uvs,
                    const std::vector<std::uint32_t>& prelit,
                    const std::vector<float>* normals) {
    rw::V3d* dv = geo->morphTargets[0].vertices;
    for (std::int32_t i = 0; i < nv; ++i) {
        dv[i].x = verts[i * 3 + 0];
        dv[i].y = verts[i * 3 + 1];
        dv[i].z = verts[i * 3 + 2];
    }
    if (normals && !normals->empty() && geo->morphTargets[0].normals) {
        rw::V3d* dn = geo->morphTargets[0].normals;
        for (std::int32_t i = 0; i < nv; ++i) {
            dn[i].x = (*normals)[i * 3 + 0];
            dn[i].y = (*normals)[i * 3 + 1];
            dn[i].z = (*normals)[i * 3 + 2];
        }
    }
    if (geo->colors && !prelit.empty()) {
        // Prelight is an RGBA dword in RW byte order (R low) -- TrackRenderer.cpp:199
        // states the same convention -- and rw::RGBA is {red,green,blue,alpha} in
        // that order, so the dword copies straight across on little-endian.
        for (std::int32_t i = 0; i < nv; ++i) {
            const std::uint32_t p = prelit[i];
            geo->colors[i].red   = static_cast<rw::uint8>(p & 0xFF);
            geo->colors[i].green = static_cast<rw::uint8>((p >> 8) & 0xFF);
            geo->colors[i].blue  = static_cast<rw::uint8>((p >> 16) & 0xFF);
            geo->colors[i].alpha = static_cast<rw::uint8>((p >> 24) & 0xFF);
        }
    } else if (geo->colors) {
        for (std::int32_t i = 0; i < nv; ++i)
            geo->colors[i] = rw::RGBA{255, 255, 255, 255};
    }
    if (geo->texCoords[0] && !uvs.empty()) {
        for (std::int32_t i = 0; i < nv; ++i) {
            geo->texCoords[0][i].u = uvs[i * 2 + 0];
            geo->texCoords[0][i].v = uvs[i * 2 + 1];
        }
    }
}

// Finish a geometry: bound sphere + mesh build. unlock() builds the meshes that
// the D3D9 pipeline instances from (geometry.cpp Geometry::unlock).
void FinishGeometry(rw::Geometry* geo) {
    geo->calculateBoundingSphere();
    geo->unlock();
}

rw::Atomic* MakeAtomic(rw::Geometry* geo, rw::Frame* parent) {
    rw::Atomic* a = rw::Atomic::create();
    if (!a) return nullptr;
    rw::Frame* f = rw::Frame::create();
    // [D-S3-7 FIX] The arguments were INVERTED here. Frame::addChild(child) makes
    // `this` the PARENT (frame.cpp:87-99), so `f->addChild(parent)` hung the clump
    // root off the atomic instead of the atomic off the root. Consequence: moving
    // the clump's frame moved a CHILD, the atomic's own frame stayed at identity,
    // and every instanced model drew at the world origin regardless of the
    // transform submitted. Measured directly: clumpframe=(-25.21,0.04,15.78) while
    // the atomic's LTM read (0.00,0.00,0.00).
    //
    // It went unnoticed in E2'b step 2 because the only consumer was the static
    // world, whose frame is identity -- wrong parenting and correct parenting are
    // indistinguishable at identity. It surfaced the moment anything had to MOVE.
    if (parent) parent->addChild(f);
    a->setFrame(f);
    a->setGeometry(geo, 0);
    return a;
}

}  // namespace

// Defined further down next to kSceneLog, inside the unnamed namespace. All
// `namespace {}` blocks in a TU are the SAME namespace, so the declaration must
// go in one too -- at outer scope it becomes a DIFFERENT function and every later
// call is ambiguous. Declared HERE, above BuildWorld, because BuildWorld logs too
// (it used to sit between BuildWorld and BuildClump, which only the latter needed).
namespace { void SLog(const char* fmt, ...); }

void* BuildWorld(const Track::World& world, const TextureSource& tex,
                 std::vector<void*>* sector_atomics) {
    if (sector_atomics) sector_atomics->assign(world.sectors.size(), nullptr);
    std::vector<rw::Material*> mats;
    int w_named = 0, w_resolved = 0;
    BuildMaterials(world.materials, tex, mats, &w_named, &w_resolved);
    // D-S3-BANK probe: the WORLD's texture resolution has never been logged --
    // BuildClump asks for these counts, BuildWorld did not. An unresolved world
    // material draws with flat material colour instead of its texture, which is
    // one of the few things that could put a warm wash on the D3D9 snow bank and
    // not on librw's. Name every material so a specific one can be blamed rather
    // than the set being declared "fine" from a total.
    SLog("world: mats=%zu named=%d resolved=%d sectors=%zu",
         world.materials.size(), w_named, w_resolved, world.sectors.size());
    for (std::size_t i = 0; i < world.materials.size(); ++i) {
        const auto& m = world.materials[i];
        const bool has_tex = m.tex_name[0] != 0;
        const bool got_tex = has_tex && ResolveTexture(m.tex_name, tex) != nullptr;
        SLog("  world.mat[%zu] tex='%s' %s rgba=(%u,%u,%u,%u)",
             i, has_tex ? m.tex_name : "(none)",
             !has_tex ? "NO-TEXNAME" : (got_tex ? "resolved" : "*** UNRESOLVED ***"),
             (unsigned)m.rgba[0], (unsigned)m.rgba[1],
             (unsigned)m.rgba[2], (unsigned)m.rgba[3]);
    }

    rw::World* rww = rw::World::create();
    if (!rww) return nullptr;

    // The sectors go into a CLUMP, not straight onto the world via addAtomic.
    // librw's World::addAtomic only sets atomic->world -- it adds the atomic to
    // no list (world.cpp) -- and World::render() walks the CLUMP list only, with
    // upstream's own comment "this is very wrong, we really want world sectors".
    // So a bare atomic added to the world would silently never draw. One clump of
    // sector atomics is the shape that actually renders.
    rw::Clump* sectors = rw::Clump::create();
    if (!sectors) { rww->destroy(); return nullptr; }
    rw::Frame* worldFrame = rw::Frame::create();
    sectors->setFrame(worldFrame);

    std::size_t sector_index = static_cast<std::size_t>(-1);   // [rstates]
    for (const Track::Sector& s : world.sectors) {
        ++sector_index;
        const std::int32_t nv = static_cast<std::int32_t>(s.verts.size() / 3);
        const std::int32_t nt = static_cast<std::int32_t>(s.tris.size() / 4);
        if (nv == 0 || nt == 0) continue;

        // [rtex] two texcoord sets when the world declares them and DUAL is on.
        const bool two_sets = DualEnabled() && !s.uvs2.empty();
        bool uses_dual = false;
        for (std::size_t i = 0; i < s.tris.size() / 4 && !uses_dual; ++i) {
            const std::uint16_t m = s.tris[i * 4 + 0];
            uses_dual = m < world.materials.size() && IsDualMaterial(world.materials[m]);
        }
        std::uint32_t flags = rw::Geometry::POSITIONS;
        if (!s.uvs.empty())    flags |= TexFlags(two_sets ? 2 : 1);
        if (!s.prelit.empty()) flags |= rw::Geometry::PRELIT;
        // Deliberately no NORMALS/LIGHT: the static world carries no vertex
        // normals, so its baked prelight IS the lighting -- same as D3D9 today.

        rw::Geometry* geo = rw::Geometry::create(nv, nt, flags);
        if (!geo) continue;
        FillVertexData(geo, nv, s.verts, s.uvs, s.prelit, nullptr);
        if (two_sets) FillTexCoords1(geo, nv, s.uvs2);
        for (std::int32_t i = 0; i < nt; ++i) {
            // Track::Sector::tris is (mat, v0, v1, v2) -- TrackWorld.h:30.
            const std::uint16_t tmat = s.tris[i * 4 + 0];
            geo->triangles[i].matId = tmat;
            geo->triangles[i].v[0]  = s.tris[i * 4 + 1];
            geo->triangles[i].v[1]  = s.tris[i * 4 + 2];
            geo->triangles[i].v[2]  = s.tris[i * 4 + 3];
        }
        AppendMaterials(geo, mats);   // matId is a world-global index
        FinishGeometry(geo);

        if (rw::Atomic* a = MakeAtomic(geo, worldFrame)) {
            // [rtex] a sector holding a DUAL material renders through the MatFX
            // pipeline, as the original's sector draws through FUN_00543500.
            if (uses_dual) rw::MatFX::enableEffects(a);
            sectors->addAtomic(a);
            if (sector_atomics) (*sector_atomics)[sector_index] = a;   // [rstates]
        }
    }
    // D-S3-BANK probe: per-material triangle totals across all sectors. The D3D9
    // path logs the same tally from batches_, so a material that gets a different
    // number of triangles on the two paths -- or none at all -- is visible as a
    // number instead of being argued about from a screenshot.
    {
        std::vector<std::size_t> per_mat(world.materials.size(), 0);
        for (const Track::Sector& s : world.sectors) {
            const std::size_t nt = s.tris.size() / 4;
            for (std::size_t i = 0; i < nt; ++i) {
                const std::uint16_t m = s.tris[i * 4 + 0];
                if (m < per_mat.size()) ++per_mat[m];
            }
        }
        for (std::size_t i = 0; i < per_mat.size(); ++i)
            SLog("  world.tris mat[%zu]=%zu", i, per_mat[i]);
    }
    rww->addClump(sectors);
    return rww;
}

// [geomlight 2026-08-30] Per RenderWare, an atomic whose geometry lacks
// rpGEOMETRYLIGHT (0x20) receives NO runtime lighting -- its prelit is the final
// colour. The TRAINING ground ROAD.DFF (all 21 geos flags=0x2008b) and the water
// props (LAKE/WATER0x.DFF flags=0x1000f) carry no rpGEOMETRYLIGHT and no normals,
// so DffModel sets b.lit=false (DffModel.cpp:186/:346) and the original engine
// adds them NO ambient. BuildClump already honours this for librw's OWN pipeline:
// with b.lit false it never sets rw::Geometry::LIGHT (:468), so lightingCB_Shader
// never runs and the g_amb light (RwRaceSubmit.cpp:555) cannot touch these
// atomics. The over-brightness came from OUR manual [D-S3-6] fold below, which
// injected amb_world_ straight into the prelit -- the librw analogue of the D3D9
// LightAtomicVertex non-lit fill (TrackRenderer.cpp:252). Both were the same
// defect. Default now SKIPS the fold so non-lit prelit renders with its authored
// prelit alone, matching the asset flag. MASHED_LIBRW_AMBFOLD=1 restores the old
// fold for A/B measurement.
static bool AmbientFoldEnabled() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_LIBRW_AMBFOLD");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    return on;
}

// [geomlight class-scope 2026-08-31] The blanket fold (AMBFOLD=1) restores ambient
// for BOTH the road (ROAD.DFF 0x2008b) and the water (LAKE/WATER0x.DFF 0x1000f).
// Measured against the pose-matched Arctic reference (verify/arctic_ref): the
// original keeps the road DARK (no ambient -> TRAINING 18.47->15.45 came from
// dropping the fold there) but the SEA BRIGHT (sea luma ~28-33; fold-off crushes it
// to ~9). So the correct behaviour is scoped: fold water, not road.
//
// DEFAULT OFF. It was defaulted ON earlier on 2026-08-31 on the strength of Arctic
// alone, and REVERTED the same day once Forest and SuperG got pose-matched references
// (verify/water_refs/RESULT.md). The fold does not generalise:
//
//   track       water mask   original   unfolded          folded
//   Arctic s8      69.63%       28.0    9.1  (d18.9)      29.8 (d1.8)
//   SuperG s14     41.78%      154.1    159.4 (d5.3)     190.8 (d36.7)
//   SuperG s8      26.89%      148.0    151.9 (d3.9)     187.8 (d39.8)
//   Forest s8       4.32%       21.6    44.1 (d22.5)      63.3 (d41.7)
//
// Arctic's water is far too DARK unfolded and the fold rescues it; SuperG's is already
// right unfolded (within 4-5 luma) and the fold pushes it 37-40 AWAY. Same code, same
// surface class, opposite verdicts -- so "non-lit prelit water needs ambient folded in"
// is NOT the rule. The fold was fitted to Arctic. Off by default is correct on 2 of the
// 3 referenced water tracks; the cost is that the ARCTIC SEA SHIPS TOO DARK (luma ~9
// against the original's ~28), which is a known, filed defect and not an oversight.
//
// The real mechanism is unresolved and is NOT this: no track declares Ambient_RGB in
// COURSE.LUA, all declare Lights_Filename("Lights.dff"), and the decompiled course
// loader 0x00479330 loads that clump and calls RpWorldAddLight per light -- i.e. the
// original lights these surfaces with real RenderWare lights, which this manual prelit
// fold only imitates. How those lights reach a non-rpGEOMETRYLIGHT atomic has not been
// read, and until it is, no fold setting is right everywhere.
//
// MASHED_LIBRW_AMBFOLD_SEA=1 enables the water fold (restores the Arctic sea, breaks
// Forest/SuperG). MASHED_LIBRW_AMBFOLD=1 is the older blanket road+water fold.
static bool AmbientFoldScopedSea() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_LIBRW_AMBFOLD_SEA");
        return e && e[0] == '1' && e[1] == '\0';   // default OFF; only "1" enables
    }();
    return on;
}

// The geometry-flag half of the scope key: water carries no rpGEOMETRYLIGHT and
// one texcoord set (0x1000f, bits 16-23 == 1); the road carries two (0x2008b).
static bool BatchIsWaterClass(std::uint32_t geo_flags) {
    return ((geo_flags >> 16) & 0xFFu) <= 1u;   // numTexCoordSets<=1
}

// [geomlight water-scope 2026-08-31] The ASSET-NAME half of the scope key, and the
// reason this exists: the flag half alone is NOT a water discriminator. The broad
// 13-track check (verify/geomlight_broadcheck/RESULT.md) found `numTexCoordSets<=1
// && !lit && prelit` is also satisfied by a green awning and a lamppost on City
// and by a skydome sliver on Dump, so the scoped fold over-brightened those props
// (City orig 73.9 -> geomSEA 148.4; Dump orig 53.5 -> 92.5) while correctly fixing
// the Arctic sea. Water surfaces live in single-purpose DFFs across all 13 tracks
// (piz listings, 2026-08-31):
//   Arctic SEA.DFF | City WATER.DFF | Forest WATER.DFF WATERFALL.DFF
//   sands WATER.DFF | Storm WATER.DFF | SuperG SEA.DFF
//   training LAKE.DFF WATER02.DFF WATER03.DFF | Warzone RIVER.DFF
//   Egypt/Highway/Neustein/rouabout/dump: none
// so the name is a sound key. MIST.DFF (Arctic, Storm) is deliberately NOT matched:
// it is a fog billboard, not a water surface, and folding ambient into it is the
// same additive over-bright defect. The two halves are ANDed, which makes this
// scope strictly NARROWER than the flags-only prototype -- every track the
// prototype was a no-op on stays a no-op.
static bool ModelIsWaterAsset(const char* name) {
    if (!name || !name[0]) return false;         // world path / car model
    char up[64];
    std::size_t n = 0;
    for (; n + 1 < sizeof(up) && name[n]; ++n) {
        const char c = name[n];
        up[n] = (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
    }
    up[n] = '\0';
    // Prefix match, not substring: the basename of a water asset always STARTS
    // with the class word (SEA/WATER/LAKE/RIVER). A substring test would drag in
    // anything merely containing them.
    return std::strncmp(up, "SEA",   3) == 0 ||
           std::strncmp(up, "WATER", 5) == 0 ||
           std::strncmp(up, "LAKE",  4) == 0 ||
           std::strncmp(up, "RIVER", 5) == 0;
}

void* BuildClump(const Track::DffModel& model, const TextureSource& tex,
                 std::uint32_t ambient,
                 std::vector<std::uint32_t>* out_atomic_mat,
                 std::vector<std::int32_t>* out_atomic_part,
                 bool hierarchy,
                 std::vector<void*>* out_frames,
                 std::vector<std::int32_t>* out_atomic_frame,
                 std::vector<std::int32_t>* out_atomic_dff) {
    if (out_atomic_mat) out_atomic_mat->clear();
    if (out_atomic_part) out_atomic_part->clear();
    if (out_frames) out_frames->clear();
    if (out_atomic_frame) out_atomic_frame->clear();
    if (out_atomic_dff) out_atomic_dff->clear();
    if (model.frames.empty()) hierarchy = false;
    // [geomlight water-scope] Model-level half of the ambient-fold scope key.
    // Evaluated once here, not per batch -- it depends only on the asset name.
    const bool water_asset = ModelIsWaterAsset(model.source_name);
    std::vector<rw::Material*> mats;
    int named = 0, resolved = 0;
    BuildMaterials(model.materials, tex, mats, &named, &resolved);
    // D-S3-6 instrumentation: one large ground/sea surface renders BLACK through
    // librw while banner/building props render correctly. Two candidate causes are
    // visible here and nowhere else -- a material whose texture failed to resolve
    // (draws with the material colour), and a material colour that is itself dark.
    // Log both per material so the black surface can be NAMED rather than guessed.
    {
        static int s_clump_no = 0;
        const int id = s_clump_no++;
        // [geomlight water-scope] name + water_asset are logged so a capture can
        // NAME the asset the fold fired on instead of inferring it from a mask.
        SLog("clump[%d]: dff='%s' water_asset=%d mats=%zu named=%d resolved=%d "
             "batches=%zu",
             id, model.source_name[0] ? model.source_name : "(unnamed)",
             (int)water_asset, model.materials.size(), named, resolved,
             model.batches.size());
        for (std::size_t i = 0; i < model.materials.size(); ++i) {
            const auto& m = model.materials[i];
            const bool has_tex = m.tex_name[0] != 0;
            const bool got_tex = has_tex && ResolveTexture(m.tex_name, tex) != nullptr;
            SLog("  clump[%d].mat[%zu] tex='%s' %s rgba=(%u,%u,%u,%u)",
                 id, i, has_tex ? m.tex_name : "(none)",
                 !has_tex ? "NO-TEXNAME" : (got_tex ? "resolved" : "*** UNRESOLVED ***"),
                 (unsigned)m.rgba[0], (unsigned)m.rgba[1],
                 (unsigned)m.rgba[2], (unsigned)m.rgba[3]);
        }
        for (std::size_t bi = 0; bi < model.batches.size(); ++bi) {
            const Track::DffBatch& b = model.batches[bi];
            SLog("  clump[%d].batch[%zu] mat=%u nv=%zu nt=%zu uv=%d prelit=%d "
                 "norm=%d lit=%d mod=%d prelit[0]=0x%08X",
                 id, bi, (unsigned)b.material, b.verts.size() / 3, b.tris.size() / 3,
                 (int)!b.uvs.empty(), (int)!b.prelit.empty(), (int)!b.normals.empty(),
                 (int)b.lit, (int)b.modulate_mat,
                 b.prelit.empty() ? 0u : (unsigned)b.prelit[0]);
        }
    }

    rw::Clump* clump = rw::Clump::create();
    if (!clump) return nullptr;
    rw::Frame* root = nullptr;
    // [librwrest] hierarchy mode: one rw::Frame per FRAMELIST entry, parented as streamed.
    std::vector<rw::Frame*> dff_frames;
    if (hierarchy) {
        dff_frames.resize(model.frames.size(), nullptr);
        for (std::size_t i = 0; i < model.frames.size(); ++i) {
            const Track::DffFrame& df = model.frames[i];
            rw::Frame* f = rw::Frame::create();
            rw::Matrix* m = &f->matrix;
            m->right.x = df.rot[0]; m->right.y = df.rot[1]; m->right.z = df.rot[2];
            m->up.x    = df.rot[3]; m->up.y    = df.rot[4]; m->up.z    = df.rot[5];
            m->at.x    = df.rot[6]; m->at.y    = df.rot[7]; m->at.z    = df.rot[8];
            m->pos.x   = df.pos[0]; m->pos.y   = df.pos[1]; m->pos.z   = df.pos[2];
            m->update();
            if (df.parent >= 0 && static_cast<std::size_t>(df.parent) < i && dff_frames[df.parent])
                dff_frames[df.parent]->addChild(f);
            else if (!root)
                root = f;
            else
                root->addChild(f);   // a second parentless entry hangs off the first root
            dff_frames[i] = f;
        }
        if (out_frames)
            for (rw::Frame* f : dff_frames) out_frames->push_back(f);
    }
    if (!root) root = rw::Frame::create();
    clump->setFrame(root);

    // [librwrest] the bind-pose chain of FRAMELIST entry `fi` as a column-vector affine map
    // (A, t): model = A * local + t, the composition DffModel.cpp FrameXform applies; and
    // its inverse, to take a batch's baked vertices back to its frame's local space.
    auto chain = [&](int fi, double A[9], double t[3]) {
        for (int k = 0; k < 9; ++k) A[k] = (k % 4 == 0) ? 1.0 : 0.0;   // row-major 3x3
        t[0] = t[1] = t[2] = 0.0;
        for (int i = fi; i >= 0; i = model.frames[static_cast<std::size_t>(i)].parent) {
            const Track::DffFrame& f = model.frames[static_cast<std::size_t>(i)];
            // R (row-major) has columns right/up/at: R[r][c] = rot[c*3 + r]
            double R[9];
            for (int r = 0; r < 3; ++r)
                for (int c = 0; c < 3; ++c) R[r * 3 + c] = f.rot[c * 3 + r];
            double nA[9], nt[3];
            for (int r = 0; r < 3; ++r) {
                for (int c = 0; c < 3; ++c)
                    nA[r * 3 + c] = R[r * 3 + 0] * A[0 * 3 + c] + R[r * 3 + 1] * A[1 * 3 + c] +
                                    R[r * 3 + 2] * A[2 * 3 + c];
                nt[r] = R[r * 3 + 0] * t[0] + R[r * 3 + 1] * t[1] + R[r * 3 + 2] * t[2] + f.pos[r];
            }
            for (int k = 0; k < 9; ++k) A[k] = nA[k];
            for (int k = 0; k < 3; ++k) t[k] = nt[k];
        }
    };
    auto invert3 = [](const double a[9], double o[9]) {
        const double c00 = a[4] * a[8] - a[5] * a[7], c01 = a[5] * a[6] - a[3] * a[8],
                     c02 = a[3] * a[7] - a[4] * a[6];
        const double det = a[0] * c00 + a[1] * c01 + a[2] * c02;
        const double id = (det != 0.0) ? 1.0 / det : 0.0;
        o[0] = c00 * id; o[1] = (a[2] * a[7] - a[1] * a[8]) * id; o[2] = (a[1] * a[5] - a[2] * a[4]) * id;
        o[3] = c01 * id; o[4] = (a[0] * a[8] - a[2] * a[6]) * id; o[5] = (a[2] * a[3] - a[0] * a[5]) * id;
        o[6] = c02 * id; o[7] = (a[1] * a[6] - a[0] * a[7]) * id; o[8] = (a[0] * a[4] - a[1] * a[3]) * id;
    };

    std::size_t bi = static_cast<std::size_t>(-1);
    for (const Track::DffBatch& b0 : model.batches) {
        ++bi;
        const std::int32_t nv = static_cast<std::int32_t>(b0.verts.size() / 3);
        const std::int32_t nt = static_cast<std::int32_t>(b0.tris.size() / 3);
        if (nv == 0 || nt == 0) continue;
        // [librwrest] hierarchy mode: this batch's vertices/normals in its frame's space
        const bool own_frame = hierarchy && b0.frame >= 0 &&
                               static_cast<std::size_t>(b0.frame) < dff_frames.size();
        Track::DffBatch local;
        if (own_frame) {
            double A[9], t[3], Ai[9];
            chain(b0.frame, A, t);
            invert3(A, Ai);
            local.verts.resize(b0.verts.size());
            for (std::int32_t i = 0; i < nv; ++i) {
                const double d[3] = { b0.verts[i * 3 + 0] - t[0], b0.verts[i * 3 + 1] - t[1],
                                      b0.verts[i * 3 + 2] - t[2] };
                for (int r = 0; r < 3; ++r)
                    local.verts[i * 3 + r] = static_cast<float>(
                        Ai[r * 3 + 0] * d[0] + Ai[r * 3 + 1] * d[1] + Ai[r * 3 + 2] * d[2]);
            }
            local.normals.resize(b0.normals.size());
            for (std::size_t i = 0; i + 2 < b0.normals.size(); i += 3)
                for (int r = 0; r < 3; ++r)
                    local.normals[i + r] = static_cast<float>(
                        Ai[r * 3 + 0] * b0.normals[i] + Ai[r * 3 + 1] * b0.normals[i + 1] +
                        Ai[r * 3 + 2] * b0.normals[i + 2]);
        }
        struct BatchView {
            const Track::DffBatch& src; const std::vector<float>& verts;
            const std::vector<float>& normals;
        } bv{ b0, own_frame ? local.verts : b0.verts, own_frame ? local.normals : b0.normals };
        const Track::DffBatch& b = bv.src;

        // [rtex] two texcoord sets when the geometry declares them and DUAL is on.
        const bool two_sets = DualEnabled() && !b.uvs2.empty();
        const bool uses_dual = b.material < model.materials.size() &&
                               IsDualMaterial(model.materials[b.material]);
        std::uint32_t flags = rw::Geometry::POSITIONS;
        if (!b.uvs.empty())     flags |= TexFlags(two_sets ? 2 : 1);
        if (!b.prelit.empty())  flags |= rw::Geometry::PRELIT;
        if (!b.normals.empty()) flags |= rw::Geometry::NORMALS;
        // I5: these two map 1:1 onto RW's own geometry flags, which is why the
        // parser records them -- rpGEOMETRYLIGHT and
        // rpGEOMETRYMODULATEMATERIALCOLOR (DffModel.h).
        if (b.lit)          flags |= rw::Geometry::LIGHT;
        if (b.modulate_mat) flags |= rw::Geometry::MODULATE;

        rw::Geometry* geo = rw::Geometry::create(nv, nt, flags);
        if (!geo) continue;
        // [D-S3-6 FIX] Fold the track ambient into prelit for non-LIGHT batches.
        // See the header note: this is what the D3D9 bake does, and librw's
        // lighting provably cannot supply it here (no normals => no LIGHT flag =>
        // lightingCB_Shader takes the setAmbient(black) branch).
        std::vector<std::uint32_t> prelit_amb;
        const std::vector<std::uint32_t>* prelit_src = &b.prelit;
        // Fold gate: blanket (AMBFOLD=1, road+water) OR water-scoped (water ASSET
        // and water flag class), the latter being the DEFAULT. AMBFOLD_SEA=0 turns
        // the water fold off, which is the "no fold at all" arm.
        const bool fold_this =
            ambient && !b.prelit.empty() && !b.lit &&
            (AmbientFoldEnabled() ||
             (AmbientFoldScopedSea() && water_asset &&
              BatchIsWaterClass(b.geo_flags)));
        if (fold_this) {
            // CHANNEL ORDER, and it bit once. `ambient` (amb_world_) is
            // 0x00RRGGBB, but DffModel prelit is RW-native RGBA bytes, i.e.
            // 0xAABBGGRR -- FillVertexData below reads red from the LOW byte and
            // blue from bits 16-23. The first version of this bake used the ARGB
            // layout for both, so it added the ambient's RED to the blue channel
            // and its BLUE to the red channel. With Arctic's (51,77,77) that
            // pushed red up and blue down and turned the sea olive where the D3D9
            // baseline is dark teal. Unpack each side in its own convention.
            const unsigned ar = (ambient >> 16) & 0xFF;   // ambient is 0x00RRGGBB
            const unsigned ag = (ambient >> 8)  & 0xFF;
            const unsigned ab = (ambient)       & 0xFF;
            prelit_amb.resize(b.prelit.size());
            for (std::size_t i = 0; i < b.prelit.size(); ++i) {
                const std::uint32_t c = b.prelit[i];       // prelit is 0xAABBGGRR
                unsigned r = ((c)       & 0xFF) + ar; if (r > 255) r = 255;
                unsigned g = ((c >> 8)  & 0xFF) + ag; if (g > 255) g = 255;
                unsigned bl = ((c >> 16) & 0xFF) + ab; if (bl > 255) bl = 255;
                prelit_amb[i] = (c & 0xFF000000u) | (bl << 16) | (g << 8) | r;
            }
            prelit_src = &prelit_amb;
        }
        // D-S3-SEA probe: the value librw actually UPLOADS for vertex 0, to be
        // compared against the D3D9 path's `D-S3-6 bake: baked_v0` for the same
        // vertex. Both are the final per-vertex colour their renderer starts
        // from, so if they agree the 1.5x sea divergence is downstream of the
        // bake (pipeline/shader) and if they differ it is the bake itself.
        // Printed in RW byte order (0xAABBGGRR), same as `prelit[0]` above.
        if (!b.prelit.empty()) {
            SLog("  UPLOAD mat=%u lit=%d mod=%d amb=0x%06X raw=0x%08X -> up=0x%08X",
                 (unsigned)b.material, (int)b.lit, (int)b.modulate_mat,
                 (unsigned)ambient, (unsigned)b.prelit[0],
                 (unsigned)(*prelit_src)[0]);
        }
        FillVertexData(geo, nv, bv.verts, b.uvs, *prelit_src, &bv.normals);
        if (two_sets) FillTexCoords1(geo, nv, b.uvs2);
        for (std::int32_t i = 0; i < nt; ++i) {
            // DffBatch::tris is v0,v1,v2 -- material is per BATCH, not per tri.
            geo->triangles[i].matId =
                static_cast<rw::uint16>(b.material);
            geo->triangles[i].v[0] = b.tris[i * 3 + 0];
            geo->triangles[i].v[1] = b.tris[i * 3 + 1];
            geo->triangles[i].v[2] = b.tris[i * 3 + 2];
        }
        AppendMaterials(geo, mats);
        FinishGeometry(geo);
        // [librwrest] hierarchy mode: the geometry keeps the sphere the DFF streams (its own
        // space, like the un-baked vertices), as the RW geometry does
        if (own_frame && b.sphere[3] > 0.f && geo->numMorphTargets > 0) {
            geo->morphTargets[0].boundingSphere.center.x = b.sphere[0];
            geo->morphTargets[0].boundingSphere.center.y = b.sphere[1];
            geo->morphTargets[0].boundingSphere.center.z = b.sphere[2];
            geo->morphTargets[0].boundingSphere.radius   = b.sphere[3];
        }

        rw::Atomic* a = nullptr;
        if (own_frame) {   // [librwrest] the atomic's own DFF frame
            a = rw::Atomic::create();
            if (a) {
                a->setFrame(dff_frames[static_cast<std::size_t>(b.frame)]);
                a->setGeometry(geo, 0);
            }
        } else {
            a = MakeAtomic(geo, root);
        }
        if (a) {
            if (uses_dual) rw::MatFX::enableEffects(a);   // [rtex] FUN_00543500 path
            clump->addAtomic(a);
            // Record the material HERE, where the atomic is actually created --
            // batches that produced none (nv==0 || nt==0, skipped above) must not
            // shift the mapping.
            if (out_atomic_mat) out_atomic_mat->push_back(b.material);
            if (out_atomic_part) out_atomic_part->push_back(b.part_id);   // [rtex]
            if (out_atomic_frame) out_atomic_frame->push_back(own_frame ? b.frame : -1);
            if (out_atomic_dff) out_atomic_dff->push_back(b.atomic);      // [rstates]
        }
    }
    return clump;
}

// ---------------------------------------------------------------------------
// Self-test
// ---------------------------------------------------------------------------
namespace {

const char* const kSceneLog = "log/librw_scene.txt";

void SLog(const char* fmt, ...) {
    std::FILE* f = std::fopen(kSceneLog, "a");
    if (!f) return;
    va_list ap; va_start(ap, fmt);
    std::vfprintf(f, fmt, ap); va_end(ap);
    std::fputc('\n', f); std::fclose(f);
}

const std::uint8_t* FindEntry(mashed_re::Piz::Archive& ar, const char* suffix,
                              std::uint32_t* len) {
    const std::size_t sl = std::strlen(suffix);
    for (std::uint32_t i = 0; i < ar.count(); ++i) {
        const char* n = ar.entry(i).name;
        const std::size_t nl = std::strlen(n);
        if (nl >= sl && _stricmp(n + nl - sl, suffix) == 0)
            return ar.blob(i, len);
    }
    return nullptr;
}

}  // namespace

// Write the current D3D9 backbuffer to a 24-bit BMP. Self-contained (no D3DX).
static bool DumpBackbuffer(const char* path) {
    IDirect3DDevice9* dev = rw::d3d::d3ddevice;
    if (!dev) return false;
    IDirect3DSurface9* rt = nullptr;
    if (FAILED(dev->GetRenderTarget(0, &rt)) || !rt) return false;
    D3DSURFACE_DESC de; rt->GetDesc(&de);
    IDirect3DSurface9* sys = nullptr;
    if (FAILED(dev->CreateOffscreenPlainSurface(de.Width, de.Height, de.Format,
                                                D3DPOOL_SYSTEMMEM, &sys, nullptr))) {
        rt->Release(); return false;
    }
    bool ok = false;
    if (SUCCEEDED(dev->GetRenderTargetData(rt, sys))) {
        D3DLOCKED_RECT lr;
        if (SUCCEEDED(sys->LockRect(&lr, nullptr, D3DLOCK_READONLY))) {
            const int w = (int)de.Width, h = (int)de.Height;
            const int rowsz = (w * 3 + 3) & ~3;
            const int imgsz = rowsz * h;
            std::uint8_t fh[14] = {'B','M'};
            const std::uint32_t total = 14 + 40 + imgsz, off = 54;
            std::memcpy(fh + 2, &total, 4); std::memcpy(fh + 10, &off, 4);
            std::uint8_t ih[40] = {}; const std::uint32_t hs = 40; const std::uint16_t pl = 1, bc = 24;
            std::memcpy(ih + 0, &hs, 4); std::memcpy(ih + 4, &w, 4); std::memcpy(ih + 8, &h, 4);
            std::memcpy(ih + 12, &pl, 2); std::memcpy(ih + 14, &bc, 2);
            std::memcpy(ih + 20, &imgsz, 4);
            if (std::FILE* f = std::fopen(path, "wb")) {
                std::fwrite(fh, 1, 14, f); std::fwrite(ih, 1, 40, f);
                std::vector<std::uint8_t> row((std::size_t)rowsz, 0);
                for (int y = h - 1; y >= 0; --y) {           // BMP is bottom-up
                    const std::uint8_t* src = (const std::uint8_t*)lr.pBits + (std::size_t)y * lr.Pitch;
                    for (int x = 0; x < w; ++x) {
                        row[(std::size_t)x * 3 + 0] = src[x * 4 + 0];  // B
                        row[(std::size_t)x * 3 + 1] = src[x * 4 + 1];  // G
                        row[(std::size_t)x * 3 + 2] = src[x * 4 + 2];  // R
                    }
                    std::fwrite(row.data(), 1, (std::size_t)rowsz, f);
                }
                std::fclose(f); ok = true;
            }
            sys->UnlockRect();
        }
    }
    sys->Release(); rt->Release();
    return ok;
}

int RenderWorldProbe(int width, int height, const char* out_bmp) {
    SLog("# world render probe (E2'b step 2) -- STATIC WORLD ONLY, not a parity shot");
    const char* kPiz = "original/TOASTART/TRACKS/Arctic.piz";
    mashed_re::Piz::Archive ar;
    if (!ar.Load(kPiz)) { SLog("FAIL: piz %s", ar.last_error()); return 1; }
    std::uint32_t bl = 0, tl = 0;
    const std::uint8_t* bsp = FindEntry(ar, "GRAPH.BSP", &bl);
    const std::uint8_t* txd = FindEntry(ar, "TEXTURES.TXD", &tl);
    if (!bsp || !txd) { SLog("FAIL: missing GRAPH.BSP or TEXTURES.TXD"); return 2; }

    static Txd::Dictionary dict;
    static Track::World world;
    if (!dict.Decode(txd, tl))   { SLog("FAIL: TXD %s", dict.last_error());  return 3; }
    if (!world.Parse(bsp, bl))   { SLog("FAIL: BSP %s", world.last_error()); return 4; }

    TextureSource ts{ &dict, 1 };
    rw::World* rww = static_cast<rw::World*>(BuildWorld(world, ts));
    if (!rww) { SLog("FAIL: BuildWorld"); return 5; }

    // Deterministic overview camera from the world bbox (sup.xyz, inf.xyz).
    const float cx = (world.bbox[0] + world.bbox[3]) * 0.5f;
    const float cy = (world.bbox[1] + world.bbox[4]) * 0.5f;
    const float cz = (world.bbox[2] + world.bbox[5]) * 0.5f;
    float ex = world.bbox[0] - world.bbox[3];
    float ey = world.bbox[1] - world.bbox[4];
    float ez = world.bbox[2] - world.bbox[5];
    if (ex < 0) ex = -ex; if (ey < 0) ey = -ey; if (ez < 0) ez = -ez;
    float radius = ex; if (ey > radius) radius = ey; if (ez > radius) radius = ez;
    if (radius < 1.f) radius = 1.f;
    SLog("world bbox centre=(%.2f,%.2f,%.2f) radius=%.2f", cx, cy, cz, radius);

    rw::Camera* cam = rw::Camera::create();
    if (!cam) { SLog("FAIL: Camera::create"); return 6; }
    rw::Frame* cf = rw::Frame::create();
    cam->setFrame(cf);
    cam->frameBuffer = rw::Raster::create(width, height, 0, rw::Raster::CAMERA);
    cam->zBuffer     = rw::Raster::create(width, height, 0, rw::Raster::ZBUFFER);
    { rw::V2d vw; vw.x = 1.0f; vw.y = (float)height / (float)width; cam->setViewWindow(&vw); }
    cam->setNearPlane(radius * 0.01f);
    cam->setFarPlane(radius * 6.0f);

    // Look-at: eye above and to one side of the centre, aimed at it.
    const float eye[3] = { cx + radius * 0.9f, cy + radius * 0.7f, cz + radius * 0.9f };
    float at[3] = { cx - eye[0], cy - eye[1], cz - eye[2] };
    float len = std::sqrt(at[0]*at[0] + at[1]*at[1] + at[2]*at[2]);
    if (len < 1e-6f) len = 1.f;
    at[0] /= len; at[1] /= len; at[2] /= len;
    const float wup[3] = { 0.f, 1.f, 0.f };
    float right[3] = { wup[1]*at[2] - wup[2]*at[1],
                       wup[2]*at[0] - wup[0]*at[2],
                       wup[0]*at[1] - wup[1]*at[0] };
    float rl = std::sqrt(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    if (rl < 1e-6f) rl = 1.f;
    right[0] /= rl; right[1] /= rl; right[2] /= rl;
    const float up[3] = { at[1]*right[2] - at[2]*right[1],
                          at[2]*right[0] - at[0]*right[2],
                          at[0]*right[1] - at[1]*right[0] };
    rw::Matrix* m = &cf->matrix;
    m->right.x = right[0]; m->right.y = right[1]; m->right.z = right[2];
    m->up.x    = up[0];    m->up.y    = up[1];    m->up.z    = up[2];
    m->at.x    = at[0];    m->at.y    = at[1];    m->at.z    = at[2];
    m->pos.x   = eye[0];   m->pos.y   = eye[1];   m->pos.z   = eye[2];
    m->update();
    cf->updateObjects();

    rw::RGBA sky = { 40, 48, 64, 255 };   // distinguishable from both black and the terrain
    cam->clear(&sky, rw::Camera::CLEARIMAGE | rw::Camera::CLEARZ);
    cam->beginUpdate();
    rww->render();
    cam->endUpdate();

    const bool dumped = DumpBackbuffer(out_bmp);
    cam->showRaster(0);
    SLog("rendered world -> %s (%s)", out_bmp, dumped ? "dumped" : "DUMP FAILED");
    return dumped ? 0 : 7;
}

int SceneBuild_SelfTest() {
    if (std::FILE* f = std::fopen(kSceneLog, "w")) std::fclose(f);
    SLog("# librw scene build self-test (E2'b)");

    const char* kPiz = "original/TOASTART/TRACKS/Arctic.piz";
    mashed_re::Piz::Archive ar;
    if (!ar.Load(kPiz)) { SLog("FAIL: piz load %s: %s", kPiz, ar.last_error()); return 1; }

    std::uint32_t blen = 0, tlen = 0;
    const std::uint8_t* bsp = FindEntry(ar, "GRAPH.BSP", &blen);
    const std::uint8_t* txd = FindEntry(ar, "TEXTURES.TXD", &tlen);
    if (!bsp) { SLog("FAIL: no GRAPH.BSP in %s", kPiz); return 2; }
    if (!txd) { SLog("FAIL: no TEXTURES.TXD in %s", kPiz); return 3; }

    static Txd::Dictionary dict;
    if (!dict.Decode(txd, tlen)) { SLog("FAIL: TXD: %s", dict.last_error()); return 4; }

    static Track::World world;
    if (!world.Parse(bsp, blen)) { SLog("FAIL: BSP: %s", world.last_error()); return 5; }
    SLog("parsed: %zu materials, %zu sectors, total_verts=%u total_tris=%u",
         world.materials.size(), world.sectors.size(),
         world.total_verts, world.total_tris);

    TextureSource ts{ &dict, 1 };

    // Count texture bindings independently of BuildWorld so the check is not
    // just the builder agreeing with itself.
    int named = 0, resolved = 0;
    {
        std::vector<rw::Material*> probe;
        BuildMaterials(world.materials, ts, probe, &named, &resolved);
        for (rw::Material* m : probe) if (m) m->destroy();
    }
    SLog("materials with a texture name: %d, resolved against the TXD: %d",
         named, resolved);

    rw::World* rww = static_cast<rw::World*>(BuildWorld(world, ts));
    if (!rww) { SLog("FAIL: BuildWorld returned null"); return 6; }

    // Walk the built world and compare against the PARSER's own totals. This is
    // the non-degenerate part: a builder that dropped a sector, mis-strided the
    // vertex array or mis-sliced the (mat,v0,v1,v2) quads would not add up.
    std::int32_t atomics = 0, verts = 0, tris = 0;
    FORLIST(clnk, rww->clumps) {
        rw::Clump* c = rw::Clump::fromWorld(clnk);
        FORLIST(alnk, c->atomics) {
            rw::Atomic* a = rw::Atomic::fromClump(alnk);
            rw::Geometry* g = a->geometry;
            if (!g) continue;
            ++atomics;
            verts += g->numVertices;
            tris  += g->numTriangles;
        }
    }
    SLog("built: %d atomics, %d verts, %d tris  (%zu sectors parsed; %zu skipped "
         "as empty -- the totals below prove nothing was dropped)",
         atomics, verts, tris, world.sectors.size(),
         world.sectors.size() - static_cast<std::size_t>(atomics));

    // Counts alone would pass a builder that transposed indices or mis-sliced the
    // (mat,v0,v1,v2) quads in a length-preserving way, so validate the CONTENT of
    // every triangle too: indices must address this geometry's vertices, and every
    // matId must address the material list.
    std::int32_t badIdx = 0, badMat = 0;
    FORLIST(clnk2, rww->clumps) {
        rw::Clump* c = rw::Clump::fromWorld(clnk2);
        FORLIST(alnk2, c->atomics) {
            rw::Geometry* g = rw::Atomic::fromClump(alnk2)->geometry;
            if (!g) continue;
            for (std::int32_t i = 0; i < g->numTriangles; ++i) {
                const rw::Triangle& tr = g->triangles[i];
                for (int k = 0; k < 3; ++k)
                    if (tr.v[k] >= g->numVertices) ++badIdx;
                if (tr.matId >= g->matList.numMaterials) ++badMat;
            }
        }
    }
    SLog("triangle validation: %d out-of-range vertex indices, %d out-of-range matIds",
         badIdx, badMat);

    int rc = 0;
    if (badIdx) { SLog("FAIL: %d out-of-range vertex indices", badIdx); rc = 10; }
    if (badMat) { SLog("FAIL: %d out-of-range matIds", badMat); rc = 11; }
    if (verts != static_cast<std::int32_t>(world.total_verts)) {
        SLog("FAIL: vertex total %d != parser total %u", verts, world.total_verts);
        rc = 7;
    }
    if (tris != static_cast<std::int32_t>(world.total_tris)) {
        SLog("FAIL: triangle total %d != parser total %u", tris, world.total_tris);
        rc = 8;
    }
    if (named > 0 && resolved == 0) {
        SLog("FAIL: %d materials name a texture but none resolved", named);
        rc = 9;
    }
    if (rc == 0) SLog("RESULT: PASS");
    return rc;
}

}  // namespace LibRw
}  // namespace mashed_re
