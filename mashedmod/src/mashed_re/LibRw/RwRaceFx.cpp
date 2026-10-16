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

// [rfx2] this frame's car inputs, kept for the Headlight pass (RaceFx_DrawWorldFx), which runs
// after DrawCarFx has consumed the queue
static CarFxInput g_hl_cars[4];
static bool       g_hl_have[4] = {};

void RaceFx_DrawCarFx(void* rw_camera) {
    for (int i = 0; i < 4; ++i) { g_hl_cars[i] = g_cars[i]; g_hl_have[i] = g_have[i]; }
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

// ---- SFX.piz textures by name (the FX passes' "RWObjShad" etc.) --------------------------------
// [txdscope U-9724] FUN_0040bb30 @ 0x0040bb30 is FUN_004c5c00(DAT_0063b8f8, name): the FX names are looked up in
// the `fx.txd` dictionary only (loaded by FUN_0040bbb0, which ends with FUN_004c5c80(0)); badges.txd,
// TrackImages.txd and Interface.txd are separate dictionaries that this lookup never walks (RWObjShad
// 0x0040bcbd, Headlight 0x00459297, lensflare-fs8 0x00402908 / 0x00454f22 / 0x00458484). All five names asked
// for here exist only in FX.TXD, so the scope does not change a result. MASHED_NO_TXDSCOPE=1 keeps every TXD.
std::vector<rw::Texture*> g_sfx_tex;
bool                      g_sfx_tried = false;
rw::Texture* SfxTexture(const char* name) {
    if (!g_sfx_tried) {
        g_sfx_tried = true;
        static const bool s_all_txd = std::getenv("MASHED_NO_TXDSCOPE") != nullptr;
        Piz::Archive a;
        if (a.Load("original/TOASTART/Common/SFX.piz")) {
            for (std::uint32_t i = 0; i < a.count(); ++i) {
                const char* n = a.entry(i).name;
                const std::size_t ln = std::strlen(n);
                if (ln < 4 || _stricmp(n + ln - 4, ".TXD") != 0) continue;
                if (!s_all_txd && _stricmp(n, "FX.TXD") != 0) continue;
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
// [UNCERTAIN U-9710] FUN_004c4eb0 is not transcribed: cofactor inverse here (equal in exact arithmetic).
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
    // [UNCERTAIN U-9710] RW's BSP walk stops at the FIRST intersection its sector order finds
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
    // shadow). [UNCERTAIN U-9710] the 0xffNN3cff / 0xffNNffff codes read the runtime table
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

// ---- [rfx2] the Headlight sprites (C09) -------------------------------------------------------
float g_lp[8][3] = {};
bool  g_lp_have[8] = {};

// RwV3dLength (FUN_004c3ac0 @ 0x004c3ac0): (z z + y y) + x x through the RW sqrt table.
float LutLength(const float* v) {
    float s = v[2] * v[2] + v[1] * v[1];
    s = s + v[0] * v[0];
    return Math::LutSqrt(s);
}

// One record of the 0x2c-byte list at 0x0089bea0 (FUN_00412cf0 @ 0x00412cf0).
struct HlRec { float pos[3]; float size; std::uint8_t rgba[4]; };

// FUN_00412cf0: pos = (part - cam) * fade + part, size = |part - cam| * [0x5cc9a4] 0.025 + [0x5cc9a0]
// 0.05, alpha = ftol(v * v * [0x5cd04c] 255 * scale) with v = min(clamp(at . camera at), 0)
// (0x00412d74..0x00412d86), colour rgb from the caller's 4-byte array.
void HlRecord(std::vector<HlRec>& out, const float* ppos, const float* pat, const float* cam_pos,
              const float* cam_at, float scale, float fade, const std::uint8_t* rgb) {
    if (out.size() >= 0x7f) return;
    const float d[3] = {ppos[0] - cam_pos[0], ppos[1] - cam_pos[1], ppos[2] - cam_pos[2]};
    const float dist = LutLength(d);
    float v = DotClamp(pat, cam_at);
    if (!(v <= 0.0f)) v = 0.0f;
    HlRec r;
    r.pos[0] = d[0] * fade + ppos[0];
    r.pos[1] = d[1] * fade + ppos[1];
    r.pos[2] = d[2] * fade + ppos[2];
    r.size = dist * 0.025f + 0.05f;
    r.rgba[0] = rgb[0]; r.rgba[1] = rgb[1]; r.rgba[2] = rgb[2];
    float a = v * v;
    a = a * 255.0f;
    a = a * scale;
    r.rgba[3] = FtolByte(a);
    out.push_back(r);
}

void DrawHeadlights(rw::Camera* cam) {
    if (!RaceFx_HeadlightEnabled() || !cam || !cam->getFrame() || !cam->frameBuffer) return;
    // the camera frame's modelling matrix (FUN_00467210(0): frame + 0x10); the race camera is a
    // root frame, so this is its LTM too (witness: modelling == LTM)
    const rw::Matrix& cm = cam->getFrame()->matrix;
    const float cam_pos[3] = {cm.pos.x, cm.pos.y, cm.pos.z};
    const float cam_at[3] = {cm.at.x, cm.at.y, cm.at.z};
    static const std::uint8_t kWhite[3] = {0xff, 0xff, 0xff};    // local_78 (0x00412f62..)
    static const std::uint8_t kRed[3] = {0xff, 0x00, 0x00};      // local_74 / local_30
    std::vector<HlRec> recs;
    // ---- FUN_00412f30 @ 0x00412f30, per car slot 0..DAT_008a94d0-1 (FUN_0040e340: 4 here) ----
    for (int i = 0; i < 4; ++i) {
        if (!g_hl_have[i]) continue;
        const CarFxInput& c = g_hl_cars[i];
        const M body = FromD3D(c.body);
        // fade DAT_005f2a4c from the vehicle record position (FUN_0046d4a0 -> +0x30) to the camera:
        // |d| * [0x5cc32c] 0.5 - [0x5cc320] 1.0, then -0.05 above [0x5cd054] -0.05, -1.0 below -1
        // [UNCERTAIN U-9711] the vehicle record's +0x30 position is stood in by the car clump root
        // (they differ by the render offsets; the fade is -0.05 whenever the car is 2+ units away).
        const float dc[3] = {c.root[12] - cam_pos[0], c.root[13] - cam_pos[1], c.root[14] - cam_pos[2]};
        float fade = LutLength(dc) * 0.5f - 1.0f;
        if (fade <= -0.05f) { if (fade < -1.0f) fade = -1.0f; } else fade = -0.05f;
        // flags rec+0x294 (DAT_0063dc74 + i*0x2ac): 0x10 / 0x20 / 0x400 are never set by the
        // standalone (U-9706), so FUN_0041f090 / FUN_0041f0d0 read 0
        auto part = [&](int k, bool neg, float scale, const std::uint8_t* rgb) {
            if (!g_lp_have[k]) return;                                 // FUN_0041f1c0 == 0
            const float* o = g_lp[k];
            float p[3], a[3];
            for (int q = 0; q < 3; ++q) {
                p[q] = body.p[q] + o[0] * body.r[q] + o[1] * body.u[q] + o[2] * body.a[q];
                a[q] = neg ? body.a[q] * -1.0f : body.a[q];           // [0x5cc33c] -1.0
            }
            HlRecord(recs, p, a, cam_pos, cam_at, scale, fade, rgb);
        };
        // the brake lights 0x39 / 0x3a when the controller's brake byte is set (0x00413035..)
        if (c.brake) { part(6, true, 1.0f, kRed); part(7, true, 1.0f, kRed); }
        // DAT_0063bc44[i] (rec+0x294 & 0x40): headlights 0x19, 0x1a, 0x1c, 0x1b, then the tail
        // lights 0x37 / 0x38 at 0x3e800000 = 0.25
        if (c.lights) {
            for (int k = 0; k < 4; ++k) part(k, false, 1.0f, kWhite);
            part(4, true, 0.25f, kRed); part(5, true, 0.25f, kRed);
        }
    }
    if (recs.empty()) return;
    // ---- FUN_00413a00 @ 0x00413a00 -> FUN_004128f0 @ 0x004128f0 (param 0x3fcccccd = 1.6) per record ----
    // camera space = the record through the camera's viewMatrix (camera + 0x20): RW's view matrix
    // is the inverse LTM with x' = -0.5 * recipViewWindow.x * x + 0.5 * z (y likewise) plus the view
    // offset, z' = z. [UNCERTAIN U-9711] that relation is MEASURED (camera_probe --fx-out, City: the
    // formula equals the original's viewMatrix to 5.7e-6), RW's camera sync is not transcribed.
    float Linv[16];
    {
        float L[16];
        const rw::Matrix* ltm = cam->getFrame()->getLTM();
        L[0] = ltm->right.x; L[1] = ltm->right.y; L[2] = ltm->right.z; L[3] = 0.f;
        L[4] = ltm->up.x;    L[5] = ltm->up.y;    L[6] = ltm->up.z;    L[7] = 0.f;
        L[8] = ltm->at.x;    L[9] = ltm->at.y;    L[10] = ltm->at.z;   L[11] = 0.f;
        L[12] = ltm->pos.x;  L[13] = ltm->pos.y;  L[14] = ltm->pos.z;  L[15] = 1.f;
        InvertAffine(L, Linv);
    }
    const float rvx = 1.0f / cam->viewWindow.x, rvy = 1.0f / cam->viewWindow.y;
    float V[16];
    for (int r = 0; r < 4; ++r) {
        const float x = Linv[r * 4 + 0], y = Linv[r * 4 + 1], z = Linv[r * 4 + 2];
        V[r * 4 + 0] = -0.5f * rvx * x + 0.5f * z;
        V[r * 4 + 1] = -0.5f * rvy * y + 0.5f * z;
        V[r * 4 + 2] = z;
        V[r * 4 + 3] = 0.f;
    }
    const float nearp = cam->nearPlane, farp = cam->farPlane;      // camera +0x80 / +0x84
    const float zn = 0.0f, zf = 1.0f;                               // RwGlobals +0x18 / +0x1c (witness)
    const float w = static_cast<float>(cam->frameBuffer->width);    // raster +0xc / +0x10
    const float h = static_cast<float>(cam->frameBuffer->height);
    std::vector<rw::RWDEVICE::Im2DVertex> vs;
    std::vector<rw::uint16> idx;
    for (const HlRec& r : recs) {
        float c3[3];
        for (int k = 0; k < 3; ++k) c3[k] = r.pos[0] * V[0 + k] + r.pos[1] * V[4 + k] + r.pos[2] * V[8 + k] + V[12 + k];
        const float z = c3[2];
        if (!(nearp < z && z < farp)) continue;
        const float rz = 1.0f / z;                                  // [0x5cc320] / z
        const float sx = rz * w * c3[0];
        const float sy = rz * h * c3[1];
        const float ip = 1.0f / 1.6f;
        const float hw = (w / h) * w * ip * 0.8f * r.size * rz;      // [0x5cc9bc] 0.8
        const float hh = ip * h * 0.8f * r.size * rz;
        if (!(-hw < sx && sx < hw + w && -hh < sy && sy < hh + h)) continue;
        float x0 = sx - hw, y0 = sy - hh, x1 = hw + sx, y1 = hh + sy;
        // the clip to the raster with the uv pulled in: the record's uv +0x14..+0x20 are (1, 0, 1, 0)
        // (FUN_00412cf0 0x00412dc7..0x00412ddb), so u0 = +0x18, u1 = +0x14, v0 = +0x20, v1 = +0x1c,
        // and each clipped edge moves its coordinate by the clipped fraction of the full size
        float u0 = 0.f, u1 = 1.f, v0 = 0.f, v1 = 1.f;
        if (y0 < 0.0f) { v0 = 1.f - (1.f - 0.f) * (((hh + hh) - -y0) / (hh + hh)); y0 = 0.f; }
        if (h < y1) { v1 = 1.f - (1.f - 0.f) * ((y1 - h) / (hh + hh)); y1 = h; }
        if (w < x1) { u1 = 1.f - (1.f - 0.f) * ((x1 - w) / (hw + hw)); x1 = w; }
        if (x0 < 0.0f) { u0 = 1.f - (1.f - 0.f) * (((hw + hw) - -x0) / (hw + hw)); x0 = 0.f; }
        const float sz = (z - nearp) * (farp / z) * ((zf - zn) / (farp - nearp)) + zn;
        const rw::uint16 b = static_cast<rw::uint16>(vs.size());
        const float X[4] = {x0, x1, x0, x1}, Y[4] = {y0, y0, y1, y1};
        const float U[4] = {u0, u1, u0, u1}, Vv[4] = {v0, v0, v1, v1};
        for (int q = 0; q < 4; ++q) {
            rw::RWDEVICE::Im2DVertex o;
            o.setScreenX(X[q]); o.setScreenY(Y[q]); o.setScreenZ(sz);
            o.setRecipCameraZ(rz);
            o.setColor(r.rgba[0], r.rgba[1], r.rgba[2], r.rgba[3]);
            o.setU(U[q], rz); o.setV(Vv[q], rz);
            vs.push_back(o);
        }
        const rw::uint16 t[6] = {b, static_cast<rw::uint16>(b + 1), static_cast<rw::uint16>(b + 2),
                                 static_cast<rw::uint16>(b + 2), static_cast<rw::uint16>(b + 1), static_cast<rw::uint16>(b + 3)};
        for (int k = 0; k < 6; ++k) idx.push_back(t[k]);
    }
    if (vs.empty()) return;
    rw::Texture* tex = SfxTexture("Headlight");
    // ---- FUN_00413a40 @ 0x00413a40: raster DAT_0063bc54, filter 2, vertex alpha on, cull NONE, fog
    // off, Z test on, ZWRITE off, blend 5 / 2; then ZWRITE on, cull and fog back, blend 5 / 6 ----
    rw::int32 sv_cull = rw::GetRenderState(rw::CULLMODE);
    rw::int32 sv_fog = rw::GetRenderState(rw::FOGENABLE);
    rw::SetRenderStatePtr(rw::TEXTURERASTER, tex ? tex->raster : nullptr);
    rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::LINEAR);
    rw::SetRenderState(rw::VERTEXALPHA, 1);
    rw::SetRenderState(rw::CULLMODE, rw::CULLNONE);
    rw::SetRenderState(rw::FOGENABLE, 0);
    rw::SetRenderState(rw::ZTESTENABLE, 1);
    rw::SetRenderState(rw::ZWRITEENABLE, 0);
    rw::SetRenderState(rw::SRCBLEND, rw::BLENDSRCALPHA);
    rw::SetRenderState(rw::DESTBLEND, rw::BLENDONE);
    static const float kIdent[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    if (rw_drawrec_pre) {
        DrawRec::SetNextGeometry(vs.data(), sizeof(vs[0]), idx.data(), static_cast<unsigned>(idx.size()), true);
        rw_drawrec_pre(tex ? tex->name : "Headlight", kIdent);
    }
    rw::im2d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, vs.data(), static_cast<rw::int32>(vs.size()),
                                     idx.data(), static_cast<rw::int32>(idx.size()));
    if (rw_drawrec_post) rw_drawrec_post();
    rw::SetRenderState(rw::ZWRITEENABLE, 1);
    rw::SetRenderState(rw::CULLMODE, sv_cull);
    rw::SetRenderState(rw::FOGENABLE, sv_fog);
    rw::SetRenderState(rw::SRCBLEND, rw::BLENDSRCALPHA);
    rw::SetRenderState(rw::DESTBLEND, rw::BLENDINVSRCALPHA);
}

// ---- [rfx2] lensflare-fs8 ------------------------------------------------------------------
// One camera-facing RpPTank particle (flags 0x10000087: position, 2D size, colour, 4-float uv).
struct Bill { float p[3]; float sx, sy; std::uint8_t rgba[4]; float uv[4]; };
struct LensEntry { int type; float pos[3]; float k; };
std::vector<LensEntry> g_lens;
int                    g_lens_bob = -1;
bool                   g_lens_req = false;
std::vector<Bill>      g_lens_small, g_lens_big;   // DAT_007030a8 / DAT_007030ac contents

// FUN_00487e70 @ 0x00487e70's per-type element count (+0x10) and tables: sizes +0x20, colours +0x28,
// uv indices +0x24 (0x007030b8 / 0x007030dc: BSS, read 0 on the original, witness). Values read from
// the anchored binary: sizes 0x00614368 (2, 0.25, 0.4, 0.7, 0.95) and 0x0061442c (2, 0.25, 0.24, 0,
// 0.9); colours (RwRGBA dwords) 0x00614388, 0x006143a8, 0x0061440c, 0x0061444c.
struct LensType { int n; const float* sz; const std::uint32_t* col; };
const float kLensSzA[5] = {2.0f, 0.25f, 0.4f, 0.7f, 0.95f};
const float kLensSzB[5] = {2.0f, 0.25f, 0.24f, 0.0f, 0.9f};
const std::uint32_t kLensCol2[1] = {0x301080ffu};
const std::uint32_t kLensCol3[2] = {0x80ff8080u, 0xffffc8c8u};
const std::uint32_t kLensCol7[3] = {0x804040ffu, 0x802020ffu, 0xc0ffffffu};
const std::uint32_t kLensColC[2] = {0x401080ffu, 0xff1080ffu};
bool LensTypeOf(int type, LensType* t) {
    switch (type) {
    case 2:   *t = {1, kLensSzA, kLensCol2}; return true;     // 0x00487efd
    case 3:   *t = {2, kLensSzA, kLensCol3}; return true;     // 0x00487f23
    case 7:   *t = {3, kLensSzB, kLensCol7}; return true;     // 0x00487fd9 -> 0x00488031
    case 0xc: *t = {2, kLensSzB, kLensColC}; return true;     // 0x00488004
    default:  return false;                                    // types not created on these tracks
    }
}
// The uv table entry 0 at 0x00614260: (0, 0, 0.5, 1).
const float kLensUv[4] = {0.0f, 0.0f, 0.5f, 1.0f};

// FUN_00488e70 / FUN_004883b0 from this frame's camera frame modelling matrix (param_1).
void LensBuild(const rw::Matrix& cm) {
    g_lens_small.clear();
    g_lens_big.clear();
    // base = at * [0x5cc31c] 3.0 + pos (local_18..local_10 / local_24..local_1c)
    const float base[3] = {cm.at.x * 3.0f + cm.pos.x, cm.at.y * 3.0f + cm.pos.y, cm.at.z * 3.0f + cm.pos.z};
    for (std::size_t li = 0; li < g_lens.size(); ++li) {
        const LensEntry& e = g_lens[li];
        LensType t;
        if (!LensTypeOf(e.type, &t)) continue;
        // local_4c: 1.0 for a light without a frame (+0x2c == 0); FUN_00488320 @ 0x00488320 sets it
        // to 1.0 again for type 0xc (0x00488331) and leaves types 2 / 3 / 7 alone; then * k, >= 0
        float l = 1.0f * e.k;
        if (l < 0.0f) l = 0.0f;
        // ---- the small batch: every element at |d| * d/|d| + base (RwV3dNormalize, LUT) ----
        {
            const float d0[3] = {e.pos[0] - base[0], e.pos[1] - base[1], e.pos[2] - base[2]};
            float dn[3];
            const float len = Math::LutNormalize(dn, d0);
            for (int i = 0; i < t.n; ++i) {
                Bill b;
                b.p[0] = len * dn[0] + base[0];
                b.p[1] = len * dn[1] + base[1];
                b.p[2] = len * dn[2] + base[2];
                if (i == 4) {
                    b.sx = (l * 10.0f + 2.0f) * t.sz[4] * l;           // [0x5cc55c] / [0x5cc574]
                    b.sy = l * 0.5f * t.sz[4] * l;
                } else {
                    const float f = l + 1.0f;                          // [0x5cc320]
                    b.sx = f * t.sz[i] * l;
                    b.sy = f * t.sz[i] * l;
                }
                std::memcpy(b.rgba, &t.col[i], 4);                     // RwRGBA bytes r, g, b, a
                std::memcpy(b.uv, kLensUv, sizeof(b.uv));
                g_lens_small.push_back(b);
            }
        }
        // ---- the big batch: the light mirrored in y (local_8 = -y), 16 particles stepping down by
        // j * l * [0x5cc32c] 0.5, size (l + 0.5) * sz[0] * l, colour[0] with alpha (16 - j) * 8 ----
        {
            const float pm[3] = {e.pos[0], -e.pos[1], e.pos[2]};
            const float d0[3] = {pm[0] - base[0], pm[1] - base[1], pm[2] - base[2]};
            float dn[3];
            const float len = Math::LutNormalize(dn, d0);
            const float s = (l + 0.5f) * t.sz[0] * l;
            std::uint8_t c[4];
            std::memcpy(c, &t.col[0], 4);
            for (int j = 0; j < 16; ++j) {
                Bill b;
                b.p[0] = len * dn[0] + base[0];
                b.p[1] = static_cast<float>((static_cast<double>(len) * dn[1] + base[1]) -
                                            static_cast<double>(j) * l * 0.5);
                b.p[2] = len * dn[2] + base[2];
                b.sx = s; b.sy = s;
                b.rgba[0] = c[0]; b.rgba[1] = c[1]; b.rgba[2] = c[2];
                b.rgba[3] = static_cast<std::uint8_t>((16 - j) * 8);
                std::memcpy(b.uv, kLensUv, sizeof(b.uv));
                g_lens_big.push_back(b);
            }
        }
    }
}

// The RpPTank render of a camera-facing batch: FUN_00537fb0 @ 0x00537fb0 takes the current camera's
// frame LTM, right' = -right, up' = up; FUN_00536e90 @ 0x00536e90 builds v0 = p - s.x/2 r' - s.y/2 u',
// v1 = p + s.x/2 r' - s.y/2 u', v2 = p + s.x/2 r' + s.y/2 u', v3 = p - s.x/2 r' + s.y/2 u' (0.5 =
// [0x5cc32c]); FUN_00537d70 @ 0x00537d70 the uv (u0,v1)(u1,v1)(u1,v0)(u0,v0); the colour on all four
// (FUN_00537b20); indices (0,1,2)(0,2,3) (FUN_00536170); blend +0xa4 / +0xa8 = 5 / 2 (FUN_004881d0 /
// FUN_004880a0), vertex alpha +0xac = 1.
void DrawBills(const std::vector<Bill>& bs, rw::Camera* cam, rw::Texture* tex) {
    if (bs.empty() || !tex || !cam || !cam->getFrame()) return;
    const rw::Matrix* L = cam->getFrame()->getLTM();
    const float R[3] = {-L->right.x, -L->right.y, -L->right.z};
    const float U[3] = {L->up.x, L->up.y, L->up.z};
    std::vector<rw::RWDEVICE::Im3DVertex> v(bs.size() * 4);
    std::vector<rw::uint16> idx;
    idx.reserve(bs.size() * 6);
    // librw's im3d buffer holds up to NUMVERTICES; draw in chunks of 1024 particles (4096 vertices)
    const std::size_t kChunk = 1024;
    for (std::size_t i = 0; i < bs.size(); ++i) {
        const Bill& b = bs[i];
        const float hx = b.sx * 0.5f, hy = b.sy * 0.5f;
        const float X[3] = {hx * R[0], hx * R[1], hx * R[2]};
        const float Y[3] = {hy * U[0], hy * U[1], hy * U[2]};
        const float P[4][3] = {
            {(-X[0] - Y[0]) + b.p[0], (-X[1] - Y[1]) + b.p[1], (-X[2] - Y[2]) + b.p[2]},
            {(X[0] - Y[0]) + b.p[0], (X[1] - Y[1]) + b.p[1], (X[2] - Y[2]) + b.p[2]},
            {Y[0] + X[0] + b.p[0], Y[1] + X[1] + b.p[1], Y[2] + X[2] + b.p[2]},
            {(Y[0] - X[0]) + b.p[0], (Y[1] - X[1]) + b.p[1], (Y[2] - X[2]) + b.p[2]}};
        const float UVs[4][2] = {{b.uv[0], b.uv[3]}, {b.uv[2], b.uv[3]}, {b.uv[2], b.uv[1]}, {b.uv[0], b.uv[1]}};
        for (int q = 0; q < 4; ++q) {
            rw::RWDEVICE::Im3DVertex& o = v[i * 4 + q];
            o.setX(P[q][0]); o.setY(P[q][1]); o.setZ(P[q][2]);
            o.setNormalX(0.f); o.setNormalY(0.f); o.setNormalZ(0.f);
            o.setColor(b.rgba[0], b.rgba[1], b.rgba[2], b.rgba[3]);
            o.setU(UVs[q][0]); o.setV(UVs[q][1]);
        }
    }
    rw::SetRenderStatePtr(rw::TEXTURERASTER, tex->raster);
    rw::SetRenderState(rw::TEXTUREFILTER, rw::Texture::LINEAR);   // texture + 0x50 = 2 (0x004880fe)
    rw::SetRenderState(rw::TEXTUREADDRESSU, tex->getAddressU());
    rw::SetRenderState(rw::TEXTUREADDRESSV, tex->getAddressV());
    rw::SetRenderState(rw::VERTEXALPHA, 1);
    rw::SetRenderState(rw::SRCBLEND, rw::BLENDSRCALPHA);
    rw::SetRenderState(rw::DESTBLEND, rw::BLENDONE);
    static const float kIdent[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    // one draw for the whole batch (the original's single RpPTank render); librw's im3d buffer is
    // sized for it when the batch fits (the big batch on City is 1104 particles = 4416 vertices)
    (void)kChunk;
    for (std::size_t i = 0; i < bs.size(); ++i) {
        const rw::uint16 s = static_cast<rw::uint16>(i * 4);
        const rw::uint16 tri[6] = {s, static_cast<rw::uint16>(s + 1), static_cast<rw::uint16>(s + 2),
                                   s, static_cast<rw::uint16>(s + 2), static_cast<rw::uint16>(s + 3)};
        for (int k = 0; k < 6; ++k) idx.push_back(tri[k]);
    }
    rw::im3d::Transform(v.data(), static_cast<rw::int32>(v.size()), nullptr, rw::im3d::VERTEXUV);
    if (rw_drawrec_pre) {
        DrawRec::SetNextGeometry(v.data(), sizeof(v[0]), idx.data(), static_cast<unsigned>(idx.size()));
        rw_drawrec_pre(tex->name, kIdent);
    }
    rw::im3d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, idx.data(), static_cast<rw::int32>(idx.size()));
    if (rw_drawrec_post) rw_drawrec_post();
    rw::im3d::End();
}

// The small batch, FUN_004891f0 @ 0x004891f0 (FUN_00410b30, after FUN_0040bde0): ZWRITE off, Z test
// on around the PTank render; cull NONE and fog off as recorded on the original at that point.
void DrawLensSmall(rw::Camera* cam) {
    if (!RaceFx_LensEnabled() || g_lens_small.empty()) return;
    rw::int32 sv_cull = rw::GetRenderState(rw::CULLMODE);
    rw::int32 sv_fog = rw::GetRenderState(rw::FOGENABLE);
    rw::int32 sv_sb = rw::GetRenderState(rw::SRCBLEND), sv_db = rw::GetRenderState(rw::DESTBLEND);
    rw::int32 sv_va = rw::GetRenderState(rw::VERTEXALPHA);
    rw::SetRenderState(rw::ZWRITEENABLE, 0);
    rw::SetRenderState(rw::ZTESTENABLE, 1);
    rw::SetRenderState(rw::CULLMODE, rw::CULLNONE);
    rw::SetRenderState(rw::FOGENABLE, 0);
    DrawBills(g_lens_small, cam, SfxTexture("lensflare-fs8"));
    rw::SetRenderState(rw::ZWRITEENABLE, 1);
    rw::SetRenderState(rw::ZTESTENABLE, 1);
    rw::SetRenderState(rw::CULLMODE, sv_cull);
    rw::SetRenderState(rw::FOGENABLE, sv_fog);
    rw::SetRenderState(rw::SRCBLEND, sv_sb);
    rw::SetRenderState(rw::DESTBLEND, sv_db);
    rw::SetRenderState(rw::VERTEXALPHA, sv_va);
}

}  // namespace

bool RaceFx_LensEnabled() { static const bool on = !EnvOff("MASHED_LENSFLARE"); return on; }
static bool g_lens_after_sky = false;
bool RaceFx_LensAfterSky() { return g_lens_after_sky; }
void RaceFx_SetLensLights(const LensLight* l, int n, int bob, bool after_sky) {
    g_lens_after_sky = after_sky;
    g_lens.clear();
    g_lens_small.clear();
    g_lens_big.clear();
    g_lens_req = false;
    for (int i = 0; l && i < n; ++i) g_lens.push_back({l[i].type, {l[i].pos[0], l[i].pos[1], l[i].pos[2]}, l[i].k});
    g_lens_bob = bob;
    if (std::FILE* f = std::fopen("log/librw_race.txt", "a")) {
        int ne = 0;
        for (const LensEntry& e : g_lens) { LensType t; if (LensTypeOf(e.type, &t)) ne += t.n; }
        std::fprintf(f, "[rfx2] lensflare lights: %d, elements %d (bob %d)\n", n, ne, bob);
        if (std::getenv("MASHED_DBG_LENS"))
            for (int i = 0; i < n; ++i)
                std::fprintf(f, "  lens %d type %d pos (%.4f,%.4f,%.4f) k %.4f\n", i, l[i].type, l[i].pos[0],
                             l[i].pos[1], l[i].pos[2], l[i].k);
        std::fclose(f);
    }
}
void RaceFx_SetLensLightK(int index, float k) {
    if (index >= 0 && static_cast<std::size_t>(index) < g_lens.size()) g_lens[static_cast<std::size_t>(index)].k = k;
}
void RaceFx_LensRequestBuild() { g_lens_req = true; }
void RaceFx_BeginFrame(void* rw_camera) {
    rw::Camera* cam = static_cast<rw::Camera*>(rw_camera);
    if (g_lens_req && cam && cam->getFrame()) {
        LensBuild(cam->getFrame()->matrix);
        g_lens_req = false;
    }
}
void RaceFx_DrawLensBig(void* rw_camera, int mode) {
    if (!RaceFx_LensEnabled() || g_lens_big.empty()) return;
    rw::Camera* cam = static_cast<rw::Camera*>(rw_camera);
    rw::int32 sv_zt = rw::GetRenderState(rw::ZTESTENABLE), sv_zw = rw::GetRenderState(rw::ZWRITEENABLE);
    rw::int32 sv_cull = rw::GetRenderState(rw::CULLMODE), sv_fog = rw::GetRenderState(rw::FOGENABLE);
    rw::int32 sv_sb = rw::GetRenderState(rw::SRCBLEND), sv_db = rw::GetRenderState(rw::DESTBLEND);
    rw::int32 sv_va = rw::GetRenderState(rw::VERTEXALPHA);
    if (mode == 0) {
        // FUN_004492b0 @ 0x004492b0: RS 0xc 0, blend 5 / 6, RS 6 (Z test) 0, RS 8 (ZWRITE) 0, then
        // the sky clump +0x100f0 and SkySecondaryDispatch; cull NONE and fog off as recorded
        // librw keeps ZENABLE on (ZFUNC ALWAYS) when the Z test goes off while ZWRITE is on, which
        // is the state the sky copies leave; turning the test off with ZWRITE already off gives
        // the device ZENABLE 0, what the original records here (d3ddevice.cpp setDepthTest)
        rw::SetRenderState(rw::ZTESTENABLE, 1);
        rw::SetRenderState(rw::ZWRITEENABLE, 0);
        rw::SetRenderState(rw::ZTESTENABLE, 0);
        rw::SetRenderState(rw::CULLMODE, rw::CULLNONE);
        rw::SetRenderState(rw::FOGENABLE, 0);
    } else {
        // FUN_0044b8f0 @ 0x0044b8f0: RS 8 (ZWRITE) 0, RS 6 (Z test) 1, SkySecondaryDispatch, RS 8 1;
        // cull BACK and fog on from the world pass, as recorded
        rw::SetRenderState(rw::ZWRITEENABLE, 0);
        rw::SetRenderState(rw::ZTESTENABLE, 1);
        rw::SetRenderState(rw::CULLMODE, rw::CULLBACK);
    }
    DrawBills(g_lens_big, cam, SfxTexture("lensflare-fs8"));
    if (mode == 0) {
        // back to the sky pass's device state (ZENABLE on, ZFUNC ALWAYS, ZWRITE off): ZWRITE on with
        // the test off re-enables ZENABLE / ALWAYS, ZWRITE off then leaves it
        rw::SetRenderState(rw::ZWRITEENABLE, 1);
        rw::SetRenderState(rw::ZWRITEENABLE, 0);
    }
    rw::SetRenderState(rw::ZTESTENABLE, sv_zt);
    rw::SetRenderState(rw::ZWRITEENABLE, mode == 0 ? sv_zw : 1);
    rw::SetRenderState(rw::CULLMODE, sv_cull);
    rw::SetRenderState(rw::FOGENABLE, sv_fog);
    rw::SetRenderState(rw::SRCBLEND, sv_sb);
    rw::SetRenderState(rw::DESTBLEND, sv_db);
    rw::SetRenderState(rw::VERTEXALPHA, sv_va);
}

bool RaceFx_HeadlightEnabled() { static const bool on = !EnvOff("MASHED_HEADLIGHT"); return on; }
void RaceFx_SetCarLightParts(const float pos[8][3], const bool have[8]) {
    std::memcpy(g_lp, pos, sizeof(g_lp));
    for (int k = 0; k < 8; ++k) g_lp_have[k] = have[k];
}

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
        o.flag = objs[i].casts && ObjShadowMatrix(objs[i], light, Linv, soup, &o.m);
        nflag += o.flag ? 1 : 0;
    }
    if (std::FILE* f = std::fopen("log/librw_race.txt", "a")) {
        std::fprintf(f, "[rfx2] object shadows: %d objects, %d with a shadow (DAT_006cad50)\n", n, nflag);
        // MASHED_DBG_OBJSHAD=1 (DIAGNOSTIC, default unset): one line per object
        if (std::getenv("MASHED_DBG_OBJSHAD"))
            for (int i = 0; i < n; ++i) {
                const ObjShadow& o = g_objshad[static_cast<std::size_t>(i)];
                std::fprintf(f, "  obj %d pos (%.4f,%.4f,%.4f) flag %d m.p (%.5f,%.5f,%.5f) r (%.5f,%.5f,%.5f)\n", i,
                             objs[i].ltm[12], objs[i].ltm[13], objs[i].ltm[14], o.flag ? 1 : 0,
                             o.m.p[0], o.m.p[1], o.m.p[2], o.m.r[0], o.m.r[1], o.m.r[2]);
            }
        std::fclose(f);
    }
}

void RaceFx_SetObjShadowFrame(std::uint32_t f101c) { g_objshad_frame = f101c; }

static void DrawObjShadows(rw::Camera* cam) {
    if (g_objshad.empty() || !RaceFx_ObjShadowEnabled()) return;
    if (!cam || !cam->getFrame()) return;
    // ---- FUN_00481a30 @ 0x00481a30, per object i ---------------------------------------------
    // [UNCERTAIN U-9710] the original runs this in the tick, against the camera frame's modelling
    // matrix (FUN_00467210(0)) and the frustum planes of the last RwCameraBeginUpdate; here both are
    // this frame's render camera. DAT_007f0fd0 == 5's branch (0x00481ab3) is not taken in the modes
    // the standalone runs; FUN_00484cf0 / FUN_004818d0 (physics registration, over-speed break) and
    // the per-frame recompute of a moving object are not modelled (the objects are static).
    const rw::Matrix& cm = cam->getFrame()->matrix;
    // The tick's frustum is the last render's: FUN_0040fc00 @ 0x0040fc00 moves the camera frame
    // (FUN_0040d470) before FUN_0040ce00 -> FUN_00481a30, but the camera's world planes
    // (camera + 0x94, FUN_004c1b40) are only re-derived when the frame hierarchy syncs at the next
    // RwCameraBeginUpdate. MASHED_OBJSHAD_PLANES=cur tests against this frame's planes instead.
    static const bool s_cur_planes = [] {
        const char* e = std::getenv("MASHED_OBJSHAD_PLANES");
        return e && std::strcmp(e, "cur") == 0;
    }();
    static rw::FrustumPlane s_prev[6];
    static bool s_have_prev = false;
    const rw::FrustumPlane* planes = (s_have_prev && !s_cur_planes) ? s_prev : cam->frustumPlanes;
    auto test = [&](const float* c, float r) -> int {    // FUN_004c1b40 @ 0x004c1b40
        int res = 2;
        for (int k = 0; k < 6; ++k) {
            const rw::V3d& nn = planes[k].plane.normal;
            const float dist = (nn.z * c[2] + nn.x * c[0] + nn.y * c[1]) - planes[k].plane.distance;
            if (r < dist) return 0;
            if (-r < dist) res = 1;
        }
        return res;
    };
    std::vector<Particle> ps;
    for (std::size_t i = 0; i < g_objshad.size(); ++i) {
        ObjShadow& o = g_objshad[i];
        const bool par = (static_cast<std::uint32_t>(i) & 1u) == (g_objshad_frame & 1u);   // 0x00481a80..
        const float dx = o.sphere[0] - cm.pos.x, dy = o.sphere[1] - cm.pos.y, dz = o.sphere[2] - cm.pos.z;
        float d2 = dz * dz + dx * dx;
        d2 = d2 + dy * dy;
        if (!(d2 < 2500.0f)) continue;                                   // [0x5cd098]
        o.prev = o.state;                                                // 0x00481ba8
        if (par) o.state = test(o.sphere, o.sphere[3] * 10.0f);         // [0x5cc55c] 10.0
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
    std::memcpy(s_prev, cam->frustumPlanes, sizeof(s_prev));
    s_have_prev = true;
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

void RaceFx_DrawWorldFx(void* rw_camera) {
    rw::Camera* cam = static_cast<rw::Camera*>(rw_camera);
    DrawObjShadows(cam);     // FUN_0040bde0 -> FUN_00487de0 (first)
    DrawLensSmall(cam);      // FUN_004891f0 (FUN_00410b30, after FUN_0040bde0)
    DrawHeadlights(cam);     // FUN_00413a00 / FUN_00413a40 (FUN_00410b30, after FUN_0040bde0)
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
bool RaceFx_HudNonTeamEnabled() { static const bool on = !EnvOff("MASHED_HUD_NONTEAM"); return on; }

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

// =============================================================================================
// [rres 2026-10-16] The car shadow render-to-texture (C10, U-9691; verify/rres_20261016/PREREG_rres.md
// amendment 1). Every constant is cited to its address in the original.
// =============================================================================================
namespace {

// FUN_0041eaa0 @ 0x0041eaa0: FUN_004b48e0(0x100, 0x100, &DAT_0063e490, &DAT_0063d854) makes the [0] camera
// and texture (A8R8G8B8 camera raster 0x505, z raster, RwTextureCreate with filter byte 2); the camera is
// parallel (FUN_004c1c10(cam, 2)) with view window 0.75 x 0.75 (FUN_004c1c80, local_8 = local_4 = 0x3f400000).
// FUN_0041eaa0 makes both: [0] 256x256 (DAT_0063e490 / DAT_0063d854) and [1] 64x64 (DAT_0063e494 / DAT_0063d858,
// FUN_004b48e0(0x40, 0x40, ...), same parallel projection and view window); FUN_0041f8f0 picks [rec+0x298 == 2], the far
// level of detail (the car record's FUN_00420870 state), so a far car draws its shadow from the 64x64 texture.
constexpr int   kShadowRtSize[2] = {256, 64};
constexpr float kShadowWindow  = 0.75f;
// FUN_0041f8f0: the camera sits at lightAt * _DAT_005cca00 + carpos (0x0041f981..0x0041f9ab), _DAT_005cca00 = -4.0.
constexpr float kShadowLift    = -4.0f;
// FUN_00422570: sphere radius local_107c = 0x3fc00000; the query keeps at most 0x40 (FUN_004b4ad0 arg 4).
constexpr float kShadowRadius  = 1.5f;
constexpr int   kShadowMaxTris = 0x40;
constexpr float kShadowMinNormalY = 0.75f;   // _DAT_005cc950: `0.75 <= record.normal.y`
constexpr float kShadowFrontMax   = 1.0f;    // _DAT_005cc320, plane class 0
constexpr float kShadowBehindMax  = 0.1f;    // _DAT_005cc56c, plane class 2
constexpr float kShadowBias       = 0.01f;   // DAT_00641324 = 0x3c23d70a, added to each vertex y
// The three collision material colours FUN_00422570 skips as RwRGBA dwords (r | g << 8 | b << 16 | a << 24):
// -0xfefeff = 0xff010101, -0xffff01 = 0xff0000ff, -0xff80 = 0xffff0080.
constexpr std::uint32_t kShadowSkipRgba[3] = {0xff010101u, 0xff0000ffu, 0xffff0080u};
// Vertex colour alpha byte (DAT_00641313): FUN_00422570 takes a 4th stack argument, the float product
// DAT_00644368 * rec+0x288 * rec+0x284 that FUN_0041f8f0 stores for it (0x0041fa51..0x0041fa63), and does
// `fld [esp+0x10a0]; fmul [0x005cd320]` (0x0042257a / 0x00422581, 0x005cd320 = 153.0f) then FUN_004a2c48
// (__ftol): alpha = trunc(product * 153), low byte. DAT_00644368 = 1.0 on every track measured (3 of 3).
constexpr double kShadowAlphaK = 153.0;
// The re-query threshold: `fcom qword ptr [0x005cd318]` (0x00422624), the DOUBLE 0.125 (the float read of
// the same address is 0.0, which is what the decompilation prints): the triangle set is kept until the car
// has moved more than sqrt(0.125) = 0.354 from the position of the last query.
constexpr double kShadowMoveD2 = 0.125;

struct ShadowRt {
    rw::Camera*  cam = nullptr;
    rw::Raster*  ras = nullptr;
    rw::Raster*  zb  = nullptr;
    rw::Texture* tex = nullptr;
    bool tried = false, ok = false;
} g_srt[2];

bool          g_shadow_gate = false;       // DAT_00657490
ObjShadowSoup g_ssoup;
float         g_slight[16] = {};
bool          g_slight_ok = false;

// The 0xf40-byte per-player record at 0x006403e8 + slot * 0xf40: the previous query position (0x006403b8),
// the triangle count (0x00641318), vertices (entry dwords 0..8, y already biased), texture coordinates
// (dwords 9..14) and the colour alpha byte (0x00641313).
struct ShadowSlot {
    float last[3] = {0.f, 0.f, 0.f};
    int   n = 0;
    int   rt = 0;            // the render texture this car's pass drew into this frame (0 = 256, 1 = 64)
    bool  ran = false;       // the RT pass ran for this car this frame (FUN_00420050 got past its sphere test)
    float v[kShadowMaxTris][9] = {};
    float uv[kShadowMaxTris][6] = {};
    std::uint8_t alpha = 0;
};
ShadowSlot g_sslot[4];
float      g_ssun[4] = {1.f, 1.f, 1.f, 1.f};
int        g_dbg_row = -1;

// The recorder's name for the unnamed camera texture: "#WxH:format" (the original's recorder falls back to the
// D3D descriptor, camera_probe.py texDesc, 21 = D3DFMT_A8R8G8B8); the newer captures name the two textures
// "@shadowrt0" / "@shadowrt1" (MASHED_DBG_SHADOWRT_NAMED=1 selects those).
const char* ShadowRtName(int rt) {
    static const bool named = std::getenv("MASHED_DBG_SHADOWRT_NAMED") != nullptr;
    if (named) return rt ? "@shadowrt1" : "@shadowrt0";
    return rt ? "#64x64:21" : "#256x256:21";
}

bool ShadowRtBuild(int rt) {
    ShadowRt& R = g_srt[rt];
    if (R.tried) return R.ok;
    R.tried = true;
    const int sz = kShadowRtSize[rt];
    R.ras = rw::Raster::create(sz, sz, 32, rw::Raster::CAMERATEXTURE | rw::Raster::C8888);
    R.zb  = rw::Raster::create(sz, sz, 0, rw::Raster::ZBUFFER);
    R.cam = rw::Camera::create();
    if (R.ras && R.zb && R.cam) {
        R.cam->setFrame(rw::Frame::create());
        R.cam->frameBuffer = R.ras;
        R.cam->zBuffer = R.zb;
        R.cam->setProjection(rw::Camera::PARALLEL);
        rw::V2d vw = {kShadowWindow, kShadowWindow};
        R.cam->setViewWindow(&vw);
        R.tex = rw::Texture::create(R.ras);
        if (R.tex) {
            R.tex->setFilter(rw::Texture::LINEAR);
            R.tex->setAddressU(rw::Texture::CLAMP);
            R.tex->setAddressV(rw::Texture::CLAMP);
        }
    }
    R.ok = R.tex != nullptr;
    if (std::FILE* f = std::fopen("log/librw_race.txt", "a")) {
        std::fprintf(f, "[rres] shadow RT %dx%d camera texture: %s\n", sz, sz, R.ok ? "ok" : "FAILED");
        std::fclose(f);
    }
    return R.ok;
}

// Squared distance from p to triangle abc (closest point, Ericson, Real-Time Collision Detection 5.1.5).
double TriDist2(const double p[3], const double a[3], const double b[3], const double c[3]) {
    auto sub = [](const double* x, const double* y, double* o) { for (int k = 0; k < 3; ++k) o[k] = x[k] - y[k]; };
    auto dot = [](const double* x, const double* y) { return x[0] * y[0] + x[1] * y[1] + x[2] * y[2]; };
    double ab[3], ac[3], ap[3];
    sub(b, a, ab); sub(c, a, ac); sub(p, a, ap);
    const double d1 = dot(ab, ap), d2 = dot(ac, ap);
    double q[3];
    auto done = [&](const double* r) { double d[3]; sub(p, r, d); return dot(d, d); };
    if (d1 <= 0.0 && d2 <= 0.0) return done(a);
    double bp[3]; sub(p, b, bp);
    const double d3 = dot(ab, bp), d4 = dot(ac, bp);
    if (d3 >= 0.0 && d4 <= d3) return done(b);
    const double vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0) {
        const double v = d1 / (d1 - d3);
        for (int k = 0; k < 3; ++k) q[k] = a[k] + v * ab[k];
        return done(q);
    }
    double cp[3]; sub(p, c, cp);
    const double d5 = dot(ab, cp), d6 = dot(ac, cp);
    if (d6 >= 0.0 && d5 <= d6) return done(c);
    const double vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0) {
        const double w = d2 / (d2 - d6);
        for (int k = 0; k < 3; ++k) q[k] = a[k] + w * ac[k];
        return done(q);
    }
    const double va = d3 * d6 - d5 * d4;
    if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0) {
        const double w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        for (int k = 0; k < 3; ++k) q[k] = b[k] + w * (c[k] - b[k]);
        return done(q);
    }
    const double denom = 1.0 / (va + vb + vc);
    const double v = vb * denom, w = vc * denom;
    for (int k = 0; k < 3; ++k) q[k] = a[k] + ab[k] * v + ac[k] * w;
    return done(q);
}

// FUN_00422570 @ 0x00422570 for car `slot`, with the RT camera's frame already posed (its viewMatrix is the
// texture-coordinate map). `pos` = the clump frame's modelling position (param_2 + 0x30..0x38).
// [UNCERTAIN U-9734] the switch-table colour exclusion (FUN_00472500) of the original's collect loop is not modelled.
void ShadowCollect(int slot, const float pos[3], const double cp[3], const double r[3], const double u[3],
                   std::uint8_t alpha) {
    ShadowSlot& S = g_sslot[slot];
    S.alpha = alpha;                                                        // 0x0042258d
    const double dx = static_cast<double>(S.last[0]) - pos[0], dy = static_cast<double>(S.last[1]) - pos[1],
                 dz = static_cast<double>(S.last[2]) - pos[2];
    if (kShadowMoveD2 < dz * dz + dx * dx + dy * dy) {                      // the double at 0x005cd318
        S.last[0] = pos[0]; S.last[1] = pos[1]; S.last[2] = pos[2];
        S.n = 0;
        if (!g_ssoup.verts || !g_ssoup.tris || g_ssoup.ntri == 0) return;
        const double P[3] = {pos[0], pos[1], pos[2]};
        const double r2 = static_cast<double>(kShadowRadius) * kShadowRadius;
        std::vector<std::size_t> hits;
        for (std::size_t t = 0; t < g_ssoup.ntri && hits.size() < static_cast<std::size_t>(kShadowMaxTris); ++t) {
            const float* a = &g_ssoup.verts[g_ssoup.tris[t * 3 + 0] * 3];
            const float* b = &g_ssoup.verts[g_ssoup.tris[t * 3 + 1] * 3];
            const float* c = &g_ssoup.verts[g_ssoup.tris[t * 3 + 2] * 3];
            bool out = false;                                               // cheap reject on the triangle's box
            for (int k = 0; k < 3 && !out; ++k) {
                const double lo = std::fmin(a[k], std::fmin(b[k], c[k])), hi = std::fmax(a[k], std::fmax(b[k], c[k]));
                out = (P[k] + kShadowRadius < lo) || (P[k] - kShadowRadius > hi);
            }
            if (out) continue;
            const double A[3] = {a[0], a[1], a[2]}, B[3] = {b[0], b[1], b[2]}, C[3] = {c[0], c[1], c[2]};
            if (TriDist2(P, A, B, C) <= r2) hits.push_back(t);
        }
        // MASHED_DBG_SHADOWSEL=<csv>, DIAGNOSTIC ONLY (default unset): every sphere hit with its verdict
        static std::FILE* s_sel = [] {
            const char* e = std::getenv("MASHED_DBG_SHADOWSEL");
            std::FILE* f = (e && e[0]) ? std::fopen(e, "w") : nullptr;
            if (f) std::fprintf(f, "slot,px,py,pz,tri,rgba,ny,sd,verdict,v0x,v0y,v0z,v1x,v1y,v1z,v2x,v2y,v2z,row\n");
            return f; }();
        if (hits.empty()) {                                                 // the query returned 0: n stays 0
            if (s_sel)
                std::fprintf(s_sel, "%d,%.5f,%.5f,%.5f,-1,0,0,0,nohit,0,0,0,0,0,0,0,0,0,%d\n", slot, pos[0], pos[1], pos[2], g_dbg_row);
            return;
        }
        for (std::size_t t : hits) {
            const std::uint32_t rgba = g_ssoup.tri_rgba ? g_ssoup.tri_rgba[t] : 0u;
            const float* v0 = &g_ssoup.verts[g_ssoup.tris[t * 3 + 0] * 3];
            const float* v1 = &g_ssoup.verts[g_ssoup.tris[t * 3 + 1] * 3];
            const float* v2 = &g_ssoup.verts[g_ssoup.tris[t * 3 + 2] * 3];
            auto sel = [&](double ny, double sdv, const char* verdict) {
                if (s_sel)
                    std::fprintf(s_sel, "%d,%.5f,%.5f,%.5f,%zu,%08x,%.4f,%.4f,%s,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%d\n",
                                 slot, pos[0], pos[1], pos[2], t, rgba, ny, sdv, verdict, v0[0], v0[1], v0[2],
                                 v1[0], v1[1], v1[2], v2[0], v2[1], v2[2], g_dbg_row);
            };
            if (rgba == kShadowSkipRgba[0] || rgba == kShadowSkipRgba[1] || rgba == kShadowSkipRgba[2]) { sel(0, 0, "color"); continue; }
            // FUN_00477e60's plane: n = (v2 - v1) x (v0 - v1), normalised; d = -(n . v0)
            const double e1[3] = {static_cast<double>(v2[0]) - v1[0], static_cast<double>(v2[1]) - v1[1],
                                  static_cast<double>(v2[2]) - v1[2]};
            const double e0[3] = {static_cast<double>(v0[0]) - v1[0], static_cast<double>(v0[1]) - v1[1],
                                  static_cast<double>(v0[2]) - v1[2]};
            double n[3] = {e1[1] * e0[2] - e1[2] * e0[1], e1[2] * e0[0] - e1[0] * e0[2], e1[0] * e0[1] - e1[1] * e0[0]};
            const double nl = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
            if (nl <= 0.0) continue;
            for (double& x : n) x /= nl;
            if (!(static_cast<double>(kShadowMinNormalY) <= n[1])) { sel(n[1], 0, "normal"); continue; }
            const double d = -(n[2] * v0[2] + n[0] * v0[0] + n[1] * v0[1]);
            const double sd = n[2] * pos[2] + n[1] * pos[1] + n[0] * pos[0] + d;              // FUN_00477f50
            const double dist = std::fabs(sd);
            if (sd > 0.0) { if (!(dist <= kShadowFrontMax)) { sel(n[1], sd, "front"); continue; } }   // class 0
            else if (sd < 0.0) { if (!(dist <= kShadowBehindMax)) { sel(n[1], sd, "behind"); continue; } }   // class 2
            if (S.n >= kShadowMaxTris) break;
            sel(n[1], sd, "keep");
            float* o = S.v[S.n++];
            for (int k = 0; k < 3; ++k) { o[k] = v0[k]; o[3 + k] = v1[k]; o[6 + k] = v2[k]; }
            o[1] += kShadowBias; o[4] += kShadowBias; o[7] += kShadowBias;
        }
    }
    // FUN_004c3d90 over every kept triangle with the RT camera's viewMatrix (camera + 0x20), x and y kept as the
    // texture coordinates (entry dwords 9..14). RenderWare's camera sync builds that matrix as the inverse LTM with the
    // x row negated times (right = 1/(2 vw.x), up = -1/(2 vw.y), pos = 0.5, 0.5) (librw camera.cpp cameraSync), so for
    // a parallel camera u = 0.5 - (d . right) / (2 vw), v = 0.5 - (d . up) / (2 vw) with d = p - camera position
    // (verified on 291 recorded vertices of the original, error <= 2e-4). Closed form here: the camera's own viewMatrix
    // is only re-synced at its BeginUpdate, which runs after this.
    const double k2 = 1.0 / (2.0 * static_cast<double>(kShadowWindow));
    for (int i = 0; i < S.n; ++i)
        for (int k = 0; k < 3; ++k) {
            const float* p = &S.v[i][3 * k];
            const double d0 = p[0] - cp[0], d1 = p[1] - cp[1], d2 = p[2] - cp[2];
            S.uv[i][2 * k]     = static_cast<float>(0.5 - (d0 * r[0] + d1 * r[1] + d2 * r[2]) * k2);
            S.uv[i][2 * k + 1] = static_cast<float>(0.5 - (d0 * u[0] + d1 * u[1] + d2 * u[2]) * k2);
        }
}

}  // namespace

bool RaceFx_ShadowRtEnabled() { static const bool on = !EnvOff("MASHED_CAR_SHADOW_RT"); return on; }

void RaceFx_SetShadowCourse(int course_id) {
    g_shadow_gate = !(course_id == 0 || course_id == 0x1a || course_id == 0x24);   // end of 0x00479330
}

void RaceFx_SetShadowWorld(const ObjShadowSoup& soup, const float light_ltm[16]) {
    g_ssoup = soup;
    g_slight_ok = light_ltm != nullptr;
    if (light_ltm) std::memcpy(g_slight, light_ltm, sizeof(g_slight));
}

void RaceFx_SetDbgRow(int row) { g_dbg_row = row; }

// [UNCERTAIN U-9735] the original's initial per-car last-query position is not read; the chain restarts with the trace.
void RaceFx_ShadowResetCache() {
    for (ShadowSlot& s : g_sslot) s = ShadowSlot();
}

void RaceFx_SetCarSun(int slot, float sun) {
    if (slot >= 0 && slot < 4) g_ssun[slot] = sun;
}

bool RaceFx_ShadowActive(int slot) {
    return slot >= 0 && slot < 4 && g_have[slot] && RaceFx_ShadowRtEnabled() && g_shadow_gate && g_slight_ok &&
           g_cars[slot].hull_v && g_cars[slot].hull_t && g_cars[slot].hull_nt > 0;
}

void RaceFx_ShadowPass(int slot, void* main_cam, void (*rebegin)(), int lod_state) {
    if (!RaceFx_ShadowActive(slot) || !main_cam) return;
    const CarFxInput& c = g_cars[slot];
    rw::Camera* mc = static_cast<rw::Camera*>(main_cam);
    // FUN_00420050 0x00420081: the car's sphere against the race camera; outside returns before any draw
    {
        rw::Sphere sp;
        sp.center.x = c.body[12]; sp.center.y = c.body[13]; sp.center.z = c.body[14];
        const std::uint32_t rb = 0x3f088c2du;
        std::memcpy(&sp.radius, &rb, sizeof(sp.radius));
        if (mc->frustumTestSphere(&sp) == rw::Camera::SPHEREOUTSIDE) {
            static std::FILE* s_out = [] {
                const char* e = std::getenv("MASHED_DBG_SHADOWSEL");
                return (e && e[0]) ? std::fopen((std::string(e) + ".outside").c_str(), "w") : nullptr; }();
            if (s_out) std::fprintf(s_out, "%d,%d,%.4f,%.4f,%.4f\n", g_dbg_row, slot, sp.center.x, sp.center.y, sp.center.z);
            g_sslot[slot].ran = false;      // the batch (count, vertices) stays, as in the record at 0x006403e8
            return;
        }
    }
    const int rt = (lod_state == 2) ? 1 : 0;                      // FUN_0041f8f0: (rec+0x298 == 2) indexes the pair
    if (!ShadowRtBuild(rt)) return;
    rw::Camera* rc = g_srt[rt].cam;
    g_sslot[slot].rt = rt;
    g_sslot[slot].ran = true;

    // ---- FUN_0041f8f0: the RT camera frame --------------------------------------------------------------
    // position = lightAt * -4 + rec+0x258; FUN_004b42c0: at = normalise(target - position), right =
    // normalise(up x at) with up = DAT_006146fc = (0,1,0), up' = at x right (verified against the recorded
    // camera LTM of the original, verify/rres_20261016).
    const float* L = g_slight;                                    // D3D rows: right, up, at, pos
    const double lat[3] = {L[8], L[9], L[10]};
    const double tgt[3] = {c.body[12], c.body[13], c.body[14]};
    const double cp[3] = {tgt[0] + lat[0] * kShadowLift, tgt[1] + lat[1] * kShadowLift, tgt[2] + lat[2] * kShadowLift};
    double a[3] = {tgt[0] - cp[0], tgt[1] - cp[1], tgt[2] - cp[2]};
    const double al = std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    if (al < 1e-9) return;
    for (double& x : a) x /= al;
    double r[3] = {a[2], 0.0, -a[0]};                              // (0,1,0) x a
    double rl = std::sqrt(r[0] * r[0] + r[1] * r[1] + r[2] * r[2]);
    if (rl < 1e-9) { r[0] = 1.0; r[1] = r[2] = 0.0; rl = 1.0; }
    for (double& x : r) x /= rl;
    const double u[3] = {a[1] * r[2] - a[2] * r[1], a[2] * r[0] - a[0] * r[2], a[0] * r[1] - a[1] * r[0]};
    {
        rw::Frame* f = rc->getFrame();
        rw::Matrix* m = &f->matrix;
        m->right.x = static_cast<float>(r[0]); m->right.y = static_cast<float>(r[1]); m->right.z = static_cast<float>(r[2]);
        m->up.x = static_cast<float>(u[0]); m->up.y = static_cast<float>(u[1]); m->up.z = static_cast<float>(u[2]);
        m->at.x = static_cast<float>(a[0]); m->at.y = static_cast<float>(a[1]); m->at.z = static_cast<float>(a[2]);
        m->pos.x = static_cast<float>(cp[0]); m->pos.y = static_cast<float>(cp[1]); m->pos.z = static_cast<float>(cp[2]);
        m->update();
        f->updateObjects();
    }

    // the vertex colour FUN_004228f0 builds from DAT_00641310..13: white, alpha as measured above
    const std::uint8_t alpha = FtolByte(kShadowAlphaK * static_cast<double>(c.fade) * static_cast<double>(g_ssun[slot]));
    const float pos[3] = {c.root[12], c.root[13], c.root[14]};     // param_2 + 0x30..0x38 of the clump frame
    ShadowCollect(slot, pos, cp, r, u, alpha);

    // ---- FUN_00420050: the race camera's EndUpdate, then FUN_0041f8f0: clear to (0,0,0,0) (FUN_004c1bb0 mode 1 =
    // image), the RT camera's BeginUpdate, the hull, EndUpdate; then the race camera's BeginUpdate again ----------
    rw::RGBA clear = {0, 0, 0, 0};
    rc->clear(&clear, rw::Camera::CLEARIMAGE);
    mc->endUpdate();
    rc->beginUpdate();
    {
        // RS 8 (ZWRITE) = 0, RS 6 (ZTEST) = 0, the hull's render callback (atomic + 0x48), RS 6 = 1, RS 8 = 1.
        // ZWRITE goes first: librw keeps ZENABLE on (ZFUNC ALWAYS) when the test goes off with ZWRITE still on.
        rw::SetRenderState(rw::ZWRITEENABLE, 0);
        rw::SetRenderState(rw::ZTESTENABLE, 0);
        // the hull in the world: p' = p.x r + p.y u + p.z a + pos of the body frame (the hull frames' parent: `body`)
        const M root = FromD3D(c.body);
        std::vector<rw::RWDEVICE::Im3DVertex> v(static_cast<std::size_t>(c.hull_nv));
        for (int i = 0; i < c.hull_nv; ++i) {
            const float* p = &c.hull_v[3 * i];
            rw::RWDEVICE::Im3DVertex& o = v[static_cast<std::size_t>(i)];
            o.setX(p[0] * root.r[0] + p[1] * root.u[0] + p[2] * root.a[0] + root.p[0]);
            o.setY(p[0] * root.r[1] + p[1] * root.u[1] + p[2] * root.a[1] + root.p[1]);
            o.setZ(p[0] * root.r[2] + p[1] * root.u[2] + p[2] * root.a[2] + root.p[2]);
            o.setNormalX(0.f); o.setNormalY(0.f); o.setNormalZ(0.f);
            o.setColor(0, 0, 0, 255);                                  // FUN_00420420: the hull material colour 0xff000000
            o.setU(0.f); o.setV(0.f);
        }
        std::vector<rw::uint16> idx(static_cast<std::size_t>(c.hull_nt) * 3);
        for (int i = 0; i < c.hull_nt * 3; ++i) idx[static_cast<std::size_t>(i)] = c.hull_t[i];
        rw::SetRenderStatePtr(rw::TEXTURERASTER, nullptr);
        rw::SetRenderState(rw::VERTEXALPHA, 0);
        rw::im3d::Transform(v.data(), c.hull_nv, nullptr, 0);
        static const float kIdent[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        if (rw_drawrec_pre) {
            DrawRec::SetNextGeometry(v.data(), sizeof(v[0]), idx.data(), static_cast<unsigned>(idx.size()));
            rw_drawrec_pre("", kIdent);
        }
        rw::im3d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, idx.data(), static_cast<rw::int32>(idx.size()));
        if (rw_drawrec_post) rw_drawrec_post();
        rw::im3d::End();
        rw::SetRenderState(rw::ZTESTENABLE, 1);
        rw::SetRenderState(rw::ZWRITEENABLE, 1);
    }
    rc->endUpdate();
    // MASHED_DBG_SHADOWRT_DUMP=<prefix>, DIAGNOSTIC ONLY (default unset): the render texture right after the hull pass, as
    // <prefix>_<slot>_<n>.tga (BGRA), one per car for the first 8 passes of each car
    {
        static const char* s_dump = std::getenv("MASHED_DBG_SHADOWRT_DUMP");
        static int s_n[4] = {0, 0, 0, 0};
        if (s_dump && s_dump[0] && s_n[slot] < 8) {
            const int sz = kShadowRtSize[rt];
            if (rw::uint8* px = g_srt[rt].ras->lock(0, rw::Raster::LOCKREAD)) {
                char name[512];
                std::snprintf(name, sizeof(name), "%s_%d_%d.tga", s_dump, slot, s_n[slot]++);
                if (std::FILE* f = std::fopen(name, "wb")) {
                    unsigned char h[18] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, static_cast<unsigned char>(sz & 255),
                                           static_cast<unsigned char>(sz >> 8), static_cast<unsigned char>(sz & 255),
                                           static_cast<unsigned char>(sz >> 8), 32, 0x28};
                    std::fwrite(h, 1, 18, f);
                    for (int y = 0; y < sz; ++y) std::fwrite(px + static_cast<std::size_t>(y) * g_srt[rt].ras->stride, 1, static_cast<std::size_t>(sz) * 4, f);
                    std::fclose(f);
                }
                g_srt[rt].ras->unlock(0);
            }
        }
    }
    mc->beginUpdate();
    if (rebegin) rebegin();
}

void RaceFx_ShadowQuads(int slot) {
    if (!RaceFx_ShadowActive(slot)) return;
    ShadowSlot& S = g_sslot[slot];
    if (!g_srt[S.rt].ok) return;
    if (!S.ran) return;
    S.ran = false;
    if (S.n <= 0) return;                                           // FUN_004228f0: count == 0 draws nothing
    std::vector<rw::RWDEVICE::Im3DVertex> v(static_cast<std::size_t>(S.n) * 3);
    std::vector<rw::uint16> idx(static_cast<std::size_t>(S.n) * 3);
    for (int i = 0; i < S.n; ++i)
        for (int k = 0; k < 3; ++k) {
            rw::RWDEVICE::Im3DVertex& o = v[static_cast<std::size_t>(i * 3 + k)];
            o.setX(S.v[i][3 * k]); o.setY(S.v[i][3 * k + 1]); o.setZ(S.v[i][3 * k + 2]);
            o.setNormalX(0.f); o.setNormalY(0.f); o.setNormalZ(0.f);
            o.setColor(0xff, 0xff, 0xff, S.alpha);                  // DAT_00641310..13: white (DAT_005f6154), alpha
            o.setU(S.uv[i][2 * k]); o.setV(S.uv[i][2 * k + 1]);
            idx[static_cast<std::size_t>(i * 3 + k)] = static_cast<rw::uint16>(i * 3 + k);
        }
    // FUN_00420050: RS 8 (ZWRITE) = 0 around FUN_004228f0; there RS 1 = the RT raster, RS 12 = 1, RS 2 = 3
    // (clamp), the triangle list (FUN_004cd070 / FUN_004cd2d0 / FUN_004cd140), RS 12 = 0
    rw::SetRenderState(rw::ZWRITEENABLE, 0);
    rw::SetRenderStatePtr(rw::TEXTURERASTER, g_srt[S.rt].tex->raster);
    rw::SetRenderState(rw::TEXTUREFILTER, g_srt[S.rt].tex->getFilter());
    rw::SetRenderState(rw::TEXTUREADDRESSU, rw::Texture::CLAMP);
    rw::SetRenderState(rw::TEXTUREADDRESSV, rw::Texture::CLAMP);
    rw::SetRenderState(rw::VERTEXALPHA, 1);
    rw::SetRenderState(rw::SRCBLEND, rw::BLENDSRCALPHA);
    rw::SetRenderState(rw::DESTBLEND, rw::BLENDINVSRCALPHA);
    rw::im3d::Transform(v.data(), S.n * 3, nullptr, rw::im3d::VERTEXUV);
    static const float kIdent[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    if (rw_drawrec_pre) {
        DrawRec::SetNextGeometry(v.data(), sizeof(v[0]), idx.data(), static_cast<unsigned>(idx.size()));
        rw_drawrec_pre(ShadowRtName(S.rt), kIdent);
    }
    rw::im3d::RenderIndexedPrimitive(rw::PRIMTYPETRILIST, idx.data(), static_cast<rw::int32>(idx.size()));
    if (rw_drawrec_post) rw_drawrec_post();
    rw::im3d::End();
    rw::SetRenderState(rw::VERTEXALPHA, 0);
    rw::SetRenderState(rw::ZWRITEENABLE, 1);
}

}  // namespace LibRw
}  // namespace mashed_re
