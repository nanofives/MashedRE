// Mashed RE — dev-mode hook DLL (mashed_re_dev.asi).
// Loaded into MASHED.exe by Ultimate-ASI-Loader during development.
// Never shipped: the standalone exe links the same source directly.
//
// Auto-install can be suppressed by setting env var MASHED_RE_NO_AUTO_HOOK=1
// before the DLL loads. The Frida A/B harness uses this so it can call our
// reimpl side-by-side with the unpatched original against the same live LUT.
#include <windows.h>
#include <cstdlib>
#include "Core/HookSystem.h"

extern "C" __declspec(dllexport) void InjectHooks() {
    HookSystem::InstallAll();
}

extern "C" __declspec(dllexport) void UninjectHooks() {
    HookSystem::UninstallAll();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        const char* skip = std::getenv("MASHED_RE_NO_AUTO_HOOK");
        if (skip == nullptr || skip[0] == '\0' || skip[0] == '0') {
            InjectHooks();
        }
    }
    return TRUE;
}
