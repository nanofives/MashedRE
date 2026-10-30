// DllGuard — load a system DLL from System32 only (exe target).
//
// D5-SHIM-GUARD (d3d9.dll / dinput8.dll, via the delay-load hook in exe_main.cpp)
// and D5-SHIM-GUARD-DSOUND (dsound.dll, explicit load). A by-name load searches
// the exe's own directory first, so a proxy DLL beside mashed_re.exe used to be
// bound (verify/d5shimstatic_20261030 R1/R14). LOAD_LIBRARY_SEARCH_SYSTEM32
// searches System32 only. The resolved path goes to mashed_re.log as
// "dllguard: <dll> -> <path>". Callers fail closed on nullptr: none of them falls
// back to the default search.
#pragma once

#include <windows.h>
#include <cstdio>

namespace mashed_re {

inline HMODULE DllGuardLoadSystem32(const char* name) {
    HMODULE mod = LoadLibraryExA(name, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    char path[MAX_PATH] = "(load failed)";
    if (mod != nullptr) GetModuleFileNameA(mod, path, MAX_PATH);
    if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
        std::fprintf(lf, "dllguard: %s -> %s\n", name, path);
        std::fclose(lf);
    }
    return mod;
}

}  // namespace mashed_re
