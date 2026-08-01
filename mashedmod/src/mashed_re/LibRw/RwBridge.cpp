// LibRw/RwBridge.cpp -- bring vendored librw up on the standalone's window.
//
// Lane M3-E1' (gate D2). See re/analysis/LIBRW_SIZING_2026-08.md.
//
// THE ONLY TU that includes <rw.h>. Compiled in isolation with the librw include
// path into a dedicated .obj (mashedmod/build.bat), mirroring how
// Collision/QhullBridge.cpp contains vendored qhull. Do not include rw.h anywhere
// else; the containment is the point.
//
// Startup sequence mirrors librw's own skeleton (skeleton/skeleton.cpp:11-54 in
// the pristine upstream clone at re/prior_art/renderware/librw), which is the
// reference for correct init ordering:
//     Engine::init -> register plugins -> Engine::open(&params)
//     -> setSubSystem -> Engine::start -> Charset::open
// Plugins MUST be registered between init and open: they claim per-object plugin
// offsets, and objects created after open would otherwise be laid out without
// them.

#include "RwBridge.h"   // pulls <windows.h> first, which rwd3d.h needs for HWND
#include "RwRasterBridge.h"
#include "RwSceneBuild.h"

// WITH_D3D makes librw's rwd3d.h include <d3d9.h> and expose the D3D9-typed part
// of its interface -- notably `extern IDirect3DDevice9 *d3ddevice` (rwd3d.h:2,36).
// Without it rw.h compiles fine but rw::d3d::d3ddevice does not exist, and the
// backbuffer readback below cannot see the device librw created.
#define WITH_D3D
#include <rw.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace LibRw {
namespace {

const char* const kSmokeLog = "log/librw_smoke.txt";

void LogLine(const char* fmt, ...) {
    std::FILE* f = std::fopen(kSmokeLog, "a");
    if (!f) return;
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(f, fmt, ap);
    va_end(ap);
    std::fputc('\n', f);
    std::fclose(f);
}

// The plugin set librw's own examples register (tools/camera/main.cpp:209+).
// Registered up-front even though the smoke draws nothing: E2' will construct
// geometry/atomics/rasters through these same offsets, and registering them from
// the start means the smoke exercises the layout we will actually ship.
void RegisterPlugins() {
    rw::registerMeshPlugin();
    rw::registerNativeDataPlugin();
    rw::registerAtomicRightsPlugin();
    rw::registerMaterialRightsPlugin();
    rw::xbox::registerVertexFormatPlugin();
    rw::registerMatFXPlugin();
    rw::registerUVAnimPlugin();
    rw::registerSkinPlugin();
    rw::registerHAnimPlugin();
    rw::registerUserDataPlugin();
}

// Pump the window queue so the app stays responsive during the probe and so a
// close request ends it early rather than hanging the smoke.
// Returns false when a quit was requested.
bool PumpMessages() {
    MSG msg;
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return true;
}

}  // namespace

bool SmokeRequested() {
    const char* e = std::getenv("MASHED_RENDER_LIBRW");
    return e && e[0] == '1' && e[1] == '\0';
}

namespace {

// Read the centre pixel of librw's backbuffer. "Presented N frames" on its own is
// a degenerate observation -- a device that silently did nothing would report the
// same. This reads back what librw actually put in the framebuffer and compares it
// to the clear colour, which is the only thing that distinguishes the two.
// Uses rw::d3d::d3ddevice (deps/librw/src/d3d/rwd3d.h) -- the device librw created.
bool ReadBackCentrePixel(int width, int height, DWORD* out) {
    IDirect3DDevice9* dev = rw::d3d::d3ddevice;
    if (!dev) return false;

    IDirect3DSurface9* back = nullptr;
    if (FAILED(dev->GetRenderTarget(0, &back)) || !back) return false;

    D3DSURFACE_DESC desc;
    if (FAILED(back->GetDesc(&desc))) { back->Release(); return false; }

    IDirect3DSurface9* sys = nullptr;
    HRESULT hr = dev->CreateOffscreenPlainSurface(
        desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &sys, nullptr);
    if (FAILED(hr) || !sys) { back->Release(); return false; }

    bool ok = false;
    if (SUCCEEDED(dev->GetRenderTargetData(back, sys))) {
        D3DLOCKED_RECT lr;
        if (SUCCEEDED(sys->LockRect(&lr, nullptr, D3DLOCK_READONLY))) {
            const int cx = width / 2, cy = height / 2;
            const BYTE* row = (const BYTE*)lr.pBits + (size_t)cy * lr.Pitch;
            *out = *(const DWORD*)(row + (size_t)cx * 4) & 0x00FFFFFFu;
            sys->UnlockRect();
            ok = true;
        }
    }
    sys->Release();
    back->Release();
    return ok;
}

}  // namespace

int RunSmoke(HWND hwnd, int width, int height, int frames) {
    // Truncate the log so each run stands alone.
    if (std::FILE* f = std::fopen(kSmokeLog, "w")) std::fclose(f);
    LogLine("librw smoke -- E1' acceptance probe");
    LogLine("hwnd=%p size=%dx%d frames=%d", (void*)hwnd, width, height, frames);

    if (!rw::Engine::init()) {
        LogLine("FAIL: rw::Engine::init()");
        return 1;
    }
    LogLine("ok: Engine::init");

    RegisterPlugins();
    LogLine("ok: plugins registered");

    rw::EngineOpenParams params;
    std::memset(&params, 0, sizeof(params));
    params.window = hwnd;
    if (!rw::Engine::open(&params)) {
        LogLine("FAIL: rw::Engine::open() -- librw could not bind the window");
        return 2;
    }
    LogLine("ok: Engine::open");

    // Pick the last subsystem, as the skeleton does (skeleton.cpp:23-27). On D3D9
    // the subsystems are the enumerated adapters; the last one is the skeleton's
    // convention and matches what re3 ships.
    const rw::int32 numSub = rw::Engine::getNumSubSystems();
    rw::SubSystemInfo ssinfo;
    for (rw::int32 i = 0; i < numSub; i++)
        if (rw::Engine::getSubSystemInfo(&ssinfo, i))
            LogLine("  subsystem[%d] = %s", (int)i, ssinfo.name);
    if (numSub > 0) rw::Engine::setSubSystem(numSub - 1);
    LogLine("ok: setSubSystem(%d of %d)", (int)(numSub - 1), (int)numSub);

    if (!rw::Engine::start()) {
        LogLine("FAIL: rw::Engine::start() -- CreateDevice path");
        return 3;
    }
    LogLine("ok: Engine::start -- D3D9 device created by librw");

    rw::Charset::open();

    rw::Camera* cam = rw::Camera::create();
    if (!cam) {
        LogLine("FAIL: Camera::create()");
        return 4;
    }
    cam->setFrame(rw::Frame::create());
    cam->frameBuffer = rw::Raster::create(width, height, 0, rw::Raster::CAMERA);
    cam->zBuffer     = rw::Raster::create(width, height, 0, rw::Raster::ZBUFFER);
    if (!cam->frameBuffer || !cam->zBuffer) {
        LogLine("FAIL: camera raster create (frameBuffer=%p zBuffer=%p)",
                (void*)cam->frameBuffer, (void*)cam->zBuffer);
        return 5;
    }
    // Standard RW default view window, aspect-corrected (skeleton.cpp:111-122).
    {
        rw::V2d vw;
        if (width > height) {
            vw.x = 1.0f;
            vw.y = 1.0f / ((float)width / (float)height);
        } else {
            vw.x = 1.0f / ((float)height / (float)width);
            vw.y = 1.0f;
        }
        cam->setViewWindow(&vw);
    }
    cam->setNearPlane(0.1f);
    cam->setFarPlane(1000.0f);
    LogLine("ok: camera %dx%d", width, height);

    // A deliberately non-black, non-grey clear colour. Black would also be what a
    // dead device leaves behind, so it would not distinguish "librw cleared the
    // frame" from "nothing happened" -- the observation has to be non-degenerate.
    rw::RGBA clearcol = { 0x20, 0x80, 0xC0, 0xFF };

    LARGE_INTEGER freq, t0, t1;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t0);

    int presented = 0;
    bool readback_ok = false;
    DWORD centre = 0;
    for (int i = 0; i < frames; i++) {
        if (!PumpMessages()) {
            LogLine("note: quit requested at frame %d", i);
            break;
        }
        cam->clear(&clearcol, rw::Camera::CLEARIMAGE | rw::Camera::CLEARZ);
        cam->beginUpdate();
        cam->endUpdate();
        // Sample mid-run, after the clear, before showRaster flips it away.
        if (i == frames / 2)
            readback_ok = ReadBackCentrePixel(width, height, &centre);
        cam->showRaster(0);
        presented++;
    }

    QueryPerformanceCounter(&t1);
    const double secs =
        (double)(t1.QuadPart - t0.QuadPart) / (double)freq.QuadPart;
    LogLine("ok: presented %d frames in %.3f s = %.1f fps",
            presented, secs, secs > 0.0 ? presented / secs : 0.0);
    // This rate is EXPECTED to be uncapped, and that is not a defect. The
    // standalone deliberately binds System32's d3d9, NOT the dev d3d9 shim that
    // hosts MASHED_FPS_CAP (see build.bat's RUNTIME NOTE), and showRaster(0)
    // requests no vsync. Frame pacing for the real standalone comes from its own
    // QPC 60 Hz accumulator, which this probe bypasses.

    // The non-degenerate check: prove librw actually wrote the framebuffer.
    const DWORD expected = ((DWORD)clearcol.red   << 16) |
                           ((DWORD)clearcol.green <<  8) |
                            (DWORD)clearcol.blue;
    if (!readback_ok) {
        LogLine("FAIL: backbuffer readback did not run - cannot prove librw drew");
        return 6;
    }
    if (centre != expected) {
        LogLine("FAIL: backbuffer centre = 0x%06lX, expected clear colour 0x%06lX",
                centre, expected);
        return 7;
    }
    LogLine("ok: backbuffer centre = 0x%06lX == clear colour (librw really drew)",
            centre);

    // E2'a task 2: the raster bridge needs a live engine (Raster::create goes
    // through the D3D9 driver), so it runs here, inside the started engine.
    {
        const int n = RasterBridge_SelfTest();
        if (n < 0) {
            LogLine("FAIL: raster bridge self-test (see log/librw_raster.txt)");
            return 8;
        }
        LogLine("ok: raster bridge hashed %d textures -> log/librw_raster.txt", n);
    }

    // E2'b: build a real track's world geometry through librw and check it
    // against the parser's own totals.
    {
        const int rc = SceneBuild_SelfTest();
        if (rc != 0) {
            LogLine("FAIL: scene build self-test rc=%d (log/librw_scene.txt)", rc);
            return 9;
        }
        LogLine("ok: scene build self-test PASS -> log/librw_scene.txt");
    }

    // Teardown in reverse (skeleton.cpp:56-65).
    cam->frameBuffer->destroy();
    cam->zBuffer->destroy();
    if (rw::Frame* f = cam->getFrame()) {
        cam->setFrame(nil);
        f->destroy();
    }
    cam->destroy();
    rw::Charset::close();
    rw::Engine::stop();
    rw::Engine::close();
    rw::Engine::term();
    LogLine("ok: teardown clean");
    LogLine("RESULT: PASS");
    return 0;
}

}  // namespace LibRw
}  // namespace mashed_re
