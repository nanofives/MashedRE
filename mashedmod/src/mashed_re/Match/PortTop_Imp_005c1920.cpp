// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-AUDIO-5AEF30-KNOB (2026-11-16): exe body of the import thunk 0x005c1920 = `jmp dword ptr [0x5cc01c]`
// (listing of MASHED.exe.unpatched), IAT slot 0x005cc01c = DSOUND.dll ordinal 11 (import directory, pefile). The exe has
// no import of it, so the thunk resolves the same ordinal at its first call and jumps there with the caller's arguments.
// Called by the provisional 0x005bad30 (call at 0x005bad76, three pushed arguments, no add esp after it: stdcall).
// Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT (+ MASHED_OT_AUDIOTHREAD_STANDIN to get past 0x005aef30).
#ifdef MASHED_STANDALONE
#include <windows.h>
#include <intrin.h>
#include <cstdio>

namespace {
typedef int(__stdcall* Ord11Fn)(int, int**, int);
Ord11Fn s_ord11 = nullptr;
bool s_tried = false;
} // namespace

extern "C" {
void PortTop_AudioCalleeStop(unsigned rva, unsigned caller, void* ret);   // Match/PortTop_00492370.cpp
// Thunk body for rva 0x005c1920 (import thunk, DSOUND.dll ordinal 11)
int __stdcall PortTop_Imp_005c1920(int p0, int** p1, int p2) {
    if (!s_tried) {
        s_tried = true;
        if (HMODULE m = LoadLibraryA("dsound.dll"))
            s_ord11 = reinterpret_cast<Ord11Fn>(GetProcAddress(m, MAKEINTRESOURCEA(11)));
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x005c1920: import thunk -> DSOUND.dll ordinal 11 %p (PORT-TOP-AUDIO-5AEF30-KNOB)\n",
                         reinterpret_cast<void*>(s_ord11));
            std::fclose(f);
        }
    }
    if (!s_ord11) PortTop_AudioCalleeStop(0x005c1920u, 0u, _ReturnAddress());   // no ordinal 11: stop (logged above)
    return s_ord11(p0, p1, p2);
}
}
#endif // MASHED_STANDALONE
