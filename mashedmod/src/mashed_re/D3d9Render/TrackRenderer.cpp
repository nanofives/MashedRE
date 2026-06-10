// Mashed RE — R4 opener track renderer implementation. See TrackRenderer.h.

#include "TrackRenderer.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#include "../Piz/PizReader.h"
#include "../Track/TrackWorld.h"
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

    // GRAPH*.BSP (name varies: GRAPH.BSP / GRAPHICS.BSP). TXD: prefer
    // *TEXTURES.TXD, else the LARGEST *.TXD (CITY.TXD / DUMP.TXD naming).
    const std::uint8_t* world_blob = nullptr; std::uint32_t world_len = 0;
    const std::uint8_t* txd_blob = nullptr;   std::uint32_t txd_len = 0;
    bool txd_preferred = false;
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
        } else if (ln > 4 && std::strcmp(u + ln - 4, ".TXD") == 0 &&
                   !txd_preferred) {
            const bool pref = std::strstr(u, "TEXTURES.TXD") != nullptr;
            if (pref || txd_blob == nullptr || piz.entry(i).length > txd_len) {
                txd_blob = piz.blob(i, &txd_len);
                txd_preferred = pref;
            }
        }
    }
    if (!world_blob) return fail("GRAPH*.BSP not found");

    Track::World world;
    if (!world.Parse(world_blob, world_len)) return fail(world.last_error());

    Txd::Dictionary dict;
    const bool txd_ok = txd_blob && dict.Decode(txd_blob, txd_len);

    // material -> texture
    textures_.assign(world.materials.size(), nullptr);
    if (txd_ok) {
        for (std::size_t mi = 0; mi < world.materials.size(); ++mi) {
            const char* want = world.materials[mi].tex_name;
            if (!want[0]) continue;
            for (std::uint32_t ti = 0; ti < dict.count(); ++ti) {
                if (std::strcmp(dict.texture(ti).name, want) == 0) {
                    textures_[mi] = MakeTexture(dev, dict.texture(ti));
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

    center_[0] = (world.bbox[0] + world.bbox[3]) * 0.5f;
    center_[1] = (world.bbox[1] + world.bbox[4]) * 0.5f;
    center_[2] = (world.bbox[2] + world.bbox[5]) * 0.5f;
    const float dx = world.bbox[0] - world.bbox[3];
    const float dz = world.bbox[2] - world.bbox[5];
    radius_ = 0.5f * std::sqrt(dx * dx + dz * dz);

    if (log) {
        int textured = 0;
        for (auto* t : textures_) if (t) ++textured;
        std::fprintf(log, "R4 track load OK: %s — tris=%u verts=%u sectors=%zu "
                          "mats=%zu textured=%d radius=%.2f\n",
                     piz_path, world.total_tris, world.total_verts,
                     world.sectors.size(), world.materials.size(), textured,
                     radius_);
        std::fclose(log);
    }
    ready_ = true;
    return true;
}

void TrackRenderer::Render(IDirect3DDevice9* dev, float t) {
    if (!ready_) return;
    // fly-through: slow orbit around the world center, slightly above.
    const float yaw = t * 0.3f;
    const float eye[3] = {
        center_[0] + radius_ * 1.15f * std::cos(yaw),
        center_[1] + radius_ * 0.55f,
        center_[2] + radius_ * 1.15f * std::sin(yaw),
    };
    D3DMATRIX viewm, projm, worldm;
    MatLookAtLH(&viewm, eye, center_);
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
    dev->SetTexture(0, nullptr);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);  // back to the 2D menu path
}

}  // namespace D3d9Render
}  // namespace mashed_re
