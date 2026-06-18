// Mashed RE — R4 opener track renderer implementation. See TrackRenderer.h.

#include "TrackRenderer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "../Piz/PizReader.h"
#include "../Track/TrackWorld.h"
#include "../Track/DffModel.h"
#include "../Track/LapLogic.h"      // F4 lap-line crossing sequence (shared w/ test)
#include "../Txd/TxdDecoder.h"
#include "../Audio/AudioEngine.h"   // real SFX (permdict.rws) for countdown/powerups
#include "RwWorldRender.h"          // WS-E1: RW world render path (behind MASHED_RW_RENDER)
#include "../Ai/AiStandalone.h"     // WS-C-WIRE: standalone AI tick (behind MASHED_REAL_AI)
#include "../Vehicle/VehiclePhysicsRun.h"  // WS-A8: ported physics chain (behind MASHED_REAL_PHYSICS)
#include "../Ai/AiState.h"          // WS-AI-BRIDGE: ctrl-block / slot-table / spline addrs
#include "../Ai/AiData.h"           // WS-AI-BRIDGE: .AI loader (AiData_LoadInto)

namespace mashed_re {
namespace D3d9Render {

namespace {

// ===========================================================================
// WS-AI-BRIDGE (2026-06-17): make the standalone AI tick (Ai_Standalone_Tick,
// Ai/AiStandalone.cpp) actually drive the opponents. Three parts:
//   (1) Ai_BridgeLoad  — load AI<course>.AI (Common/AI.piz) into the controller
//       image @0x007f1a9c so the race-line banks fill (tick non-inert) + init the
//       per-vehicle slot table and behaviour records (race line, type0/idx0).
//   (2) host snapshot + Ai::Host fns — feed the tick each car's world pos/vel
//       (Ai::Host uses context-free fn ptrs, so they read file-static g_aib).
//   (3) the adapter (inline in UpdateCar) reads the tick's ctrl-block outputs and
//       steers/throttles ai_cars_, REPLACING the gate-ribbon scaffold when on.
// Absolute writes (0x007f1a9c, kSlotTableBase, kCtrlBlockBase) target the
// image-pad in mashed_re.exe (valid writable memory — standalone-exe-phase-h);
// everything is gated on MASHED_REAL_AI so the dev .asi (injected into MASHED.exe)
// never clobbers the original's live AI image. PENDING diff-original C4 — the
// ctrl->yaw/throttle mapping (turn rate, accel bands) is APPROXIMATE: the exact
// bands live in the un-ported FUN_00416250 ([U-C-BANDS]).
// ===========================================================================
struct AiBridgeState {
    float pos[4][2];   // [v] = {x,z};  v0 = player, v1..3 = ai_cars_[v-1]
    float vel[4][2];
    int   alive[4];
    int   course;
    bool  loaded;
};
AiBridgeState g_aib = {};

void aib_own_xz(int v, float* x, float* z) {
    if (v < 0 || v > 3) { *x = *z = 0.f; return; }
    *x = g_aib.pos[v][0]; *z = g_aib.pos[v][1];
}
void aib_own_vel_xz(int v, float* vx, float* vz) {
    if (v < 0 || v > 3) { *vx = *vz = 0.f; return; }
    *vx = g_aib.vel[v][0]; *vz = g_aib.vel[v][1];
}
int aib_alive(int v)       { return (v >= 0 && v <= 3) ? g_aib.alive[v] : 0; }
int aib_veh_type(int v)    { return v == 0 ? 0 : 2; }   // 0 = player(human); else AI
int aib_game_sub_mode()    { return 6; }                // race (FUN_0040e350)
int aib_round_type()       { return 3; }                // AI-enabled round (FUN_0042f6a0)
int aib_game_mode_fd0()    { return 0; }
int aib_track_index()      { return g_aib.course; }
int aib_ai_target_enable() { return 1; }

// (1) load AI<course>.AI from Common/AI.piz into the controller image.
bool Ai_BridgeLoad(int course, const char* trackPizPath) {
    // derive <...>/Common/AI.piz from <...>/TRACKS/<track>.piz (mirrors LED.piz).
    std::string ai(trackPizPath ? trackPizPath : "");
    std::size_t cut = ai.find_last_of("/\\");
    if (cut == std::string::npos) return false;
    cut = ai.find_last_of("/\\", cut - 1);
    if (cut == std::string::npos) return false;
    ai.resize(cut + 1);
    ai += "Common\\AI.piz";

    Piz::Archive arc;
    if (!arc.Load(ai.c_str())) return false;
    char want[32];
    std::snprintf(want, sizeof(want), "AI%d.AI", course);
    const std::uint8_t* raw = nullptr; std::uint32_t len = 0;
    for (std::uint32_t i = 0; i < arc.count(); ++i) {
        const char* n = arc.entry(i).name;
        const char* base = n;
        for (const char* p = n; *p; ++p) if (*p == '\\' || *p == '/') base = p + 1;
        if (_stricmp(base, want) == 0) { raw = arc.blob(i, &len); break; }
    }
    if (!raw) return false;
    if (!Ai::AiData_LoadInto(raw, len, reinterpret_cast<void*>(0x007f1a9cu))) return false;

    // slot table: vehicle v -> ctrl slot v (image-pad is zeroed; make it explicit).
    for (int v = 0; v < 4; ++v)
        Ai::I32(Ai::kSlotTableBase + static_cast<std::uintptr_t>(v) * Ai::kSlotTableStride) = v;
    // per-vehicle behaviour record -> race line (type 0, index 0).
    for (int v = 0; v < 4; ++v) {
        Ai::I32(Ai::kAiLineType    + static_cast<std::uintptr_t>(v) * Ai::kAiStateDwords * 4u) = 0;
        Ai::I32(Ai::kAiSplineIndex + static_cast<std::uintptr_t>(v) * Ai::kAiStateDwords * 4u) = 0;
    }
    Ai::Host h = {
        aib_game_sub_mode, aib_round_type, aib_game_mode_fd0, aib_track_index,
        aib_alive, aib_veh_type, aib_ai_target_enable, aib_own_xz, aib_own_vel_xz,
    };
    Ai::Ai_SetHost(&h);
    return true;
}

// G3: the internal-velocity -> world-position scale (shared with the player path's
// [U-A8-WORLDVEL] kWorldVel) so AI opponents driven through the real physics chain
// cover ground at the same calibrated pace. Env MASHED_WORLDVEL, default 0.22.
float AiPhysWorldVel() {
    static const float v = [] {
        const char* e = std::getenv("MASHED_WORLDVEL");
        float f = e ? (float)std::atof(e) : 0.22f;
        return (f > 0.f) ? f : 0.22f;
    }();
    return v;
}

// [item 2 — renderer] Flat per-face directional+ambient lighting for car models.
// The DFF path carries no per-vertex normals, so cars render flat; this folds a
// face-normal diffuse into the vertex colours so vehicles gain form/shading.
// APPROXIMATION toward RW's RpWorld lighting (the verbatim RW lighting model is
// part of the larger renderer port); two-sided (CULLMODE is NONE).
void ApplyCarLighting(std::vector<TrackRenderer::V>& vs) {
    const float L[3] = {0.40f, 0.78f, 0.48f};   // ~normalised sun direction
    const float amb = 0.45f;
    for (std::size_t i = 0; i + 2 < vs.size(); i += 3) {
        const TrackRenderer::V& a = vs[i]; const TrackRenderer::V& b = vs[i+1];
        const TrackRenderer::V& c = vs[i+2];
        const float e1[3] = {b.x-a.x, b.y-a.y, b.z-a.z};
        const float e2[3] = {c.x-a.x, c.y-a.y, c.z-a.z};
        float nx = e1[1]*e2[2]-e1[2]*e2[1];
        float ny = e1[2]*e2[0]-e1[0]*e2[2];
        float nz = e1[0]*e2[1]-e1[1]*e2[0];
        const float nl = std::sqrt(nx*nx+ny*ny+nz*nz);
        if (nl > 1e-6f) { nx/=nl; ny/=nl; nz/=nl; }
        float d = nx*L[0]+ny*L[1]+nz*L[2]; if (d < 0.f) d = -d;   // two-sided
        float s = amb + (1.f-amb)*d; if (s > 1.f) s = 1.f;
        for (int k = 0; k < 3; ++k) {
            std::uint32_t c0 = vs[i+k].c;
            std::uint8_t A = static_cast<std::uint8_t>(c0 >> 24);
            std::uint8_t R = static_cast<std::uint8_t>((c0 >> 16) & 0xff);
            std::uint8_t G = static_cast<std::uint8_t>((c0 >> 8) & 0xff);
            std::uint8_t B = static_cast<std::uint8_t>(c0 & 0xff);
            R = static_cast<std::uint8_t>(R*s); G = static_cast<std::uint8_t>(G*s);
            B = static_cast<std::uint8_t>(B*s);
            vs[i+k].c = (static_cast<std::uint32_t>(A)<<24)|(static_cast<std::uint32_t>(R)<<16)|
                        (static_cast<std::uint32_t>(G)<<8)|B;
        }
    }
}

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

// out = a * b (row-vector convention: v' = v * a * b)
void MatMul(D3DMATRIX* out, const D3DMATRIX& a, const D3DMATRIX& b) {
    D3DMATRIX r;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] +
                        a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
    *out = r;
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

// [F2] World matrix from an HAnim sample: unit quaternion (x,y,z,w) -> rotation
// + translation, in the row-vector convention the MTS/prop path already feeds
// to D3DTS_WORLD (matches D3DXMatrixRotationQuaternion).
void QuatPosMatrix(const float q[4], const float p[3], D3DMATRIX* m) {
    const float x = q[0], y = q[1], z = q[2], w = q[3];
    const float xx = x*x, yy = y*y, zz = z*z;
    const float xy = x*y, xz = x*z, yz = y*z, wx = w*x, wy = w*y, wz = w*z;
    m->_11 = 1.f - 2.f*(yy+zz); m->_12 = 2.f*(xy+wz);     m->_13 = 2.f*(xz-wy);     m->_14 = 0.f;
    m->_21 = 2.f*(xy-wz);       m->_22 = 1.f - 2.f*(xx+zz); m->_23 = 2.f*(yz+wx);   m->_24 = 0.f;
    m->_31 = 2.f*(xz+wy);       m->_32 = 2.f*(yz-wx);       m->_33 = 1.f - 2.f*(xx+yy); m->_34 = 0.f;
    m->_41 = p[0];             m->_42 = p[1];             m->_43 = p[2];           m->_44 = 1.f;
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

    // ---- F3 .UVA UV-anim: bind each track .UVA (rwID_UVANIMDICT) entry's
    // scroll rate (units/sec) to the materials that reference it through their
    // RW UVAnim material extension (rwID_UVANIMPLUGIN 0x135) — the original's
    // real binding (TrackWorld/DffModel parse the extension into
    // Material::uv_anim). Applies to WORLD materials here and to DFF props/sky
    // below: on Arctic the world BSP carries no UVAnim extension and the sea
    // (SEA.DFF) / sky (SKY.DFF) clumps do (names "bmp_Sea_M"/"bmp_Sky_M"), so
    // the prop path is where the scroll actually shows. See the format doc F3.
    std::vector<std::pair<std::string, MatScroll>> uv_rates;   // entry name -> rate
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if (ln < 4 || _stricmp(n + ln - 4, ".UVA") != 0) continue;
        std::uint32_t ul = 0;
        const std::uint8_t* ub = piz.blob(i, &ul);
        Track::UVDict uv;
        if (!ub || !uv.Parse(ub, ul)) break;
        for (const auto& e : uv.anims)
            if (e.du_dt != 0.f || e.dv_dt != 0.f)
                uv_rates.push_back({e.name, MatScroll{e.du_dt, e.dv_dt}});
        break;   // one .UVA dictionary per track
    }
    // UVAnim-extension name -> scroll rate (exact match; empty -> {0,0})
    auto uv_rate = [&](const char* nm) -> MatScroll {
        if (nm && nm[0])
            for (const auto& kv : uv_rates)
                if (kv.first == nm) return kv.second;
        return {};
    };
    // world materials: scroll those whose UVAnim extension names a .UVA entry.
    mat_scroll_.assign(world.materials.size(), {});
    uv_anim_ = false;
    for (std::size_t mi = 0; mi < world.materials.size(); ++mi) {
        const MatScroll s = uv_rate(world.materials[mi].uv_anim);
        if (s.du != 0.f || s.dv != 0.f) { mat_scroll_[mi] = s; uv_anim_ = true; }
    }

    // ---- F4 LAPDATA.LUA: real lap lines (parsed once; used by UpdateRace to
    // count laps at the data-declared finish gate instead of a hardcoded 0).
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        if (_strnicmp(piz.entry(i).name, "LAPDATA", 7) != 0) continue;
        std::uint32_t ll = 0;
        const std::uint8_t* lb = piz.blob(i, &ll);
        if (lb && ll)
            lap_data_.Parse(reinterpret_cast<const char*>(lb), ll);
        break;
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
        // POWERUPS_GOLD.LUA -> real power-up placement (pos/type/respawn). The
        // type ids are local consts in the LUA; we read the numeric arg from
        // Set_Current_Type and resolve via a name->id alias table.
        powerup_spawns_.clear();
        for (std::uint32_t i = 0; i < piz.count(); ++i) {
            if (_strnicmp(piz.entry(i).name, "POWERUPS", 8) != 0) continue;
            std::uint32_t pl = 0;
            const std::uint8_t* pb = piz.blob(i, &pl);
            if (!pb || pl == 0) break;
            std::string txt(reinterpret_cast<const char*>(pb), pl);
            // alias map from the "local NAME = id" lines at the top of the file
            std::vector<std::pair<std::string,int>> alias;
            {
                std::size_t p = 0;
                while ((p = txt.find("local ", p)) != std::string::npos) {
                    char nm[32]; int id = 0;
                    if (std::sscanf(txt.c_str() + p, "local %31[A-Za-z0-9_] = %d", nm, &id) == 2)
                        alias.emplace_back(nm, id);
                    p += 6;
                }
            }
            auto resolve = [&](const char* a) -> int {
                for (auto& kv : alias) if (kv.first == a) return kv.second;
                return std::atoi(a);   // numeric type
            };
            // walk Set_Current_Position(...) / Set_Current_Type(...) /
            // Set_Current_Respawn_Time(...) / Place_Powerup()
            PickupField::Spawn cur{}; cur.respawn = 5.f; bool havePos = false;
            std::size_t p = 0;
            auto findNext = [&](const char* key) { return txt.find(key, p); };
            while (p < txt.size()) {
                std::size_t pos = txt.find("Set_Current_Position", p);
                std::size_t typ = txt.find("Set_Current_Type", p);
                std::size_t rsp = txt.find("Set_Current_Respawn_Time", p);
                std::size_t plc = txt.find("Place_Powerup", p);
                std::size_t nxt = pos;
                if (typ < nxt) nxt = typ;
                if (rsp < nxt) nxt = rsp;
                if (plc < nxt) nxt = plc;
                if (nxt == std::string::npos) break;
                if (nxt == pos) {
                    std::sscanf(txt.c_str() + pos,
                                "Set_Current_Position( %f , %f , %f )",
                                &cur.pos[0], &cur.pos[1], &cur.pos[2]);
                    havePos = true; p = pos + 20;
                } else if (nxt == typ) {
                    char a[32] = {};
                    std::sscanf(txt.c_str() + typ, "Set_Current_Type( %31[A-Za-z0-9_] )", a);
                    cur.type = resolve(a); p = typ + 16;
                } else if (nxt == rsp) {
                    std::sscanf(txt.c_str() + rsp, "Set_Current_Respawn_Time( %f )", &cur.respawn);
                    p = rsp + 24;
                } else {
                    if (havePos) powerup_spawns_.push_back(cur);
                    havePos = false; cur.respawn = 5.f; p = plc + 13;
                }
            }
            (void)findNext;
            break;
        }
        auto load_prop = [&](const char* dff_name, Prop* p) -> bool {
            std::uint32_t dl = 0;
            const std::uint8_t* db = find_entry(dff_name, &dl);
            if (!db) return false;
            Track::DffModel m;
            if (!m.Parse(db, dl)) return false;
            BuildDffBatches(dev, m, dicts, &p->batches, &p->textures);
            // F3: bind each material's UVAnim-extension name to its .UVA rate.
            p->mat_scroll.assign(m.materials.size(), {});
            for (std::size_t mi = 0; mi < m.materials.size(); ++mi)
                p->mat_scroll[mi] = uv_rate(m.materials[mi].uv_anim);
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
                } else if (std::sscanf(line.c_str(),
                                       " Course_Id( %d )", &idx) == 1 ||
                           std::sscanf(line.c_str(),
                                       " Course_Id(%d)", &idx) == 1) {
                    course_id_ = idx;   // -> Common/LED.piz LE<id>.LED
                } else {
                    // Setup_Fog(start_frac, end, r, g, b) — track fog wiring
                    float fa = 0.f, fb = 0.f; int fr = 0, fg2 = 0, fb2 = 0;
                    char sk[64] = {};
                    if (std::sscanf(line.c_str(),
                                    " Setup_Fog( %f , %f , %d , %d , %d )",
                                    &fa, &fb, &fr, &fg2, &fb2) == 5 ||
                        std::sscanf(line.c_str(),
                                    " Setup_Fog(%f,%f,%d,%d,%d)",
                                    &fa, &fb, &fr, &fg2, &fb2) == 5) {
                        fog_on_ = true;
                        fog_start_ = fa * fb;   // start = fraction of range
                        fog_end_   = fb;
                        fog_color_ = D3DCOLOR_XRGB(fr & 0xFF, fg2 & 0xFF,
                                                   fb2 & 0xFF);
                    } else if (std::sscanf(line.c_str(),
                                    " Sky_Filename( %*d , \"%63[^\"]\" )",
                                    sk) == 1 ||
                               std::sscanf(line.c_str(),
                                    " Sky_Filename(%*d,\"%63[^\"]\")",
                                    sk) == 1) {
                        // sky clump: drawn first, z-write off, unfogged
                        load_prop(sk, &sky_);
                    } else if (std::sscanf(line.c_str(),
                                    " AI_Bsp_Filename( \"%63[^\"]\" )", sk) == 1 ||
                               std::sscanf(line.c_str(),
                                    " AI_Bsp_Filename(\"%63[^\"]\")", sk) == 1) {
                        // the gate BSP filename varies per track (ai.bsp /
                        // lap.bsp / Ai1.bsp); capture it so the gate parser finds
                        // the right entry instead of guessing "AI*.BSP".
                        std::snprintf(gate_bsp_, sizeof(gate_bsp_), "%s", sk);
                    }
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

    // F3 diag: materials that actually scroll, by where they live.
    if (log) {
        int wc = 0; for (const auto& s : mat_scroll_) if (s.du != 0.f || s.dv != 0.f) ++wc;
        int pc = 0; for (const auto& pr : props_) for (const auto& s : pr.mat_scroll)
                        if (s.du != 0.f || s.dv != 0.f) ++pc;
        int sc = 0; for (const auto& s : sky_.mat_scroll) if (s.du != 0.f || s.dv != 0.f) ++sc;
        std::fprintf(log, "  F3 UV-anim: %zu .UVA rate(s); scrolling materials "
                          "world=%d props=%d sky=%d\n", uv_rates.size(), wc, pc, sc);
    }

    // ---- F2 animated copters: COURSE.LUA SetCopter / KTCSCRIPT.LUA
    // KTC_NewCopter bind a copter model DFF (Common/Perm.piz) to a
    // General_Anim_Filename .ANM flight path (track piz); flown under the
    // per-frame HAnim transform (see LoadCopters + the Render copter pass).
    LoadCopters(dev, piz, piz_path, log);

    // ---- AI path gates (AI*.BSP — same world stream; 4-vert vertical quad
    // per gate; the material RED byte = gate index; LAPDATA's Lap_Line
    // numbers index these). Build ordered gate centers; gate 0 = start line.
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        // Prefer the COURSE.LUA-declared gate BSP (ai.bsp / lap.bsp / Ai1.bsp...);
        // fall back to the "AI*.BSP" heuristic when none was declared.
        const bool is_gate_bsp =
            gate_bsp_[0] ? (_stricmp(n, gate_bsp_) == 0)
                         : (ln >= 6 && _strnicmp(n, "AI", 2) == 0 &&
                            _stricmp(n + ln - 4, ".BSP") == 0);
        if (is_gate_bsp) {
            std::uint32_t al = 0;
            const std::uint8_t* ab = piz.blob(i, &al);
            Track::World aw;
            if (ab && aw.Parse(ab, al)) {
                gates_.assign(aw.materials.size(), Gate{});
                std::vector<int> counts(aw.materials.size(), 0);
                // per-gate unique vertex indices in stream order — the
                // original keeps the quad's 4 verts as node corners j=0..3
                // (FUN_00426d00); we need j=0 and j=3 for the camera port.
                std::vector<std::vector<std::uint32_t>> gate_verts(
                    aw.materials.size());
                std::vector<const Track::Sector*> gate_sector(
                    aw.materials.size(), nullptr);
                for (const auto& s : aw.sectors) {
                    const std::size_t nt = s.tris.size() / 4;
                    for (std::size_t ti = 0; ti < nt; ++ti) {
                        const std::uint16_t mat = s.tris[ti * 4 + 0];
                        // gate index = material RED byte (== mat order here)
                        const std::uint8_t gi = aw.materials[mat].rgba[0];
                        if (gi >= gates_.size()) continue;
                        gate_sector[gi] = &s;
                        for (int k = 1; k <= 3; ++k) {
                            const std::uint16_t vi = s.tris[ti * 4 + k];
                            gates_[gi].center[0] += s.verts[vi * 3 + 0];
                            gates_[gi].center[1] += s.verts[vi * 3 + 1];
                            gates_[gi].center[2] += s.verts[vi * 3 + 2];
                            ++counts[gi];
                            auto& gv = gate_verts[gi];
                            bool seen = false;
                            for (auto u : gv) if (u == vi) { seen = true; break; }
                            if (!seen) gv.push_back(vi);
                        }
                    }
                }
                for (std::size_t gi = 0; gi < gates_.size(); ++gi)
                    if (counts[gi] > 0)
                        for (int k = 0; k < 3; ++k)
                            gates_[gi].center[k] /= static_cast<float>(counts[gi]);
                // corners (vertex order ascending = stream order) + race dir
                for (std::size_t gi = 0; gi < gates_.size(); ++gi) {
                    Gate& g = gates_[gi];
                    auto& gv = gate_verts[gi];
                    if (gv.size() < 4 || !gate_sector[gi]) continue;
                    std::sort(gv.begin(), gv.end());
                    const float* vs = gate_sector[gi]->verts.data();
                    for (int k = 0; k < 3; ++k) {
                        g.c0[k] = vs[gv[0] * 3 + k];
                        g.c3[k] = vs[gv[3] * 3 + k];
                    }
                    // dir = quad normal (cross of two edges from c0),
                    // oriented toward the next gate's center
                    const float e1[3] = {vs[gv[1] * 3 + 0] - g.c0[0],
                                         vs[gv[1] * 3 + 1] - g.c0[1],
                                         vs[gv[1] * 3 + 2] - g.c0[2]};
                    const float e2[3] = {vs[gv[2] * 3 + 0] - g.c0[0],
                                         vs[gv[2] * 3 + 1] - g.c0[1],
                                         vs[gv[2] * 3 + 2] - g.c0[2]};
                    float n3[3] = {e1[1] * e2[2] - e1[2] * e2[1],
                                   e1[2] * e2[0] - e1[0] * e2[2],
                                   e1[0] * e2[1] - e1[1] * e2[0]};
                    const float nl = std::sqrt(n3[0] * n3[0] + n3[1] * n3[1] +
                                               n3[2] * n3[2]);
                    if (nl > 0.f)
                        for (float& v : n3) v /= nl;
                    const Gate& gn = gates_[(gi + 1) % gates_.size()];
                    const float to_next[3] = {gn.center[0] - g.center[0],
                                              gn.center[1] - g.center[1],
                                              gn.center[2] - g.center[2]};
                    const float dot = n3[0] * to_next[0] + n3[1] * to_next[1] +
                                      n3[2] * to_next[2];
                    for (int k = 0; k < 3; ++k)
                        g.dir[k] = (dot < 0.f) ? -n3[k] : n3[k];
                }
            }
            break;
        }
    }

    // RaceCamera wiring: gate ribbon -> camera-path nodes, plus the per-node
    // angle table from Common/LED.piz LE<Course_Id>.LED (see Race/RaceCamera).
    cam_nodes_.clear();
    for (const auto& g : gates_) {
        Race::RaceCamNode n{};
        for (int k = 0; k < 3; ++k) {
            n.dir[k] = g.dir[k];
            n.c0[k]  = g.c0[k];
            n.c3[k]  = g.c3[k];
        }
        cam_nodes_.push_back(n);
    }
    race_cam_.SetNodes(cam_nodes_.data(), static_cast<int>(cam_nodes_.size()));
    {
        // <...>/TRACKS/<track>.piz -> <...>/Common/LED.piz
        std::string led(piz_path);
        std::size_t cut = led.find_last_of("/\\");
        if (cut != std::string::npos) {
            cut = led.find_last_of("/\\", cut - 1);
            if (cut != std::string::npos) {
                led.resize(cut + 1);
                led += "Common\\LED.piz";
                race_cam_.LoadLed(led.c_str(), course_id_);
            }
        }
    }

    // WS-AI-BRIDGE: load AI<course>.AI -> controller image so Ai_Standalone_Tick is
    // non-inert (gated on MASHED_REAL_AI; gate-ribbon stays the driver otherwise).
    {
        static const bool s_real_ai_load = (std::getenv("MASHED_REAL_AI") != nullptr);
        g_aib.course  = course_id_;
        g_aib.loaded  = s_real_ai_load ? Ai_BridgeLoad(course_id_, piz_path) : false;
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

    // Orbit-camera focus from the AI gate ribbon (the raceable track). The raw
    // world bbox above is skewed by the skybox/backdrop, so its midpoint sits in
    // empty air well above the track and orbiting it frames mostly sky. The gate
    // centers trace the racing line, so their XZ centroid + max extent give a
    // vantage that keeps the track in frame and at the track surface Y.
    for (int k = 0; k < 3; ++k) track_center_[k] = center_[k];
    track_radius_ = radius_;
    if (gates_.size() >= 3) {
        float c[3] = {0.f, 0.f, 0.f};
        for (const auto& g : gates_)
            for (int k = 0; k < 3; ++k) c[k] += g.center[k];
        const float inv = 1.f / static_cast<float>(gates_.size());
        for (int k = 0; k < 3; ++k) c[k] *= inv;
        float rmax = 0.f;
        for (const auto& g : gates_) {
            const float gdx = g.center[0] - c[0];
            const float gdz = g.center[2] - c[2];
            const float r = std::sqrt(gdx * gdx + gdz * gdz);
            if (r > rmax) rmax = r;
        }
        for (int k = 0; k < 3; ++k) track_center_[k] = c[k];
        track_radius_ = (rmax > 1.f) ? rmax : radius_;
    }

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
        std::fprintf(log, "  CAM-DIAG bbox=[%.1f,%.1f,%.1f .. %.1f,%.1f,%.1f] "
                          "bbox_center=(%.1f,%.1f,%.1f) bbox_R=%.1f | "
                          "gate_center=(%.1f,%.1f,%.1f) gate_R=%.1f\n",
                     world.bbox[0], world.bbox[1], world.bbox[2],
                     world.bbox[3], world.bbox[4], world.bbox[5],
                     center_[0], center_[1], center_[2], radius_,
                     track_center_[0], track_center_[1], track_center_[2],
                     track_radius_);
        std::fclose(log);
    }
    ready_ = true;
    return true;
}

// [F2] Animated copters. COURSE.LUA wires them in two parts:
//   General_Anim_Filename( slot , "X.anm" )   slot -> flight-path .ANM (track piz)
//   SetCopter( "Copter"|"JetRanger" , slot )   scenery copter on that path
// and KTCSCRIPT.LUA adds the gameplay copter:
//   KTC_NewCopter( "KTC_Apache", slot )
// The named models (COPTER/JETRANGER/KTC_APACHE.DFF) + COPTERS.TXD live in the
// shared Common/Perm.piz, not the track piz. Each copter flies its bound HAnim
// path looped on the race clock (the .ANM durations are in seconds).
void TrackRenderer::LoadCopters(IDirect3DDevice9* dev, Piz::Archive& piz,
                                const char* piz_path, std::FILE* log) {
    auto basename = [](const char* n) -> const char* {
        const char* b = n;
        for (const char* p = n; *p; ++p)
            if (*p == '/' || *p == '\\') b = p + 1;
        return b;
    };
    auto find_blob = [&](Piz::Archive& a, const char* want_base,
                         std::uint32_t* len) -> const std::uint8_t* {
        for (std::uint32_t i = 0; i < a.count(); ++i)
            if (_stricmp(basename(a.entry(i).name), want_base) == 0)
                return a.blob(i, len);
        return nullptr;
    };

    // slot -> .anm filename; (model, slot) bindings
    char anm[16][64] = {};
    struct Bind { char model[32]; int slot; };
    std::vector<Bind> binds;
    auto scan_lua = [&](const char* txt, std::size_t len) {
        std::size_t pos = 0;
        while (pos < len) {
            std::size_t eol = pos;
            while (eol < len && txt[eol] != '\n') ++eol;
            std::string line(txt + pos, eol - pos);
            pos = eol + 1;
            const std::size_t cmt = line.find("--");
            if (cmt != std::string::npos) line.resize(cmt);
            int slot = -1;
            char nm[64] = {};
            const char* g = std::strstr(line.c_str(), "General_Anim_Filename");
            const char* s = std::strstr(line.c_str(), "SetCopter");
            const char* k = std::strstr(line.c_str(), "KTC_NewCopter");
            if (g) {
                char f[64] = {};
                if (std::sscanf(g, "General_Anim_Filename( %d %*[ ,]\"%63[^\"]\"",
                                &slot, f) == 2 && slot >= 0 && slot < 16)
                    std::snprintf(anm[slot], sizeof(anm[slot]), "%s", f);
            } else if (s) {
                if (std::sscanf(s, "SetCopter( \"%31[^\"]\" %*[ ,]%d",
                                nm, &slot) == 2 && slot >= 0) {
                    Bind b{}; std::snprintf(b.model, sizeof(b.model), "%s", nm);
                    b.slot = slot; binds.push_back(b);
                }
            } else if (k) {
                if (std::sscanf(k, "KTC_NewCopter( \"%31[^\"]\" %*[ ,]%d",
                                nm, &slot) == 2 && slot >= 0) {
                    Bind b{}; std::snprintf(b.model, sizeof(b.model), "%s", nm);
                    b.slot = slot; binds.push_back(b);
                }
            }
        }
    };
    std::uint32_t cl = 0;
    const std::uint8_t* cb = find_blob(piz, "COURSE.LUA", &cl);
    if (cb && cl) scan_lua(reinterpret_cast<const char*>(cb), cl);
    std::uint32_t kl = 0;
    const std::uint8_t* kb = find_blob(piz, "KTCSCRIPT.LUA", &kl);
    if (kb && kl) scan_lua(reinterpret_cast<const char*>(kb), kl);
    if (binds.empty()) return;

    // Common/Perm.piz (sibling of TRACKS/) — derived like LED.piz in Load().
    std::string perm(piz_path);
    std::size_t cut = perm.find_last_of("/\\");
    if (cut == std::string::npos) return;
    cut = perm.find_last_of("/\\", cut - 1);
    if (cut == std::string::npos) return;
    perm.resize(cut + 1);
    perm += "Common\\Perm.piz";
    Piz::Archive pp;
    if (!pp.Load(perm.c_str())) {
        if (log) std::fprintf(log, "  F2 copters: Perm.piz load FAILED (%s): %s\n",
                              perm.c_str(), pp.last_error());
        return;
    }
    std::vector<Txd::Dictionary> dicts(1);
    {
        std::uint32_t tl = 0;
        const std::uint8_t* tb = find_blob(pp, "COPTERS.TXD", &tl);
        if (tb) dicts[0].Decode(tb, tl);
    }

    // build a model DFF on demand, cached by "<NAME>.DFF"
    std::vector<std::string> model_names;   // parallel to copter_models_
    auto model_index = [&](const char* name) -> int {
        char up[40];
        std::size_t j = 0;
        for (; name[j] && j < sizeof(up) - 1; ++j)
            up[j] = static_cast<char>(::toupper(static_cast<unsigned char>(name[j])));
        up[j] = '\0';
        char want[48];
        std::snprintf(want, sizeof(want), "%s.DFF", up);
        for (std::size_t mi = 0; mi < model_names.size(); ++mi)
            if (model_names[mi] == want) return static_cast<int>(mi);
        std::uint32_t dl = 0;
        const std::uint8_t* db = find_blob(pp, want, &dl);
        if (!db) return -1;
        Track::DffModel m;
        if (!m.Parse(db, dl)) return -1;
        CopterModel cm;
        BuildDffBatches(dev, m, dicts, &cm.batches, &cm.textures);
        copter_models_.push_back(std::move(cm));
        model_names.emplace_back(want);
        return static_cast<int>(copter_models_.size()) - 1;
    };

    int loaded = 0;
    for (const auto& b : binds) {
        if (b.slot < 0 || b.slot >= 16 || !anm[b.slot][0]) continue;
        std::uint32_t al = 0;
        const std::uint8_t* ab = find_blob(piz, anm[b.slot], &al);
        if (!ab) continue;
        Track::HAnim ha;
        if (!ha.Parse(ab, al)) continue;
        const int mi = model_index(b.model);
        if (mi < 0) continue;
        AnimCopter c;
        c.model = mi;
        c.anim = std::move(ha);
        copters_.push_back(std::move(c));
        ++loaded;
    }
    if (log) std::fprintf(log,
        "  F2 copters: %d flying, %zu models from %s\n",
        loaded, copter_models_.size(), perm.c_str());
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

float TrackRenderer::GroundProbe(float x, float z, bool* ok,
                                 float normal[3]) const {
    // Like GroundHeight but also returns the hit triangle's (upward) normal
    // for slope gravity.
    float best = -1e9f;
    bool found = false;
    normal[0] = 0.f; normal[1] = 1.f; normal[2] = 0.f;
    const std::size_t nt = col_tris_.size() / 3;
    for (std::size_t i = 0; i < nt; ++i) {
        const float* a = &col_verts_[col_tris_[i * 3 + 0] * 3];
        const float* b = &col_verts_[col_tris_[i * 3 + 1] * 3];
        const float* c = &col_verts_[col_tris_[i * 3 + 2] * 3];
        const float d00x = b[0] - a[0], d00z = b[2] - a[2];
        const float d01x = c[0] - a[0], d01z = c[2] - a[2];
        const float den = d00x * d01z - d01x * d00z;
        if (den > -1e-9f && den < 1e-9f) continue;
        const float px = x - a[0], pz = z - a[2];
        const float u = (px * d01z - d01x * pz) / den;
        const float v = (d00x * pz - px * d00z) / den;
        if (u < 0.f || v < 0.f || u + v > 1.f) continue;
        const float y = a[1] + u * (b[1] - a[1]) + v * (c[1] - a[1]);
        if (y > best) {
            best = y; found = true;
            const float e1[3] = {b[0]-a[0], b[1]-a[1], b[2]-a[2]};
            const float e2[3] = {c[0]-a[0], c[1]-a[1], c[2]-a[2]};
            float n[3] = {e1[1]*e2[2] - e1[2]*e2[1],
                          e1[2]*e2[0] - e1[0]*e2[2],
                          e1[0]*e2[1] - e1[1]*e2[0]};
            const float l = std::sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
            if (l > 1e-6f) {
                const float s = (n[1] < 0.f ? -1.f : 1.f) / l;  // upward
                normal[0] = n[0]*s; normal[1] = n[1]*s; normal[2] = n[2]*s;
            }
        }
    }
    if (ok) *ok = found;
    return best;
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

    // ---- wheel identification (visual spin/steer) -------------------------
    // Per-atomic bboxes are on the batches. A wheel atomic: its two largest
    // dims are nearly equal (the disc) with the smallest dim on a horizontal
    // axis (the axle), small overall, sitting at the model's ground level.
    // Pick the best candidate in each of the 4 corners (long x lateral).
    struct AtomInfo { float box[6]; bool wheel; };
    std::vector<AtomInfo> atoms;
    for (const auto& b : model.batches) {
        if (b.atomic >= static_cast<std::int32_t>(atoms.size()))
            atoms.resize(static_cast<std::size_t>(b.atomic) + 1);
        std::memcpy(atoms[static_cast<std::size_t>(b.atomic)].box, b.abox,
                    sizeof(b.abox));
        atoms[static_cast<std::size_t>(b.atomic)].wheel = false;
    }
    const float cdx = model.bbox[3] - model.bbox[0];
    const float cdz = model.bbox[5] - model.bbox[2];
    const bool long_is_x = cdx >= cdz;     // car's long (fwd/back) model axis
    const float ccx = (model.bbox[0] + model.bbox[3]) * 0.5f;
    const float ccz = (model.bbox[2] + model.bbox[5]) * 0.5f;
    int best[4] = {-1, -1, -1, -1};
    float best_d[4] = {};
    for (std::size_t ai = 0; ai < atoms.size(); ++ai) {
        const float* bx = atoms[ai].box;
        const float dx = bx[3] - bx[0], dy = bx[4] - bx[1], dz = bx[5] - bx[2];
        const float lat = long_is_x ? dz : dx;     // axle thickness
        const float d1 = dy, d2 = long_is_x ? dx : dz;
        const float dia = (d1 + d2) * 0.5f;
        if (dia < 0.05f || dia > 1.2f) continue;            // wheel-sized
        if (std::fabs(d1 - d2) > 0.25f * dia) continue;     // disc-shaped
        if (lat > dia * 0.9f) continue;                     // thin on the axle
        if (bx[1] > model.bbox[1] + 0.25f * (model.bbox[4] - model.bbox[1]))
            continue;                                       // at ground level
        const float cx = (bx[0] + bx[3]) * 0.5f - ccx;
        const float cz = (bx[2] + bx[5]) * 0.5f - ccz;
        const float lng = long_is_x ? cx : cz;
        const float sde = long_is_x ? cz : cx;
        const int q = (lng >= 0.f ? 1 : 0) * 2 + (sde >= 0.f ? 1 : 0);
        if (dia > best_d[q]) { best_d[q] = dia; best[q] = static_cast<int>(ai); }
    }
    int nwheels = 0;
    for (int q = 0; q < 4; ++q) if (best[q] >= 0) ++nwheels;
    if (nwheels == 4) {
        for (int q = 0; q < 4; ++q)
            atoms[static_cast<std::size_t>(best[q])].wheel = true;
    }

    // textures for ALL car materials
    std::vector<std::vector<V>> all_batches;
    BuildDffBatches(dev, model, dicts, &all_batches, &car_textures_);
    // body batches exclude wheel atomics; wheels collected pivot-relative
    car_batches_.assign(model.materials.size(), {});
    wheels_.clear();
    if (nwheels == 4) {
        for (int q = 0; q < 4; ++q) {
            CarWheel w;
            const float* bx = atoms[static_cast<std::size_t>(best[q])].box;
            w.pivot[0] = (bx[0] + bx[3]) * 0.5f;
            w.pivot[1] = (bx[1] + bx[4]) * 0.5f;
            w.pivot[2] = (bx[2] + bx[5]) * 0.5f;
            w.radius = (bx[4] - bx[1]) * 0.5f;
            w.lateral_is_x = !long_is_x;
            const float lng = (long_is_x ? w.pivot[0] - ccx
                                         : w.pivot[2] - ccz);
            w.front = lng >= 0.f;   // +long pair steers (verified visually)
            wheels_.push_back(std::move(w));
        }
    }
    for (const auto& b : model.batches) {
        const bool is_wheel =
            nwheels == 4 &&
            atoms[static_cast<std::size_t>(b.atomic)].wheel;
        // convert this model batch to V verts
        std::vector<V> vs;
        const std::size_t n = b.tris.size();
        vs.reserve(n);
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
            vs.push_back(v);
        }
        ApplyCarLighting(vs);   // [item 2] flat directional shading (was flat-white)
        if (is_wheel) {
            for (auto& w : wheels_) {
                const float* bx = atoms[static_cast<std::size_t>(b.atomic)].box;
                const float px = (bx[0] + bx[3]) * 0.5f;
                if (std::fabs(px - w.pivot[0]) < 1e-4f &&
                    std::fabs((bx[2] + bx[5]) * 0.5f - w.pivot[2]) < 1e-4f) {
                    for (auto& v : vs) {
                        v.x -= w.pivot[0]; v.y -= w.pivot[1]; v.z -= w.pivot[2];
                    }
                    w.parts.emplace_back(b.material, std::move(vs));
                    break;
                }
            }
        } else {
            auto& out = car_batches_[b.material];
            out.insert(out.end(), vs.begin(), vs.end());
        }
    }
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
            // stretch: 3 AI cars seeded a few gates ahead, looping the path
            ai_cars_.clear();
            for (int i = 0; i < 3 && gates_.size() > 8; ++i) {
                AiCar a{};
                const std::size_t g = static_cast<std::size_t>(2 + i * 2);
                a.pos[0] = gates_[g].center[0];
                a.pos[2] = gates_[g].center[2];
                bool aok = false;
                const float ay = GroundHeight(a.pos[0], a.pos[2], &aok);
                a.pos[1] = (aok ? ay : gates_[g].center[1]) + car_ground_off_;
                a.target = static_cast<int>(g + 1);
                a.speed  = radius_ * (0.10f + 0.02f * static_cast<float>(i));
                a.yaw    = 0.f;
                ai_cars_.push_back(a);
            }
            if (log) {
                std::fprintf(log, "R6 car spawn at REAL start line: gate0="
                                  "(%.2f, %.2f, %.2f) yaw=%.2f (gates=%zu) "
                                  "wheels=%zu",
                             car_pos_[0], car_pos_[1], car_pos_[2], car_yaw_,
                             gates_.size(), wheels_.size());
                for (const auto& w : wheels_)
                    std::fprintf(log, " [%.2f,%.2f,%.2f r=%.2f%s]",
                                 w.pivot[0], w.pivot[1], w.pivot[2], w.radius,
                                 w.front ? " F" : "");
                std::fprintf(log, "\n");
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

// PORTED car selection (FUN_0040d110, 0x0040d110): the menu vehicle id snaps
// to its model group ((id/6)*6 — six livery DFFs per model in one piz) and
// each player's car = base + livery. Mode 0xb assigns livery = player index;
// we use that as the default for the three AI cars (liveries 1..3) until the
// per-player DAT_007f1a1c writer is traced.
bool TrackRenderer::LoadCarLiveries(IDirect3DDevice9* dev,
                                    const char* piz_path,
                                    const char* dff_base,
                                    const char* log_path) {
    std::FILE* log = log_path ? std::fopen(log_path, "a") : nullptr;
    Piz::Archive piz;
    if (!piz.Load(piz_path)) {
        if (log) std::fclose(log);
        return false;
    }
    std::vector<std::pair<const std::uint8_t*, std::uint32_t>> txds;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if (ln > 4 && _stricmp(n + ln - 4, ".TXD") == 0) {
            std::uint32_t bl = 0;
            const std::uint8_t* b = piz.blob(i, &bl);
            if (b) txds.emplace_back(b, bl);
        }
    }
    std::vector<Txd::Dictionary> dicts(txds.size());
    for (std::size_t di = 0; di < txds.size(); ++di)
        dicts[di].Decode(txds[di].first, txds[di].second);

    car_variants_.clear();
    car_variants_.resize(3);
    int loaded = 0;
    for (int li = 1; li <= 3; ++li) {
        char want[64];
        std::snprintf(want, sizeof(want), "%s%d.DFF", dff_base, li);
        const std::uint8_t* dff = nullptr;
        std::uint32_t dl = 0;
        for (std::uint32_t i = 0; i < piz.count(); ++i) {
            if (_stricmp(piz.entry(i).name, want) == 0) {
                dff = piz.blob(i, &dl);
                break;
            }
        }
        if (!dff) continue;
        Track::DffModel model;
        if (!model.Parse(dff, dl)) continue;
        CarVariant& v = car_variants_[static_cast<std::size_t>(li - 1)];
        BuildDffBatches(dev, model, dicts, &v.batches, &v.textures);
        ++loaded;
    }
    if (log) {
        std::fprintf(log, "R6 car liveries: %s base=%s loaded=%d/3\n",
                     piz_path, dff_base, loaded);
        std::fclose(log);
    }
    return loaded > 0;
}

void TrackRenderer::UpdateCar(const DriveInput& in) {
    if (!car_ready_ || in.dt <= 0.f) return;
    // R6 handling v2 — velocity-vector model whose STRUCT SHAPE follows
    // VehicleControlUpdate (0x00470670): velocity vec (+0x9b0), forward vec
    // (+0x9d4), throttle as a force along forward (+0x1a8 pipeline, negated
    // for reverse), slide = the velocity component orthogonal to forward
    // (the +0xb0c measure). Tuning constants are SCAFFOLD (see
    // re/analysis/standalone_menu_sm/HANDLING_V2_2026-06-10.md).
    // Constants fitted to LIVE original telemetry (Quick Battle, 4 cars
    // sampled at VehicleControlUpdate 0x00470670; harvest_handling.py,
    // log/handling_telemetry.jsonl). Transferable measurements (rates, unit-
    // independent): coast decay k ~= 0.21/s (weighty ~4.7s time-constant);
    // lateral slide rms ~2% of forward speed (strong grip). max-speed const
    // +0x190 = 34.0 (internal units; world scale differs, so kTop stays
    // track-radius-relative). See HANDLING_V2_2026-06-10.md.
    float       kTop      = radius_ * 0.25f;
    if (boost_timer_ > 0.f) kTop *= 1.4f;   // Boost power-up: +40% top speed
    const float kDrag     = 0.21f;          // HARVESTED coast decay
    const float kThrottle = kTop * 0.42f;   // top=kThrottle/kDrag=2x kTop ->
                                            // clamped; ~3.3s spool (faithful)
    const float kGrip     = 6.0f;    // strong (slide ~2% of fwd, harvested)
    const float kSteer    = 2.2f;
    const float kGravity  = radius_ * 0.12f;

    // Countdown freeze (3-2-1-GO): cars are held still until it elapses.
    if (countdown_ > 0.f) {
        countdown_ -= in.dt;
        car_speed_ = 0.f; car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
        for (auto& a : ai_cars_) a.cur_speed = 0.f;
        if (countdown_ <= 0.f) Audio::SfxPlay("go", 0.9f);   // real "GO!" SFX
        if (countdown_ > 0.f) return;
    }

    float accel = in.accel, steer = in.steer;
    if (round_mode_ && !human_drive_) {
        // exhibition round: the player car auto-follows the gate loop too
        // (skipped when human_drive_ — then accel/steer come from the player).
        if (!gates_.empty() && race_[0].alive) {
            const float* g = gates_[static_cast<std::size_t>(race_[0].gate) %
                                    gates_.size()].center;
            const float want = std::atan2(g[2] - car_pos_[2],
                                          g[0] - car_pos_[0]);
            float err = want - car_yaw_;
            while (err >  3.14159f) err -= 6.28318f;
            while (err < -3.14159f) err += 6.28318f;
            steer = (err > 0.25f) ? 1.f : (err < -0.25f ? -1.f : err * 4.f);
            accel = 0.82f;   // slowest car on the grid
        } else {
            accel = 0.f; steer = 0.f;
        }
    }
    // [G4] AI-driven player (autonomous playthrough, MASHED_AI_DRIVES_PLAYER): source the
    // player's steer/accel from slot 0's AI ctrl block — the standalone AI controller already
    // steps v0 (Ai_Standalone_Tick), so a complete race/cup can run + be verified headless on
    // the REAL systems (no human, no gate-ribbon scaffold). Last frame's ctrl (the tick runs
    // later in this fn); one-frame lag is negligible. Requires the .AI bridge (real AI) loaded.
    static const bool s_ai_player = (std::getenv("MASHED_AI_DRIVES_PLAYER") != nullptr);
    if (s_ai_player && g_aib.loaded && Ai::I32(Ai::kSplineRaceCnt) > 3) {
        const int slot = Ai::I32(Ai::kSlotTableBase + 0 * Ai::kSlotTableStride);
        const std::uint8_t* c = reinterpret_cast<const std::uint8_t*>(
            Ai::kCtrlBlockBase + static_cast<std::uintptr_t>(slot) * Ai::kCtrlBlockStride);
        steer = (static_cast<float>(c[0]) - static_cast<float>(c[1])) / 255.0f;
        accel = c[4] ? static_cast<float>(c[4]) / 255.0f
                     : -(static_cast<float>(c[5]) / 255.0f);
    }

    // WS-A8 (2026-06-17): when MASHED_REAL_PHYSICS is on, drive the player car
    // through the ported chain (A3 init -> A4 control -> A5 -> A6a -> A6b) instead
    // of the kinematic scaffold. PENDING runtime-smoke + C4 (WS-A-VERIFY-3).
    if (Vehicle::VehiclePhysics_Enabled()) {
        static bool s_pinit = false;
        if (!s_pinit) {
            Vehicle::VehiclePhysics_Init(1 + static_cast<int>(ai_cars_.size()), course_id_);
            // WS-A8-GAPS: feed the track collision soup so the wheel solver finds
            // ground -> A5 suspension force is live (was the empty-terrain gap).
            Vehicle::VehiclePhysics_SetWorld(
                col_verts_.data(), static_cast<int>(col_verts_.size() / 3),
                col_tris_.data(),  static_cast<int>(col_tris_.size()  / 3));
            s_pinit = true;
        }
        Vehicle::PlayerCarIO io;
        io.pos[0] = car_pos_[0]; io.pos[1] = car_pos_[1]; io.pos[2] = car_pos_[2];
        io.vel[0] = car_vel_[0]; io.vel[1] = car_vel_[1]; io.vel[2] = car_vel_[2];
        io.yaw = car_yaw_; io.speed = car_speed_;
        for (int k = 0; k < 8; ++k) io.input[k] = 0;
        // Input byte map RESOLVED (ai_ctrl_byte_map_RESOLVED + WS-A8-STEER):
        // [0]/[1]=STEER (sign A/B; A4 FUN_00470670 reads them and writes the front-
        // wheel steer angle to +0x1a8/+0x26c), [4]=accelerator, [5]=brake/reverse
        // (A6a FUN_00467650 reads them). StepPlayer sets input[0]/[1] from io.steer.
        io.input[4] = static_cast<unsigned char>((accel > 0.f ? accel : 0.f) * 255.f);
        io.input[5] = static_cast<unsigned char>((accel < 0.f ? -accel : 0.f) * 255.f);
        io.steer    = steer;   // [-1,+1] -> descriptor steer bytes [0]/[1]
        // G2 DRIVE-STABILIZE (2026-06-18): ground from the car's CURRENT pos (is it on
        // the ground NOW?), not a probe-ahead with last frame's velocity — the
        // probe-ahead + a runaway velocity made `grounded` oscillate 1/0 each frame,
        // so A6a's grounded-gated grip fired only intermittently -> no yaw. (diag:
        // re/analysis/STEER_CALIB_RUNTIME_2026-06-18.md)
        bool gok = false;
        GroundHeight(car_pos_[0], car_pos_[2], &gok);
        io.grounded = gok ? 1 : 0;
        Vehicle::VehiclePhysics_StepPlayer(in.dt, io);
        car_vel_[0] = io.vel[0]; car_vel_[1] = io.vel[1]; car_vel_[2] = io.vel[2];
        car_speed_  = io.speed;
        car_yaw_ = io.yaw;   // WS-A8-STEER: physics-integrated heading (+0x9c0)
        // G2 (2026-06-18): do NOT cap speed here. A6a's grip-clamp #6 (Integrate2.cpp
        // 324-348) self-limits speed + damps angular velocity against thresholds tuned
        // for the chain's NATURAL internal velocity scale (32768, low-speed-stop at 16);
        // an external cap to ~kTop pushes the car permanently into the over-damp /
        // full-stop regime and KILLS the yaw rate. Let the chain run at its scale; convert
        // to world units only for the POSITION step via kWorldVel (internal->world).
        // [U-A8-WORLDVEL] the chain integrates velocity in its own internal length
        // scale (self-limits ~150); the standalone track (original .piz geometry) is
        // small, so convert internal->world for the POSITION step. Scaled WITH kYawScale
        // (VehiclePhysicsRun) by the same factor so the turn radius (path shape) is
        // preserved — both linear + angular advance proportionally slower. Env-tunable
        // (MASHED_WORLDVEL) for runtime calibration without a rebuild.
        static const float kWorldVel = [] {
            const char* e = std::getenv("MASHED_WORLDVEL");
            float v = e ? (float)std::atof(e) : 0.22f;   // calibrated 2026-06-18 (smooth lap)
            return (v > 0.f) ? v : 0.22f;
        }();
        const float nx = car_pos_[0] + car_vel_[0] * kWorldVel * in.dt;
        const float nz = car_pos_[2] + car_vel_[2] * kWorldVel * in.dt;
        bool nok = false;
        const float ngy = GroundHeight(nx, nz, &nok);
        if (nok) { car_pos_[0] = nx; car_pos_[2] = nz; car_pos_[1] = ngy + car_ground_off_; }
        else { car_speed_ = 0.f; car_vel_[0] = car_vel_[2] = 0.f; }
    } else {
    const float fwd[3] = {std::cos(car_yaw_), 0.f, std::sin(car_yaw_)};
    // throttle force along forward
    car_vel_[0] += fwd[0] * accel * kThrottle * in.dt;
    car_vel_[2] += fwd[2] * accel * kThrottle * in.dt;
    // slope gravity: project (0,-g,0) onto the ground plane (XZ components)
    bool gok = false;
    float n[3];
    GroundProbe(car_pos_[0], car_pos_[2], &gok, n);
    if (gok) {
        car_vel_[0] += kGravity * n[1] * n[0] * in.dt;
        car_vel_[2] += kGravity * n[1] * n[2] * in.dt;
    }
    // grip: bleed the lateral (slide) component; drag the forward one
    float vf = car_vel_[0] * fwd[0] + car_vel_[2] * fwd[2];
    float lx = car_vel_[0] - fwd[0] * vf;
    float lz = car_vel_[2] - fwd[2] * vf;
    const float bleed = 1.f - ((kGrip * in.dt > 1.f) ? 1.f : kGrip * in.dt);
    lx *= bleed; lz *= bleed;
    vf -= vf * kDrag * in.dt;
    if (vf >  kTop) vf = kTop;
    if (vf < -kTop * 0.4f) vf = -kTop * 0.4f;
    car_vel_[0] = fwd[0] * vf + lx;
    car_vel_[2] = fwd[2] * vf + lz;
    car_speed_ = vf;
    car_yaw_ += steer * kSteer * (vf / kTop) * in.dt;
    const float nx = car_pos_[0] + car_vel_[0] * in.dt;
    const float nz = car_pos_[2] + car_vel_[2] * in.dt;
    bool ok = false;
    const float gy = GroundHeight(nx, nz, &ok);
    if (ok) {
        car_pos_[0] = nx; car_pos_[2] = nz;
        car_pos_[1] = gy + car_ground_off_;
    } else {
        car_speed_ = 0.f;   // island edge / off-collision: stop
        car_vel_[0] = car_vel_[2] = 0.f;
    }
    }  // end MASHED_REAL_PHYSICS else (scaffold body)
    UpdateRace(in.dt);
    // WS-AI-BRIDGE (2026-06-17): when MASHED_REAL_AI is on AND the .AI race-line banks
    // loaded (kSplineRaceCnt>3), the standalone AI controller (Ai_Standalone_Tick) drives
    // the opponents via its ctrl-block output; otherwise the gate-ribbon scaffold drives.
    static const bool s_real_ai = (std::getenv("MASHED_REAL_AI") != nullptr);
    const int ng = static_cast<int>(gates_.size());
    const bool real_ai = s_real_ai && g_aib.loaded && (Ai::I32(Ai::kSplineRaceCnt) > 3);
    if (real_ai) {
        // (a) snapshot car world pos/vel for the controller host (v0=player, v1..3=ai).
        g_aib.pos[0][0] = car_pos_[0]; g_aib.pos[0][1] = car_pos_[2];
        g_aib.vel[0][0] = car_vel_[0]; g_aib.vel[0][1] = car_vel_[2];
        g_aib.alive[0] = 1;
        for (int i = 0; i < 3 && i < static_cast<int>(ai_cars_.size()); ++i) {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(i)];
            g_aib.pos[i + 1][0] = a.pos[0]; g_aib.pos[i + 1][1] = a.pos[2];
            g_aib.vel[i + 1][0] = std::cos(a.yaw) * a.cur_speed;
            g_aib.vel[i + 1][1] = std::sin(a.yaw) * a.cur_speed;
            g_aib.alive[i + 1] = (round_mode_ && !race_[i + 1].alive) ? 0 : 1;
        }
        // (b) run the real controller — writes per-vehicle ctrl blocks.
        Ai::Ai_Standalone_Tick();
        // (c) G3 adapter: feed each opponent's AI descriptor ctrl bytes into the SAME
        // physics chain as the player (VehiclePhysics_StepCar on slots 1..3), replacing
        // the old kinematic turn-rate/accel-band approximation. The AI ctrl block is
        // already in descriptor format (ctrl[0]/[1]=steer, ctrl[4]=accel, ctrl[5]=brake),
        // exactly what the chain's input[] expects -> no lossy remap.
        const bool phys = Vehicle::VehiclePhysics_Enabled();
        const float kWorldVelAi = AiPhysWorldVel();
        for (int ci = 0; ci < static_cast<int>(ai_cars_.size()); ++ci) {
            AiCar& a = ai_cars_[static_cast<std::size_t>(ci)];
            const int v = ci + 1;
            if (round_mode_ && !race_[ci + 1].alive) {
                a.cur_speed = 0.f; a.vel[0] = a.vel[1] = a.vel[2] = 0.f; continue;
            }
            if (a.spin > 0.f) {   // spun out by a missile/mine: kinematic spin in place
                a.spin -= in.dt; a.yaw += 12.0f * in.dt;
                a.cur_speed = 0.f; a.vel[0] = a.vel[1] = a.vel[2] = 0.f; continue;
            }
            if (a.slow > 0.f) a.slow -= in.dt;
            const int slot = Ai::I32(Ai::kSlotTableBase + static_cast<std::uintptr_t>(v) * Ai::kSlotTableStride);
            const std::uint8_t* ctrl = reinterpret_cast<const std::uint8_t*>(
                Ai::kCtrlBlockBase + static_cast<std::uintptr_t>(slot) * Ai::kCtrlBlockStride);
            if (phys) {
                // --- REAL physics: AI descriptor bytes -> the verbatim chain (StepCar) ---
                Vehicle::PlayerCarIO io{};
                io.pos[0] = a.pos[0]; io.pos[1] = a.pos[1]; io.pos[2] = a.pos[2];
                io.vel[0] = a.vel[0]; io.vel[1] = a.vel[1]; io.vel[2] = a.vel[2];
                io.yaw    = a.yaw;
                io.speed  = std::sqrt(a.vel[0]*a.vel[0] + a.vel[2]*a.vel[2]);
                std::uint8_t accel = ctrl[4];
                if (a.slow > 0.f) accel = static_cast<std::uint8_t>(accel * 0.35f);  // shock power-up
                io.input[4] = accel;            // accelerator byte
                io.input[5] = ctrl[5];          // brake byte
                io.steer = (static_cast<float>(ctrl[0]) - static_cast<float>(ctrl[1])) / 255.0f;
                bool gok = false; GroundHeight(a.pos[0], a.pos[2], &gok);
                io.grounded = gok ? 1 : 0;
                Vehicle::VehiclePhysics_StepCar(v, in.dt, io);
                a.vel[0] = io.vel[0]; a.vel[1] = io.vel[1]; a.vel[2] = io.vel[2];
                a.yaw = io.yaw;
                a.cur_speed = std::sqrt(io.vel[0]*io.vel[0] + io.vel[2]*io.vel[2]);
                const float nx2 = a.pos[0] + io.vel[0] * kWorldVelAi * in.dt;
                const float nz2 = a.pos[2] + io.vel[2] * kWorldVelAi * in.dt;
                bool aok = false;
                const float ay = GroundHeight(nx2, nz2, &aok);
                if (aok) { a.pos[0] = nx2; a.pos[2] = nz2; a.pos[1] = ay + car_ground_off_; }
                else { a.vel[0] = a.vel[2] = 0.f; a.cur_speed = 0.f; }
            } else {
                // --- physics off: keep the lightweight kinematic ctrl->motion mapping ---
                const float steer = (ctrl[0] ? 1.f : 0.f) - (ctrl[1] ? 1.f : 0.f);
                a.yaw += steer * 2.4f * in.dt;
                float tgt = a.speed * (static_cast<float>(ctrl[4]) / 255.0f);
                if (ctrl[5]) tgt = 0.f;
                if (a.slow > 0.f) tgt *= 0.35f;
                a.cur_speed += (tgt - a.cur_speed) * (2.5f * in.dt > 1.f ? 1.f : 2.5f * in.dt);
                const float nx2 = a.pos[0] + std::cos(a.yaw) * a.cur_speed * in.dt;
                const float nz2 = a.pos[2] + std::sin(a.yaw) * a.cur_speed * in.dt;
                bool aok = false;
                const float ay = GroundHeight(nx2, nz2, &aok);
                if (aok) { a.pos[0] = nx2; a.pos[2] = nz2; a.pos[1] = ay + car_ground_off_; }
            }
        }
        // G3 one-shot diagnostic: confirm the .AI loaded + the controller is producing
        // descriptor bytes for each opponent (env MASHED_AI_DIAG -> mashed_re.log).
        {
            static const bool s_aidiag = (std::getenv("MASHED_AI_DIAG") != nullptr);
            static int s_aidn = 0;
            if (s_aidiag && s_aidn < 40) {
                if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                    std::fprintf(lf, "AI-DIAG splineCnt=%d", Ai::I32(Ai::kSplineRaceCnt));
                    for (int ci = 0; ci < static_cast<int>(ai_cars_.size()) && ci < 3; ++ci) {
                        const int v = ci + 1;
                        const int slot = Ai::I32(Ai::kSlotTableBase + (std::uintptr_t)v * Ai::kSlotTableStride);
                        const std::uint8_t* c = reinterpret_cast<const std::uint8_t*>(
                            Ai::kCtrlBlockBase + (std::uintptr_t)slot * Ai::kCtrlBlockStride);
                        const AiCar& a = ai_cars_[(std::size_t)ci];
                        std::fprintf(lf, " | v%d ctrl[%d,%d,%d,%d] spd=%.1f pos=(%.0f,%.0f)",
                                     v, c[0], c[1], c[4], c[5], a.cur_speed, a.pos[0], a.pos[2]);
                    }
                    std::fprintf(lf, "\n");
                    std::fclose(lf);
                }
                ++s_aidn;
            }
        }
    } else {
    // AI v2: follow the gate ribbon with a lateral lane offset, braking for
    // sharp upcoming corners, velocity-shaped speed (no teleport).
    for (int ci = 0; ci < static_cast<int>(ai_cars_.size()); ++ci) {
        AiCar& a = ai_cars_[static_cast<std::size_t>(ci)];
        if (ng < 2) break;
        const RaceCar& rc = race_[ci + 1];
        if (round_mode_ && !rc.alive) { a.cur_speed = 0.f; continue; }
        // power-up effect: spun out by a missile/mine -> can't drive, spins in
        // place until the timer elapses.
        if (a.spin > 0.f) {
            a.spin -= in.dt;
            a.yaw += 12.0f * in.dt;
            a.cur_speed = 0.f;
            continue;
        }
        if (a.slow > 0.f) a.slow -= in.dt;     // shocked: speed capped below
        const float* g  = gates_[static_cast<std::size_t>(a.target) % ng].center;
        const float* g2 = gates_[static_cast<std::size_t>(a.target + 1) % ng].center;
        // lateral aim = gate center + lane offset along the gate's local right
        float tdx = g2[0] - g[0], tdz = g2[2] - g[2];
        const float tl = std::sqrt(tdx * tdx + tdz * tdz);
        if (tl > 1e-3f) { tdx /= tl; tdz /= tl; }
        const float aimx = g[0] + (-tdz) * a.lane;
        const float aimz = g[2] + ( tdx) * a.lane;
        const float dx = aimx - a.pos[0], dz = aimz - a.pos[2];
        const float dist = std::sqrt(dx * dx + dz * dz);
        if (dist < 3.0f) {
            a.target = (a.target + 1) % ng;
        }
        // corner sharpness ahead: angle between this and next ribbon segment
        const float* g3 = gates_[static_cast<std::size_t>(a.target + 2) % ng].center;
        float adx = g2[0] - g[0], adz = g2[2] - g[2];
        float bdx = g3[0] - g2[0], bdz = g3[2] - g2[2];
        const float al = std::sqrt(adx*adx + adz*adz), bl = std::sqrt(bdx*bdx + bdz*bdz);
        float corner = 1.f;   // 1 = straight, ->0 = hairpin
        if (al > 1e-3f && bl > 1e-3f)
            corner = (adx*bdx + adz*bdz) / (al * bl);
        const float want_yaw = std::atan2(dz, dx);
        float yerr = want_yaw - a.yaw;
        while (yerr >  3.14159f) yerr -= 6.28318f;
        while (yerr < -3.14159f) yerr += 6.28318f;
        a.yaw += yerr * (6.0f * in.dt > 1.f ? 1.f : 6.0f * in.dt);   // turn rate
        // brake into corners: target speed scaled by corner straightness
        const float brake = 0.45f + 0.55f * (corner < 0.f ? 0.f : corner);
        float tgt = a.speed * brake;
        if (a.slow > 0.f) tgt *= 0.35f;        // shock power-up: throttled
        a.cur_speed += (tgt - a.cur_speed) * (2.5f * in.dt > 1.f ? 1.f : 2.5f * in.dt);
        const float nx2 = a.pos[0] + std::cos(a.yaw) * a.cur_speed * in.dt;
        const float nz2 = a.pos[2] + std::sin(a.yaw) * a.cur_speed * in.dt;
        bool aok = false;
        const float ay = GroundHeight(nx2, nz2, &aok);
        if (aok) {
            a.pos[0] = nx2; a.pos[2] = nz2;
            a.pos[1] = ay + car_ground_off_;
        } else {
            a.target = (a.target + 1) % ng;   // edge: skip to next gate
        }
    }
    }

    // visual wheels: spin by distance/radius, steer toward the input
    const float wr = wheels_.empty() ? 0.3f : wheels_[0].radius;
    wheel_spin_ -= (car_speed_ / (wr > 0.05f ? wr : 0.3f)) * in.dt;
    if (wheel_spin_ >  6.2831853f) wheel_spin_ -= 6.2831853f;
    if (wheel_spin_ < -6.2831853f) wheel_spin_ += 6.2831853f;
    const float steer_target = in.steer * 0.45f;
    const float k = (10.f * in.dt > 1.f) ? 1.f : 10.f * in.dt;
    steer_vis_ += (steer_target - steer_vis_) * k;

    // power-up pickups: collect orbs the player drives through (respawn loop).
    pickups_.Update(in.dt, car_pos_);
    // power-up EFFECTS: advance boost/shield timers, missiles, mines.
    UpdatePowerups(in.dt);
}

// slot 0 = player, 1..3 = ai_cars_[slot-1]. Spins the car out (brief loss of
// control) unless it's the player and a shield is up (which absorbs the hit).
void TrackRenderer::SpinOut(int slot) {
    if (slot == 0) {
        if (shield_timer_ > 0.f) { shield_timer_ = 0.f; return; }   // absorbed
        car_speed_ = 0.f;
        car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
    } else {
        const int idx = slot - 1;
        if (idx >= 0 && idx < static_cast<int>(ai_cars_.size()))
            ai_cars_[static_cast<std::size_t>(idx)].spin = 1.6f;   // seconds
    }
    const float* p = (slot == 0) ? car_pos_
                                  : ai_cars_[static_cast<std::size_t>(slot-1)].pos;
    parts_.SpawnBurst(p, 36, 0xffffd060u, track_radius_ * 0.10f,
                      track_radius_ * 0.018f, 0.8f);
    Audio::SfxPlay("explosion1", 0.85f);   // real spin-out explosion SFX
}

// ---- WS-D3: drive the ported power-up dispatch (Powerup/PowerupSystem) -------
// The invented boost/shield/missile/mine/shock ApplyPowerup switch is replaced by
// PowerupBackendImpl, which realises each ported per-type effect's LEAF on the
// existing in-race visuals (missiles_/mines_/ai_cars_/parts_). This is the
// WS-B/WS-E subsystem stand-in (the original effects attach RW models + run the
// contact system); the DISPATCH + per-type decision logic is the verbatim port.
class PowerupBackendImpl : public Powerup::IPowerupBackend {
    TrackRenderer* t_;
    float R() const { return t_->track_radius_ > 1.f ? t_->track_radius_ : t_->radius_; }
public:
    explicit PowerupBackendImpl(TrackRenderer* t) : t_(t) {}

    const Powerup::HostCar& Player() const override { return t_->pu_player_; }
    int  AiCount() const override { return static_cast<int>(t_->pu_ai_.size()); }
    Powerup::HostCar& Ai(int i) override { return t_->pu_ai_[static_cast<std::size_t>(i)]; }

    // MISSILE FIRE leaf (orig FUN_00455150: projectile pool + RwFrameAddChild).
    void SpawnMissile(const float pos[3], const float vel[3], int target) override {
        const float r = R();
        TrackRenderer::Missile mi;
        mi.pos[0]=pos[0]; mi.pos[1]=pos[1]+r*0.01f; mi.pos[2]=pos[2];
        mi.vel[0]=vel[0]*r; mi.vel[1]=vel[1]*r; mi.vel[2]=vel[2]*r;
        mi.target = (target >= 0) ? target : t_->MissileTargetAhead();
        mi.life = 4.0f;
        t_->missiles_.push_back(mi);
    }
    // MORTAR FIRE leaf (orig FUN_004533b0: lobbed pool DAT_00684ea8).
    void SpawnMortar(const float pos[3], const float vel[3], int /*target*/) override {
        const float r = R();
        TrackRenderer::Missile mi;
        mi.pos[0]=pos[0]; mi.pos[1]=pos[1]+r*0.02f; mi.pos[2]=pos[2];
        mi.vel[0]=vel[0]*r; mi.vel[1]=vel[1]*r*0.3f; mi.vel[2]=vel[2]*r;
        mi.target = -1;                       // arcs forward, no homing
        mi.life = 3.0f;
        t_->missiles_.push_back(mi);
    }
    // DRUM/P_MINE FIRE leaf (orig FUN_00454740 / FUN_00457ef0: dropped hazard).
    void DropHazard(const float pos[3], bool /*proximity*/) override {
        const float r = R();
        TrackRenderer::Mine m;
        m.pos[0]=pos[0]-std::cos(t_->car_yaw_)*r*0.04f;
        m.pos[1]=pos[1];
        m.pos[2]=pos[2]-std::sin(t_->car_yaw_)*r*0.04f;
        m.life = 20.0f;
        t_->mines_.push_back(m);
    }
    // GUN FIRE leaf (orig FUN_004561c0: forward hitscan tracer).
    void HitscanForward(const float pos[3], const float fwd[3]) override {
        const float r = R();
        const int tgt = t_->MissileTargetAhead();
        if (tgt >= 0) {
            const TrackRenderer::AiCar& a = t_->ai_cars_[static_cast<std::size_t>(tgt)];
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.8f)*(r*0.8f)) t_->SpinOut(tgt+1);
        }
        float sp[3] = { pos[0]+fwd[0]*r*0.1f, pos[1]+r*0.01f, pos[2]+fwd[2]*r*0.1f };
        t_->parts_.SpawnBurst(sp, 12, 0xffffe080u, r*0.05f, r*0.008f, 0.3f);
    }
    // SHOTGUN FIRE leaf (orig FUN_0045b6e0 -> FUN_0045b390: short spread cone).
    void SpreadCone(const float pos[3], const float fwd[3]) override {
        const float r = R();
        for (auto& a : t_->ai_cars_) {
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.4f)*(r*0.4f) && (dx*fwd[0]+dz*fwd[2]) > 0.f) a.slow = 2.5f;
        }
        float sp[3] = {pos[0], pos[1]+r*0.02f, pos[2]};
        t_->parts_.SpawnBurst(sp, 40, 0xffffe040u, r*0.25f, r*0.012f, 0.5f);
    }
    // R_FLAME jet leaf (orig FUN_0045a850: continuous forward burner).
    void FlameJet(const float pos[3], const float fwd[3], bool on) override {
        if (!on) return;
        const float r = R();
        for (auto& a : t_->ai_cars_) {
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.25f)*(r*0.25f) && (dx*fwd[0]+dz*fwd[2]) > 0.f) a.slow = 1.5f;
        }
        float sp[3] = {pos[0]+fwd[0]*r*0.08f, pos[1]+r*0.015f, pos[2]+fwd[2]*r*0.08f};
        t_->parts_.SpawnBurst(sp, 18, 0xffff8020u, r*0.10f, r*0.010f, 0.4f);
    }
    // FLASH FIRE leaf (orig FUN_00454db0: blind/screen flash).
    void BlindFlash(const float pos[3]) override {
        const float r = R();
        for (auto& a : t_->ai_cars_) {
            const float dx=a.pos[0]-pos[0], dz=a.pos[2]-pos[2];
            if (dx*dx+dz*dz < (r*0.6f)*(r*0.6f)) a.slow = 2.0f;
        }
        float sp[3] = {pos[0], pos[1]+r*0.03f, pos[2]};
        t_->parts_.SpawnBurst(sp, 60, 0xffffffffu, r*0.35f, r*0.010f, 0.5f);
    }
    // OIL FIRE leaf (orig FUN_00457800: ground slick).
    void DropOilSlick(const float pos[3]) override {
        TrackRenderer::Mine m;
        m.pos[0]=pos[0]; m.pos[1]=pos[1]; m.pos[2]=pos[2];
        m.life = 25.0f;
        t_->mines_.push_back(m);
    }
    void SfxByName(const char* name, float vol) override { Audio::SfxPlay(name, vol); }
    // OIL distance gate (orig: |pos-lastDrop|^2 >= _DAT_005cc56c, trail per owner).
    bool OilDropDue(int /*owner*/, const float pos[3]) override {
        const float r = R();
        if (!t_->pu_oil_has_) {
            t_->pu_oil_has_ = true;
            std::memcpy(t_->pu_oil_last_, pos, sizeof(float)*3);
            return true;
        }
        const float dx=pos[0]-t_->pu_oil_last_[0], dz=pos[2]-t_->pu_oil_last_[2];
        if (dx*dx+dz*dz >= (r*0.06f)*(r*0.06f)) {
            std::memcpy(t_->pu_oil_last_, pos, sizeof(float)*3);
            return true;
        }
        return false;
    }
};

// nearest AI car ahead of the player (replicates the old missile targeting).
int TrackRenderer::MissileTargetAhead() const {
    int best = -1; float bestd = 1e30f;
    const float fx = std::cos(car_yaw_), fz = std::sin(car_yaw_);
    for (int i = 0; i < static_cast<int>(ai_cars_.size()); ++i) {
        const AiCar& a = ai_cars_[static_cast<std::size_t>(i)];
        if (round_mode_ && !race_[i+1].alive) continue;
        const float dx = a.pos[0]-car_pos_[0], dz = a.pos[2]-car_pos_[2];
        if (dx*fx + dz*fz <= 0.f) continue;       // must be ahead
        const float d = dx*dx + dz*dz;
        if (d < bestd) { bestd = d; best = i; }
    }
    return best;
}

void TrackRenderer::EnsurePowerupBackend() {
    if (!pu_be_) { pu_be_ = new PowerupBackendImpl(this); pw_.Init(pu_be_); }
}

void TrackRenderer::SyncHostCar() {
    pu_player_.pos[0]=car_pos_[0]; pu_player_.pos[1]=car_pos_[1]; pu_player_.pos[2]=car_pos_[2];
    pu_player_.fwd[0]=std::cos(car_yaw_); pu_player_.fwd[1]=0.f; pu_player_.fwd[2]=std::sin(car_yaw_);
    pu_player_.vel[0]=car_vel_[0]; pu_player_.vel[1]=car_vel_[1]; pu_player_.vel[2]=car_vel_[2];
    pu_player_.yaw=car_yaw_; pu_player_.owner=0; pu_player_.alive=true;
    pu_ai_.resize(ai_cars_.size());
    for (std::size_t i=0;i<ai_cars_.size();++i) {
        const AiCar& a = ai_cars_[i];
        pu_ai_[i].pos[0]=a.pos[0]; pu_ai_[i].pos[1]=a.pos[1]; pu_ai_[i].pos[2]=a.pos[2];
        pu_ai_[i].fwd[0]=std::cos(a.yaw); pu_ai_[i].fwd[1]=0.f; pu_ai_[i].fwd[2]=std::sin(a.yaw);
        pu_ai_[i].yaw=a.yaw; pu_ai_[i].owner=static_cast<int>(i)+1;
        pu_ai_[i].alive = !(round_mode_ && !race_[i+1].alive);
    }
}

// Drive the ported dispatch for a single use of the held type. The original is a
// per-frame fire-button loop; the standalone fires one pickup per key press, so
// we pulse primary (mode 2) then secondary (mode 1) so whichever button the
// type's FIRE gates on triggers, then deactivate (one use per collected pickup).
void TrackRenderer::PowerupFireOnce(int code) {
    EnsurePowerupBackend();
    SyncHostCar();
    pw_.Activate(code);
    pw_.SetFireButtons(true, false);  pw_.Tick(0.016f, pu_player_, 6);   // primary
    if (pw_.Armed()) { pw_.SetFireButtons(false, true); pw_.Tick(0.016f, pu_player_, 6); }  // secondary
    pw_.SetFireButtons(false, false);
    pw_.Deactivate();
}

bool TrackRenderer::FireHeldPowerup() {
    if (!car_ready_) return false;
    if (pickups_.held() < 0) return false;
    int code = pickups_.held_type();          // real MASHED code, or -1 (index orb)
    pickups_.ConsumeHeld();                    // clear the HUD held slot
    if (code < 0) code = Powerup::kMissile;    // index-only orb -> a default weapon
    PowerupFireOnce(code);
    return true;
}

void TrackRenderer::FirePowerupKind(int code) {
    if (!car_ready_) return;
    // demo/testing: small indices (0..6) map to representative real type codes;
    // the 9 real codes (7,9,10,11,12,16,17,18,19) pass straight through.
    int real = code;
    if (code >= 0 && code < 7) {
        static const int demo[7] = { Powerup::kMissile, Powerup::kPMine,
                                     Powerup::kShotgun,  Powerup::kGun,
                                     Powerup::kFlash,    Powerup::kDrum, Powerup::kOil };
        real = demo[code];
    }
    PowerupFireOnce(real);
}

void TrackRenderer::UpdatePowerups(float dt) {
    if (dt <= 0.f || dt > 0.25f) dt = 0.016f;
    const float R = track_radius_ > 1.f ? track_radius_ : radius_;
    if (boost_timer_  > 0.f) boost_timer_  -= dt;
    if (shield_timer_ > 0.f) shield_timer_ -= dt;

    // missiles: home toward their target AI car, trail particles, hit on contact
    const float hitR = R * 0.05f;
    for (size_t i = 0; i < missiles_.size();) {
        Missile& m = missiles_[i];
        m.life -= dt;
        bool dead = (m.life <= 0.f);
        if (m.target >= 0 && m.target < static_cast<int>(ai_cars_.size())) {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(m.target)];
            float to[3] = {a.pos[0]-m.pos[0], a.pos[1]-m.pos[1], a.pos[2]-m.pos[2]};
            const float d = std::sqrt(to[0]*to[0]+to[1]*to[1]+to[2]*to[2]);
            if (d < hitR) { SpinOut(m.target + 1); dead = true; }
            else if (d > 1e-3f) {                        // steer velocity toward target
                const float spd = R * 0.7f;
                for (int k = 0; k < 3; ++k) m.vel[k] += (to[k]/d*spd - m.vel[k]) * 4.f * dt;
            }
        }
        m.pos[0] += m.vel[0]*dt; m.pos[1] += m.vel[1]*dt; m.pos[2] += m.vel[2]*dt;
        parts_.SpawnTrail(m.pos, 0xffff8020u, R*0.012f, 0.4f);   // orange comet
        if (dead) { missiles_.erase(missiles_.begin()+i); }
        else ++i;
    }

    // mines: armed hazard; spin out any car (player or AI) that contacts it.
    for (size_t i = 0; i < mines_.size();) {
        Mine& m = mines_[i];
        m.life -= dt;
        parts_.SpawnTrail(m.pos, 0xffff4030u, R*0.010f, 0.5f);   // red pulse
        bool det = false;
        {
            const float dx = car_pos_[0]-m.pos[0], dz = car_pos_[2]-m.pos[2];
            if (dx*dx+dz*dz < hitR*hitR) { SpinOut(0); det = true; }
        }
        for (int c = 0; c < static_cast<int>(ai_cars_.size()) && !det; ++c) {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(c)];
            const float dx = a.pos[0]-m.pos[0], dz = a.pos[2]-m.pos[2];
            if (dx*dx+dz*dz < hitR*hitR) { SpinOut(c+1); det = true; }
        }
        if (det || m.life <= 0.f) {
            if (det) parts_.SpawnBurst(m.pos, 30, 0xffff6020u, R*0.12f, R*0.016f, 0.7f);
            mines_.erase(mines_.begin()+i);
        } else ++i;
    }
}

void TrackRenderer::StartRound() {
    if (gates_.size() < 4 || !car_ready_) return;
    round_mode_ = true;
    round_alive_ = kRaceCars;
    round_winner_ = -1;
    for (int i = 0; i < kRaceCars; ++i) race_[i] = RaceCar{};
    // F4: reset the race clock + the player's per-lap split records.
    race_time_ = 0.f;
    for (int k = 0; k < kMaxSplits; ++k) { split_time_[k] = 0.f; split_done_[k] = false; }
    // grid: 2x2 behind gate 0, offset along the start line's lateral axis
    const float* g0 = gates_[0].center;
    const float* g1 = gates_[1].center;
    float dir[2] = {g1[0] - g0[0], g1[2] - g0[2]};
    const float dl = std::sqrt(dir[0]*dir[0] + dir[1]*dir[1]);
    dir[0] /= dl; dir[1] /= dl;
    const float lat[2] = {-dir[1], dir[0]};
    auto place = [&](float* pos, float* yaw, int row, int col) {
        const float side = (col == 0) ? -0.9f : 0.9f;
        const float back = 1.2f + 1.8f * static_cast<float>(row);
        const float x = g0[0] - dir[0] * back + lat[0] * side;
        const float z = g0[2] - dir[1] * back + lat[1] * side;
        bool ok = false;
        const float y = GroundHeight(x, z, &ok);
        pos[0] = x; pos[2] = z;
        pos[1] = (ok ? y : g0[1]) + car_ground_off_;
        *yaw = std::atan2(dir[1], dir[0]);
    };
    place(car_pos_, &car_yaw_, 0, 0);
    car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
    car_speed_ = 0.f;
    ai_cars_.assign(3, AiCar{});
    for (int i = 0; i < 3; ++i) {
        AiCar& a = ai_cars_[static_cast<std::size_t>(i)];
        place(a.pos, &a.yaw, (i + 1) / 2, (i + 1) % 2);
        a.target = 1;
        // staggered top speeds (all faster than the auto-driven player) +
        // distinct racing lanes so they spread out and overtake
        a.speed = radius_ * (0.20f + 0.03f * static_cast<float>(i));
        a.cur_speed = 0.f;
        a.lane = (static_cast<float>(i) - 1.0f) * 3.0f;   // -3, 0, +3
    }
    countdown_ = 3.0f;   // 3-2-1 freeze before GO
    Audio::SfxPlay("threetwoone", 0.9f);   // real countdown SFX (permdict)
    race_cam_.Reset();   // force_reset path on the first camera tick
    // clear any in-flight power-up effects from the previous round
    missiles_.clear();
    mines_.clear();
    boost_timer_ = shield_timer_ = 0.f;
}

void TrackRenderer::StartMatch(int /*first_to: superseded by the ported
                                 score rules — match win at score > 11
                                 (0x00410510)*/) {
    match_winner_ = -1;
    round_no_ = 1;
    for (int i = 0; i < kRaceCars; ++i) {
        scores_[i] = score_prev_[i] = score_delta_[i] = 0;
        delta_timer_[i] = 0.f;
        elim_order_[i] = -1;
    }
    elim_count_ = 0;
    StartRound();
}

void TrackRenderer::InitPickups() {
    const float R = track_radius_ > 1.f ? track_radius_ : radius_;
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "R4 pickups: %zu real powerup spawns (POWERUPS_GOLD.LUA)\n",
                     powerup_spawns_.size());
        std::fclose(lf);
    }
    if (!powerup_spawns_.empty()) {
        pickups_.InitReal(powerup_spawns_, R);    // REAL POWERUPS_GOLD.LUA placement
        return;
    }
    std::vector<std::array<float, 3>> spots;       // fallback: gate-stride orbs
    spots.reserve(gates_.size());
    for (const auto& g : gates_)
        spots.push_back({g.center[0], g.center[1], g.center[2]});
    pickups_.Init(spots, R);
}

// 0x0040b290 (standard path, mode 0 / no teams): prev snapshot, signed delta
// display + 6000 ms timer, score += delta, floor at 0. (Modes 1/2 gating and
// the replay event ring are not ported — standalone round is mode 0.)
void TrackRenderer::ScoreAward(int car, int delta) {
    if (car < 0 || car >= kRaceCars) return;
    score_prev_[car] = scores_[car];        // DAT_008a9570
    score_delta_[car] = delta;              // DAT_008a9520 (signed display)
    scores_[car] += delta;                  // DAT_008a94e0
    delta_timer_[car] = 6000.f;             // DAT_008a9510
    if (scores_[car] < 0) scores_[car] = 0;
}

// 0x0040eee0, DAT_008a94d0 == 4 path (4 participants, mode 0, no teams),
// delta = 1; called AFTER the victim's alive flag drops (matching the
// original's FUN_00422fd0-then-callback order). AI auto-resolve branches
// (FUN_0040e470 type-2 fast-forward) are not ported — the player is always
// in our round.
void TrackRenderer::ScoreOnElimination(int victim) {
    if (elim_count_ < kRaceCars) elim_order_[elim_count_++] = victim;  // DAT_008a94c0
    const int remaining = round_alive_;
    if (remaining == 3) {
        ScoreAward(victim, -2);             // param_2 * -2
    } else if (remaining == 2) {
        ScoreAward(victim, -1);             // -param_2
    } else if (remaining == 1) {
        int survivor = -1;
        for (int i = 0; i < kRaceCars; ++i)
            if (race_[i].alive) survivor = i;
        // runner-up +1, zeroed when its score already exceeds 10
        ScoreAward(victim, (scores_[victim] > 10) ? 0 : 1);   // FUN_0040b6d0 cap
        ScoreAward(survivor, 2);
    }
}

void TrackRenderer::NextRoundOrEnd() {
    if (round_winner_ < 0) return;
    // Race::EvaluateResult (0x00410510): with 4 participants the match ends
    // when an alive player's score exceeds 11.
    for (int i = 0; i < kRaceCars; ++i)
        if (scores_[i] > 11) { match_winner_ = i; return; }
    ++round_no_;
    StartRound();   // re-grid for the next round
}

void TrackRenderer::UpdateRace(float dt) {
    if (gates_.empty()) return;
    const int n = static_cast<int>(gates_.size());
    if (countdown_ <= 0.f) race_time_ += dt;   // F4 race clock (seconds since GO)
    auto step = [&](int i, const float* pos) {
        RaceCar& r = race_[i];
        if (!r.alive) return;
        const float* g = gates_[static_cast<std::size_t>(r.gate) %
                                gates_.size()].center;
        const float dx = g[0] - pos[0], dz = g[2] - pos[2];
        const float d = std::sqrt(dx * dx + dz * dz);
        if (d < 3.0f) {
            const int G = r.gate;            // reached this gate
            bool lapped = false;
            // F4 (algorithm-faithful port of FUN_00408610 + FUN_00411600; map in
            // re/analysis/lap_progress_subsystem_2026-06-16.md). The original
            // flags a LAPDATA Lap_Line gate only on a forward one-gate advance
            // onto it (cur==prev+1 && cur==lapLine via FUN_00426cf0's list) and a
            // lap is the declared lap-line set crossed in order — the multi-
            // Lap_Line anti-shortcut. The standalone advances gates forward by
            // proximity, so each reach IS that forward advance; we keep the
            // per-car crossed set in r.lap_mask and complete a lap on the primary
            // line once every line is set. NOT bit-identical: the original's gate
            // index comes from a spline racing-line projection we don't port.
            if (Track::LapLineStep(G, lap_data_.lap_lines, r.lap_mask)) {
                ++r.laps; lapped = true;
            }
            // F4 split times (player car only): record the race clock at each
            // Split_Sector gate, one record per split per lap (FUN_00411600's
            // intermediate timing / DAT_008a964c). Re-armed on lap completion.
            if (i == 0) {
                for (const auto& s : lap_data_.split_sectors)
                    if (G == s.second && s.first >= 0 && s.first < kMaxSplits &&
                        !split_done_[s.first]) {
                        split_time_[s.first] = race_time_;
                        split_done_[s.first] = true;
                    }
                if (lapped) {
                    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                        std::fprintf(lf, "F4 player lap %d @ t=%.2fs splits:",
                                     r.laps, race_time_);
                        for (int k = 0; k < kMaxSplits; ++k)
                            if (split_done_[k])
                                std::fprintf(lf, " s%d=%.2f", k, split_time_[k]);
                        std::fprintf(lf, "\n");
                        std::fclose(lf);
                    }
                    for (int k = 0; k < kMaxSplits; ++k) split_done_[k] = false;
                }
            }
            r.gate = (G + 1) % n;
        }
        const float frac = (d > 8.f) ? 0.f : (1.f - d / 8.f);
        r.progress = static_cast<float>(r.laps * n + r.gate) + frac;
    };
    step(0, car_pos_);
    for (int i = 0; i < 3 && i < static_cast<int>(ai_cars_.size()); ++i)
        step(i + 1, ai_cars_[static_cast<std::size_t>(i)].pos);

    if (!round_mode_) return;   // race rules only run during a match (both modes
                                // update the shared camera below).

    // ---- VERBATIM-PORTED camera + elimination (replaces the ">7 gates"
    // scaffold). Adapters: standalone race state -> RaceCamCar quantities
    // the original reads through getters (see Race/RaceCamera.h).
    Race::RaceCamCar cc[kRaceCars] = {};
    const float* P2[kRaceCars] = {car_pos_, nullptr, nullptr, nullptr};
    for (int i = 0; i < 3 && i < static_cast<int>(ai_cars_.size()); ++i)
        P2[i + 1] = ai_cars_[static_cast<std::size_t>(i)].pos;
    for (int i = 0; i < kRaceCars; ++i) {
        Race::RaceCamCar& c = cc[i];
        c.active = (P2[i] != nullptr);
        c.alive = race_[i].alive && c.active;
        if (!c.active) continue;
        for (int k = 0; k < 3; ++k) c.pos[k] = P2[i][k];
        if (i == 0) {
            for (int k = 0; k < 3; ++k) c.vel[k] = car_vel_[k];
        } else {
            const AiCar& a = ai_cars_[static_cast<std::size_t>(i - 1)];
            c.vel[0] = std::cos(a.yaw) * a.cur_speed;
            c.vel[2] = std::sin(a.yaw) * a.cur_speed;
        }
        // dead_flag/dead_ms: the scaffold has no dying state (cars vanish on
        // elimination); the original's explosion window is not modeled yet.
        const float prog_in_lap =
            std::fmod(race_[i].progress, static_cast<float>(n));
        c.path_prog = prog_in_lap;                       // 0x00408a50 equiv
        c.race_pct = prog_in_lap / static_cast<float>(n) * 100.f;  // 0x00408ad0
    }
    cam_ticks_ += static_cast<double>(dt) * 3000000.0;   // DAT_007f1030 rate
    race_cam_.Update(cc, course_id_, 1.f / 60.f /*DAT_007f100c live*/,
                     static_cast<std::uint32_t>(cam_ticks_),
                     0.f /*DAT_007f0fc8 live*/, false);

    for (int i = 0; i < kRaceCars; ++i)        // DAT_008a9510 decay
        if (delta_timer_[i] > 0.f) delta_timer_[i] -= dt * 1000.f;

    // Laps mode: no elimination — first car to complete lap_target_ laps wins.
    if (race_mode_ == 1) {
        if (match_winner_ < 0 && countdown_ <= 0.f)
            for (int i = 0; i < kRaceCars; ++i)
                if (race_[i].laps >= lap_target_) { match_winner_ = i; break; }
        return;
    }
    if (round_winner_ >= 0) return;            // elimination round-end hold

    if (countdown_ <= 0.f) {
        const int victim = race_cam_.EliminationCheck(cc);
        if (victim >= 0 && race_[victim].alive) {
            race_[victim].alive = false;
            --round_alive_;
            if (victim > 0 && victim - 1 < static_cast<int>(ai_cars_.size()))
                ai_cars_[static_cast<std::size_t>(victim - 1)].speed = 0.f;
            ScoreOnElimination(victim);        // FUN_0040eee0(victim, 1)
        }
    }
    if (round_alive_ <= 1) {
        for (int i = 0; i < kRaceCars; ++i)
            if (race_[i].alive) { round_winner_ = i; break; }
    }
}

void TrackRenderer::camera(float eye[3], float at[3]) const {
    for (int i = 0; i < 3; ++i) { eye[i] = last_eye_[i]; at[i] = last_at_[i]; }
}

void TrackRenderer::Render(IDirect3DDevice9* dev, float t, const CamInput* in) {
    if (!ready_) return;
    // F3 A/B verification toggle: suppress all UV-scroll texture transforms.
    static const bool s_uvscroll_off = std::getenv("MASHED_NO_UVSCROLL") != nullptr;
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
    } else if (round_mode_ && car_ready_) {
        // VERBATIM-PORTED shared race camera (Race/RaceCamera.cpp, from
        // 0x00446520) — updated in UpdateRace(); consumed here. Replaces
        // the invented centroid camera.
        const float* cp = race_cam_.pos();
        const float* ct = race_cam_.target();
        for (int i = 0; i < 3; ++i) { eye[i] = cp[i]; at[i] = ct[i]; }
    } else if (car_ready_) {
        // chase cam: behind and above the car, looking at it
        const float back = 7.0f, up = 3.0f;
        eye[0] = car_pos_[0] - std::cos(car_yaw_) * back;
        eye[1] = car_pos_[1] + up;
        eye[2] = car_pos_[2] - std::sin(car_yaw_) * back;
        at[0] = car_pos_[0]; at[1] = car_pos_[1] + 0.8f; at[2] = car_pos_[2];
    } else {
        // Auto-orbit around the racing-line focus (gate centroid/extent), not
        // the bbox midpoint — keeps the track framed and at the surface Y. Eye
        // raised to ~1.1x extent for a clear, slightly-overhead vantage.
        const float* C = track_center_;
        const float  R = track_radius_;
        const float yaw = t * 0.3f;
        eye[0] = C[0] + R * 1.60f * std::cos(yaw);
        eye[1] = C[1] + R * 1.10f;
        eye[2] = C[2] + R * 1.60f * std::sin(yaw);
        at[0] = C[0]; at[1] = C[1]; at[2] = C[2];
    }
    for (int i = 0; i < 3; ++i) { last_eye_[i] = eye[i]; last_at_[i] = at[i]; }

    D3DMATRIX viewm, projm, worldm;
    MatLookAtLH(&viewm, eye, at);
    // round mode: FOV from the camera struct's view-window 0.6 (cam[0x16],
    // Camera::Apply 0x00441760/FUN_00441700): fovy = 2*atan(vw * h/w).
    const float fovy = (round_mode_ && car_ready_ && !free_)
        ? 2.f * std::atan(race_cam_.view_window() * 600.f / 800.f)
        : 1.0472f /*60 deg*/;
    MatPerspectiveFovLH(&projm, fovy, 800.f / 600.f,
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

    // WS-E1: real RenderWare world render path (B-full), behind MASHED_RW_RENDER.
    // It is INERT in the standalone until the RW engine substrate is bound
    // (RwEngineOpen + BSP->RpWorld loader = E2/B-full), so RwWorldRender_Render()
    // returns 0 and the spike world draw below stays the shipping path. When active
    // it traverses RpWorld sectors -> RpAtomic render callbacks (full world incl.
    // sky/props route through it; the spike world-geometry loop is then skipped).
    const bool rw_world = D3d9Render::RwWorldRender_Render(/*world*/nullptr, /*cam*/nullptr) != 0;

    // sky clump first: unfogged, no depth write (renderer-gap closure)
    if (!sky_.batches.empty()) {
        dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
        dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        for (std::size_t mi = 0; mi < sky_.batches.size(); ++mi) {
            const auto& b = sky_.batches[mi];
            if (b.empty()) continue;
            dev->SetTexture(0, sky_.textures[mi]);
            // F3: scroll the UV-animated sky material (SKY.DFF "bmp_Sky_M").
            const bool scroll = !s_uvscroll_off && mi < sky_.mat_scroll.size() &&
                (sky_.mat_scroll[mi].du != 0.f || sky_.mat_scroll[mi].dv != 0.f);
            if (scroll) {
                D3DMATRIX tm; MatIdentity(&tm);
                tm._31 = std::fmod(sky_.mat_scroll[mi].du * t, 1.f);
                tm._32 = std::fmod(sky_.mat_scroll[mi].dv * t, 1.f);
                dev->SetTransform(D3DTS_TEXTURE0, &tm);
                dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                          D3DTTFF_COUNT2);
            }
            dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                 static_cast<UINT>(b.size() / 3),
                                 b.data(), sizeof(V));
            if (scroll)
                dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                          D3DTTFF_DISABLE);
        }
        dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    }
    // fog (COURSE.LUA Setup_Fog) + alpha cutouts (fence/grate textures).
    // The fog range (Setup_Fog end) is calibrated for the in-game chase camera,
    // which sits a few units behind the car. The orbit/free OVERVIEW cameras sit
    // a full track-radius back (~1.6x extent), well beyond fog_end_, so applying
    // the gameplay fog there washes the entire track to fog_color (the dark
    // sliver we saw). Apply fog only when following the car (chase/race cam);
    // the overview cameras render unfogged so the whole track is visible.
    const bool chase_cam = car_ready_ && !free_;
    if (fog_on_ && chase_cam) {
        dev->SetRenderState(D3DRS_FOGENABLE, TRUE);
        dev->SetRenderState(D3DRS_FOGCOLOR, fog_color_);
        dev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
        DWORD fs, fe;
        std::memcpy(&fs, &fog_start_, 4);
        std::memcpy(&fe, &fog_end_, 4);
        dev->SetRenderState(D3DRS_FOGSTART, fs);
        dev->SetRenderState(D3DRS_FOGEND, fe);
    } else {
        dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    }
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    dev->SetRenderState(D3DRS_ALPHAREF, 0x30);
    dev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    // WS-E1: skip the spike's world-geometry batches when the RW world render path
    // drew the world (the `if` gates the entire for-statement; inert today).
    if (!rw_world)
    for (std::size_t mi = 0; mi < batches_.size(); ++mi) {
        const auto& b = batches_[mi];
        if (b.empty()) continue;
        dev->SetTexture(0, textures_[mi]);
        // F3: scroll the UVs of UV-animated materials (sea/sky) via a texture
        // transform — translation in row 3 with D3DTTFF_COUNT2 (the standard
        // 2D scroll), offset = rate * time wrapped to [0,1).
        const bool scroll = !s_uvscroll_off && uv_anim_ && mi < mat_scroll_.size() &&
                            (mat_scroll_[mi].du != 0.f || mat_scroll_[mi].dv != 0.f);
        if (scroll) {
            D3DMATRIX tm; MatIdentity(&tm);
            tm._31 = std::fmod(mat_scroll_[mi].du * t, 1.f);
            tm._32 = std::fmod(mat_scroll_[mi].dv * t, 1.f);
            dev->SetTransform(D3DTS_TEXTURE0, &tm);
            dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
        }
        if (!textures_[mi])
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                             static_cast<UINT>(b.size() / 3),
                             b.data(), sizeof(V));
        if (!textures_[mi])
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        if (scroll)
            dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
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
                // F3: scroll UV-animated prop materials (Arctic sea/sky clumps)
                // — the .UVA rate bound via the material's UVAnim extension.
                const bool scroll = !s_uvscroll_off && mi < p.mat_scroll.size() &&
                    (p.mat_scroll[mi].du != 0.f || p.mat_scroll[mi].dv != 0.f);
                if (scroll) {
                    D3DMATRIX tm; MatIdentity(&tm);
                    tm._31 = std::fmod(p.mat_scroll[mi].du * t, 1.f);
                    tm._32 = std::fmod(p.mat_scroll[mi].dv * t, 1.f);
                    dev->SetTransform(D3DTS_TEXTURE0, &tm);
                    dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                              D3DTTFF_COUNT2);
                }
                if (!p.textures[mi])
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_SELECTARG2);
                dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                     static_cast<UINT>(b.size() / 3),
                                     b.data(), sizeof(V));
                if (!p.textures[mi])
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_MODULATE);
                if (scroll)
                    dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                              D3DTTFF_DISABLE);
            }
        }
    }

    // F2: animated copters — the model DFF under the per-frame HAnim transform
    // (COURSE.LUA SetCopter / KTCSCRIPT.LUA KTC_NewCopter; flight path = the
    // bound .ANM). Looped on the race clock t (the .ANM durations are seconds).
    // MASHED_NO_COPTERS suppresses the pass (A/B verification toggle).
    static const bool s_copters_off = std::getenv("MASHED_NO_COPTERS") != nullptr;
    if (!s_copters_off)
    for (const auto& c : copters_) {
        if (c.model < 0 || c.model >= static_cast<int>(copter_models_.size()) ||
            c.anim.frames.empty())
            continue;
        float cp[3], cq[4];
        c.anim.Sample(t, cp, cq);
        D3DMATRIX cm;
        QuatPosMatrix(cq, cp, &cm);
        dev->SetTransform(D3DTS_WORLD, &cm);
        const CopterModel& m = copter_models_[c.model];
        for (std::size_t mi = 0; mi < m.batches.size(); ++mi) {
            const auto& b = m.batches[mi];
            if (b.empty()) continue;
            dev->SetTexture(0, m.textures[mi]);
            if (!m.textures[mi])
                dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
            dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                 static_cast<UINT>(b.size() / 3),
                                 b.data(), sizeof(V));
            if (!m.textures[mi])
                dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
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
        // wheels: pivot-relative parts under spin (axle) + steer (front pair)
        for (const auto& w : wheels_) {
            D3DMATRIX local;
            MatIdentity(&local);
            const float c = std::cos(wheel_spin_), s = std::sin(wheel_spin_);
            if (w.lateral_is_x) {           // axle = model X: rotate Y/Z
                local._22 = c; local._23 = s; local._32 = -s; local._33 = c;
            } else {                        // axle = model Z: rotate X/Y
                local._11 = c; local._12 = s; local._21 = -s; local._22 = c;
            }
            if (w.front && steer_vis_ != 0.f) {
                D3DMATRIX steer;
                MatIdentity(&steer);
                const float cs = std::cos(steer_vis_), ss = std::sin(steer_vis_);
                steer._11 = cs; steer._13 = ss; steer._31 = -ss; steer._33 = cs;
                MatMul(&local, local, steer);
            }
            D3DMATRIX tp;
            MatIdentity(&tp);
            tp._41 = w.pivot[0]; tp._42 = w.pivot[1]; tp._43 = w.pivot[2];
            MatMul(&local, local, tp);
            D3DMATRIX wm;
            MatMul(&wm, local, carm);
            dev->SetTransform(D3DTS_WORLD, &wm);
            for (const auto& part : w.parts) {
                const auto& b = part.second;
                if (b.empty()) continue;
                IDirect3DTexture9* tex = car_textures_[part.first];
                dev->SetTexture(0, tex);
                if (!tex)
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_SELECTARG2);
                dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                     static_cast<UINT>(b.size() / 3),
                                     b.data(), sizeof(V));
                if (!tex)
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_MODULATE);
            }
        }
        // AI cars: per-car LIVERY variant (PORTED selection, FUN_0040d110 —
        // livery = player index) when loaded; else the shared body batches.
        for (const auto& a : ai_cars_) {
            const std::size_t li = static_cast<std::size_t>(&a - ai_cars_.data());
            const CarVariant* var =
                (li < car_variants_.size() &&
                 !car_variants_[li].batches.empty()) ? &car_variants_[li]
                                                     : nullptr;
            D3DMATRIX am;
            MatIdentity(&am);
            const float ac = std::cos(a.yaw), as = std::sin(a.yaw);
            am._11 =  ac; am._13 = as;
            am._31 = -as; am._33 = ac;
            am._41 = a.pos[0]; am._42 = a.pos[1]; am._43 = a.pos[2];
            dev->SetTransform(D3DTS_WORLD, &am);
            const auto& bats = var ? var->batches : car_batches_;
            const auto& texs = var ? var->textures : car_textures_;
            for (std::size_t mi = 0; mi < bats.size(); ++mi) {
                const auto& b = bats[mi];
                if (b.empty()) continue;
                dev->SetTexture(0, texs[mi]);
                if (!texs[mi])
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_SELECTARG2);
                dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                     static_cast<UINT>(b.size() / 3),
                                     b.data(), sizeof(V));
                if (!texs[mi])
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_MODULATE);
            }
            if (var) continue;   // variant models carry their wheels baked in
            // wheels (unsteered, shared spin)
            for (const auto& w : wheels_) {
                D3DMATRIX local;
                MatIdentity(&local);
                const float c2 = std::cos(wheel_spin_),
                            s2 = std::sin(wheel_spin_);
                if (w.lateral_is_x) {
                    local._22 = c2; local._23 = s2;
                    local._32 = -s2; local._33 = c2;
                } else {
                    local._11 = c2; local._12 = s2;
                    local._21 = -s2; local._22 = c2;
                }
                D3DMATRIX tp;
                MatIdentity(&tp);
                tp._41 = w.pivot[0]; tp._42 = w.pivot[1]; tp._43 = w.pivot[2];
                MatMul(&local, local, tp);
                D3DMATRIX wm2;
                MatMul(&wm2, local, am);
                dev->SetTransform(D3DTS_WORLD, &wm2);
                for (const auto& part : w.parts) {
                    const auto& b = part.second;
                    if (b.empty()) continue;
                    dev->SetTexture(0, car_textures_[part.first]);
                    dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,
                                         static_cast<UINT>(b.size() / 3),
                                         b.data(), sizeof(V));
                }
            }
        }
        dev->SetTransform(D3DTS_WORLD, &worldm);
    }

    dev->SetTexture(0, nullptr);

    // In-race particles (weather + car dust): update with the per-frame dt and
    // draw camera-facing billboards. Depth-test ON so geometry occludes them;
    // depth-write OFF so they blend. Only while actually racing (car present).
    if (parts_.ambient() != ParticleSystem::None || car_ready_) {
        const float dt = (last_t_ < 0.f) ? 0.f : (t - last_t_);
        last_t_ = t;
        float fwd[3] = {at[0]-eye[0], at[1]-eye[1], at[2]-eye[2]};
        parts_.Update(dt, eye, fwd, car_ready_ ? car_pos_ : nullptr,
                      car_ready_ ? car_speed_ : 0.f);
        parts_.Render(dev, eye, at);
        // power-up orbs (additive glow billboards) on top of the weather
        if (pickups_.enabled()) pickups_.Render(dev, eye, at);
    }

    dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);  // back to the 2D menu path
}

}  // namespace D3d9Render
}  // namespace mashed_re
