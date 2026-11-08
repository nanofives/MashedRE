// PORT-STATE-02 (2026-11-08, L-101): port body of 0x00498910 / 0x00498950 / 0x004989b0 = the byte-matched source
// re/match/s16/00498950.c (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%,
// MATCH-S16). The same source text is these RVAs' only body in both targets (the hand bodies in Save/SettingsConfig.cpp
// are gone). Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBALA). The
// callees bind to the per-target hosts in Save/SettingsConfig.cpp (MatchVcfg*), which also keeps the exe's ConfigLoadCore /
// ConfigSaveCore entry points. The .asi installs are here.
// In the exe the .rdata strings come from the port-state seed (Compat/PortStateSeed.cpp, default ON).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00498910 ConfigFilenameGet
#define FUN_00498950 ConfigLoad
#define FUN_004989b0 ConfigSave
#define FUN_004963e0 MatchVcfgLogError
#define FUN_00496400 MatchVcfgLogDebug
#define FUN_004a4541 MatchVcfgFopen
#define FUN_004a49cf MatchVcfgFread
#define FUN_004a4b22 MatchVcfgFwrite
#define FUN_004a4368 MatchVcfgFclose
// The original calls the filename getter (0x00498951 / 0x004989b1); keep it a call, not an inline copy.
#pragma auto_inline(off)
extern "C" {
#include "../../../../re/match/s16/00498950.c"
}
#pragma auto_inline(on)
#pragma comment(linker, "/EXPORT:ConfigFilenameGet=_ConfigFilenameGet")
#pragma comment(linker, "/EXPORT:ConfigLoad=_ConfigLoad")
#pragma comment(linker, "/EXPORT:ConfigSave=_ConfigSave")
RH_ScopedInstall(ConfigFilenameGet, 0x00498910);
RH_ScopedInstall(ConfigLoad, 0x00498950);
RH_ScopedInstall(ConfigSave, 0x004989b0);
