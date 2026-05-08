// Mashed RE — dev-mode hook DLL (mashed_re_dev.asi).
// Loaded into MASHED.exe by Ultimate-ASI-Loader during development.
// Never shipped: the standalone exe links the same source directly.
#include <windows.h>
#include "Core/HookSystem.h"

extern "C" __declspec(dllexport) void InjectHooks() {
    HookSystem::InstallAll();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        InjectHooks();
    }
    return TRUE;
}
