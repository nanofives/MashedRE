// Mashed RE — dev-mode hook DLL scaffold (mashed_re_dev.asi).
// Phase 4 milestone: empty DllMain that registers an InjectHooks bootstrap.
// Loaded into MASHED.exe by Ultimate-ASI-Loader during development.
// Never shipped: the standalone exe links the same source directly.
#include <windows.h>

extern "C" __declspec(dllexport) void InjectHooks() {
    // Phase 4 leaf-function hook lands here once the F-DoD demo is picked.
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        InjectHooks();
    }
    return TRUE;
}
