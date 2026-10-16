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

}  // namespace

bool RaceFx_HudEnabled() { static const bool on = !EnvOff("MASHED_HUD3D"); return on; }

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
    // FUN_0040dfc0 @ 0x0040dfc0: FUN_00426ba0() == 0 or DAT_0063ba8c < 5 returns; 7 is the
    // standings branch (the standalone's 2D overlay); 5 / 6 below.
    if (!in.world_up || in.state < 5 || in.state > 6) return;
    LoadHud(width, height);
    if (!g_hud_ok) return;
    bool panels = false;
    switch (in.round_type) {
    case 2: break;                                   // FUN_0041e850 (not ported)
    case 3: case 4: case 5:
        if (in.mode_fd0 == 4 || in.mode_fd0 == 7 || in.mode_fd0 == 9 || in.mode_fd0 == 5 ||
            in.mode_fd0 == 8 || in.mode_fd0 == 10)
            break;                                   // FUN_0041ded0 / FUN_0041c300 / FUN_0041a3e0
        panels = in.team_play != 0;                  // default -> caseD_7
        break;
    case 6: case 10:
        panels = in.team_play != 0 && in.mode_fd0 != 2;
        break;
    case 0xb: break;                                 // FUN_00403160 (not ported)
    default:
        panels = in.team_play != 0;
        break;
    }
    // [UNCERTAIN U-9704] the non-team panels FUN_0041b630 @ 0x0041b630 -> FUN_0041b340 and the
    // per-mode HUDs above are not ported: nothing is drawn where the original draws them.
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
