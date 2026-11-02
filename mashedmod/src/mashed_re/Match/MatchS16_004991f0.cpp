// MATCH-S16: port body of 0x004991f0 = the byte-matched source re/match/s16/004991f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Save/SettingsDialog.cpp; .asi only). Compiled here as C++ with MATCH_PORT; globals bind to their original
// addresses (preamble MGLOBAL). The file's nine other video-dialog functions are HELD (not compiled here).
// __stdcall DLGPROC (ret 0x10), installed as is. Callees (the old body called the original RVAs, which the .asi
// hooks to these same bodies): 0x00498f60 = VideoDialogInit_Body, 0x00499170 = SubsystemSelectionChanged_Body,
// 0x00498d20 = ReadModeFromCombo_Body (Match/MatchS16_<va8>.cpp, C linkage, cdecl stack args).
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
#define FUN_004991f0 VideoSettingsDlgProc_s2
#define FUN_00498f60 VideoDialogInit_Body
#define FUN_00499170 SubsystemSelectionChanged_Body
#define FUN_00498d20 ReadModeFromCombo_Body
#pragma comment(linker, "/EXPORT:VideoSettingsDlgProc_s2=_VideoSettingsDlgProc_s2@16")
extern "C" {
#include "../../../../re/match/s16/004991f0.c"
}
RH_ScopedInstall(VideoSettingsDlgProc_s2, 0x004991f0);
