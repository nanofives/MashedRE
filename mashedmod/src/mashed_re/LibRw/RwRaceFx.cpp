// LibRw/RwRaceFx.cpp — see RwRaceFx.h. EXE-ONLY isolated TU (build.bat); includes <rw.h>.
//
// [rfx 2026-10-16] The car shadow (C04) and headlight beam (C09) of the d1r1 adjudication,
// verify/rfx_20261016/PREREG_rfx.md section 2. Every constant below is cited to its address.

#include "RwRaceFx.h"

#include "RwRasterBridge.h"

#define WITH_D3D
#include <rw.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "../D3d9Render/DrawRec.h"
#include "../Math/RwLutTables.h"
#include "../Piz/PizReader.h"
#include "../Txd/TxdDecoder.h"

// d1r1 per-draw recorder hooks (deps/librw/src/d3d/d3d9render.cpp); null unless armed.
extern void (*rw_drawrec_pre)(const char* texname, const float* ltm16);
extern void (*rw_drawrec_post)(void);

namespace mashed_re {
namespace LibRw {
namespace {

bool EnvOff(const char* name) {
    const char* e = std::getenv(name);
    return e && e[0] == '0' && e[1] == '\0';
}

// ---- the two FX.TXD textures (SFX.piz), resolved like FUN_0040bb30 @ 0x0040bb30 ----------------
// FUN_0041eaa0 @ 0x0041eaa0 binds "beam" (string 0x005cd268, 0x0041eac2) and "car_shadow" (0x005cd25c, 0x0041eaf9) from the
// dictionary at DAT_0063b8f8 through FUN_004c5c00; the names on disk are "Beam" / "car_shadow",
// so the lookup is case-insensitive.
rw::Texture* g_tex_shadow = nullptr;
rw::Texture* g_tex_beam   = nullptr;
bool         g_tex_tried  = false;

void LoadTextures() {
    if (g_tex_tried) return;
    g_tex_tried = true;
    Piz::Archive a;
    if (!a.Load("original/TOASTART/Common/SFX.piz")) return;
    for (std::uint32_t i = 0; i < a.count(); ++i) {
        if (_stricmp(a.entry(i).name, "FX.TXD") != 0) continue;
        std::uint32_t len = 0;
        const std::uint8_t* b = a.blob(i, &len);
        Txd::Dictionary d;
        if (!b || !d.Decode(b, len)) return;
        for (std::uint32_t t = 0; t < d.count(); ++t) {
            const Txd::Texture& tx = d.texture(t);
            if (!g_tex_shadow && _stricmp(tx.name, "car_shadow") == 0)
                g_tex_shadow = static_cast<rw::Texture*>(TextureFromTxdTexture(tx));
            else if (!g_tex_beam && _stricmp(tx.name, "beam") == 0)
                g_tex_beam = static_cast<rw::Texture*>(TextureFromTxdTexture(tx));
        }
        return;
    }
}

// ---- per-model and per-frame inputs ----------------------------------------------------------
int   g_family = -1;
float g_p25[3] = {}, g_p28[3] = {};
bool  g_parts_ok = false;
CarFxInput g_cars[4];
bool       g_have[4] = {};

// RW matrix rows (right, up, at, pos) from a D3DMATRIX in memory order.
struct M { float r[3], u[3], a[3], p[3]; };
M FromD3D(const float* m) {
    M o;
    for (int k = 0; k < 3; ++k) {
        o.r[k] = m[0 + k]; o.u[k] = m[4 + k]; o.a[k] = m[8 + k]; o.p[k] = m[12 + k];
    }
    return o;
}

// RwMatrixRotate(m, (1,0,0), 90, rwCOMBINEPRECONCAT) (FUN_004c4d20, 0x0041fd6e / 0x0041fbbe):
// m = R x m, R = rows (1,0,0), (0,c,s), (0,-s,c). c = cos 90 is not exactly 0 in float; RW builds
// R from the angle, and so does this (the AABB check in PREREG section 2 holds to 1e-5 either way).
void RotX90Pre(M& m) {
    const float rad = 90.0f * 3.14159265358979323846f / 180.0f;
    const float c = std::cos(rad), s = std::sin(rad);
    float u[3], a[3];
    for (int k = 0; k < 3; ++k) {
        u[k] = c * m.u[k] + s * m.a[k];
        a[k] = -s * m.u[k] + c * m.a[k];
    }
    for (int k = 0; k < 3; ++k) { m.u[k] = u[k]; m.a[k] = a[k]; }
}
// RwMatrixTranslate(m, t, rwCOMBINEPRECONCAT) (FUN_004c51a0): pos += t.x r + t.y u + t.z a
void TransPre(M& m, const float t[3]) {
    for (int k = 0; k < 3; ++k) m.p[k] += t[0] * m.r[k] + t[1] * m.u[k] + t[2] * m.a[k];
}
// RwMatrixScale(m, s, rwCOMBINEPRECONCAT) (FUN_004c5010): rows scaled
void ScalePre(M& m, const float s[3]) {
    for (int k = 0; k < 3; ++k) { m.r[k] *= s[0]; m.u[k] *= s[1]; m.a[k] *= s[2]; }
}

// The car shadow tables FUN_0041fcc0 indexes by family (0x0041fcf5..0x0041fd34): scale at
// 0x005f5fe8 and translation at 0x005f6088, 12 bytes per family (read from the anchored binary).
const float kShadowScale[13][3] = {
    {0.55f, 0.90f, 1.0f}, {0.55f, 1.00f, 1.0f}, {0.55f, 0.95f, 1.0f}, {0.55f, 1.00f, 1.0f},
    {0.55f, 1.00f, 1.0f}, {0.45f, 0.70f, 1.0f}, {0.50f, 0.90f, 1.0f}, {0.55f, 0.95f, 1.0f},
    {0.55f, 1.00f, 1.0f}, {0.55f, 1.10f, 1.0f}, {0.55f, 0.95f, 1.0f}, {0.55f, 1.35f, 1.0f},
    {0.65f, 1.05f, 1.0f}};
const float kShadowTrans[13][3] = {
    {0.f, 0.000f, -0.01f}, {0.f, -0.050f, -0.01f}, {0.f, 0.000f, -0.01f}, {0.f, -0.050f, -0.01f},
    {0.f, 0.000f, -0.01f}, {0.f, 0.000f, -0.01f}, {0.f, 0.000f, -0.01f}, {0.f, -0.025f, -0.01f},
    {0.f, 0.000f, -0.01f}, {0.f, 0.050f, -0.01f}, {0.f, -0.025f, -0.01f}, {0.f, -0.050f, -0.01f},
    {0.f, -0.050f, -0.01f}};

// One RpPTank matrix particle: its matrix and RGBA colour (FUN_00476d00 stores both per element,
// count < capacity 4 from FUN_004770c0(b, 0x80c, 4, ...)).
struct Particle { M m; std::uint8_t rgba[4]; };
struct Batch { Particle p[4]; int n = 0; };

// __ftol (FUN_004a2c48 @ 0x004a2c48): truncation toward zero under the default control word; the
// colour byte is its low 8 bits.
std::uint8_t FtolByte(double v) { return static_cast<std::uint8_t>(static_cast<long long>(v) & 0xff); }

// RpPTank's D3D9 instance + render for matrix + colour particles with constant 2 texcoords:
// positions FUN_00537890 (v0 = p + r/2 - u/2, v1 = p - r/2 - u/2, v2 = p - r/2 + u/2,
// v3 = p + r/2 + u/2; 0.5 = [0x005cc32c]), indices (0,1,2)(0,2,3) (FUN_00536170), the colour on all
// four vertices (FUN_00537b20), uv (u0,v1)(u1,v1)(u1,v0)(u0,v0) from the constant pair at
// plugin+0xd8 (FUN_00537e60), which FUN_00476c10 sets to (0,0)-(1,1) (DAT_00613294). Drawn by
// FUN_00535cb0 with the batch's blend pair and vertex alpha on.
void DrawBatch(const Batch& b, rw::Texture* tex, int src, int dst) {
    if (b.n <= 0 || !tex) return;
    rw::RWDEVICE::Im3DVertex v[16];
    rw::uint16 idx[24];
    for (int i = 0; i < b.n; ++i) {
        const M& m = b.p[i].m;
        float A[3], B[3];
        for (int k = 0; k < 3; ++k) {
            A[k] = m.u[k] * 0.5f + m.r[k] * 0.5f;   // fVar7/8/4
            B[k] = m.r[k] * 0.5f - m.u[k] * 0.5f;   // fVar6/9/5
        }
        const float P[4][3] = {
            {m.p[0] + B[0], m.p[1] + B[1], m.p[2] + B[2]},
            {m.p[0] - A[0], m.p[1] - A[1], m.p[2] - A[2]},
            {m.p[0] - B[0], m.p[1] - B[1], m.p[2] - B[2]},
            {m.p[0] + A[0], m.p[1] + A[1], m.p[2] + A[2]}};
        const float UV[4][2] = {{0.f, 1.f}, {1.f, 1.f}, {1.f, 0.f}, {0.f, 0.f}};
        for (int q = 0; q < 4; ++q) {
            rw::RWDEVICE::Im3DVertex& o = v[i * 4 + q];
            o.setX(P[q][0]); o.setY(P[q][1]); o.setZ(P[q][2]);
            o.setNormalX(0.f); o.setNormalY(0.f); o.setNormalZ(0.f);
            o.setColor(b.p[i].rgba[0], b.p[i].rgba[1], b.p[i].rgba[2], b.p[i].rgba[3]);
            o.setU(UV[q][0]); o.setV(UV[q][1]);
        }
        const rw::uint16 s = static_cast<rw::uint16>(i * 4);
        const rw::uint16 tri[6] = {s, static_cast<rw::uint16>(s + 1), static_cast<rw::uint16>(s + 2),
                                   s, static_cast<rw::uint16>(s + 2), static_cast<rw::uint16>(s + 3)};
        for (int k = 0; k < 6; ++k) idx[i * 6 + k] = tri[k];
    }
    rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
    rw::SetRenderState(rw::TEXTUREFILTER, tex->getFilter());
    rw::SetRenderState(rw::TEXTUREADDRESSU, tex->getAddressU());
    rw::SetRenderState(rw::TEXTUREADDRESSV, tex->getAddressV());
    rw::SetRenderState(rw::VERTEXALPHA, 1);   // plugin+0xac = 1 (FUN_004770c0) -> RS 0xc = 1
    rw::SetRenderState(rw::SRCBLEND, src);
    rw::SetRenderState(rw::DESTBLEND, dst);
    rw::im3d::Transform(v, b.n * 4, nullptr, rw::im3d::VERTEXUV);
    static const float kIdent[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    if (rw_drawrec_pre) {
        DrawRec::SetNextGeometry(v, sizeof(v[0]), idx, static_cast<unsigned>(b.n * 6));
        rw_drawrec_pre(tex->name, kIdent);
    }
    rw::im3d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, idx, b.n * 6);
    if (rw_drawrec_post) rw_drawrec_post();
    rw::im3d::End();
}

}  // namespace

bool RaceFx_CarShadowEnabled() { static const bool on = !EnvOff("MASHED_CAR_SHADOW"); return on; }
bool RaceFx_BeamEnabled()      { static const bool on = !EnvOff("MASHED_CAR_BEAM");   return on; }

int RaceFx_FamilyOfPiz(const char* piz_path) {
    // The 13 family names of the descriptor table at 0x005f37a8 (the first 16 bytes of every
    // 0x84-byte entry; entries 6k..6k+5 share family k, FUN_0041f330 @ 0x0041f330).
    static const char* const kFam[13] = {"Shuriken", "Advantag", "Atmos", "Stallion", "Bullet",
                                         "Sputter", "Cooler", "Shorty", "Crusader", "Creeper",
                                         "BamBam", "Kustom", "Formula"};
    if (!piz_path) return -1;
    const char* s = piz_path;
    for (const char* q = piz_path; *q; ++q)
        if (*q == '/' || *q == '\\') s = q + 1;
    char base[64] = {};
    std::size_t n = 0;
    while (s[n] && s[n] != '.' && n + 1 < sizeof(base)) { base[n] = s[n]; ++n; }
    for (int f = 0; f < 13; ++f)
        if (_stricmp(base, kFam[f]) == 0) return f;
    return -1;
}

void RaceFx_SetCarModel(int family, const float part25[3], const float part28[3], bool parts_ok) {
    g_family = family;
    std::memcpy(g_p25, part25, sizeof(g_p25));
    std::memcpy(g_p28, part28, sizeof(g_p28));
    g_parts_ok = parts_ok;
}

void RaceFx_QueueCar(int slot, const CarFxInput& in) {
    if (slot < 0 || slot > 3) return;
    g_cars[slot] = in;
    g_have[slot] = true;
}

void RaceFx_DrawCarFx(void* rw_camera) {
    const bool want_shadow = RaceFx_CarShadowEnabled();
    const bool want_beam = RaceFx_BeamEnabled();
    bool any = false;
    for (bool h : g_have) any = any || h;
    if (!any || (!want_shadow && !want_beam)) { for (bool& h : g_have) h = false; return; }
    LoadTextures();
    rw::Camera* cam = static_cast<rw::Camera*>(rw_camera);

    Batch shadow, beam;
    for (int i = 0; i < 4; ++i) {
        if (!g_have[i]) continue;
        const CarFxInput& c = g_cars[i];
        // FUN_00420050 0x00420081: RwCameraFrustumTestSphere(camera, rec+0x258) == 0 returns
        // before any per-car draw. rec+0x258..0x260 = the body frame's LTM translation
        // (Race/CarRegistrant.cpp), rec+0x264 = 0x3f088c2d (measured, U-9218).
        if (cam) {
            rw::Sphere sp;
            sp.center.x = c.body[12]; sp.center.y = c.body[13]; sp.center.z = c.body[14];
            const std::uint32_t rb = 0x3f088c2du;
            std::memcpy(&sp.radius, &rb, sizeof(sp.radius));
            if (cam->frustumTestSphere(&sp) == rw::Camera::SPHEREOUTSIDE) continue;
        }
        // ---- FUN_0041faf0 @ 0x0041faf0: the headlight beam (runs first, 0x004200c8) ----------
        if (want_beam && c.lights && g_parts_ok && beam.n < 4) {
            const M body = FromD3D(c.body);
            // A, B = LTMs of part 25 / part 28's frames (rec+0x64 / rec+0x70 -> +4 -> RwFrameGetLTM,
            // 0x0041fb45..0x0041fb7b): the parts' frames are translations under the body frame.
            float pa[3], pb[3];
            for (int k = 0; k < 3; ++k) {
                pa[k] = body.p[k] + g_p25[0] * body.r[k] + g_p25[1] * body.u[k] + g_p25[2] * body.a[k];
                pb[k] = body.p[k] + g_p28[0] * body.r[k] + g_p28[1] * body.u[k] + g_p28[2] * body.a[k];
            }
            M m = body;            // B keeps the body frame's rotation
            std::memcpy(m.p, pb, sizeof(m.p));
            const float d[3] = {pa[0] - pb[0], pa[1] - pb[1], pa[2] - pb[2]};   // 0x0041fb80..0x0041fbba
            RotX90Pre(m);                                                          // 0x0041fbbe
            const float sc[3] = {0.75f, 2.0f, 1.0f};   // 0x3f400000, 0x40000000, 0x3f800000 (0x0041fb27..)
            const float s = sc[1] * 0.5f;              // 0x0041fbc3: scale.y * [0x005cc32c]
            for (int k = 0; k < 3; ++k) m.p[k] = m.p[k] + s * m.u[k] + d[k] * 0.5f;   // ..0x0041fc31
            ScalePre(m, sc);                                                       // 0x0041fc35
            // alpha = ftol(max(dot(camera frame at, m.at), [0x5d757c] = 0) * [0x5cd27c] = 32)
            float camat[3] = {0.f, 0.f, 1.f};
            if (cam && cam->getFrame()) {
                const rw::Matrix* cm = &cam->getFrame()->matrix;
                camat[0] = cm->at.x; camat[1] = cm->at.y; camat[2] = cm->at.z;
            }
            double dot = camat[1] * m.a[1];
            dot = dot + camat[0] * m.a[0];
            dot = dot + camat[2] * m.a[2];
            if (!(0.0 < dot)) dot = 0.0;
            Particle& p = beam.p[beam.n++];
            p.m = m;
            p.rgba[0] = p.rgba[1] = p.rgba[2] = 0xff;   // 0x0041faf6..0x0041fb04
            p.rgba[3] = FtolByte(dot * 32.0);
        }
        // ---- FUN_0041fcc0 @ 0x0041fcc0: the car shadow --------------------------------------
        if (want_shadow && shadow.n < 4) {
            const int f = (g_family >= 0 && g_family < 13) ? g_family : 1;
            M m = FromD3D(c.root);                       // 0x0041fd3c..0x0041fd58
            RotX90Pre(m);                                // 0x0041fd6e
            TransPre(m, kShadowTrans[f]);                // 0x0041fd7f
            ScalePre(m, kShadowScale[f]);                // 0x0041fd90
            // alpha = ftol(max(up.x * 0 + up.y + up.z * 0, 0) * rec+0x288 * 255) on the root's
            // modelling matrix (esi = frame+0x10 here), 0x0041fd9f..0x0041fddd
            const M root = FromD3D(c.root);
            double v = root.u[0] * 0.0 + root.u[1];
            v = v + root.u[2] * 0.0;
            if (!(0.0 < v)) v = 0.0;
            v = v * c.fade;
            v = v * 255.0;                                // [0x5cd04c]
            Particle& p = shadow.p[shadow.n++];
            p.m = m;
            p.rgba[0] = p.rgba[1] = p.rgba[2] = 0;       // 0x0041fcc9..0x0041fcd8
            p.rgba[3] = FtolByte(v);
        }
    }
    for (bool& h : g_have) h = false;
    if (shadow.n == 0 && beam.n == 0) return;

    // FUN_00410b30 @ 0x00410b30 state at FUN_0041ebb0's call: RW cull mode 2 (0x00410b36..0x00410b3a, RS
    // 0x14), fog off (0x00410b9f..0x00410ba3, RS 0xe = 0); the device's alpha test GREATER / 0 from
    // FUN_004d5bc0 (0x004d60ec / 0x004d611a); depth test as left by the scene. FUN_0041ebb0
    // turns ZWRITE off (RS 8 = 0) around the two draws.
    rw::int32 sv_zw = rw::GetRenderState(rw::ZWRITEENABLE);
    rw::int32 sv_zt = rw::GetRenderState(rw::ZTESTENABLE);
    rw::int32 sv_fog = rw::GetRenderState(rw::FOGENABLE);
    rw::int32 sv_cull = rw::GetRenderState(rw::CULLMODE);
    rw::int32 sv_af = rw::GetRenderState(rw::ALPHATESTFUNC);
    rw::int32 sv_ar = rw::GetRenderState(rw::ALPHATESTREF);
    rw::int32 sv_va = rw::GetRenderState(rw::VERTEXALPHA);
    rw::int32 sv_sb = rw::GetRenderState(rw::SRCBLEND);
    rw::int32 sv_db = rw::GetRenderState(rw::DESTBLEND);
    rw::SetRenderState(rw::ZTESTENABLE, 1);
    rw::SetRenderState(rw::ZWRITEENABLE, 0);
    rw::SetRenderState(rw::FOGENABLE, 0);
    rw::SetRenderState(rw::CULLMODE, rw::CULLBACK);
    rw::SetRenderState(rw::ALPHATESTFUNC, rw::ALPHAGREATER);
    rw::SetRenderState(rw::ALPHATESTREF, 0);
    // car_shadow: FUN_004770c0 blend default DAT_006132c4 / c8 = 5 / 6; beam: FUN_00476cb0(b, 5, 2)
    DrawBatch(shadow, g_tex_shadow, rw::BLENDSRCALPHA, rw::BLENDINVSRCALPHA);
    DrawBatch(beam, g_tex_beam, rw::BLENDSRCALPHA, rw::BLENDONE);
    rw::SetRenderState(rw::ZWRITEENABLE, sv_zw);
    rw::SetRenderState(rw::ZTESTENABLE, sv_zt);
    rw::SetRenderState(rw::FOGENABLE, sv_fog);
    rw::SetRenderState(rw::CULLMODE, sv_cull);
    rw::SetRenderState(rw::ALPHATESTFUNC, sv_af);
    rw::SetRenderState(rw::ALPHATESTREF, sv_ar);
    rw::SetRenderState(rw::VERTEXALPHA, sv_va);
    rw::SetRenderState(rw::SRCBLEND, sv_sb);
    rw::SetRenderState(rw::DESTBLEND, sv_db);
}

// =============================================================================================
// [rfx2] The dynamic-object shadows (C04 "RWObjShad", verify/rfx2_20261016/PREREG_rfx2.md).
// =============================================================================================
namespace {

// ---- SFX.piz textures by name (the FX passes' "RWObjShad" etc.; every TXD of the archive) ------
std::vector<rw::Texture*> g_sfx_tex;
bool                      g_sfx_tried = false;
rw::Texture* SfxTexture(const char* name) {
    if (!g_sfx_tried) {
        g_sfx_tried = true;
        Piz::Archive a;
        if (a.Load("original/TOASTART/Common/SFX.piz")) {
            for (std::uint32_t i = 0; i < a.count(); ++i) {
                const char* n = a.entry(i).name;
                const std::size_t ln = std::strlen(n);
                if (ln < 4 || _stricmp(n + ln - 4, ".TXD") != 0) continue;
                std::uint32_t len = 0;
                const std::uint8_t* b = a.blob(i, &len);
                Txd::Dictionary d;
                if (!b || !d.Decode(b, len)) continue;
                static const char* const kWant[] = {"RWObjShad", "flatshad", "Headlight",
                                                    "lensflare-fs8", "VehicleIcons"};
                for (std::uint32_t t = 0; t < d.count(); ++t)
                    for (const char* w : kWant)
                        if (_stricmp(d.texture(t).name, w) == 0) {
                            bool have = false;
                            for (rw::Texture* x : g_sfx_tex) have = have || _stricmp(x->name, w) == 0;
                            if (!have)
                                if (rw::Texture* x = static_cast<rw::Texture*>(TextureFromTxdTexture(d.texture(t))))
                                    g_sfx_tex.push_back(x);
                        }
            }
        }
    }
    for (rw::Texture* t : g_sfx_tex)
        if (_stricmp(t->name, name) == 0) return t;
    return nullptr;
}

// A batch of any size (RpPTank matrix + colour particles, constant UV): the DrawBatch recipe.
void DrawParticles(const std::vector<Particle>& ps, rw::Texture* tex, int src, int dst) {
    if (ps.empty() || !tex) return;
    std::vector<rw::RWDEVICE::Im3DVertex> v(ps.size() * 4);
    std::vector<rw::uint16> idx(ps.size() * 6);
    for (std::size_t i = 0; i < ps.size(); ++i) {
        const M& m = ps[i].m;
        float A[3], B[3];
        for (int k = 0; k < 3; ++k) {
            A[k] = m.u[k] * 0.5f + m.r[k] * 0.5f;
            B[k] = m.r[k] * 0.5f - m.u[k] * 0.5f;
        }
        const float P[4][3] = {
            {m.p[0] + B[0], m.p[1] + B[1], m.p[2] + B[2]},
            {m.p[0] - A[0], m.p[1] - A[1], m.p[2] - A[2]},
            {m.p[0] - B[0], m.p[1] - B[1], m.p[2] - B[2]},
            {m.p[0] + A[0], m.p[1] + A[1], m.p[2] + A[2]}};
        const float UV[4][2] = {{0.f, 1.f}, {1.f, 1.f}, {1.f, 0.f}, {0.f, 0.f}};
        for (int q = 0; q < 4; ++q) {
            rw::RWDEVICE::Im3DVertex& o = v[i * 4 + q];
            o.setX(P[q][0]); o.setY(P[q][1]); o.setZ(P[q][2]);
            o.setNormalX(0.f); o.setNormalY(0.f); o.setNormalZ(0.f);
            o.setColor(ps[i].rgba[0], ps[i].rgba[1], ps[i].rgba[2], ps[i].rgba[3]);
            o.setU(UV[q][0]); o.setV(UV[q][1]);
        }
        const rw::uint16 s = static_cast<rw::uint16>(i * 4);
        const rw::uint16 tri[6] = {s, static_cast<rw::uint16>(s + 1), static_cast<rw::uint16>(s + 2),
                                   s, static_cast<rw::uint16>(s + 2), static_cast<rw::uint16>(s + 3)};
        for (int k = 0; k < 6; ++k) idx[i * 6 + k] = tri[k];
    }
    rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
    rw::SetRenderState(rw::TEXTUREFILTER, tex->getFilter());
    rw::SetRenderState(rw::TEXTUREADDRESSU, tex->getAddressU());
    rw::SetRenderState(rw::TEXTUREADDRESSV, tex->getAddressV());
    rw::SetRenderState(rw::VERTEXALPHA, 1);
    rw::SetRenderState(rw::SRCBLEND, src);
    rw::SetRenderState(rw::DESTBLEND, dst);
    rw::im3d::Transform(v.data(), static_cast<rw::int32>(v.size()), nullptr, rw::im3d::VERTEXUV);
    static const float kIdent[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    if (rw_drawrec_pre) {
        DrawRec::SetNextGeometry(v.data(), sizeof(v[0]), idx.data(), static_cast<unsigned>(idx.size()));
        rw_drawrec_pre(tex->name, kIdent);
    }
    rw::im3d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, idx.data(), static_cast<rw::int32>(idx.size()));
    if (rw_drawrec_post) rw_drawrec_post();
    rw::im3d::End();
}

struct ObjShadow {
    float sphere[4] = {};   // the atomic's world bounding sphere
    bool  flag = false;     // DAT_006cad50[i]
    M     m{};              // DAT_006c3670[i] (the particle matrix)
    int   state = 0;        // DAT_006c3350[i] (FUN_004c1b40: 0 out, 1 boundary, 2 in)
    int   prev = 0;         // DAT_006c3030[i]
};
std::vector<ObjShadow> g_objshad;
std::uint32_t          g_objshad_frame = 0;

// FUN_004726f0 @ 0x004726f0: (p.z q.z + p.x q.x) + p.y q.y, clamped to [-1 ([0x5cc33c]), 1 ([0x5cc320])].
float DotClamp(const float* p, const float* q) {
    float v = p[2] * q[2] + p[0] * q[0];
    v = v + p[1] * q[1];
    if (v < -1.0f) return -1.0f;
    if (1.0f < v) v = 1.0f;
    return v;
}
// FUN_00547230 @ 0x00547230: one-sided ray / triangle (det > [0x5e4574] 1e-8, the bounds scaled by
// det * [0x5cea1c] -1e-5); t / det on a hit.
bool RayTri(const float* o, const float* d, const float* v0, const float* v1, const float* v2, float* t) {
    const float e1[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]};
    const float e2[3] = {v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]};
    const float p12 = d[1] * e2[2] - d[2] * e2[1];
    const float p14 = d[2] * e2[0] - e2[2] * d[0];
    const float p11 = e2[1] * d[0] - d[1] * e2[0];
    const float det = p11 * e1[2] + p12 * e1[0] + p14 * e1[1];
    if (!(1e-8f < det)) return false;
    std::uint32_t kb = 0xb727c5acu;                   // _DAT_005cea1c
    float k;
    std::memcpy(&k, &kb, sizeof k);
    const float lo = det * k, hi = det - lo;
    const float s[3] = {o[0] - v0[0], o[1] - v0[1], o[2] - v0[2]};
    const float u = s[2] * p11 + s[0] * p12 + s[1] * p14;
    if (u < lo || hi < u) return false;
    const float q11 = s[1] * e1[2] - s[2] * e1[1];
    const float q3 = s[2] * e1[0] - s[0] * e1[2];
    const float q1 = s[0] * e1[1] - s[1] * e1[0];
    const float v = d[2] * q1 + q11 * d[0] + d[1] * q3;
    if (!(lo <= v) || hi < u + v) return false;
    const float tt = q1 * e2[2] + q11 * e2[0] + q3 * e2[1];
    if (!(lo <= tt) || hi < tt) return false;
    *t = tt / det;
    return true;
}
// RW row-vector transform v * M (RwV3dTransformPoints, FUN_004c3df0 -> RwGlobals +0x14).
void XformPoint(const float* m16, const float* v, float* o) {
    for (int k = 0; k < 3; ++k) o[k] = v[0] * m16[0 + k] + v[1] * m16[4 + k] + v[2] * m16[8 + k] + m16[12 + k];
}
// RwMatrixInvert (FUN_004c4dc0 @ 0x004c4dc0) for a general affine LTM (the light frame's LTM flags
// read 0 on the original: not identity, not orthonormal, so FUN_004c4eb0's general path).
// [UNCERTAIN U-9707] FUN_004c4eb0 is not transcribed: cofactor inverse here (equal in exact arithmetic).
void InvertAffine(const float* a, float* o) {
    const float r[9] = {a[0], a[1], a[2], a[4], a[5], a[6], a[8], a[9], a[10]};
    const double c00 = (double)r[4] * r[8] - (double)r[5] * r[7], c01 = (double)r[5] * r[6] - (double)r[3] * r[8],
                 c02 = (double)r[3] * r[7] - (double)r[4] * r[6];
    const double det = r[0] * c00 + r[1] * c01 + r[2] * c02;
    const double id = det != 0.0 ? 1.0 / det : 0.0;
    float ir[9];
    ir[0] = (float)(c00 * id); ir[1] = (float)(((double)r[2] * r[7] - (double)r[1] * r[8]) * id);
    ir[2] = (float)(((double)r[1] * r[5] - (double)r[2] * r[4]) * id);
    ir[3] = (float)(c01 * id); ir[4] = (float)(((double)r[0] * r[8] - (double)r[2] * r[6]) * id);
    ir[5] = (float)(((double)r[2] * r[3] - (double)r[0] * r[5]) * id);
    ir[6] = (float)(c02 * id); ir[7] = (float)(((double)r[1] * r[6] - (double)r[0] * r[7]) * id);
    ir[8] = (float)(((double)r[0] * r[4] - (double)r[1] * r[3]) * id);
    for (int k = 0; k < 16; ++k) o[k] = 0.f;
    o[0] = ir[0]; o[1] = ir[1]; o[2] = ir[2];
    o[4] = ir[3]; o[5] = ir[4]; o[6] = ir[5];
    o[8] = ir[6]; o[9] = ir[7]; o[10] = ir[8];
    for (int k = 0; k < 3; ++k) o[12 + k] = -(a[12] * o[0 + k] + a[13] * o[4 + k] + a[14] * o[8 + k]);
    o[15] = 1.f;
}

// FUN_00487280 @ 0x00487280 for one static object. The original runs it at the race reset
// (FUN_00480100, triangle cache 0x006c74f8 + i*0x28 cleared, so through the world query) and again
// when the object first turns visible on a parity frame (FUN_00481a30, cache filled, so through
// FUN_00547230 on the cached triangle). Both runs see the same inputs here, so the drawn matrix is the
// second run's. Returns DAT_006cad50[i].
bool ObjShadowMatrix(const ObjShadowIn& in, const float* L, const float* Linv, const ObjShadowSoup& soup,
                     M* out) {
    const float* at = L + 8;
    const float start[3] = {in.ltm[12], in.ltm[13], in.ltm[14]};            // param_2 + 0x30
    const float dl[3] = {2.0f * at[0], at[1] * 2.0f, at[2] * 2.0f};        // [0x5cf47c] 2.0
    const float end[3] = {dl[0] + start[0], start[1] + dl[1], start[2] + dl[2]};
    // ---- the world query, FUN_00538c80(DAT_0080332c, line, FUN_00487180, data) ----------------
    // [UNCERTAIN U-9707] RW's BSP walk stops at the FIRST intersection its sector order finds
    // (FUN_00487180 returns 0); the soup here takes the nearest, two-sided (CollisionPrelitSample's rule).
    const float dd[3] = {end[0] - start[0], end[1] - start[1], end[2] - start[2]};
    float best = 2.f;
    std::size_t bi = static_cast<std::size_t>(-1);
    for (std::size_t i = 0; i < soup.ntri; ++i) {
        const float* v0 = &soup.verts[soup.tris[i * 3 + 0] * 3];
        const float* v1 = &soup.verts[soup.tris[i * 3 + 1] * 3];
        const float* v2 = &soup.verts[soup.tris[i * 3 + 2] * 3];
        const float e1[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]};
        const float e2[3] = {v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]};
        const float p[3] = {dd[1] * e2[2] - dd[2] * e2[1], dd[2] * e2[0] - dd[0] * e2[2], dd[0] * e2[1] - dd[1] * e2[0]};
        const float det = e1[0] * p[0] + e1[1] * p[1] + e1[2] * p[2];
        if (det > -1e-12f && det < 1e-12f) continue;
        const float inv = 1.0f / det;
        const float s[3] = {start[0] - v0[0], start[1] - v0[1], start[2] - v0[2]};
        const float u = (s[0] * p[0] + s[1] * p[1] + s[2] * p[2]) * inv;
        if (u < 0.0f || u > 1.0f) continue;
        const float q[3] = {s[1] * e1[2] - s[2] * e1[1], s[2] * e1[0] - s[0] * e1[2], s[0] * e1[1] - s[1] * e1[0]};
        const float w = (dd[0] * q[0] + dd[1] * q[1] + dd[2] * q[2]) * inv;
        if (w < 0.0f || u + w > 1.0f) continue;
        const float t = (e2[0] * q[0] + e2[1] * q[1] + e2[2] * q[2]) * inv;
        if (t < 0.0f || t > 1.0f || t >= best) continue;
        best = t; bi = i;
    }
    if (bi == static_cast<std::size_t>(-1)) return false;                  // local_88 == 0
    // FUN_0045c110 @ 0x0045c110 on the hit's material: colour 0xff010101 or 0xffff0080 -> 1 (no
    // shadow). [UNCERTAIN U-9707] the 0xffNN3cff / 0xffNNffff codes read the runtime table
    // DAT_00691508 (FUN_00472550); none of them is treated as blocking here.
    const std::uint32_t rgba = soup.tri_rgba ? soup.tri_rgba[bi] : 0u;
    if (rgba == 0xff010101u || rgba == 0xffff0080u) return false;
    const float* v0 = &soup.verts[soup.tris[bi * 3 + 0] * 3];
    const float* v1 = &soup.verts[soup.tris[bi * 3 + 1] * 3];
    const float* v2 = &soup.verts[soup.tris[bi * 3 + 2] * 3];
    // RpCollisionTriangle (FUN_00487180's param_3, copied to the cache): normal = the LUT-normalized
    // (v1 - v0) x (v2 - v0), point = v0, vertices v0, v1, v2 (the witness verify/rfx2_20261016,
    // 23/23 TRAINING caches)
    float n[3];
    {
        const float e1[3] = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]};
        const float e2[3] = {v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]};
        const float c[3] = {e1[1] * e2[2] - e1[2] * e2[1], e1[2] * e2[0] - e1[0] * e2[2], e1[0] * e2[1] - e1[1] * e2[0]};
        Math::LutNormalize(n, c);
    }
    // ---- the hit point (local_94 / local_90 / local_8c) ------------------------------------------
    float hp[3];
    float t2 = 0.f;
    if (RayTri(start, dl, v0, v1, v2, &t2)) {
        // the cached-triangle path, 0x004872f9.. (FUN_00547230 hit): (x * [0x5ce18c] 0.02 + d * t) + o,
        // with [0x5cc328] 0.01 added in y
        hp[0] = n[0] * 0.02f + (end[0] - start[0]) * t2 + start[0];
        hp[1] = n[1] * 0.02f + 0.01f + (end[1] - start[1]) * t2 + start[1];
        hp[2] = n[2] * 0.02f + (end[2] - start[2]) * t2 + start[2];
    } else {
        // FUN_00487180 @ 0x00487180: (end - start) * t + start, then + n * 0.02
        for (int k = 0; k < 3; ++k) hp[k] = (end[k] - start[k]) * best + start[k];
        for (int k = 0; k < 3; ++k) hp[k] = n[k] * 0.02f + hp[k];
    }
    // ---- LAB_0048761d: FUN_0045c030 @ 0x0045c030 (right = |v0 - v1|, up = |n x (v0 - v1)|, at = n) ----
    M m{};
    {
        const float e[3] = {v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2]};
        const float c[3] = {e[2] * n[1] - e[1] * n[2], e[0] * n[2] - e[2] * n[0], e[1] * n[0] - e[0] * n[1]};
        Math::LutNormalize(m.r, e);
        Math::LutNormalize(m.u, c);
        std::memcpy(m.a, n, sizeof(m.a));
        m.p[0] = m.p[1] = m.p[2] = 0.f;                // FUN_004c51a0(m, (0,0,0), POSTCONCAT)
    }
    // P = light up + hit; ratio = clamp(n . (v0 - P)) / clamp(n . light at) (0 -> [0x5cc328] 0.01)
    const float* lu = L + 4;
    const float P[3] = {lu[0] + hp[0], lu[1] + hp[1], lu[2] + hp[2]};
    const float w[3] = {v0[0] - P[0], v0[1] - P[1], v0[2] - P[2]};
    const float r1 = DotClamp(n, w);
    float r2 = DotClamp(n, at);
    if (r2 == 0.0f) r2 = 0.01f;
    const float ratio = r1 / r2;
    const float Q[3] = {at[0] * ratio + P[0], ratio * at[1] + P[1], ratio * at[2] + P[2]};
    const float D[3] = {Q[0] - hp[0], Q[1] - hp[1], Q[2] - hp[2]};
    Math::LutNormalize(m.r, D);                         // FUN_004c39b0(param_4, &local_d0)
    m.u[0] = m.a[1] * m.r[2] - m.r[1] * m.a[2];         // up = at x right (0x0048771b..)
    m.u[1] = m.a[2] * m.r[0] - m.a[0] * m.r[2];
    m.u[2] = m.a[0] * m.r[1] - m.a[1] * m.r[0];
    // the eight bbox corners through the object LTM and the inverse light LTM (FUN_004c3df0 twice)
    const float* b = in.bbox;
    const float C[8][3] = {{b[3], b[4], b[5]}, {b[0], b[4], b[5]}, {b[0], b[1], b[5]}, {b[3], b[1], b[5]},
                           {b[3], b[4], b[2]}, {b[0], b[4], b[2]}, {b[0], b[1], b[2]}, {b[3], b[1], b[2]}};
    float maxy = -1000.f, miny = 1000.f, maxx = -1000.f, minx = 1000.f;   // [0x5cda84] / [0x5cc9fc]
    for (int c = 0; c < 8; ++c) {
        float wv[3], lv[3];
        XformPoint(in.ltm, C[c], wv);
        XformPoint(Linv, wv, lv);
        if (maxy < lv[1]) maxy = lv[1];
        if (lv[1] < miny) miny = lv[1];
        if (maxx < lv[0]) maxx = lv[0];
        if (lv[0] < minx) minx = lv[0];
    }
    // scale (span y / tan(acos(light at . right)), span x * [0x5cc9ac] 1.4, 1), rwCOMBINEPRECONCAT
    float cosv = at[2] * m.r[2] + at[0] * m.r[0];
    cosv = cosv + at[1] * m.r[1];
    const float tn = static_cast<float>(std::tan(std::acos(static_cast<double>(cosv))));
    const float sx = (maxy - miny) / tn;
    const float sy = (maxx - minx) * 1.4f;
    for (int k = 0; k < 3; ++k) { m.r[k] *= sx; m.u[k] *= sy; }
    // translate (POSTCONCAT) by hit + (sx * [0x5cc56c] 0.1) * scaled right
    const float k1 = sx * 0.1f;
    for (int k = 0; k < 3; ++k) m.p[k] = hp[k] + k1 * m.r[k];
    *out = m;
    return true;
}

}  // namespace

bool RaceFx_ObjShadowEnabled() { static const bool on = !EnvOff("MASHED_OBJ_SHADOW"); return on; }

void RaceFx_SetObjShadows(const ObjShadowIn* objs, int n, const float light[16], const ObjShadowSoup& soup) {
    g_objshad.clear();
    if (!RaceFx_ObjShadowEnabled() || !objs || n <= 0 || !light) return;
    float Linv[16];
    InvertAffine(light, Linv);
    g_objshad.resize(static_cast<std::size_t>(n));
    int nflag = 0;
    for (int i = 0; i < n; ++i) {
        ObjShadow& o = g_objshad[static_cast<std::size_t>(i)];
        std::memcpy(o.sphere, objs[i].sphere, sizeof(o.sphere));
        o.flag = ObjShadowMatrix(objs[i], light, Linv, soup, &o.m);
        nflag += o.flag ? 1 : 0;
    }
    if (std::FILE* f = std::fopen("log/librw_race.txt", "a")) {
        std::fprintf(f, "[rfx2] object shadows: %d objects, %d with a shadow (DAT_006cad50)\n", n, nflag);
        std::fclose(f);
    }
}

void RaceFx_SetObjShadowFrame(std::uint32_t f101c) { g_objshad_frame = f101c; }

void RaceFx_DrawWorldFx(void* rw_camera) {
    if (g_objshad.empty() || !RaceFx_ObjShadowEnabled()) return;
    rw::Camera* cam = static_cast<rw::Camera*>(rw_camera);
    if (!cam || !cam->getFrame()) return;
    // ---- FUN_00481a30 @ 0x00481a30, per object i ---------------------------------------------
    // [UNCERTAIN U-9707] the original runs this in the tick, against the camera frame's modelling
    // matrix (FUN_00467210(0)) and the frustum planes of the last RwCameraBeginUpdate; here both are
    // this frame's render camera. DAT_007f0fd0 == 5's branch (0x00481ab3) is not taken in the modes
    // the standalone runs; FUN_00484cf0 / FUN_004818d0 (physics registration, over-speed break) and
    // the per-frame recompute of a moving object are not modelled (the objects are static).
    const rw::Matrix& cm = cam->getFrame()->matrix;
    std::vector<Particle> ps;
    for (std::size_t i = 0; i < g_objshad.size(); ++i) {
        ObjShadow& o = g_objshad[i];
        const bool par = (static_cast<std::uint32_t>(i) & 1u) == (g_objshad_frame & 1u);   // 0x00481a80..
        const float dx = o.sphere[0] - cm.pos.x, dy = o.sphere[1] - cm.pos.y, dz = o.sphere[2] - cm.pos.z;
        float d2 = dz * dz + dx * dx;
        d2 = d2 + dy * dy;
        if (!(d2 < 2500.0f)) continue;                                   // [0x5cd098]
        o.prev = o.state;                                                // 0x00481ba8
        if (par) {
            rw::Sphere s;
            s.center.x = o.sphere[0]; s.center.y = o.sphere[1]; s.center.z = o.sphere[2];
            s.radius = o.sphere[3] * 10.0f;                              // [0x5cc55c]
            o.state = cam->frustumTestSphere(&s);                        // FUN_004c1b40
        }
        if (o.state != 2 && o.state != 1) continue;
        if (!o.flag) continue;
        // the angle gate 0x00481d03..0x00481d24: acos(clamp(at . (0,1,0) [0x6146fc])) < [0x5cd07c]
        const float up[3] = {0.f, 1.f, 0.f};
        const float a = static_cast<float>(std::acos(static_cast<double>(DotClamp(o.m.a, up))));
        if (!(a < 0.785398185f)) continue;
        if (ps.size() >= 50) continue;                                   // DAT_00703070 (witness: 50)
        // colour (0, 0, 0, ftol((1 - a * [0x5cf234]) + 1) * [0x5cd6d4] 64)), FUN_00487d00
        float al = 1.0f - a * 1.27323949f;
        al = al + 1.0f;
        al = al * 64.0f;
        Particle p;
        p.m = o.m;
        p.rgba[0] = p.rgba[1] = p.rgba[2] = 0;
        p.rgba[3] = FtolByte(al);
        ps.push_back(p);
    }
    if (ps.empty()) return;
    // ---- FUN_00487d70 @ 0x00487d70: cull NONE (RS 0x14 = 1), ZWRITE off, the batch's 5 / 6 blend
    // (FUN_00487150 -> FUN_00476cb0(&DAT_00703068, 5, 6)); fog is still off from FUN_00410b30
    // (0x00410b9f) and the device alpha test is GREATER / 0 as for the car FX.
    rw::int32 sv_zw = rw::GetRenderState(rw::ZWRITEENABLE);
    rw::int32 sv_zt = rw::GetRenderState(rw::ZTESTENABLE);
    rw::int32 sv_fog = rw::GetRenderState(rw::FOGENABLE);
    rw::int32 sv_cull = rw::GetRenderState(rw::CULLMODE);
    rw::int32 sv_af = rw::GetRenderState(rw::ALPHATESTFUNC);
    rw::int32 sv_ar = rw::GetRenderState(rw::ALPHATESTREF);
    rw::int32 sv_va = rw::GetRenderState(rw::VERTEXALPHA);
    rw::int32 sv_sb = rw::GetRenderState(rw::SRCBLEND);
    rw::int32 sv_db = rw::GetRenderState(rw::DESTBLEND);
    rw::SetRenderState(rw::ZTESTENABLE, 1);
    rw::SetRenderState(rw::ZWRITEENABLE, 0);
    rw::SetRenderState(rw::FOGENABLE, 0);
    rw::SetRenderState(rw::CULLMODE, rw::CULLNONE);
    rw::SetRenderState(rw::ALPHATESTFUNC, rw::ALPHAGREATER);
    rw::SetRenderState(rw::ALPHATESTREF, 0);
    DrawParticles(ps, SfxTexture("RWObjShad"), rw::BLENDSRCALPHA, rw::BLENDINVSRCALPHA);
    rw::SetRenderState(rw::ZWRITEENABLE, sv_zw);
    rw::SetRenderState(rw::ZTESTENABLE, sv_zt);
    rw::SetRenderState(rw::FOGENABLE, sv_fog);
    rw::SetRenderState(rw::CULLMODE, sv_cull);
    rw::SetRenderState(rw::ALPHATESTFUNC, sv_af);
    rw::SetRenderState(rw::ALPHATESTREF, sv_ar);
    rw::SetRenderState(rw::VERTEXALPHA, sv_va);
    rw::SetRenderState(rw::SRCBLEND, sv_sb);
    rw::SetRenderState(rw::DESTBLEND, sv_db);
}

// =============================================================================================
// [rfx part B] The in-race HUD (C08): team panels + start lights under the HUD camera.
// =============================================================================================
namespace {

HudInputs g_hud;
bool      g_hud_set = false;

// ---- MASHED_HUD_REPLAY (PREREG_rfx.md amendment 1) -------------------------------------------
struct HudReplay {
    bool  on = false;
    HudInputs cfg;          // mode / slot / score inputs measured on the original
    int   state = 0;        // this row's ba8c
    float t = 0.f;          // DAT_0063d588 before this row's increment
    float acc = 0.f;        // running DAT_0063d588
    int   prev = -1;
};
HudReplay& Rep() {
    static HudReplay r = [] {
        HudReplay x;
        const char* p = std::getenv("MASHED_HUD_REPLAY");
        if (!p || !p[0]) return x;
        std::FILE* f = std::fopen(p, "r");
        if (!f) return x;
        char line[256];
        while (std::fgets(line, sizeof(line), f)) {
            char key[32] = {};
            int v[4] = {0, 0, 0, 0};
            const int n = std::sscanf(line, "%31s %d %d %d %d", key, &v[0], &v[1], &v[2], &v[3]);
            if (n < 2 || key[0] == '#') continue;
            if (!std::strcmp(key, "team_play")) x.cfg.team_play = v[0];
            else if (!std::strcmp(key, "round_type")) x.cfg.round_type = v[0];
            else if (!std::strcmp(key, "mode_fd0")) x.cfg.mode_fd0 = v[0];
            else if (!std::strcmp(key, "n4d0")) x.cfg.n4d0 = v[0];
            else if (!std::strcmp(key, "score") && n == 5) for (int i = 0; i < 4; ++i) x.cfg.score[i] = v[i];
            else if (!std::strcmp(key, "player") && n == 5 && v[0] >= 0 && v[0] < 4) {
                x.cfg.slot[v[0]] = v[1]; x.cfg.team[v[0]] = v[2]; x.cfg.colour[v[0]] = v[3];
            }
        }
        std::fclose(f);
        x.on = true;
        return x;
    }();
    return r;
}

// ---- assets: Panel.piz TEAM0PANEL / TEAM1PANEL / STARTLIGHTS through librw's own DFF reader, so
// the frame hierarchy is RenderWare's (the score bar scales its own frame, FUN_0041b7a0) ----------
struct HudTexSrc { const Txd::Dictionary* d[2] = {}; };
HudTexSrc* g_hud_src = nullptr;          // valid only while LoadHud runs
std::vector<rw::Texture*> g_hud_tex;     // created on demand, by name

rw::Texture* HudFindTexture(const char* name) {
    for (rw::Texture* t : g_hud_tex)
        if (_stricmp(t->name, name) == 0) return t;
    if (!g_hud_src) return nullptr;
    for (const Txd::Dictionary* d : g_hud_src->d) {
        if (!d) continue;
        for (std::uint32_t i = 0; i < d->count(); ++i)
            if (_stricmp(d->texture(i).name, name) == 0) {
                rw::Texture* t = static_cast<rw::Texture*>(TextureFromTxdTexture(d->texture(i)));
                if (t) g_hud_tex.push_back(t);
                return t;
            }
    }
    return nullptr;
}

struct Panel {                       // the 0x16c-byte record at 0x0063cab8 + p*0x16c
    int         members[4] = {};     // +0x00
    int         count = 0;           // +0x0c
    int         vis[40] = {};        // +0x10
    rw::Atomic* atom[40] = {};       // +0xb0 (slot = geometry user data part code)
    float       pos[3] = {};         // +0x150
    rw::Clump*  clump = nullptr;     // +0x15c
    int         idx = 0;             // +0x164
    std::uint32_t flags = 0;         // +0x168
};
Panel        g_panel[2];
// [rfx2] the non-team panels: 4 records of 0x74 bytes at 0x0063c8d0 (FUN_0041b450 @ 0x0041b450)
struct NtPanel {
    rw::Atomic* atom[17] = {};       // +0x00..+0x40, slot = geometry part code (FUN_0041ad60)
    float       pos[3] = {};         // +0x50
    rw::Clump*  clump = nullptr;     // +0x5c (its frame = +0x60)
    int         idx = 0;             // +0x64
    std::uint32_t flags = 0;         // +0x68
    int         active = 0;          // +0x6c
    float       t70 = 0.f;           // +0x70
};
NtPanel      g_nt[4];
int          g_nt_cab0 = 0;          // DAT_0063cab0 (FUN_0041b540 increments, FUN_0041b510 zeroes)
int          g_nt_prev = -1;         // the HUD state of the previous frame
rw::Clump*   g_lights = nullptr;     // 0x0063d57c
rw::Atomic*  g_latom[12] = {};       // 0x0063d55c (FUN_0041d8b0 zeroes 0x30 bytes)
rw::Camera*  g_hud_cam = nullptr;    // DAT_006905b4
bool         g_hud_tried = false;
bool         g_hud_ok = false;

// FUN_004b5190 @ 0x004b5190 with (0, 0): the geometry's user-data array 0, element 0.
int PartCode(rw::Atomic* a) {
    rw::Geometry* g = a ? a->geometry : nullptr;
    if (!g || rw::UserDataArray::geometryGetCount(g) < 1) return -1;
    rw::UserDataArray* u = rw::UserDataArray::geometryGet(g, 0);
    return (u && u->numElements > 0) ? u->getInt(0) : -1;
}

rw::Clump* ReadClump(const Piz::Archive& a, const char* entry) {
    for (std::uint32_t i = 0; i < a.count(); ++i) {
        if (_stricmp(a.entry(i).name, entry) != 0) continue;
        std::uint32_t len = 0;
        const std::uint8_t* b = a.blob(i, &len);
        if (!b) return nullptr;
        rw::StreamMemory s;
        s.open(const_cast<rw::uint8*>(b), len);
        rw::Clump* c = nullptr;
        if (rw::findChunk(&s, rw::ID_CLUMP, nullptr, nullptr)) c = rw::Clump::streamRead(&s);
        s.close();
        return c;
    }
    return nullptr;
}

void LoadHud(int width, int height) {
    if (g_hud_tried) return;
    g_hud_tried = true;
    Piz::Archive panel, sfx;
    if (!panel.Load("original/TOASTART/Common/PANEL/Panel.piz")) return;
    sfx.Load("original/TOASTART/Common/SFX.piz");
    Txd::Dictionary dpanel, diface;
    for (std::uint32_t i = 0; i < panel.count(); ++i)
        if (_stricmp(panel.entry(i).name, "PANEL.TXD") == 0) {
            std::uint32_t l = 0; const std::uint8_t* b = panel.blob(i, &l);
            if (b) dpanel.Decode(b, l);
        }
    for (std::uint32_t i = 0; sfx.valid() && i < sfx.count(); ++i)
        if (_stricmp(sfx.entry(i).name, "INTERFACE.TXD") == 0) {
            std::uint32_t l = 0; const std::uint8_t* b = sfx.blob(i, &l);
            if (b) diface.Decode(b, l);
        }
    // The textures resolve by name across the loaded dictionaries: FUN_0041bec0 clears the current
    // dictionary first (FUN_004c5c80(0)), and OrangeDisplay / GreenStartLight live only in
    // PANEL.TXD, the NFL* team badges only in SFX.piz INTERFACE.TXD.
    HudTexSrc src;
    src.d[0] = &dpanel; src.d[1] = &diface;
    g_hud_src = &src;
    rw::Texture* (*save_find)(const char*) = rw::Texture::findCB;
    rw::Texture::findCB = HudFindTexture;
    // FUN_0041bec0 @ 0x0041bec0: "team0panel" / "team1panel" into the two records, idx = p
    // (0x0041befc); FUN_0041b690 @ 0x0041b690 files each atomic under its part code and paints the
    // score bar (slot 2, record +0xb8) (0x32,0x32,0x32,0xff) (FUN_004b5260 at 0x0041b70a).
    static const char* const kPanel[2] = {"TEAM0PANEL.DFF", "TEAM1PANEL.DFF"};
    for (int p = 0; p < 2; ++p) {
        Panel& P = g_panel[p];
        P.clump = ReadClump(panel, kPanel[p]);
        P.idx = p;
        if (!P.clump) continue;
        FORLIST(lnk, P.clump->atomics) {
            rw::Atomic* a = rw::Atomic::fromClump(lnk);
            const int part = PartCode(a);
            if (part >= 0 && part < 40) P.atom[part] = a;
        }
        if (rw::Atomic* bar = P.atom[2]) {
            rw::Geometry* g = bar->geometry;
            for (int m = 0; g && m < g->matList.numMaterials; ++m) {
                rw::Material* mt = g->matList.materials[m];
                mt->color.red = mt->color.green = mt->color.blue = 0x32; mt->color.alpha = 0xff;
            }
        }
        if (std::FILE* lf = std::fopen("log/rfx_hud_load.txt", "a")) {
            int n = 0;
            for (rw::Atomic* a : P.atom) if (a) ++n;
            std::fprintf(lf, "[rfx] HUD %s: %d atomics by part code\n", kPanel[p], n);
            for (int s = 0; s < 40; ++s) {
                rw::Atomic* a = P.atom[s];
                if (!a || !a->geometry) continue;
                rw::Geometry* g = a->geometry;
                rw::Material* mt = g->matList.numMaterials > 0 ? g->matList.materials[0] : nullptr;
                std::fprintf(lf, "  part %2d flags %#x mats %d tex %s raster %p col %d,%d,%d,%d\n", s,
                             (unsigned)g->flags, g->matList.numMaterials,
                             (mt && mt->texture) ? mt->texture->name : "(none)",
                             (mt && mt->texture) ? (void*)mt->texture->raster : nullptr,
                             mt ? mt->color.red : -1, mt ? mt->color.green : -1,
                             mt ? mt->color.blue : -1, mt ? mt->color.alpha : -1);
            }
            std::fclose(lf);
        }
    }
    // [rfx2] FUN_0041b450 @ 0x0041b450: "panel0".."panel3" (strings 0x005cd1a8..0x005cd190, ".dff"
    // appended by FUN_0042a5d0) into the records at 0x0063c8d0 (stride 0x74), idx = k (0x0041b4b6);
    // FUN_0041ad60 files each of the 17 atomics at rec + 4 * part code (FUN_004b5190(atomic, 0, 0)).
    static const char* const kNt[4] = {"PANEL0.DFF", "PANEL1.DFF", "PANEL2.DFF", "PANEL3.DFF"};
    for (int k = 0; RaceFx_HudNonTeamEnabled() && k < 4; ++k) {
        NtPanel& R = g_nt[k];
        R.clump = ReadClump(panel, kNt[k]);
        R.idx = k;
        if (!R.clump) continue;
        FORLIST(lnk, R.clump->atomics) {
            rw::Atomic* a = rw::Atomic::fromClump(lnk);
            const int part = PartCode(a);
            if (part >= 0 && part < 17) R.atom[part] = a;
        }
    }
    // FUN_0041d8b0 @ 0x0041d8b0: "startlights.dff", atomics by part code into 0x0063d55c, and each
    // of the six geometries' flag 0x40 (MODULATE) set (FUN_004b52f0 -> FUN_004b52c0, 0x0041d8fb).
    g_lights = ReadClump(panel, "STARTLIGHTS.DFF");
    if (g_lights) {
        FORLIST(lnk, g_lights->atomics) {
            rw::Atomic* a = rw::Atomic::fromClump(lnk);
            const int part = PartCode(a);
            if (part >= 0 && part < 12) g_latom[part] = a;
        }
        for (int k = 0; k < 6; ++k)
            if (g_latom[k] && g_latom[k]->geometry)
                g_latom[k]->geometry->flags |= rw::Geometry::MODULATE;
    }
    rw::Texture::findCB = save_find;
    g_hud_src = nullptr;
    // DAT_006905b4: RwCameraClone of the race camera after FUN_00467110 set its near plane 0.1
    // (0x3dcccccd) and far plane 180 (0x43340000).
    g_hud_cam = rw::Camera::create();
    if (g_hud_cam) {
        g_hud_cam->setFrame(rw::Frame::create());
        g_hud_cam->frameBuffer = rw::Raster::create(width, height, 0, rw::Raster::CAMERA);
        g_hud_cam->zBuffer = rw::Raster::create(width, height, 0, rw::Raster::ZBUFFER);
        g_hud_cam->setNearPlane(0.1f);
        g_hud_cam->setFarPlane(180.0f);
    }
    g_hud_ok = g_hud_cam && g_hud_cam->frameBuffer && (g_panel[0].clump || g_lights);
}

// FUN_0040b890 @ 0x0040b890 (score target) and FUN_0040b8e0 @ 0x0040b8e0 (leader threshold).
int ScoreTarget(const HudInputs& in) {
    int v = (in.n4d0 != 4) ? 8 : 12;
    if (in.team_play != 0) v = 8;
    if (in.mode_fd0 == 1) return 8;
    return (in.mode_fd0 == 2) ? 8 : v;
}
int LeaderThreshold(const HudInputs& in) {
    int v = (in.n4d0 != 4) ? 7 : 10;
    if (in.team_play != 0) v = 7;
    if (in.mode_fd0 == 1) return 7;
    return (in.mode_fd0 == 2) ? 7 : v;
}

void FrameReplace(rw::Frame* f, const float pos[3], float scale) {
    // RwMatrixTranslate(m, pos, rwCOMBINEREPLACE) into an identity matrix, RwFrameTransform(frame,
    // m, rwCOMBINEREPLACE) (FUN_004c51a0 / FUN_004c1480), then RwFrameScale(frame, s, PRECONCAT).
    rw::Matrix m;
    m.setIdentity();
    rw::V3d t = {pos[0], pos[1], pos[2]};
    m.translate(&t, rw::COMBINEREPLACE);
    f->transform(&m, rw::COMBINEREPLACE);
    rw::V3d s = {scale, scale, scale};
    f->scale(&s, rw::COMBINEPRECONCAT);
}

// FUN_0041bf20 @ 0x0041bf20 + FUN_0041b720 @ 0x0041b720 + FUN_0041c010 @ 0x0041c010 (round set-up)
// and FUN_0041c090 @ 0x0041c090 -> FUN_0041b7a0 @ 0x0041b7a0 (per frame), from the inputs. The
// round set-up is a pure function of the slot table, so it is re-derived every frame.
void UpdatePanels(const HudInputs& in) {
    for (Panel& P : g_panel) { P.count = 0; for (int& m : P.members) m = 0; }
    for (int p = 0; p < 4; ++p) {                                 // 0x0041bf3f..0x0041bfcc
        if (in.slot[p] == -1 || in.team[p] == -1) continue;
        if (in.team[p] < 0 || in.team[p] > 1) continue;
        Panel& P = g_panel[in.team[p]];
        if (P.count < 4) P.members[P.count] = p;
        ++P.count;
    }
    // FUN_0041c010: positions (0.45, 0.33, 1.0) / (-0.45, 0.33, 1.0) (0x3ee66666, 0x3ea8f5c3)
    static const float kPos[2][3] = {{0.45f, 0.33f, 1.0f}, {-0.45f, 0.33f, 1.0f}};
    // FUN_0040b930(&DAT_0063cd90): leader flag per player = (FUN_0040b8e0() <= score)
    int lead[4];
    const int thr = LeaderThreshold(in);
    for (int i = 0; i < 4; ++i) lead[i] = (thr <= in.score[i]) ? 1 : 0;
    const int target = ScoreTarget(in);
    for (Panel& P : g_panel) {
        std::memcpy(P.pos, kPos[P.idx], sizeof(P.pos));
        // FUN_0041b720: 0x800000 << ((count - 1) & 0x1f) (SHL masks the count; count 0 shifts
        // by 31 and leaves 0), then bit (colour + 6k) per member
        P.flags = 0x800000u << (static_cast<unsigned>(P.count - 1) & 0x1fu);
        for (int k = 0; k < P.count && k < 4; ++k) {
            const std::uint8_t bit = static_cast<std::uint8_t>(static_cast<std::uint8_t>(in.colour[P.members[k]]) + 6 * k);
            P.flags |= 1u << (bit & 0x1f);
        }
        if (!P.clump) continue;
        // FUN_0041b7a0: 0x40000 from the first member's leader flag (0x0041b7a5..0x0041b7da)
        if (lead[P.members[0] & 3]) P.flags |= 0x40000u; else P.flags &= ~0x40000u;
        const std::uint32_t f = P.flags;
        auto any3 = [&](std::uint32_t b) { return (f & 0x3000000u) != 0 && (f & b) != 0; };
        auto any2 = [&](std::uint32_t b) { return (f & 0x2000000u) != 0 && (f & b) != 0; };
        auto any1 = [&](std::uint32_t b) { return (f & 0x800000u) != 0 && (f & b) != 0; };
        for (int s = 0; s < 40; ++s) {
            if (!P.atom[s]) continue;                              // atom slot empty: flag untouched
            int v = 0;
            switch (s) {
            case 0: case 1: case 2: case 3: v = 1; break;
            case 4: v = (f & 0x40000u) ? 1 : 0; break;
            case 5: v = any3(0x1); break;   case 6: v = any3(0x2); break;
            case 7: v = any3(0x4); break;   case 8: v = any3(0x8); break;
            case 9: v = any3(0x10); break;  case 10: v = any3(0x20); break;
            case 11: v = any3(0x40); break; case 12: v = any3(0x80); break;
            case 13: v = any3(0x100); break; case 14: v = any3(0x200); break;
            case 15: v = any3(0x400); break; case 16: v = any3(0x800); break;
            case 22: v = any2(0x1000); break; case 23: v = any2(0x2000); break;
            case 24: v = any2(0x4000); break; case 25: v = any2(0x8000); break;
            case 26: v = any2(0x10000); break; case 27: v = any2(0x20000); break;
            case 28: v = any1(0x1); break;  case 29: v = any1(0x2); break;
            case 30: v = any1(0x4); break;  case 31: v = any1(0x8); break;
            case 32: v = any1(0x10); break; case 33: v = any1(0x20); break;
            default: v = 0; break;
            }
            P.vis[s] = v;
        }
        // clump frame: translate(P+0x150) REPLACE, scale (1,1,1) PRECONCAT (0x0041b9ac..0x0041ba56)
        FrameReplace(P.clump->getFrame(), P.pos, 1.0f);
        // the score bar: RwFrameScale(slot 2's frame, (score / target, 1, 1), rwCOMBINEREPLACE)
        // (0x0041ba5e..0x0041baa5; FUN_0040b6d0(first member) / FUN_0040b890())
        if (P.atom[2] && P.atom[2]->getFrame()) {
            rw::V3d s = {static_cast<float>(in.score[P.members[0] & 3]) / static_cast<float>(target), 1.f, 1.f};
            P.atom[2]->getFrame()->scale(&s, rw::COMBINEREPLACE);
        }
    }
}

// FUN_0041d930 @ 0x0041d930: the start lights' frame from the timer (before its increment).
void UpdateLights(const HudInputs& in) {
    if (!g_lights) return;
    float y = 0.35f;                                               // [0x5cd228]
    if (2.4f < in.timer) y = (in.timer - 2.4f) * 0.25f + 0.35f;    // [0x5cd22c], [0x5cc564]
    const float pos[3] = {0.f, y, 1.0f};                           // local_4c / 48 / 44
    FrameReplace(g_lights->getFrame(), pos, 1.125f);               // [0x5cd118]
}

// RenderWare's D3D9 driver binds each material's texture, NULL when it has none (RwD3D9SetTexture,
// FUN_004d6e70, as FUN_00535cb0 calls it with the material texture), so an untextured draw runs
// with texture alpha off: the original's two score-bar draws record ALPHABLENDENABLE 0 and
// ALPHATESTENABLE 0. librw's defaultRenderCB_Shader skips setTexture for an untextured material
// and keeps the previous raster's alpha flag, so the raster is unbound here first.
void RenderAtom(rw::Atomic* a) {
    if (!a) return;
    rw::Geometry* g = a->geometry;
    if (g && g->matList.numMaterials > 0 && !g->matList.materials[0]->texture)
        rw::SetRenderStatePtr(rw::TEXTURERASTER, nullptr);
    a->render();
}

// FUN_0041bc50 @ 0x0041bc50: the 29 (flag, atomic) pairs in this order.
void RenderPanel(const Panel& P) {
    static const int kOrder[29] = {3, 2, 0, 1, 4, 28, 29, 30, 31, 32, 33, 22, 23, 24, 25, 26, 27,
                                   11, 12, 13, 14, 15, 16, 5, 6, 7, 8, 9, 10};
    for (int s : kOrder)
        if (P.vis[s] && P.atom[s]) RenderAtom(P.atom[s]);
}

// FUN_0041db80 @ 0x0041db80 -> FUN_0041dab0 (thresholds 4.0 [0x5cc35c], 0.34 [0x5cd238],
// 0.82 [0x5cd234], 1.30 [0x5cd230], 1.86 [0x5ccdf4]).
void RenderLights(float t) {
    if (t < 4.0f) { RenderAtom(g_latom[0]); RenderAtom(g_latom[5]); }
    if (0.34f <= t) {
        if (t < 0.82f) RenderAtom(g_latom[1]);
        else if (t < 1.30f) { RenderAtom(g_latom[1]); RenderAtom(g_latom[2]); }
        else if (t < 1.86f) { RenderAtom(g_latom[1]); RenderAtom(g_latom[2]); RenderAtom(g_latom[3]); }
        if (1.86f <= t && t < 4.0f) RenderAtom(g_latom[4]);
    }
}

// ---- [rfx2] the non-team panels ----------------------------------------------------------------
// RwFrameScale(atomic frame, (s, s, 1), rwCOMBINEREPLACE) (FUN_004c13e0 with combine 0).
void ScaleAtomFrame(rw::Atomic* a, float sx, float sy, float sz) {
    if (!a || !a->getFrame()) return;
    rw::V3d s = {sx, sy, sz};
    a->getFrame()->scale(&s, rw::COMBINEREPLACE);
}
// FUN_0041ae60 @ 0x0041ae60: the six colour badges, slot 7 + b for flag bit 1 << b (0x0041ae60..0x0041aef0).
void NtScaleBadges(const NtPanel& R, float s0, float s1) {
    for (int b = 0; b < 6; ++b)
        if (R.flags & (1u << b)) ScaleAtomFrame(R.atom[7 + b], s0, s1, 1.0f);
}
// FUN_0041af00 @ 0x0041af00: slots 13 / 14 / 15 for flag bits 0x2000 / 0x4000 / 0x8000.
void NtScaleMinus(const NtPanel& R, float s0, float s1) {
    static const std::uint32_t kBit[3] = {0x2000u, 0x4000u, 0x8000u};
    for (int b = 0; b < 3; ++b)
        if (R.flags & kBit[b]) ScaleAtomFrame(R.atom[13 + b], s0, s1, 1.0f);
}
// The position table DAT_005f3304 (12 bytes per idx, read from the anchored binary).
const float kNtPos[4][3] = {{0.48f, 0.35f, 1.0f}, {-0.48f, 0.35f, 1.0f},     // 0x3ef5c28f, 0x3eb33333
                            {0.48f, -0.35f, 1.0f}, {-0.48f, -0.35f, 1.0f}};  // 0xbef5c28f, 0xbeb33333

// FUN_0041b4d0 @ 0x0041b4d0 (race state 1) -> FUN_0041adb0 @ 0x0041adb0 per record, and
// FUN_0041b510 @ 0x0041b510 (DAT_0063cab0 = 0) on the same path of FUN_004111c0.
void NtSetup(const HudInputs& in) {
    for (NtPanel& R : g_nt) {
        R.flags = 0; R.active = 0;                                         // 0x0041adb3 / 0x0041adba
        // 1 << (byte)DAT_007f1a1c[idx * 4] (the slot table's colour), shift masked to 5 bits
        R.flags = 1u << (static_cast<std::uint8_t>(in.colour[R.idx & 3]) & 0x1fu);
        // FUN_004b5260(slot 5 / slot 2, 0xff323232): the two score bars' material colour
        for (int s : {5, 2}) {
            rw::Atomic* a = R.atom[s];
            rw::Geometry* g = a ? a->geometry : nullptr;
            for (int m = 0; g && m < g->matList.numMaterials; ++m) {
                rw::Material* mt = g->matList.materials[m];
                mt->color.red = mt->color.green = mt->color.blue = 0x32; mt->color.alpha = 0xff;
            }
        }
        if (R.idx < in.n4d0) R.active = 1;                                 // DAT_0063c89c = FUN_0040e340()
    }
    g_nt_cab0 = 0;
}
// FUN_0040dbd0 (state 4) -> FUN_0041b520 @ 0x0041b520 -> FUN_0041ae20 @ 0x0041ae20 per record.
void NtState4() {
    for (NtPanel& R : g_nt) {
        std::memcpy(R.pos, kNtPos[R.idx & 3], sizeof(R.pos));
        R.t70 = 0.f;
        R.pos[0] = R.pos[0] + R.pos[0];
    }
}
// FUN_0040fc00 -> FUN_0041b540 @ 0x0041b540 (each race tick), FUN_0041af50 @ 0x0041af50 per active record.
void NtTick(const HudInputs& in) {
    int lead[4], top[4], low[4], zero[4], dead[4];
    const int thr = LeaderThreshold(in);
    for (int k = 0; k < 4; ++k) lead[k] = (thr <= in.score[k]) ? 1 : 0;     // FUN_0040b930
    int mx = 0;                                                            // FUN_0040b9a0(.., 1)
    for (int k = 0; k < 4; ++k) if (in.alive[k] != 0 && mx < in.score[k]) mx = in.score[k];
    for (int k = 0; k < 4; ++k) top[k] = (in.score[k] == mx) ? 1 : 0;
    int mn = 99;                                                           // FUN_0040ba00
    for (int k = 0; k < 4; ++k) if (in.score[k] < mn) mn = in.score[k];
    for (int k = 0; k < 4; ++k) low[k] = (in.score[k] == mn) ? 1 : 0;
    for (int k = 0; k < 4; ++k) zero[k] = (in.score[k] == 0) ? 1 : 0;      // FUN_0040b970
    for (int k = 0; k < 4; ++k) dead[k] = (in.alive[k] == 0) ? 1 : 0;      // FUN_0040ba60
    const int all_top = top[0] && top[1] && top[2] && top[3];              // DAT_0063c8c8
    const int all_low = low[0] && low[1] && low[2] && low[3];              // DAT_0063c8a0
    const int target = ScoreTarget(in);                                    // FUN_0040b890
    for (NtPanel& R : g_nt) {
        if (!R.active || !R.clump) continue;
        const int i = R.idx & 3;
        std::uint32_t f = R.flags;
        f = lead[i] ? (f | 0x40u) : (f & ~0x40u);
        f = (top[i] && !all_top) ? (f | 0x80u) : (f & ~0x80u);
        f = (low[i] && !all_low) ? (f | 0x100u) : (f & ~0x100u);
        f = zero[i] ? (f | 0x200u) : (f & ~0x200u);
        f = dead[i] ? (f | 0x400u) : (f & ~0x400u);
        if ((f & 0x40u) && (f & 0x100u)) f &= ~0x100u;
        if (f & 0x400u) f &= 0xfffffc3fu;
        const std::uint32_t was = f;
        f &= 0xffff1fffu;                                                  // 0x0041b056
        if (was & 0x400u) {
            const int v = in.minus[i];                                     // FUN_0040b420(idx)
            if (v == -2) f |= 0x8000u; else if (v == -1) f |= 0x4000u; else if (v == 0) f |= 0x2000u;
            R.t70 = R.t70 + 1.0f;                                          // [0x5cc320]
        }
        if (target == 8) f = (f & ~0x1000u) | 0x800u;
        else if (target == 12) f = (f & ~0x800u) | 0x1000u;
        R.flags = f;
        if (std::fabs(kNtPos[i][0]) < std::fabs(R.pos[0]))               // slide in by [0x5cd11c] 0.015
            R.pos[0] = (i & 1) == 0 ? R.pos[0] - 0.015f : R.pos[0] + 0.015f;
        FrameReplace(R.clump->getFrame(), R.pos, 1.125f);                  // 0x0041b0e9..0x0041b175, [0x5cd118]
        // the score bar: (fild score / fidiv target, 1, 1), combine 0, slot 5 (0x800) or slot 2 (0x1000)
        const float bar = static_cast<float>(static_cast<double>(in.score[i]) / static_cast<double>(target));
        if (f & 0x800u) ScaleAtomFrame(R.atom[5], bar, 1.f, 1.f);
        else if (f & 0x1000u) ScaleAtomFrame(R.atom[2], bar, 1.f, 1.f);
        // the badges: sin(DAT_0063cab0 * 0.2) * 0.2 + 1.0 ([0x5cc9c0], [0x5cc320]) when 0x80 && !0x40
        float s = 1.0f;
        if ((f & 0x80u) && !(f & 0x40u)) {
            double c = static_cast<double>(g_nt_cab0);
            if (g_nt_cab0 < 0) c += 4294967296.0;                          // [0x5cc94c]
            s = static_cast<float>(std::sin(c * static_cast<double>(0.2f)) * static_cast<double>(0.2f) + 1.0);
        }
        NtScaleBadges(R, s, s);
        if (f & 0x400u) {
            // x87: product and difference in extended, one rounding at the float store
            float a = static_cast<float>(1.0 - static_cast<double>(R.t70) * static_cast<double>(0.04f));  // [0x5cd18c]
            if (a <= 0.0f) a = 0.0f;
            NtScaleBadges(R, a, a);
            float b = static_cast<float>((static_cast<double>(R.t70) - 25.0) * static_cast<double>(0.04f));  // [0x5cc9e0]
            if (b > 1.0f) b = 1.0f; else if (b <= 0.0f) b = 0.0f;
            NtScaleMinus(R, b, b);
        }
    }
    ++g_nt_cab0;
}
// FUN_0041b630 @ 0x0041b630 -> FUN_0041b340 @ 0x0041b340 per record with +0x6c != 0, in this order.
void RenderNonTeam() {
    for (const NtPanel& R : g_nt) {
        if (!R.active || !R.clump) continue;
        const std::uint32_t f = R.flags;
        RenderAtom(R.atom[16]);
        RenderAtom(R.atom[0]);
        if (f & 0x800u) { RenderAtom(R.atom[5]); RenderAtom(R.atom[4]); if (f & 0x40u) RenderAtom(R.atom[6]); }
        else if (f & 0x1000u) { RenderAtom(R.atom[2]); RenderAtom(R.atom[1]); if (f & 0x40u) RenderAtom(R.atom[3]); }
        for (int b = 0; b < 6; ++b) if (f & (1u << b)) RenderAtom(R.atom[7 + b]);
        if (f & 0x2000u) RenderAtom(R.atom[13]);
        if (f & 0x4000u) RenderAtom(R.atom[14]);
        if (f & 0x8000u) RenderAtom(R.atom[15]);
    }
}

}  // namespace

bool RaceFx_HudEnabled() { static const bool on = !EnvOff("MASHED_HUD3D"); return on; }
bool RaceFx_HudNonTeamEnabled() {
    static const bool on = [] { const char* e = std::getenv("MASHED_HUD_NONTEAM"); return e && e[0] == '1'; }();
    return on;
}

void RaceFx_SetHud(const HudInputs& in) { g_hud = in; g_hud_set = true; }

void RaceFx_ReplayHudRow(int ba8c, float dtb) {
    HudReplay& r = Rep();
    if (!r.on) return;
    // FUN_0040dbd0 (state 4) -> FUN_0041d910 zeroes DAT_0063d588 before FUN_0040fc00's
    // FUN_0041d930 places the lights and adds DAT_007f100c (0x0041da76).
    if (ba8c == 4 && r.prev != 4) r.acc = 0.f;
    r.state = ba8c;
    r.t = r.acc;
    r.acc = r.acc + dtb;
    r.prev = ba8c;
}

void RaceFx_RenderHud(int width, int height) {
    if (!RaceFx_HudEnabled()) return;
    HudReplay& r = Rep();
    if (!g_hud_set && !r.on) return;
    HudInputs in = g_hud;
    if (r.on) {
        HudInputs c = r.cfg;
        c.state = r.state;
        c.timer = r.t;
        c.world_up = true;
        in = c;
    }
    // FUN_0040dfc0 @ 0x0040dfc0's branch (jump tables 0x0040e12c / 0x0040e140 / 0x0040e14c): the
    // team panels FUN_0041c0c0, the non-team panels FUN_0041b630, or a per-mode HUD.
    bool panels = false, nonteam = false;
    switch (in.round_type) {
    case 2: break;                                   // FUN_0041e850 (not ported)
    case 3: case 4: case 5:
        if (in.mode_fd0 == 4 || in.mode_fd0 == 7 || in.mode_fd0 == 9 || in.mode_fd0 == 5 ||
            in.mode_fd0 == 8 || in.mode_fd0 == 10)
            break;                                   // FUN_0041ded0 / FUN_0041c300 / FUN_0041a3e0
        panels = in.team_play != 0;                  // default -> caseD_7
        nonteam = in.team_play == 0;
        break;
    case 6: case 10:
        panels = in.team_play != 0 && in.mode_fd0 != 2;
        nonteam = !panels;
        break;
    case 0xb: break;                                 // FUN_00403160 (not ported)
    default:
        panels = in.team_play != 0;
        nonteam = in.team_play == 0;
        break;
    }
    // [UNCERTAIN U-9704] the per-mode HUDs above are not ported: nothing is drawn where the
    // original draws them.
    // [rfx2] the non-team records' logic, ahead of the draw as FUN_004111c0's tick precedes
    // FUN_00492e90: set-up at the round's first HUD frame (race state 1, FUN_0041b4d0 + FUN_0041b510),
    // positions at the state-4 tick (FUN_0041b520; the stand-in countdown has no state 4, so its
    // first state-5 frame after 3 stands for it), then FUN_0041b540 on every tick of states 3..11
    // (FUN_0040fc00's callers in FUN_004111c0).
    nonteam = nonteam && RaceFx_HudNonTeamEnabled();
    if (nonteam) {
        LoadHud(width, height);
        const int prev = g_nt_prev;
        if (prev < 0 || in.state < prev) NtSetup(in);
        if (in.state == 4 || (in.state == 5 && prev == 3)) NtState4();
        if (in.state >= 3 && in.state <= 11) NtTick(in);
    }
    g_nt_prev = in.state;
    // FUN_0040dfc0 @ 0x0040dfc0: FUN_00426ba0() == 0 or DAT_0063ba8c < 5 returns; 7 is the
    // standings branch (the standalone's 2D overlay); 5 / 6 below.
    if (!in.world_up || in.state < 5 || in.state > 6) return;
    LoadHud(width, height);
    if (!g_hud_ok) return;
    const bool lights = in.round_type != 0xb;
    UpdatePanels(in);
    UpdateLights(in);
    // FUN_0040de30: identity frame, view window (0.6, 0.45) (0x3f19999a, 0x3ee66666), rwPARALLEL
    rw::Frame* f = g_hud_cam->getFrame();
    rw::Matrix id;
    id.setIdentity();
    f->transform(&id, rw::COMBINEREPLACE);
    rw::V2d vw = {0.6f, 0.45f};
    g_hud_cam->setViewWindow(&vw);
    g_hud_cam->setProjection(rw::Camera::PARALLEL);
    // librw's state is a write-back cache that persists into the next frame's world pass: save
    // what this pass changes and put it back after (the device itself is restored by
    // RaceSubmit_Render's state block).
    const rw::int32 sv_zt = rw::GetRenderState(rw::ZTESTENABLE);
    const rw::int32 sv_zw = rw::GetRenderState(rw::ZWRITEENABLE);
    const rw::int32 sv_fog = rw::GetRenderState(rw::FOGENABLE);
    const rw::int32 sv_cull = rw::GetRenderState(rw::CULLMODE);
    const rw::int32 sv_sb = rw::GetRenderState(rw::SRCBLEND);
    const rw::int32 sv_db = rw::GetRenderState(rw::DESTBLEND);
    const rw::int32 sv_af = rw::GetRenderState(rw::ALPHATESTFUNC);
    const rw::int32 sv_ar = rw::GetRenderState(rw::ALPHATESTREF);
    g_hud_cam->beginUpdate();
    // FUN_00492e90: RS 6 (ZTEST) = 0, RS 8 (ZWRITE) = 0 around FUN_0040dfc0; the state FUN_00410b30
    // leaves (cull 1, SRCBLEND 5, DESTBLEND 6; 0x00410ca6..0x00410cc4) and the device alpha test
    // GREATER / 0 (FUN_004d5bc0); no fog on the HUD camera. ZWRITE goes off first: librw keeps
    // ZENABLE on (with ZFUNC ALWAYS) when depth test is turned off while depth write is still on
    // (d3ddevice.cpp setDepthTest), where RenderWare's driver leaves the device at ZENABLE 0.
    rw::SetRenderState(rw::ZWRITEENABLE, 0);
    rw::SetRenderState(rw::ZTESTENABLE, 0);
    rw::SetRenderState(rw::FOGENABLE, 0);
    rw::SetRenderState(rw::CULLMODE, rw::CULLNONE);
    rw::SetRenderState(rw::SRCBLEND, rw::BLENDSRCALPHA);
    rw::SetRenderState(rw::DESTBLEND, rw::BLENDINVSRCALPHA);
    rw::SetRenderState(rw::ALPHATESTFUNC, rw::ALPHAGREATER);
    rw::SetRenderState(rw::ALPHATESTREF, 0);
    if (panels)
        for (const Panel& P : g_panel)                // FUN_0041c0c0: 0x0063cab8, then +0x16c
            if (P.clump) RenderPanel(P);
    if (nonteam) RenderNonTeam();                     // FUN_0041b630, before the lights (0x0040e0ee)
    if (lights) RenderLights(in.timer);
    rw::SetRenderState(rw::ZTESTENABLE, sv_zt);
    rw::SetRenderState(rw::ZWRITEENABLE, sv_zw);
    rw::SetRenderState(rw::FOGENABLE, sv_fog);
    rw::SetRenderState(rw::CULLMODE, sv_cull);
    rw::SetRenderState(rw::SRCBLEND, sv_sb);
    rw::SetRenderState(rw::DESTBLEND, sv_db);
    rw::SetRenderState(rw::ALPHATESTFUNC, sv_af);
    rw::SetRenderState(rw::ALPHATESTREF, sv_ar);
    g_hud_cam->endUpdate();
}

}  // namespace LibRw
}  // namespace mashed_re
