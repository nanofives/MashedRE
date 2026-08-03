// LibRw/RwRaceSubmit.cpp — in-loop librw submit path. See RwRaceSubmit.h.
//
// EXE-ONLY isolated TU (build.bat). Includes <rw.h>; nothing else may.

#include "RwRaceSubmit.h"

#include "RwBridge.h"
#include "RwSceneBuild.h"

#define WITH_D3D
#include <rw.h>

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "../Race/RaceSceneState.h"
#include "../Track/TrackWorld.h"
#include "../Txd/TxdDecoder.h"

namespace mashed_re {
namespace LibRw {
namespace {

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
struct Inst { int model; float m[16]; };
std::vector<Inst>       g_insts;
long long               g_inst_drawn = 0;

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

    // [D-S3-4 FIX] Cancel librw's view-space X negation.
    //
    // beginUpdate builds the view matrix as inverse(LTM) with the X COMPONENT of
    // every basis row negated (d3ddevice.cpp:1229-1240) -- i.e. world->view
    // carries a built-in mirror in X. Handing it the plain D3D-LookAtLH basis
    // above therefore renders the whole scene horizontally mirrored. Negating
    // `right` (and only `right`, leaving `up` derived from the un-negated one so
    // the frame stays orthonormal and upright) puts a matching mirror into the
    // LTM, and the two cancel.
    //
    // MEASURED, not reasoned into place: with the frame as-built, mirroring the
    // captured image dropped mean-abs against the D3D9 control from 25.37 to
    // 15.41 -- so the delta really was a horizontal flip and not a camera
    // position error. NOTE the E2'b step 2 probe (RwSceneBuild.cpp
    // RenderWorldProbe) builds its basis the same un-compensated way, so its
    // world_probe_arctic.png is mirrored too; it was only ever checked
    // structurally, never against a reference.
    const float right_lh[3] = { -right[0], -right[1], -right[2] };

    rw::Frame*  cf = g_cam->getFrame();
    rw::Matrix* m  = &cf->matrix;
    m->right.x = right_lh[0]; m->right.y = right_lh[1]; m->right.z = right_lh[2];
    m->up.x    = up[0];    m->up.y    = up[1];    m->up.z    = up[2];
    m->at.x    = fwd[0];   m->at.y    = fwd[1];   m->at.z    = fwd[2];
    m->pos.x   = eye[0];   m->pos.y   = eye[1];   m->pos.z   = eye[2];
    m->update();
    cf->updateObjects();
}

}  // namespace

bool RaceSubmit_Requested() {
    const char* e = std::getenv("MASHED_RENDER_LIBRW");
    return e && e[0] == '1' && e[1] == '\0';
}

bool RaceSubmit_Active() { return g_engine_up && g_scene_up; }

bool RaceSubmit_Init(HWND hwnd, IDirect3DDevice9* dev, int width, int height) {
    if (g_engine_up) return true;
    if (std::FILE* f = std::fopen(kRaceLog, "w")) std::fclose(f);
    RLog("# librw in-loop submit (E2'b step 3) — STATIC WORLD ONLY");
    RLog("# cars, props, particles, pickups, sky and HUD still come from the");
    RLog("# D3D9 path, so imgdiff deltas against verify/librw_ref/ are SCOPE.");

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
    g_models.clear(); g_insts.clear();
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
    g_world = static_cast<rw::World*>(BuildWorld(world, ts));
    if (!g_world) { RLog("FAIL: BuildWorld"); return false; }

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

// DEFAULT ON since 2026-08-02. It was staged off while the instanced path ran a
// uniform ~1.5x bright on fogged surfaces; that turned out to be missing UV
// ANIMATION (D-S3-SEA), not shading, and carrying the scroll over took the gating
// shots from 0.06-7.12 to 0.06-0.59 -- at or below the world-only path. Note this
// only takes effect when the librw renderer is on at all (MASHED_RENDER_LIBRW);
// with no env set the shipping D3D9 path still runs and still diffs 0.00.
// MASHED_LIBRW_INST=0 reverts to world-only.
bool RaceSubmit_InstancesEnabled() {
    static const bool on = [] {
        const char* e = std::getenv("MASHED_LIBRW_INST");
        return !(e && e[0] == '0' && e[1] == 0);
    }();
    return on;
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
    g_insts.clear();
    g_uvanim.clear();
    g_uvanim_count = 0;
}

int RaceSubmit_RegisterModel(const Track::DffModel& model,
                             const Txd::Dictionary* dicts, std::size_t ndicts,
                             std::uint32_t ambient,
                             const float* uv_rates, std::size_t nmats) {
    if (!g_engine_up) return -1;
    TextureSource ts{ dicts, (int)ndicts };
    std::vector<std::uint32_t> atomic_mat;
    rw::Clump* c = static_cast<rw::Clump*>(
        BuildClump(model, ts, ambient, &atomic_mat));
    if (!c) { RLog("WARN: BuildClump failed -- model stays on the D3D9 path"); return -1; }
    RegisterUvAnim(c, atomic_mat, uv_rates, nmats);
    // Deliberately NOT added to the rw::World: World::render() walks the clump
    // list and would draw every registered model once, at its authored transform,
    // regardless of how many copies are actually placed this frame. Instanced
    // models are drawn explicitly below instead.
    g_models.push_back(c);
    RLog("model[%d] registered: atomics=%d uvanim=%d", (int)g_models.size() - 1,
         (int)c->countAtomics(),
         g_uvanim.empty() ? 0 : (int)g_uvanim.back().size());
    return (int)g_models.size() - 1;
}

void RaceSubmit_AddInstance(int handle, const float* m44) {
    if (handle < 0 || !m44 || (std::size_t)handle >= g_models.size()) return;
    Inst i; i.model = handle;
    for (int k = 0; k < 16; ++k) i.m[k] = m44[k];
    g_insts.push_back(i);
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
        // COLOR_ARGB is a MACRO (rwd3d.h:41), so it cannot be namespace-qualified.
        rw::SetRenderState(rw::FOGCOLOR,
                           COLOR_ARGB(255,
                                      (st.fog_color_ >> 16) & 0xFF,
                                      (st.fog_color_ >> 8)  & 0xFF,
                                      (st.fog_color_)       & 0xFF));
    } else {
        rw::SetRenderState(rw::FOGENABLE, 0);
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

    if (!s_nodraw) {
        g_world->render();
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

        for (const Inst& in : g_insts) {
            if (in.model < 0 || (std::size_t)in.model >= g_models.size()) continue;
            rw::Clump* c = g_models[(std::size_t)in.model];
            if (!c) continue;
            float mm[16];
            for (int k = 0; k < 16; ++k) mm[k] = in.m[k];
            mm[13] += s_lift;
            SetClumpTransform(c, mm);
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
            c->render();
            ++g_inst_drawn;
        }
    }
    g_insts.clear();

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
    if (++g_frames == 1)
        RLog("ok: first frame submitted — eye=(%.2f,%.2f,%.2f) at=(%.2f,%.2f,%.2f) "
             "fov=%.4f aspect=%.4f near=%.3f far=%.1f fog=%d[%.1f..%.1f]",
             st.last_eye_[0], st.last_eye_[1], st.last_eye_[2],
             st.last_at_[0], st.last_at_[1], st.last_at_[2],
             st.last_fov_, st.last_aspect_, st.last_near_, st.last_far_,
             (int)st.fog_on_, st.fog_start_, st.fog_end_);
}

}  // namespace LibRw
}  // namespace mashed_re
