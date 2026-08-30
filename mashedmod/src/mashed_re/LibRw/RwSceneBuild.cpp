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
        out.push_back(mat);
    }
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

// D-S3-BANK: material-ID visualisation. With MASHED_WORLD_MATID=1 the world is
// drawn as flat per-material colours instead of shaded texture, on BOTH paths
// (TrackRenderer does the same via D3DRS_TEXTUREFACTOR), so the question "which
// material owns these pixels" is read off the image rather than inferred. Colour
// for material i is (20 + i*18, 200, 60) -- i is recoverable as (R-20)/18.
bool WorldMatIdMode() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_WORLD_MATID");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    return on;
}
// MASHED_WORLD_ONLYMAT=N: keep only world material N's triangles, collapsing the
// rest to degenerates (v0=v1=v2) so vertex indices, matIds and mesh structure
// stay exactly as in the real build -- the probe then measures the real geometry
// rather than a rebuilt one. BuildWorld logs the surviving non-degenerate count
// per material; a run whose log does not show only material N is void.
int WorldOnlyMat() {
    static const int v = [] {
        const char* e = std::getenv("MASHED_WORLD_ONLYMAT");
        return (e && *e) ? std::atoi(e) : -1;
    }();
    return v;
}
// MASHED_WORLD_PRELITONLY=1: strip world material textures so the librw PS TEX
// path is off and Color = interpolated prelit alone (matCol stays white; no
// MODULATE). Paired with the D3D9 SELECTARG2 branch, this isolates vertex-colour
// interpolation from texture/UV sampling in the D-S3-BANK diff.
bool WorldPrelitOnly() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_WORLD_PRELITONLY");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    return on;
}
// [D-S3-BANK discriminator] MASHED_WORLD_FLATSNOW=1: force EVERY world vertex
// colour to a single constant grey (180,180,180) on both paths. Meant to run WITH
// MASHED_WORLD_ONLYMAT=4 so only Snow draws: with no colour gradient left to
// interpolate, an FF-DIFFUSE-vs-VS-COLOR0 attribute-iteration difference (which
// only shows across a gradient) must VANISH, while a transform/coverage difference
// (which moves edge pixels regardless of colour) would survive as a thin edge.
// Separates the two remaining FF-vs-shader candidates by OBSERVATION.
// Returns the flat grey LEVEL (0..255) to force, or -1 when off. The value of the
// env var IS the level, so a sweep (=32,=64,...,=250) fits the D3D9 FF output
// transfer curve per channel. `=1` therefore means grey level 1, not "on".
int WorldFlatSnowLevel() {
    static const int lv = [] {
        const char* e = std::getenv("MASHED_WORLD_FLATSNOW");
        if (!e || !*e) return -1;
        int v = std::atoi(e);
        if (v < 0) v = 0; if (v > 255) v = 255;
        return v;
    }();
    return lv;
}
bool WorldFlatSnow() { return WorldFlatSnowLevel() >= 0; }
// MASHED_WORLD_VDUMP=1: dump every world triangle's 3 corners as
// "matId x y z r g b" to log/vdump_librw.txt (the D3D9 path writes vdump_d3d9.txt
// the same way). Lets a per-vertex prelit readback be matched by position across
// the two paths -- the last open D-S3-BANK candidate (does the packed prelit that
// reaches the rasterizer differ?). Run WITHOUT ONLYMAT so triangles are real.
bool WorldVDump() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_WORLD_VDUMP");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    return on;
}
// [D-S3-PROP] MASHED_PROP_VDUMP=<handle>: which registered model to dump every
// vertex colour for. Keyed on the librw model HANDLE so it names exactly the same
// model as MASHED_LIBRW_ONLYPROP and as the D3D9 side's `p->rw_model` -- not on a
// clump counter, which would silently drift from the handle if any BuildClump
// failed.
static int s_registering_handle = -1;

int PropVDumpHandle() {
    static const int h = [] {
        const char* e = std::getenv("MASHED_PROP_VDUMP");
        return (e && *e) ? std::atoi(e) : -1;
    }();
    return h;
}

void SceneBuild_SetRegisteringHandle(int h) {
    s_registering_handle = h;
    // Truncate at the start of the targeted model's build; the per-batch writes
    // below append, so without this a re-registration would concatenate.
    if (h >= 0 && h == PropVDumpHandle()) {
        if (std::FILE* f = std::fopen("log/pvdump_librw.txt", "w")) std::fclose(f);
    }
}

void WorldMatIdColour(std::size_t i, std::uint8_t* rgb) {
    rgb[0] = static_cast<std::uint8_t>(20 + i * 18);
    rgb[1] = 200;
    rgb[2] = 60;
}

void* BuildWorld(const Track::World& world, const TextureSource& tex) {
    std::vector<rw::Material*> mats;
    int w_named = 0, w_resolved = 0;
    BuildMaterials(world.materials, tex, mats, &w_named, &w_resolved);
    if (WorldMatIdMode()) {
        // Strip the texture and paint the material its ID colour. Combined with
        // the white vertex colours and the MODULATE flag set below, the shader
        // reduces to Color = white * matCol = the ID colour, flat.
        for (std::size_t i = 0; i < mats.size(); ++i) {
            if (!mats[i]) continue;
            mats[i]->setTexture(nullptr);
            std::uint8_t c[3];
            WorldMatIdColour(i, c);
            mats[i]->color.red = c[0];
            mats[i]->color.green = c[1];
            mats[i]->color.blue = c[2];
            mats[i]->color.alpha = 255;
        }
        SLog("world: MATID mode -- flat per-material colours, (20+i*18, 200, 60)");
    }
    if (WorldPrelitOnly()) {
        // Strip every world material's texture; keep matCol white and PRELIT on.
        // PS #ifdef TEX is then off, so Color == interpolated prelit vertex colour.
        int stripped = 0;
        for (std::size_t i = 0; i < mats.size(); ++i) {
            if (!mats[i]) continue;
            if (mats[i]->texture) ++stripped;
            mats[i]->setTexture(nullptr);
        }
        SLog("world: PRELITONLY mode -- textures stripped from %d materials", stripped);
    }
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

    const int only_mat = WorldOnlyMat();
    if (WorldVDump()) { if (std::FILE* f = std::fopen("log/vdump_librw.txt", "w")) std::fclose(f); }
    std::vector<std::size_t> kept(world.materials.size(), 0);
    for (const Track::Sector& s : world.sectors) {
        const std::int32_t nv = static_cast<std::int32_t>(s.verts.size() / 3);
        const std::int32_t nt = static_cast<std::int32_t>(s.tris.size() / 4);
        if (nv == 0 || nt == 0) continue;

        std::uint32_t flags = rw::Geometry::POSITIONS;
        if (!s.uvs.empty())    flags |= rw::Geometry::TEXTURED;
        if (!s.prelit.empty()) flags |= rw::Geometry::PRELIT;
        // Deliberately no NORMALS/LIGHT: the static world carries no vertex
        // normals, so its baked prelight IS the lighting -- same as D3D9 today.

        // MATID: matCol only reaches the shader when the geometry carries
        // MODULATE (rwd3d.h:321-328 hands setMaterial white otherwise), so the
        // flag is required for the ID colour to show at all.
        if (WorldMatIdMode()) flags |= rw::Geometry::MODULATE;

        rw::Geometry* geo = rw::Geometry::create(nv, nt, flags);
        if (!geo) continue;
        FillVertexData(geo, nv, s.verts, s.uvs, s.prelit, nullptr);
        if (WorldMatIdMode() && geo->colors)
            for (std::int32_t i = 0; i < nv; ++i)
                geo->colors[i] = rw::RGBA{255, 255, 255, 255};
        if (WorldFlatSnow() && geo->colors) {
            const auto g = static_cast<rw::uint8>(WorldFlatSnowLevel());
            for (std::int32_t i = 0; i < nv; ++i)
                geo->colors[i] = rw::RGBA{g, g, g, 255};
        }
        for (std::int32_t i = 0; i < nt; ++i) {
            // Track::Sector::tris is (mat, v0, v1, v2) -- TrackWorld.h:30.
            const std::uint16_t tmat = s.tris[i * 4 + 0];
            geo->triangles[i].matId = tmat;
            if (only_mat >= 0 && (int)tmat != only_mat) {
                geo->triangles[i].v[0] = geo->triangles[i].v[1] =
                    geo->triangles[i].v[2] = s.tris[i * 4 + 1];
                continue;
            }
            if (tmat < kept.size()) ++kept[tmat];
            geo->triangles[i].v[0]  = s.tris[i * 4 + 1];
            geo->triangles[i].v[1]  = s.tris[i * 4 + 2];
            geo->triangles[i].v[2]  = s.tris[i * 4 + 3];
        }
        if (WorldVDump() && geo->colors && geo->morphTargets[0].vertices) {
            if (std::FILE* f = std::fopen("log/vdump_librw.txt", "a")) {
                const rw::V3d* vp = geo->morphTargets[0].vertices;
                for (std::int32_t i = 0; i < nt; ++i) {
                    if (s.tris[i * 4 + 0] != 4) continue;   // snow only
                    for (int j = 0; j < 3; ++j) {
                        const std::uint16_t vi = s.tris[i * 4 + 1 + j];
                        const rw::RGBA& c = geo->colors[vi];
                        std::fprintf(f, "4 %.3f %.3f %.3f %u %u %u\n",
                                     vp[vi].x, vp[vi].y, vp[vi].z,
                                     (unsigned)c.red, (unsigned)c.green, (unsigned)c.blue);
                    }
                }
                std::fclose(f);
            }
        }
        AppendMaterials(geo, mats);   // matId is a world-global index
        FinishGeometry(geo);

        if (rw::Atomic* a = MakeAtomic(geo, worldFrame))
            sectors->addAtomic(a);
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
        // Liveness for ONLYMAT: what SURVIVED as drawable triangles. If
        // onlymat=N and any material other than N shows a non-zero kept count,
        // the filter did not take and the run must not be measured.
        if (only_mat >= 0) {
            char line[512];
            int off = 0;
            for (std::size_t i = 0; i < kept.size() && off < 400; ++i)
                if (kept[i])
                    off += std::snprintf(line + off, sizeof(line) - off,
                                         " mat[%zu]=%zu", i, kept[i]);
            if (off == 0) std::snprintf(line, sizeof(line), " (nothing)");
            SLog("  world ONLYMAT=%d PROBE kept:%s", only_mat, line);
        }
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

void* BuildClump(const Track::DffModel& model, const TextureSource& tex,
                 std::uint32_t ambient,
                 std::vector<std::uint32_t>* out_atomic_mat) {
    if (out_atomic_mat) out_atomic_mat->clear();
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
        SLog("clump[%d]: mats=%zu named=%d resolved=%d batches=%zu",
             id, model.materials.size(), named, resolved, model.batches.size());
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
    rw::Frame* root = rw::Frame::create();
    clump->setFrame(root);

    std::size_t bi = static_cast<std::size_t>(-1);
    for (const Track::DffBatch& b : model.batches) {
        ++bi;
        const std::int32_t nv = static_cast<std::int32_t>(b.verts.size() / 3);
        const std::int32_t nt = static_cast<std::int32_t>(b.tris.size() / 3);
        if (nv == 0 || nt == 0) continue;

        std::uint32_t flags = rw::Geometry::POSITIONS;
        if (!b.uvs.empty())     flags |= rw::Geometry::TEXTURED;
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
        if (AmbientFoldEnabled() && ambient && !b.prelit.empty() && !b.lit) {
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
        FillVertexData(geo, nv, b.verts, b.uvs, *prelit_src, &b.normals);
        // [D-S3-PROP] MASHED_PROP_VDUMP=<handle>: the librw half of the pair. Dump
        // every vertex this batch uploads as "batch x y z r g b" -- same columns
        // and same order as the D3D9 side (both walk model.batches in order), so
        // the two files can be diffed directly. Appends across batches; the caller
        // truncates the file once per build. See the D3D9 side for why v0-only was
        // not enough.
        if (PropVDumpHandle() >= 0 && PropVDumpHandle() == s_registering_handle &&
            geo->colors && geo->morphTargets[0].vertices) {
            if (std::FILE* vf = std::fopen("log/pvdump_librw.txt", "a")) {
                const rw::V3d* vp = geo->morphTargets[0].vertices;
                for (std::int32_t i = 0; i < nv; ++i) {
                    const rw::RGBA& c = geo->colors[i];
                    std::fprintf(vf, "%zu %.3f %.3f %.3f %u %u %u\n", bi,
                                 vp[i].x, vp[i].y, vp[i].z,
                                 (unsigned)c.red, (unsigned)c.green, (unsigned)c.blue);
                }
                std::fclose(vf);
            }
        }
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

        if (rw::Atomic* a = MakeAtomic(geo, root)) {
            clump->addAtomic(a);
            // Record the material HERE, where the atomic is actually created --
            // batches that produced none (nv==0 || nt==0, skipped above) must not
            // shift the mapping.
            if (out_atomic_mat) out_atomic_mat->push_back(b.material);
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
