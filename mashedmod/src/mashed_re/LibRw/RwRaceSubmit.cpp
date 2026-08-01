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

    rw::Frame*  cf = g_cam->getFrame();
    rw::Matrix* m  = &cf->matrix;
    m->right.x = right[0]; m->right.y = right[1]; m->right.z = right[2];
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
    if (!EngineStartAdopted(hwnd, dev, width, height)) {
        RLog("FAIL: EngineStartAdopted");
        return false;
    }
    if (!BuildCamera()) { EngineStop(); return false; }
    g_engine_up = true;
    RLog("ok: init (%dx%d)", width, height);
    return true;
}

void RaceSubmit_Shutdown() {
    if (!g_engine_up) return;
    // Lights are owned by the world once added; destroy the world last.
    if (g_cam)   { g_cam->destroy();   g_cam   = nullptr; }
    if (g_world) { g_world->destroy(); g_world = nullptr; }
    g_amb = g_sun = nullptr;
    g_scene_up = false;
    EngineStop();
    g_engine_up = false;
    RLog("ok: shutdown after %lld submitted frames", g_frames);
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

    g_scene_up = true;
    RLog("ok: scene built — sectors=%zu mats=%zu tris=%u verts=%u (dicts=%zu)",
         world.sectors.size(), world.materials.size(),
         world.total_tris, world.total_verts, ndicts);
    return true;
}

void RaceSubmit_Render(const Race::RaceSceneState& st) {
    if (!RaceSubmit_Active()) return;

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
    // [DELTA I4-fog] RW ties the fog END to the camera's FAR plane
    // (d3ddevice.cpp:1289-1291: fogData.end = cam->farPlane), so fog_end_ cannot
    // be honoured independently of the clip distance the way D3D9's discrete
    // D3DRS_FOGEND can. We set fogPlane = fog_start_ and leave farPlane at the
    // clip distance; the resulting fog ramp is therefore LONGER than the D3D9
    // path's whenever fog_end_ < last_far_ (Arctic: 70 vs radius*8). Recorded as
    // an I4 delta, not silently approximated.
    if (st.fog_on_) {
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
        if (g_amb) { g_amb->setFrame(rw::Frame::create()); g_world->addLight(g_amb); }
    }
    if (g_amb) g_amb->setColor(st.amb_f_[0], st.amb_f_[1], st.amb_f_[2]);

    if (!g_sun && st.has_sun_dir_) {
        g_sun = rw::Light::create(rw::Light::DIRECTIONAL);
        if (g_sun) { g_sun->setFrame(rw::Frame::create()); g_world->addLight(g_sun); }
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
    g_cam->beginUpdate();
    g_world->render();
    g_cam->endUpdate();

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
