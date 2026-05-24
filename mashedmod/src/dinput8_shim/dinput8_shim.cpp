// Mashed RE — dinput8.dll proxy that autoloads mashed_re_dev.asi.
//
// Purpose: give the build an Ultimate-ASI-Loader-style autoload mechanism
// without disturbing the d3d9 windowed-mode shim. MASHED imports dinput8
// for keyboard/joystick input; Windows resolves the import to our proxy
// (which lives in original/ next to MASHED.exe; app dir precedes
// System32 in DLL search order).
//
// All real dinput8 exports are forwarded via the .def file to
// dinput8_real.dll (a copy of SysWOW64\dinput8.dll). Our DllMain only
// LoadLibrarys the .asi and returns.
//
// Why dinput8 not d3d9: empirically, LoadLibrary on the .asi from
// inside d3d9_shim's DllMain crashes MASHED at t<1s regardless of
// mechanism (DllMain, deferred-thread, lazy-on-Direct3DCreate9). The
// dinput8 import lands at a different point in MASHED's loader chain;
// testing whether the .asi survives load via this slot.

#include <windows.h>

// Export forwarders — MSVC link.exe resolves these to dinput8_real.dll
// at our DLL load time without requiring local stubs. dinput8_real.dll is
// pre-copied next to this proxy by build_dinput8_shim.bat (loader-dedup
// workaround, same as d3d9_real.dll).
#pragma comment(linker, "/EXPORT:DirectInput8Create=dinput8_real.DirectInput8Create")
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=dinput8_real.DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:DllGetClassObject=dinput8_real.DllGetClassObject,PRIVATE")
#pragma comment(linker, "/EXPORT:DllRegisterServer=dinput8_real.DllRegisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:DllUnregisterServer=dinput8_real.DllUnregisterServer,PRIVATE")
#pragma comment(linker, "/EXPORT:GetdfDIJoystick=dinput8_real.GetdfDIJoystick")

namespace {
HMODULE g_HookAsi = nullptr;

void LoadDevAsi(HINSTANCE hThis) {
    wchar_t our_dir[MAX_PATH];
    DWORD got = GetModuleFileNameW(hThis, our_dir, MAX_PATH);
    if (got == 0 || got >= MAX_PATH) return;
    for (DWORD i = got; i > 0; --i) {
        if (our_dir[i-1] == L'\\' || our_dir[i-1] == L'/') {
            our_dir[i] = 0;
            break;
        }
    }

    // 1) next to this DLL (original\mashed_re_dev.asi) — deploy target
    wchar_t local_asi[MAX_PATH];
    if (lstrlenW(our_dir) + 20 < MAX_PATH) {
        lstrcpyW(local_asi, our_dir);
        lstrcatW(local_asi, L"mashed_re_dev.asi");
        g_HookAsi = LoadLibraryW(local_asi);
        if (g_HookAsi) return;
    }

    // 2) ..\mashedmod\build\mashed_re_dev.asi (dev path)
    wchar_t build_asi[MAX_PATH];
    if (lstrlenW(our_dir) + 40 < MAX_PATH) {
        lstrcpyW(build_asi, our_dir);
        lstrcatW(build_asi, L"..\\mashedmod\\build\\mashed_re_dev.asi");
        g_HookAsi = LoadLibraryW(build_asi);
    }
}
} // namespace

BOOL WINAPI DllMain(HINSTANCE hThis, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hThis);
        LoadDevAsi(hThis);
    }
    return TRUE;
}
