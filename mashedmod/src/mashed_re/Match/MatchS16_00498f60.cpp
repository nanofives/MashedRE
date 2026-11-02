// MATCH-S16: port body of 0x00498f60 = the byte-matched source re/match/s16/00498f60.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Save/SettingsAndIO.cpp; .asi only). Compiled here as C++ with MATCH_PORT; globals bind to their original
// addresses (preamble MGLOBAL). The file's nine other video-dialog functions are HELD (not compiled here).
// The original takes hDlg in EAX plus three pushed args it never reads (caller 0x0049939b..0x004993ab), so the
// matched body is the cdecl VideoDialogInit_Body and the installed/exported VideoDialogInit_i3 is a register adapter.
// Callees: 0x00499740 = SetControlTextFromResource_i3 (Match/MatchS16_00499740.cpp), 0x00498d60 =
// PopulateModeCombo_s2 (Match/MatchS16_00498d60.cpp). Both C linkage, cdecl.
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
#define FUN_00498f60 VideoDialogInit_Body
#define FUN_00499740 SetControlTextFromResource_i3
#define FUN_00498d60 PopulateModeCombo_s2
extern "C" {
#include "../../../../re/match/s16/00498f60.c"
}

// EAX-hDlg entry for the hook and the Frida export. Not a body: it re-pushes the caller's three stack args
// (0x110, wParam, lParam), adds hDlg from EAX and forwards to VideoDialogInit_Body.
#pragma comment(linker, "/EXPORT:VideoDialogInit_i3=_VideoDialogInit_i3")
extern "C" __declspec(naked) void __cdecl VideoDialogInit_i3(void) {
    __asm {
        push dword ptr [esp+12]
        push dword ptr [esp+12]
        push dword ptr [esp+12]
        push eax
        call VideoDialogInit_Body
        add  esp, 16
        ret
    }
}
RH_ScopedInstall(VideoDialogInit_i3, 0x00498f60);
