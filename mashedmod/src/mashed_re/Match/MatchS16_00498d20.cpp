// MATCH-S16: port body of 0x00498d20 = the byte-matched source re/match/s16/00498d20.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Save/SettingsDialog.cpp; .asi only). Compiled here as C++ with MATCH_PORT; globals bind to their original
// addresses (preamble MGLOBAL). The file's nine other video-dialog functions are HELD (not compiled here).
// The original passes hDlg in EAX (callers 0x0049925e / 0x004992ab), so the matched body is the cdecl
// ReadModeFromCombo_Body and the installed/exported entry ReadModeFromCombo_j5 is a register adapter.
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
#define FUN_00498d20 ReadModeFromCombo_Body
extern "C" {
#include "../../../../re/match/s16/00498d20.c"
}

// EAX-hDlg entry for the hook and the Frida export. Not a body: it only forwards to ReadModeFromCombo_Body.
// The original returns with EAX = the CB_GETITEMDATA result it also stores at 0x00773200 (the MOV does not
// clobber EAX), so the adapter reloads it from there.
#pragma comment(linker, "/EXPORT:ReadModeFromCombo_j5=_ReadModeFromCombo_j5")
extern "C" __declspec(naked) void __cdecl ReadModeFromCombo_j5(void) {
    __asm {
        push eax
        call ReadModeFromCombo_Body
        add  esp, 4
        mov  eax, dword ptr ds:[00773200h]
        ret
    }
}
RH_ScopedInstall(ReadModeFromCombo_j5, 0x00498d20);
