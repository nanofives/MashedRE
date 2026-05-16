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

constexpr UINT kForceBackBufferWidth  = 640;
constexpr UINT kForceBackBufferHeight = 480;

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
    return g_OriginalCreateDevice(pThis, Adapter, DeviceType, hFocusWindow,
                                  BehaviorFlags, pPP, ppDevice);
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
