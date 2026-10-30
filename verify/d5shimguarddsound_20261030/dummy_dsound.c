/* D5-SHIM-GUARD-DSOUND dummy proxy: drops a marker beside itself, then fails DllMain.
   Ordinals match System32 dsound.dll (dsound.lib imports DirectSoundCreate8 by ordinal 11). */
#include <windows.h>
BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID r) {
    if (reason == DLL_PROCESS_ATTACH) {
        char p[MAX_PATH]; DWORD n = GetModuleFileNameA(h, p, MAX_PATH);
        if (n > 0 && n < MAX_PATH - 16) {
            lstrcatA(p, ".LOADED.txt");
            HANDLE f = CreateFileA(p, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
            if (f != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(f, "dsound", 6, &w, NULL); CloseHandle(f); }
        }
        return FALSE;
    }
    return TRUE;
}
#pragma comment(linker, "/EXPORT:DirectSoundCreate8=_DirectSoundCreate8@12,@11")
#pragma comment(linker, "/EXPORT:DirectSoundCreate=_DirectSoundCreate@12,@1")
long __stdcall DirectSoundCreate8(void* a, void** b, void* c) { return -1; }
long __stdcall DirectSoundCreate(void* a, void** b, void* c) { return -1; }
