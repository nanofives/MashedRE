// MATCH-S16: port body of 0x00498d60 = the byte-matched source re/match/s16/00498d60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Save/SettingsDialog.cpp; .asi only). Compiled here as C++ with MATCH_PORT; globals bind to their original
// addresses (preamble MGLOBAL). The file's nine other video-dialog functions are HELD (not compiled here).
// __cdecl, the combo HWND on the stack (callers 0x00499136 / 0x004991ce), so the matched body is installed as is.
// Callee 0x00498a00 (mode record in EBX) has no .asi port body (its only port body, ModeLabel in
// Boot/VideoCfgFirstRun.cpp, is exe-only and takes a port VideoMode): the old body called the original by address,
// and so does the forwarder below.
//
// HookSystem.h is not included: it pulls <windows.h>, whose HWND/UINT/WPARAM/LPARAM typedefs collide with the
// matched source's own Win32 typedefs. The same registration (Core/HookSystem.h RH_ScopedInstall) is declared here.
#include <cstdint>
namespace HookSystem { void Register(std::uintptr_t target_rva, void* replacement, const char* name); }
#ifndef RH_ScopedInstall
#define RH_ScopedInstall(Method, RVA)                                       \
    static struct AutoRegister_##Method {                                   \
        AutoRegister_##Method() {                                           \
            ::HookSystem::Register(                                         \
                (RVA), reinterpret_cast<void*>(&Method), #Method);          \
        }                                                                   \
    } s_AutoReg_##Method
#endif

#define MATCH_PORT 1
#define FUN_00498d60 PopulateModeCombo_s2
#define FUN_00498a00 MatchS16_00498d60_Call00498a00
#pragma comment(linker, "/EXPORT:PopulateModeCombo_s2=_PopulateModeCombo_s2")
extern "C" {
#include "../../../../re/match/s16/00498d60.c"

// Raw-address call of the original 0x00498a00: the buffer on the stack, the mode record in EBX, the caller
// cleans 4 (0x00498dd4..0x00498ddd).
void MatchS16_00498d60_Call00498a00(Mode *m, char *dst) {
    __asm {
        push ebx
        mov  ebx, m
        push dst
        mov  ecx, 00498a00h
        call ecx
        add  esp, 4
        pop  ebx
    }
}
}
RH_ScopedInstall(PopulateModeCombo_s2, 0x00498d60);
