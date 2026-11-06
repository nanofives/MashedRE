// MATCH-S16: port body of 0x00499740 = the byte-matched source re/match/s16/00499740.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Save/SettingsAndIO.cpp; .asi only). Compiled here as C++ with MATCH_PORT; globals bind to their original
// addresses (preamble MGLOBAL). __cdecl (HWND, id) on the stack, installed as is.
//
// HookSystem.h is not included: it pulls <windows.h>, whose HWND/UINT typedefs collide with the matched source's
// own Win32 typedefs. The same registration (Core/HookSystem.h RH_ScopedInstall) is declared here.
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
#define FUN_00499740 SetControlTextFromResource_i3
#pragma comment(linker, "/EXPORT:SetControlTextFromResource_i3=_SetControlTextFromResource_i3")
extern "C" {
#include "../../../../re/match/s16/00499740.c"
}
RH_ScopedInstall(SetControlTextFromResource_i3, 0x00499740);
