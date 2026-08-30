// Mashed RE — R4 opener track renderer implementation. See TrackRenderer.h.

#include "TrackRenderer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>   // WS-A s3 PERF: static-geometry vertex-buffer cache

#include "../Piz/PizReader.h"
#include "../Track/TrackWorld.h"
#include "../Track/DffModel.h"
#include "../Track/LapLogic.h"      // F4 lap-line crossing sequence (shared w/ test)
#include "../Txd/TxdDecoder.h"
#include "../Audio/AudioEngine.h"   // real SFX (permdict.rws) for countdown/powerups
#include "RwWorldRender.h"          // WS-E1: RW world render path (behind MASHED_RW_RENDER)
#include "../LibRw/RwRaceSubmit.h"  // E2'b step 3: in-loop librw submit (MASHED_RENDER_LIBRW)
#include "DrawStreamDump.h"         // parity harness: MASHED_DBG_DRAWSTREAM3D race-3D summary
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
// Active TrackRenderer for the AI host's LOS query (set each frame in the real_ai
// block). The aib_* host fns are context-free, so the GroundHeight raycast they need
// for the spline lookahead's wall-march reaches the instance through this pointer.
const TrackRenderer* g_aiTrack = nullptr;

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
// LOS clearance for the lookahead wall-march (FUN_00443dc0 Phase 8): march the
// straight segment (ax,az)->(bx,bz) and report blocked (0) if any intermediate
// sample leaves the drivable surface. Backs Ai::Host::los_clear with the track
// collision (GroundHeight) instead of the original's AI tile grid.
int aib_los_clear(float ax, float az, float bx, float bz) {
    if (!g_aiTrack) return 1;
    const float dx = bx - ax, dz = bz - az;
    const float dist = std::sqrt(dx*dx + dz*dz);
    if (dist < 1e-4f) return 1;
    const int n = 10;
    for (int i = 1; i < n; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(n);
        bool ok = false;
        g_aiTrack->GroundHeight(ax + dx*t, az + dz*t, &ok);
        if (!ok) return 0;
    }
    return 1;
}

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
        aib_los_clear,
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

// WS-E s2 — RenderWare atomic lighting (props + cars), baked into vertex diffuse.
// The standalone's track path is unlit fixed-function (D3DRS_LIGHTING FALSE; the
// per-vertex colour modulates the texture), so RW's per-vertex CPU lighting is
// reproduced here at build time instead of via D3D T&L. Model (RW _rpWorldLight /
// atomic instance, matching the asset flags confirmed from the DFFs):
//   * geometry with rpGEOMETRYLIGHT(0x20) + per-vertex NORMALS  ("lit"):
//        colour = prelight(or 0) + ambient + sunColour * max(0, N·L)
//     (surface ambient/diffuse coefficients are 1 for the shipped materials, so
//      they drop out; one directional light is present per track LIGHTS.DFF).
//   * MODULATEMATERIALCOLOR(0x40): the lit colour is then multiplied by the
//        material's own RGBA (car-livery panels, geo flags 0x73).
//   * prelit, non-lit geometry (car glass; no normals): colour = prelight (+the
//        track ambient, matching the world/prop prelit path) — no directional.
// ambient/sun are 0x00RRGGBB (0 = none); sunL is the unit direction TO the light
// (= -lightDir). N·L is one-sided (CULLMODE is NONE so back faces self-shadow,
// matching RW's single-sided lighting of a closed hull).
struct AtomicLight {
    float amb[3]  = {0, 0, 0};   // ambient colour, 0..1
    float sun[3]  = {0, 0, 0};   // directional colour, 0..1
    float L[3]    = {0, 0, 0};   // unit direction TO the light
    bool  has_sun = false;       // a directional light exists
};

inline AtomicLight MakeAtomicLight(D3DCOLOR ambient, D3DCOLOR sun_color,
                                   const float sun_dir[3]) {
    AtomicLight lt;
    lt.amb[0] = ((ambient >> 16) & 0xFF) / 255.f;
    lt.amb[1] = ((ambient >>  8) & 0xFF) / 255.f;
    lt.amb[2] = ( ambient        & 0xFF) / 255.f;
    if (sun_color) {
        lt.sun[0] = ((sun_color >> 16) & 0xFF) / 255.f;
        lt.sun[1] = ((sun_color >>  8) & 0xFF) / 255.f;
        lt.sun[2] = ( sun_color        & 0xFF) / 255.f;
        // L = -lightDir (direction the light travels), then normalise.
        float lx = -sun_dir[0], ly = -sun_dir[1], lz = -sun_dir[2];
        const float ll = std::sqrt(lx*lx + ly*ly + lz*lz);
        if (ll > 1e-6f) { lx/=ll; ly/=ll; lz/=ll; lt.has_sun = true; }
        lt.L[0] = lx; lt.L[1] = ly; lt.L[2] = lz;
    }
    return lt;
}

// Compute one atomic vertex's D3D diffuse. prelit = RGBA dword (RW order) or
// nullptr; n = world-space normal (x,y,z) or nullptr; mat = material RGBA bytes.
inline D3DCOLOR LightAtomicVertex(const AtomicLight& lt, bool lit, bool modmat,
                                  const std::uint8_t mat[4],
                                  const std::uint32_t* prelit, const float* n) {
    std::uint8_t a = 255;
    float r = 0.f, g = 0.f, b = 0.f;
    if (prelit) {
        const std::uint32_t p = *prelit;            // RW byte order: R,G,B,A
        r = ( p        & 0xFF) / 255.f;
        g = ((p >>  8) & 0xFF) / 255.f;
        b = ((p >> 16) & 0xFF) / 255.f;
        a = static_cast<std::uint8_t>(p >> 24);
    }
    if (lit && n) {
        r += lt.amb[0]; g += lt.amb[1]; b += lt.amb[2];
        if (lt.has_sun) {
            float nx = n[0], ny = n[1], nz = n[2];
            const float n2 = nx*nx + ny*ny + nz*nz;
            if (n2 > 1e-12f) { const float inv = 1.f/std::sqrt(n2);
                               nx*=inv; ny*=inv; nz*=inv; }
            float ndl = nx*lt.L[0] + ny*lt.L[1] + nz*lt.L[2];
            if (ndl < 0.f) ndl = 0.f;
            r += lt.sun[0]*ndl; g += lt.sun[1]*ndl; b += lt.sun[2]*ndl;
        }
    } else if (!prelit) {
        r = g = b = 1.f;                            // untextured/unlit fallback
    } else {
        // prelit, non-lit (e.g. glass / sea): keep the track ambient as a fill so
        // the prop sits at the same exposure as the lit geometry. WS-E s4 NOTE:
        // strictly per RW a non-lit prelit atomic gets NO runtime ambient (sea.dff
        // GEOM flags 0x0001000f have no rpGEOMETRYLIGHT 0x20), but zeroing it here
        // darkens the big Arctic sea clump to near-black (whole-frame lum ~24 in
        // the sea-heavy chase view) — further from the original's exposure than
        // keeping the fill. The dominant teal cast lived in the WORLD path (fixed
        // above); fog was measured negligible here, so the fill stays.
        r += lt.amb[0]; g += lt.amb[1]; b += lt.amb[2];
    }
    if (modmat) {
        r *= mat[0]/255.f; g *= mat[1]/255.f; b *= mat[2]/255.f;
    }
    auto cl = [](float f) -> std::uint8_t {
        int v = static_cast<int>(f*255.f + 0.5f);
        return static_cast<std::uint8_t>(v < 0 ? 0 : v > 255 ? 255 : v);
    };
    return D3DCOLOR_ARGB(a, cl(r), cl(g), cl(b));
}

// WS-E vehicle lighting (RpLight subset). Toggle: MASHED_RPLIGHT (default ON;
// =0 reverts to the legacy load-time model-space bake + quantized colours).
bool RpLightEnabled() {
    static const bool v = [] {
        const char* e = std::getenv("MASHED_RPLIGHT");
        return !(e && e[0] == '0' && e[1] == '\0');
    }();
    return v;
}

// Float-precision AtomicLight (RpLight colours are RwRGBAReal floats; the
// legacy MakeAtomicLight quantizes them through D3DCOLOR first). `L` is the
// precomputed unit world direction TO the light (= -lightDir).
inline AtomicLight MakeAtomicLightF(const float amb[3], const float sun[3],
                                    bool has_sun, const float L[3]) {
    AtomicLight lt;
    lt.amb[0] = amb[0]; lt.amb[1] = amb[1]; lt.amb[2] = amb[2];
    lt.sun[0] = sun[0]; lt.sun[1] = sun[1]; lt.sun[2] = sun[2];
    lt.L[0] = L[0]; lt.L[1] = L[1]; lt.L[2] = L[2];
    lt.has_sun = has_sun &&
        (L[0]*L[0] + L[1]*L[1] + L[2]*L[2]) > 0.5f;   // unit or zero
    return lt;
}

// Static part of LightAtomicVertex for the per-frame relit path: base =
// prelight + ambient (pre-clamp floats, x material where MODULATE). The
// directional term is added per frame by RelightBatch. Only meaningful for
// lit-with-normals vertices (callers bake everything else via
// LightAtomicVertex, which already never adds sun to them).
inline void LightAtomicVertexBase(const AtomicLight& lt, bool modmat,
                                  const std::uint8_t mat[4],
                                  const std::uint32_t* prelit,
                                  float out_base[3], std::uint8_t* out_a) {
    float r = 0.f, g = 0.f, b = 0.f;
    std::uint8_t a = 255;
    if (prelit) {
        const std::uint32_t p = *prelit;            // RW byte order: R,G,B,A
        r = ( p        & 0xFF) / 255.f;
        g = ((p >>  8) & 0xFF) / 255.f;
        b = ((p >> 16) & 0xFF) / 255.f;
        a = static_cast<std::uint8_t>(p >> 24);
    }
    r += lt.amb[0]; g += lt.amb[1]; b += lt.amb[2];
    if (modmat) {
        r *= mat[0]/255.f; g *= mat[1]/255.f; b *= mat[2]/255.f;
    }
    out_base[0] = r; out_base[1] = g; out_base[2] = b;
    *out_a = a;
}

inline D3DCOLOR PackClamped(const float rgb[3], std::uint8_t a) {
    auto cl = [](float f) -> std::uint8_t {
        int v = static_cast<int>(f*255.f + 0.5f);
        return static_cast<std::uint8_t>(v < 0 ? 0 : v > 255 ? 255 : v);
    };
    return D3DCOLOR_ARGB(a, cl(rgb[0]), cl(rgb[1]), cl(rgb[2]));
}

// Build one relit vertex source (normalized model-space normal + pre-clamp
// base + material-premultiplied sun colour) and return the packed base colour
// for the static V (what the vertex shows when the sun term is zero).
inline D3DCOLOR MakeRelitVert(const AtomicLight& lt, bool modmat,
                              const std::uint8_t mat[4],
                              const std::uint32_t* pl, const float nrm[3],
                              TrackRenderer::RelitSrc* out) {
    float nx = nrm[0], ny = nrm[1], nz = nrm[2];
    const float n2 = nx*nx + ny*ny + nz*nz;
    if (n2 > 1e-12f) { const float inv = 1.f/std::sqrt(n2);
                       nx*=inv; ny*=inv; nz*=inv; }
    out->n[0] = nx; out->n[1] = ny; out->n[2] = nz;
    std::uint8_t a = 255;
    LightAtomicVertexBase(lt, modmat, mat, pl, out->base, &a);
    out->sunmul[0] = lt.sun[0]; out->sunmul[1] = lt.sun[1];
    out->sunmul[2] = lt.sun[2];
    if (modmat) {
        out->sunmul[0] *= mat[0]/255.f;
        out->sunmul[1] *= mat[1]/255.f;
        out->sunmul[2] *= mat[2]/255.f;
    }
    out->relit = 1;
    return PackClamped(out->base, a);
}

// Per-frame relight kernel: append `baked` to `dst` and overwrite the diffuse
// of relit vertices with base + sunmul * max(0, N . Lm). Lm = unit TO-light
// direction in the instance's MODEL space (n . Lm == n_world . L_world for
// the orthonormal instance rotations used by the car/wheel draws).
inline void RelightAppend(std::vector<TrackRenderer::V>& dst,
                          const std::vector<TrackRenderer::V>& baked,
                          const std::vector<TrackRenderer::RelitSrc>& src,
                          const float Lm[3]) {
    const std::size_t at = dst.size();
    dst.insert(dst.end(), baked.begin(), baked.end());
    const std::size_t n = src.size() < baked.size() ? src.size()
                                                    : baked.size();
    for (std::size_t i = 0; i < n; ++i) {
        const auto& s = src[i];
        if (!s.relit) continue;
        float ndl = s.n[0]*Lm[0] + s.n[1]*Lm[1] + s.n[2]*Lm[2];
        if (ndl < 0.f) ndl = 0.f;
        const float rgb[3] = { s.base[0] + s.sunmul[0]*ndl,
                               s.base[1] + s.sunmul[1]*ndl,
                               s.base[2] + s.sunmul[2]*ndl };
        TrackRenderer::V& v = dst[at + i];
        v.c = PackClamped(rgb, static_cast<std::uint8_t>(v.c >> 24));
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
                // PAL4 and PAL8 are BOTH stored one byte per pixel; `depth`
                // selects the PALETTE SIZE (16 vs 256 entries), not the storage
                // width. Measured 2026-07-31 over every TXD in the game
                // (re/tools/txd_format_census.py, 5194 mips): stride is always
                // max(4, width * bpp) with bpp==1 for depth 4 AND 8, zero
                // violations, and stride*height+palette exactly fills the IMAGE
                // chunk (total slack 0). Independently, no byte in any depth-4
                // mip exceeds 0x0F -- impossible if two indices were packed per
                // byte. This previously read `row[x >> 1]` and unpacked nibble
                // pairs, which squashed every PAL4 track texture to half width
                // and scrambled its indices (dump.piz "Roadslipblend2" rendered
                // as vertical stripes). Affects Warzone/training/rouabout/sands/
                // City/Forest/Neustein/dump + SFX BADGES.
                const std::uint32_t idx =
                    (mip.depth == 4) ? (row[x] & 0x0Fu) : row[x];
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
//
// WS-E s2 lighting: `lt` (or nullptr) is the track's atomic light set (ambient +
// directional sun). When supplied, each vertex's diffuse is computed by the RW
// atomic-lighting model (see LightAtomicVertex): rpGEOMETRYLIGHT batches that
// carry normals get ambient + sun·N·L (×material colour where MODULATEMATERIAL-
// COLOR), prelit batches get prelight (+ambient), others fall back to white.
// nullptr reproduces the legacy unlit behaviour (prelight as-is, else white) —
// used for the frontend chrome models that are not in a lit track scene.
// WS-E vehicle lighting: `relit_out` (optional, parallel to *batches) makes
// lit-with-normals vertices bake WITHOUT the directional term (base =
// prelight+ambient) and emits a RelitSrc per vertex so the sun is applied per
// frame in world space (RelightAppend). Non-lit vertices bake exactly as
// before and get relit=0.
void BuildDffBatches(IDirect3DDevice9* dev,
                     const mashed_re::Track::DffModel& model,
                     const std::vector<mashed_re::Txd::Dictionary>& dicts,
                     std::vector<std::vector<TrackRenderer::V>>* batches,
                     std::vector<IDirect3DTexture9*>* textures,
                     const AtomicLight* lt = nullptr,
                     std::vector<std::vector<TrackRenderer::RelitSrc>>* relit_out
                         = nullptr) {
    using V = TrackRenderer::V;
    textures->assign(model.materials.size(), nullptr);
    // P3b diagnostic (MASHED_DBG_TEXMATCH): classify each material's texture
    // resolution as matched / empty-name (colour-only, correctly untextured) /
    // unmatched (a name the dicts don't contain — a real binding bug). The 2026-06-19
    // run of this on the car model answered the question it was built for: 71 materials,
    // 44 matched, 27 colour-only, 0 unmatched — i.e. the untextured batches are
    // by-design, NOT a binding bug. Kept because the same question recurs per-model,
    // and because the earlier "65% vs 97%" framing was a batch-vs-draw denominator
    // artefact that this counter is what disambiguates.
    static const bool s_dbg_tex = std::getenv("MASHED_DBG_TEXMATCH") != nullptr;
    int n_match = 0, n_empty = 0, n_unmatched = 0;
    std::string unmatched_names;
    for (std::size_t mi = 0; mi < model.materials.size(); ++mi) {
        const char* want = model.materials[mi].tex_name;
        if (!want[0]) { ++n_empty; continue; }
        for (std::size_t di = 0; di < dicts.size() && !(*textures)[mi]; ++di)
            for (std::uint32_t ti = 0; ti < dicts[di].count(); ++ti)
                if (std::strcmp(dicts[di].texture(ti).name, want) == 0) {
                    (*textures)[mi] = MakeTexture(dev, dicts[di].texture(ti));
                    break;
                }
        if ((*textures)[mi]) { ++n_match; }
        else { ++n_unmatched; if (unmatched_names.size() < 400) { unmatched_names += want; unmatched_names += ' '; } }
    }
    if (s_dbg_tex) {
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::fprintf(lf, "TEXMATCH: %zu mats match=%d empty=%d unmatched=%d"
                         " dicts=%zu unmatched_names=[%s]\n",
                         model.materials.size(), n_match, n_empty, n_unmatched,
                         dicts.size(), unmatched_names.c_str());
            // also list the available dict texture names once, to spot case/format diffs
            for (std::size_t di = 0; di < dicts.size(); ++di) {
                std::fprintf(lf, "  dict[%zu] (%u texs):", di, dicts[di].count());
                for (std::uint32_t ti = 0; ti < dicts[di].count() && ti < 24; ++ti)
                    std::fprintf(lf, " %s", dicts[di].texture(ti).name);
                std::fputc('\n', lf);
            }
            std::fclose(lf);
        }
    }
    batches->assign(model.materials.size(), {});
    if (relit_out) relit_out->assign(model.materials.size(), {});
    for (const auto& b : model.batches) {
        auto& out = (*batches)[b.material];
        const std::uint8_t* matRGBA = model.materials[b.material].rgba;
        const bool has_n  = !b.normals.empty();
        // per-frame relighting applies to lit+normal-bearing vertices when a
        // directional light exists; everything else keeps the static bake.
        const bool relight = relit_out && lt && lt->has_sun && b.lit && has_n;
        std::vector<TrackRenderer::RelitSrc>* rout =
            relit_out ? &(*relit_out)[b.material] : nullptr;
        const std::size_t n = b.tris.size();
        for (std::size_t i = 0; i < n; ++i) {
            const std::uint16_t vi = b.tris[i];
            V v;
            v.x = b.verts[vi * 3 + 0];
            v.y = b.verts[vi * 3 + 1];
            v.z = b.verts[vi * 3 + 2];
            const std::uint32_t* pl = b.prelit.empty() ? nullptr : &b.prelit[vi];
            if (relight) {
                TrackRenderer::RelitSrc s = {};
                v.c = MakeRelitVert(*lt, b.modulate_mat, matRGBA, pl,
                                    &b.normals[vi * 3], &s);
                rout->push_back(s);
            } else if (rout) {
                rout->push_back(TrackRenderer::RelitSrc{});
            }
            if (relight) {
                // colour already set above (base, sun applied per frame)
            } else if (lt) {
                const float* nrm = has_n ? &b.normals[vi * 3] : nullptr;
                v.c = LightAtomicVertex(*lt, b.lit, b.modulate_mat, matRGBA,
                                        pl, nrm);
            } else if (pl) {
                const std::uint32_t p = *pl;     // legacy: prelight as-is
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
    if (relit_out) {
        // drop all-static parallel arrays so those batches stay on the
        // cached-VB path (empty inner vector == "not runtime-lit")
        for (auto& r : *relit_out) {
            bool any = false;
            for (const auto& s : r) if (s.relit) { any = true; break; }
            if (!any) r.clear();
        }
    }
}

// WS-E lighting: parse a track's LIGHTS.DFF (COURSE.LUA Lights_Filename) for
// its ambient term. LIGHTS.DFF is a standard RW clump (rwID_CLUMP 0x10) whose
// only payload is RpLight chunks (rwID_LIGHT 0x12); each light's STRUCT body is
// { f32 radius; f32 r,g,b; f32 minusCosAngle; u32 type_flags } (type =
// type_flags>>16: 1=DIRECTIONAL 2=AMBIENT; flags = type_flags&0xffff, bit 0x2 =
// rpLIGHTLIGHTWORLD). Arctic LIGHTS.DFF (verified): AMBIENT (0.2,0.3,0.3) +
// DIRECTIONAL (0.6,0.7,0.7) both flags=0x3. The WORLD BSP has NO vertex normals
// (format 0x4001004d — TRISTRIP|TEXTURED|PRELIT|MODULATEMATERIALCOLOR, no 0x10
// NORMALS, no 0x20 LIGHT), so RW's directional term cannot apply to it per-
// vertex; only the directionless AMBIENT brightens the world. Returns the summed
// AMBIENT colour (rpLIGHTLIGHTWORLD lights only) as 0x00RRGGBB; 0 if none.
D3DCOLOR ParseLightsDffAmbient(const std::uint8_t* d, std::uint32_t len) {
    if (!d || len < 24) return 0;
    auto u32 = [&](std::size_t o) -> std::uint32_t {
        std::uint32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto f32 = [&](std::size_t o) -> float {
        float v; std::memcpy(&v, d + o, 4); return v;
    };
    if (u32(0) != 0x10) return 0;                 // not a CLUMP
    std::size_t end = 12 + u32(4);
    if (end > len) end = len;
    float ar = 0.f, ag = 0.f, ab = 0.f;
    // CLUMP's direct children: LIGHT chunks are siblings of FRAMELIST/GEOMLIST.
    for (std::size_t o = 12; o + 12 <= end; ) {
        const std::uint32_t t = u32(o), sz = u32(o + 4);
        const std::size_t body = o + 12;
        if (t == 0x12 && body + 12 <= end && u32(body) == 0x01) {  // LIGHT->STRUCT
            const std::size_t sb = body + 12;
            if (sb + 24 <= end) {
                const float r = f32(sb + 4), g = f32(sb + 8), b = f32(sb + 12);
                const std::uint32_t tf = u32(sb + 20);
                const std::uint32_t type = tf >> 16, flags = tf & 0xffff;
                if (type == 2 && (flags & 0x2)) { ar += r; ag += g; ab += b; }
            }
        }
        o = body + sz;
    }
    auto cl = [](float f) -> int {
        int v = static_cast<int>(f * 255.f + 0.5f);
        return v < 0 ? 0 : v > 255 ? 255 : v;
    };
    return D3DCOLOR_XRGB(cl(ar), cl(ag), cl(ab));
}

// WS-E s2: parse the track's DIRECTIONAL RpLight (type-1) from LIGHTS.DFF — its
// colour AND world-space direction. Unlike ambient, a directional light's
// direction is NOT in the light STRUCT; it is the light frame's at-vector. Layout
// (asset-verified, arctic LIGHTS.DFF): CLUMP -> STRUCT{numAtomics,numLights,
// numCameras} -> FRAMELIST{ STRUCT{int n; then n*(9f rot, 3f pos, i32 parent,
// u32 flags)=0x38 each} } -> GEOMETRYLIST -> (per light) STRUCT{i32 frameIdx} +
// LIGHT{ STRUCT{f32 radius; 3f rgb; f32 minusCosAngle; u32 type_flags} }. We pair
// each LIGHT with the preceding size-4 STRUCT (its frame index), and for a
// DIRECTIONAL light that lights atomics (flags bit rpLIGHTLIGHTATOMICS 0x1) we
// return its colour (0x00RRGGBB) and the frame's parent-chain-composed at-vector.
// Returns false if there is no such light. Arctic -> colour (153,178,178),
// dir (0.577,-0.577,-0.577).
// E2'b step 3: out_color retyped D3DCOLOR* -> std::uint32_t* to match sun_color_
// after its move into RaceSceneState. D3DCOLOR is `unsigned long` and uint32_t is
// `unsigned int` -- distinct types, so the VALUE sites converted silently but this
// POINTER site did not, which is what caught it at compile time.
bool ParseLightsDffDirectional(const std::uint8_t* d, std::uint32_t len,
                               std::uint32_t* out_color, float out_dir[3]) {
    if (!d || len < 24) return false;
    auto u32 = [&](std::size_t o) -> std::uint32_t {
        std::uint32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto i32 = [&](std::size_t o) -> std::int32_t {
        std::int32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto f32 = [&](std::size_t o) -> float {
        float v; std::memcpy(&v, d + o, 4); return v;
    };
    if (u32(0) != 0x10) return false;                  // not a CLUMP
    std::size_t end = 12 + u32(4);
    if (end > len) end = len;
    // CLUMP STRUCT
    if (12 + 12 > end || u32(12) != 0x01) return false;
    std::size_t o = 12 + 12 + u32(16);                 // past the clump struct
    // FRAMELIST
    struct Fr { float rot[9]; std::int32_t parent; };
    std::vector<Fr> frames;
    if (o + 12 <= end && u32(o) == 0x0E) {
        const std::size_t flb = o + 12, flsz = u32(o + 4);
        if (flb + 12 <= end && u32(flb) == 0x01) {
            const std::size_t fstb = flb + 12;
            const std::int32_t nf = i32(fstb);
            std::size_t q = fstb + 4;
            for (std::int32_t i = 0; i < nf && q + 0x38 <= end; ++i) {
                Fr f;
                for (int k = 0; k < 9; ++k) f.rot[k] = f32(q + k * 4u);
                f.parent = i32(q + 48);
                frames.push_back(f);
                q += 0x38;
            }
        }
        o = flb + flsz;
    }
    // remaining top-level children: pair STRUCT(frameIdx) with the next LIGHT.
    std::int32_t pending_frame = -1;
    for (; o + 12 <= end; ) {
        const std::uint32_t t = u32(o), sz = u32(o + 4);
        const std::size_t body = o + 12;
        if (t == 0x01 && sz == 4) {
            pending_frame = i32(body);
        } else if (t == 0x12 && body + 12 <= end && u32(body) == 0x01) {
            const std::size_t sb = body + 12;
            if (sb + 24 <= end) {
                const float r = f32(sb + 4), g = f32(sb + 8), b = f32(sb + 12);
                const std::uint32_t tf = u32(sb + 20);
                const std::uint32_t type = tf >> 16, flags = tf & 0xffff;
                if (type == 1 && (flags & 0x1) && pending_frame >= 0 &&
                    pending_frame < static_cast<std::int32_t>(frames.size())) {
                    // world at-vector = parent-chain rotation of local at
                    // (cols 6,7,8 of the frame's 3x3).
                    float v[3] = {frames[pending_frame].rot[6],
                                  frames[pending_frame].rot[7],
                                  frames[pending_frame].rot[8]};
                    for (int fi = pending_frame; fi >= 0; ) {
                        const float* m = frames[fi].rot;
                        const float x = m[0]*v[0] + m[3]*v[1] + m[6]*v[2];
                        const float y = m[1]*v[0] + m[4]*v[1] + m[7]*v[2];
                        const float z = m[2]*v[0] + m[5]*v[1] + m[8]*v[2];
                        const std::int32_t pa = frames[fi].parent;
                        if (pa == fi) break;            // guard
                        if (pa < 0) { v[0]=x; v[1]=y; v[2]=z; break; }
                        v[0]=x; v[1]=y; v[2]=z; fi = pa;
                    }
                    const float l = std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
                    if (l > 1e-6f) { v[0]/=l; v[1]/=l; v[2]/=l; }
                    auto cl = [](float f) -> int {
                        int x = static_cast<int>(f * 255.f + 0.5f);
                        return x < 0 ? 0 : x > 255 ? 255 : x;
                    };
                    if (out_color) *out_color = D3DCOLOR_XRGB(cl(r), cl(g), cl(b));
                    if (out_dir) { out_dir[0]=v[0]; out_dir[1]=v[1]; out_dir[2]=v[2]; }
                    return true;
                }
            }
            pending_frame = -1;
        }
        o = body + sz;
    }
    return false;
}

// WS-E vehicle lighting: faithful FUN_00479330 (0x00479330) DFF-branch light
// extraction. The original collects EVERY clump light (RpClumpForAllLights
// via FUN_004b4010; callback LAB_004b3f20 is 13 instructions with NO filter —
// code units read 2026-07-02, Mashed_pool14), keys on the light's subtype
// byte alone (light+0x01: 1=DIRECTIONAL, 2=AMBIENT — three in-function
// witnesses, see re/prior_art/notes/rw_lighting_research_2026-07.md §8.1),
// applies NO stream-flag test, and stores last-wins per subtype (plain scalar
// stores into course+0x105e0/+0x105e4). In the stream the subtype is
// typeAndFlags>>16. Directional direction = the light frame's parent-chain
// at-vector (same layout walk as ParseLightsDffDirectional above). Colours
// returned as floats (RwRGBAReal precision — the original never quantizes).
void ParseLightsDffFaithful(const std::uint8_t* d, std::uint32_t len,
                            float out_amb[3], bool* has_amb,
                            float out_sun[3], float out_dir[3],
                            bool* has_sun) {
    *has_amb = false;
    *has_sun = false;
    if (!d || len < 24) return;
    auto u32 = [&](std::size_t o) -> std::uint32_t {
        std::uint32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto i32 = [&](std::size_t o) -> std::int32_t {
        std::int32_t v; std::memcpy(&v, d + o, 4); return v;
    };
    auto f32 = [&](std::size_t o) -> float {
        float v; std::memcpy(&v, d + o, 4); return v;
    };
    if (u32(0) != 0x10) return;                        // not a CLUMP
    std::size_t end = 12 + u32(4);
    if (end > len) end = len;
    if (12 + 12 > end || u32(12) != 0x01) return;      // CLUMP STRUCT
    std::size_t o = 12 + 12 + u32(16);
    struct Fr { float rot[9]; std::int32_t parent; };
    std::vector<Fr> frames;
    if (o + 12 <= end && u32(o) == 0x0E) {             // FRAMELIST
        const std::size_t flb = o + 12, flsz = u32(o + 4);
        if (flb + 12 <= end && u32(flb) == 0x01) {
            const std::size_t fstb = flb + 12;
            const std::int32_t nf = i32(fstb);
            std::size_t q = fstb + 4;
            for (std::int32_t i = 0; i < nf && q + 0x38 <= end; ++i) {
                Fr f;
                for (int k = 0; k < 9; ++k) f.rot[k] = f32(q + k * 4u);
                f.parent = i32(q + 48);
                frames.push_back(f);
                q += 0x38;
            }
        }
        o = flb + flsz;
    }
    std::int32_t pending_frame = -1;
    for (; o + 12 <= end; ) {
        const std::uint32_t t = u32(o), sz = u32(o + 4);
        const std::size_t body = o + 12;
        if (t == 0x01 && sz == 4) {
            pending_frame = i32(body);
        } else if (t == 0x12 && body + 12 <= end && u32(body) == 0x01) {
            const std::size_t sb = body + 12;
            if (sb + 24 <= end) {
                const float r = f32(sb + 4), g = f32(sb + 8), b = f32(sb + 12);
                const std::uint32_t type = u32(sb + 20) >> 16;
                if (type == 2) {                       // AMBIENT — last wins
                    out_amb[0] = r; out_amb[1] = g; out_amb[2] = b;
                    *has_amb = true;
                } else if (type == 1) {                // DIRECTIONAL — last wins
                    out_sun[0] = r; out_sun[1] = g; out_sun[2] = b;
                    // frame's world at-vector (parent-chain composed); a
                    // frameless stream light keeps the previous/zero dir —
                    // shipped LIGHTS.DFF directionals all carry a frame.
                    if (pending_frame >= 0 &&
                        pending_frame < static_cast<std::int32_t>(frames.size())) {
                        float v[3] = {frames[pending_frame].rot[6],
                                      frames[pending_frame].rot[7],
                                      frames[pending_frame].rot[8]};
                        for (int fi = pending_frame; fi >= 0; ) {
                            const float* m = frames[fi].rot;
                            const float x = m[0]*v[0] + m[3]*v[1] + m[6]*v[2];
                            const float y = m[1]*v[0] + m[4]*v[1] + m[7]*v[2];
                            const float z = m[2]*v[0] + m[5]*v[1] + m[8]*v[2];
                            const std::int32_t pa = frames[fi].parent;
                            if (pa == fi) break;        // guard
                            if (pa < 0) { v[0]=x; v[1]=y; v[2]=z; break; }
                            v[0]=x; v[1]=y; v[2]=z; fi = pa;
                        }
                        const float l = std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
                        if (l > 1e-6f) { v[0]/=l; v[1]/=l; v[2]/=l; }
                        out_dir[0]=v[0]; out_dir[1]=v[1]; out_dir[2]=v[2];
                    }
                    *has_sun = true;
                }
            }
            pending_frame = -1;
        }
        o = body + sz;
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
    // MIRROR FIX (2026-08-16). RenderWare's camera space and D3D's disagree on
    // which way the right axis points on screen, and the standalone had been
    // rendering the world REFLECTED relative to the original because of it --
    // self-consistently, which is why it survived this long unnoticed.
    //
    // Measured: transplanting the original's own RwCamera basis verbatim gives a
    // mirror image (89.68% vs the original capture); negating the right axis
    // gives 33.79%, and beats a post-hoc pixel flip of the same render (44.38%)
    // because it fixes occlusion rather than just moving pixels. It also
    // retro-explains the ~90px car-projection offset previously blamed on
    // capture timing. Evidence: verify/d1_basis/RESULT.md.
    //
    // Applied to the axis, not to the projection's _11, deliberately: librw
    // builds its own projection from the published fov/aspect/near/far, so
    // negating there would flip only the D3D9 path and silently break the
    // renderer A/B. Negating here keeps the change in the view basis.
    // y is computed from the ORIGINAL x above, so the negation happens after.
    x[0] = -x[0]; x[1] = -x[1]; x[2] = -x[2];
    MatIdentity(m);
    m->_11 = x[0]; m->_12 = y[0]; m->_13 = z[0];
    m->_21 = x[1]; m->_22 = y[1]; m->_23 = z[1];
    m->_31 = x[2]; m->_32 = y[2]; m->_33 = z[2];
    m->_41 = -(x[0]*eye[0] + x[1]*eye[1] + x[2]*eye[2]);
    m->_42 = -(y[0]*eye[0] + y[1]*eye[1] + y[2]*eye[2]);
    m->_43 = -(z[0]*eye[0] + z[1]*eye[1] + z[2]*eye[2]);
}

// Same view matrix, but from an EXPLICIT orthonormal basis instead of a
// look-at pair. MatLookAtLH above hardcodes up = world +Y, which silently
// forces roll to zero; the original's race camera carries a real roll (measured
// ~26 deg, RwCamera frame right.y = 0.440, verify/d1_carproj/RESULT.md), so a
// look-at pair CANNOT reproduce its view and any transplant through one is
// wrong about the horizon by construction. Rows are the basis, translation is
// -dot(axis, pos) -- identical convention to MatLookAtLH, just without
// re-deriving the axes from a target point.
void MatViewFromBasis(D3DMATRIX* m, const float pos[3], const float right[3],
                      const float up[3], const float at[3]) {
    // Same right-axis negation as MatLookAtLH -- see the block there. The basis
    // passed in is RenderWare's, straight off the RwCamera frame, so it gets the
    // identical treatment and a transplanted pose needs NO pre-negation.
    const float r[3] = { -right[0], -right[1], -right[2] };
    MatIdentity(m);
    m->_11 = r[0]; m->_12 = up[0]; m->_13 = at[0];
    m->_21 = r[1]; m->_22 = up[1]; m->_23 = at[1];
    m->_31 = r[2]; m->_32 = up[2]; m->_33 = at[2];
    m->_41 = -(r[0]*pos[0]  + r[1]*pos[1]  + r[2]*pos[2]);
    m->_42 = -(up[0]*pos[0] + up[1]*pos[1] + up[2]*pos[2]);
    m->_43 = -(at[0]*pos[0] + at[1]*pos[1] + at[2]*pos[2]);
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

namespace { void InvalidateBatchCache(); }  // WS-A s3 PERF (defined before Render)

bool TrackRenderer::Load(IDirect3DDevice9* dev, const char* piz_path,
                         const char* log_path) {
    InvalidateBatchCache();   // drop any prior track's cached VBs (reload safety)
    // E2'b step 3: same reload safety on the librw side -- see the header note on
    // why this cannot live at the tail alongside RaceSubmit_OnTrackLoaded.
    if (LibRw::RaceSubmit_Requested()) LibRw::RaceSubmit_BeginTrackLoad();
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
                        // element 0 is the 16-bit material index (same index
                        // space the render path uses at :1132/:1581) — keep it
                        // so the wheel contact can reach record +0x1ec.
                        col_mat_.push_back(s.tris[ti * 4 + 0]);
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

    // WS-E lighting: parse the track's RpLight set (COURSE.LUA
    // Lights_Filename -> LIGHTS.DFF) BEFORE building the world batches, so it
    // can be baked into the prelit vertex colours below. The world BSP carries
    // no vertex normals, so RW applies only the (directionless) ambient to the
    // static world — see ParseLightsDffAmbient.
    //
    // WS-E vehicle lighting (MASHED_RPLIGHT, default ON): faithful
    // FUN_00479330 (0x00479330) acquisition — float colours, no stream-flag
    // filter, last-wins per subtype, default lights when no Lights_Filename,
    // Ambient_RGB override in the DFF branch only. =0 reverts to the legacy
    // quantized first/summed extraction below.
    rp_light_on_ = RpLightEnabled();
    amb_world_ = 0;
    sun_color_ = 0;
    sun_dir_[0] = sun_dir_[1] = sun_dir_[2] = 0.f;
    has_sun_dir_ = false;
    amb_f_[0] = amb_f_[1] = amb_f_[2] = 0.f;
    sun_f_[0] = sun_f_[1] = sun_f_[2] = 0.f;
    sun_L_[0] = sun_L_[1] = sun_L_[2] = 0.f;
    char  lights_fn[64] = {};
    float amb_cfg[3] = {0.f, 0.f, 0.f};   // COURSE.LUA Ambient_RGB(r,g,b)
    bool  has_amb_cfg = false;
    for (std::uint32_t i = 0; i < piz.count(); ++i) {
        const char* n = piz.entry(i).name;
        const std::size_t ln = std::strlen(n);
        if (ln < 4 || _strnicmp(n, "COURSE", 6) != 0 ||
            _stricmp(n + ln - 4, ".LUA") != 0)
            continue;
        std::uint32_t cl = 0;
        const std::uint8_t* cb = piz.blob(i, &cl);
        if (!cb) break;
        const char* cs = reinterpret_cast<const char*>(cb);
        for (std::uint32_t pos = 0; pos < cl; ) {
            std::uint32_t eol = pos;
            while (eol < cl && cs[eol] != '\n') ++eol;
            std::string line(cs + pos, eol - pos);
            pos = eol + 1;
            const std::size_t cmt = line.find("--");
            if (cmt != std::string::npos) line.resize(cmt);
            if (!lights_fn[0]) {
                std::sscanf(line.c_str(), " Lights_Filename( \"%63[^\"]\" )",
                            lights_fn) == 1 ||
                std::sscanf(line.c_str(), " Lights_Filename(\"%63[^\"]\")",
                            lights_fn);
            }
            // Ambient_RGB -> handler 0x0047aad0 -> course-description floats
            // +0x76..0x78 (config-table entry 0x00440d10). No shipped course
            // sets it; call syntax follows the other config calls.
            if (!has_amb_cfg &&
                std::sscanf(line.c_str(), " Ambient_RGB( %f , %f , %f",
                            &amb_cfg[0], &amb_cfg[1], &amb_cfg[2]) == 3)
                has_amb_cfg = true;
        }
        break;
    }
    const std::uint8_t* lights_db = nullptr;
    std::uint32_t       lights_dl = 0;
    if (lights_fn[0])
        for (std::uint32_t j = 0; j < piz.count(); ++j)
            if (_stricmp(piz.entry(j).name, lights_fn) == 0) {
                lights_db = piz.blob(j, &lights_dl);
                break;
            }
    if (!rp_light_on_) {
        // legacy path (pre-RpLight-subset behaviour, byte-identical)
        if (lights_db) {
            amb_world_ = ParseLightsDffAmbient(lights_db, lights_dl);
            ParseLightsDffDirectional(lights_db, lights_dl, &sun_color_,
                                      sun_dir_);
        }
    } else if (!lights_db) {
        // FUN_00479330 default branch (course_block[0x2640] empty,
        // LAB_00479951): frameless ambient from DAT_006132dc =
        // (0.25,0.25,0.25,1.0) + directional from DAT_006132ec =
        // (0.75,0.75,0.75,1.0) on a fresh frame rotated 60.0f (0x42700000)
        // about (1,0,0), combine 0 (replace). at-row of the axis-angle matrix
        // (FUN_004c4d20 deg->rad 0x3c8efa35, FUN_004c4a50 element formulas):
        // (0, -sin60, cos60).
        amb_f_[0] = amb_f_[1] = amb_f_[2] = 0.25f;
        sun_f_[0] = sun_f_[1] = sun_f_[2] = 0.75f;
        sun_dir_[0] = 0.f;
        sun_dir_[1] = -0.8660254f;   // -sin(60 deg)
        sun_dir_[2] = 0.5f;          //  cos(60 deg)
        has_sun_dir_ = true;
    } else {
        // FUN_00479330 DFF branch: every clump light, subtype byte only,
        // last-wins; then the Ambient_RGB override (this branch only) when
        // any component exceeds DAT_005d757c = 0.0f, replacing/creating the
        // ambient light's colour (FUN_004e4900 @ 0x00479944).
        bool has_amb = false;
        ParseLightsDffFaithful(lights_db, lights_dl, amb_f_, &has_amb,
                               sun_f_, sun_dir_, &has_sun_dir_);
        if (has_amb_cfg &&
            (amb_cfg[0] > 0.f || amb_cfg[1] > 0.f || amb_cfg[2] > 0.f)) {
            amb_f_[0] = amb_cfg[0]; amb_f_[1] = amb_cfg[1];
            amb_f_[2] = amb_cfg[2];
        }
    }
    if (rp_light_on_) {
        // packed mirrors for the legacy consumers (world prelit fill, logs)
        auto cl255 = [](float f) -> int {
            int v = static_cast<int>(f * 255.f + 0.5f);
            return v < 0 ? 0 : v > 255 ? 255 : v;
        };
        amb_world_ = D3DCOLOR_XRGB(cl255(amb_f_[0]), cl255(amb_f_[1]),
                                   cl255(amb_f_[2]));
        if (has_sun_dir_) {
            sun_color_ = D3DCOLOR_XRGB(cl255(sun_f_[0]), cl255(sun_f_[1]),
                                       cl255(sun_f_[2]));
            float lx = -sun_dir_[0], ly = -sun_dir_[1], lz = -sun_dir_[2];
            const float ll = std::sqrt(lx*lx + ly*ly + lz*lz);
            if (ll > 1e-6f) { sun_L_[0] = lx/ll; sun_L_[1] = ly/ll;
                              sun_L_[2] = lz/ll; }
            else            { has_sun_dir_ = false; }
        }
    }
    if (log)
        std::fprintf(log,
                     "  WS-E lights: ambient=0x%06lX (RGB %lu,%lu,%lu)"
                     " sun=0x%06lX dir=(%.3f,%.3f,%.3f)\n",
                     static_cast<unsigned long>(amb_world_),
                     static_cast<unsigned long>((amb_world_ >> 16) & 0xFF),
                     static_cast<unsigned long>((amb_world_ >> 8) & 0xFF),
                     static_cast<unsigned long>(amb_world_ & 0xFF),
                     static_cast<unsigned long>(sun_color_),
                     sun_dir_[0], sun_dir_[1], sun_dir_[2]);

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
                // prelight is RGBA bytes; D3D diffuse is BGRA dwords.
                // WS-E s4 colour-grade fix: render the BAKED prelit colours
                // AS-IS — do NOT re-add the track RpLight ambient at frame time.
                // The world BSP geometry carries no rpGEOMETRYLIGHT flag (format
                // 0x4001004d, see comment at ParseLightsDffAmbient), so RenderWare
                // renders its prelit colours directly; the ambient is baked into
                // them offline, not re-applied per frame. s1/s2 added amb_world_
                // (Arctic (51,76,76) = cool/teal, B=G>>R) to every world vertex,
                // double-counting the ambient and dragging the whole frame teal
                // (B-highest, lum +~7). DECISIVE evidence it is not runtime-added:
                // the original Arctic frame measures B as the LOWEST channel
                // (29.8 < R 34.7, G 34.8); had the original re-added (51,76,76),
                // its B would be the HIGHEST like the cool standalone — it is not.
                if (has_pl) {
                    const std::uint32_t p = s.prelit[vi];
                    v.c = D3DCOLOR_ARGB(static_cast<std::uint8_t>(p >> 24),
                                        static_cast<std::uint8_t>(p),         // R
                                        static_cast<std::uint8_t>(p >> 8),    // G
                                        static_cast<std::uint8_t>(p >> 16));  // B
                } else {
                    v.c = 0xFFFFFFFFu;
                }
                // [D-S3-BANK discriminator] MASHED_WORLD_FLATSNOW=1: constant grey
                // on the snow (mat 4), same value the librw side forces. Kills the
                // colour gradient so an FF-vs-shader COLOR iteration delta cannot
                // show; a surviving delta is transform/coverage. Snow-only so it
                // pairs with ONLYMAT=4 and leaves the rest of the world untouched.
                // MASHED_WORLD_FLATSNOW=<level 0..255>: the value IS the grey level,
                // so a sweep fits the D3D9 FF output transfer curve per channel.
                static const int s_flatsnow = [] {
                    const char* e = std::getenv("MASHED_WORLD_FLATSNOW");
                    if (!e || !*e) return -1;
                    int v = std::atoi(e); if (v < 0) v = 0; if (v > 255) v = 255;
                    return v;
                }();
                if (s_flatsnow >= 0 && mat == 4) {
                    const auto g = static_cast<std::uint8_t>(s_flatsnow);
                    v.c = D3DCOLOR_ARGB(255, g, g, g);
                }
                v.u = has_uv ? s.uvs[vi * 2 + 0] : 0.f;
                v.v = has_uv ? s.uvs[vi * 2 + 1] : 0.f;
                b.push_back(v);
            }
        }
    }

    // D-S3-BANK probe: the same per-material triangle tally BuildWorld logs, so
    // the two world builds can be compared as numbers rather than screenshots.
    if (log)
        for (std::size_t mi = 0; mi < batches_.size(); ++mi)
            std::fprintf(log, "D3D9 world.tris mat[%zu]=%zu tex=%d\n",
                         mi, batches_[mi].size() / 3,
                         mi < textures_.size() && textures_[mi] ? 1 : 0);

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
        // WS-E s2: the track's atomic light set (ambient + directional sun),
        // applied to prop atomics by the RW lighting model. The sky dome passes
        // nullptr (it is a prelit camera-locked backdrop, not a lit atomic).
        const AtomicLight track_light = rp_light_on_
            ? MakeAtomicLightF(amb_f_, sun_f_, has_sun_dir_, sun_L_)
            : MakeAtomicLight(amb_world_, sun_color_, sun_dir_);
        auto load_prop = [&](const char* dff_name, Prop* p,
                             const AtomicLight* lt = nullptr) -> bool {
            std::uint32_t dl = 0;
            const std::uint8_t* db = find_entry(dff_name, &dl);
            if (!db) return false;
            Track::DffModel m;
            if (!m.Parse(db, dl)) return false;
            BuildDffBatches(dev, m, dicts, &p->batches, &p->textures, lt);
        // D-S3-6 probe: what does the D3D9 bake produce for vertex 0 of this
        // prop, and was an AtomicLight applied? Compare against the raw prelit
        // value BuildClump uploads for the same vertex (log/librw_scene.txt).
        // If they differ by the track ambient, the librw path is missing the
        // ambient the D3D9 path bakes in -- which is the standing hypothesis.
        if (log) {
            // D-S3-SEA: log EVERY batch, not just the first non-empty one. The
            // sea is one batch of a multi-batch prop, so a single "first batch"
            // sample cannot be lined up against the librw UPLOAD lines, which
            // are per batch. Batch order is the same on both sides (both walk
            // model.batches in order), so index i pairs with index i.
            for (std::size_t bi = 0; bi < p->batches.size(); ++bi) {
                const auto& bb = p->batches[bi];
                if (bb.empty()) continue;
                std::fprintf(log,
                    "D-S3-6 bake: dff=%s lt=%s batch=%zu baked_v0=0x%08X\n",
                    dff_name, lt ? "YES" : "no", bi, bb[0].c);
            }
        }
            // F3: bind each material's UVAnim-extension name to its .UVA rate.
            p->mat_scroll.assign(m.materials.size(), {});
            for (std::size_t mi = 0; mi < m.materials.size(); ++mi)
                p->mat_scroll[mi] = uv_rate(m.materials[mi].uv_anim);
        // E2'b step 3: hand this model to librw HERE -- `m` is a local that dies
        // at the closing brace, and nothing else retains a parsed DffModel.
        // The UV rates must be resolved BEFORE this call, not after: librw has no
        // texture matrix and applies the scroll by moving the coordinates, so it
        // needs the rates at registration time. They used to be filled in below
        // this call, which left the librw side with none.
        if (LibRw::RaceSubmit_Requested()) {
            std::vector<float> rates(m.materials.size() * 2, 0.f);
            for (std::size_t mi = 0; mi < p->mat_scroll.size(); ++mi) {
                rates[mi * 2 + 0] = p->mat_scroll[mi].du;
                rates[mi * 2 + 1] = p->mat_scroll[mi].dv;
            }
            p->rw_model = LibRw::RaceSubmit_RegisterModel(
                m, dicts.data(), dicts.size(), amb_world_,
                rates.empty() ? nullptr : rates.data(), m.materials.size());
            // [D-S3-PROP] MASHED_PROP_VDUMP=<handle>: dump EVERY vertex of the
            // model registered under that handle, not just v0. v0 agreeing is
            // exactly what made the ambient bake look innocent -- one vertex
            // cannot separate "the bake matches" from "the bake matches at this
            // one vertex". Written as "batch x y z r g b" so it pairs
            // positionally with the librw dump, the same protocol as the world
            // MASHED_WORLD_VDUMP pair. Must follow RegisterModel: rw_model is
            // assigned by that call.
            static const int s_prop_vdump = [] {
                const char* e = std::getenv("MASHED_PROP_VDUMP");
                return (e && *e) ? std::atoi(e) : -1;
            }();
            if (s_prop_vdump >= 0) {
                // Manifest: EVERY registration, so which handle a given dff got is
                // visible by counting (the AM run's "only model[5]" error was a
                // gated-log misread; never infer registration from a gated run).
                // This is also how we learned the D3D9 per-vertex dump was not
                // firing for model[8] -- read the manifest, not the guard.
                if (std::FILE* mf = std::fopen("log/pvdump_manifest.txt", "a")) {
                    std::fprintf(mf, "REG handle=%d dff=%s d3d9_batches=%zu "
                                 "m_batches=%zu requested=1\n",
                                 p->rw_model, dff_name, p->batches.size(),
                                 m.batches.size());
                    std::fclose(mf);
                }
            }
            if (s_prop_vdump >= 0 && p->rw_model == s_prop_vdump) {
                // D3D9's CPU-baked lit colour + the inputs that produced it, per
                // vertex, for the lit batches librw cannot dump (they carry no
                // uploaded vertex colour -- lit in-shader). Columns:
                //   batch mat lit mod matR matG matB  x y z  nx ny nz  bakedR bakedG bakedB
                // p->batches[bi] is aligned 1:1 with m.batches[bi] (both walk the
                // DffModel in order); m carries the flags/normals/matcol, p carries
                // the baked V.c. Lets the bake be checked against
                // matCol*(amb + sun*max(0,N.L)) offline and against librw's HLSL.
                if (std::FILE* vf = std::fopen("log/pvdump_d3d9.txt", "w")) {
                    const auto& amb = amb_f_; const auto& sun = sun_f_;
                    std::fprintf(vf, "# dff=%s handle=%d batches=%zu "
                                 "amb=(%.3f,%.3f,%.3f) sun=(%.3f,%.3f,%.3f) "
                                 "sunL=(%.3f,%.3f,%.3f) has_sun=%d\n",
                                 dff_name, p->rw_model, p->batches.size(),
                                 amb[0], amb[1], amb[2], sun[0], sun[1], sun[2],
                                 sun_L_[0], sun_L_[1], sun_L_[2], (int)has_sun_dir_);
                    const std::size_t nb = p->batches.size() < m.batches.size()
                        ? p->batches.size() : m.batches.size();
                    for (std::size_t bi = 0; bi < nb; ++bi) {
                        const auto& db = p->batches[bi];
                        const auto& mb = m.batches[bi];
                        const std::uint8_t* mc =
                            m.materials[mb.material].rgba;
                        const bool hasN = !mb.normals.empty();
                        for (std::size_t vi = 0; vi < db.size(); ++vi) {
                            const V& v = db[vi];
                            float nx = 0, ny = 0, nz = 0;
                            if (hasN && vi * 3 + 2 < mb.normals.size()) {
                                nx = mb.normals[vi * 3 + 0];
                                ny = mb.normals[vi * 3 + 1];
                                nz = mb.normals[vi * 3 + 2];
                            }
                            std::fprintf(vf,
                                "%zu %u %d %d %u %u %u  %.3f %.3f %.3f  "
                                "%.4f %.4f %.4f  %u %u %u\n",
                                bi, (unsigned)mb.material, (int)mb.lit,
                                (int)mb.modulate_mat, mc[0], mc[1], mc[2],
                                v.x, v.y, v.z, nx, ny, nz,
                                (v.c >> 16) & 0xFF, (v.c >> 8) & 0xFF, v.c & 0xFF);
                        }
                    }
                    std::fclose(vf);
                }
            }
        }
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
                    if (mb && ml >= 4 && load_prop(a, &p, &track_light)) {
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
                    // Setup_Fog(near, far, r, g, b) — track fog wiring. args 1/2
                    // are ABSOLUTE camera-space fog distances (near < far), NOT a
                    // fraction. (Asset survey: Arctic 0.1/70, Egypt 20/100,
                    // Highway 50/300, training 20/360, Storm 20/100, sands
                    // 250/255 — every track has near < far.)
                    float fa = 0.f, fb = 0.f; int fr = 0, fg2 = 0, fb2 = 0;
                    char sk[64] = {};
                    if (std::sscanf(line.c_str(),
                                    " Setup_Fog( %f , %f , %d , %d , %d )",
                                    &fa, &fb, &fr, &fg2, &fb2) == 5 ||
                        std::sscanf(line.c_str(),
                                    " Setup_Fog(%f,%f,%d,%d,%d)",
                                    &fa, &fb, &fr, &fg2, &fb2) == 5) {
                        fog_on_ = true;
                        // WS-E TRACKFIX (2026-06-29): fog_start_ = fa (the near
                        // distance), NOT fa*fb. The old `fa*fb` only stayed valid
                        // for Arctic (0.1*70 = 7 < 70); for every track with
                        // fa >= 1 it produced fog_start_ > fog_end_, INVERTING the
                        // D3D linear-fog ramp (factor = (end-d)/(end-start) goes
                        // negative for near d) so ALL near geometry clamped to
                        // factor 0 = full fog colour. That is the blown-out frame
                        // (Egypt/Highway/training/sands == the exact fog colour)
                        // and the black frame (Storm, fog colour 0,0,0).
                        fog_start_ = fa;
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
                if (load_prop(c.dff, &p, &track_light)) {
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

    // WS-AI-BRIDGE / Option B: load AI<course>.AI -> controller image so the faithful
    // racing-line lookahead (Ai_ComputeTarget) is available. Loaded by DEFAULT now (the
    // opponents drive the real line); MASHED_GATE_RIBBON_AI=1 forces the legacy gate-ribbon
    // scaffold (A/B hatch). A load failure also falls back to the gate ribbon.
    {
        static const bool s_gate_ribbon = (std::getenv("MASHED_GATE_RIBBON_AI") != nullptr);
        g_aib.course  = course_id_;
        g_aib.loaded  = s_gate_ribbon ? false : Ai_BridgeLoad(course_id_, piz_path);
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
    // E2'b step 3: hand the librw submitter the SAME parsed world + texture
    // dictionaries this function just used. Called here because `world` and
    // `dicts` are locals that die at the closing brace -- building the librw
    // scene from anywhere else would mean re-parsing GRAPH.BSP/TEXTURES.TXD into
    // a second copy that could disagree with this one. No-op unless
    // MASHED_RENDER_LIBRW=1 brought the engine up.
    if (LibRw::RaceSubmit_Requested())
        LibRw::RaceSubmit_OnTrackLoaded(world, dicts.data(), dicts.size());

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
    // gameplay=true: KTC_NewCopter (KTCSCRIPT.LUA, rule-5 feed); false:
    // SetCopter (COURSE.LUA, scenery-only, D-11056/U-8997).
    struct Bind { char model[32]; int slot; bool gameplay; };
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
                    b.slot = slot; b.gameplay = true; binds.push_back(b);
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
        // WS-E s2: copters are lit atomics (normals + rpGEOMETRYLIGHT) — light
        // them with the track's directional sun + ambient like the props/cars.
        const AtomicLight copter_light = rp_light_on_
            ? MakeAtomicLightF(amb_f_, sun_f_, has_sun_dir_, sun_L_)
            : MakeAtomicLight(amb_world_, sun_color_, sun_dir_);
        BuildDffBatches(dev, m, dicts, &cm.batches, &cm.textures, &copter_light);
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
        c.gameplay = b.gameplay;
        copters_.push_back(std::move(c));
        ++loaded;
    }
    if (log) {
        int gameplay_n = 0;
        for (const auto& c : copters_) if (c.gameplay) ++gameplay_n;
        std::fprintf(log,
            "  F2 copters: %d flying, %zu models from %s (%d gameplay/rule-5)\n",
            loaded, copter_models_.size(), perm.c_str(), gameplay_n);
    }
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

// 16-ray ring probe for an on-mesh escape heading, preferring the one nearest curYaw.
bool TrackRenderer::FindOnMeshHeading(float px, float pz, float curYaw,
                                     float radius, float& outYaw) const {
    const int   kRays = 16;
    const float kPi   = 3.14159265358979f;
    bool found = false; float bestDev = 1e9f, bestYaw = curYaw;
    for (int i = 0; i < kRays; ++i) {
        const float a = (2.f * kPi) * static_cast<float>(i) / static_cast<float>(kRays);
        bool ok = false;
        GroundHeight(px + std::cos(a) * radius, pz + std::sin(a) * radius, &ok);
        if (!ok) continue;
        float d = a - curYaw;
        while (d >  kPi) d -= 2.f * kPi;
        while (d < -kPi) d += 2.f * kPi;
        d = (d < 0.f) ? -d : d;
        if (d < bestDev) { bestDev = d; bestYaw = a; found = true; }
    }
    outYaw = bestYaw;
    return found;
}

// Gate-independent off-mesh recovery (called from the off-mesh branches of UpdateCar).
// car_pos_ is still the last ON-MESH point (the edge) — only the attempted next step
// went off. Find an on-mesh heading near the current one, nudge the position a little
// inward (committing only an on-mesh landing), and redirect heading + a modest forward
// speed. Guarantees the car never permanently freezes against an edge; the player's
// steering (or the chain) then carries it back to the interior.
void TrackRenderer::RecoverOffMesh() {
    float ry; float rad = 0.f;
    if      (FindOnMeshHeading(car_pos_[0], car_pos_[2], car_yaw_, 1.2f, ry)) rad = 1.2f;
    else if (FindOnMeshHeading(car_pos_[0], car_pos_[2], car_yaw_, 0.5f, ry)) rad = 0.5f;
    if (rad <= 0.f) {                 // fully stranded (isolated point) — rare
        car_vel_[0] = car_vel_[2] = 0.f; car_speed_ = 0.f; return;
    }
    float mx = car_pos_[0] + std::cos(ry) * 0.3f;
    float mz = car_pos_[2] + std::sin(ry) * 0.3f;
    bool mok = false; float my = GroundHeight(mx, mz, &mok);
    if (!mok) {                       // small step landed off; jump to the verified ring point
        mx = car_pos_[0] + std::cos(ry) * rad;
        mz = car_pos_[2] + std::sin(ry) * rad;
        my = GroundHeight(mx, mz, &mok);
    }
    if (mok) { car_pos_[0] = mx; car_pos_[2] = mz; car_pos_[1] = my + car_ground_off_; }
    const float sp = (car_speed_ > 4.f ? car_speed_ * 0.5f : 4.f);
    car_yaw_ = ry;
    // A8: re-seed the integrated body basis to the recovered heading.
    Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);
    car_vel_[0] = std::cos(ry) * sp; car_vel_[2] = std::sin(ry) * sp;
    car_speed_  = sp;
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

    // textures for ALL car materials (all_batches' colours are discarded — the
    // lit body/wheel verts are built in the custom loop below)
    std::vector<std::vector<V>> all_batches;
    BuildDffBatches(dev, model, dicts, &all_batches, &car_textures_);
    // E2'b step 3: same lifetime rule as props -- register before `model` dies.
    if (LibRw::RaceSubmit_Requested())
        rw_car_model_ = LibRw::RaceSubmit_RegisterModel(model, dicts.data(), dicts.size(),
                                                       amb_world_);
    // WS-E s2: the track's atomic light set drives the car's directional shading
    // (RW lights the body panels — normals + rpGEOMETRYLIGHT — per LIGHTS.DFF).
    // WS-E vehicle lighting: under MASHED_RPLIGHT the directional term is NOT
    // baked here — lit vertices get RelitSrc entries and are relit per frame
    // in world space (RW lights atomics by their current LTM, not once at
    // load; the legacy bake froze the shading to the body).
    const AtomicLight car_light = rp_light_on_
        ? MakeAtomicLightF(amb_f_, sun_f_, has_sun_dir_, sun_L_)
        : MakeAtomicLight(amb_world_, sun_color_, sun_dir_);
    const bool car_relight = rp_light_on_ && car_light.has_sun;
    // body batches exclude wheel atomics; wheels collected pivot-relative
    car_batches_.assign(model.materials.size(), {});
    car_relit_.assign(model.materials.size(), {});
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
        // convert this model batch to V verts, lit by the RW atomic model
        // (WS-E s2: real per-vertex normals -> ambient + sun·N·L; ×material
        // colour where MODULATEMATERIALCOLOR — e.g. livery panels).
        const std::uint8_t* matRGBA = model.materials[b.material].rgba;
        const bool has_n = !b.normals.empty();
        // WS-E vehicle lighting: lit batches get per-vertex relight sources
        // (sun applied per frame in world space); everything else bakes as
        // before. `rs` stays in lockstep with `vs` whenever relighting is on
        // so merged per-material arrays keep parallel indexing.
        const bool relit_b = car_relight && b.lit && has_n;
        std::vector<V> vs;
        std::vector<RelitSrc> rs;
        const std::size_t n = b.tris.size();
        vs.reserve(n);
        if (car_relight) rs.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            const std::uint16_t vi = b.tris[i];
            V v;
            v.x = b.verts[vi * 3 + 0];
            v.y = b.verts[vi * 3 + 1];
            v.z = b.verts[vi * 3 + 2];
            const std::uint32_t* pl = b.prelit.empty() ? nullptr : &b.prelit[vi];
            const float* nrm = has_n ? &b.normals[vi * 3] : nullptr;
            if (relit_b) {
                RelitSrc s = {};
                v.c = MakeRelitVert(car_light, b.modulate_mat, matRGBA, pl,
                                    nrm, &s);
                rs.push_back(s);
            } else {
                if (car_relight) rs.push_back(RelitSrc{});
                v.c = LightAtomicVertex(car_light, b.lit, b.modulate_mat,
                                        matRGBA, pl, nrm);
            }
            v.u = b.uvs[vi * 2 + 0];
            v.v = b.uvs[vi * 2 + 1];
            vs.push_back(v);
        }
        if (!relit_b && car_relight && !is_wheel &&
            car_relit_[b.material].empty()) {
            // no lit vertex and the material has no relit content so far —
            // stay on the static path (the append below backfills zeros if a
            // later batch brings relit content to this material).
            rs.clear();
        }
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
                    w.parts_relit.push_back(relit_b ? std::move(rs)
                                                    : std::vector<RelitSrc>{});
                    break;
                }
            }
        } else {
            auto& out = car_batches_[b.material];
            auto& rout = car_relit_[b.material];
            if (!rs.empty() && rout.empty() && !out.empty()) {
                // material had earlier static-only batches; backfill zeros to
                // restore parallel indexing before appending relit content.
                rout.resize(out.size());
            }
            out.insert(out.end(), vs.begin(), vs.end());
            if (!rs.empty())
                rout.insert(rout.end(), rs.begin(), rs.end());
        }
    }
    car_ground_off_ = -model.bbox[1];   // lift so model min-Y sits on ground
    // WS-E s3: car model size -> chase-camera scale. The Arctic geometry loads at
    // a small unit scale (car ~0.7 units long), so the chase rig is derived from
    // the car's own bbox rather than absolute units.
    car_len_    = (cdx > cdz ? cdx : cdz);
    if (car_len_ < 0.05f) car_len_ = 0.05f;
    car_height_ = model.bbox[4] - model.bbox[1];
    if (car_height_ < 0.02f) car_height_ = 0.02f;
    car_long_is_x_ = long_is_x;   // nose axis for the body-render orientation

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
            // A8: re-seed the integrated body basis to the grid heading.
            Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);
            car_ready_ = true;
            // 3 AI opponents as a starting GRID just ahead of the start line, on the
            // unambiguous forward leg of the racing-line spline, all facing the race
            // direction. (The old "scatter at gates 2/4/6" put a car at a spot where the
            // spline passes near itself, so the faithful lookahead's geometric-nearest seed
            // landed on the wrong leg and the car drove backward into a jitter trap.)
            ai_cars_.clear();
            if (gates_.size() > 8) {
                const float* gg0 = gates_[0].center;
                const float* gg1 = gates_[1].center;
                float fx = gg1[0] - gg0[0], fz = gg1[2] - gg0[2];
                const float fl = std::sqrt(fx*fx + fz*fz);
                if (fl > 1e-3f) { fx /= fl; fz /= fl; }
                // Track lateral axis. CONVENTION: perp(f) = (-fz, +fx), the SAME one used by
                // StartRound (:3133 lat = {-dir.z, dir.x}) and the AI lane offset (:2770
                // aim = g + (-tdz, +tdx)*lane). This site used to compute the OPPOSITE
                // perpendicular, (+fz, -fx), which was harmless where consumers are symmetric
                // (+/-0.9 grid sides, lanes -3/0/+3) but NOT here: lat[] below is asymmetric
                // and puts all three cars on ONE side, so the side depended on the odd-one-out
                // convention. Unified 2026-08-16 after the mirror-fix audit flagged it as a
                // trap for any future code that assumes one convention and calls the other.
                //
                // Behaviour is UNCHANGED: the axis is negated and lat[] is negated with it, so
                // every spawn lands on the same world position as before. The side itself is
                // load-bearing and stays put -- it is chosen so the faithful lookahead's
                // geometric-nearest seed lands on the forward leg of the spline (see the
                // comment above), which is a world-space/spline argument, not a visual one.
                const float rx = -fz, rz = fx;               // track "right" = perp(forward)
                const float baseYaw = std::atan2(fz, fx);    // race direction
                const float unit = (car_len_ > 0.05f ? car_len_ : 0.5f);
                const float lane = unit * 1.6f, row = unit * 2.2f;
                const float lat[3] = { -0.7f, -1.7f, -0.7f }; // 2-wide grid on the forward-seeding side
                const float lon[3] = {  1.f,   1.f,  2.4f };  // (front row + one behind), staggered ahead
                for (int i = 0; i < 3; ++i) {
                    AiCar a{};
                    float px = gg0[0] + rx*lane*lat[i] + fx*row*lon[i];
                    float pz = gg0[2] + rz*lane*lat[i] + fz*row*lon[i];
                    bool aok = false; float ay = GroundHeight(px, pz, &aok);
                    if (!aok) { px = gg0[0]; pz = gg0[2]; ay = GroundHeight(px, pz, &aok); }
                    a.pos[0] = px; a.pos[2] = pz;
                    a.pos[1] = (aok ? ay : gg0[1]) + car_ground_off_;
                    a.target = 1;
                    a.speed  = radius_ * (0.10f + 0.02f * static_cast<float>(i));
                    a.yaw    = baseYaw;
                    ai_cars_.push_back(a);
                }
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
    // A8: re-seed the integrated body basis to the spawn heading.
    Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);

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
    // WS-E s2: AI-car liveries are lit by the same track atomic light as the
    // player car (directional sun + ambient on their normal-bearing body panels).
    // WS-E vehicle lighting: relight sources emitted per variant (per-frame
    // world-space sun on the AI bodies) when MASHED_RPLIGHT is on.
    const AtomicLight car_light = rp_light_on_
        ? MakeAtomicLightF(amb_f_, sun_f_, has_sun_dir_, sun_L_)
        : MakeAtomicLight(amb_world_, sun_color_, sun_dir_);
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
        BuildDffBatches(dev, model, dicts, &v.batches, &v.textures, &car_light,
                        rp_light_on_ ? &v.relit : nullptr);
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
    const float kDrag     = 0.21f;          // HARVESTED coast decay (throttle-OFF), weighty
    // Throttle factor: was 0.42 (reached kTop in ~3.3s). CALIBRATED 2026-06-30 against the
    // original via the in-race injector (re/frida/capture_player_dynamics.py, Arctic): the
    // original accelerates to ~top in ~2.2s from GO (63% in ~1s), so the old value was ~1.4x
    // too sluggish. 0.58 reaches kTop in ~2.2s. Raising it ONLY speeds the approach to the
    // clamp — top speed (kTop clamp) and the weighty coast decay (kDrag, throttle-off) are
    // unchanged. Env MASHED_THROTTLE_K overrides (=0.42 reverts). re/analysis/feel_calibration.md.
    static const float s_throttleK = []{ char b[16];
        DWORD n = GetEnvironmentVariableA("MASHED_THROTTLE_K", b, sizeof b);
        float f = n ? (float)atof(b) : 0.58f; return (f > 0.f) ? f : 0.58f; }();
    const float kThrottle = kTop * s_throttleK;
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
        // Drive the player via the SAME faithful racing-line target + robust steer the
        // opponents use (lets a full field run headless for verification). No verbatim
        // bands -> no accel+brake deadlock; the player's own motion path integrates it.
        g_aiTrack = this;   // bind the LOS host before Ai_ComputeTarget
        float tx, tz;
        if (Ai::Ai_ComputeTarget(0, car_pos_[0], car_pos_[2], &tx, &tz)) {
            float dx = tx - car_pos_[0], dz = tz - car_pos_[2];
            float ye = std::atan2(dz, dx) - car_yaw_;
            while (ye >  3.14159265f) ye -= 6.28318531f;
            while (ye < -3.14159265f) ye += 6.28318531f;
            steer = ye / 0.5f;
            if (steer >  1.f) steer =  1.f;
            if (steer < -1.f) steer = -1.f;
            accel = 1.0f;
        }
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
                col_tris_.data(),  static_cast<int>(col_tris_.size()  / 3),
                col_mat_.size() == col_tris_.size() / 3 ? col_mat_.data() : nullptr);
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
        // CORRECTED 2026-08-24: this said "recovered chain-grip heading (+0x9c0)",
        // which named the wrong source. io.yaw is NOT derived from the yaw rate —
        // +0x9c0 is never read in VehiclePhysicsRun.cpp. StepPlayer relaxes io.yaw
        // toward the chain VELOCITY heading, in this same atan2(z,x) convention
        // (VehiclePhysicsRun.cpp:596-603): velHeading = atan2(cvz,cvx);
        // io.yaw += (velHeading - io.yaw) * frac. So this is a velocity-heading
        // follow, not a yaw-rate integral. The distinction matters: a sign bug was
        // predicted on the yaw-rate-integral reading and does not exist — measured
        // 2026-08-24, car_yaw decreases under +steer exactly as the original does
        // (D2_REALPHYS_REMEASURE_2026-08-21.md "A8 standalone arm").
        // A8 (2026-08-25): io.yaw is now the BODY heading read back from the
        // integrated orientation basis (FUN_0046e9e0's omega x R row integration),
        // not a lag chasing the velocity heading. The two can now differ — that
        // difference IS the slip angle, which the old model could not represent.
        car_yaw_ = io.yaw;
        // A8: the POSITION step advances by the VELOCITY VECTOR, as FUN_0046e9e0
        // does (`EBX[0xc..0xe] = EDI[0xc..0xe] + k*dt*ESI[0x26c..0x26e]`, stores at
        // 0x0046ea5f/0x0046ea69/0x0046ea73). io.drive_delta is that increment
        // accumulated over the frame's substeps, already scaled by dt and by the
        // original's own internal->world factor _DAT_005cc948 * _DAT_005cea80.
        // REPLACES `pos += {cos,0,sin(car_yaw_)} * io.drive_speed * dt`, which
        // forced the path to follow the nose and so could not express a slide.
        const float nx = car_pos_[0] + io.drive_delta[0];
        const float nz = car_pos_[2] + io.drive_delta[2];
        bool nok = false;
        const float ngy = GroundHeight(nx, nz, &nok);
        if (nok) { car_pos_[0] = nx; car_pos_[2] = nz; car_pos_[1] = ngy + car_ground_off_; }
        else {
            // [G4][U-A8-OFFTRACK] off-mesh recovery.
            //
            // FIXED 2026-08-25. The old version re-aimed velocity + heading toward a
            // gate and multiplied car_speed_ by 0.6, but it NEVER MOVED THE CAR back
            // onto the drivable surface. So if the re-aim did not by itself clear the
            // boundary, this branch ran again the next frame and the 0.6 COMPOUNDED.
            // Measured in verify/a8_velvec_20260825/tb_A.txt: 4198.28 -> 21.88, which
            // is 0.6^10.3 to three figures — ten consecutive frames of decay with no
            // recovery, after which the race logic respawned the car. That
            // decay-to-respawn loop is what floors the gate-(a) median.
            //
            // It was latent before and became load-bearing at the corrected time
            // base, where the car covers ground 3x faster and reaches the boundary
            // far more often. The bug is the missing relocation, not the time base.
            //
            // Now: RELOCATE FIRST (RecoverOffMesh finds a VERIFIED on-mesh point via
            // FindOnMeshHeading and moves the car there, so the next frame starts on
            // the mesh and this branch cannot re-fire indefinitely), then re-aim
            // toward the gate ahead ONLY IF a short probe in that direction is itself
            // on-mesh — otherwise keep the heading RecoverOffMesh verified.
            RecoverOffMesh();
            if (!gates_.empty()) {
                const int n = static_cast<int>(gates_.size());
                const float* g = gates_[static_cast<std::size_t>((race_[0].gate + 3) % n)].center;  // a few AHEAD
                float dx = g[0] - car_pos_[0], dz = g[2] - car_pos_[2];
                float dl = std::sqrt(dx*dx + dz*dz);
                if (dl > 1e-3f) {
                    // Probe one short step along the gate chord. Same 0.3 step
                    // RecoverOffMesh uses for its own on-mesh verification.
                    bool pok = false;
                    GroundHeight(car_pos_[0] + dx/dl * 0.3f,
                                 car_pos_[2] + dz/dl * 0.3f, &pok);
                    if (pok) {
                        const float sp = car_speed_;      // keep RecoverOffMesh's speed;
                                                          // do NOT damp again here (that
                                                          // double-damp was the compounding)
                        car_vel_[0] = dx/dl * sp; car_vel_[2] = dz/dl * sp;
                        car_yaw_ = std::atan2(dz, dx);
                    }
                }
            }
            // A8: the integrated basis is the authority for the body heading, so any
            // re-aim above must re-seed it. RecoverOffMesh re-seeds for its own case;
            // this covers the gate re-aim that may have overwritten car_yaw_ after it.
            Vehicle::VehiclePhysics_ResetOrientation(0, car_yaw_);
        }
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
        RecoverOffMesh();   // island edge / off-collision: steer back instead of freezing
    }
    }  // end MASHED_REAL_PHYSICS else (scaffold body)
    UpdateRace(in.dt);
    // Option B (2026-06-30): opponents drive the FAITHFUL racing-line target
    // (Ai_ComputeTarget = SelectSpline + ported FUN_00443dc0 lookahead) with a robust
    // turn-rate + velocity-shaped-speed motion model, by default once the .AI banks load
    // (re/analysis/ai_spline_lookahead.md). This races the real line without the verbatim
    // ControlStep bands' accel+brake deadlock against the approximate physics chain. The
    // gate-ribbon scaffold is the fallback (no .AI, or MASHED_GATE_RIBBON_AI=1).
    const int ng = static_cast<int>(gates_.size());
    const bool faithful_nav = g_aib.loaded && (Ai::I32(Ai::kSplineRaceCnt) > 3);
    if (faithful_nav) {
        g_aiTrack = this;   // bind the LOS host (aib_los_clear -> GroundHeight)
        for (int ci = 0; ci < static_cast<int>(ai_cars_.size()); ++ci) {
            AiCar& a = ai_cars_[static_cast<std::size_t>(ci)];
            const int v = ci + 1;
            if (round_mode_ && !race_[ci + 1].alive) {
                a.cur_speed = 0.f; a.vel[0] = a.vel[1] = a.vel[2] = 0.f; continue;
            }
            if (a.spin > 0.f) {   // spun out by a missile/mine: spin in place
                a.spin -= in.dt; a.yaw += 12.0f * in.dt;
                a.cur_speed = 0.f; a.vel[0] = a.vel[1] = a.vel[2] = 0.f; continue;
            }
            if (a.slow > 0.f) a.slow -= in.dt;
            // anti-stuck marshal recovery: if a car's gate progress stalls (a lookahead
            // corner-case can pin it at a track feature the racing line skirts), relocate it
            // to the next gate ahead on the correctly-ordered ribbon, facing forward, at
            // cruise speed. Robust by construction — every trigger advances it +2 gates, so
            // no car can stay stuck. (re/analysis/ai_spline_lookahead.md)
            if (ng >= 2) {
                if (race_[v].gate != a.prog_gate) { a.prog_gate = race_[v].gate; a.stuck_t = 0.f; }
                else { a.stuck_t += in.dt; }
                if (a.stuck_t > 2.5f) {
                    const int gA = (race_[v].gate + 2) % ng;
                    const int gB = (race_[v].gate + 3) % ng;
                    const float* gp = gates_[static_cast<std::size_t>(gA)].center;
                    bool rok = false; const float gy2 = GroundHeight(gp[0], gp[2], &rok);
                    if (rok) {
                        a.pos[0] = gp[0]; a.pos[2] = gp[2]; a.pos[1] = gy2 + car_ground_off_;
                        a.yaw = std::atan2(gates_[static_cast<std::size_t>(gB)].center[2] - gp[2],
                                           gates_[static_cast<std::size_t>(gB)].center[0] - gp[0]);
                        a.cur_speed = a.speed * 0.6f;
                    }
                    a.stuck_t = 0.f;
                }
            }
            // "where to go": the faithful racing-line lookahead target (falls back to the
            // gate ribbon point if the bank is momentarily empty).
            float tx, tz;
            if (!Ai::Ai_ComputeTarget(v, a.pos[0], a.pos[2], &tx, &tz)) {
                if (ng < 1) continue;
                const float* g = gates_[static_cast<std::size_t>(a.target) % ng].center;
                tx = g[0]; tz = g[2];
            }
            // forward-progress safety net: where the racing-line spline passes near itself
            // (e.g. by the start/finish), the geometric-nearest seed can land on the wrong
            // leg and the lookahead points BACKWARD, sending the car the wrong way into a
            // seam jitter-trap. The gate ribbon (race_[v].gate) is correctly ordered, so if
            // the target opposes the local race direction, retarget the next gate ahead.
            if (ng >= 2) {
                const int gi = ((race_[v].gate % ng) + ng) % ng;
                const int gj = (gi + 1) % ng;
                const float gfx = gates_[(std::size_t)gj].center[0] - gates_[(std::size_t)gi].center[0];
                const float gfz = gates_[(std::size_t)gj].center[2] - gates_[(std::size_t)gi].center[2];
                const float tdx0 = tx - a.pos[0], tdz0 = tz - a.pos[2];
                const float gfl = std::sqrt(gfx*gfx + gfz*gfz);
                const float tdl = std::sqrt(tdx0*tdx0 + tdz0*tdz0);
                if (gfl > 1e-3f && tdl > 1e-3f &&
                    (gfx*tdx0 + gfz*tdz0) / (gfl * tdl) < -0.25f) {   // target >~105deg off race dir
                    tx = gates_[(std::size_t)gj].center[0];
                    tz = gates_[(std::size_t)gj].center[2];
                }
            }
            // "how to move": steer toward the target (turn-rate) + velocity-shaped speed,
            // scrubbing for a sharp heading error but FLOORED at 45% cruise so the car
            // never fully stops (the verbatim bands' brake-latch deadlock cannot occur).
            float dx = tx - a.pos[0], dz = tz - a.pos[2];
            float yerr = std::atan2(dz, dx) - a.yaw;
            while (yerr >  3.14159265f) yerr -= 6.28318531f;
            while (yerr < -3.14159265f) yerr += 6.28318531f;
            a.yaw += yerr * (6.0f * in.dt > 1.f ? 1.f : 6.0f * in.dt);
            float ae = (yerr < 0.f ? -yerr : yerr);
            float align = 1.f - ae / 1.5707963f;      // 1 aligned -> 0 at >=90deg off
            if (align < 0.f) align = 0.f;
            float tgt = a.speed * (0.45f + 0.55f * align);
            if (a.slow > 0.f) tgt *= 0.35f;            // shock power-up: throttled
            a.cur_speed += (tgt - a.cur_speed) * (2.5f * in.dt > 1.f ? 1.f : 2.5f * in.dt);
            const float nx2 = a.pos[0] + std::cos(a.yaw) * a.cur_speed * in.dt;
            const float nz2 = a.pos[2] + std::sin(a.yaw) * a.cur_speed * in.dt;
            bool aok = false;
            const float ay = GroundHeight(nx2, nz2, &aok);
            if (aok) { a.pos[0] = nx2; a.pos[2] = nz2; a.pos[1] = ay + car_ground_off_; }
            else {
                // off-mesh step: the racing-line target leads off the drivable surface here.
                // Just redirecting the heading (without moving) left the car frozen facing a
                // wall. Ring-probe for an on-mesh heading near the target bearing and COMMIT a
                // nudge — exactly as the player's RecoverOffMesh does — so the car follows the
                // edge back onto the line instead of pinning. (re/analysis/ai_spline_lookahead.md)
                const float want = std::atan2(dz, dx);
                float ry; float rad = 0.f;
                if      (FindOnMeshHeading(a.pos[0], a.pos[2], want, 1.2f, ry)) rad = 1.2f;
                else if (FindOnMeshHeading(a.pos[0], a.pos[2], want, 0.5f, ry)) rad = 0.5f;
                const float fl = a.speed * 0.4f;
                a.cur_speed = (a.cur_speed * 0.5f > fl) ? a.cur_speed * 0.5f : fl;
                if (rad > 0.f) {
                    float mx = a.pos[0] + std::cos(ry) * 0.3f;
                    float mz = a.pos[2] + std::sin(ry) * 0.3f;
                    bool mok = false; float my = GroundHeight(mx, mz, &mok);
                    if (!mok) {   // the small nudge landed in a gap; jump to the verified ring
                        mx = a.pos[0] + std::cos(ry) * rad;     // point (on-mesh by construction)
                        mz = a.pos[2] + std::sin(ry) * rad;     // so the car leaves the island.
                        my = GroundHeight(mx, mz, &mok);
                    }
                    if (mok) { a.pos[0] = mx; a.pos[2] = mz; a.pos[1] = my + car_ground_off_; }
                    a.yaw = ry;
                }
            }
            a.vel[0] = std::cos(a.yaw) * a.cur_speed;
            a.vel[1] = 0.f;
            a.vel[2] = std::sin(a.yaw) * a.cur_speed;
        }
        // diagnostic (env MASHED_AI_DIAG -> mashed_re.log, ~every 30 frames): pos/yaw/speed
        // so a stalled car's trajectory is visible. Read-only (does NOT call
        // Ai_ComputeTarget, which would double-update the continuity index this frame).
        {
            static const bool s_aidiag = (std::getenv("MASHED_AI_DIAG") != nullptr);
            static int s_aidn = 0;
            if (s_aidiag && (s_aidn % 30) == 0) {
                if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                    std::fprintf(lf, "AI-DIAG f=%d splineCnt=%d", s_aidn, Ai::I32(Ai::kSplineRaceCnt));
                    for (int ci = 0; ci < static_cast<int>(ai_cars_.size()) && ci < 3; ++ci) {
                        const AiCar& a = ai_cars_[(std::size_t)ci];
                        std::fprintf(lf, " | v%d spd=%.1f pos=(%.1f,%.1f) yaw=%.2f",
                                     ci + 1, a.cur_speed, a.pos[0], a.pos[2], a.yaw);
                    }
                    std::fprintf(lf, "\n");
                    std::fclose(lf);
                }
            }
            ++s_aidn;
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
    // DEACT/teardown leaf (orig per-type DEACTs entry+0x10: GUN 0x4566f0 lock-
    // indicator RwFrameRemoveChild, MISSILE 0x455390, OIL 0x456e00, MORTAR 0x453630
    // detach; R_FLAME 0x45a8b0 stop-FX FUN_004661a0(0x16); DRUM/P_MINE/SHOTGUN/FLASH
    // flip a pool state byte). The spike has no on-car weapon attachment model, so
    // the visible teardown is a brief dissipation puff at the firing car, coloured
    // per weapon to match its FIRE FX; the dropped hazards/projectiles persist on
    // their own life timers (matching the original — DEACT only detaches the on-car
    // model, never the in-flight instances).
    void EffectEnd(int code, const float pos[3]) override {
        const float r = R();
        float sp[3] = { pos[0], pos[1] + r * 0.02f, pos[2] };
        std::uint32_t col; int n; float spd, sz;
        switch (code) {
            case Powerup::kGun:     col = 0xc0ffe080u; n = 8;  spd = r*0.04f; sz = r*0.008f; break;
            case Powerup::kMissile: col = 0xc0ff8020u; n = 10; spd = r*0.05f; sz = r*0.010f; break;
            case Powerup::kMortar:  col = 0xc0ff6020u; n = 10; spd = r*0.05f; sz = r*0.012f; break;
            case Powerup::kRFlame:  col = 0xa0ff8020u; n = 14; spd = r*0.06f; sz = r*0.012f; break;
            case Powerup::kShotgun: col = 0xc0ffe040u; n = 8;  spd = r*0.05f; sz = r*0.010f; break;
            case Powerup::kFlash:   col = 0xa0ffffffu; n = 12; spd = r*0.07f; sz = r*0.010f; break;
            case Powerup::kOil:     // detaches drip + drops the per-owner trail
                t_->pu_oil_has_ = false;
                col = 0xa0403020u; n = 6; spd = r*0.03f; sz = r*0.012f; break;
            case Powerup::kDrum:
            case Powerup::kPMine:
            default:                col = 0xa0c0c0c0u; n = 6;  spd = r*0.04f; sz = r*0.010f; break;
        }
        t_->parts_.SpawnBurst(sp, n, col, spd, sz, 0.35f);
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
    // U-9005 RESOLVED (rules oracle 2026-07-02, log/rules_oracle_rule8.json
    // round-resets=1): the original DOES re-init the finish order on the
    // per-round restart path (order [0,-1,-1,-1] observed reset to all -1.0f
    // across a round cycle with no track reload). Mirror FUN_00414060's
    // slot reset per round. The rule-10 countdown is NOT re-seeded per round
    // (3 back-to-back expiry rounds observed with a stale timer) — leave it.
    for (int s = 0; s < Race::RuleEngine::kCars; ++s)
        rulep_.finishOrder[s] = Race::RuleEngine::kEmptySlot;
    // [D-11056] re-arm the rule-5 collectible feed each round (collectTotal
    // is fixed per-track by SetRaceRule; only the per-round done state resets).
    rulep_.collectDone = 0;
    for (auto& c : copters_) c.done = false;
    // F4: reset the race clock + the player's per-lap split records.
    race_time_ = 0.f;
    for (int k = 0; k < kMaxSplits; ++k) { split_time_[k] = 0.f; split_done_[k] = false; }
    // grid: the ORIGINAL's staggered zig-zag formation, ported verbatim from
    // FUN_00408b00 (0x00408b00, "race grid start-position calculator"; decode in
    // verify/grid_orig_ts/FUN_00408b00_decomp.txt). The prior 2x2 box was
    // invented (see :80 "instead of the original's AI tile grid") and did not
    // match the original — measured 2026-08-30, the original spawns a single-file
    // 4-rank stagger, frozen on the grid through the countdown (byte-identical
    // t=0.121..6.910s, so this IS the grid, not roll-away; verify/grid_orig_ts/).
    //
    // Original per-slot (param_3==4): pos_s = A + F*fwdOff[s] - L*latOff[s], with
    //   A = FUN_00426cb0() anchor, F = normalize(node0-node3), L = FUN_00426cc0()
    //   fwdOff = 0.8*(1-/+0.4) = {+0.48,-1.12,+1.12,-0.48}   (_cc9bc, _ccac0)
    //   latOff = {0, 1.1, 2.2, 3.3}                          (_ccabc/ab8/ab4)
    // The standalone start-line frame maps 1:1: gate0 == A (both ~(0.03,·,-2.0)),
    // lat == F, dir == L (dir~=(0,0,-1), lat~=(1,0,0), yaw=-1.576). So:
    //   pos_s = g0 + lat*latMul[s] - dir*dirMul[s]
    // reproduces the original grid to <=0.02 in x/z. See verify/grid_orig_ts/
    // ANALYSIS.md for the algebra and the measured-vs-formula check.
    const float* g0 = gates_[0].center;
    const float* g1 = gates_[1].center;
    float dir[2] = {g1[0] - g0[0], g1[2] - g0[2]};
    const float dl = std::sqrt(dir[0]*dir[0] + dir[1]*dir[1]);
    dir[0] /= dl; dir[1] /= dl;
    const float lat[2] = {-dir[1], dir[0]};
    // slot == spawn order: player=0, ai0=1, ai1=2, ai2=3
    static const float kLatMul[4] = { 0.48f, -1.12f, 1.12f, -0.48f };
    static const float kDirMul[4] = { 0.00f,  1.10f, 2.20f,  3.30f };
    auto place = [&](float* pos, float* yaw, int slot) {
        const float x = g0[0] + lat[0] * kLatMul[slot] - dir[0] * kDirMul[slot];
        const float z = g0[2] + lat[1] * kLatMul[slot] - dir[1] * kDirMul[slot];
        bool ok = false;
        const float y = GroundHeight(x, z, &ok);
        pos[0] = x; pos[2] = z;
        pos[1] = (ok ? y : g0[1]) + car_ground_off_;
        *yaw = std::atan2(dir[1], dir[0]);
    };
    place(car_pos_, &car_yaw_, 0);
    car_vel_[0] = car_vel_[1] = car_vel_[2] = 0.f;
    car_speed_ = 0.f;
    ai_cars_.assign(3, AiCar{});
    for (int i = 0; i < 3; ++i) {
        AiCar& a = ai_cars_[static_cast<std::size_t>(i)];
        place(a.pos, &a.yaw, i + 1);   // ai i -> slot i+1
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

// [D-11052] arm the per-rule win-condition engine for the race that is about
// to start. rule = the real DAT_007f0fd0 race rule (0..10) derived by
// Race/RaceModes. MASHED_RULE_ENGINE=0 reverts to the legacy two-objective
// collapse. Rule 10 seeds its countdown + checkpoint bonus from the course id
// (FUN_004046a0; difficulty _DAT_0089a360 fed as 0.0 — the fresh-race default
// before the FUN_00414060 rubber-band seed, which is not modeled standalone).
// Finish order / timers reset here (FUN_00414060 runs on the race-SETUP path:
// FUN_0042c960 -> FUN_004111c0 -> FUN_00414060). (U-9005 RESOLVED 2026-07-02:
// the original ALSO re-inits the finish order per round restart — witnessed
// live, rules oracle round-resets=1 — so StartRound() now mirrors that.)
void TrackRenderer::SetRaceRule(int rule) {
    rule_ = rule;
    const char* e = std::getenv("MASHED_RULE_ENGINE");
    rule_engine_on_ = !(e && e[0] == '0');
    rulep_ = Race::RuleEngine::Persist{};             // FUN_00414060: slots=-1.0
    rulep_.timer = Race::RuleEngine::Rule10Seed(course_id_, 0.f, &rule10_bonus_);
    rule10_hit_mask_ = 0;
    rule10_lap_seen_ = -1;
    match_draw_ = false;
    // [D-11056] rule-5 collectible feed (U-8997 RESOLVED 2026-07-04): total =
    // the KTC_NewCopter gameplay copters loaded for this track (registrar
    // FUN_00405780's per-object DAT_0063a5d0 increments, one per KTC_NewCopter
    // command; confirmed disjoint from KTC_AddPickUp's separate FUN_00405730
    // registrar). Each copter reports done via OnCollect() on completing one
    // flight-path traversal (UpdateRace), mirroring FUN_004064c0's per-object
    // DAT_0063a5d4 completion tick. See
    // re/analysis/d11056_fable_ghidra_findings_2026-07-03.md.
    {
        int collect_total = 0;
        for (auto& c : copters_) {
            c.done = false;
            if (c.gameplay) ++collect_total;
        }
        SetCollectibles(collect_total);
    }
    // Fallback: a track assigned rule 5 with zero KTC_NewCopter copters could
    // never satisfy CollectAllDone (0x00405890: total==0 -> false), stalling
    // the round forever. Disable the rule engine for THIS race only (per-race
    // flag; the rule_engine_on_ master latch is untouched) so the legacy
    // laps/elimination collapse in UpdateRace terminates rounds instead.
    // Re-armed true here on every new race.
    rule_engine_race_on_ = true;
    if (rule_ == 5 && rulep_.collectTotal == 0) {
        rule_engine_race_on_ = false;
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::fprintf(lf, "rule-5: track has no KTC_NewCopter collectibles -- "
                             "legacy flow fallback\n");
            std::fclose(lf);
        }
    }
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
        // [G4] Robust forward-projection progress (replaces the strict 3.0-unit proximity
        // to gate[r.gate], which the AI cars — following the race-line SPLINE, offset from
        // the gate ribbon — often never satisfied, so r.gate stalled and no lap/race ever
        // completed). Project the car onto the NEAREST gate within a forward window and
        // advance r.gate through the intervening gates (forward only — no shortcut/backward
        // jumps), running the same lap-line + split logic on each gate left behind. A lap
        // completes on the LAPDATA lap-line set (FUN_00426cf0 anti-shortcut) when present,
        // else on a start/finish wrap (gate n-1 -> 0).
        const int W = 15;                        // forward search window (gates)
        int best = r.gate; float bestD = 1e18f;
        for (int k = 0; k <= W; ++k) {
            int gi = (r.gate + k) % n;
            const float* gg = gates_[static_cast<std::size_t>(gi)].center;
            const float ex = gg[0] - pos[0], ez = gg[2] - pos[2];
            const float ed = ex * ex + ez * ez;
            if (ed < bestD) { bestD = ed; best = gi; }
        }
        int guard = 0;
        while (r.gate != best && guard++ < W + 2) {
            const int G = r.gate;                // forward advance onto G+1
            bool lapped = false;
            if (!lap_data_.lap_lines.empty()) {
                if (Track::LapLineStep(G, lap_data_.lap_lines, r.lap_mask)) { ++r.laps; lapped = true; }
            } else if (((G + 1) % n) == 0) {      // wrap fallback (no LAPDATA lap lines)
                ++r.laps; lapped = true;
            }
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
        const float* gc = gates_[static_cast<std::size_t>(r.gate)].center;
        const float dx = gc[0] - pos[0], dz = gc[2] - pos[2];
        const float d = std::sqrt(dx * dx + dz * dz);
        const float frac = (d > 8.f) ? 0.f : (1.f - d / 8.f);
        r.progress = static_cast<float>(r.laps * n + r.gate) + frac;
    };
    step(0, car_pos_);
    for (int i = 0; i < 3 && i < static_cast<int>(ai_cars_.size()); ++i)
        step(i + 1, ai_cars_[static_cast<std::size_t>(i)].pos);

    // [G4] lap-progress diagnostic (env MASHED_LAP_DIAG -> mashed_re.log, ~1/s).
    {
        static const bool s_lapdiag = (std::getenv("MASHED_LAP_DIAG") != nullptr);
        static int s_lf = 0;
        if (s_lapdiag && (++s_lf % 60) == 0) {
            if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
                std::fprintf(lf, "LAP-DIAG round=%d mode=%d t=%.1f gates=%d laptgt=%d | "
                    "p0 gate=%d lap=%d prog=%.1f | a1 gate=%d lap=%d | a2 gate=%d lap=%d | a3 gate=%d lap=%d\n",
                    round_mode_ ? 1 : 0, race_mode_, race_time_, n, lap_target_,
                    race_[0].gate, race_[0].laps, race_[0].progress,
                    race_[1].gate, race_[1].laps, race_[2].gate, race_[2].laps,
                    race_[3].gate, race_[3].laps);
                std::fclose(lf);
            }
        }
    }

    if (!round_mode_) return;   // race rules only run during a match (both modes
                                // update the shared camera below).

    // [D-11056] rule-5 collectible feed: a KTC_NewCopter gameplay copter
    // "collects" by completing one traversal of its bound flight path —
    // mirrors FUN_004064c0's per-object completion tick (unconditional on
    // rule, like the original; only rule 5's predicate reads collectDone).
    for (auto& c : copters_) {
        if (!c.gameplay || c.done) continue;
        if (c.anim.duration > 0.f && race_time_ >= c.anim.duration) {
            c.done = true;
            OnCollect();
        }
    }

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

    // ---- [D-11052] per-rule win-condition engine (Race/RuleEngine): the
    // FUN_004177b0 metric/finish-order update, the FUN_00410d10 segment check
    // (per-rule pre-blocks + elimination gating + last-man logic) and — on
    // segment end — the FUN_00410510 result evaluation, following the caller
    // contract of FUN_00411170 (result != 0 -> match over; == 0 -> next
    // round). MASHED_RULE_ENGINE=0 reverts to the legacy collapse below.
    if (rule_engine_on_ && rule_engine_race_on_) {   // [D-11056] per-race gate
        if (match_winner_ >= 0) return;        // result declared (FUN_00443080)
        namespace RE = Race::RuleEngine;
        RE::Cars rc;
        rc.participants = kRaceCars;           // DAT_008a94d0 (standalone round = 4)
        for (int i = 0; i < kRaceCars; ++i) {
            rc.active[i] = (P2[i] != nullptr);
            rc.alive[i]  = race_[i].alive && rc.active[i];
            rc.score[i]  = scores_[i];
            // DAT_0089a880[i] = lapCounter + lapFrac% * 0.01 (FUN_004177b0).
            const float pct = std::fmod(race_[i].progress, static_cast<float>(n))
                              / static_cast<float>(n);
            rc.metric[i] = static_cast<float>(race_[i].laps) + pct;
        }
        RE::UpdateFinishOrder(rule_, rc, rulep_);

        // Rule-10 countdown (FUN_004039f0): -= dt; checkpoint award once per
        // lap-line gate per lap (flag reset when the lap counter advances).
        if (rule_ == 10 && countdown_ <= 0.f) {
            RE::Rule10Tick(rulep_, dt);
            if (race_[0].laps != rule10_lap_seen_) {
                rule10_lap_seen_ = race_[0].laps;
                rule10_hit_mask_ = 0;
            }
            if (rule10_bonus_ > 0.f) {
                int li = 0;
                for (int g : lap_data_.lap_lines) {
                    const std::uint32_t bit = 1u << li++;
                    // width-1.0 window (0x004039f0.md step 8: *piVar3 <=
                    // fVar5 < *piVar3 + 1.0). Fast multi-gate advance
                    // (>1 gate/tick) can skip it — exactly as the original's
                    // sampled window can; do NOT add crossing-tracking.
                    if ((rule10_hit_mask_ & bit) == 0 && race_[0].gate > g &&
                        race_[0].gate <= g + 1) {
                        rule10_hit_mask_ |= bit;
                        rulep_.timer += rule10_bonus_;
                    }
                    if (li >= 6) break;             // 6-entry table (0x004046a0)
                }
            }
        }

        if (round_winner_ >= 0) return;        // round-end hold (results delay)
        bool runElim = false;
        const int seg = RE::SegmentCheck(rule_, rc, rulep_,
                                         /*resultDeclared=*/false, &runElim);
        if (runElim && countdown_ <= 0.f) {
            const int victim = race_cam_.EliminationCheck(cc);
            if (victim >= 0 && race_[victim].alive) {
                race_[victim].alive = false;
                --round_alive_;
                if (victim > 0 && victim - 1 < static_cast<int>(ai_cars_.size()))
                    ai_cars_[static_cast<std::size_t>(victim - 1)].speed = 0.f;
                ScoreOnElimination(victim);    // FUN_0040eee0(victim, 1)
            }
        }
        // re-snapshot alive flags AND scores after any elimination this tick:
        // the original's elimination scoring (FUN_0040eee0) runs INSIDE the
        // segment check and FUN_00410510 then reads the live DAT_008a94e0
        // scores in the same tick (0x00410d10.md l.56; FUN_00410510.md
        // Phase 1) — EvaluateResult must not see the pre-elimination snapshot.
        for (int i = 0; i < kRaceCars; ++i) {
            rc.alive[i] = race_[i].alive && rc.active[i];
            rc.score[i] = scores_[i];          // ScoreOnElimination -> ScoreAward
        }
        if (seg == 1 ||
            RE::SegmentCheck(rule_, rc, rulep_, false, &runElim) == 1) {
            bool p0 = false;
            const int r = RE::EvaluateResult(rule_, rc, rulep_, &p0);
            if (r > 0) {                       // match over, winner r-1
                match_winner_ = r - 1;
            } else if (r == -1) {              // over with no winner (draw /
                match_winner_ = 0;             // player failed); results text
                match_draw_ = true;            // shows RACE OVER via match_draw
            } else {                           // r == 0: next round
                for (int i = 0; i < kRaceCars; ++i)
                    if (race_[i].alive) { round_winner_ = i; break; }
                if (round_winner_ < 0) round_winner_ = 0;  // all dead: equalized
            }
        }
        return;
    }

    // ---- legacy two-objective collapse (MASHED_RULE_ENGINE=0) -------------
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

namespace {
// ===========================================================================
// WS-A s3 PERF: persistent vertex-buffer cache for STATIC geometry batches.
//
// Profiling (MASHED_RENDER_PROF, Arctic + car + 3 AI liveries) attributed the
// in-race CPU frame as: cars 45.8ms, world 12.2ms, props 1.5ms, particles
// 0.4ms — i.e. the cost is the per-frame DrawPrimitiveUP calls, dominated by
// the 4 cars + wheels (each part = its own UP call). DrawPrimitiveUP is the
// deprecated user-pointer path: every call re-uploads the vertices into a
// driver scratch buffer and can stall the pipeline. These batches are STATIC
// (built at Load, never mutated during Render; cars/wheels only change their
// WORLD matrix), so we upload each once into a D3DPOOL_MANAGED vertex buffer
// and draw it with DrawPrimitive — same vertices, same order, same textures,
// identical pixels, but no per-frame re-upload.
//
// Keyed by the batch vector's data() pointer (stable per track session). Reused
// batches (e.g. car_batches_ drawn once per AI car under different transforms)
// share one VB. Invalidated on track (re)load so a recycled address can't draw
// stale geometry. Only the STATIC passes use this; the dynamic billboard passes
// (particles/pickups, rebuilt every frame) keep DrawPrimitiveUP.
// ===========================================================================
constexpr DWORD kBatchFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;  // == TrackRenderer::kFVF
struct CachedVB { IDirect3DVertexBuffer9* vb = nullptr; UINT verts = 0; };
std::unordered_map<const void*, CachedVB> g_vbCache;

// WS-E vehicle lighting: shared dynamic VB for the per-frame relit vehicle
// batches. ALL relit geometry of ALL car instances is packed into ONE
// Lock(DISCARD)/upload per frame — per-batch UP calls were the measured
// in-race hotspot (profiling note above), so the relit path must not
// reintroduce them.
IDirect3DVertexBuffer9* g_rlVb    = nullptr;
UINT                    g_rlVbCap = 0;   // capacity in vertices

void InvalidateBatchCache() {
    for (auto& kv : g_vbCache) if (kv.second.vb) kv.second.vb->Release();
    g_vbCache.clear();
    if (g_rlVb) { g_rlVb->Release(); g_rlVb = nullptr; g_rlVbCap = 0; }
}

// Upload the packed relit vertices; returns false on failure (callers fall
// back to DrawPrimitiveUP from the CPU copy).
bool UploadRelit(IDirect3DDevice9* dev,
                 const std::vector<TrackRenderer::V>& verts) {
    if (verts.empty()) return false;
    const UINT nv = (UINT)verts.size();
    if (!g_rlVb || g_rlVbCap < nv) {
        if (g_rlVb) { g_rlVb->Release(); g_rlVb = nullptr; }
        UINT cap = g_rlVbCap ? g_rlVbCap : 4096;
        while (cap < nv) cap *= 2;
        if (FAILED(dev->CreateVertexBuffer(
                cap * (UINT)sizeof(TrackRenderer::V),
                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, kBatchFVF,
                D3DPOOL_DEFAULT, &g_rlVb, nullptr)) || !g_rlVb) {
            g_rlVb = nullptr; g_rlVbCap = 0;
            return false;
        }
        g_rlVbCap = cap;
    }
    void* dst = nullptr;
    if (FAILED(g_rlVb->Lock(0, nv * (UINT)sizeof(TrackRenderer::V), &dst,
                            D3DLOCK_DISCARD)) || !dst)
        return false;
    std::memcpy(dst, verts.data(), nv * sizeof(TrackRenderer::V));
    g_rlVb->Unlock();
    return true;
}

// Draw one static batch via its cached VB (created on first use). Falls back to
// DrawPrimitiveUP if VB creation/lock fails. The caller sets FVF / texture /
// world transform exactly as before — only the vertex SOURCE changes.
void DrawBatch(IDirect3DDevice9* dev, const std::vector<TrackRenderer::V>& b) {
    if (b.empty()) return;
    const UINT nv = (UINT)b.size();
    auto it = g_vbCache.find(b.data());
    if (it == g_vbCache.end() || it->second.verts != nv) {
        if (it != g_vbCache.end()) {
            if (it->second.vb) it->second.vb->Release();
            g_vbCache.erase(it);
        }
        const UINT bytes = nv * (UINT)sizeof(TrackRenderer::V);
        IDirect3DVertexBuffer9* vb = nullptr;
        if (SUCCEEDED(dev->CreateVertexBuffer(bytes, D3DUSAGE_WRITEONLY, kBatchFVF,
                                              D3DPOOL_MANAGED, &vb, nullptr)) && vb) {
            void* dst = nullptr;
            if (SUCCEEDED(vb->Lock(0, bytes, &dst, 0)) && dst) {
                std::memcpy(dst, b.data(), bytes);
                vb->Unlock();
                it = g_vbCache.emplace(b.data(), CachedVB{ vb, nv }).first;
            } else {
                if (vb) vb->Release();
                dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nv / 3, b.data(),
                                     sizeof(TrackRenderer::V));
                return;
            }
        } else {
            dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, nv / 3, b.data(),
                                 sizeof(TrackRenderer::V));
            return;
        }
    }
    dev->SetStreamSource(0, it->second.vb, 0, (UINT)sizeof(TrackRenderer::V));
    dev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nv / 3);
}
}  // namespace

// WS-E vehicle lighting: relit car draw pass. Mirrors the legacy car section
// below (player body -> player wheels -> AI cars [-> AI wheels]) with the
// SAME draw order, matrices, textures and TSS wraps; only the vertex SOURCE
// of runtime-lit batches changes — their diffuse is recomputed for this
// frame's instance orientation (sun in model space, RelightAppend) and packed
// into one dynamic-VB upload. Static batches (glass, prelit) keep the cached
// VBs. RW ground truth: the original lights atomics by their CURRENT frame
// LTM every time the pipeline runs — a load-time bake freezes the shading to
// the body, which is wrong the moment the car yaws.
void TrackRenderer::RenderCarsRelit(IDirect3DDevice9* dev,
                                    const D3DMATRIX& worldm) {
    // pass 1: transforms + packed relight + draw list (scratch reused)
    static std::vector<V>         rl_verts;
    static std::vector<D3DMATRIX> rl_xf;
    struct Draw { const std::vector<V>* stat; UINT first, count;
                  IDirect3DTexture9* tex; int xf; bool tss_wrap; };
    static std::vector<Draw>      rl_draws;
    rl_verts.clear(); rl_xf.clear(); rl_draws.clear();

    auto emit = [&](const std::vector<V>& baked,
                    const std::vector<RelitSrc>* src, int xf,
                    IDirect3DTexture9* tex, bool tss_wrap) {
        if (baked.empty()) return;
        if (src && !src->empty()) {
            const D3DMATRIX& m = rl_xf[static_cast<std::size_t>(xf)];
            // L_model[i] = row_i(R) . L_world (row-vector convention; the
            // instance rotations are orthonormal, so n.Lm == n_world.L_world)
            const float Lm[3] = {
                m._11*sun_L_[0] + m._12*sun_L_[1] + m._13*sun_L_[2],
                m._21*sun_L_[0] + m._22*sun_L_[1] + m._23*sun_L_[2],
                m._31*sun_L_[0] + m._32*sun_L_[1] + m._33*sun_L_[2] };
            const UINT first = (UINT)rl_verts.size();
            RelightAppend(rl_verts, baked, *src, Lm);
            rl_draws.push_back({nullptr, first, (UINT)baked.size(), tex, xf,
                                tss_wrap});
        } else {
            rl_draws.push_back({&baked, 0, 0, tex, xf, tss_wrap});
        }
    };

    // player body — matrix identical to the legacy section (WS-E s3)
    D3DMATRIX carm;
    MatIdentity(&carm);
    {
        const float cy = std::cos(car_yaw_), sy2 = std::sin(car_yaw_);
        if (car_long_is_x_) {
            carm._11 =  cy;  carm._13 = sy2;
            carm._31 = -sy2; carm._33 = cy;
        } else {
            carm._31 =  cy;  carm._33 = sy2;
            carm._11 = sy2;  carm._13 = -cy;
        }
        carm._41 = car_pos_[0]; carm._42 = car_pos_[1]; carm._43 = car_pos_[2];
    }
    rl_xf.push_back(carm);
    for (std::size_t mi = 0; mi < car_batches_.size(); ++mi)
        emit(car_batches_[mi],
             mi < car_relit_.size() ? &car_relit_[mi] : nullptr,
             0, car_textures_[mi], true);

    // player wheels: spin (+steer on the front pair) under carm
    for (const auto& w : wheels_) {
        D3DMATRIX local;
        MatIdentity(&local);
        const float c = std::cos(wheel_spin_), s = std::sin(wheel_spin_);
        if (w.lateral_is_x) {
            local._22 = c; local._23 = s; local._32 = -s; local._33 = c;
        } else {
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
        rl_xf.push_back(wm);
        const int xf_w = static_cast<int>(rl_xf.size()) - 1;
        for (std::size_t pi = 0; pi < w.parts.size(); ++pi)
            emit(w.parts[pi].second,
                 pi < w.parts_relit.size() ? &w.parts_relit[pi] : nullptr,
                 xf_w, car_textures_[w.parts[pi].first], true);
    }

    // AI cars: livery variant when loaded, else the shared body + wheels
    for (const auto& a : ai_cars_) {
        const std::size_t li = static_cast<std::size_t>(&a - ai_cars_.data());
        const CarVariant* var =
            (li < car_variants_.size() &&
             !car_variants_[li].batches.empty()) ? &car_variants_[li]
                                                 : nullptr;
        D3DMATRIX am;
        MatIdentity(&am);
        const float ac = std::cos(a.yaw), as = std::sin(a.yaw);
        if (car_long_is_x_) {
            am._11 =  ac; am._13 = as;
            am._31 = -as; am._33 = ac;
        } else {
            am._31 =  ac; am._33 = as;
            am._11 =  as; am._13 = -ac;
        }
        am._41 = a.pos[0]; am._42 = a.pos[1]; am._43 = a.pos[2];
        rl_xf.push_back(am);
        const int xf_a = static_cast<int>(rl_xf.size()) - 1;
        const auto& bats = var ? var->batches : car_batches_;
        const auto& texs = var ? var->textures : car_textures_;
        const auto& rel  = var ? var->relit    : car_relit_;
        for (std::size_t mi = 0; mi < bats.size(); ++mi)
            emit(bats[mi], mi < rel.size() ? &rel[mi] : nullptr,
                 xf_a, texs[mi], true);
        if (var) continue;   // variant models carry their wheels baked in
        for (const auto& w : wheels_) {
            D3DMATRIX local;
            MatIdentity(&local);
            const float c2 = std::cos(wheel_spin_), s2 = std::sin(wheel_spin_);
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
            rl_xf.push_back(wm2);
            const int xf_w2 = static_cast<int>(rl_xf.size()) - 1;
            // legacy AI wheels do not TSS-wrap null textures — keep that.
            for (std::size_t pi = 0; pi < w.parts.size(); ++pi)
                emit(w.parts[pi].second,
                     pi < w.parts_relit.size() ? &w.parts_relit[pi] : nullptr,
                     xf_w2, car_textures_[w.parts[pi].first], false);
        }
    }

    // pass 2: one upload, then draw in the recorded (legacy) order
    const bool vb_ok = UploadRelit(dev, rl_verts);
    int cur_xf = -1;
    for (const auto& d : rl_draws) {
        if (d.xf != cur_xf) {
            dev->SetTransform(D3DTS_WORLD, &rl_xf[static_cast<std::size_t>(d.xf)]);
            cur_xf = d.xf;
        }
        dev->SetTexture(0, d.tex);
        if (d.tss_wrap && !d.tex)
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        if (d.stat) {
            DrawBatch(dev, *d.stat);
        } else if (vb_ok) {
            dev->SetStreamSource(0, g_rlVb, 0, (UINT)sizeof(V));
            dev->DrawPrimitive(D3DPT_TRIANGLELIST, d.first, d.count / 3);
        } else {
            dev->DrawPrimitiveUP(D3DPT_TRIANGLELIST, d.count / 3,
                                 rl_verts.data() + d.first, sizeof(V));
        }
        if (d.tss_wrap && !d.tex)
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    }
    // MASHED_DBG_DRAWSTREAM3D "cars" tally (relit path): rl_draws IS the
    // submitted draw list (emit() drops empty batches), so the counts match
    // the legacy section's per-DrawBatch tally exactly. Race3DCat early-outs
    // when the channel is off.
    {
        unsigned ds_b = 0, ds_v = 0, ds_t = 0;
        for (const auto& d : rl_draws) {
            ++ds_b;
            ds_v += d.stat ? static_cast<unsigned>(d.stat->size()) : d.count;
            if (d.tex) ++ds_t;
        }
        DrawStreamDump_Race3DCat("cars", ds_b, ds_v, ds_t);
    }
    dev->SetTransform(D3DTS_WORLD, &worldm);
}

// Collision/slide FX. [SCAFFOLD] presentation, real data path: reuses the existing
// ParticleSystem billboard pool and scales every threshold to track radius.
//
// CALIBRATION CAVEAT (load-bearing, do not lose this):
// the skid trigger is driven off YAW-RATE x speed, not off lateral slip, because the
// kinematic/AI cars keep velocity aligned with heading and encode motion as
// (speed, yaw) with vel[] zero. The first pass used slip and fired 0 skids per race;
// after the change it fired 251. That calibration is therefore tuned to the CURRENT
// DEFAULT drive model. ROADMAP v3 D2 inverts MASHED_REAL_PHYSICS and makes the ported
// chain the default, at which point vel[] becomes real and the slip term starts
// carrying signal — these thresholds must be re-measured then, not assumed to carry
// over. The `slip` branch below is retained precisely so it takes over when that lands.
void TrackRenderer::EmitCarFx(int slot, const float pos[3], const float vel[3],
                              float speed, float yaw, float dt) {
    if (slot < 0 || slot > 3 || dt <= 0.f) return;
    const float R  = track_radius_ > 1.f ? track_radius_ : radius_;
    const float fx = std::cos(yaw), fz = std::sin(yaw);
    const float vf = vel[0] * fx + vel[2] * fz;             // forward component
    const float lx = vel[0] - fx * vf, lz = vel[2] - fz * vf;
    const float slip = std::sqrt(lx * lx + lz * lz);        // lateral slide speed

    // skid smoke: lateral slip OR hard cornering at speed.
    float dyaw = yaw - fx_prev_yaw_[slot];
    while (dyaw >  3.14159265f) dyaw -= 6.28318531f;
    while (dyaw < -3.14159265f) dyaw += 6.28318531f;
    const float spd = (speed < 0.f) ? -speed : speed;
    const float yawRate = std::fabs(dyaw) / dt;                  // rad/s
    const float corner = yawRate * spd;                          // cornering intensity
    fx_prev_yaw_[slot] = yaw;
    const float kSlip = R * 0.02f, kCorner = R * 0.12f;
    float skidI = slip / kSlip;
    if (corner / kCorner > skidI) skidI = corner / kCorner;      // whichever is stronger
    if (skidI > 1.f && spd > R * 0.04f) {
        fx_skid_accum_[slot] += (skidI > 3.f ? 3.f : skidI) * 36.f * dt;
        int n = static_cast<int>(fx_skid_accum_[slot]);
        if (n > 4) n = 4;
        fx_skid_accum_[slot] -= static_cast<float>(n);
        fx_skids_[slot] += n;
        for (int i = 0; i < n; ++i) {
            const float w  = (i & 1) ? 1.f : -1.f;          // two wheel tracks
            const float wx = pos[0] - fx * (R * 0.012f) + (-fz) * w * (R * 0.006f);
            const float wz = pos[2] - fz * (R * 0.012f) + ( fx) * w * (R * 0.006f);
            const float p[3] = { wx, pos[1] + R * 0.002f, wz };
            float aF = 0.28f + 0.30f * (skidI > 2.f ? 1.f : skidI * 0.5f);
            std::uint8_t a = static_cast<std::uint8_t>(aF * 255.f);
            std::uint32_t col = (static_cast<std::uint32_t>(a) << 24) | 0x00303030u; // dark smoke
            parts_.SpawnTrail(p, col, R * 0.010f, 0.55f);
        }
    } else {
        fx_skid_accum_[slot] = 0.f;
    }

    // impact sparks: a sudden one-frame deceleration is a collision
    const float decel = fx_prev_speed_[slot] - speed;
    const float kImpact = R * 0.06f;
    if (decel > kImpact && fx_prev_speed_[slot] > R * 0.05f) {
        const float k = decel / kImpact;                    // >= 1
        const int n = static_cast<int>(8.f + 8.f * (k > 2.f ? 2.f : k));
        const float p[3] = { pos[0], pos[1] + R * 0.012f, pos[2] };
        parts_.SpawnBurst(p, n, 0xE0FFC850u /*warm spark*/, R * 0.07f, R * 0.006f, 0.35f);
        ++fx_sparks_[slot];
    }
    // [diag] MASHED_FX_DEBUG: one line PER SLOT per sample window, each carrying that
    // slot's OWN inputs and its OWN counters. The previous form logged only slot 1 but
    // printed a counter summed over all four cars, so a rising number could not be
    // attributed and a stationary car appeared to emit thousands of skids.
    static int s_fxdbg = -1;
    if (s_fxdbg < 0) { char b[8]; s_fxdbg = GetEnvironmentVariableA("MASHED_FX_DEBUG", b, sizeof b) ? 1 : 0; }
    if (s_fxdbg) {
        static int dn[4] = {};
        if ((dn[slot]++ % 12) == 0) {
            if (std::FILE* f = std::fopen("log/fx_debug.txt", "a")) {
                // alive/pool: the question "is the FX rate starving the shared pool?"
                // is answerable only from occupancy, not from cumulative emit counts.
                std::fprintf(f, "[s%d] R=%.1f spd=%.2f yawRate=%.2f corner=%.2f kCorner=%.2f decel=%.2f skidI=%.2f skids=%d sparks=%d alive=%d\n",
                             slot, R, spd, yawRate, corner, kCorner, decel, skidI,
                             fx_skids_[slot], fx_sparks_[slot], parts_.alive());
                std::fclose(f);
            }
        }
    }
    fx_prev_speed_[slot] = speed;
}

void TrackRenderer::Render(IDirect3DDevice9* dev, float t, const CamInput* in) {
    if (!ready_) return;
    // Parity 3D channel (MASHED_DBG_DRAWSTREAM3D) frame delimiter: Render() is
    // the standalone's single race-scene entry — exe_main's race branch calls
    // it exactly once per rendered frame, and it never runs for the menu.
    DrawStreamDump_Race3DBegin();
    // Per-category geometry tallies for the channel: summed beside every
    // DrawBatch submit below and flushed once per category via Race3DCat
    // (sky/world/props/copters/cars). A few dozen integer adds per frame;
    // Race3DBegin/Cat early-out on g_state3d when the channel is off.
    unsigned ds_b = 0, ds_v = 0, ds_t = 0;
    auto ds_tally = [&](const std::vector<V>& b, IDirect3DTexture9* tex) {
        ++ds_b; ds_v += static_cast<unsigned>(b.size()); if (tex) ++ds_t; };
    auto ds_flush = [&](const char* cat) {
        DrawStreamDump_Race3DCat(cat, ds_b, ds_v, ds_t);
        ds_b = ds_v = ds_t = 0; };
    // WS-A s3 render-section profiler (env MASHED_RENDER_PROF): attribute the
    // 3D frame's CPU cost to sky/world/props/copters/cars/particles so the real
    // hot path is measured. QPC; logs render_prof.log. Off unless the env is set.
    static const bool s_rprof = (std::getenv("MASHED_RENDER_PROF") != nullptr);
    static LARGE_INTEGER s_rqf = []{ LARGE_INTEGER f;
        QueryPerformanceFrequency(&f); return f; }();
    auto RT  = []{ LARGE_INTEGER c; QueryPerformanceCounter(&c); return c.QuadPart; };
    auto RMs = [](long long a, long long b){
        return (double)(b - a) * 1000.0 / (double)s_rqf.QuadPart; };
    double d_world = 0, d_props = 0, d_car = 0, d_part = 0, d_other = 0;
    long long _rt0 = s_rprof ? RT() : 0;
    long long _mk  = _rt0;
    auto MARK = [&](double& acc){ if (s_rprof) { long long n = RT();
        acc += RMs(_mk, n); _mk = n; } };
    // F3 A/B verification toggle: suppress all UV-scroll texture transforms.
    static const bool s_uvscroll_off = std::getenv("MASHED_NO_UVSCROLL") != nullptr;
    // P3 same-view parity: MASHED_CAM_POSE=ex,ey,ez,ax,ay,az pins the camera to a
    // fixed world-space eye/at (overrides orbit/chase/free), so the standalone can
    // render a track from the ORIGINAL's exact captured pose and an in-race diff is
    // finally same-view. The producer is re/frida/race_draw_burst.py, which reads the
    // original's race-camera struct DAT_00897fe0 (+0x40 eye, +0x4c at) and prints the
    // value in this exact format. NOTE the pose is read from that RW struct via Frida,
    // NOT from any D3D transform: 3755c57e established that RenderWare pre-transforms
    // world geometry on the CPU, so D3DTS_VIEW and the first world matrix are BOTH
    // identity and the shim cannot recover the camera from fixed-function state.
    //
    // TWO FORMS, and the 12-float one is the faithful one:
    //   6  floats: ex,ey,ez,ax,ay,az            -- eye + at-point (legacy)
    //   12 floats: px,py,pz,rx,ry,rz,ux,uy,uz,ax,ay,az
    //              -- the camera's full orthonormal BASIS: position, right, up,
    //                 at. This exists because the 6-float form assumes up is
    //                 world +Y and therefore cannot express roll, and the
    //                 original's race camera measurably HAS roll (RwCamera frame
    //                 right.y = 0.440, ~26 deg, verify/d1_carproj/RESULT.md).
    //
    // Source, and this is the part that was wrong for months: feed it from the
    // RwCamera's FRAME (*(RwCamera+4), LTM +0x50 -> right +0x00 / up +0x10 /
    // at +0x20 / pos +0x30), NOT from the controller struct DAT_00897fe0
    // +0x40/+0x4c. Those controller fields are not the world camera: projecting
    // the original's own car world positions through them puts the cars at
    // z 4.8-7.7 spread across mid-frame, while the capture shows them clustered
    // at z ~18.5-21.8 -- which is what the frame basis predicts.
    static float s_campose[12];
    static int  s_campose_n = 0;
    static const bool s_have_campose = [] {
        char b[256];
        if (GetEnvironmentVariableA("MASHED_CAM_POSE", b, sizeof(b)) == 0) return false;
        s_campose_n = std::sscanf(b,
            "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
            &s_campose[0], &s_campose[1], &s_campose[2], &s_campose[3],
            &s_campose[4], &s_campose[5], &s_campose[6], &s_campose[7],
            &s_campose[8], &s_campose[9], &s_campose[10], &s_campose[11]);
        return s_campose_n == 6 || s_campose_n == 12;
    }();
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
        // WS-E s3 GROUND CHASE (2026-06-28): a behind-the-car chase rig — the
        // distilled single-follow form of the RE'd shared race camera
        // (re/analysis/race_camera/race_camera.md; director FUN_00446520 +
        // Camera::Apply 0x00441760). The standalone's shared-pack race_cam_
        // (race_cam_.pos()/target()) is built to frame ALL FOUR cars, so it sits
        // in a high orbit looking down — wrong for the single-car standalone view
        // and the reference Arctic frame (car bottom-centre, ground-level, looking
        // down the road). Here we follow the player car directly: eye 4.75 behind
        // + 3.3 above, look-at 0.15 above the car and ~2 units down-track
        // (=> ~25 deg down-tilt). race_cam_ is still advanced in UpdateRace() for
        // the elimination/zoom rule; only the RENDER eye/target is overridden.
        // Distances scale with the car's loaded size (car_len_ ~0.7 units on the
        // small-scale Arctic geometry): the original rig's absolute 4.75/3.3 would
        // sit ~8 car-lengths back here and shrink the car to a speck. In car-length
        // units the original chase is ~1.5 behind / ~1.0 above, look-at ~1.5 ahead
        // and ~half a car-height up (=> ~15-20 deg down-tilt, car at bottom-centre).
        const float fwd[3] = {std::cos(car_yaw_), 0.f, std::sin(car_yaw_)};
        const float L = car_len_;
        const float kBack = L * 1.5f, kUp = L * 1.0f, kAhead = L * 1.5f;
        const float kAtY = car_height_ * 0.5f;
        eye[0] = car_pos_[0] - fwd[0] * kBack;
        eye[1] = car_pos_[1] + kUp;
        eye[2] = car_pos_[2] - fwd[2] * kBack;
        at[0]  = car_pos_[0] + fwd[0] * kAhead;
        at[1]  = car_pos_[1] + kAtY;
        at[2]  = car_pos_[2] + fwd[2] * kAhead;
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
    // P3 same-view parity: pin to the captured pose. The 12-float basis form
    // carries roll, so it is applied as a basis rather than collapsed into an
    // at-point -- collapsing would throw the roll away, which is the whole
    // reason the form exists.
    const bool campose_basis = (s_have_campose && s_campose_n == 12);
    if (s_have_campose && !campose_basis) {
        for (int i = 0; i < 3; ++i) { eye[i] = s_campose[i]; at[i] = s_campose[3 + i]; }
    } else if (campose_basis) {
        // last_eye_/last_at_ still get a look-at pair because RaceSceneState
        // publishes a point (librw + the HUD read it). The VIEW MATRIX below is
        // built from the basis, so the roll survives where it matters; only the
        // published at-point is the lossy summary.
        for (int i = 0; i < 3; ++i) {
            eye[i] = s_campose[i];
            at[i]  = s_campose[i] + s_campose[9 + i];
        }
    }
    for (int i = 0; i < 3; ++i) { last_eye_[i] = eye[i]; last_at_[i] = at[i]; }
    // Publish the BASIS too, not just the lossy at-point. librw is the default
    // renderer and draws the static world from what it is handed; handing it the
    // pair silently dropped the roll for the world pass (RaceSceneState.h).
    last_basis_valid_ = campose_basis;
    if (campose_basis) {
        for (int i = 0; i < 3; ++i) {
            last_right_[i] = s_campose[3 + i];
            last_up_[i]    = s_campose[6 + i];
            last_atdir_[i] = s_campose[9 + i];
        }
    }

    D3DMATRIX viewm, projm, worldm;
    if (campose_basis)
        MatViewFromBasis(&viewm, &s_campose[0], &s_campose[3],
                         &s_campose[6], &s_campose[9]);
    else
        MatLookAtLH(&viewm, eye, at);
    // E2'b step 3: publish the resolved frustum into RaceSceneState and build
    // the D3D9 projection FROM those members, so the librw submitter reads the
    // same four numbers rather than a hand-copied second set that could drift.
    //
    // LENS: MEASURED FROM THE ORIGINAL (2026-08-16), replacing the invented
    // 60 deg that stood here since WS-E s3. RenderWare stores no angle -- the
    // lens is RwCamera::viewWindow, the half-extents at unit distance -- so the
    // measured quantity is kViewWindowY and the angle below is arithmetic on it.
    //
    // Read live from MASHED.exe (TRAINING, Quick Battle) at
    // *(DAT_00897fe0 + 0x84) + 0x6c: viewWindow = (0.6000, 0.4500), near 0.1,
    // far 360.0, projType 1. Two cross-checks passed, so the offsets are right
    // and the value is not a hopeful pointer read:
    //   * recipViewWindow (+0x70/+0x74) == 1/viewWindow;
    //   * Camera::SetupFOV (0x00441700) reproduces it exactly from the
    //     controller fields -- vw.y = 0.75 * ctrl[0x70] * ctrl[0x58]
    //     = 0.75 * 1.0 * 0.6 = 0.45, with ctrl[0x6c]/[0x70] measured at 1.0.
    // Corroborated independently by hooks.csv:559 (MinimapCameraOrthoSetup,
    // "ortho 0.6 x 0.45"). Evidence: verify/d1_lens/RESULT.md + orig_lens.json.
    //
    // NOTE the WS-E s3 comment removed from here claimed the old ~48 deg branch
    // was superseded because it "framed the whole pack from a high orbit". That
    // branch had derived 48 deg from view-window 0.6 and was RIGHT; deleting it
    // is what left an invented constant in a shipping path for two months.
    //
    // 0.6/0.45 = 4:3, consistent with last_aspect_ below.
    // MASHED_CAM_FOV=<degrees> still overrides, for A/B against a capture.
    static const float kViewWindowY = 0.45f;              // MEASURED half-extent
    last_fov_    = 2.f * std::atan(kViewWindowY);         // = 48.46 deg
    {
        static const float s_fov_override = [] {
            char b[32];
            if (GetEnvironmentVariableA("MASHED_CAM_FOV", b, sizeof(b)) == 0) return 0.f;
            const float deg = static_cast<float>(std::atof(b));
            return (deg > 1.f && deg < 179.f) ? deg * 3.14159265f / 180.f : 0.f;
        }();
        if (s_fov_override > 0.f) last_fov_ = s_fov_override;
    }
    last_aspect_ = 800.f / 600.f;
    last_near_   = 0.1f;      // MEASURED (RwCamera +0x80); was an invented 0.05
    // FAR: the original's far clip is COURSE.LUA Setup_Fog's SECOND argument.
    // Not a formula on any radius -- the original computes no world radius at
    // all; radius_ is ours, invented at :1657, and radius_*8 stood here purely
    // as a guess.
    //
    // The track loader (0x00426e10) reads the course-description fog pair
    // (Setup_Fog handler 0x0047ab30 stores near at +0x1ec, far at +0x1f0),
    // guards on near != far, then writes near to the camera's fogPlane (+0x88)
    // and far through 0x004c1b10 (the RwCameraSetFarClipPlane equivalent, which
    // writes +0x84). 0x00426810 does the same +0x88 / +0x84 pairing per frame
    // for tracks with a camera anim.
    //
    // Confirmed by measurement on TRAINING, both arguments at once, which is
    // why no second track was needed: the live camera read back
    // fogPlane = 20.0 and farPlane = 360.0, and our own COURSE.LUA asset survey
    // at :1436-1440 records training as Setup_Fog(20, 360). Exact match on the
    // pair, so the near/far operand binding is settled too. Evidence:
    // verify/d1_lens/RESULT.md + orig_lens.json.
    //
    // No Setup_Fog -> the loader's near != far guard fails and the camera keeps
    // the init-time default, which is 180.0f at every camera-init site found
    // (0x0042d560, 0x0042f660, 0x00467110 all write 0x43340000).
    last_far_    = fog_on_ ? fog_end_ : 180.f;
    MatPerspectiveFovLH(&projm, last_fov_, last_aspect_,
                        last_near_, last_far_);
    // Same source of truth for the particle FX lens-wall guard: it sizes
    // billboards against the viewport, so it must read the FOV this frame
    // actually projects with rather than keep a second copy.
    parts_.SetFovY(last_fov_);
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
    // E2'b step 3: when the librw submit path is live it draws the static world,
    // so the D3D9 world batches must be skipped — otherwise BOTH renderers write
    // the same pixels and no difference is attributable to either. Folded into the
    // existing rw_world gate, which already means exactly "someone else drew the
    // world". Everything else (cars, props, particles, pickups, sky, HUD) still
    // comes from D3D9, so remaining deltas are scope, not overdraw.
    const bool rw_world = D3d9Render::RwWorldRender_Render(/*world*/nullptr, /*cam*/nullptr) != 0
                          || LibRw::RaceSubmit_Active();

    // F3: hand librw the SAME animation clock this function scrolls its own
    // texture transforms with. Passing anything else (a private timer, a frame
    // count) would put the two renderers at different scroll phases, which is
    // indistinguishable from a shading bug -- that is precisely how D-S3-SEA read
    // for several rounds. `t` is the single source of truth; it is not re-derived.
    // Honours MASHED_NO_UVSCROLL so the kill-switch still disables BOTH paths
    // together and stays usable as the positive control that found this.
    LibRw::RaceSubmit_SetAnimTime(s_uvscroll_off ? 0.f : t);

    // sky clump first: unfogged, no depth write (renderer-gap closure)
    if (!sky_.batches.empty()) {
        dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
        dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        // WS-E skybox: SKY.DFF is a small CAMERA-LOCKED dome (Arctic radius
        // ~1.9, centred at origin), NOT world-scale. Drawn at world origin it
        // collapses to a distant speck in-race (camera tens of units away) ->
        // black sky. Centre it on the camera eye so it surrounds the viewer.
        // Translate only (clouds keep world orientation as the camera turns);
        // z-write is off so every world surface draws over the dome.
        D3DMATRIX skym; MatIdentity(&skym);
        skym._41 = eye[0]; skym._42 = eye[1]; skym._43 = eye[2];
        dev->SetTransform(D3DTS_WORLD, &skym);
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
            DrawBatch(dev, b);
            ds_tally(b, sky_.textures[mi]);
            if (scroll)
                dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                          D3DTTFF_DISABLE);
        }
        dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        dev->SetTransform(D3DTS_WORLD, &worldm);  // restore identity for world
    }
    ds_flush("sky");   // zeros when no sky clump loaded = the dark-void signal
    // fog (COURSE.LUA Setup_Fog) + alpha cutouts (fence/grate textures).
    // The fog range (Setup_Fog end) is calibrated for the in-game chase camera,
    // which sits a few units behind the car. The orbit/free OVERVIEW cameras sit
    // a full track-radius back (~1.6x extent), well beyond fog_end_, so applying
    // the gameplay fog there washes the entire track to fog_color (the dark
    // sliver we saw). Apply fog only when following the car (chase/race cam);
    // the overview cameras render unfogged so the whole track is visible.
    const bool chase_cam = car_ready_ && !free_;
    // WS-E FOG (2026-06-29): restore the in-race distance fog as the DEFAULT.
    // Background: the WS-A s3 perf fix moved the device to HARDWARE vertex
    // processing (9->180 fps). Under HW T&L, *vertex* fog (D3DRS_FOGVERTEXMODE)
    // white-washes our pre-lit world/car verts, which is why the perf merge gated
    // ALL chase fog off behind MASHED_CHASE_FOG. The HW-correct path is *table*
    // (pixel) fog: FOGVERTEXMODE = NONE, FOGTABLEMODE = LINEAR, computed per-pixel
    // from interpolated depth — independent of vertex format, so it does NOT wash
    // out. The earlier "table fog also measured ~221 whiteout" reading pre-dated
    // the WS-E trackfix (commit f3091cdb): the old `fa*fb` near-plane parse
    // INVERTED the linear ramp for every track with near >= 1, clamping all near
    // geometry to full fog colour. With the corrected parse (fog_start_ = near,
    // fog_end_ = far, near < far on every track) the ramp is monotone: near
    // geometry clear, distance fades to the track fog colour. The projection
    // (MatPerspectiveFovLH above) emits w = eye-space z (m._34 = 1), so on
    // WFOG-capable hardware FOGSTART/FOGEND are the Setup_Fog absolute camera
    // distances directly.
    //
    // Fog applies only to the chase/race cam (chase_cam): the orbit/overview
    // cameras sit a full track-radius back, well beyond fog_end_, and would wash
    // the whole track to fog colour — those render unfogged. Set MASHED_NO_FOG=1
    // to disable the distance fog entirely (escape hatch).
    static const bool s_no_fog = (std::getenv("MASHED_NO_FOG") != nullptr);
    if (fog_on_ && chase_cam && !s_no_fog) {
        dev->SetRenderState(D3DRS_FOGENABLE, TRUE);
        dev->SetRenderState(D3DRS_FOGCOLOR, fog_color_);
        dev->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);   // NOT vertex fog (HW T&L washes)
        dev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);  // table/pixel fog (HW-correct)
        dev->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
        DWORD fs, fe;
        std::memcpy(&fs, &fog_start_, 4);
        std::memcpy(&fe, &fog_end_, 4);
        dev->SetRenderState(D3DRS_FOGSTART, fs);   // Setup_Fog near (abs cam dist)
        dev->SetRenderState(D3DRS_FOGEND, fe);     // Setup_Fog far  (abs cam dist)
    } else {
        dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
        dev->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
    }
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    dev->SetRenderState(D3DRS_ALPHAREF, 0x30);
    dev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

    MARK(d_other);   // setup + sky + fog state
    // WS-E1: skip the spike's world-geometry batches when the RW world render path
    // drew the world (the `if` gates the entire for-statement; inert today).
    // D-S3-BANK probe: does the world contain COPLANAR surfaces whose winner is
    // decided by draw order? It matters because the two renderers order the world
    // differently and neither sets D3DRS_ZFUNC, so both use D3D9's default
    // LESSEQUAL -- under which the LAST draw wins a depth tie. D3D9 accumulates
    // batches_ per material ACROSS all sectors (material-major); librw gives each
    // sector its own geometry and draws matIds 0..n within it (sector-major, see
    // geometry.cpp buildMeshes). MASHED_WORLD_REVORDER=1 reverses this path's
    // material order: if the frame is unchanged there is no coplanar overlap and
    // order cannot be the cause; if it moves, order is live.
    // Announces itself once: a reversal that changes nothing and a gate that never
    // fired produce the SAME 0.00, so the log line is what separates them.
    static const bool s_rev_order = [] {
        const bool on = std::getenv("MASHED_WORLD_REVORDER") != nullptr;
        if (on) {
            if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
                std::fprintf(f, "D-S3-BANK: world material order REVERSED\n");
                std::fclose(f);
            }
        }
        return on;
    }();
    static const bool s_matid = [] {
        const char* e = std::getenv("MASHED_WORLD_MATID");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    // MATID needs fog OFF or the flat colours get blended toward fog colour with
    // distance and stop being readable as indices.
    if (s_matid) dev->SetRenderState(D3DRS_FOGENABLE, FALSE);

    // MASHED_WORLD_ONLYMAT=N draws only world material N, isolating its raw
    // rasterised coverage. The previous version of this probe SILENTLY DID
    // NOTHING -- a "Snow only" run still showed World -- and its output looked
    // clean and quantitative enough to be believed. So this one reports what it
    // actually submitted: `drew` is a bitmask of the material indices that
    // reached DrawBatch, logged once. If ONLYMAT=4 does not print drew=0x10,
    // the run is void and must not be measured.
    static const int s_only_mat = [] {
        const char* e = std::getenv("MASHED_WORLD_ONLYMAT");
        return (e && *e) ? std::atoi(e) : -1;
    }();
    // D-S3-BANK discriminator: MASHED_WORLD_PRELITONLY=1 draws the world with the
    // TEXTURE suppressed on BOTH paths -- output becomes the interpolated prelit
    // vertex colour alone (COLOROP=SELECTARG2 -> DIFFUSE here; librw strips the
    // material texture so its PS TEX path is off). If the snow delta VANISHES here
    // the difference is texture/UV sampling; if it PERSISTS it is vertex-colour
    // interpolation (fixed-function iterated DIFFUSE vs shader COLOR0 varying).
    static const bool s_prelit_only = [] {
        const char* e = std::getenv("MASHED_WORLD_PRELITONLY");
        return e && e[0] == '1' && e[1] == '\0';
    }();
    unsigned drew = 0;

    // MASHED_WORLD_VDUMP=1: dump the D3D9 snow (mat 4) batch's triangle vertices
    // as "4 x y z r g b" to log/vdump_d3d9.txt, once. Matched by position against
    // the librw dump (RwSceneBuild) to see if the packed prelit differs per vertex.
    static const bool s_vdump = std::getenv("MASHED_WORLD_VDUMP") != nullptr;
    if (!rw_world && s_vdump) {
        static bool s_vdumped = false;
        if (!s_vdumped && batches_.size() > 4) {
            s_vdumped = true;
            if (std::FILE* f = std::fopen("log/vdump_d3d9.txt", "w")) {
                for (const V& v : batches_[4]) {
                    const std::uint32_t c = v.c;
                    std::fprintf(f, "4 %.3f %.3f %.3f %u %u %u\n", v.x, v.y, v.z,
                                 (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
                }
                std::fclose(f);
            }
        }
    }

    if (!rw_world)
    for (std::size_t k = 0; k < batches_.size(); ++k) {
        const std::size_t mi = s_rev_order ? batches_.size() - 1 - k : k;
        const auto& b = batches_[mi];
        if (b.empty()) continue;
        if (s_only_mat >= 0 && (int)mi != s_only_mat) continue;
        drew |= 1u << mi;
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
        // D-S3-BANK MATID: draw each material as a flat ID colour instead of
        // shaded texture, matching what BuildWorld does on the librw side, so
        // "which material owns these pixels" is read off the image rather than
        // inferred. Same encoding: (20 + i*18, 200, 60), i = (R-20)/18.
        if (s_prelit_only) {
            // DIFFUSE (interpolated prelit) only -- texture ignored by SELECTARG2.
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        } else if (s_matid) {
            dev->SetRenderState(D3DRS_TEXTUREFACTOR,
                                D3DCOLOR_XRGB(20 + (int)mi * 18, 200, 60));
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
            dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
        } else if (!textures_[mi]) {
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        }
        DrawBatch(dev, b);
        ds_tally(b, textures_[mi]);
        if (s_prelit_only) {
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        } else if (s_matid) {
            dev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        } else if (!textures_[mi]) {
            dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        }
        if (scroll)
            dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
    }
    // Liveness for the two world probes. Printed once, from inside the frame that
    // actually rendered, so it cannot be confused with build-time intent.
    if (s_only_mat >= 0 || s_matid || s_prelit_only) {
        static bool s_probe_logged = false;
        if (!s_probe_logged) {
            s_probe_logged = true;
            if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
                std::fprintf(f, "D-S3-BANK PROBE: onlymat=%d matid=%d prelitonly=%d "
                                "rw_world=%d drew=0x%X (bit i set = world material i "
                                "submitted)\n",
                             s_only_mat, (int)s_matid, (int)s_prelit_only,
                             (int)rw_world, drew);
                std::fclose(f);
            }
        }
    }
    ds_flush("world");   // reports zeros when rw_world drew instead (inert today)

    // MASHED_DBG_CARPOS=1 appends this frame's car WORLD positions to
    // log/carpos_re.txt (override MASHED_DBG_CARPOS_OUT). Built for race
    // first-frame parity: the original side already reports its car array
    // (re/frida/race_draw_burst.py CARPROJ), and without the same numbers from
    // this side there is no way to tell "the standalone did not DRAW the cars"
    // from "the standalone drew them somewhere else" -- the two look identical
    // in a screenshot taken through a transplanted camera. Off unless asked for.
    {
        static const bool s_carpos = [] {
            return GetEnvironmentVariableA("MASHED_DBG_CARPOS", nullptr, 0) != 0;
        }();
        if (s_carpos) {
            static int s_cp_frame = 0;
            char path[MAX_PATH] = {};
            if (GetEnvironmentVariableA("MASHED_DBG_CARPOS_OUT", path,
                                        sizeof(path)) == 0)
                std::strcpy(path, "log/carpos_re.txt");
            if (std::FILE* f = std::fopen(path, "a")) {
                std::fprintf(f, "f%d player_ready=%d pos=(%.4f,%.4f,%.4f) "
                                "yaw=%.4f ai_n=%d\n",
                             s_cp_frame, (int)car_ready_,
                             car_pos_[0], car_pos_[1], car_pos_[2], car_yaw_,
                             (int)ai_cars_.size());
                for (std::size_t i = 0; i < ai_cars_.size(); ++i)
                    std::fprintf(f, "f%d ai%u pos=(%.4f,%.4f,%.4f) yaw=%.4f\n",
                                 s_cp_frame, (unsigned)i,
                                 ai_cars_[i].pos[0], ai_cars_[i].pos[1],
                                 ai_cars_[i].pos[2], ai_cars_[i].yaw);
                std::fclose(f);
            }
            ++s_cp_frame;
        }
    }

    MARK(d_world);   // static world geometry batches_ (DrawPrimitiveUP)
    // R6: track props — instanced DFF batches (tyre walls, crates, sea,
    // freighter...) under their MTS / identity world matrices.
    for (const auto& p : props_) {
        // E2'b step 3: when this model is registered with librw, queue its placed
        // copies there instead of drawing them here -- passing the SAME D3DMATRIX
        // the D3D9 path would have used, so both renderers place it identically.
        // A -1 handle falls through to the D3D9 draw below, which is what makes
        // the port incremental: an unregistered model still renders.
        if (rw_world && p.rw_model >= 0 &&
            LibRw::RaceSubmit_InstanceModelEnabled(p.rw_model)) {
            for (const auto& inst : p.instances)
                LibRw::RaceSubmit_AddInstance(p.rw_model, (const float*)&inst);
            continue;
        }
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
                DrawBatch(dev, b);
                ds_tally(b, p.textures[mi]);
                if (!p.textures[mi])
                    dev->SetTextureStageState(0, D3DTSS_COLOROP,
                                              D3DTOP_MODULATE);
                if (scroll)
                    dev->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS,
                                              D3DTTFF_DISABLE);
            }
        }
    }

    ds_flush("props");

    MARK(d_props);   // instanced track props
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
            DrawBatch(dev, b);
            ds_tally(b, m.textures[mi]);
            if (!m.textures[mi])
                dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        }
    }
    dev->SetTransform(D3DTS_WORLD, &worldm);
    ds_flush("copters");

    MARK(d_other);   // copters
    // R5: the car — model-space batches under a yaw+translate world matrix.
    // WS-E vehicle lighting: with MASHED_RPLIGHT + a track directional light,
    // the car section runs through the relit pass (per-frame world-space sun
    // on the lit batches; identical draw order/states, packed dynamic VB).
    const bool relit_cars = car_ready_ && rp_light_on_ && has_sun_dir_;
    // E2'b step 3: submit the player body through librw when it is registered.
    // This takes precedence over BOTH D3D9 car paths so the car is never drawn
    // twice. The transform is built exactly as the D3D9 branch below builds it.
    //
    // [DELTA D-S3-5] This bypasses RenderCarsRelit, the per-frame world-space sun
    // relight (MASHED_RPLIGHT, default ON) -- which is the ACTIVE path on any track
    // with a directional light, Arctic included. Through librw the body therefore
    // carries its baked prelight plus the rw::Light ambient/directional instead of
    // the ported per-vertex N.L. That is a real visual delta, logged rather than
    // hidden; closing it means giving the librw path an equivalent relight pass.
    const bool car_via_rw = rw_world && car_ready_ && rw_car_model_ >= 0 &&
                            LibRw::RaceSubmit_InstanceModelEnabled(rw_car_model_);
    if (car_via_rw) {
        D3DMATRIX carm;
        MatIdentity(&carm);
        const float cy = std::cos(car_yaw_), sy2 = std::sin(car_yaw_);
        if (car_long_is_x_) {
            carm._11 =  cy;  carm._13 = sy2;
            carm._31 = -sy2; carm._33 = cy;
        } else {
            carm._31 =  cy;  carm._33 = sy2;
            carm._11 = sy2;  carm._13 = -cy;
        }
        carm._41 = car_pos_[0]; carm._42 = car_pos_[1]; carm._43 = car_pos_[2];
        LibRw::RaceSubmit_AddInstance(rw_car_model_, (const float*)&carm);
        // AI cars reuse the player body when they have no livery variant of their
        // own; those that do keep drawing through D3D9 (their variants are not
        // registered yet), which is the same incremental fallback as props.
        for (const auto& a : ai_cars_) {
            D3DMATRIX am;
            MatIdentity(&am);
            const float ay = std::cos(a.yaw), asy = std::sin(a.yaw);
            if (car_long_is_x_) {
                am._11 =  ay;  am._13 = asy;
                am._31 = -asy; am._33 = ay;
            } else {
                am._31 =  ay;  am._33 = asy;
                am._11 = asy;  am._13 = -ay;
            }
            am._41 = a.pos[0]; am._42 = a.pos[1]; am._43 = a.pos[2];
            LibRw::RaceSubmit_AddInstance(rw_car_model_, (const float*)&am);
        }
    } else if (relit_cars) {
        RenderCarsRelit(dev, worldm);   // emits its own "cars" Race3DCat tally
    } else if (car_ready_) {
        D3DMATRIX carm;
        MatIdentity(&carm);
        const float cy = std::cos(car_yaw_), sy2 = std::sin(car_yaw_);
        // WS-E s3: orient the body so the car's LONG (nose) axis points along the
        // travel/heading direction F=(cos yaw, sin yaw). For this Advantage model
        // the nose is model +Z (front wheels at +Z => long_is_x=false), but the
        // old matrix mapped model +X -> F, rendering the car ~90 deg sideways — so
        // the new ground chase saw its flank, not its rear. Map the nose axis to F
        // (and the other axis to F's right) per long_is_x. Wheels inherit carm, so
        // their spin/steer stay aligned.
        if (car_long_is_x_) {
            carm._11 =  cy;  carm._13 = sy2;   // model +X (nose) -> F
            carm._31 = -sy2; carm._33 = cy;    // model +Z -> left
        } else {
            carm._31 =  cy;  carm._33 = sy2;   // model +Z (nose) -> F
            carm._11 = sy2;  carm._13 = -cy;   // model +X -> right
        }
        carm._41 = car_pos_[0]; carm._42 = car_pos_[1]; carm._43 = car_pos_[2];
        dev->SetTransform(D3DTS_WORLD, &carm);
        for (std::size_t mi = 0; mi < car_batches_.size(); ++mi) {
            const auto& b = car_batches_[mi];
            if (b.empty()) continue;
            dev->SetTexture(0, car_textures_[mi]);
            if (!car_textures_[mi])
                dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
            DrawBatch(dev, b);
            ds_tally(b, car_textures_[mi]);
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
                DrawBatch(dev, b);
                ds_tally(b, tex);
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
            if (car_long_is_x_) {            // nose=+X (see player carm above)
                am._11 =  ac; am._13 = as;
                am._31 = -as; am._33 = ac;
            } else {                         // nose=+Z
                am._31 =  ac; am._33 = as;
                am._11 =  as; am._13 = -ac;
            }
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
                DrawBatch(dev, b);
                ds_tally(b, texs[mi]);
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
                    DrawBatch(dev, b);
                    ds_tally(b, car_textures_[part.first]);
                }
            }
        }
        dev->SetTransform(D3DTS_WORLD, &worldm);
    }
    if (!relit_cars)
        ds_flush("cars");   // relit path reports inside RenderCarsRelit

    dev->SetTexture(0, nullptr);
    MARK(d_car);     // player + AI cars + wheels

    // In-race particles (weather + car dust): update with the per-frame dt and
    // draw camera-facing billboards. Depth-test ON so geometry occludes them;
    // depth-write OFF so they blend. Only while actually racing (car present).
    // MASHED_NO_PARTICLES=1 suppresses the whole pass (A/B + the dust/snow bloom
    // is a separate WS-E tuning issue; this isolates the world/car render).
    static const bool s_no_particles = (std::getenv("MASHED_NO_PARTICLES") != nullptr);
    if (!s_no_particles && (parts_.ambient() != ParticleSystem::None || car_ready_)) {
        const float dt = (last_t_ < 0.f) ? 0.f : (t - last_t_);
        last_t_ = t;
        float fwd[3] = {at[0]-eye[0], at[1]-eye[1], at[2]-eye[2]};
        // Collision/slide FX (skids + impact sparks) for every car, before the
        // pool advances this frame. Emit once per frame; parts_.Render is the
        // per-view half (kept separate — see D-11063's particle constraint).
        if (car_ready_) {
            EmitCarFx(0, car_pos_, car_vel_, car_speed_, car_yaw_, dt);
            // AiCar carries no velocity vector on main (pos/yaw/target/speed/
            // cur_speed/lane), so pass an explicit zero. This is faithful rather
            // than lossy: the AI/kinematic cars encode motion as (speed, yaw) and
            // their lateral-slip term was already ~0, which is exactly why the
            // trigger is yaw-rate-based (see EmitCarFx).
            static const float kNoVel[3] = { 0.f, 0.f, 0.f };
            for (int i = 0; i < static_cast<int>(ai_cars_.size()) && i < 3; ++i)
                EmitCarFx(i + 1, ai_cars_[i].pos, kNoVel,
                          ai_cars_[i].cur_speed, ai_cars_[i].yaw, dt);
        }
        parts_.Update(dt, eye, fwd, car_ready_ ? car_pos_ : nullptr,
                      car_ready_ ? car_speed_ : 0.f);
        parts_.Render(dev, eye, at);
        // power-up orbs (additive glow billboards) on top of the weather.
        // D1 isolation: the orbs blend ADDITIVE (PickupField.cpp:231
        // DESTBLEND_ONE), so they are a candidate for a saturated white-out on
        // their own. MASHED_NO_PICKUPS separates them from the particle pass,
        // which MASHED_NO_PARTICLES alone cannot do (it kills both).
        static const bool s_no_pickups = (std::getenv("MASHED_NO_PICKUPS") != nullptr);
        if (!s_no_pickups && pickups_.enabled()) pickups_.Render(dev, eye, at);
    }

    MARK(d_part);    // particles + pickups

    dev->SetRenderState(D3DRS_FOGENABLE, FALSE);
    dev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    dev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);  // back to the 2D menu path

    if (s_rprof) {
        static std::FILE* lf = nullptr; static bool op = false; static int fn = 0;
        if (!op) { lf = std::fopen("render_prof.log", "w");
            if (lf) std::fprintf(lf, "frame,total_ms,world_ms,props_ms,car_ms,part_ms,other_ms,"
                                     "world_batches,world_verts\n");
            op = true; }
        // one-shot world size (static): count non-empty batches + total verts.
        static int s_wb = -1, s_wv = -1;
        if (s_wb < 0) { s_wb = 0; s_wv = 0;
            for (const auto& b : batches_) { if (!b.empty()) { ++s_wb; s_wv += (int)b.size(); } } }
        const double total = RMs(_rt0, RT());
        if (lf && fn < 6000) {
            std::fprintf(lf, "%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%d,%d\n",
                fn, total, d_world, d_props, d_car, d_part, d_other, s_wb, s_wv);
            std::fflush(lf);
        }
        ++fn;
    }
}

}  // namespace D3d9Render
}  // namespace mashed_re
