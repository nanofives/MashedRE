/* D5-SHIM-GUARD dummy proxy: drops a marker beside itself, then fails DllMain. */
#include <windows.h>
#ifndef DUMMY_NAME
#define DUMMY_NAME "dummy"
#endif
BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, LPVOID r) {
    if (reason == DLL_PROCESS_ATTACH) {
        char p[MAX_PATH]; DWORD n = GetModuleFileNameA(h, p, MAX_PATH);
        if (n > 0 && n < MAX_PATH - 16) {
            lstrcatA(p, ".LOADED.txt");
            HANDLE f = CreateFileA(p, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
            if (f != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(f, DUMMY_NAME, lstrlenA(DUMMY_NAME), &w, NULL); CloseHandle(f); }
        }
        return FALSE;
    }
    return TRUE;
}
#pragma comment(linker, "/EXPORT:Direct3DCreate9=_Direct3DCreate9@4")
#pragma comment(linker, "/EXPORT:DirectInput8Create=_DirectInput8Create@20")
void* __stdcall Direct3DCreate9(UINT v) { return NULL; }
long __stdcall DirectInput8Create(void* a, DWORD b, void* c, void** d, void* e) { return -1; }
