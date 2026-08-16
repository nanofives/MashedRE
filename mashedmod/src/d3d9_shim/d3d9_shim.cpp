// Mashed RE — d3d9.dll proxy that forces IDirect3D9::CreateDevice into
// windowed mode at 800×600.
//
// Deployment: this DLL is named d3d9.dll and placed in original/. When
// MASHED loads its d3d9 import, Windows resolves to our proxy first
// (application directory precedes System32 in DLL search order).
//
// Real d3d9.dll is pre-copied to original/d3d9_real.dll by the build
// script. We LoadLibrary that filename rather than the system d3d9.dll
// because Windows loader dedups by basename — LoadLibraryW on the system
// d3d9 while our proxy is loaded as "d3d9.dll" would return our own
// HMODULE → infinite recursion through Direct3DCreate9_Hook.
//
// AppCompat coupling: the shim layer set by scripts/setup_mashed_compat.ps1
// must NOT include DISABLEDXMAXIMIZEDWINDOWEDMODE while this proxy is
// installed. That shim hooks d3d9 exports and conflicts with our
// trampolines, hanging MASHED at process init (4 threads, 3 suspended,
// no main window). The forced-windowed CreateDevice in this proxy
// makes that shim redundant anyway.
//
// Internal symbol names differ from the export table names because
// d3d9.h declares the real exports with specific signatures we cannot
// shadow. d3d9_shim.def remaps: Direct3DCreate9 = Direct3DCreate9_Hook,
// D3DPERF_BeginEvent = Forward_D3DPERF_BeginEvent, etc.
#include <windows.h>
#include <d3d9.h>
#include <cstdio>
#include <cstring>

namespace {

HMODULE   g_RealD3D9 = nullptr;
HINSTANCE g_hThis    = nullptr;

using Direct3DCreate9Fn = IDirect3D9* (WINAPI*)(UINT);
Direct3DCreate9Fn g_RealDirect3DCreate9 = nullptr;

FARPROC g_p_D3DPERF_BeginEvent                       = nullptr;
FARPROC g_p_D3DPERF_EndEvent                         = nullptr;
FARPROC g_p_D3DPERF_GetStatus                        = nullptr;
FARPROC g_p_D3DPERF_QueryRepeatFrame                 = nullptr;
FARPROC g_p_D3DPERF_SetMarker                        = nullptr;
FARPROC g_p_D3DPERF_SetOptions                       = nullptr;
FARPROC g_p_D3DPERF_SetRegion                        = nullptr;
FARPROC g_p_DebugSetLevel                            = nullptr;
FARPROC g_p_DebugSetMute                             = nullptr;
FARPROC g_p_Direct3D9EnableMaximizedWindowedModeShim = nullptr;
FARPROC g_p_Direct3DCreate9Ex                        = nullptr;
FARPROC g_p_Direct3DCreate9On12                      = nullptr;
FARPROC g_p_Direct3DCreate9On12Ex                    = nullptr;
FARPROC g_p_Direct3DShaderValidatorCreate9           = nullptr;
FARPROC g_p_PSGPError                                = nullptr;
FARPROC g_p_PSGPSampleTexture                        = nullptr;

using CreateDeviceFn = HRESULT (STDMETHODCALLTYPE*)(
    IDirect3D9*, UINT, D3DDEVTYPE, HWND, DWORD,
    D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
CreateDeviceFn g_OriginalCreateDevice = nullptr;
LONG           g_VtablePatched        = 0;

// Forced windowed backbuffer. Default 640x480 (matches the camera-res patch);
// env MASHED_HIRES=1 -> 1280x960 (2x) for high-res parity capture;
// env MASHED_RES=WxH (e.g. 2560x1440) overrides both — QoL play resolution.
// COUPLING: must equal the camera-res screen-dim getters (on-disk
// patch_mashed_fix_camera_res.py, or mashed_qol.asi's runtime MASHED_RES
// re-target of 0x00498bc0/0x00498bd0) — a mismatched camera raster AVs at boot.
static void ForcedBackBufInit(UINT* pw, UINT* ph) {
    char v[32] = {};
    if (GetEnvironmentVariableA("MASHED_RES", v, sizeof(v)) > 0) {
        UINT w = 0, h = 0; const char* p = v;
        for (; *p >= '0' && *p <= '9'; ++p) w = w * 10 + (UINT)(*p - '0');
        if (*p == 'x' || *p == 'X') {
            for (++p; *p >= '0' && *p <= '9'; ++p) h = h * 10 + (UINT)(*p - '0');
        }
        if (w >= 320 && h >= 240 && w <= 7680 && h <= 4320) { *pw = w; *ph = h; return; }
    }
    const bool hires =
        (GetEnvironmentVariableA("MASHED_HIRES", v, sizeof(v)) > 0 && v[0] == '1');
    *pw = hires ? 1280u : 640u;
    *ph = hires ? 960u : 480u;
}
static UINT ForcedBackBufW() {
    static UINT w = 0, h = 0;
    if (!w) ForcedBackBufInit(&w, &h);
    return w;
}
static UINT ForcedBackBufH() {
    static UINT w = 0, h = 0;
    if (!h) ForcedBackBufInit(&w, &h);
    return h;
}
#define kForceBackBufferWidth  ForcedBackBufW()
#define kForceBackBufferHeight ForcedBackBufH()

// Reshape the device window with a normal title bar / borders, for comfort when
// arranging several concurrent MASHED instances on screen (parallel C2->C3 diff
// pool). The D3D9 backbuffer is fixed at kForceBackBufferWidth×Height; we resize
// the OUTER window via AdjustWindowRect so the CLIENT area stays exactly that
// size — the backbuffer presents 1:1, so the render path is unaffected. The
// window class and title text are untouched (FindWindowA-based tooling still
// works). On by default; opt out by setting env MASHED_RE_BORDERLESS=1 (restores
// the original borderless window for any flow that screenshots the whole frame).
void ApplyWindowBorders(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    char buf[8] = { 0 };
    // QoL borderless play mode: undecorated popup at (0,0) sized exactly to the
    // backbuffer (use with MASHED_RES at the monitor's native size for
    // borderless-fullscreen). Takes precedence over the titled-border reshape.
    if (GetEnvironmentVariableA("MASHED_QOL_BORDERLESS", buf, sizeof(buf)) > 0 &&
        buf[0] == '1') {
        LONG_PTR cur = GetWindowLongPtr(hWnd, GWL_STYLE);
        if (cur & WS_CHILD) return;
        SetWindowLongPtr(hWnd, GWL_STYLE,
                         WS_POPUP | (cur & WS_VISIBLE));
        SetWindowPos(hWnd, HWND_TOP, 0, 0,
                     (int)kForceBackBufferWidth, (int)kForceBackBufferHeight,
                     SWP_FRAMECHANGED | SWP_NOACTIVATE);
        return;
    }
    if (GetEnvironmentVariableA("MASHED_RE_BORDERLESS", buf, sizeof(buf)) > 0 &&
        buf[0] == '1') {
        return;  // explicit opt-out: leave the original (borderless) style
    }
    LONG_PTR cur = GetWindowLongPtr(hWnd, GWL_STYLE);
    if (cur & WS_CHILD) return;  // never reshape a child window

    // Fixed-size titled window: caption (drag), sysmenu/close, minimize. No
    // WS_THICKFRAME / WS_MAXIMIZEBOX — a fixed backbuffer can't follow a resize.
    const LONG_PTR kBorderStyle =
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    SetWindowLongPtr(hWnd, GWL_STYLE, kBorderStyle | (cur & WS_VISIBLE));

    // Grow the outer rect so the client area remains the backbuffer size.
    RECT rc = { 0, 0, (LONG)kForceBackBufferWidth, (LONG)kForceBackBufferHeight };
    AdjustWindowRect(&rc, (DWORD)kBorderStyle, FALSE);
    // Pin onto screen 1 (the primary monitor — its top-left is (0,0) in Windows
    // virtual-screen coords by definition, always). A small positive offset keeps
    // the whole window on the primary, away from secondary monitors that may sleep
    // and wedge the windowed D3D9 output. Opt out with MASHED_RE_NO_SCREEN1_PIN=1.
    char pinBuf[8] = { 0 };
    bool pinScreen1 =
        !(GetEnvironmentVariableA("MASHED_RE_NO_SCREEN1_PIN", pinBuf, sizeof(pinBuf)) > 0 &&
          pinBuf[0] == '1');
    UINT moveFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED |
                     (pinScreen1 ? 0u : SWP_NOMOVE);
    int posX = 64, posY = 64;

    // MASHED_WIN_POS overrides the screen-1 pin: park the ORIGINAL's window on a
    // chosen monitor/corner so a test run does not land on top of what the user is
    // doing. Same syntax as the standalone's (exe_main.cpp PlaceDevWindow):
    //   left-bl | right-bl | primary-bl | s<N>-bl   (also -br / -tl / -tr)
    //   <x>,<y>
    // Directional selectors are preferred because monitor NUMBERS disagree between
    // Windows Display Settings, EnumDisplayMonitors and Screen.AllScreens.
    //
    // HAZARD, deliberately preserved from the pin comment above: a secondary monitor
    // that goes to SLEEP can wedge the windowed D3D9 output. That is exactly what the
    // screen-1 pin exists to avoid, so this override is opt-in and the risk moves to
    // the caller. MASHED_RE_NO_SCREEN1_PIN is unrelated and still just disables moving.
    char wp[64] = { 0 };
    if (GetEnvironmentVariableA("MASHED_WIN_POS", wp, sizeof(wp)) > 0 && wp[0]) {
        const int w = rc.right - rc.left, h = rc.bottom - rc.top;
        const char* dash = strrchr(wp, '-');
        RECT work = {};
        bool got = false;
        if (dash && dash > wp) {
            const size_t nlen = (size_t)(dash - wp);
            int mode = -1, want = 0;
            if      (nlen == 4 && _strnicmp(wp, "left",    4) == 0) mode = 1;
            else if (nlen == 5 && _strnicmp(wp, "right",   5) == 0) mode = 2;
            else if (nlen == 7 && _strnicmp(wp, "primary", 7) == 0) mode = 3;
            else if (wp[0] == 's' && sscanf(wp + 1, "%d", &want) == 1 && want >= 1) mode = 0;
            if (mode >= 0) {
                struct P { int mode, want, idx; RECT work; bool got; } p{mode, want, 0, {}, false};
                EnumDisplayMonitors(nullptr, nullptr,
                    [](HMONITOR hm, HDC, LPRECT, LPARAM lp) -> BOOL {
                        P* e = reinterpret_cast<P*>(lp);
                        ++e->idx;
                        MONITORINFO mi{}; mi.cbSize = sizeof(mi);
                        if (!GetMonitorInfoA(hm, &mi)) return TRUE;
                        switch (e->mode) {
                            case 0: if (e->idx == e->want) { e->work = mi.rcWork; e->got = true; return FALSE; } break;
                            case 1: if (!e->got || mi.rcWork.left < e->work.left) { e->work = mi.rcWork; e->got = true; } break;
                            case 2: if (!e->got || mi.rcWork.left > e->work.left) { e->work = mi.rcWork; e->got = true; } break;
                            case 3: if (mi.dwFlags & MONITORINFOF_PRIMARY) { e->work = mi.rcWork; e->got = true; return FALSE; } break;
                        }
                        return TRUE;
                    }, reinterpret_cast<LPARAM>(&p));
                if (p.got) {
                    const char* c = dash + 1;
                    const bool bottom = (c[0] == 'b' || c[0] == 'B');
                    const bool right  = (c[0] && (c[1] == 'r' || c[1] == 'R'));
                    posX = right  ? (p.work.right  - w) : p.work.left;
                    posY = bottom ? (p.work.bottom - h) : p.work.top;
                    got = true;
                }
            }
        }
        if (!got && sscanf(wp, "%d,%d", &posX, &posY) == 2) got = true;
        if (got) moveFlags &= ~SWP_NOMOVE;   // an explicit position beats the pin
    }

    SetWindowPos(hWnd, nullptr, posX, posY,
                 rc.right - rc.left, rc.bottom - rc.top, moveFlags);
}

// ── Original-side backbuffer dump (font/pixel parity instrument) ──────────
// Env MASHED_ORIG_BBDUMP="N[,N...]" — at those Present-call counts, copy the
// backbuffer to ..\verify\orig_backbuffer_f<N>.bmp (24bpp). MASHED's CWD is
// original\, hence the ..\ prefix. Inert when the env var is unset (the
// Present vtable slot is only patched when armed). Counterpart of the
// standalone's MASHED_DBG_BBDUMP truth channel — window screenshots are
// untrustworthy on this machine (multi-monitor Present issue).
using PresentFn = HRESULT (STDMETHODCALLTYPE*)(
    IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
PresentFn g_OriginalPresent      = nullptr;
LONG      g_PresentPatched       = 0;
LONG      g_PresentCount         = 0;
int       g_DumpFrames[16]       = {};
int       g_DumpFrameCount       = -1;   // -1 = env unparsed

void ParseDumpFramesOnce() {
    if (g_DumpFrameCount != -1) return;
    g_DumpFrameCount = 0;
    char buf[256] = {};
    if (GetEnvironmentVariableA("MASHED_ORIG_BBDUMP", buf, sizeof(buf)) == 0)
        return;
    const char* p = buf;
    while (*p && g_DumpFrameCount < 16) {
        int v = atoi(p);
        if (v > 0) g_DumpFrames[g_DumpFrameCount++] = v;
        while (*p && *p != ',') ++p;
        if (*p == ',') ++p;
    }
}

void DumpBackbufferBMPPath(IDirect3DDevice9* dev, const char* path) {
    IDirect3DSurface9* bb = nullptr;
    if (FAILED(dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb)) || !bb)
        return;
    D3DSURFACE_DESC desc = {};
    bb->GetDesc(&desc);
    IDirect3DSurface9* off = nullptr;
    if (FAILED(dev->CreateOffscreenPlainSurface(
            desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM,
            &off, nullptr)) || !off) {
        bb->Release();
        return;
    }
    if (SUCCEEDED(dev->GetRenderTargetData(bb, off))) {
        D3DLOCKED_RECT lr = {};
        if (SUCCEEDED(off->LockRect(&lr, nullptr, D3DLOCK_READONLY))) {
            std::FILE* f = std::fopen(path, "wb");
            if (f) {
                const int W = (int)desc.Width, H = (int)desc.Height;
                const int rowbytes = W * 3;
                const int datasz   = rowbytes * H;
                unsigned char hdr[54] = {};
                hdr[0] = 'B'; hdr[1] = 'M';
                *(int*)(hdr + 2)  = 54 + datasz;
                *(int*)(hdr + 10) = 54;
                *(int*)(hdr + 14) = 40;
                *(int*)(hdr + 18) = W;
                *(int*)(hdr + 22) = H;          // bottom-up
                *(short*)(hdr + 26) = 1;
                *(short*)(hdr + 28) = 24;
                std::fwrite(hdr, 1, 54, f);
                // Format-aware row conversion, bottom-up. MASHED's 640x480
                // mode is 16-bit (R5G6B5); handle 32-bit too.
                for (int y = H - 1; y >= 0; --y) {
                    const unsigned char* src =
                        (const unsigned char*)lr.pBits + (size_t)y * lr.Pitch;
                    for (int x = 0; x < W; ++x) {
                        unsigned char bgr[3];
                        if (desc.Format == D3DFMT_R5G6B5) {
                            const unsigned short v =
                                *(const unsigned short*)(src + (size_t)x * 2);
                            bgr[0] = (unsigned char)((v & 0x1f) << 3);
                            bgr[1] = (unsigned char)(((v >> 5) & 0x3f) << 2);
                            bgr[2] = (unsigned char)(((v >> 11) & 0x1f) << 3);
                        } else if (desc.Format == D3DFMT_X1R5G5B5 ||
                                   desc.Format == D3DFMT_A1R5G5B5) {
                            const unsigned short v =
                                *(const unsigned short*)(src + (size_t)x * 2);
                            bgr[0] = (unsigned char)((v & 0x1f) << 3);
                            bgr[1] = (unsigned char)(((v >> 5) & 0x1f) << 3);
                            bgr[2] = (unsigned char)(((v >> 10) & 0x1f) << 3);
                        } else {  // X8R8G8B8 / A8R8G8B8
                            std::memcpy(bgr, src + (size_t)x * 4, 3);
                        }
                        std::fwrite(bgr, 1, 3, f);
                    }
                }
                std::fclose(f);
            }
            off->UnlockRect();
        }
    }
    off->Release();
    bb->Release();
}

void DumpBackbufferBMP(IDirect3DDevice9* dev, int frame) {
    char path[MAX_PATH];
    std::snprintf(path, sizeof(path), "..\\verify\\orig_backbuffer_f%d.bmp", frame);
    DumpBackbufferBMPPath(dev, path);
}

// On-demand dump: env MASHED_ORIG_BBDUMP_REQ names a request file. When that
// file exists, its first line is the target .bmp path; we dump the current
// backbuffer there and delete the request. Lets an external nav driver
// (re/frida/capture_orig_screens.py) grab the settled frame of any screen it
// pushes, instead of guessing fixed present-counts. Poll is one
// GetFileAttributes per Present (~60/s) — negligible.
char g_ReqPath[MAX_PATH] = {};
int  g_ReqArmed = -1;   // -1 unparsed, 0 off, 1 on

void ParseReqOnce() {
    if (g_ReqArmed != -1) return;
    g_ReqArmed = (GetEnvironmentVariableA("MASHED_ORIG_BBDUMP_REQ",
                                          g_ReqPath, sizeof(g_ReqPath)) > 0) ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Frame limiter. MASHED is frame-COUPLED (game logic advances per rendered
// frame) with NO in-game limiter, so on modern GPUs it runs uncapped (~360 FPS
// measured => ~6x too fast; a full challenge ran in ~40 s). Pace the Present
// rate (one Present per game frame) to a target FPS so the frame-coupled logic
// runs at its intended speed. Tunable via env MASHED_FPS_CAP (default 60 ~=
// 360/6; 0 disables). RE: time source FUN_004950b0 = QPC*3e6/QPF (accurate,
// uses QPF correctly => no frequency bug); no native frame cap exists.
// NOTE: the Present hook (PatchPresentSlot) is installed unconditionally so this
// runs every frame (it used to be gated behind the BBDump screenshot feature).
// kernel32-only: Sleep for the bulk, spin (SwitchToThread) for the final <2ms.
// Set MASHED_FPS_LOG=1 to write measured Present FPS to log/fps_limiter.txt.
static void FrameLimit()
{
    static int           s_cap  = -1;          // -1 uninit, 0 disabled
    static int           s_capRace = -1;       // -1 feature off; >=0 cap for race phases
    static int           s_log  = 0;
    static LARGE_INTEGER s_freq = {};
    static LARGE_INTEGER s_last = {};
    static LARGE_INTEGER s_logLast = {};
    static int           s_frames = 0;
    if (s_cap == -1) {
        char buf[16] = {};
        DWORD got = GetEnvironmentVariableA("MASHED_FPS_CAP", buf, sizeof(buf));
        int v = 60;                            // default 60 FPS
        if (got > 0 && got < sizeof(buf)) { v = 0; for (char* c = buf; *c >= '0' && *c <= '9'; ++c) v = v * 10 + (*c - '0'); }
        s_cap = (v < 0) ? 0 : v;
        // MASHED_FPS_CAP_RACE: alternate cap applied ONLY while the game-state
        // enum DAT_00771968 is 3 or 6 (the tick-loop race phases — see
        // FUN_00492d30 cases 3/6). Menus stay on MASHED_FPS_CAP (they are
        // per-frame-coupled); races may run faster once MASHED_DECOUPLE
        // (mashed_qol.asi) makes game speed framerate-independent. 0 = uncapped
        // in race. Unset = feature off (single cap, legacy behavior).
        got = GetEnvironmentVariableA("MASHED_FPS_CAP_RACE", buf, sizeof(buf));
        if (got > 0 && got < sizeof(buf)) { v = 0; for (char* c = buf; *c >= '0' && *c <= '9'; ++c) v = v * 10 + (*c - '0'); s_capRace = (v < 0) ? 0 : v; }
        s_log = (GetEnvironmentVariableA("MASHED_FPS_LOG", buf, sizeof(buf)) > 0);
        QueryPerformanceFrequency(&s_freq);
        QueryPerformanceCounter(&s_last);
        s_logLast = s_last;
    }
    int cap = s_cap;
    if (s_capRace >= 0) {
        const DWORD phase = *reinterpret_cast<volatile DWORD*>(0x00771968); // DAT_00771968
        if (phase == 3 || phase == 6) cap = s_capRace;
    }
    LARGE_INTEGER now;
    if (cap > 0 && s_freq.QuadPart != 0) {
        const LONGLONG target = s_freq.QuadPart / cap;     // ticks per frame
        for (;;) {
            QueryPerformanceCounter(&now);
            LONGLONG elapsed = now.QuadPart - s_last.QuadPart;
            if (elapsed >= target) break;
            LONGLONG remainMs = (target - elapsed) * 1000 / s_freq.QuadPart;
            if (remainMs > 2) Sleep(1); else SwitchToThread();
        }
        s_last = now;
    } else {
        QueryPerformanceCounter(&now);
    }
    if (s_log && s_freq.QuadPart != 0) {       // optional: measured Present FPS once/sec
        s_frames++;
        LONGLONG since = now.QuadPart - s_logLast.QuadPart;
        if (since >= s_freq.QuadPart) {
            double fps = (double)s_frames * (double)s_freq.QuadPart / (double)since;
            std::FILE* lf = std::fopen("C:\\Users\\maria\\Desktop\\Proyectos\\Mashed\\log\\fps_limiter.txt", "a");
            if (lf) { std::fprintf(lf, "present_fps=%.1f (cap=%d)\n", fps, cap); std::fclose(lf); }
            s_frames = 0; s_logLast = now;
        }
    }
}

// ── FPS / frame-time OSD ────────────────────────────────────────────────────
// Dependency-free on-screen indicators drawn with Clear-rect 7-segment digits
// (no D3DX/font — works on the 16-bit backbuffer, drawn in the Present hook).
// Top line: FPS (green). Second line: frame time in ms, 1 decimal (yellow).
// Values are a rolling average over ~1/4 s. Env MASHED_FPS_OSD=1 starts
// visible; F11 toggles at any time. Drawn AFTER the BBDump copy so parity
// captures never include the overlay.
static void OsdRect(IDirect3DDevice9* dev, int x1, int y1, int x2, int y2,
                    D3DCOLOR c) {
    D3DRECT r = { x1, y1, x2, y2 };
    dev->Clear(1, &r, D3DCLEAR_TARGET, c, 1.0f, 0);
}

// Segment bits: A=1 B=2 C=4 D=8 E=16 F=32 G=64 (A top, clockwise, G middle).
static const unsigned char kSegDigits[10] = {
    63, 6, 91, 79, 102, 109, 125, 7, 127, 111
};

// Draw one glyph ('0'-'9' or '.') at (x,y), scale s. Returns x advance.
static int OsdGlyph(IDirect3DDevice9* dev, char ch, int x, int y, int s,
                    D3DCOLOR c) {
    const int t = 2 * s, w = 7 * s, h = 12 * s;
    if (ch == '.') {
        OsdRect(dev, x, y + h - t, x + t, y + h, c);
        return t + 2 * s;
    }
    if (ch < '0' || ch > '9') return w + 3 * s;   // unknown: blank advance
    const unsigned char m = kSegDigits[ch - '0'];
    const int ym = y + h / 2;
    if (m & 1)  OsdRect(dev, x, y, x + w, y + t, c);                  // A
    if (m & 2)  OsdRect(dev, x + w - t, y, x + w, ym, c);             // B
    if (m & 4)  OsdRect(dev, x + w - t, ym, x + w, y + h, c);         // C
    if (m & 8)  OsdRect(dev, x, y + h - t, x + w, y + h, c);          // D
    if (m & 16) OsdRect(dev, x, ym, x + t, y + h, c);                 // E
    if (m & 32) OsdRect(dev, x, y, x + t, ym, c);                     // F
    if (m & 64) OsdRect(dev, x, ym - t / 2, x + w, ym + t / 2, c);    // G
    return w + 3 * s;
}

static int OsdText(IDirect3DDevice9* dev, const char* str, int x, int y, int s,
                   D3DCOLOR c) {
    for (const char* p = str; *p; ++p) x += OsdGlyph(dev, *p, x, y, s, c);
    return x;
}

static void OsdDraw(IDirect3DDevice9* dev) {
    static int           s_visible = -1;   // -1 = env unparsed
    static SHORT         s_prevKey = 0;
    static LARGE_INTEGER s_freq = {}, s_winStart = {};
    static int           s_winFrames = 0;
    static double        s_fps = 0.0, s_ms = 0.0;

    if (s_visible == -1) {
        char buf[8] = {};
        s_visible = (GetEnvironmentVariableA("MASHED_FPS_OSD", buf, sizeof(buf)) > 0 &&
                     buf[0] == '1') ? 1 : 0;
        QueryPerformanceFrequency(&s_freq);
        QueryPerformanceCounter(&s_winStart);
    }
    // F11 toggle (edge-triggered).
    const SHORT key = GetAsyncKeyState(VK_F11);
    if ((key & 0x8000) && !(s_prevKey & 0x8000)) s_visible = !s_visible;
    s_prevKey = key;

    // Rolling ~1/4-second measurement window (always runs, so the readout is
    // fresh the moment the overlay is toggled on).
    s_winFrames++;
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    const LONGLONG span = now.QuadPart - s_winStart.QuadPart;
    if (s_freq.QuadPart > 0 && span >= s_freq.QuadPart / 4) {
        s_fps = (double)s_winFrames * (double)s_freq.QuadPart / (double)span;
        s_ms  = (double)span * 1000.0 /
                ((double)s_freq.QuadPart * (double)s_winFrames);
        s_winFrames = 0;
        s_winStart  = now;
    }
    if (!s_visible || s_fps <= 0.0) return;

    const int s = (int)(kForceBackBufferHeight / 480u);
    const int scale = s < 1 ? 1 : s;
    const int margin = 6 * scale, lineH = 16 * scale;

    char fpsStr[16], msStr[16];
    std::snprintf(fpsStr, sizeof(fpsStr), "%d", (int)(s_fps + 0.5));
    std::snprintf(msStr,  sizeof(msStr),  "%.1f", s_ms);

    // Backdrop sized to the wider line, for readability over the scene.
    const int advD = 10 * scale, advDot = 4 * scale;
    int wFps = 0; for (const char* p = fpsStr; *p; ++p) wFps += advD;
    int wMs  = 0; for (const char* p = msStr;  *p; ++p) wMs += (*p == '.') ? advDot : advD;
    const int wMax = (wFps > wMs ? wFps : wMs);
    OsdRect(dev, margin - 2 * scale, margin - 2 * scale,
            margin + wMax + 2 * scale, margin + 2 * lineH + 2 * scale,
            D3DCOLOR_XRGB(12, 12, 12));

    OsdText(dev, fpsStr, margin, margin,         scale, D3DCOLOR_XRGB(64, 255, 64));
    OsdText(dev, msStr,  margin, margin + lineH, scale, D3DCOLOR_XRGB(255, 208, 64));
}

// P3 3D draw-call accounting (defined below, with the counting thunks).
void DumpDraw3DStats(const char* bmpPath);
void ResetDraw3DCounters();

HRESULT STDMETHODCALLTYPE Present_BBDump(
    IDirect3DDevice9* pThis, const RECT* src, const RECT* dst,
    HWND wnd, const RGNDATA* dirty)
{
    FrameLimit();
    const LONG n = InterlockedIncrement(&g_PresentCount);
    for (int i = 0; i < g_DumpFrameCount; ++i) {
        if (g_DumpFrames[i] == (int)n) {
            DumpBackbufferBMP(pThis, (int)n);
            break;
        }
    }
    if (g_ReqArmed == 1 &&
        GetFileAttributesA(g_ReqPath) != INVALID_FILE_ATTRIBUTES) {
        char target[MAX_PATH] = {};
        std::FILE* rf = std::fopen(g_ReqPath, "rb");
        if (rf) { if (!std::fgets(target, sizeof(target), rf)) target[0] = 0;
                  std::fclose(rf); }
        for (char* c = target; *c; ++c)
            if (*c == '\r' || *c == '\n') { *c = 0; break; }
        DeleteFileA(g_ReqPath);
        if (target[0]) {
            DumpBackbufferBMPPath(pThis, target);
            DumpDraw3DStats(target);   // P3: this frame's 3D draw totals
        }
    }
    OsdDraw(pThis);   // after dumps: captures never include the overlay
    // Reset the per-frame 3D counters every Present so each captured frame's
    // draw totals are isolated (draws for frame N accumulate before Present(N)).
    // Placed after OsdDraw for correctness-by-construction; the OSD itself uses
    // Clear (see OsdRect), not DrawPrimitive, so it never reaches these counters.
    ResetDraw3DCounters();
    return g_OriginalPresent(pThis, src, dst, wnd, dirty);
}

// Parity-capture background override. env MASHED_PARITY_BG = an ARGB hex (e.g.
// ffffffff) — when set, the Clear hook below forces every target-clear to that
// colour so the original draws its chrome on a flat field (white reads the dark
// elements that vanish on black). 0 = off.
using ClearFn = HRESULT (STDMETHODCALLTYPE*)(IDirect3DDevice9*, DWORD,
    const D3DRECT*, DWORD, D3DCOLOR, float, DWORD);
ClearFn g_OriginalClear = nullptr;

D3DCOLOR ParityBgColor() {
    static int init = 0; static D3DCOLOR c = 0;
    if (!init) {
        init = 1;
        char v[16] = {};
        if (GetEnvironmentVariableA("MASHED_PARITY_BG", v, sizeof(v)) > 0) {
            unsigned long val = 0;
            for (const char* p = v; *p; ++p) {
                int d;
                if (*p >= '0' && *p <= '9') d = *p - '0';
                else if (*p >= 'a' && *p <= 'f') d = *p - 'a' + 10;
                else if (*p >= 'A' && *p <= 'F') d = *p - 'A' + 10;
                else break;
                val = (val << 4) | (unsigned long)d;
            }
            c = (D3DCOLOR)val;
        }
    }
    return c;
}

HRESULT STDMETHODCALLTYPE Clear_ForceColor(IDirect3DDevice9* pThis, DWORD count,
    const D3DRECT* rects, DWORD flags, D3DCOLOR color, float z, DWORD stencil)
{
    // Only override the colour of full-target clears (leave z-only clears alone).
    if ((flags & D3DCLEAR_TARGET) != 0) color = ParityBgColor();
    return g_OriginalClear(pThis, count, rects, flags, color, z, stencil);
}

// ── P3 parity: per-frame 3D draw-call accounting (original side) ──────────
// The standalone reports loaded 3D geometry per category via
// MASHED_DBG_DRAWSTREAM3D; this counts what the ORIGINAL actually submits to
// D3D9 each frame, so the two are comparable on the camera-INVARIANT metric:
// total primitives (triangles) + draw calls per frame. If the original submits
// far more than the RE, the RE is missing geometry; similar totals mean the
// divergence is lighting/material, not content. Dumped as a
// "<bmp>.draw3d.json" sibling whenever MASHED_ORIG_BBDUMP_REQ fires.
LONG g_dcCalls = 0, g_dcPrims = 0, g_dcVerts = 0;
LONG g_dp = 0, g_di = 0, g_dpup = 0, g_diup = 0;

using DrawPrimitiveFn = HRESULT (STDMETHODCALLTYPE*)(
    IDirect3DDevice9*, D3DPRIMITIVETYPE, UINT, UINT);
using DrawIndexedPrimitiveFn = HRESULT (STDMETHODCALLTYPE*)(
    IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
using DrawPrimitiveUPFn = HRESULT (STDMETHODCALLTYPE*)(
    IDirect3DDevice9*, D3DPRIMITIVETYPE, UINT, const void*, UINT);
using DrawIndexedPrimitiveUPFn = HRESULT (STDMETHODCALLTYPE*)(
    IDirect3DDevice9*, D3DPRIMITIVETYPE, UINT, UINT, UINT, const void*,
    D3DFORMAT, const void*, UINT);
DrawPrimitiveFn          g_OrigDP   = nullptr;
DrawIndexedPrimitiveFn   g_OrigDI   = nullptr;
DrawPrimitiveUPFn        g_OrigDPUP = nullptr;
DrawIndexedPrimitiveUPFn g_OrigDIUP = nullptr;

HRESULT STDMETHODCALLTYPE DP_Count(IDirect3DDevice9* t, D3DPRIMITIVETYPE pt,
    UINT start, UINT prims) {
    ++g_dcCalls; ++g_dp; g_dcPrims += (LONG)prims;
    return g_OrigDP(t, pt, start, prims);
}
HRESULT STDMETHODCALLTYPE DI_Count(IDirect3DDevice9* t, D3DPRIMITIVETYPE pt,
    INT base, UINT minIdx, UINT numV, UINT startIdx, UINT prims) {
    ++g_dcCalls; ++g_di; g_dcPrims += (LONG)prims; g_dcVerts += (LONG)numV;
    return g_OrigDI(t, pt, base, minIdx, numV, startIdx, prims);
}
HRESULT STDMETHODCALLTYPE DPUP_Count(IDirect3DDevice9* t, D3DPRIMITIVETYPE pt,
    UINT prims, const void* data, UINT stride) {
    ++g_dcCalls; ++g_dpup; g_dcPrims += (LONG)prims;
    return g_OrigDPUP(t, pt, prims, data, stride);
}
HRESULT STDMETHODCALLTYPE DIUP_Count(IDirect3DDevice9* t, D3DPRIMITIVETYPE pt,
    UINT minV, UINT numV, UINT prims, const void* idx, D3DFORMAT fmt,
    const void* vtx, UINT stride) {
    ++g_dcCalls; ++g_diup; g_dcPrims += (LONG)prims; g_dcVerts += (LONG)numV;
    return g_OrigDIUP(t, pt, minV, numV, prims, idx, fmt, vtx, stride);
}

void DumpDraw3DStats(const char* bmpPath) {
    char p[MAX_PATH];
    std::snprintf(p, sizeof(p), "%s.draw3d.json", bmpPath);
    std::FILE* f = std::fopen(p, "w");
    if (!f) return;
    std::fprintf(f, "{\"draw_calls\": %ld, \"prims\": %ld, \"verts\": %ld, "
        "\"dp\": %ld, \"di\": %ld, \"dpup\": %ld, \"diup\": %ld}\n",
        g_dcCalls, g_dcPrims, g_dcVerts, g_dp, g_di, g_dpup, g_diup);
    std::fclose(f);
}

void ResetDraw3DCounters() {
    g_dcCalls = g_dcPrims = g_dcVerts = g_dp = g_di = g_dpup = g_diup = 0;
}

void PatchDrawSlot(void** vtbl, int slot, void* thunk, void** saveOrig) {
    DWORD oldProt = 0, tmp = 0;
    if (!VirtualProtect(&vtbl[slot], sizeof(void*), PAGE_READWRITE, &oldProt))
        return;
    *saveOrig = vtbl[slot];
    vtbl[slot] = thunk;
    VirtualProtect(&vtbl[slot], sizeof(void*), oldProt, &tmp);
}

void PatchPresentSlot(IDirect3DDevice9* dev) {
    ParseDumpFramesOnce();
    ParseReqOnce();
    if (!dev) return;  // ALWAYS patch: frame limiter needs Present hooked unconditionally
                       // (BBDump's dump logic stays gated by g_DumpFrameCount/g_ReqArmed inside Present_BBDump)
    if (InterlockedExchange(&g_PresentPatched, 1) != 0) return;
    void** vtbl = *reinterpret_cast<void***>(dev);
    DWORD oldProt = 0;
    if (!VirtualProtect(&vtbl[17], sizeof(void*), PAGE_READWRITE, &oldProt)) {
        g_PresentPatched = 0;
        return;
    }
    g_OriginalPresent = reinterpret_cast<PresentFn>(vtbl[17]);
    vtbl[17] = reinterpret_cast<void*>(&Present_BBDump);
    DWORD tmp = 0;
    VirtualProtect(&vtbl[17], sizeof(void*), oldProt, &tmp);
    // P3: 3D draw-call counting slots (only in REQ capture mode, so normal runs
    // are byte-identical-in-effect — the slots are only patched when armed).
    // IDirect3DDevice9 vtable: DrawPrimitive=81, DrawIndexedPrimitive=82,
    // DrawPrimitiveUP=83, DrawIndexedPrimitiveUP=84.
    if (g_ReqArmed == 1) {
        PatchDrawSlot(vtbl, 81, (void*)&DP_Count,   (void**)&g_OrigDP);
        PatchDrawSlot(vtbl, 82, (void*)&DI_Count,   (void**)&g_OrigDI);
        PatchDrawSlot(vtbl, 83, (void*)&DPUP_Count, (void**)&g_OrigDPUP);
        PatchDrawSlot(vtbl, 84, (void*)&DIUP_Count, (void**)&g_OrigDIUP);
    }
    // Clear hook (vtable slot 43) — only when a parity background is requested.
    if (ParityBgColor() != 0 &&
        VirtualProtect(&vtbl[43], sizeof(void*), PAGE_READWRITE, &oldProt)) {
        g_OriginalClear = reinterpret_cast<ClearFn>(vtbl[43]);
        vtbl[43] = reinterpret_cast<void*>(&Clear_ForceColor);
        VirtualProtect(&vtbl[43], sizeof(void*), oldProt, &tmp);
    }
}

HRESULT STDMETHODCALLTYPE CreateDevice_ForceWindowed(
    IDirect3D9* pThis,
    UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
    DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPP,
    IDirect3DDevice9** ppDevice)
{
    if (pPP) {
        pPP->Windowed                   = TRUE;
        pPP->BackBufferWidth            = kForceBackBufferWidth;
        pPP->BackBufferHeight           = kForceBackBufferHeight;
        pPP->FullScreen_RefreshRateInHz = 0;
    }
    // The windowed present target: explicit device window if set, else the focus
    // window. Capture before the call (we don't modify these fields).
    HWND hWnd = (pPP && pPP->hDeviceWindow) ? pPP->hDeviceWindow : hFocusWindow;
    HRESULT hr = g_OriginalCreateDevice(pThis, Adapter, DeviceType, hFocusWindow,
                                        BehaviorFlags, pPP, ppDevice);
    if (SUCCEEDED(hr)) {
        ApplyWindowBorders(hWnd);
        if (ppDevice && *ppDevice) PatchPresentSlot(*ppDevice);
    }
    return hr;
}

void PatchCreateDeviceSlot(IDirect3D9* pD3D) {
    if (!pD3D) return;
    if (InterlockedExchange(&g_VtablePatched, 1) != 0) return;

    void** vtbl = *reinterpret_cast<void***>(pD3D);
    DWORD oldProt = 0;
    if (!VirtualProtect(&vtbl[16], sizeof(void*), PAGE_READWRITE, &oldProt)) {
        g_VtablePatched = 0;
        return;
    }
    g_OriginalCreateDevice = reinterpret_cast<CreateDeviceFn>(vtbl[16]);
    vtbl[16] = reinterpret_cast<void*>(&CreateDevice_ForceWindowed);
    DWORD tmp = 0;
    VirtualProtect(&vtbl[16], sizeof(void*), oldProt, &tmp);
}

// Deploy expectation: d3d9_real.dll has been pre-copied next to this DLL
// (mashedmod/build_d3d9_shim.bat or a manual `copy SysWOW64\d3d9.dll`
// step). We deliberately avoid file I/O inside DllMain — CreateFile,
// CopyFile, etc. take their own internal locks and can deadlock with
// loader lock in a 32-bit process under WIN98RTM AppCompat shim.
bool LoadRealD3D9(HINSTANCE hThis) {
    wchar_t our_path[MAX_PATH];
    DWORD got = GetModuleFileNameW(hThis, our_path, MAX_PATH);
    if (got == 0 || got >= MAX_PATH) return false;
    for (DWORD i = got; i > 0; --i) {
        if (our_path[i-1] == L'\\' || our_path[i-1] == L'/') {
            our_path[i] = 0;
            break;
        }
    }
    wchar_t real_path[MAX_PATH];
    if (lstrlenW(our_path) + 14 >= MAX_PATH) return false;
    lstrcpyW(real_path, our_path);
    lstrcatW(real_path, L"d3d9_real.dll");

    g_RealD3D9 = LoadLibraryW(real_path);
    if (!g_RealD3D9) return false;

    g_RealDirect3DCreate9 = reinterpret_cast<Direct3DCreate9Fn>(
        GetProcAddress(g_RealD3D9, "Direct3DCreate9"));

    g_p_D3DPERF_BeginEvent                       = GetProcAddress(g_RealD3D9, "D3DPERF_BeginEvent");
    g_p_D3DPERF_EndEvent                         = GetProcAddress(g_RealD3D9, "D3DPERF_EndEvent");
    g_p_D3DPERF_GetStatus                        = GetProcAddress(g_RealD3D9, "D3DPERF_GetStatus");
    g_p_D3DPERF_QueryRepeatFrame                 = GetProcAddress(g_RealD3D9, "D3DPERF_QueryRepeatFrame");
    g_p_D3DPERF_SetMarker                        = GetProcAddress(g_RealD3D9, "D3DPERF_SetMarker");
    g_p_D3DPERF_SetOptions                       = GetProcAddress(g_RealD3D9, "D3DPERF_SetOptions");
    g_p_D3DPERF_SetRegion                        = GetProcAddress(g_RealD3D9, "D3DPERF_SetRegion");
    g_p_DebugSetLevel                            = GetProcAddress(g_RealD3D9, "DebugSetLevel");
    g_p_DebugSetMute                             = GetProcAddress(g_RealD3D9, "DebugSetMute");
    g_p_Direct3D9EnableMaximizedWindowedModeShim = GetProcAddress(g_RealD3D9, "Direct3D9EnableMaximizedWindowedModeShim");
    g_p_Direct3DCreate9Ex                        = GetProcAddress(g_RealD3D9, "Direct3DCreate9Ex");
    g_p_Direct3DCreate9On12                      = GetProcAddress(g_RealD3D9, "Direct3DCreate9On12");
    g_p_Direct3DCreate9On12Ex                    = GetProcAddress(g_RealD3D9, "Direct3DCreate9On12Ex");
    g_p_Direct3DShaderValidatorCreate9           = GetProcAddress(g_RealD3D9, "Direct3DShaderValidatorCreate9");
    g_p_PSGPError                                = GetProcAddress(g_RealD3D9, "PSGPError");
    g_p_PSGPSampleTexture                        = GetProcAddress(g_RealD3D9, "PSGPSampleTexture");

    return g_RealDirect3DCreate9 != nullptr;
}

} // namespace

extern "C" IDirect3D9* WINAPI Direct3DCreate9_Hook(UINT SDKVersion) {
    if (!g_RealDirect3DCreate9) {
        if (!LoadRealD3D9(g_hThis)) return nullptr;
    }
    IDirect3D9* p = g_RealDirect3DCreate9(SDKVersion);
    PatchCreateDeviceSlot(p);
    return p;
}

// 16 passthrough exports. Naked JMP trampolines preserve the original
// calling convention regardless of what each function expects.
extern "C" {

__declspec(naked) void Forward_D3DPERF_BeginEvent() {
    __asm { jmp dword ptr [g_p_D3DPERF_BeginEvent] }
}
__declspec(naked) void Forward_D3DPERF_EndEvent() {
    __asm { jmp dword ptr [g_p_D3DPERF_EndEvent] }
}
__declspec(naked) void Forward_D3DPERF_GetStatus() {
    __asm { jmp dword ptr [g_p_D3DPERF_GetStatus] }
}
__declspec(naked) void Forward_D3DPERF_QueryRepeatFrame() {
    __asm { jmp dword ptr [g_p_D3DPERF_QueryRepeatFrame] }
}
__declspec(naked) void Forward_D3DPERF_SetMarker() {
    __asm { jmp dword ptr [g_p_D3DPERF_SetMarker] }
}
__declspec(naked) void Forward_D3DPERF_SetOptions() {
    __asm { jmp dword ptr [g_p_D3DPERF_SetOptions] }
}
__declspec(naked) void Forward_D3DPERF_SetRegion() {
    __asm { jmp dword ptr [g_p_D3DPERF_SetRegion] }
}
__declspec(naked) void Forward_DebugSetLevel() {
    __asm { jmp dword ptr [g_p_DebugSetLevel] }
}
__declspec(naked) void Forward_DebugSetMute() {
    __asm { jmp dword ptr [g_p_DebugSetMute] }
}
__declspec(naked) void Forward_Direct3D9EnableMaximizedWindowedModeShim() {
    __asm { jmp dword ptr [g_p_Direct3D9EnableMaximizedWindowedModeShim] }
}
__declspec(naked) void Forward_Direct3DCreate9Ex() {
    __asm { jmp dword ptr [g_p_Direct3DCreate9Ex] }
}
__declspec(naked) void Forward_Direct3DCreate9On12() {
    __asm { jmp dword ptr [g_p_Direct3DCreate9On12] }
}
__declspec(naked) void Forward_Direct3DCreate9On12Ex() {
    __asm { jmp dword ptr [g_p_Direct3DCreate9On12Ex] }
}
__declspec(naked) void Forward_Direct3DShaderValidatorCreate9() {
    __asm { jmp dword ptr [g_p_Direct3DShaderValidatorCreate9] }
}
__declspec(naked) void Forward_PSGPError() {
    __asm { jmp dword ptr [g_p_PSGPError] }
}
__declspec(naked) void Forward_PSGPSampleTexture() {
    __asm { jmp dword ptr [g_p_PSGPSampleTexture] }
}

} // extern "C"

BOOL WINAPI DllMain(HINSTANCE hThis, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_hThis = hThis;
        // Resolve real d3d9 eagerly so AppCompat shims that call our
        // passthrough exports during process init get real targets, not
        // nullptr trampolines (DISABLEDXMAXIMIZEDWINDOWEDMODE shim is
        // known to call Direct3D9EnableMaximizedWindowedModeShim early).
        if (!LoadRealD3D9(hThis)) return FALSE;
    }
    return TRUE;
}
