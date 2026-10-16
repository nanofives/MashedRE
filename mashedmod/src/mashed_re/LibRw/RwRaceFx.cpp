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

#include "../D3d9Render/DrawRec.h"
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
// FUN_0041eaa0 @ 0x0041eaa0 binds "beam" (string 0x005cd268) and "car_shadow" (0x0041eb08) from the
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

    // FUN_00410b30 @ 0x00410b30 state at FUN_0041ebb0's call: RW cull mode 2 (0x00410b39, RS
    // 0x14), fog off (0x00410b9x, RS 0xe = 0); the device's alpha test GREATER / 0 from
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

void RaceFx_ReplayHudRow(int /*ba8c*/, float /*dtb*/) {}

}  // namespace LibRw
}  // namespace mashed_re
