// MATCH-S16: port body of 0x00499170 = the byte-matched source re/match/s16/00499170.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Save/SettingsAndIO.cpp; .asi only). Compiled here as C++ with MATCH_PORT; globals bind to their original
// addresses (preamble MGLOBAL). The file's nine other video-dialog functions are HELD (not compiled here).
// The original takes hDlg in EAX (0x00499179), so the matched body is the cdecl SubsystemSelectionChanged_Body and
// the installed/exported SubsystemSelectionChanged_i3 is a register adapter.
// Callees: 0x004c2e70 = RwEngineSetSubSystem (Render/RwPluginHelpers_o3.cpp), 0x004c2f00 = RwEngineGetCurrentMode
// (Boot/VideoConfig.cpp), 0x00498d60 = PopulateModeCombo_s2 (Match/MatchS16_00498d60.cpp). All C linkage, cdecl.
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
#define FUN_00499170 SubsystemSelectionChanged_Body
#define FUN_004c2e70 RwEngineSetSubSystem
#define FUN_004c2f00 RwEngineGetCurrentMode
#define FUN_00498d60 PopulateModeCombo_s2
extern "C" {
#include "../../../../re/match/s16/00499170.c"
}

// EAX-hDlg entry for the hook and the Frida export. Not a body: it only forwards to SubsystemSelectionChanged_Body.
#pragma comment(linker, "/EXPORT:SubsystemSelectionChanged_i3=_SubsystemSelectionChanged_i3")
extern "C" __declspec(naked) void __cdecl SubsystemSelectionChanged_i3(void) {
    __asm {
        push eax
        call SubsystemSelectionChanged_Body
        add  esp, 4
        ret
    }
}
RH_ScopedInstall(SubsystemSelectionChanged_i3, 0x00499170);
