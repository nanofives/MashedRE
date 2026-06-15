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
// env MASHED_HIRES=1 -> 1280x960 (2x) for high-res parity capture. COUPLING:
// must equal the camera-res patch's screen-dim getters AND the RE's kWidth/
// kHeight — re-apply patch_mashed_fix_camera_res.py at the matching res.
static UINT ForcedBackBufW() {
    static UINT w = 0;
    if (!w) { char v[16] = {}; w = (GetEnvironmentVariableA("MASHED_HIRES", v, sizeof(v)) > 0 && v[0] == '1') ? 1280u : 640u; }
    return w;
}
static UINT ForcedBackBufH() {
    static UINT h = 0;
    if (!h) { char v[16] = {}; h = (GetEnvironmentVariableA("MASHED_HIRES", v, sizeof(v)) > 0 && v[0] == '1') ? 960u : 480u; }
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
    SetWindowPos(hWnd, nullptr, 64, 64,
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

HRESULT STDMETHODCALLTYPE Present_BBDump(
    IDirect3DDevice9* pThis, const RECT* src, const RECT* dst,
    HWND wnd, const RGNDATA* dirty)
{
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
        if (target[0]) DumpBackbufferBMPPath(pThis, target);
    }
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

void PatchPresentSlot(IDirect3DDevice9* dev) {
    ParseDumpFramesOnce();
    ParseReqOnce();
    if ((g_DumpFrameCount <= 0 && g_ReqArmed != 1) || !dev) return;  // not armed
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
