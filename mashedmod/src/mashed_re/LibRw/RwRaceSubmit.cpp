// LibRw/RwRaceSubmit.cpp — in-loop librw submit path. See RwRaceSubmit.h.
//
// EXE-ONLY isolated TU (build.bat). Includes <rw.h>; nothing else may.

#include "RwRaceSubmit.h"

#include "RwBridge.h"
#include "RwCarFx.h"
#include "RwRaceFx.h"
#include "RwRasterBridge.h"
#include "RwSceneBuild.h"

#define WITH_D3D
#include <rw.h>

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "../Math/RwLutTables.h"
#include "../Race/RaceSceneState.h"
#include "../Track/DffModel.h"
#include "../Track/TrackWorld.h"
#include "../Txd/TxdDecoder.h"

// [rtex] librw P9 hook (d3d9matfx.cpp): the original's raster alpha flag for the MatFX env
// single / two pass choice.
namespace rw { namespace d3d9 { extern rw::int32 (*rw_mashed_rasteralpha)(rw::Raster*); } }
// [rstates] librw's opt-in sticky vertex-alpha switches (d3d9render.cpp, MASHED_PATCHES.md).
extern bool rw_mashed_skip_empty_meshes;
extern bool rw_mashed_keep_vertex_alpha;

namespace mashed_re {
namespace LibRw {
namespace {

// RwRasterBridge's texel rule when it built the raster, librw's own flag otherwise.
rw::int32 TexelAlphaHook(rw::Raster* r) {
    const int v = RasterTexelAlpha(r);
    if (v >= 0) return v;
    return (r && r->platform == rw::PLATFORM_D3D9) ? GETD3DRASTEREXT(r)->hasAlpha : 0;
}

const char* const kRaceLog = "log/librw_race.txt";

void RLog(const char* fmt, ...) {
    std::FILE* f = std::fopen(kRaceLog, "a");
    if (!f) return;
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(f, fmt, ap);
    va_end(ap);
    std::fputc('\n', f);
    std::fclose(f);
}

// ---- module state ---------------------------------------------------------
bool         g_engine_up = false;
bool         g_scene_up  = false;
int          g_width     = 0;
int          g_height    = 0;
rw::World*   g_world     = nullptr;
rw::Camera*  g_cam       = nullptr;
rw::Light*   g_amb       = nullptr;
rw::Light*   g_sun       = nullptr;
long long    g_frames    = 0;
HWND         g_hwnd_dbg  = nullptr;   // kept only for the D-S3-1 surface probe
IDirect3DStateBlock9* g_state_block = nullptr;  // D-S3-2 outbound fix

// Registered instanced models (props / cars / copters) and this frame's queue.
std::vector<rw::Clump*> g_models;
// [rtex] per instance: the car's env coefficient and env frame matrix (FUN_00420340 @
// 0x00420340 re-applies both per car each frame; one shared clump draws all four cars).
constexpr int kCarFxMarker = -2;   // [rfx] RaceSubmit_AddCarFxMarker's queue entry
// [rstates] `kind`: where RenderWare's world pass draws it (RwRaceSubmit.h kInst*).
// [carlod] and, for a car copy, its record state / flags (FUN_00420870's inputs).
struct Inst { int model; float m[16]; bool fx = false; float env_coef = 0.f; float env_m[16];
              std::vector<FramePose> poses; float sun_scale = -1.f;   // [librwrest]
              int kind = kInstAfterWorld;
              bool vis = false; int vis_state = 0; std::uint32_t vis_flags = 0; };
std::vector<Inst>       g_insts;
// [librwrest] per model, parallel to g_models: the FRAMELIST frames of a hierarchy model and
// their streamed matrices (restored after each posed copy draws). Empty otherwise.
std::vector<std::vector<rw::Frame*>>  g_model_frames;
std::vector<std::vector<rw::Matrix>>  g_model_bind;
// [librwrest] per model: each librw atomic's FRAMELIST frame (clump order), and the draw order /
// cull frame RaceSubmit_SetModelDrawOrder sets (empty / -1 = clump order, no test).
std::vector<std::vector<std::int32_t>> g_model_aframe;
struct DrawOrder { std::vector<rw::Atomic*> seq; rw::Atomic* cull = nullptr; };
std::vector<DrawOrder>                g_model_order;
std::vector<bool>                     g_model_rwstates;   // RaceSubmit_SetModelRwStates
long long g_culled = 0;
long long               g_inst_drawn = 0;
// [rtex] per model, parallel to g_models: each atomic's part code (BuildClump
// out_atomic_part) and the materials RaceSubmit_SetupCarEnv turned into ENVMAP, with the
// env frame they share.
std::vector<std::vector<std::int32_t>> g_model_parts;
// [rtex] and each atomic's own material index (BuildClump out_atomic_mat). Every atomic's
// geometry carries the model's WHOLE material list (BuildClump AppendMaterials), so the
// material an atomic draws is matList.materials[this], not the whole list.
std::vector<std::vector<std::uint32_t>> g_model_amat;
// [carlod] and each atomic's RENDER bit as BuildClump left it: a copy without a record state
// draws exactly these, a car copy draws these AND what CarPartVisible gives its part code.
std::vector<std::vector<std::uint8_t>> g_model_render0;
// [carlod] 1 once a model has had a car copy: only those models need their RENDER bits
// re-set for every copy (props never leave the BuildClump state, so they skip the loop).
std::vector<std::uint8_t> g_model_vis_used;
struct ModelEnv { std::vector<rw::Material*> mats; rw::Frame* frame = nullptr; };
std::vector<ModelEnv>   g_model_env;
// [rprops U-9696] sky copies queued this frame: drawn BEFORE the world with depth test, depth write
// and fog off (the original's sky pass, FUN_004492b0 render states 8 and 6 = 0, 0x00427110..0x00427124).
std::vector<Inst>       g_sky;

// [rstates] RenderWare world-pass data (PREREG_rstates.md F2). Per registered model, the
// owning DFF atomic of each rw::Atomic BuildClump made (one per batch), grouped per DFF
// atomic in creation (= material) order, plus each DFF atomic's stream bounding sphere.
struct ModelRw {
    std::vector<std::vector<rw::Atomic*>>       by_dff;
    std::vector<Track::DffModel::AtomicSphere>  spheres;
};
std::vector<ModelRw> g_model_rw;   // parallel to g_models

struct WorldWalk {
    std::vector<Track::PlaneNode> planes;
    std::int32_t                  root = -1;
    std::vector<float>            cell;      // 6 per sector: inf xyz, sup xyz (FUN_004e51e0)
    std::vector<rw::Atomic*>      sector_atomic;
    bool                          ok = false;
};
WorldWalk       g_walk;
RaceCourseFacts g_facts;

// Build the camera once. frameBuffer is a plain Raster::CAMERA: on the D3D9
// backend such a raster has natras->texture == nil, and setRenderSurfaces then
// binds d3d9Globals.defaultRenderTarget (d3ddevice.cpp:953) — which, under
// adoption, IS the exe's backbuffer. The zBuffer raster resolves to
// defaultDepthSurf the same way (:1068). That is what makes librw draw into the
// frame the D3D9 path is already building, sharing its depth buffer, rather than
// into a private off-screen target.
bool BuildCamera() {
    g_cam = rw::Camera::create();
    if (!g_cam) { RLog("FAIL: Camera::create"); return false; }
    g_cam->setFrame(rw::Frame::create());
    g_cam->frameBuffer = rw::Raster::create(g_width, g_height, 0, rw::Raster::CAMERA);
    g_cam->zBuffer     = rw::Raster::create(g_width, g_height, 0, rw::Raster::ZBUFFER);
    if (!g_cam->frameBuffer || !g_cam->zBuffer) {
        RLog("FAIL: camera rasters (fb=%p zb=%p)",
             (void*)g_cam->frameBuffer, (void*)g_cam->zBuffer);
        return false;
    }
    g_cam->setProjection(rw::Camera::PERSPECTIVE);
    return true;
}

// Set the camera frame from eye/at, matching MatLookAtLH's basis so the two
// renderers frame the SAME shot. RW's convention is at = forward, and beginUpdate
// negates X to reach a left-handed view space (d3ddevice.cpp:1229-1234), so a
// right-handed cross-product basis built here lands consistently.
void SetCameraLookAt(const float eye[3], const float at_pt[3]) {
    float fwd[3] = { at_pt[0] - eye[0], at_pt[1] - eye[1], at_pt[2] - eye[2] };
    float len = std::sqrt(fwd[0]*fwd[0] + fwd[1]*fwd[1] + fwd[2]*fwd[2]);
    if (len < 1e-6f) { fwd[0] = 0.f; fwd[1] = 0.f; fwd[2] = 1.f; len = 1.f; }
    fwd[0] /= len; fwd[1] /= len; fwd[2] /= len;

    const float wup[3] = { 0.f, 1.f, 0.f };
    float right[3] = { wup[1]*fwd[2] - wup[2]*fwd[1],
                       wup[2]*fwd[0] - wup[0]*fwd[2],
                       wup[0]*fwd[1] - wup[1]*fwd[0] };
    float rl = std::sqrt(right[0]*right[0] + right[1]*right[1] + right[2]*right[2]);
    if (rl < 1e-6f) { right[0] = 1.f; right[1] = 0.f; right[2] = 0.f; rl = 1.f; }
    right[0] /= rl; right[1] /= rl; right[2] /= rl;

    const float up[3] = { fwd[1]*right[2] - fwd[2]*right[1],
                          fwd[2]*right[0] - fwd[0]*right[2],
                          fwd[0]*right[1] - fwd[1]*right[0] };

    // [D-S3-4 REVERTED, 2026-08-16] Do NOT cancel librw's view-space X negation.
    //
    // beginUpdate builds the view matrix as inverse(LTM) with the X COMPONENT of
    // every basis row negated (d3ddevice.cpp:1229-1240) -- world->view carries a
    // built-in mirror in X. D-S3-4 read that as a librw quirk and negated `right`
    // here to cancel it, on the measurement that doing so moved librw's output
    // toward the D3D9 path (mean-abs 25.37 -> 15.41).
    //
    // That measurement was real and the conclusion drawn from it was wrong: the
    // D3D9 path was itself mirrored relative to the ORIGINAL, so D-S3-4 tuned
    // librw to agree with a bad reference. librw is a RenderWare implementation,
    // and its built-in X negation IS the original's convention -- it was right
    // all along and got compensated into being wrong.
    //
    // Established 2026-08-16 by transplanting the original's own RwCamera basis
    // into the standalone and comparing against an original capture at that pose:
    // verbatim basis 89.68%, right-axis negated 33.79%. TrackRenderer's
    // MatLookAtLH/MatViewFromBasis now carry the same negation, so both paths
    // share the original's handedness and this compensation must go.
    // Evidence: verify/d1_basis/RESULT.md, verify/d1_mirrorfix/RESULT.md.
    //
    // Still outstanding: the E2'b step 2 probe (RwSceneBuild.cpp
    // RenderWorldProbe) builds its basis the same un-compensated way. Under the
    // corrected reading its world_probe_arctic.png was never mirrored -- the
    // note claiming it was is wrong and is superseded here.
    const float right_lh[3] = { right[0], right[1], right[2] };

    rw::Frame*  cf = g_cam->getFrame();
    rw::Matrix* m  = &cf->matrix;
    m->right.x = right_lh[0]; m->right.y = right_lh[1]; m->right.z = right_lh[2];
    m->up.x    = up[0];    m->up.y    = up[1];    m->up.z    = up[2];
    m->at.x    = fwd[0];   m->at.y    = fwd[1];   m->at.z    = fwd[2];
    m->pos.x   = eye[0];   m->pos.y   = eye[1];   m->pos.z   = eye[2];
    m->update();
    cf->updateObjects();
}

// Set the camera frame from a full ORTHONORMAL BASIS rather than an eye/at pair.
// SetCameraLookAt above rebuilds `up` as cross(fwd, cross(worldUp, fwd)), which
// pins up to the world +Y plane and throws roll away. The original's race camera
// has roll, so for a transplanted pose that reconstruction is lossy and the
// static world (which librw draws) came out level while D3D9's sky came out
// banked -- measured 2026-08-30, verify/parity_race_20260830/noroll.
//
// The basis is written VERBATIM, with no negation of `right`. It is read straight
// off the original's RwCamera frame and librw's camera frame is the same kind of
// object, so the two conventions already agree; librw's own X negation in
// beginUpdate IS the original's convention (see the [D-S3-4 REVERTED] block
// above -- verbatim basis measured 89.68% against right-axis-negated 33.79%).
void SetCameraBasis(const float pos[3], const float right[3],
                    const float up[3], const float at_dir[3]) {
    rw::Frame*  cf = g_cam->getFrame();
    rw::Matrix* m  = &cf->matrix;
    m->right.x = right[0];  m->right.y = right[1];  m->right.z = right[2];
    m->up.x    = up[0];     m->up.y    = up[1];     m->up.z    = up[2];
    m->at.x    = at_dir[0]; m->at.y    = at_dir[1]; m->at.z    = at_dir[2];
    m->pos.x   = pos[0];    m->pos.y   = pos[1];    m->pos.z   = pos[2];
    m->update();
    cf->updateObjects();
}

}  // namespace

bool RaceSubmit_Active() { return g_engine_up && g_scene_up; }

bool RaceSubmit_Init(HWND hwnd, IDirect3DDevice9* dev, int width, int height) {
    if (g_engine_up) return true;
    if (std::FILE* f = std::fopen(kRaceLog, "w")) std::fclose(f);
    RLog("# librw in-loop submit (E2'b step 3): static world + instanced props and cars");
    RLog("# sky, copters, particles, pickups and HUD come from the D3D9 path");

    g_width  = width;
    g_height = height;
    g_hwnd_dbg = hwnd;
    if (!EngineStartAdopted(hwnd, dev, width, height)) {
        RLog("FAIL: EngineStartAdopted");
        return false;
    }
    if (!BuildCamera()) { EngineStop(); return false; }
    if (FAILED(dev->CreateStateBlock(D3DSBT_ALL, &g_state_block)))
        g_state_block = nullptr;   // non-fatal; D-S3-2 just stays open
    RLog("ok: state block %s", g_state_block ? "created" : "UNAVAILABLE");
    rw::d3d9::rw_mashed_rasteralpha = &TexelAlphaHook;   // [rtex] MatFX env pass choice
    g_engine_up = true;
    RLog("ok: init (%dx%d)", width, height);
    return true;
}

void RaceSubmit_Shutdown() {
    if (!g_engine_up) return;
    // Lights are owned by the world once added; destroy the world last.
    if (g_state_block) { g_state_block->Release(); g_state_block = nullptr; }
    if (g_cam)   { g_cam->destroy();   g_cam   = nullptr; }
    for (rw::Clump* c : g_models) if (c) c->destroy();
    g_models.clear(); g_model_rw.clear(); g_insts.clear();
    if (g_world) { g_world->destroy(); g_world = nullptr; }
    g_amb = g_sun = nullptr;
    g_scene_up = false;
    EngineStop();
    g_engine_up = false;
    RLog("ok: shutdown after %lld frames, %lld instance draws", g_frames, g_inst_drawn);
}

bool RaceSubmit_OnTrackLoaded(const Track::World& world,
                              const Txd::Dictionary* dicts, std::size_t ndicts) {
    if (!g_engine_up) return false;

    // A track reload replaces the scene wholesale.
    if (g_world) { g_world->destroy(); g_world = nullptr; g_amb = g_sun = nullptr; }
    g_scene_up = false;

    TextureSource ts{ dicts, (int)ndicts };
    std::vector<void*> sector_atomics;
    g_world = static_cast<rw::World*>(BuildWorld(world, ts, &sector_atomics));
    if (!g_world) { RLog("FAIL: BuildWorld"); return false; }

    // [rstates] The BSP and each sector's CELL box, as MASHED.exe's FUN_004e51e0 builds
    // it at world load: copy the world box (world+0x50), walk the tree, narrow the right
    // child to inf[type] = rightValue (fld [edx+0x14] 0x004e5241 -> 0x004e5259) and the
    // left child to sup[type] = leftValue (fld [edx+0x10] 0x004e525c -> 0x004e5262), and
    // store the running box into every atomic sector (rep movsd to sector+0x48,
    // 0x004e5219/0x004e5226). The frustum walk tests this box, not the stream box.
    g_walk = WorldWalk{};
    g_walk.planes = world.planes;
    g_walk.root   = world.root;
    g_walk.cell.assign(world.sectors.size() * 6, 0.f);
    g_walk.sector_atomic.assign(world.sectors.size(), nullptr);
    for (std::size_t i = 0; i < sector_atomics.size() && i < world.sectors.size(); ++i)
        g_walk.sector_atomic[i] = static_cast<rw::Atomic*>(sector_atomics[i]);
    {
        struct Item { std::int32_t node; float inf[3], sup[3]; };
        Item top{ world.root, {}, {} };
        for (int k = 0; k < 3; ++k) {   // header box order is sup,inf; take it per component
            top.inf[k] = std::fmin(world.bbox[k], world.bbox[k + 3]);
            top.sup[k] = std::fmax(world.bbox[k], world.bbox[k + 3]);
        }
        std::vector<Item> stack{ top };
        bool bad = world.root == -1 && world.sectors.empty();
        while (!stack.empty() && !bad) {
            Item it = stack.back(); stack.pop_back();
            if (it.node < 0) {
                const std::size_t s = static_cast<std::size_t>(-1 - it.node);
                if (s >= world.sectors.size()) { bad = true; break; }
                for (int k = 0; k < 3; ++k) {
                    g_walk.cell[s * 6 + k]     = it.inf[k];
                    g_walk.cell[s * 6 + 3 + k] = it.sup[k];
                }
                continue;
            }
            if ((std::size_t)it.node >= world.planes.size()) { bad = true; break; }
            const Track::PlaneNode& p = world.planes[(std::size_t)it.node];
            const int c = p.type / 4;
            Item l = it, r = it;
            l.node = p.left;  l.sup[c] = p.left_value;
            r.node = p.right; r.inf[c] = p.right_value;
            stack.push_back(r);
            stack.push_back(l);
        }
        g_walk.ok = !bad && !world.planes.empty();
        RLog("rstates: world walk %s planes=%zu sectors=%zu root=%d",
             g_walk.ok ? "ready" : "UNAVAILABLE", world.planes.size(), world.sectors.size(),
             (int)world.root);
    }

    // Bind the camera to the world. This is what installs worldBeginUpdateCB,
    // which sets engine->currentWorld (camera.cpp:258-261) -- and any geometry
    // carrying rw::Geometry::LIGHT dereferences that in lightingCB_Shader
    // (d3drender.cpp:358). The static world sectors have no normals and no LIGHT
    // flag, so they never touched it; props and cars DO, which is why submitting
    // them segfaulted until the camera was added here. Re-added on every rebuild
    // because the old world is destroyed above.
    // Bind the camera to the world. Required: worldBeginUpdateCB sets
    // engine->currentWorld (camera.cpp:258-261), which lightingCB_Shader
    // dereferences for LIGHT-flagged geometry (d3drender.cpp:358) -- props and
    // cars segfault without it. MEASURED harmless to the world-only path: with
    // this unconditional and props still on D3D9, the gating shots were unchanged
    // (0.93/0.39/0.93), which is what exonerated it as the cause of D-S3-6.
    g_world->addCamera(g_cam);

    g_scene_up = true;
    RLog("ok: scene built — sectors=%zu mats=%zu tris=%u verts=%u (dicts=%zu)",
         world.sectors.size(), world.materials.size(),
         world.total_tris, world.total_verts, ndicts);
    return true;
}

// ---------------------------------------------------------------------------
// F3 UV animation on the librw path
//
// The D3D9 path scrolls UV-animated materials with a texture transform
// (TrackRenderer.cpp:3839-3843 / :3968-3980, offset = fmod(rate*t, 1)). That
// lever does not exist here: librw's d3d9 shader pipeline passes input.TexCoord
// through untouched (default_VS.hlsl:26) and ignores D3DTSS_TEXTURETRANSFORMFLAGS
// entirely, and librw's UVAnim plugin is stream-only -- nothing under src/d3d/
// references it. So the coordinates themselves are moved.
//
// Cost is one texcoord re-upload per animated atomic per frame, NOT a full
// re-instance: setting lockedSinceInst to LOCKTEXCOORDS makes d3d9.cpp:416 call
// instanceCB(reinstance=1), and every other block there is guarded by its own
// lock bit (:588 vertices, :598 prelight, :627 normals), so only the texcoord
// loop at :616 runs. Geometry::lock() is deliberately NOT used: it is fine for
// this flag today, but it also frees meshHeader for LOCKPOLYGONS, and setting
// the one bit we mean is clearer than relying on which flags it special-cases.
// ---------------------------------------------------------------------------
namespace {

struct UvAnimAtomic {
    rw::Atomic*                 atomic = nullptr;
    float                       du = 0.f, dv = 0.f;
    std::vector<rw::TexCoords>  base;    // authored UVs; offsets apply to these
};
// Parallel to g_models: per model, the atomics that actually scroll.
std::vector<std::vector<UvAnimAtomic>> g_uvanim;
float g_anim_t = 0.f;
int   g_uvanim_count = 0;   // total scrolling atomics, for the log

void RegisterUvAnim(rw::Clump* c, const std::vector<std::uint32_t>& atomic_mat,
                    const float* uv_rates, std::size_t nmats) {
    g_uvanim.emplace_back();
    if (!uv_rates || nmats == 0) return;
    std::vector<UvAnimAtomic>& out = g_uvanim.back();

    std::size_t ai = 0;
    FORLIST(lnk, c->atomics) {
        rw::Atomic* a = rw::Atomic::fromClump(lnk);
        if (ai >= atomic_mat.size()) break;
        const std::uint32_t mi = atomic_mat[ai++];
        if (mi >= nmats) continue;
        const float du = uv_rates[mi * 2 + 0], dv = uv_rates[mi * 2 + 1];
        if (du == 0.f && dv == 0.f) continue;
        rw::Geometry* g = a->geometry;
        if (!g || g->numTexCoordSets < 1 || !g->texCoords[0]) continue;
        UvAnimAtomic e;
        e.atomic = a; e.du = du; e.dv = dv;
        e.base.assign(g->texCoords[0], g->texCoords[0] + g->numVertices);
        out.push_back(std::move(e));
        ++g_uvanim_count;
    }
}

// Re-derive every animated atomic's UVs from its AUTHORED base, never from last
// frame's values: accumulating a per-frame delta would drift, and the D3D9 path
// it must match is itself absolute (fmod(rate * t, 1) of a fixed rate).
void ApplyUvAnim() {
    for (std::vector<UvAnimAtomic>& model : g_uvanim)
        for (UvAnimAtomic& e : model) {
            rw::Geometry* g = e.atomic ? e.atomic->geometry : nullptr;
            if (!g || !g->texCoords[0]) continue;
            const float ou = std::fmod(e.du * g_anim_t, 1.f);
            const float ov = std::fmod(e.dv * g_anim_t, 1.f);
            const std::int32_t n =
                g->numVertices < (std::int32_t)e.base.size()
                    ? g->numVertices : (std::int32_t)e.base.size();
            for (std::int32_t i = 0; i < n; ++i) {
                g->texCoords[0][i].u = e.base[i].u + ou;
                g->texCoords[0][i].v = e.base[i].v + ov;
            }
            g->lockedSinceInst |= rw::Geometry::LOCKTEXCOORDS;
        }
}

}  // namespace

void RaceSubmit_SetAnimTime(float t) { g_anim_t = t; }

void RaceSubmit_BeginTrackLoad() {
    if (!g_engine_up) return;
    for (rw::Clump* c : g_models) if (c) c->destroy();
    g_models.clear();
    g_model_rw.clear();
    g_insts.clear();
    g_sky.clear();
    g_uvanim.clear();
    g_uvanim_count = 0;
    for (ModelEnv& e : g_model_env) if (e.frame) e.frame->destroy();
    g_model_env.clear();
    g_model_parts.clear();
    g_model_amat.clear();
    g_model_frames.clear();   // owned by their clumps, destroyed above
    g_model_bind.clear();
    g_model_aframe.clear();
    g_model_order.clear();
    g_model_rwstates.clear();
    g_model_render0.clear();
    g_model_vis_used.clear();
}

void RaceSubmit_SetCourseFacts(const RaceCourseFacts& facts) {
    g_facts = facts;
    RLog("rstates: course facts sector_keeps_vertex_alpha=%d sky_last_vertex_alpha=%d rain=%d",
         (int)facts.sector_keeps_vertex_alpha, (int)facts.sky_last_vertex_alpha,
         (int)facts.rain);
}

int RaceSubmit_RegisterModel(const Track::DffModel& model,
                             const Txd::Dictionary* dicts, std::size_t ndicts,
                             std::uint32_t ambient,
                             const float* uv_rates, std::size_t nmats, bool hierarchy) {
    if (!g_engine_up) return -1;
    TextureSource ts{ dicts, (int)ndicts };
    std::vector<std::uint32_t> atomic_mat;
    std::vector<std::int32_t>  atomic_part;   // [rtex]
    std::vector<void*>         frames;        // [librwrest]
    std::vector<std::int32_t>  atomic_frame;  // [librwrest]
    std::vector<std::int32_t>  atomic_dff;    // [rstates]
    rw::Clump* c = static_cast<rw::Clump*>(
        BuildClump(model, ts, ambient, &atomic_mat, &atomic_part, hierarchy, &frames,
                   &atomic_frame, &atomic_dff));
    if (!c) { RLog("WARN: BuildClump failed -- model is NOT drawn (no D3D9 fallback)"); return -1; }
    RegisterUvAnim(c, atomic_mat, uv_rates, nmats);
    {
        // [rstates] group the clump's rw::Atomics by owning DFF atomic (creation order).
        ModelRw mr;
        mr.spheres = model.atomic_spheres;
        mr.by_dff.resize(model.atomic_spheres.size());
        std::size_t ai = 0;
        FORLIST(lnk, c->atomics) {
            rw::Atomic* a = rw::Atomic::fromClump(lnk);
            if (ai < atomic_dff.size() && atomic_dff[ai] >= 0) {
                const std::size_t d = (std::size_t)atomic_dff[ai];
                if (d >= mr.by_dff.size()) mr.by_dff.resize(d + 1);
                mr.by_dff[d].push_back(a);
            }
            ++ai;
        }
        g_model_rw.push_back(std::move(mr));
    }
    // Deliberately NOT added to the rw::World: World::render() walks the clump
    // list and would draw every registered model once, at its authored transform,
    // regardless of how many copies are actually placed this frame. Instanced
    // models are drawn explicitly below instead.
    g_models.push_back(c);
    g_model_parts.push_back(std::move(atomic_part));
    g_model_amat.push_back(atomic_mat);
    {
        std::vector<std::uint8_t> r0;
        FORLIST(lnk, c->atomics) {
            rw::Atomic* a = rw::Atomic::fromClump(lnk);
            r0.push_back((a->object.object.flags & rw::Atomic::RENDER) ? 1 : 0);
        }
        g_model_render0.push_back(std::move(r0));
        g_model_vis_used.push_back(0);
        // [carlod] a vehicle (part 0x13 present): list each atomic's part code, triangles and
        // material count, for PREREG_carlod's part table (evidence only)
        const std::vector<std::int32_t>& pc = g_model_parts.back();
        bool is_car = false;
        for (std::int32_t p : pc) if (p == 0x13) is_car = true;
        if (is_car) {
            int ai = 0;
            FORLIST(lnk, c->atomics) {
                rw::Atomic* a = rw::Atomic::fromClump(lnk);
                RLog("[carlod] model[%d] atomic %d part %#x tris %d mats %d render0 %d",
                     (int)g_models.size() - 1, ai,
                     ai < (int)pc.size() ? pc[(std::size_t)ai] : -1,
                     a->geometry ? (int)a->geometry->numTriangles : -1,
                     a->geometry ? (int)a->geometry->matList.numMaterials : -1,
                     (int)g_model_render0.back()[(std::size_t)ai]);
                ++ai;
            }
        }
    }
    g_model_env.emplace_back();
    g_model_frames.emplace_back();
    g_model_bind.emplace_back();
    g_model_aframe.push_back(std::move(atomic_frame));
    g_model_order.emplace_back();
    g_model_rwstates.push_back(false);
    for (void* f : frames) {
        rw::Frame* rf = static_cast<rw::Frame*>(f);
        g_model_frames.back().push_back(rf);
        g_model_bind.back().push_back(rf ? rf->matrix : rw::Matrix{});
    }
    RLog("model[%d] registered: atomics=%d uvanim=%d frames=%d", (int)g_models.size() - 1,
         (int)c->countAtomics(),
         g_uvanim.empty() ? 0 : (int)g_uvanim.back().size(), (int)frames.size());
    return (int)g_models.size() - 1;
}

void RaceSubmit_SetModelDrawOrder(int handle, const std::int32_t* frames, std::size_t n,
                                  std::int32_t cull_frame) {
    if (handle < 0 || (std::size_t)handle >= g_models.size()) return;
    rw::Clump* c = g_models[(std::size_t)handle];
    const std::vector<std::int32_t>& af = g_model_aframe[(std::size_t)handle];
    std::vector<rw::Atomic*> atoms;
    FORLIST(lnk, c->atomics) atoms.push_back(rw::Atomic::fromClump(lnk));
    DrawOrder& o = g_model_order[(std::size_t)handle];
    o.seq.clear();
    o.cull = nullptr;
    std::vector<bool> used(atoms.size(), false);
    for (std::size_t k = 0; k < n; ++k)
        for (std::size_t i = 0; i < atoms.size() && i < af.size(); ++i)
            if (!used[i] && af[i] == frames[k]) { o.seq.push_back(atoms[i]); used[i] = true; }
    if (!o.seq.empty())
        for (std::size_t i = 0; i < atoms.size(); ++i)
            if (!used[i]) o.seq.push_back(atoms[i]);
    for (std::size_t i = 0; i < atoms.size() && i < af.size(); ++i)
        if (cull_frame >= 0 && af[i] == cull_frame) { o.cull = atoms[i]; break; }
    RLog("model[%d] draw order: %zu atomics, cull=%s", handle, o.seq.size(),
         o.cull ? "on" : "off");
}

void RaceSubmit_SetModelRwStates(int handle) {
    if (handle < 0 || (std::size_t)handle >= g_models.size()) return;
    g_model_rwstates[(std::size_t)handle] = true;
    int nset = 0, nalpha = 0;
    FORLIST(lnk, g_models[(std::size_t)handle]->atomics) {
        rw::Atomic* a = rw::Atomic::fromClump(lnk);
        rw::Geometry* g = a ? a->geometry : nullptr;
        if (!g) continue;
        for (rw::int32 i = 0; i < g->matList.numMaterials; ++i) {
            rw::Material* m = g->matList.materials[i];
            rw::Raster* r = (m && m->texture) ? m->texture->raster : nullptr;
            if (!r || r->platform != rw::PLATFORM_D3D9) continue;
            const int v = RasterTexelAlpha(r);
            if (v < 0) continue;
            GETD3DRASTEREXT(r)->hasAlpha = v;   // macro: rwd3d.h:307
            ++nset; nalpha += v;
        }
    }
    RLog("model[%d] RW driver states: %d raster flags set (%d alpha)", handle, nset, nalpha);
}

void RaceSubmit_AddInstancePosed(int handle, const float* m44, const FramePose* poses,
                                 std::size_t npose, float sun_scale, float env_coef,
                                 const float* env_m44, int kind, const CarVis* vis) {
    if (handle < 0 || !m44 || (std::size_t)handle >= g_models.size()) return;
    Inst i; i.model = handle; i.kind = kind;
    for (int k = 0; k < 16; ++k) i.m[k] = m44[k];
    if (vis) { i.vis = true; i.vis_state = vis->state; i.vis_flags = vis->flags; }   // [carlod]
    if (env_m44 && (std::size_t)handle < g_model_env.size() &&
        !g_model_env[(std::size_t)handle].mats.empty()) {
        i.fx = true;
        i.env_coef = env_coef;
        for (int k = 0; k < 16; ++k) i.env_m[k] = env_m44[k];
    }
    const std::size_t nf = g_model_frames[(std::size_t)handle].size();
    for (std::size_t p = 0; p < npose; ++p)
        if (poses[p].frame >= 0 && (std::size_t)poses[p].frame < nf) i.poses.push_back(poses[p]);
    i.sun_scale = sun_scale;
    g_insts.push_back(std::move(i));
}

void RaceSubmit_AddCarFxMarker() {
    Inst i; i.model = kCarFxMarker;
    g_insts.push_back(std::move(i));
}

void RaceSubmit_AddInstance(int handle, const float* m44, int kind, const CarVis* vis) {
    if (handle < 0 || !m44 || (std::size_t)handle >= g_models.size()) return;
    Inst i; i.model = handle; i.kind = kind;
    for (int k = 0; k < 16; ++k) i.m[k] = m44[k];
    if (vis) { i.vis = true; i.vis_state = vis->state; i.vis_flags = vis->flags; }
    g_insts.push_back(i);
}

// [rtex] as above, plus the car's env coefficient and env frame matrix for this copy.
void RaceSubmit_AddInstanceEnv(int handle, const float* m44, float env_coef,
                               const float* env_m44, const CarVis* vis) {
    if (handle < 0 || !m44 || (std::size_t)handle >= g_models.size()) return;
    Inst i; i.model = handle;
    for (int k = 0; k < 16; ++k) i.m[k] = m44[k];
    if (vis) { i.vis = true; i.vis_state = vis->state; i.vis_flags = vis->flags; }
    if (env_m44 && (std::size_t)handle < g_model_env.size() &&
        !g_model_env[(std::size_t)handle].mats.empty()) {
        i.fx = true;
        i.env_coef = env_coef;
        for (int k = 0; k < 16; ++k) i.env_m[k] = env_m44[k];
    }
    g_insts.push_back(i);
}

// [rtex] FUN_0041ecc0 @ 0x0041ecc0: every material with MatFX effect 4 (DUAL) gets the
// dirt texture -- parts 0..3 / 0x21..0x24 the wheel one, the rest the body one
// (CarDirtIsWheelPart). The two textures are unnamed C4444 rasters built from dirt.piz.
int RaceSubmit_BindCarDirt(int handle,
                           const std::uint8_t* body_bgra, std::uint32_t bw, std::uint32_t bh,
                           const std::uint8_t* wheel_bgra, std::uint32_t ww, std::uint32_t wh,
                           std::uint32_t filter_addressing) {
    if (handle < 0 || (std::size_t)handle >= g_models.size()) return 0;
    rw::Texture* body = static_cast<rw::Texture*>(
        TextureFromBgra4444(body_bgra, bw, bh, filter_addressing));
    rw::Texture* wheel = static_cast<rw::Texture*>(
        TextureFromBgra4444(wheel_bgra, ww, wh, filter_addressing));
    if (!body || !wheel) {
        RLog("WARN: car dirt textures not built (body=%p wheel=%p)", (void*)body, (void*)wheel);
        if (body) body->destroy();
        if (wheel) wheel->destroy();
        return 0;
    }
    rw::Clump* c = g_models[(std::size_t)handle];
    const std::vector<std::int32_t>& parts = g_model_parts[(std::size_t)handle];
    const std::vector<std::uint32_t>& amat = g_model_amat[(std::size_t)handle];
    int bound = 0, ai = 0, nbody = 0, nwheel = 0;
    FORLIST(lnk, c->atomics) {
        rw::Atomic* a = rw::Atomic::fromClump(lnk);
        const int part = ai < (int)parts.size() ? parts[(std::size_t)ai] : -1;
        const std::uint32_t mi = ai < (int)amat.size() ? amat[(std::size_t)ai] : 0xffffffffu;
        ++ai;
        rw::Geometry* g = a->geometry;
        if (!g || mi >= (std::uint32_t)g->matList.numMaterials) continue;
        rw::Material* m = g->matList.materials[mi];
        rw::MatFX* fx = m ? rw::MatFX::get(m) : nullptr;
        if (!fx || fx->getEffectIndex(rw::MatFX::DUAL) < 0) continue;
        const bool w = CarDirtIsWheelPart(part);
        fx->setDualTexture(w ? wheel : body);
        ++bound; w ? ++nwheel : ++nbody;
    }
    body->destroy();    // the materials hold their own references (setDualTexture addRef)
    wheel->destroy();
    RLog("model[%d] car dirt bound: %d dual materials (%d body, %d wheel)", handle, bound,
         nbody, nwheel);
    return bound;
}

// [rtex] FUN_00420420 @ 0x00420420: for each atomic whose part code FUN_0041f360 lists,
// MatFX on the atomic (FUN_0053fb70) and every material set to ENVMAP (FUN_0053fbb0(mat, 2))
// with the env texture, a per-car frame and fbAlpha 0 (FUN_0053fd30(mat, tex, frame, 0, coef)).
// The texture is the vehicle TXD's entry named by the vehicle table's +0x30 (`cargloss`).
int RaceSubmit_SetupCarEnv(int handle, const Txd::Dictionary* dicts, std::size_t ndicts,
                           const char* env_tex_name) {
    if (handle < 0 || (std::size_t)handle >= g_models.size()) return 0;
    rw::Texture* env = nullptr;
    for (std::size_t d = 0; d < ndicts && !env; ++d)
        for (std::uint32_t i = 0; i < dicts[d].count(); ++i)
            if (_stricmp(dicts[d].texture(i).name, env_tex_name) == 0) {
                env = static_cast<rw::Texture*>(TextureFromTxdTexture(dicts[d].texture(i)));
                break;
            }
    if (!env) { RLog("WARN: car env texture '%s' not in the vehicle TXD", env_tex_name); return 0; }
    ModelEnv& me = g_model_env[(std::size_t)handle];
    if (!me.frame) me.frame = rw::Frame::create();
    rw::Clump* c = g_models[(std::size_t)handle];
    const std::vector<std::int32_t>& parts = g_model_parts[(std::size_t)handle];
    const std::vector<std::uint32_t>& amat = g_model_amat[(std::size_t)handle];
    int ai = 0, n = 0;
    FORLIST(lnk, c->atomics) {
        rw::Atomic* a = rw::Atomic::fromClump(lnk);
        const int part = ai < (int)parts.size() ? parts[(std::size_t)ai] : -1;
        const std::uint32_t mi = ai < (int)amat.size() ? amat[(std::size_t)ai] : 0xffffffffu;
        ++ai;
        rw::Geometry* g = a->geometry;
        if (!CarEnvIsEnvPart(part) || !g || mi >= (std::uint32_t)g->matList.numMaterials) continue;
        rw::Material* m = g->matList.materials[mi];
        if (!m) continue;
        rw::MatFX::enableEffects(a);
        rw::MatFX::setEffects(m, rw::MatFX::ENVMAP);
        rw::MatFX* fx = rw::MatFX::get(m);
        fx->setEnvTexture(env);
        fx->setEnvFrame(me.frame);
        fx->setEnvFBAlpha(0);
        fx->setEnvCoefficient(0.f);
        me.mats.push_back(m);
        ++n;
    }
    env->destroy();
    RLog("model[%d] car env set up: %d materials, texture '%s'", handle, n, env_tex_name);
    return n;
}

void RaceSubmit_AddSky(int handle, const float* m44) {
    if (handle < 0 || !m44 || (std::size_t)handle >= g_models.size()) return;
    Inst i; i.model = handle;
    for (int k = 0; k < 16; ++k) i.m[k] = m44[k];
    g_sky.push_back(i);
}

namespace {
// A D3DMATRIX and an rw::Matrix agree field-for-field: rows 1..4 are
// right/up/at/pos, each an xyz triple (D3D's 4th column is the affine w, unused
// for a rigid transform). So the transform the D3D9 path would have used copies
// straight across -- no reconstruction, no chance of the two drifting.
void SetClumpTransform(rw::Clump* c, const float* m) {
    rw::Frame* f = c->getFrame();
    if (!f) return;
    rw::Matrix* d = &f->matrix;
    d->right.x = m[0];  d->right.y = m[1];  d->right.z = m[2];
    d->up.x    = m[4];  d->up.y    = m[5];  d->up.z    = m[6];
    d->at.x    = m[8];  d->at.y    = m[9];  d->at.z    = m[10];
    d->pos.x   = m[12]; d->pos.y   = m[13]; d->pos.z   = m[14];
    d->update();
    f->updateObjects();
}

// ---------------------------------------------------------------------------
// [rstates] RenderWare's world pass, ported (PREREG_rstates.md F2-F5). Every rule cites
// MASHED.exe; the Python model verify/rstates_20261016/bsp_order.py of the same walk
// reproduced the original's recorded sector sequence on 9/9 d1r1 frames.
// ---------------------------------------------------------------------------

// Knobs: unset -> the compiled default; "0" -> off; anything else -> on. All four are DEFAULT
// ON since rstates (2026-10-16): they passed PREREG_rstates.md section 3 on the 9 d1r1 frames
// (verify/rstates_20261016/RESULT_rstates.md); "=0" restores the d1r1 behaviour per fix.
bool Knob(const char* name, bool def) {
    const char* e = std::getenv(name);
    if (!e || !*e) return def;
    return !(e[0] == '0' && e[1] == '\0');
}
bool SectorWalkOn() { static const bool on = Knob("MASHED_LIBRW_SECTORWALK", true); return on; }
bool StickyVaOn()   { static const bool on = Knob("MASHED_LIBRW_STICKYVA",   true); return on; }
bool RainCullOn()   { static const bool on = Knob("MASHED_LIBRW_RAINCULL",   true); return on; }
bool ClumpOrderOn() { static const bool on = Knob("MASHED_LIBRW_CLUMPORDER", true); return on; }

// The camera's six frustum planes, outward normals, `n.p - d > 0` = outside, and the box of
// its eight near/far corners. This is what the camera sync leaves at camera+0x94 (6 x 0x14:
// normal, distance, closest-corner bytes) and +0x10c/+0x118 (frustumBoundBox sup/inf) before the
// sync hook 0x004e38a0 reads them, for a perspective camera with a zero view offset.
struct Frustum {
    float n[6][3], d[6];
    float inf[3], sup[3];
    float eye[3];
    float vwx;
};

void MakeFrustum(Frustum* f) {
    const rw::Matrix* m = g_cam->getFrame()->getLTM();
    const float r[3] = { m->right.x, m->right.y, m->right.z };
    const float u[3] = { m->up.x, m->up.y, m->up.z };
    const float a[3] = { m->at.x, m->at.y, m->at.z };
    const float p[3] = { m->pos.x, m->pos.y, m->pos.z };
    const float vx = g_cam->viewWindow.x, vy = g_cam->viewWindow.y;
    const float zn = g_cam->nearPlane, zf = g_cam->farPlane;
    auto dot = [](const float* x, const float* y) { return x[0]*y[0] + x[1]*y[1] + x[2]*y[2]; };
    for (int k = 0; k < 3; ++k) { f->inf[k] = 1e30f; f->sup[k] = -1e30f; f->eye[k] = p[k]; }
    for (int iz = 0; iz < 2; ++iz) {
        const float z = iz ? zf : zn;
        for (int sx = -1; sx <= 1; sx += 2)
            for (int sy = -1; sy <= 1; sy += 2)
                for (int k = 0; k < 3; ++k) {
                    const float c = p[k] + a[k] * z + r[k] * (sx * vx * z) + u[k] * (sy * vy * z);
                    if (c < f->inf[k]) f->inf[k] = c;
                    if (c > f->sup[k]) f->sup[k] = c;
                }
    }
    for (int k = 0; k < 3; ++k) { f->n[0][k] = -a[k]; f->n[1][k] = a[k]; }
    f->d[0] = -(dot(a, p) + zn);
    f->d[1] = dot(a, p) + zf;
    int i = 2;
    for (int s = -1; s <= 1; s += 2, ++i) {
        float nn[3] = { s * r[0] - vx * a[0], s * r[1] - vx * a[1], s * r[2] - vx * a[2] };
        const float l = std::sqrt(dot(nn, nn));
        for (int k = 0; k < 3; ++k) f->n[i][k] = nn[k] / l;
        f->d[i] = dot(f->n[i], p);
    }
    for (int s = -1; s <= 1; s += 2, ++i) {
        float nn[3] = { s * u[0] - vy * a[0], s * u[1] - vy * a[1], s * u[2] - vy * a[2] };
        const float l = std::sqrt(dot(nn, nn));
        for (int k = 0; k < 3; ++k) f->n[i][k] = nn[k] / l;
        f->d[i] = dot(f->n[i], p);
    }
    f->vwx = vx;
}

// RW's sign tests here are integer tests on the float bits (`test reg,reg`), so -0.0 is "< 0".
inline bool Neg(float v) { return std::signbit(v); }

// The frame's sector list, as the camera sync hook 0x004e38a0 builds it. Explicit-stack descent from the root; a plane whose
// two sides the frustum box both reaches sends the camera's side first and pushes the other
// (0x004e3a5e..0x004e3aa4, front-to-back since world+0xc != 2), otherwise the one side
// reached is taken (0x004e3aa6..0x004e3ab3); an atomic sector is listed unless its cell box
// lies wholly outside one of the six planes, tested at the plane's closest corner
// (0x004e396b..0x004e3a2b).
void WalkSectors(const Frustum& f, std::vector<int>* out) {
    out->clear();
    if (!g_walk.ok) return;
    std::vector<std::int32_t> stack;
    std::int32_t node = g_walk.root;
    for (;;) {
        if (node >= 0) {
            const Track::PlaneNode& p = g_walk.planes[(std::size_t)node];
            const int c = p.type / 4;
            const float a = f.inf[c] - p.left_value;
            const float b = p.right_value - f.sup[c];
            if (Neg(a) && Neg(b)) {
                const bool cam_right = f.eye[c] > p.value;
                stack.push_back(cam_right ? p.left : p.right);
                node = cam_right ? p.right : p.left;
            } else {
                node = !Neg(a) ? p.right : p.left;
            }
            continue;
        }
        const std::size_t s = (std::size_t)(-1 - node);
        const float* box = &g_walk.cell[s * 6];
        bool outside = false;
        for (int i = 0; i < 6 && !outside; ++i) {
            float v = -f.d[i];
            for (int k = 0; k < 3; ++k)
                v += f.n[i][k] * (f.n[i][k] >= 0.f ? box[k] : box[3 + k]);
            outside = !Neg(v) && v != 0.f;    // `test edi,edi; setg cl`: bits > 0
        }
        if (!outside) out->push_back((int)s);
        if (stack.empty()) break;
        node = stack.back(); stack.pop_back();
    }
}

// The world sphere of DFF atomic `d` of an instance: centre through the instance matrix, the
// radius scaled by the matrix's largest axis (RpAtomicGetWorldBoundingSphere 0x004e6100:
// centre via RwV3dTransformPoints 0x004c3d60; radius * sqrt(max |axis|^2) unless the LTM is
// flagged orthonormal, where the scale is 1 anyway).
void WorldSphere(const Inst& in, const Track::DffModel::AtomicSphere& s, float c[3], float* r) {
    const float* m = in.m;
    for (int k = 0; k < 3; ++k)
        c[k] = s.c[0] * m[0 + k] + s.c[1] * m[4 + k] + s.c[2] * m[8 + k] + m[12 + k];
    float mx = 0.f;
    for (int ax = 0; ax < 3; ++ax) {
        const float l2 = m[ax*4]*m[ax*4] + m[ax*4+1]*m[ax*4+1] + m[ax*4+2]*m[ax*4+2];
        if (l2 > mx) mx = l2;
    }
    *r = s.r_world * std::sqrt(mx);
}

// Sphere test of FUN_004c1b40 (RwCameraFrustumTestSphere): outside when radius < n.c - d on any plane.
bool SphereOutside(const Frustum& f, const float c[3], float r) {
    for (int i = 0; i < 6; ++i) {
        const float v = f.n[i][2] * c[2] + f.n[i][0] * c[0] + f.n[i][1] * c[1] - f.d[i];
        if (r < v) return true;
    }
    return false;
}

// Distance test of FUN_004721e0, the render callback the atomic plugin ctor FUN_00472330 installs on every
// atomic (FUN_004722e0): draw only if |camera - centre|^2 * viewWindow.x < (radius / k)^2,
// k = RW's LUT sqrt (FUN_004c3b30) of the float 0x37826136, radius = atomic+0x28 (the
// unscaled stream radius), camera = the camera frame's modelling position (+0x40).
bool DistanceKeeps(const Frustum& f, const float c[3], float r_obj) {
    static const float k = [] {
        const std::uint32_t bits = 0x37826136u;
        float x; std::memcpy(&x, &bits, 4);
        return Math::LutSqrt(x);
    }();
    const double q = (double)r_obj / (double)k;
    const float thr = (float)(q * q);
    const float dx = f.eye[0] - c[0], dy = f.eye[1] - c[1], dz = f.eye[2] - c[2];
    const float d2 = (dz * dz + dx * dx + dy * dy) * f.vwx;
    return d2 < thr;
}

// Render one DFF atomic of an instance: every rw::Atomic built from its batches, in
// material order, honouring the RENDER flag as Clump::render does (clump.cpp:386).
int g_posed_inst = -1;
void PoseInstance(int ii) {
    if (g_posed_inst == ii) return;
    const Inst& in = g_insts[(std::size_t)ii];
    SetClumpTransform(g_models[(std::size_t)in.model], in.m);
    g_posed_inst = ii;
}
void RenderDffAtomic(int ii, std::size_t d) {
    const Inst& in = g_insts[(std::size_t)ii];
    const ModelRw& mr = g_model_rw[(std::size_t)in.model];
    if (d >= mr.by_dff.size()) return;
    PoseInstance(ii);
    for (rw::Atomic* a : mr.by_dff[d])
        if (a->object.object.flags & rw::Atomic::RENDER) a->render();
}

// MASHED_LIBRW_WALKLOG=<path>, DIAGNOSTIC ONLY: per frame, the sector list and the tied
// atomics drawn after each sector, so the walk can be checked against bsp_order.py.
std::FILE* WalkLog() {
    static std::FILE* f = [] {
        const char* e = std::getenv("MASHED_LIBRW_WALKLOG");
        return (e && *e) ? std::fopen(e, "w") : nullptr;
    }();
    return f;
}

// The world pass of RpWorldRender (0x004e5660 -> 0x004e47b0 over the camera's sector list,
// per sector 0x004e5680): draw the sector (FUN_004f0900 -> the world's sector callback), then every atomic
// on the sector's +0x38 tie list with the RENDER flag and renderFrame != now; it is stamped
// with renderFrame whether or not it passes, and drawn if its world sphere is not outside
// the frustum (FUN_004c1b40) -- through its own render callback, FUN_004721e0's distance
// test. Ties are rebuilt per instance per frame with 0x004e3c00's rule: box the world sphere
// (centre +- radius), walk the BSP taking the left child if inf < leftValue and the right if
// sup > rightValue (left first, right pushed, 0x004e3d71..0x004e3db3), and insert each tie at
// the HEAD of the sector's list (0x004e3d30..0x004e3d48). Instances are tied in queue order,
// each DFF atomic in RW clump order (newest first). [UNCERTAIN U-9709] the original's tie
// order comes from RW's frame-sync order, which is not decoded; this order is the one the
// recorded frames are checked against (RESULT_rstates.md).
void RenderWorldPass(const Frustum& fr, bool sticky) {
    std::vector<int> vis;
    WalkSectors(fr, &vis);
    const std::size_t ns = g_walk.sector_atomic.size();
    struct Tie { int inst; int dff; };
    struct WS { float c[3]; float r; float r_obj; bool stamped; };
    std::vector<std::vector<Tie>> ties(ns);
    std::vector<std::vector<WS>>  ws(g_insts.size());
    for (std::size_t ii = 0; ii < g_insts.size(); ++ii) {
        const Inst& in = g_insts[ii];
        if (in.kind != kInstInWorld || in.model < 0 ||
            (std::size_t)in.model >= g_model_rw.size())
            continue;
        const ModelRw& mr = g_model_rw[(std::size_t)in.model];
        const std::size_t nd = mr.spheres.size();
        ws[ii].resize(nd);
        for (std::size_t j = 0; j < nd; ++j) {
            const std::size_t d = nd - 1 - j;          // RW clump order: newest first
            WS& w = ws[ii][d];
            WorldSphere(in, mr.spheres[d], w.c, &w.r);
            w.r_obj = mr.spheres[d].r_obj;
            w.stamped = false;
            const float inf[3] = { w.c[0] - w.r, w.c[1] - w.r, w.c[2] - w.r };
            const float sup[3] = { w.c[0] + w.r, w.c[1] + w.r, w.c[2] + w.r };
            std::vector<std::int32_t> stack;
            std::int32_t node = g_walk.root;
            for (;;) {
                if (node >= 0) {
                    const Track::PlaneNode& p = g_walk.planes[(std::size_t)node];
                    const int c = p.type / 4;
                    const bool l = inf[c] < p.left_value;
                    const bool r = sup[c] > p.right_value;
                    if (l) {
                        if (r) stack.push_back(p.right);
                        node = p.left;
                        continue;
                    }
                    if (r) { node = p.right; continue; }
                } else {
                    const std::size_t s = (std::size_t)(-1 - node);
                    if (s < ns) ties[s].push_back({ (int)ii, (int)d });
                }
                if (stack.empty()) break;
                node = stack.back(); stack.pop_back();
            }
        }
    }
    // F3 / U-9689: the world-sector callback of this course keeps the vertex-alpha flag, so
    // the world starts with the one the slot-3 sky left.
    const bool keep_va = sticky && g_facts.sector_keeps_vertex_alpha;
    if (keep_va) rw::SetRenderState(rw::VERTEXALPHA, g_facts.sky_last_vertex_alpha ? 1 : 0);
    std::FILE* lg = WalkLog();
    if (lg) std::fprintf(lg, "f%lld vis", g_frames);
    for (int s : vis) {
        if (lg) std::fprintf(lg, " S%d", s);
        if (rw::Atomic* a = g_walk.sector_atomic[(std::size_t)s]) {
            rw_mashed_keep_vertex_alpha = keep_va;
            a->render();
            rw_mashed_keep_vertex_alpha = false;
        }
        const std::vector<Tie>& t = ties[(std::size_t)s];
        for (auto it = t.rbegin(); it != t.rend(); ++it) {   // head insertion
            WS& w = ws[(std::size_t)it->inst][(std::size_t)it->dff];
            if (w.stamped) continue;
            w.stamped = true;
            if (SphereOutside(fr, w.c, w.r)) continue;
            if (!DistanceKeeps(fr, w.c, w.r_obj)) continue;
            if (lg) std::fprintf(lg, " [m%d.%d]", g_insts[(std::size_t)it->inst].model, it->dff);
            RenderDffAtomic(it->inst, (std::size_t)it->dff);
        }
    }
    if (lg) { std::fputc('\n', lg); std::fflush(lg); }
}
}  // namespace

void RaceSubmit_Render(const Race::RaceSceneState& st) {
    if (!RaceSubmit_Active()) return;

    // [D-S3-2 FIX, outbound half] Snapshot the D3D9 pipeline state FIRST -- before
    // resyncDeviceState() below overwrites the device with librw's cached state.
    // Ordering is load-bearing and was got wrong once: capturing after the resync
    // snapshots librw's own state and "restores" that, which is the opposite of
    // the intent (measured: 01_action regressed 14.99 -> 39.25).
    //
    // MEASURED cause of the delta: librw's world pass leaves D3DRS_ALPHABLENDENABLE
    // set (ZSTATE@draw ablend=0 -> ZSTATE@after ablend=1), and exe_main draws the
    // HUD AFTER this submit -- so the pips lost their colour and the lap digit its
    // yellow. A state block beats restoring a hand-picked register list: librw
    // touches far more state than the few we happened to read.
    if (g_state_block) g_state_block->Capture();

    // [D-S3-1 FIX] The D3D9 path has been drawing with this device since our last
    // submit and has changed render states librw's write-back cache cannot see --
    // in particular TrackRenderer::Render() exits with D3DRS_ZENABLE=FALSE
    // (TrackRenderer.cpp:4151) so its 2D HUD draws. librw's cache still believed
    // ztest was on, so it issued no ZENABLE write and drew the entire world with
    // DEPTH TESTING OFF, painting over the player car. Re-push the cached state
    // before every submit. (The earlier suspicion that the two projections encode
    // depth differently was WRONG: RW builds proj[10]=far/(far-near), proj[11]=1,
    // proj[14]=-near*far/(far-near) at d3ddevice.cpp:1284-1290, which is
    // algebraically identical to MatPerspectiveFovLH.)
    rw::d3d::resyncDeviceState();

    // The resync makes the DEVICE agree with librw's cache -- it does not make
    // that cache correct. librw's own default leaves ztest off, so a resync alone
    // pushes ZENABLE=FALSE and the world still overdraws whatever D3D9 already
    // put in the frame. State the requirement explicitly, after the resync so the
    // request is not swallowed by a stale cache entry that already reads 1.
    // The depth BUFFER is genuinely shared: a Raster::ZBUFFER whose native
    // texture is already set binds d3d9Globals.defaultDepthSurf (d3ddevice.cpp
    // :1062-1069), which is the exe's depth surface under adoption. Verified by
    // experiment -- forcing that binding explicitly produced bit-identical output.
    rw::SetRenderState(rw::ZTESTENABLE, 1);
    rw::SetRenderState(rw::ZWRITEENABLE, 1);

    // ---- camera: read what TrackRenderer resolved, never re-derive it -------
    // Prefer the full basis when TrackRenderer published one: the eye/at pair
    // cannot carry roll, and re-deriving up from world +Y here is exactly the
    // "re-derive it" this comment forbids.
    if (st.last_basis_valid_)
        SetCameraBasis(st.last_eye_, st.last_right_, st.last_up_, st.last_atdir_);
    else
        SetCameraLookAt(st.last_eye_, st.last_at_);

    // RW expresses the frustum as a view WINDOW at unit distance, not an FOV:
    // half-height = tan(fov/2), half-width = that * aspect. This is the exact
    // algebraic equivalent of MatPerspectiveFovLH(last_fov_, last_aspect_, ...),
    // so both renderers get the same frustum from the same four numbers.
    {
        rw::V2d vw;
        const float half_h = std::tan(st.last_fov_ * 0.5f);
        vw.y = half_h;
        vw.x = half_h * st.last_aspect_;
        g_cam->setViewWindow(&vw);
    }
    g_cam->setNearPlane(st.last_near_);
    g_cam->setFarPlane(st.last_far_);
    // [d1r2] MASHED_CAM_DUMP_LIBRW=<csv>, DIAGNOSTIC ONLY: read back the camera frame
    // and view window librw actually renders with, keyed by the publisher's seq
    // (TrackRenderer MASHED_CAM_DUMP), so wiring is measured at the consumer.
    {
        static std::FILE* s_ld = [] {
            const char* e = std::getenv("MASHED_CAM_DUMP_LIBRW");
            std::FILE* f = (e && e[0]) ? std::fopen(e, "w") : nullptr;
            if (f) std::fprintf(f, "seq,src,valid,rx,ry,rz,ux,uy,uz,ax,ay,az,px,py,pz,vwx,vwy\n");
            return f; }();
        if (s_ld) {
            const rw::Matrix* m = &g_cam->getFrame()->matrix;
            std::fprintf(s_ld, "%u,%d,%d,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,"
                               "%.9g,%.9g,%.9g,%.9g,%.9g\n",
                         st.cam_pub_seq_, st.last_cam_src_, (int)st.last_basis_valid_,
                         m->right.x, m->right.y, m->right.z, m->up.x, m->up.y, m->up.z,
                         m->at.x, m->at.y, m->at.z, m->pos.x, m->pos.y, m->pos.z,
                         g_cam->viewWindow.x, g_cam->viewWindow.y);
            std::fflush(s_ld);
        }
    }

    // ---- fog: values parsed by TrackRenderer from COURSE.LUA Setup_Fog ------
    // [I4-fog] fogPlane is the ramp's NEAR end and is honoured as-is. The FAR end
    // is the one beginUpdate welds to cam->farPlane; it is corrected immediately
    // after that call, below. Before the correction the ramp ran 70 -> 643.6
    // instead of 70 -> 70's worth, leaving fogged surfaces ~1.4x too bright.
    // MASHED_NO_FOG gates the D3D9 device state (TrackRenderer.cpp:3916-3917) but
    // does NOT clear st.fog_on_, so without this librw kept fogging while the D3D9
    // path stopped -- a one-sided switch is useless as a control and actively
    // misleading. Honour it here too, so it turns fog off on BOTH paths at once.
    static const bool s_no_fog = (std::getenv("MASHED_NO_FOG") != nullptr);
    if (st.fog_on_ && !s_no_fog) {
        g_cam->fogPlane = st.fog_start_;
        rw::SetRenderState(rw::FOGENABLE, 1);
        // [P7 FIX] Byte order. COLOR_ARGB (rwd3d.h:53) packs 0xAARRGGBB -- D3D's
        // order -- but rw::SetRenderState(FOGCOLOR) does NOT take a D3DCOLOR: its
        // handler unpacks RGBA little-endian, `red = value; blue = value>>16`
        // (d3ddevice.cpp:672-676), i.e. it wants RED in the LOW byte. Passing a
        // D3D-ordered word therefore swapped red and blue in the fog colour that
        // reaches the pixel shader. Measured signature before the fix, on 48% of
        // the frame: signed (d3d9 - librw) = (-0.75, +0.02, +0.76) -- antisymmetric
        // in R/B with G exactly zero, which is (1-fogFactor)*(F - Fswapped) and
        // nothing else. Build the word with red low; the macro is still used, with
        // its r and b arguments crossed, so the packing stays in one place.
        // COLOR_ARGB is a MACRO (rwd3d.h:53), so it cannot be namespace-qualified.
        rw::SetRenderState(rw::FOGCOLOR,
                           COLOR_ARGB(255,
                                      (st.fog_color_)       & 0xFF,   // -> blue byte
                                      (st.fog_color_ >> 8)  & 0xFF,   // -> green
                                      (st.fog_color_ >> 16) & 0xFF)); // -> red (low)
    } else {
        rw::SetRenderState(rw::FOGENABLE, 0);
    }

    // ---- [d1r1 fix F1b] the original driver's alpha-test and cull state ------
    // librw initialises ALPHAFUNC GREATEREQUAL / ALPHAREF 10 / CULLMODE NONE
    // (d3ddevice.cpp:1787-1800). RenderWare 3.7's D3D9 driver in MASHED.exe programs, at
    // device-state init FUN_004d5bc0: ALPHAFUNC 5 = D3DCMP_GREATER (`mov edx,5` 0x004d60ec,
    // `push edx; push 0x19` 0x004d6102/0x004d6103 -- FUN_004d5480 touches only EAX/ECX, so
    // EDX still holds 5), ALPHAREF 0 (`xor esi,esi` 0x004d611a, `push esi; push 0x18`
    // 0x004d6122/0x004d6123), CULLMODE 2 = D3DCULL_CW (`push 2; push 0x16`
    // 0x004d6157/0x004d6159, cached in DAT_007d6b18 = RW cull mode 2, CULLBACK, which
    // FUN_004d7c10 maps 1:1 through 0x005d8d18). Measured on the original's in-race draws
    // (verify/d1r1_20261016): every blended draw ALPHAREF 0 / ALPHAFUNC 5, the world and car
    // bodies CULLMODE 2 (the world pass FUN_004492b0 sets RW cull 2 before rendering it).
    // DEFAULT ON since rstates (2026-10-16), MASHED_LIBRW_RWSTATES=0 opts out, together with
    // the raster alpha flag in RwRasterBridge.cpp (see the reason there and
    // verify/rstates_20261016/RESULT_rstates.md).
    {
        static const bool s_rwstates = [] {
            const char* e = std::getenv("MASHED_LIBRW_RWSTATES");
            return !(e && e[0] == '0' && e[1] == '\0');
        }();
        if (s_rwstates) {
            rw::SetRenderState(rw::ALPHATESTFUNC, rw::ALPHAGREATER);
            rw::SetRenderState(rw::ALPHATESTREF, 0);
            rw::SetRenderState(rw::CULLMODE, rw::CULLBACK);
        }
    }

    // ---- lights: float-precision values TrackRenderer already parsed --------
    // [DELTA I4-light] The static world carries baked prelight and NO vertex
    // normals (TrackRenderer's own note at the amb_world_ declaration), so the
    // DIRECTIONAL light cannot affect it in either renderer — it exists here for
    // when atomics (cars/props) start being submitted. Only the ambient term is
    // observable on this geometry today.
    if (!g_amb) {
        g_amb = rw::Light::create(rw::Light::AMBIENT);
        if (g_amb) {
            // [D-S3-6 FIX] World::enumerateLights SKIPS any light without
            // LIGHTATOMICS (world.cpp:162-163), and Light::create leaves flags 0.
            // With no flags set, lightData.ambient stayed (0,0,0) and every
            // LIGHT-flagged atomic -- i.e. every prop and car -- rendered BLACK,
            // while the normal-less world sectors were unaffected because they
            // never consult lighting at all. 0x3 = LIGHTATOMICS|LIGHTWORLD is also
            // the asset-verified value: Arctic's LIGHTS.DFF carries flags 0x3
            // (TrackRenderer's own note at the former sun_color_ declaration).
            g_amb->setFlags(rw::Light::LIGHTATOMICS | rw::Light::LIGHTWORLD);
            g_amb->setFrame(rw::Frame::create());
            g_world->addLight(g_amb);
        }
    }
    if (g_amb) g_amb->setColor(st.amb_f_[0], st.amb_f_[1], st.amb_f_[2]);

    if (!g_sun && st.has_sun_dir_) {
        g_sun = rw::Light::create(rw::Light::DIRECTIONAL);
        if (g_sun) {
            g_sun->setFlags(rw::Light::LIGHTATOMICS | rw::Light::LIGHTWORLD);
            g_sun->setFrame(rw::Frame::create());
            g_world->addLight(g_sun);
        }
    }
    if (g_sun) {
        g_sun->setColor(st.sun_f_[0], st.sun_f_[1], st.sun_f_[2]);
        // A directional light points along its frame's at-vector, which is
        // exactly what sun_dir_ holds (the direction the light travels).
        rw::Frame*  lf = g_sun->getFrame();
        rw::Matrix* m  = &lf->matrix;
        m->at.x = st.sun_dir_[0]; m->at.y = st.sun_dir_[1]; m->at.z = st.sun_dir_[2];
        // Any right/up orthogonal to at will do for a directional light.
        const float ax = std::fabs(st.sun_dir_[0]);
        const float ref[3] = { ax > 0.9f ? 0.f : 1.f, ax > 0.9f ? 1.f : 0.f, 0.f };
        float r[3] = { ref[1]*m->at.z - ref[2]*m->at.y,
                       ref[2]*m->at.x - ref[0]*m->at.z,
                       ref[0]*m->at.y - ref[1]*m->at.x };
        float rl = std::sqrt(r[0]*r[0] + r[1]*r[1] + r[2]*r[2]);
        if (rl < 1e-6f) { r[0] = 1.f; r[1] = 0.f; r[2] = 0.f; rl = 1.f; }
        r[0] /= rl; r[1] /= rl; r[2] /= rl;
        m->right.x = r[0]; m->right.y = r[1]; m->right.z = r[2];
        m->up.x = m->at.y*r[2] - m->at.z*r[1];
        m->up.y = m->at.z*r[0] - m->at.x*r[2];
        m->up.z = m->at.x*r[1] - m->at.y*r[0];
        m->pos.x = m->pos.y = m->pos.z = 0.f;
        m->update();
        lf->updateObjects();
    }

    // ---- submit ------------------------------------------------------------
    // NO clear: exe_main.cpp already cleared this backbuffer to fog_color_ and
    // the D3D9 path may have drawn into it. NO Begin/EndScene: the caller owns
    // the frame (both suppressed under adoption — MASHED_PATCHES.md P3).
    // MASHED_LIBRW_NODRAW=1 runs the whole path (engine, resync, camera, fog,
    // lights, scene) but submits NOTHING. It is the isolation control for
    // "does librw's draw hide the D3D9 car, or is the car not drawn at all?" --
    // a question four rounds of reading could not settle.
    static const bool s_nodraw = [] {
        const char* e = std::getenv("MASHED_LIBRW_NODRAW");
        return e && e[0] == '1' && e[1] == '\0';
    }();

    // D-S3-1: is the depth SURFACE actually shared? Measured, not assumed --
    // capture what D3D9 was rendering into, then what librw switches to.
    IDirect3DSurface9* dsBefore = nullptr;
    IDirect3DSurface9* rtBefore = nullptr;
    if (g_frames % 200 == 0 && rw::d3d::d3ddevice) {
        rw::d3d::d3ddevice->GetDepthStencilSurface(&dsBefore);
        rw::d3d::d3ddevice->GetRenderTarget(0, &rtBefore);
    }

    g_cam->beginUpdate();

    // [I4-fog CLOSED] beginUpdate has just welded the fog END to the far plane
    // (d3ddevice.cpp:1288, fogData.end = cam->farPlane). Undo that and state the
    // ramp COURSE.LUA's Setup_Fog actually asks for, which is what the D3D9 path
    // puts in D3DRS_FOGSTART/D3DRS_FOGEND. Must follow beginUpdate, not precede it.
    // See MASHED_PATCHES.md P6 for why the far plane is not shortened instead.
    // MASHED_LIBRW_FOGFIX=0 restores the old farPlane-welded ramp. Kept as the
    // same-binary control for this change -- the capture harness cannot tell a
    // real difference from a rebuild artefact without one (librw_ref MANIFEST).
    static const bool s_fogfix = [] {
        const char* e = std::getenv("MASHED_LIBRW_FOGFIX");
        return !(e && e[0] == '0' && e[1] == '\0');
    }();
    if (st.fog_on_ && s_fogfix && !std::getenv("MASHED_NO_FOG"))
        rw::d3d::setFogRange(st.fog_start_, st.fog_end_);

    if (dsBefore || rtBefore) {
        IDirect3DSurface9* dsAfter = nullptr;
        IDirect3DSurface9* rtAfter = nullptr;
        rw::d3d::d3ddevice->GetDepthStencilSurface(&dsAfter);
        rw::d3d::d3ddevice->GetRenderTarget(0, &rtAfter);
        RECT rc{}; GetClientRect(g_hwnd_dbg, &rc);
        RLog("f%-6lld SURFACES depth d3d9=%p librw=%p %s | rt d3d9=%p librw=%p %s"
             " | client=%dx%d raster=%dx%d",
             g_frames, (void*)dsBefore, (void*)dsAfter,
             dsBefore == dsAfter ? "SHARED" : "*** DIFFERENT ***",
             (void*)rtBefore, (void*)rtAfter,
             rtBefore == rtAfter ? "SHARED" : "*** DIFFERENT ***",
             (int)rc.right, (int)rc.bottom, g_width, g_height);
        if (dsBefore) dsBefore->Release();
        if (rtBefore) rtBefore->Release();
        if (dsAfter)  dsAfter->Release();
        if (rtAfter)  rtAfter->Release();
    }

    // ---- D-S3-1 depth probe -------------------------------------------------
    // A D24S8 depth surface is not lockable, so instead of reading the GPU's
    // depth back we compute, on the CPU, the clip-space coordinate each pipeline
    // produces for ONE world point -- using the very matrices each one uses:
    //   D3D9  : D3DTS_VIEW/D3DTS_PROJECTION still on the device (TrackRenderer
    //           set them at TrackRenderer.cpp:3757-3759; librw never calls
    //           SetTransform -- those calls are commented out in beginUpdate).
    //   librw : cam->devView / cam->devProj, which beginUpdate has just filled and
    //           which uploadMatrices multiplies per draw (d3drender.cpp:400-416).
    // Both use the row-vector convention (v' = v*M), RawMatrix rows being
    // right/up/at/pos, so the two are directly comparable.
    //
    // The discriminator: if x/w and y/w agree but z/w does NOT, the scene is
    // framed identically while the depths written are incompatible -- exactly the
    // signature D-S3-1 needs, and the last hypothesis standing.
    if (g_frames % 200 == 0 && st.car_ready_) {
        const float p[3] = { st.car_pos_[0], st.car_pos_[1], st.car_pos_[2] };

        auto xformD3D = [&](const D3DMATRIX& m, const float v[4], float o[4]) {
            o[0] = v[0]*m._11 + v[1]*m._21 + v[2]*m._31 + v[3]*m._41;
            o[1] = v[0]*m._12 + v[1]*m._22 + v[2]*m._32 + v[3]*m._42;
            o[2] = v[0]*m._13 + v[1]*m._23 + v[2]*m._33 + v[3]*m._43;
            o[3] = v[0]*m._14 + v[1]*m._24 + v[2]*m._34 + v[3]*m._44;
        };
        auto xformRw = [&](const rw::RawMatrix& m, const float v[4], float o[4]) {
            o[0] = v[0]*m.right.x + v[1]*m.up.x + v[2]*m.at.x + v[3]*m.pos.x;
            o[1] = v[0]*m.right.y + v[1]*m.up.y + v[2]*m.at.y + v[3]*m.pos.y;
            o[2] = v[0]*m.right.z + v[1]*m.up.z + v[2]*m.at.z + v[3]*m.pos.z;
            o[3] = v[0]*m.rightw  + v[1]*m.upw  + v[2]*m.atw  + v[3]*m.posw;
        };

        D3DMATRIX dv, dp;
        IDirect3DDevice9* dev = rw::d3d::d3ddevice;
        if (dev && SUCCEEDED(dev->GetTransform(D3DTS_VIEW, &dv)) &&
            SUCCEEDED(dev->GetTransform(D3DTS_PROJECTION, &dp))) {
            const float p4[4] = { p[0], p[1], p[2], 1.f };
            float a[4], b[4];
            xformD3D(dv, p4, a); xformD3D(dp, a, b);           // D3D9 clip
            float c[4], d[4];
            xformRw(g_cam->devView, p4, c); xformRw(g_cam->devProj, c, d);  // librw clip

            const float bw = (b[3] != 0.f) ? b[3] : 1e-6f;
            const float dw = (d[3] != 0.f) ? d[3] : 1e-6f;
            // %.4f is far too coarse to judge a near-tie: the whole question for
            // D-S3-BANK is whether two surfaces differ in depth by less than the
            // two pipelines disagree. Print enough digits to see that.
            RLog("f%-6lld DEPTHPROBE p=(%.2f,%.2f,%.2f)\n"
                 "        d3d9  ndc=(%.9f,%.9f,%.9f) w=%.6f\n"
                 "        librw ndc=(%.9f,%.9f,%.9f) w=%.6f\n"
                 "        dz=%.3e (ndc units)",
                 g_frames, p[0], p[1], p[2],
                 b[0]/bw, b[1]/bw, b[2]/bw, b[3],
                 d[0]/dw, d[1]/dw, d[2]/dw, d[3],
                 (double)(b[2]/bw) - (double)(d[2]/dw));

            // Both transforms are LINEAR, so comparing the combined matrices
            // settles for EVERY point at once whether the two pipelines can
            // produce different depths -- far stronger than sampling positions.
            // D3D9 combined = VIEW * PROJ; librw = devView * devProj.
            float dcomb[16], rcomb[16];
            for (int r = 0; r < 4; ++r) {
                const float row[4] = { r == 0 ? 1.f : 0.f, r == 1 ? 1.f : 0.f,
                                       r == 2 ? 1.f : 0.f, r == 3 ? 1.f : 0.f };
                float t1[4], t2[4];
                xformD3D(dv, row, t1); xformD3D(dp, t1, t2);
                for (int cq = 0; cq < 4; ++cq) dcomb[r * 4 + cq] = t2[cq];
                xformRw(g_cam->devView, row, t1); xformRw(g_cam->devProj, t1, t2);
                for (int cq = 0; cq < 4; ++cq) rcomb[r * 4 + cq] = t2[cq];
            }
            double maxabs = 0.0, maxrel = 0.0;
            int wi = -1;
            for (int i = 0; i < 16; ++i) {
                const double ad = std::fabs((double)dcomb[i] - (double)rcomb[i]);
                if (ad > maxabs) { maxabs = ad; wi = i; }
                const double mag = std::fabs((double)dcomb[i]);
                if (mag > 1e-9) {
                    const double rel = ad / mag;
                    if (rel > maxrel) maxrel = rel;
                }
            }
            RLog("        MATCMP max|d3d9-librw|=%.3e at elem %d (row %d col %d)"
                 "  maxrel=%.3e\n"
                 "        d3d9 col2 = %.9f %.9f %.9f %.9f\n"
                 "        librw col2= %.9f %.9f %.9f %.9f",
                 maxabs, wi, wi / 4, wi % 4, maxrel,
                 dcomb[2], dcomb[6], dcomb[10], dcomb[14],
                 rcomb[2], rcomb[6], rcomb[10], rcomb[14]);
        }
    }

    // Last unmeasured link in the D-S3-1 chain: what the DEVICE actually believes
    // about depth at the instant librw draws. Everything upstream is confirmed --
    // shared depth surface, identical clip-space z, live camera.
    if (g_frames % 200 == 0 && rw::d3d::d3ddevice) {
        DWORD ze = 0, zw = 0, zf = 0, cull = 0, ab = 0;
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ZENABLE, &ze);
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ZWRITEENABLE, &zw);
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ZFUNC, &zf);
        rw::d3d::d3ddevice->GetRenderState(D3DRS_CULLMODE, &cull);
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &ab);
        RLog("f%-6lld ZSTATE@draw zenable=%lu zwrite=%lu zfunc=%lu cull=%lu ablend=%lu",
             g_frames, ze, zw, zf, cull, ab);
    }

    // [rstates] RenderWare's world pass replaces the one-clump world render (F2).
    const bool walk = SectorWalkOn() && g_walk.ok;
    const bool sticky = StickyVaOn();
    g_posed_inst = -1;
    if (!s_nodraw) {
        if (sticky) rw_mashed_skip_empty_meshes = true;   // [rstates] F3
        // [rprops U-9696] The sky clump copies, before the world: depth test and write off, fog off
        // (FUN_004492b0 sets render states 8 and 6 to 0 around its sky RpClumpRender, and the generic
        // sky path 0x00427110..0x0042715b does the same). UVs first: the sky material scrolls, and
        // ApplyUvAnim is absolute in g_anim_t, so running it here and again below is idempotent.
        if (!g_sky.empty()) {
            ApplyUvAnim();
            rw::SetRenderState(rw::ZTESTENABLE, 0);
            rw::SetRenderState(rw::ZWRITEENABLE, 0);
            rw::SetRenderState(rw::FOGENABLE, 0);
            for (const Inst& in : g_sky) {
                if (in.model < 0 || (std::size_t)in.model >= g_models.size()) continue;
                rw::Clump* c = g_models[(std::size_t)in.model];
                if (!c) continue;
                SetClumpTransform(c, in.m);
                c->render();
            }
            rw::SetRenderState(rw::ZTESTENABLE, 1);
            rw::SetRenderState(rw::ZWRITEENABLE, 1);
            rw::SetRenderState(rw::FOGENABLE, (st.fog_on_ && !s_no_fog) ? 1 : 0);
        }
        if (!walk) g_world->render();   // [rstates] F2 replaces it with the sector walk
        // Instanced models. Each registered clump is re-posed and re-drawn once
        // per placed copy, which is what the D3D9 path does too (one
        // SetTransform + draw per entry in Prop::instances).
        if (g_frames % 200 == 0) {
            // D-S3-6: is clump[4] (the sea) actually SUBMITTED and DRAWN? Count
            // instances per model and show each one's translation, so "the sea is
            // black" can be separated from "the sea is never drawn" and from "the
            // sea is drawn somewhere else". Measure before theorising about shading.
            RLog("f%-6lld instances=%zu models=%zu | amb=(%.3f,%.3f,%.3f) sun=(%.3f,%.3f,%.3f) has_sun=%d",
                 g_frames, g_insts.size(), g_models.size(),
                 st.amb_f_[0], st.amb_f_[1], st.amb_f_[2],
                 st.sun_f_[0], st.sun_f_[1], st.sun_f_[2], (int)st.has_sun_dir_);
            for (std::size_t mi = 0; mi < g_models.size(); ++mi) {
                int n = 0; float px = 0, py = 0, pz = 0;
                for (const Inst& in : g_insts)
                    if ((std::size_t)in.model == mi) {
                        ++n;
                        if (n == 1) { px = in.m[12]; py = in.m[13]; pz = in.m[14]; }
                    }
                // D-S3-7: Clump::render() only draws atomics with the RENDER
                // flag (clump.cpp:386). Count how many of ours actually have it,
                // and report the live light colours -- the car has prelit=0, so
                // unlike the props its colour comes ENTIRELY from lighting.
                int rend = 0;
                if (g_models[mi]) {
                    FORLIST(lnk, g_models[mi]->atomics) {
                        rw::Atomic* a = rw::Atomic::fromClump(lnk);
                        if (a->object.object.flags & rw::Atomic::RENDER) ++rend;
                    }
                }
                RLog("    model[%zu] atomics=%d render_flagged=%d instances=%d first_pos=(%.2f,%.2f,%.2f)",
                     mi, g_models[mi] ? (int)g_models[mi]->countAtomics() : -1,
                     rend, n, px, py, pz);
            }
        }
        // D-S3-7 experiment: MASHED_LIBRW_LIFT=<metres> raises every instance in
        // Y. Reasoning about why the car is invisible has failed repeatedly while
        // every individual link measured correct, so lift the geometry into empty
        // sky: if a car appears there it IS rasterising and was hidden; if nothing
        // appears, it never rasterises at all. One observation, two hypotheses.
        static const float s_lift = [] {
            const char* e = std::getenv("MASHED_LIBRW_LIFT");
            return e ? (float)std::atof(e) : 0.0f;
        }();
        // F3: scroll UV-animated materials before anything is drawn. Once per
        // FRAME, not once per instance -- the geometry is shared by every placed
        // copy of a model, so doing it per instance would redo identical work and
        // (worse) re-upload texcoords between draws of the same buffer.
        ApplyUvAnim();

        Frustum fr{};
        if (walk) {
            MakeFrustum(&fr);
            RenderWorldPass(fr, sticky);
        }
        bool car_seen = false;
        bool fx_done = false;
        for (std::size_t ii = 0; ii < g_insts.size(); ++ii) {
            const Inst& in = g_insts[ii];
            // [rfx] the car FX marker (RaceSubmit_AddCarFxMarker): FUN_0041ebb0 draws the car
            // shadow and beam right after the car loop (0x00410bb4), before the copters
            // (FUN_00425e40, 0x00410c08)
            if (in.model == kCarFxMarker) {
                if (!fx_done) RaceFx_DrawCarFx(g_cam);
                fx_done = true;
                continue;
            }
            if (in.model < 0 || (std::size_t)in.model >= g_models.size()) continue;
            rw::Clump* c = g_models[(std::size_t)in.model];
            if (!c) continue;
            if (walk && in.kind == kInstInWorld) continue;   // drawn by the sector walk
            if (in.kind == kInstCar && !car_seen) {
                car_seen = true;
                // [rstates] F4 / U-9690: on RainEnable tracks FUN_004270f0 runs the line
                // pass FUN_004914b0 -> FUN_00499d90 after the world and the sea, and that
                // pass writes D3D CULLMODE NONE through the state queue
                // (FUN_004d5480(0x16,1), 0x0049a084) without touching RW's cull cache
                // DAT_007d6b18. Every car drawn after it renders with cull NONE while RW
                // still believes CULLBACK. Same mechanism here: write the device directly,
                // so librw's cache keeps CULLBACK and its later identical sets are no-ops.
                if (RainCullOn() && g_facts.rain && rw::d3d::d3ddevice)
                    rw::d3d::d3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            }
            // [rstates] A plain RpClumpRender'ed copy: not a car, no per-frame poses, no env
            // pass, no model draw order or per-model states (librwrest/rtex draw those below,
            // with their own order and culling: the cars' FUN_00420870 path, the copters'
            // FUN_00425e40).
            const DrawOrder* od0 = (std::size_t)in.model < g_model_order.size()
                                       ? &g_model_order[(std::size_t)in.model] : nullptr;
            const bool plain = in.kind != kInstCar && in.poses.empty() && !in.fx &&
                               !(od0 && (od0->cull || !od0->seq.empty())) &&
                               !((std::size_t)in.model < g_model_rwstates.size() &&
                                 g_model_rwstates[(std::size_t)in.model]);
            if (plain && (walk || ClumpOrderOn())) {
                // [rstates] per DFF atomic. RpClumpRender walks the clump list from the
                // head (0x004e6680) and RpClumpAddAtomic inserts at the head
                // (FUN_004e6f80), so RW draws the newest atomic first: the reverse of the
                // stream order (F5). Every atomic also carries FUN_004721e0's distance test
                // under the sector walk (F2).
                const ModelRw& mr = g_model_rw[(std::size_t)in.model];
                const std::size_t nd = mr.by_dff.size();
                const bool rw_order = ClumpOrderOn();
                for (std::size_t j = 0; j < nd; ++j) {
                    const std::size_t d = rw_order ? nd - 1 - j : j;
                    if (walk && d < mr.spheres.size()) {
                        float cc[3], rr;
                        WorldSphere(in, mr.spheres[d], cc, &rr);
                        if (!DistanceKeeps(fr, cc, mr.spheres[d].r_obj)) continue;
                    }
                    RenderDffAtomic((int)ii, d);
                }
                ++g_inst_drawn;
                continue;
            }
            g_posed_inst = -1;
            float mm[16];
            for (int k = 0; k < 16; ++k) mm[k] = in.m[k];
            mm[13] += s_lift;
            SetClumpTransform(c, mm);
            // [rtex] this car's env coefficient and frame (FUN_00420340 per car per frame)
            if (in.fx) {
                ModelEnv& me = g_model_env[(std::size_t)in.model];
                for (rw::Material* m : me.mats)
                    if (rw::MatFX* fx = rw::MatFX::get(m)) fx->setEnvCoefficient(in.env_coef);
                if (me.frame) {
                    rw::Matrix* d = &me.frame->matrix;
                    d->right.x = in.env_m[0];  d->right.y = in.env_m[1];  d->right.z = in.env_m[2];
                    d->up.x    = in.env_m[4];  d->up.y    = in.env_m[5];  d->up.z    = in.env_m[6];
                    d->at.x    = in.env_m[8];  d->at.y    = in.env_m[9];  d->at.z    = in.env_m[10];
                    d->pos.x   = in.env_m[12]; d->pos.y   = in.env_m[13]; d->pos.z   = in.env_m[14];
                    d->update();
                    me.frame->updateObjects();
                }
            }
            // [carlod] FUN_00420870 @ 0x00420870: every part atomic's RENDER bit (FUN_004b5240 @
            // 0x004b5240 sets / clears bit 4 of the byte at atomic+2) from this car's state and
            // flags, by part code. The clump is shared by all copies, so every copy sets all of
            // them: a copy without a record state gets the bits BuildClump left.
            if (in.vis) g_model_vis_used[(std::size_t)in.model] = 1;
            if (g_model_vis_used[(std::size_t)in.model]) {
                const std::vector<std::int32_t>& parts = g_model_parts[(std::size_t)in.model];
                const std::vector<std::uint8_t>& r0 = g_model_render0[(std::size_t)in.model];
                int ai = 0, nvis = 0;
                FORLIST(lnk, c->atomics) {
                    rw::Atomic* a = rw::Atomic::fromClump(lnk);
                    bool on = ai < (int)r0.size() && r0[(std::size_t)ai] != 0;
                    const int part = ai < (int)parts.size() ? parts[(std::size_t)ai] : -1;
                    if (in.vis && part >= 0)
                        on = on && CarPartVisible(in.vis_state, in.vis_flags, part);
                    if (on) a->object.object.flags |= rw::Atomic::RENDER;
                    else    a->object.object.flags &= ~rw::Atomic::RENDER;
                    nvis += on ? 1 : 0;
                    ++ai;
                }
                if (in.vis) {
                    // one line per distinct (state, flags) pair seen, to the race log
                    static std::vector<std::uint64_t> s_seen;
                    const std::uint64_t key = ((std::uint64_t)(unsigned)in.vis_state << 32) | in.vis_flags;
                    bool seen = false;
                    for (std::uint64_t k : s_seen) if (k == key) { seen = true; break; }
                    if (!seen) {
                        s_seen.push_back(key);
                        RLog("[carlod] car vis: state=%d flags=0x%x -> %d of %d atomics render (model[%d])",
                             in.vis_state, in.vis_flags, nvis, ai, in.model);
                    }
                }
            }
            // D-S3-7: does the transform actually reach the atomics? uploadMatrices
            // uses atomic->getFrame()->getLTM() (d3d9render.cpp:154), so THAT is
            // the number that matters -- not what we wrote into clump->frame.
            if (g_frames % 200 == 0 && in.model == 8) {
                rw::Atomic* a0 = rw::Atomic::fromClump(c->atomics.link.next);
                const rw::Matrix* al = a0 ? a0->getFrame()->getLTM() : nullptr;
                RLog("      D-S3-7 want=(%.2f,%.2f,%.2f) clumpframe=(%.2f,%.2f,%.2f) "
                     "atomicLTM=(%.2f,%.2f,%.2f)",
                     mm[12], mm[13], mm[14],
                     c->getFrame()->matrix.pos.x, c->getFrame()->matrix.pos.y,
                     c->getFrame()->matrix.pos.z,
                     al ? al->pos.x : -999.f, al ? al->pos.y : -999.f,
                     al ? al->pos.z : -999.f);
            }
            // [librwrest] this copy's frame poses (FUN_0041f3d0 / FUN_0041f4f0 / FUN_00425ca0
            // write the frames' modelling matrices; RW re-derives the LTMs at render)
            std::vector<rw::Frame*>* mf = (std::size_t)in.model < g_model_frames.size()
                                              ? &g_model_frames[(std::size_t)in.model] : nullptr;
            for (const FramePose& p : in.poses) {
                rw::Frame* f = (*mf)[(std::size_t)p.frame];
                if (!f) continue;
                rw::Matrix* d = &f->matrix;
                d->right.x = p.m[0]; d->right.y = p.m[1];  d->right.z = p.m[2];
                d->up.x    = p.m[3]; d->up.y    = p.m[4];  d->up.z    = p.m[5];
                d->at.x    = p.m[6]; d->at.y    = p.m[7];  d->at.z    = p.m[8];
                d->pos.x   = p.m[9]; d->pos.y   = p.m[10]; d->pos.z   = p.m[11];
                d->update();
                f->updateObjects();
            }
            // [librwrest] FUN_00420050 @ 0x00420050: the directional light's colour times
            // rec+0x284 (all four components, 0x004200ea..0x00420149) for this car only
            if (in.sun_scale >= 0.f && g_sun)
                g_sun->setColor(st.sun_f_[0] * in.sun_scale, st.sun_f_[1] * in.sun_scale,
                                st.sun_f_[2] * in.sun_scale);
            // [librwrest] the model's draw order / frustum cull (RaceSubmit_SetModelDrawOrder)
            const DrawOrder* od = (std::size_t)in.model < g_model_order.size()
                                      ? &g_model_order[(std::size_t)in.model] : nullptr;
            bool culled = false;
            if (od && od->cull)
                culled = g_cam->frustumTestSphere(od->cull->getWorldBoundingSphere()) ==
                         rw::Camera::SPHEREOUTSIDE;
            const bool rws = (std::size_t)in.model < g_model_rwstates.size() &&
                             g_model_rwstates[(std::size_t)in.model];
            rw::uint32 s_af = 0, s_ar = 0, s_cm = 0;
            if (rws && !culled) {
                s_af = rw::GetRenderState(rw::ALPHATESTFUNC);
                s_ar = rw::GetRenderState(rw::ALPHATESTREF);
                s_cm = rw::GetRenderState(rw::CULLMODE);
                rw::SetRenderState(rw::ALPHATESTFUNC, rw::ALPHAGREATER);
                rw::SetRenderState(rw::ALPHATESTREF, 0);
                rw::SetRenderState(rw::CULLMODE, rw::CULLBACK);
            }
            if (culled) {
                ++g_culled;
            } else if (od && !od->seq.empty()) {
                for (rw::Atomic* a : od->seq)
                    if (a->object.object.flags & rw::Atomic::RENDER) a->render();
                ++g_inst_drawn;
            } else {
                c->render();
                ++g_inst_drawn;
            }
            // [carlod] FUN_00420050 @ 0x00420050: then the glass atomics (RENDER bit clear above),
            // each through its own render callback, the first then the second, inside the same
            // light and render-state scope and only for a car that was not culled.
            if (!culled && in.vis) {
                const std::vector<std::int32_t>& gparts = g_model_parts[(std::size_t)in.model];
                const std::vector<std::uint8_t>& gr0 = g_model_render0[(std::size_t)in.model];
                for (int which = 0; which < 2; ++which) {
                    const int gp = CarGlassPart(in.vis_state, in.vis_flags, which);
                    if (gp < 0) continue;
                    int gi = 0;
                    FORLIST(glnk, c->atomics) {
                        rw::Atomic* ga = rw::Atomic::fromClump(glnk);
                        if (gi < (int)gparts.size() && gparts[(std::size_t)gi] == gp &&
                            gi < (int)gr0.size() && gr0[(std::size_t)gi]) ga->render();
                        ++gi;
                    }
                }
            }
            if (rws && !culled) {
                rw::SetRenderState(rw::ALPHATESTFUNC, s_af);
                rw::SetRenderState(rw::ALPHATESTREF, s_ar);
                rw::SetRenderState(rw::CULLMODE, s_cm);
            }
            if (in.sun_scale >= 0.f && g_sun)   // restored after the render (0x004201d7)
                g_sun->setColor(st.sun_f_[0], st.sun_f_[1], st.sun_f_[2]);
            for (const FramePose& p : in.poses) {   // back to the streamed matrices
                rw::Frame* f = (*mf)[(std::size_t)p.frame];
                if (!f) continue;
                f->matrix = g_model_bind[(std::size_t)in.model][(std::size_t)p.frame];
                f->updateObjects();
            }
        }
        // [rfx 2026-10-16] the car shadow and headlight beam (LibRw/RwRaceFx.h): FUN_0041ebb0
        // @ 0x0041ebb0 runs after the four car renders (FUN_00410b30, 0x00410bb9); here only when
        // no marker was queued.
        if (!fx_done) RaceFx_DrawCarFx(g_cam);
        // [rfx2] FUN_0040bde0 @ 0x0040bde0 (the object shadows FUN_00487de0 first), which
        // FUN_00410b30 calls after the copters (FUN_00425e40 at 0x00410c08, FUN_0040bde0 next)
        RaceFx_DrawWorldFx(g_cam);
        rw_mashed_skip_empty_meshes = false;
        rw_mashed_keep_vertex_alpha = false;
    }
    g_insts.clear();
    g_sky.clear();

    // ...and what the pipeline LEFT set, which is a proxy for what it used during
    // the draw. The pre-draw reading above is not enough: librw's default atomic
    // pipeline issues its own setRenderState per material.
    if (g_frames % 200 == 0 && rw::d3d::d3ddevice && !s_nodraw) {
        DWORD ze = 0, zw = 0, zf = 0, ab = 0;
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ZENABLE, &ze);
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ZWRITEENABLE, &zw);
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ZFUNC, &zf);
        rw::d3d::d3ddevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &ab);
        RLog("f%-6lld ZSTATE@after zenable=%lu zwrite=%lu zfunc=%lu ablend=%lu",
             g_frames, ze, zw, zf, ab);
    }

    g_cam->endUpdate();

    // [rfx 2026-10-16] the in-race HUD (team panels + start lights): FUN_00492e90 @ 0x00492e90 renders
    // FUN_0040dfc0 with the HUD camera after the race camera's update ends (LibRw/RwRaceFx.h).
    if (!s_nodraw) RaceFx_RenderHud(g_width, g_height);

    // Hand the device back exactly as the D3D9 path left it.
    if (g_state_block) g_state_block->Apply();

    // D-S3-1 instrumentation: is the camera INPUT actually changing per frame?
    if (g_frames % 200 == 0) {
        // Compare the INPUT (st.last_eye_) against what actually reaches the
        // shader: the camera frame's LTM position, and devView's translation row
        // (uploadMatrices multiplies devView*devProj per draw, d3drender.cpp:400).
        const rw::Matrix* ltm = g_cam->getFrame()->getLTM();
        RLog("f%-6lld in=(%.2f,%.2f,%.2f) ltm=(%.2f,%.2f,%.2f) devView.pos=(%.2f,%.2f,%.2f)",
             g_frames, st.last_eye_[0], st.last_eye_[1], st.last_eye_[2],
             ltm->pos.x, ltm->pos.y, ltm->pos.z,
             g_cam->devView.right.x, g_cam->devView.up.y, g_cam->devView.pos.z);
    }
    if (++g_frames == 1) {
        // [P7 liveness] Which depth does D3D9's TABLE fog actually use? The spec
        // makes it conditional on the device: W-based when D3DPRASTERCAPS_WFOG is
        // present AND a perspective projection is set, Z-based otherwise. The
        // per-pixel librw fog we now compute is written against eye-W, so this is
        // the premise it stands on -- logged, not assumed.
        D3DCAPS9 caps{};
        if (rw::d3d::d3ddevice &&
            SUCCEEDED(rw::d3d::d3ddevice->GetDeviceCaps(&caps)))
            RLog("P7 fogcaps: RasterCaps=%08lX WFOG=%d ZFOG=%d FOGTABLE=%d "
                 "FOGVERTEX=%d FOGRANGE=%d",
                 (unsigned long)caps.RasterCaps,
                 !!(caps.RasterCaps & D3DPRASTERCAPS_WFOG),
                 !!(caps.RasterCaps & D3DPRASTERCAPS_ZFOG),
                 !!(caps.RasterCaps & D3DPRASTERCAPS_FOGTABLE),
                 !!(caps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX),
                 !!(caps.RasterCaps & D3DPRASTERCAPS_FOGRANGE));
        RLog("P7 fogconst: start=%.3f end=%.3f range=%.6f disable=%.1f",
             rw::d3d::d3dShaderState.fogData.start,
             rw::d3d::d3dShaderState.fogData.end,
             rw::d3d::d3dShaderState.fogData.range,
             rw::d3d::d3dShaderState.fogData.disable);
        // [P7 FIX liveness] The PS fog colour must equal the D3DCOLOR the D3D9
        // path puts in D3DRS_FOGCOLOR. Print both; a byte-order slip is visible
        // here as a swapped pair, so this does not have to be inferred from pixels.
        RLog("P7 fogcolor: d3d9=%02X%02X%02X librw_ps=(%.4f,%.4f,%.4f) %s",
             (unsigned)((st.fog_color_ >> 16) & 0xFF),
             (unsigned)((st.fog_color_ >> 8) & 0xFF),
             (unsigned)(st.fog_color_ & 0xFF),
             rw::d3d::d3dShaderState.fogColor.red,
             rw::d3d::d3dShaderState.fogColor.green,
             rw::d3d::d3dShaderState.fogColor.blue,
             (((int)(rw::d3d::d3dShaderState.fogColor.red   * 255.0f + 0.5f) ==
                     (int)((st.fog_color_ >> 16) & 0xFF)) &&
              ((int)(rw::d3d::d3dShaderState.fogColor.blue  * 255.0f + 0.5f) ==
                     (int)(st.fog_color_ & 0xFF))) ? "MATCH" : "*** MISMATCH ***");
        RLog("ok: first frame submitted — eye=(%.2f,%.2f,%.2f) at=(%.2f,%.2f,%.2f) "
             "fov=%.4f aspect=%.4f near=%.3f far=%.1f fog=%d[%.1f..%.1f]",
             st.last_eye_[0], st.last_eye_[1], st.last_eye_[2],
             st.last_at_[0], st.last_at_[1], st.last_at_[2],
             st.last_fov_, st.last_aspect_, st.last_near_, st.last_far_,
             (int)st.fog_on_, st.fog_start_, st.fog_end_);
    }
}

}  // namespace LibRw
}  // namespace mashed_re
