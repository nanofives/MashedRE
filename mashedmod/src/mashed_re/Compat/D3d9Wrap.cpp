// RWC-DEVICE (L-124 (C) step 1): the in-exe IDirect3D9 / IDirect3DDevice9 wrapper for the top-down path.
// EXE ONLY (exe_sources.rsp). Header: D3d9Wrap.h. Knob MASHED_RWC_DEVICE=1, default OFF.
//
// Why: under L-124 the top-down path runs the matched RW core down to d3d9. In MASHED.exe the RW D3D9 driver creates
// its device through Direct3DCreate9 (import) and IDirect3D9::CreateDevice, and presents through IDirect3DDevice9::
// Present. On this machine the original only boots with the dev d3d9 proxy in front of those calls
// (mashedmod/src/d3d9_shim/d3d9_shim.cpp, re/analysis/BOOT_PATCHES.md "d3d9 shim" and "fix_camera_res"). This TU is
// that proxy's behaviour moved into the exe as COM wrapper objects, so no d3d9.dll proxy and no binary patch is needed:
//
//   shim behaviour                                   here
//   CreateDevice_ForceWindowed (vtbl[16])            WrapD3D9::CreateDevice: Windowed=TRUE, backbuffer forced,
//                                                    FullScreen_RefreshRateInHz=0, then the real call
//   ApplyWindowBorders after CreateDevice            ApplyWindowBorders, on the window's own thread (deferred to the
//                                                    first Present when CreateDevice runs on another thread: the
//                                                    exe's CreateDeviceWatched runs it on a worker)
//   Present_BBDump -> FrameLimit (vtbl[17])          WrapDevice9::Present -> FrameLimit, same env, same pacing
//   patch_mashed_fix_camera_res (0x00498bc0/bd0)     AssertRenderDims: the getters' only source cells 0x00616028 /
//                                                    0x0061602c hold the backbuffer size from CreateDevice on, and
//                                                    again before every Present
//
// The fix_camera_res patch rewrites the two getter bodies (`mov eax,[0x00616028]` -> `mov eax,640`). The port's getters
// are byte-matched bodies (re/match/s16/00498bc0.c, 00498bd0.c) and are not edited. In MASHED.exe.unpatched the only
// instructions that reference 0x00616028 / 0x0061602c are the two getters (reads, 0x00498bc1 / 0x00498bd1) and two
// writes in FUN_00499400 (0x00499604 / 0x0049961c); scan in verify/rwcdevice_20261111/RESULT_RWC-DEVICE.md. So writing
// the cells is the same as the patch for every reader, except a getter call between a FUN_00499400 write and the next
// Present (none was seen in the knob boot: the self-test logs every mismatch it re-asserts).
//
// Not carried over (dev instruments of the shim, not behaviours the original needs): MASHED_ORIG_BBDUMP / _REQ
// backbuffer dumps, the draw-call counters, the parity Clear colour, the FPS OSD, MashedShim_PresentCounter
// (MashedD3D9Wrap_PresentCount is its equivalent).
//
// Reset (vtbl[16] of the device) is forced the same way as CreateDevice. The shim does not hook Reset; a Reset with the
// original's fullscreen parameters would undo the forced mode, so the wrapper applies the same forcing and logs it.

#include "D3d9Wrap.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" unsigned int VideoGetRenderWidth(void);    // 0x00498bc0, matched (Match/MatchS16_00498bc0.cpp)
extern "C" unsigned int VideoGetRenderHeight(void);   // 0x00498bd0, matched (Match/MatchS16_00498bd0.cpp)

namespace {

// ---- log: one line per event into mashed_re.log (the knob path's log, PortTop_Log's file) ----
void WrapLog(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        va_list ap;
        va_start(ap, fmt);
        std::fputs("[RWC-DEVICE] ", f);
        std::vfprintf(f, fmt, ap);
        std::fputc('\n', f);
        va_end(ap);
        std::fclose(f);
    }
}

bool EnvOne(const char* name) {
    char b[8] = {};
    return GetEnvironmentVariableA(name, b, sizeof(b)) > 0 && b[0] == '1';
}

// ---- forced backbuffer: d3d9_shim.cpp ForcedBackBufInit, same env and bounds ----
void ForcedBackBuf(UINT* pw, UINT* ph) {
    static UINT s_w = 0, s_h = 0;
    if (!s_w) {
        char v[32] = {};
        UINT w = 0, h = 0;
        if (GetEnvironmentVariableA("MASHED_RES", v, sizeof(v)) > 0) {
            const char* p = v;
            for (; *p >= '0' && *p <= '9'; ++p) w = w * 10 + static_cast<UINT>(*p - '0');
            if (*p == 'x' || *p == 'X')
                for (++p; *p >= '0' && *p <= '9'; ++p) h = h * 10 + static_cast<UINT>(*p - '0');
        }
        if (w >= 320 && h >= 240 && w <= 7680 && h <= 4320) {
            s_w = w; s_h = h;
        } else {
            const bool hires = EnvOne("MASHED_HIRES");
            s_w = hires ? 1280u : 640u;
            s_h = hires ? 960u : 480u;
        }
    }
    *pw = s_w;
    *ph = s_h;
}

// ---- fix_camera_res coupling: the getters' source cells hold the backbuffer size ----
volatile unsigned int& CellW() { return *reinterpret_cast<volatile unsigned int*>(0x00616028u); }   // DAT_00616028
volatile unsigned int& CellH() { return *reinterpret_cast<volatile unsigned int*>(0x0061602cu); }   // DAT_0061602c
LONG g_coupleWrites = 0;

void AssertRenderDims(const char* where) {
    UINT w = 0, h = 0;
    ForcedBackBuf(&w, &h);
    const unsigned int cw = CellW(), ch = CellH();
    if (cw == w && ch == h) return;
    CellW() = w;
    CellH() = h;
    const LONG n = InterlockedIncrement(&g_coupleWrites);
    if (n <= 8) WrapLog("coupling at %s: getter cells 0x00616028/0x0061602c were %ux%u, set to the backbuffer %ux%u",
                        where, cw, ch, w, h);
}

// ---- window border: d3d9_shim.cpp ApplyWindowBorders, same env (MASHED_QOL_BORDERLESS, MASHED_RE_BORDERLESS,
// MASHED_RE_NO_SCREEN1_PIN, MASHED_WIN_POS) ----
struct MonPick { int mode, want, idx; RECT work; bool got; };

BOOL CALLBACK PickMonitor(HMONITOR hm, HDC, LPRECT, LPARAM lp) {
    MonPick* e = reinterpret_cast<MonPick*>(lp);
    ++e->idx;
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoA(hm, &mi)) return TRUE;
    switch (e->mode) {
        case 0: if (e->idx == e->want) { e->work = mi.rcWork; e->got = true; return FALSE; } break;
        case 1: if (!e->got || mi.rcWork.left < e->work.left) { e->work = mi.rcWork; e->got = true; } break;
        case 2: if (!e->got || mi.rcWork.left > e->work.left) { e->work = mi.rcWork; e->got = true; } break;
        case 3: if (mi.dwFlags & MONITORINFOF_PRIMARY) { e->work = mi.rcWork; e->got = true; return FALSE; } break;
    }
    return TRUE;
}

void ApplyWindowBorders(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    UINT bw = 0, bh = 0;
    ForcedBackBuf(&bw, &bh);
    if (EnvOne("MASHED_QOL_BORDERLESS")) {
        const LONG_PTR cur = GetWindowLongPtrA(hWnd, GWL_STYLE);
        if (cur & WS_CHILD) return;
        SetWindowLongPtrA(hWnd, GWL_STYLE, WS_POPUP | (cur & WS_VISIBLE));
        SetWindowPos(hWnd, HWND_TOP, 0, 0, static_cast<int>(bw), static_cast<int>(bh),
                     SWP_FRAMECHANGED | SWP_NOACTIVATE);
        WrapLog("window: borderless popup %ux%u at 0,0 (MASHED_QOL_BORDERLESS)", bw, bh);
        return;
    }
    if (EnvOne("MASHED_RE_BORDERLESS")) {
        WrapLog("window: style left as created (MASHED_RE_BORDERLESS)");
        return;
    }
    const LONG_PTR cur = GetWindowLongPtrA(hWnd, GWL_STYLE);
    if (cur & WS_CHILD) return;
    const LONG_PTR kBorderStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    SetWindowLongPtrA(hWnd, GWL_STYLE, kBorderStyle | (cur & WS_VISIBLE));
    RECT rc = {0, 0, static_cast<LONG>(bw), static_cast<LONG>(bh)};
    AdjustWindowRect(&rc, static_cast<DWORD>(kBorderStyle), FALSE);
    const bool pin = !EnvOne("MASHED_RE_NO_SCREEN1_PIN");
    UINT moveFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | (pin ? 0u : SWP_NOMOVE);
    int posX = 64, posY = 64;
    char wp[64] = {};
    if (GetEnvironmentVariableA("MASHED_WIN_POS", wp, sizeof(wp)) > 0 && wp[0]) {
        const int w = rc.right - rc.left, h = rc.bottom - rc.top;
        const char* dash = std::strrchr(wp, '-');
        bool got = false;
        if (dash && dash > wp) {
            const size_t nlen = static_cast<size_t>(dash - wp);
            int mode = -1, want = 0;
            if      (nlen == 4 && _strnicmp(wp, "left",    4) == 0) mode = 1;
            else if (nlen == 5 && _strnicmp(wp, "right",   5) == 0) mode = 2;
            else if (nlen == 7 && _strnicmp(wp, "primary", 7) == 0) mode = 3;
            else if (wp[0] == 's' && std::sscanf(wp + 1, "%d", &want) == 1 && want >= 1) mode = 0;
            if (mode >= 0) {
                MonPick p{mode, want, 0, {}, false};
                EnumDisplayMonitors(nullptr, nullptr, PickMonitor, reinterpret_cast<LPARAM>(&p));
                if (p.got) {
                    const char* c = dash + 1;
                    const bool bottom = (c[0] == 'b' || c[0] == 'B');
                    const bool right = (c[0] && (c[1] == 'r' || c[1] == 'R'));
                    posX = right ? (p.work.right - w) : p.work.left;
                    posY = bottom ? (p.work.bottom - h) : p.work.top;
                    got = true;
                }
            }
        }
        if (!got && std::sscanf(wp, "%d,%d", &posX, &posY) == 2) got = true;
        if (got) moveFlags &= ~static_cast<UINT>(SWP_NOMOVE);
    }
    SetWindowPos(hWnd, nullptr, posX, posY, rc.right - rc.left, rc.bottom - rc.top, moveFlags);
    RECT cr{};
    GetClientRect(hWnd, &cr);
    WrapLog("window: titled border, client %ldx%ld at %d,%d", static_cast<long>(cr.right - cr.left),
            static_cast<long>(cr.bottom - cr.top), posX, posY);
}

// ---- frame limiter: d3d9_shim.cpp FrameLimit, same env (MASHED_FPS_CAP default 60, MASHED_FPS_CAP_RACE on
// DAT_00771968 phases 3 / 6, MASHED_FPS_LOG=1 -> log\rwc_fps_limiter.txt beside the exe's cwd) ----
int ParseUInt(const char* s) {
    int v = 0;
    for (const char* c = s; *c >= '0' && *c <= '9'; ++c) v = v * 10 + (*c - '0');
    return v;
}

void FrameLimit() {
    static int s_cap = -1, s_capRace = -1, s_log = 0, s_frames = 0;
    static LARGE_INTEGER s_freq = {}, s_last = {}, s_logLast = {};
    if (s_cap == -1) {
        char buf[16] = {};
        DWORD got = GetEnvironmentVariableA("MASHED_FPS_CAP", buf, sizeof(buf));
        int v = 60;
        if (got > 0 && got < sizeof(buf)) v = ParseUInt(buf);
        s_cap = v < 0 ? 0 : v;
        got = GetEnvironmentVariableA("MASHED_FPS_CAP_RACE", buf, sizeof(buf));
        if (got > 0 && got < sizeof(buf)) s_capRace = ParseUInt(buf);
        s_log = GetEnvironmentVariableA("MASHED_FPS_LOG", buf, sizeof(buf)) > 0;
        QueryPerformanceFrequency(&s_freq);
        QueryPerformanceCounter(&s_last);
        s_logLast = s_last;
        WrapLog("frame limiter: cap %d FPS, race cap %d (-1 = off)", s_cap, s_capRace);
    }
    int cap = s_cap;
    if (s_capRace >= 0) {
        const DWORD phase = *reinterpret_cast<volatile DWORD*>(0x00771968u);   // DAT_00771968
        if (phase == 3 || phase == 6) cap = s_capRace;
    }
    LARGE_INTEGER now;
    if (cap > 0 && s_freq.QuadPart != 0) {
        const LONGLONG target = s_freq.QuadPart / cap;
        for (;;) {
            QueryPerformanceCounter(&now);
            const LONGLONG elapsed = now.QuadPart - s_last.QuadPart;
            if (elapsed >= target) break;
            const LONGLONG remainMs = (target - elapsed) * 1000 / s_freq.QuadPart;
            if (remainMs > 2) Sleep(1); else SwitchToThread();
        }
        s_last = now;
    } else {
        QueryPerformanceCounter(&now);
    }
    if (s_log && s_freq.QuadPart != 0) {
        ++s_frames;
        const LONGLONG since = now.QuadPart - s_logLast.QuadPart;
        if (since >= s_freq.QuadPart) {
            const double fps = static_cast<double>(s_frames) * static_cast<double>(s_freq.QuadPart) /
                               static_cast<double>(since);
            CreateDirectoryA("log", nullptr);
            if (std::FILE* lf = std::fopen("log\\rwc_fps_limiter.txt", "a")) {
                std::fprintf(lf, "present_fps=%.1f (cap=%d)\n", fps, cap);
                std::fclose(lf);
            }
            s_frames = 0;
            s_logLast = now;
        }
    }
}

void ForcePresentParams(D3DPRESENT_PARAMETERS* pp, const char* what) {
    if (!pp) return;
    UINT w = 0, h = 0;
    ForcedBackBuf(&w, &h);
    WrapLog("%s: asked windowed %d %ux%u refresh %u -> forced windowed 1 %ux%u refresh 0", what,
            static_cast<int>(pp->Windowed), pp->BackBufferWidth, pp->BackBufferHeight, pp->FullScreen_RefreshRateInHz,
            w, h);
    pp->Windowed = TRUE;
    pp->BackBufferWidth = w;
    pp->BackBufferHeight = h;
    pp->FullScreen_RefreshRateInHz = 0;
}

LONG g_presentCount = 0;
// Live device wrappers, for MashedD3D9Wrap_RealDevice (one device in practice; 4 slots).
void* g_liveWraps[4] = {};

class WrapD3D9;

class WrapDevice9 final : public IDirect3DDevice9 {
public:
    WrapDevice9(IDirect3DDevice9* real, WrapD3D9* parent, HWND window);
    ~WrapDevice9();

    IDirect3DDevice9* Real() const { return m_real; }

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DDevice9) {
            *ppv = static_cast<IDirect3DDevice9*>(this);
            AddRef();
            return S_OK;
        }
        return m_real->QueryInterface(riid, ppv);
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        InterlockedIncrement(&m_ref);
        return m_real->AddRef();
    }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG r = m_real->Release();
        if (InterlockedDecrement(&m_ref) == 0) delete this;
        return r;
    }

    HRESULT STDMETHODCALLTYPE GetDirect3D(IDirect3D9** ppD3D9) override;

    HRESULT STDMETHODCALLTYPE Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) override {
        ForcePresentParams(pPresentationParameters, "Reset");
        const HRESULT hr = m_real->Reset(pPresentationParameters);
        WrapLog("Reset -> 0x%08lX", static_cast<unsigned long>(hr));
        if (SUCCEEDED(hr)) AssertRenderDims("Reset");
        return hr;
    }

    HRESULT STDMETHODCALLTYPE Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride,
                                      const RGNDATA* pDirtyRegion) override {
        if (m_bordersPending) {
            m_bordersPending = false;
            ApplyWindowBorders(m_window);
        }
        AssertRenderDims("Present");
        FrameLimit();
        const LONG n = InterlockedIncrement(&g_presentCount);
        if (n == 1) SelfTest();
        return m_real->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
    }

#include "D3d9Wrap_IDirect3DDevice9.inc"

    void SetBordersPending() { m_bordersPending = true; }

private:
    void SelfTest();

    IDirect3DDevice9* m_real;
    WrapD3D9* m_parent;
    HWND m_window;
    volatile LONG m_ref = 1;
    bool m_bordersPending = false;
};

class WrapD3D9 final : public IDirect3D9 {
public:
    explicit WrapD3D9(IDirect3D9* real) : m_real(real) {}
    IDirect3D9* Real() const { return m_real; }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3D9) {
            *ppv = static_cast<IDirect3D9*>(this);
            AddRef();
            return S_OK;
        }
        return m_real->QueryInterface(riid, ppv);
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        InterlockedIncrement(&m_ref);
        return m_real->AddRef();
    }
    ULONG STDMETHODCALLTYPE Release() override {
        const ULONG r = m_real->Release();
        if (InterlockedDecrement(&m_ref) == 0) delete this;
        return r;
    }

    // vtbl[16]: d3d9_shim.cpp CreateDevice_ForceWindowed.
    HRESULT STDMETHODCALLTYPE CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                           D3DPRESENT_PARAMETERS* pPP, IDirect3DDevice9** ppDevice) override {
        ForcePresentParams(pPP, "CreateDevice");
        const HWND hWnd = (pPP && pPP->hDeviceWindow) ? pPP->hDeviceWindow : hFocusWindow;
        const HRESULT hr = m_real->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPP, ppDevice);
        WrapLog("CreateDevice(adapter %u, type %d, flags 0x%08lX) -> 0x%08lX", Adapter, static_cast<int>(DeviceType),
                static_cast<unsigned long>(BehaviorFlags), static_cast<unsigned long>(hr));
        if (SUCCEEDED(hr) && ppDevice && *ppDevice) {
            WrapDevice9* w = new WrapDevice9(*ppDevice, this, hWnd);
            *ppDevice = w;
            AssertRenderDims("CreateDevice");
            if (hWnd && GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId()) {
                ApplyWindowBorders(hWnd);
            } else {
                // The window's thread waits on this call (exe CreateDeviceWatched runs CreateDevice on a worker), so
                // a SetWindowPos from here would block on it. The border goes on at the first Present instead.
                w->SetBordersPending();
                WrapLog("window: border deferred to the first Present (CreateDevice not on the window's thread)");
            }
        }
        return hr;
    }

#include "D3d9Wrap_IDirect3D9.inc"

private:
    IDirect3D9* m_real;
    volatile LONG m_ref = 1;
};

WrapDevice9::WrapDevice9(IDirect3DDevice9* real, WrapD3D9* parent, HWND window)
    : m_real(real), m_parent(parent), m_window(window) {
    m_parent->AddRef();
    for (void*& slot : g_liveWraps)
        if (InterlockedCompareExchangePointer(&slot, this, nullptr) == nullptr) break;
}

WrapDevice9::~WrapDevice9() {
    for (void*& slot : g_liveWraps) InterlockedCompareExchangePointer(&slot, nullptr, this);
    m_parent->Release();
}

HRESULT STDMETHODCALLTYPE WrapDevice9::GetDirect3D(IDirect3D9** ppD3D9) {
    if (!ppD3D9) return D3DERR_INVALIDCALL;
    m_parent->AddRef();
    *ppD3D9 = m_parent;
    return D3D_OK;
}

// First Present: check what this row promises, and log one PASS / FAIL line the knob boot reads.
void WrapDevice9::SelfTest() {
    UINT w = 0, h = 0;
    ForcedBackBuf(&w, &h);
    int fails = 0;
    char detail[512] = {};
    size_t used = 0;
    auto note = [&](bool ok, const char* fmt, ...) {
        if (!ok) ++fails;
        if (used < sizeof(detail) - 1) {
            va_list ap;
            va_start(ap, fmt);
            const int n = std::vsnprintf(detail + used, sizeof(detail) - used, fmt, ap);
            va_end(ap);
            if (n > 0) used += static_cast<size_t>(n);
            if (used < sizeof(detail) - 1) detail[used++] = ok ? ' ' : '!';
        }
    };
    // 1. the backbuffer is the forced size
    IDirect3DSurface9* bb = nullptr;
    D3DSURFACE_DESC d = {};
    if (SUCCEEDED(m_real->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &bb)) && bb) {
        bb->GetDesc(&d);
        bb->Release();
    }
    note(d.Width == w && d.Height == h, "backbuffer=%ux%u", d.Width, d.Height);
    // 2. windowed swap chain
    IDirect3DSwapChain9* sc = nullptr;
    D3DPRESENT_PARAMETERS pp = {};
    if (SUCCEEDED(m_real->GetSwapChain(0, &sc)) && sc) {
        sc->GetPresentParameters(&pp);
        sc->Release();
    }
    note(pp.Windowed == TRUE, "windowed=%d", static_cast<int>(pp.Windowed));
    // 3. the getters 0x00498bc0 / 0x00498bd0 return the backbuffer size
    const unsigned int gw = VideoGetRenderWidth(), gh = VideoGetRenderHeight();
    note(gw == w && gh == h, "getters=%ux%u", gw, gh);
    // 4. GetDirect3D returns the wrapper, QueryInterface(IDirect3DDevice9) returns this wrapper
    IDirect3D9* d3d = nullptr;
    GetDirect3D(&d3d);
    note(d3d == m_parent, "GetDirect3D=%s", d3d == m_parent ? "wrapper" : "other");
    if (d3d) d3d->Release();
    IDirect3DDevice9* qi = nullptr;
    QueryInterface(IID_IDirect3DDevice9, reinterpret_cast<void**>(&qi));
    note(qi == this, "QI=%s", qi == this ? "wrapper" : "other");
    if (qi) qi->Release();
    // 5. the window client area is the backbuffer size (border applied on this Present before the test)
    RECT cr{};
    GetClientRect(m_window, &cr);
    const LONG cw = cr.right - cr.left, ch = cr.bottom - cr.top;
    const bool bordered = !EnvOne("MASHED_RE_BORDERLESS");
    note(!bordered || (cw == static_cast<LONG>(w) && ch == static_cast<LONG>(h)), "client=%ldx%ld",
         static_cast<long>(cw), static_cast<long>(ch));
    detail[used < sizeof(detail) ? used : sizeof(detail) - 1] = 0;
    WrapLog("selftest %s (%d fail): %s", fails == 0 ? "PASS" : "FAIL", fails, detail);
}

}  // namespace

extern "C" {

bool MashedD3D9Wrap_Enabled(void) {
    static const bool s_on = [] {
        char b[8] = {};
        const bool on = GetEnvironmentVariableA("MASHED_RWC_DEVICE", b, sizeof(b)) > 0 && b[0] && b[0] != '0';
        if (on) WrapLog("MASHED_RWC_DEVICE=1: the exe's d3d9 goes through the in-exe wrapper");
        return on;
    }();
    return s_on;
}

IDirect3D9* MashedD3D9Wrap_Direct3DCreate9(UINT sdkVersion) {
    IDirect3D9* real = Direct3DCreate9(sdkVersion);
    WrapLog("Direct3DCreate9(%u) -> real %p", sdkVersion, static_cast<void*>(real));
    if (!real) return nullptr;
    return new WrapD3D9(real);
}

IDirect3DDevice9* MashedD3D9Wrap_RealDevice(IDirect3DDevice9* dev) {
    if (!dev) return nullptr;
    for (void* slot : g_liveWraps)
        if (slot == dev) return static_cast<WrapDevice9*>(dev)->Real();
    return dev;
}

LONG MashedD3D9Wrap_PresentCount(void) { return g_presentCount; }

}  // extern "C"
