// Mashed RE — R4 opener track renderer implementation. See TrackRenderer.h.

#include "TrackRenderer.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

#include "../Piz/PizReader.h"
#include "../Track/TrackWorld.h"
#include "../Track/DffModel.h"
#include "../Txd/TxdDecoder.h"

namespace mashed_re {
namespace D3d9Render {

namespace {

// Expand a Txd::Texture base mip (PAL4 / PAL8 / ARGB8888) into a fresh
// A8R8G8B8 D3D9 texture. TXD palettes are RGBA byte order (per TxdDecoder's
// PAL8 path in QuadRenderer); D3D wants BGRA dwords.
IDirect3DTexture9* MakeTexture(IDirect3DDevice9* dev,
                               const mashed_re::Txd::Texture& tex) {
    if (tex.mip_count == 0) return nullptr;
    const auto& mip = tex.mips[0];
    const std::uint32_t w = mip.width, h = mip.height;
    if (w == 0 || h == 0) return nullptr;
    IDirect3DTexture9* out = nullptr;
    if (FAILED(dev->CreateTexture(w, h, 1, 0, D3DFMT_A8R8G8B8,
                                  D3DPOOL_MANAGED, &out, nullptr)))
        return nullptr;
    D3DLOCKED_RECT lr;
    if (FAILED(out->LockRect(0, &lr, nullptr, 0))) { out->Release(); return nullptr; }
    for (std::uint32_t y = 0; y < h; ++y) {
        std::uint32_t* dst = reinterpret_cast<std::uint32_t*>(
            static_cast<std::uint8_t*>(lr.pBits) + static_cast<std::size_t>(y) * lr.Pitch);
        const std::uint8_t* row = mip.pixels + static_cast<std::size_t>(y) * mip.stride;
        for (std::uint32_t x = 0; x < w; ++x) {
            std::uint8_t r, g, b, a;
            if (mip.depth == 32) {
                r = row[x * 4 + 0]; g = row[x * 4 + 1];
                b = row[x * 4 + 2]; a = row[x * 4 + 3];
            } else {
                std::uint32_t idx;
                if (mip.depth == 4) {
                    const std::uint8_t pair = row[x >> 1];
                    idx = (x & 1) ? (pair >> 4) : (pair & 0x0F);
                } else {  // 8 bpp
                    idx = row[x];
                }
                const std::uint8_t* pe = mip.palette + idx * 4;
                r = pe[0]; g = pe[1]; b = pe[2]; a = pe[3];
            }
            dst[x] = (static_cast<std::uint32_t>(a) << 24) |
                     (static_cast<std::uint32_t>(r) << 16) |
                     (static_cast<std::uint32_t>(g) << 8) | b;
        }
    }
    out->UnlockRect(0);
    return out;
}

// Build per-material vertex batches (+ textures) from a parsed DffModel,
// resolving texture names across the given TXD dictionaries. Shared by the
// car and the track props.
void BuildDffBatches(IDirect3DDevice9* dev,
                     const mashed_re::Track::DffModel& model,
                     const std::vector<mashed_re::Txd::Dictionary>& dicts,
                     std::vector<std::vector<TrackRenderer::V>>* batches,
                     std::vector<IDirect3DTexture9*>* textures) {
    using V = TrackRenderer::V;
    textures->assign(model.materials.size(), nullptr);
    for (std::size_t mi = 0; mi < model.materials.size(); ++mi) {
        const char* want = model.materials[mi].tex_name;
        if (!want[0]) continue;
        for (std::size_t di = 0; di < dicts.size() && !(*textures)[mi]; ++di)
            for (std::uint32_t ti = 0; ti < dicts[di].count(); ++ti)
                if (std::strcmp(dicts[di].texture(ti).name, want) == 0) {
                    (*textures)[mi] = MakeTexture(dev, dicts[di].texture(ti));
                    break;
                }
    }
    batches->assign(model.materials.size(), {});
    for (const auto& b : model.batches) {
        auto& out = (*batches)[b.material];
        const std::size_t n = b.tris.size();
        for (std::size_t i = 0; i < n; ++i) {
            const std::uint16_t vi = b.tris[i];
            V v;
            v.x = b.verts[vi * 3 + 0];
            v.y = b.verts[vi * 3 + 1];
            v.z = b.verts[vi * 3 + 2];
            if (!b.prelit.empty()) {
                const std::uint32_t p = b.prelit[vi];
                v.c = D3DCOLOR_ARGB(static_cast<std::uint8_t>(p >> 24),
                                    static_cast<std::uint8_t>(p),
                                    static_cast<std::uint8_t>(p >> 8),
                                    static_cast<std::uint8_t>(p >> 16));
            } else {
                v.c = 0xFFFFFFFFu;
            }
            v.u = b.uvs[vi * 2 + 0];
            v.v = b.uvs[vi * 2 + 1];
            out.push_back(v);
        }
    }
}

void MatIdentity(D3DMATRIX* m) {
    std::memset(m, 0, sizeof(*m));
    m->_11 = m->_22 = m->_33 = m->_44 = 1.f;
}

void MatLookAtLH(D3DMATRIX* m, const float eye[3], const float at[3]) {
    float z[3] = {at[0] - eye[0], at[1] - eye[1], at[2] - eye[2]};
    const float zl = std::sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
    z[0] /= zl; z[1] /= zl; z[2] /= zl;
    const float up[3] = {0.f, 1.f, 0.f};
    float x[3] = {up[1]*z[2] - up[2]*z[1], up[2]*z[0] - up[0]*z[2],
                  up[0]*z[1] - up[1]*z[0]};
    const float xl = std::sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
    x[0] /= xl; x[1] /= xl; x[2] /= xl;
    const float y[3] = {z[1]*x[2] - z[2]*x[1], z[2]*x[0] - z[0]*x[2],
                        z[0]*x[1] - z[1]*x[0]};
    MatIdentity(m);
    m->_11 = x[0]; m->_12 = y[0]; m->_13 = z[0];
    m->_21 = x[1]; m->_22 = y[1]; m->_23 = z[1];
    m->_31 = x[2]; m->_32 = y[2]; m->_33 = z[2];
    m->_41 = -(x[0]*eye[0] + x[1]*eye[1] + x[2]*eye[2]);
    m->_42 = -(y[0]*eye[0] + y[1]*eye[1] + y[2]*eye[2]);
    m->_43 = -(z[0]*eye[0] + z[1]*eye[1] + z[2]*eye[2]);
}

void MatPerspectiveFovLH(D3DMATRIX* m, float fovy, float aspect,
                         float zn, float zf) {
    const float ys = 1.f / std::tan(fovy * 0.5f);
    const float xs = ys / aspect;
    std::memset(m, 0, sizeof(*m));
    m->_11 = xs; m->_22 = ys;
    m->_33 = zf / (zf - zn); m->_34 = 1.f;
    m->_43 = -zn * zf / (zf - zn);
}

}  // namespace

bool TrackRenderer::Load(IDirect3DDevice9* dev, const char* piz_path,
                         const char* log_path) {
    std::FILE* log = log_path ? std::fopen(log_path, "a") : nullptr;
    auto fail = [&](const char* why) {
        if (log) { std::fprintf(log, "R4 track load FAILED: %s\n", why); std::fclose(log); }
        return false;
    };

    Piz::Archive piz;
    if (!piz.Load(piz_path)) return fail(piz.last_error());

    // GRAPH*.BSP (name varies: GRAPH.BSP / GRAPHICS.BSP). Textures: decode
    // EVERY *.TXD in the piz (DUMP.TXD + CITY.TXD coexist; props reference
    // secondary dictionaries) and resolve material names across all of them.
    const std::uint8_t* world_blob = nullptr; std::uint32_t world_len = 0;
    std::vector<std::pair<const std::uint8_t*, std::uint32_t>> txds;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        char u[129];
        for (std::size_t k = 0; k <= ln && k < 128; ++k)
            u[k] = static_cast<char>(::toupper(static_cast<unsigned char>(n[k])));
        u[128] = '\0';
        if (std::strncmp(u, "GRAPH", 5) == 0 && ln > 4 &&
            std::strcmp(u + ln - 4, ".BSP") == 0) {
            world_blob = piz.blob(i, &world_len);
        } else if (std::strncmp(u, "COLL", 4) == 0 && ln > 4 &&
                   std::strcmp(u + ln - 4, ".BSP") == 0) {
            // COLLISIONS.BSP / COLLIDE.BSP / COLL.BSP — same world stream;
            // flatten into the raycast triangle soup.
            std::uint32_t cl = 0;
            const std::uint8_t* cb = piz.blob(i, &cl);
            Track::World cw;
            if (cb && cw.Parse(cb, cl)) {
                for (const auto& s : cw.sectors) {
                    const std::uint32_t vbase =
                        static_cast<std::uint32_t>(col_verts_.size() / 3);
                    col_verts_.insert(col_verts_.end(), s.verts.begin(),
                                      s.verts.end());
                    const std::size_t nt = s.tris.size() / 4;
                    for (std::size_t ti = 0; ti < nt; ++ti) {
                        col_tris_.push_back(vbase + s.tris[ti * 4 + 1]);
                        col_tris_.push_back(vbase + s.tris[ti * 4 + 2]);
                        col_tris_.push_back(vbase + s.tris[ti * 4 + 3]);
                    }
                }
            }
        } else if (ln > 4 && std::strcmp(u + ln - 4, ".TXD") == 0) {
            std::uint32_t bl = 0;
            const std::uint8_t* b = piz.blob(i, &bl);
            if (b) txds.emplace_back(b, bl);
        }
    }
    if (!world_blob) return fail("GRAPH*.BSP not found");

    Track::World world;
    if (!world.Parse(world_blob, world_len)) return fail(world.last_error());

    std::vector<Txd::Dictionary> dicts(txds.size());
    for (std::size_t di = 0; di < txds.size(); ++di)
        dicts[di].Decode(txds[di].first, txds[di].second);  // failures = count 0

    // material -> texture (first match across all dictionaries)
    textures_.assign(world.materials.size(), nullptr);
    for (std::size_t mi = 0; mi < world.materials.size(); ++mi) {
        const char* want = world.materials[mi].tex_name;
        if (!want[0]) continue;
        for (std::size_t di = 0; di < dicts.size() && !textures_[mi]; ++di) {
            for (std::uint32_t ti = 0; ti < dicts[di].count(); ++ti) {
                if (std::strcmp(dicts[di].texture(ti).name, want) == 0) {
                    textures_[mi] = MakeTexture(dev, dicts[di].texture(ti));
                    break;
                }
            }
        }
    }

    // batches per material
    batches_.assign(world.materials.size(), {});
    for (const auto& s : world.sectors) {
        const std::size_t nt = s.tris.size() / 4;
        const bool has_uv = !s.uvs.empty();
        const bool has_pl = !s.prelit.empty();
        for (std::size_t i = 0; i < nt; ++i) {
            const std::uint16_t mat = s.tris[i * 4 + 0];
            auto& b = batches_[mat];
            for (int k = 1; k <= 3; ++k) {
                const std::uint16_t vi = s.tris[i * 4 + k];
                V v;
                v.x = s.verts[vi * 3 + 0];
                v.y = s.verts[vi * 3 + 1];
                v.z = s.verts[vi * 3 + 2];
                // prelight is RGBA bytes; D3D diffuse is BGRA dwords
                if (has_pl) {
                    const std::uint32_t p = s.prelit[vi];
                    const std::uint8_t r = static_cast<std::uint8_t>(p),
                                       g = static_cast<std::uint8_t>(p >> 8),
                                       bb = static_cast<std::uint8_t>(p >> 16),
                                       a = static_cast<std::uint8_t>(p >> 24);
                    v.c = D3DCOLOR_ARGB(a, r, g, bb);
                } else {
                    v.c = 0xFFFFFFFFu;
                }
                v.u = has_uv ? s.uvs[vi * 2 + 0] : 0.f;
                v.v = has_uv ? s.uvs[vi * 2 + 1] : 0.f;
                b.push_back(v);
            }
        }
    }

    // ---- track props ------------------------------------------------------
    // COURSE.LUA wires them: RWP_Object(i,"name","X.dff","Y.mts") = physics
    // props placed by an MTS matrix set (ExportMatrices: u32 count + per-
    // instance chunk 0x0d {STRUCT: 3x3 rot + translation + u32}); and
    // Clump_Filename(i,"x.dff") = world clumps at identity (their frames
    // carry placement), minus Clump_Exclude_From_World(i) overlays.
    {
        auto find_entry = [&](const char* name, std::uint32_t* len)
            -> const std::uint8_t* {
            for (std::uint32_t i = 0; i < piz.count(); ++i)
                if (_stricmp(piz.entry(i).name, name) == 0)
                    return piz.blob(i, len);
            return nullptr;
        };
        std::uint32_t lua_len = 0;
        const std::uint8_t* lua = nullptr;
        for (std::uint32_t i = 0; i < piz.count(); ++i) {
            const char* n = piz.entry(i).name;
            const std::size_t ln = std::strlen(n);
            if (ln > 4 && _stricmp(n + ln - 4, ".LUA") == 0 &&
                _strnicmp(n, "COURSE", 6) == 0) {
                lua = piz.blob(i, &lua_len);
                break;
            }
        }
        auto load_prop = [&](const char* dff_name, Prop* p) -> bool {
            std::uint32_t dl = 0;
            const std::uint8_t* db = find_entry(dff_name, &dl);
            if (!db) return false;
            Track::DffModel m;
            if (!m.Parse(db, dl)) return false;
            BuildDffBatches(dev, m, dicts, &p->batches, &p->textures);
            return true;
        };
        if (lua) {
            // line-scan the Lua text for the wiring calls (comments start
            // with "--"; skip them)
            const char* s = reinterpret_cast<const char*>(lua);
            std::size_t pos = 0;
            bool excluded[64] = {};
            struct ClumpRef { int idx; char dff[64]; };
            std::vector<ClumpRef> clumps;
            while (pos < lua_len) {
                std::size_t eol = pos;
                while (eol < lua_len && s[eol] != '\n') ++eol;
                std::string line(s + pos, eol - pos);
                pos = eol + 1;
                const std::size_t cmt = line.find("--");
                if (cmt != std::string::npos) line.resize(cmt);
                char a[64] = {}, b[64] = {};
                int idx = -1;
                if (std::sscanf(line.c_str(),
                                " RWP_Object( %*d , \"%*[^\"]\" , \"%63[^\"]\" "
                                ", \"%63[^\"]\" )", a, b) == 2 ||
                    std::sscanf(line.c_str(),
                                " RWP_Object( %*d ,\"%*[^\"]\", \"%63[^\"]\", "
                                "\"%63[^\"]\" )", a, b) == 2) {
                    Prop p;
                    std::uint32_t ml = 0;
                    const std::uint8_t* mb = find_entry(b, &ml);
                    if (mb && ml >= 4 && load_prop(a, &p)) {
                        const std::uint32_t cnt =
                            *reinterpret_cast<const std::uint32_t*>(mb);
                        std::size_t off = 4;
                        for (std::uint32_t k = 0; k < cnt &&
                             off + 24 + 48 <= ml; ++k) {
                            const std::uint32_t csz =
                                *reinterpret_cast<const std::uint32_t*>(mb + off + 4);
                            const float* f =
                                reinterpret_cast<const float*>(mb + off + 24);
                            D3DMATRIX m2;
                            MatIdentity(&m2);
                            m2._11 = f[0]; m2._12 = f[1]; m2._13 = f[2];
                            m2._21 = f[3]; m2._22 = f[4]; m2._23 = f[5];
                            m2._31 = f[6]; m2._32 = f[7]; m2._33 = f[8];
                            m2._41 = f[9]; m2._42 = f[10]; m2._43 = f[11];
                            p.instances.push_back(m2);
                            off += 12 + csz;
                        }
                        if (!p.instances.empty())
                            props_.push_back(std::move(p));
                    }
                } else if (std::sscanf(line.c_str(),
                                       " Clump_Filename( %d , \"%63[^\"]\" )",
                                       &idx, a) == 2 ||
                           std::sscanf(line.c_str(),
                                       " Clump_Filename(%d,\"%63[^\"]\")",
                                       &idx, a) == 2) {
                    if (idx >= 0 && idx < 64) {
                        ClumpRef c; c.idx = idx;
                        std::snprintf(c.dff, sizeof(c.dff), "%s", a);
                        clumps.push_back(c);
                    }
                } else if (std::sscanf(line.c_str(),
                                       " Clump_Exclude_From_World( %d )",
                                       &idx) == 1 ||
                           std::sscanf(line.c_str(),
                                       " Clump_Exclude_From_World(%d)",
                                       &idx) == 1) {
                    if (idx >= 0 && idx < 64) excluded[idx] = true;
                }
            }
            for (const auto& c : clumps) {
                if (excluded[c.idx]) continue;
                Prop p;
                if (load_prop(c.dff, &p)) {
                    D3DMATRIX id;
                    MatIdentity(&id);
                    p.instances.push_back(id);
                    props_.push_back(std::move(p));
                }
            }
        }
    }

    // ---- AI path gates (AI*.BSP — same world stream; 4-vert vertical quad
    // per gate; the material RED byte = gate index; LAPDATA's Lap_Line
    // numbers index these). Build ordered gate centers; gate 0 = start line.
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if ((ln >= 6 && _strnicmp(n, "AI", 2) == 0 &&
             _stricmp(n + ln - 4, ".BSP") == 0)) {
            std::uint32_t al = 0;
            const std::uint8_t* ab = piz.blob(i, &al);
            Track::World aw;
            if (ab && aw.Parse(ab, al)) {
                gates_.assign(aw.materials.size(), Gate{});
                std::vector<int> counts(aw.materials.size(), 0);
                for (const auto& s : aw.sectors) {
                    const std::size_t nt = s.tris.size() / 4;
                    for (std::size_t ti = 0; ti < nt; ++ti) {
                        const std::uint16_t mat = s.tris[ti * 4 + 0];
                        // gate index = material RED byte (== mat order here)
                        const std::uint8_t gi = aw.materials[mat].rgba[0];
                        if (gi >= gates_.size()) continue;
                        for (int k = 1; k <= 3; ++k) {
                            const std::uint16_t vi = s.tris[ti * 4 + k];
                            gates_[gi].center[0] += s.verts[vi * 3 + 0];
                            gates_[gi].center[1] += s.verts[vi * 3 + 1];
                            gates_[gi].center[2] += s.verts[vi * 3 + 2];
                            ++counts[gi];
                        }
                    }
                }
                for (std::size_t gi = 0; gi < gates_.size(); ++gi)
                    if (counts[gi] > 0)
                        for (int k = 0; k < 3; ++k)
                            gates_[gi].center[k] /= static_cast<float>(counts[gi]);
            }
            break;
        }
    }

    // render-world soup for spawn validation (visible-surface heights)
    for (const auto& s : world.sectors) {
        const std::uint32_t vbase =
            static_cast<std::uint32_t>(rend_verts_.size() / 3);
        rend_verts_.insert(rend_verts_.end(), s.verts.begin(), s.verts.end());
        const std::size_t nt = s.tris.size() / 4;
        for (std::size_t ti = 0; ti < nt; ++ti) {
            rend_tris_.push_back(vbase + s.tris[ti * 4 + 1]);
            rend_tris_.push_back(vbase + s.tris[ti * 4 + 2]);
            rend_tris_.push_back(vbase + s.tris[ti * 4 + 3]);
        }
    }

    center_[0] = (world.bbox[0] + world.bbox[3]) * 0.5f;
    center_[1] = (world.bbox[1] + world.bbox[4]) * 0.5f;
    center_[2] = (world.bbox[2] + world.bbox[5]) * 0.5f;
    const float dx = world.bbox[0] - world.bbox[3];
    const float dz = world.bbox[2] - world.bbox[5];
    radius_ = 0.5f * std::sqrt(dx * dx + dz * dz);

    if (log) {
        int textured = 0;
        for (auto* t : textures_) if (t) ++textured;
        std::size_t prop_inst = 0;
        for (const auto& p : props_) prop_inst += p.instances.size();
        std::fprintf(log, "R4 track load OK: %s — tris=%u verts=%u sectors=%zu "
                          "mats=%zu textured=%d radius=%.2f props=%zu "
                          "(instances=%zu) gates=%zu\n",
                     piz_path, world.total_tris, world.total_verts,
                     world.sectors.size(), world.materials.size(), textured,
                     radius_, props_.size(), prop_inst, gates_.size());
        std::fclose(log);
    }
    ready_ = true;
    return true;
}

namespace {
// Vertical height query on a triangle soup: barycentric XZ test, plane height
// at (x,z); returns the HIGHEST surface (bridges: highest = drivable deck).
float HeightOnSoup(const std::vector<float>& verts,
                   const std::vector<std::uint32_t>& tris,
                   float x, float z, bool* ok) {
    float best = -1e9f;
    bool found = false;
    const std::size_t nt = tris.size() / 3;
    for (std::size_t i = 0; i < nt; ++i) {
        const float* a = &verts[tris[i * 3 + 0] * 3];
        const float* b = &verts[tris[i * 3 + 1] * 3];
        const float* c = &verts[tris[i * 3 + 2] * 3];
        const float d00x = b[0] - a[0], d00z = b[2] - a[2];
        const float d01x = c[0] - a[0], d01z = c[2] - a[2];
        const float den = d00x * d01z - d01x * d00z;
        if (den > -1e-9f && den < 1e-9f) continue;   // degenerate in XZ
        const float px = x - a[0], pz = z - a[2];
        const float u = (px * d01z - d01x * pz) / den;
        const float v = (d00x * pz - px * d00z) / den;
        if (u < 0.f || v < 0.f || u + v > 1.f) continue;
        const float y = a[1] + u * (b[1] - a[1]) + v * (c[1] - a[1]);
        if (y > best) { best = y; found = true; }
    }
    if (ok) *ok = found;
    return best;
}
}  // namespace

float TrackRenderer::GroundHeight(float x, float z, bool* ok) const {
    return HeightOnSoup(col_verts_, col_tris_, x, z, ok);
}

bool TrackRenderer::LoadCar(IDirect3DDevice9* dev, const char* piz_path,
                            const char* dff_entry, const char* log_path) {
    std::FILE* log = log_path ? std::fopen(log_path, "a") : nullptr;
    auto fail = [&](const char* why) {
        if (log) { std::fprintf(log, "R5 car load FAILED: %s\n", why); std::fclose(log); }
        return false;
    };
    Piz::Archive piz;
    if (!piz.Load(piz_path)) return fail(piz.last_error());

    const std::uint8_t* dff = nullptr; std::uint32_t dff_len = 0;
    std::vector<std::pair<const std::uint8_t*, std::uint32_t>> txds;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if (_stricmp(n, dff_entry) == 0) {
            dff = piz.blob(i, &dff_len);
        } else if (ln > 4 && _stricmp(n + ln - 4, ".TXD") == 0) {
            std::uint32_t bl = 0;
            const std::uint8_t* b = piz.blob(i, &bl);
            if (b) txds.emplace_back(b, bl);
        }
    }
    if (!dff) return fail("DFF entry not found");

    Track::DffModel model;
    if (!model.Parse(dff, dff_len)) return fail(model.last_error());

    std::vector<Txd::Dictionary> dicts(txds.size());
    for (std::size_t di = 0; di < txds.size(); ++di)
        dicts[di].Decode(txds[di].first, txds[di].second);

    BuildDffBatches(dev, model, dicts, &car_batches_, &car_textures_);
    car_ground_off_ = -model.bbox[1];   // lift so model min-Y sits on ground

    // REAL start line when the AI gates parsed: gate 0 center (LAPDATA's
    // Lap_Line(0)), facing gate 1 — the race direction.
    if (gates_.size() >= 2) {
        const float* g0 = gates_[0].center;
        const float* g1 = gates_[1].center;
        bool gok = false;
        const float gy = GroundHeight(g0[0], g0[2], &gok);
        if (gok) {
            car_pos_[0] = g0[0];
            car_pos_[2] = g0[2];
            car_pos_[1] = gy + car_ground_off_;
            car_yaw_ = std::atan2(g1[2] - g0[2], g1[0] - g0[0]);
            car_speed_ = 0.f;
            car_ready_ = true;
            if (log) {
                std::fprintf(log, "R6 car spawn at REAL start line: gate0="
                                  "(%.2f, %.2f, %.2f) yaw=%.2f (gates=%zu)\n",
                             car_pos_[0], car_pos_[1], car_pos_[2], car_yaw_,
                             gates_.size());
                std::fclose(log);
            }
            return true;
        }
    }

    // Spawn: scan the collision world for the most OPEN ground point (the
    // racing surface, not a pit/garage): score each candidate by how many of
    // 8 directions still have ground 4 units out at <1.0 height delta, and
    // face the car along the longest open run.
    bool ok = false;
    float sx = 0.f, sz = 0.f, sy = 0.f, syaw = 0.f;
    int best_score = -1;
    for (float rr = 0.f; rr < radius_; rr += radius_ * 0.07f) {
        for (int k = 0; k < 16; ++k) {
            const float ang = static_cast<float>(k) * 0.3926991f;
            const float x = center_[0] + rr * std::cos(ang);
            const float z = center_[2] + rr * std::sin(ang);
            bool hit = false;
            const float y = GroundHeight(x, z, &hit);
            if (!hit) continue;
            // the ground must be VISIBLE (render world within 1.0) — rules
            // out the frozen bay whose ice is collision-only
            bool rh = false;
            const float ry = HeightOnSoup(rend_verts_, rend_tris_, x, z, &rh);
            if (!rh || std::fabs(ry - y) > 1.0f) continue;
            int score = 0; float best_run = 0.f; float best_dir = 0.f;
            for (int d = 0; d < 8; ++d) {
                const float da = static_cast<float>(d) * 0.7853982f;
                float run = 0.f;
                for (float step = 4.f; step <= 16.f; step += 4.f) {
                    const float px = x + std::cos(da) * step;
                    const float pz = z + std::sin(da) * step;
                    bool h2 = false, h3 = false;
                    const float y2 = GroundHeight(px, pz, &h2);
                    const float y3 = HeightOnSoup(rend_verts_, rend_tris_,
                                                  px, pz, &h3);
                    if (!h2 || !h3 || std::fabs(y2 - y) > 1.5f ||
                        std::fabs(y3 - y2) > 1.0f) break;
                    run = step;
                }
                if (run >= 4.f) ++score;
                if (run > best_run) { best_run = run; best_dir = da; }
            }
            if (score > best_score) {
                best_score = score; ok = true;
                sx = x; sz = z; sy = y; syaw = best_dir;
                if (score == 8 && best_run >= 16.f) { rr = radius_; break; }
            }
        }
    }
    if (!ok) return fail("no collision ground found for spawn");
    car_pos_[0] = sx; car_pos_[1] = sy + car_ground_off_; car_pos_[2] = sz;
    car_yaw_ = syaw; car_speed_ = 0.f;

    if (log) {
        std::fprintf(log, "R5 car load OK: %s::%s — batches=%zu mats=%zu "
                          "tris=%u groundoff=%.3f spawn=(%.2f, %.2f, %.2f) "
                          "coltris=%zu\n",
                     piz_path, dff_entry, car_batches_.size(),
                     model.materials.size(), model.total_tris,
                     car_ground_off_, sx, sy, sz, col_tris_.size() / 3);
        std::fclose(log);
    }
    car_ready_ = true;
    return true;
}

void TrackRenderer::UpdateCar(const DriveInput& in) {
    if (!car_ready_ || in.dt <= 0.f) return;
    // Kinematic toy model (R5 opener; NOT the ported physics): accelerate /
    // brake toward a top speed, steer scaled by speed, integrate, snap to the
    // collision ground. Leaving the collision world reverts the move (the
    // island edge acts as a wall).
    const float kTop    = radius_ * 0.25f;   // top speed (world-relative)
    const float kAccel  = kTop * 0.8f;
    const float kDrag   = 0.6f;
    const float kSteer  = 1.8f;
    car_speed_ += in.accel * kAccel * in.dt;
    car_speed_ -= car_speed_ * kDrag * in.dt;
    if (car_speed_ >  kTop) car_speed_ =  kTop;
    if (car_speed_ < -kTop * 0.4f) car_speed_ = -kTop * 0.4f;
    const float spd_norm = car_speed_ / kTop;
    car_yaw_ += in.steer * kSteer * spd_norm * in.dt;
    const float nx = car_pos_[0] + std::cos(car_yaw_) * car_speed_ * in.dt;
    const float nz = car_pos_[2] + std::sin(car_yaw_) * car_speed_ * in.dt;
    bool ok = false;
    const float gy = GroundHeight(nx, nz, &ok);
    if (ok) {
        car_pos_[0] = nx; car_pos_[2] = nz;
        car_pos_[1] = gy + car_ground_off_;
    } else {
        car_speed_ = 0.f;   // island edge / off-collision: stop
    }
}

void TrackRenderer::camera(float eye[3], float at[3]) const {
    for (int i = 0; i < 3; ++i) { eye[i] = last_eye_[i]; at[i] = last_at_[i]; }
}

void TrackRenderer::Render(IDirect3DDevice9* dev, float t, const CamInput* in) {
    if (!ready_) return;
    // Camera: auto-orbit by default; any movement input switches to free
    // mode (WASD/QE move relative to look direction, mouse/arrow look).
    float eye[3];
    float at[3];
    if (in) {
        const bool wants_free =
            in->move_fwd != 0.f || in->move_strafe != 0.f ||
            in->move_up != 0.f || in->yaw_delta != 0.f || in->pitch_delta != 0.f;
        if (in->reset_orbit) free_ = false;
        if (wants_free && !free_) {
            // seed the free camera from the current orbit pose
            const float yaw0 = t * 0.3f;
            eye_[0] = center_[0] + radius_ * 1.15f * std::cos(yaw0);
            eye_[1] = center_[1] + radius_ * 0.55f;
            eye_[2] = center_[2] + radius_ * 1.15f * std::sin(yaw0);
            yaw_   = std::atan2(center_[2] - eye_[2], center_[0] - eye_[0]);
            pitch_ = -0.4f;
            free_  = true;
        }
        if (free_) {
            yaw_   += in->yaw_delta;
            pitch_ += in->pitch_delta;
            if (pitch_ >  1.5f) pitch_ =  1.5f;
            if (pitch_ < -1.5f) pitch_ = -1.5f;
            const float cp = std::cos(pitch_), sp = std::sin(pitch_);
            const float fwd[3] = {std::cos(yaw_) * cp, sp, std::sin(yaw_) * cp};
            const float right[3] = {-std::sin(yaw_), 0.f, std::cos(yaw_)};
            const float speed = radius_ * 0.5f * in->dt;   // half-world/s
            for (int i = 0; i < 3; ++i)
                eye_[i] += fwd[i] * in->move_fwd * speed +
                           right[i] * in->move_strafe * speed;
            eye_[1] += in->move_up * speed;
        }
    }
    if (free_) {
        const float cp = std::cos(pitch_), sp = std::sin(pitch_);
        eye[0] = eye_[0]; eye[1] = eye_[1]; eye[2] = eye_[2];
        at[0] = eye_[0] + std::cos(yaw_) * cp;
        at[1] = eye_[1] + sp;
        at[2] = eye_[2] + std::sin(yaw_) * cp;
    } else if (car_ready_) {
        // chase cam: behind and above the car, looking at it
        const float back = 7.0f, up = 3.0f;
        eye[0] = car_pos_[0] - std::cos(car_yaw_) * back;
        eye[1] = car_pos_[1] + up;
        eye[2] = car_pos_[2] - std::sin(car_yaw_) * back;
        at[0] = car_pos_[0]; at[1] = car_pos_[1] + 0.8f; at[2] = car_pos_[2];
    } else {
        const float yaw = t * 0.3f;
        eye[0] = center_[0] + radius_ * 1.15f * std::cos(yaw);
        eye[1] = center_[1] + radius_ * 0.55f;
        eye[2] = center_[2] + radius_ * 1.15f * std::sin(yaw);
        at[0] = center_[0]; at[1] = center_[1]; at[2] = center_[2];
    }
    for (int i = 0; i < 3; ++i) { last_eye_[i] = eye[i]; last_at_[i] = at[i]; }

    D3DMATRIX viewm, projm, worldm;
    MatLookAtLH(&viewm, eye, at);
    MatPerspectiveFovLH(&projm, 1.0472f /*60 deg*/, 800.f / 600.f,
                        0.05f, radius_ * 8.f);
    MatIdentity(&worldm);
    dev->SetTransform(D3DTS_WORLD, &worldm);
    dev->SetTransform(D3DTS_VIEW, &viewm);
    dev->SetTransform(D3DTS_PROJECTION, &projm);

    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    dev->SetRenderState(D3DRS_LIGHTING, FALSE);
    dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    dev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    dev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    dev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    dev->SetFVF(kFVF);

    for (std::size_t mi = 0; mi < batches_.size(); ++mi) {
        const auto& b = batches_[mi];
        if (b.empty()) continue;
        dev->SetTexture(0, textures_[mi]);
        if (!textures_[mi])
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                             static_cast<UINT>(b.size() / 3),
                             b.data(), sizeof(V));
        if (!textures_[mi])
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }

    // R6: track props — instanced DFF batches (tyre walls, crates, sea,
    // freighter...) under their MTS / identity world matrices.
    for (const auto& p : props_) {
        for (const auto& inst : p.instances) {
            dev->SetTransform(D3DTS_WORLD, &inst);
            for (std::size_t mi = 0; mi < p.batches.size(); ++mi) {
                const auto& b = p.batches[mi];
                if (b.empty()) continue;
                dev->SetTexture(0, p.textures[mi]);
                if (!p.textures[mi])
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_SELECTARG2);
                dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                     static_cast<UINT>(b.size() / 3),
                                     b.data(), sizeof(V));
                if (!p.textures[mi])
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_MODULATE);
            }
        }
    }
    dev->SetTransform(D3DTS_WORLD, &worldm);

    // R5: the car — model-space batches under a yaw+translate world matrix.
    if (car_ready_) {
        D3DMATRIX carm;
        MatIdentity(&carm);
        const float cy = std::cos(car_yaw_), sy2 = std::sin(car_yaw_);
        carm._11 =  cy;  carm._13 = sy2;
        carm._31 = -sy2; carm._33 = cy;
        carm._41 = car_pos_[0]; carm._42 = car_pos_[1]; carm._43 = car_pos_[2];
        dev->SetTransform(D3DTS_WORLD, &carm);
        for (std::size_t mi = 0; mi < car_batches_.size(); ++mi) {
            const auto& b = car_batches_[mi];
            if (b.empty()) continue;
            dev->SetTexture(0, car_textures_[mi]);
            if (!car_textures_[mi])
                dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
            dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                 static_cast<UINT>(b.size() / 3),
                                 b.data(), sizeof(V));
            if (!car_textures_[mi])
                dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        }
        dev->SetTransform(D3DTS_WORLD, &worldm);
    }

    dev->SetTexture(0, nullptr);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);  // back to the 2D menu path
}

}  // namespace D3d9Render
}  // namespace mashed_re
