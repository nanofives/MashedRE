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
    if (parent) f->addChild(parent);
    a->setFrame(f);
    a->setGeometry(geo, 0);
    return a;
}

}  // namespace

void* BuildWorld(const Track::World& world, const TextureSource& tex) {
    std::vector<rw::Material*> mats;
    BuildMaterials(world.materials, tex, mats);

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

    for (const Track::Sector& s : world.sectors) {
        const std::int32_t nv = static_cast<std::int32_t>(s.verts.size() / 3);
        const std::int32_t nt = static_cast<std::int32_t>(s.tris.size() / 4);
        if (nv == 0 || nt == 0) continue;

        std::uint32_t flags = rw::Geometry::POSITIONS;
        if (!s.uvs.empty())    flags |= rw::Geometry::TEXTURED;
        if (!s.prelit.empty()) flags |= rw::Geometry::PRELIT;
        // Deliberately no NORMALS/LIGHT: the static world carries no vertex
        // normals, so its baked prelight IS the lighting -- same as D3D9 today.

        rw::Geometry* geo = rw::Geometry::create(nv, nt, flags);
        if (!geo) continue;
        FillVertexData(geo, nv, s.verts, s.uvs, s.prelit, nullptr);
        for (std::int32_t i = 0; i < nt; ++i) {
            // Track::Sector::tris is (mat, v0, v1, v2) -- TrackWorld.h:30.
            geo->triangles[i].matId = s.tris[i * 4 + 0];
            geo->triangles[i].v[0]  = s.tris[i * 4 + 1];
            geo->triangles[i].v[1]  = s.tris[i * 4 + 2];
            geo->triangles[i].v[2]  = s.tris[i * 4 + 3];
        }
        AppendMaterials(geo, mats);   // matId is a world-global index
        FinishGeometry(geo);

        if (rw::Atomic* a = MakeAtomic(geo, worldFrame))
            sectors->addAtomic(a);
    }
    rww->addClump(sectors);
    return rww;
}

// Defined further down next to kSceneLog, inside the unnamed namespace. All
// `namespace {}` blocks in a TU are the SAME namespace, so the declaration must
// go in one too -- at outer scope it becomes a DIFFERENT function and every later
// call is ambiguous.
namespace { void SLog(const char* fmt, ...); }

void* BuildClump(const Track::DffModel& model, const TextureSource& tex) {
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
                 "norm=%d lit=%d mod=%d",
                 id, bi, (unsigned)b.material, b.verts.size() / 3, b.tris.size() / 3,
                 (int)!b.uvs.empty(), (int)!b.prelit.empty(), (int)!b.normals.empty(),
                 (int)b.lit, (int)b.modulate_mat);
        }
    }

    rw::Clump* clump = rw::Clump::create();
    if (!clump) return nullptr;
    rw::Frame* root = rw::Frame::create();
    clump->setFrame(root);

    for (const Track::DffBatch& b : model.batches) {
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
        FillVertexData(geo, nv, b.verts, b.uvs, b.prelit, &b.normals);
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

        if (rw::Atomic* a = MakeAtomic(geo, root))
            clump->addAtomic(a);
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
