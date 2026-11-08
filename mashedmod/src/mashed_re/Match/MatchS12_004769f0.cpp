// PORT-RENDER-01: port body of 0x004769f0 = the byte-matched source re/match/s12/004769f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py --bytes BYTE-IDENTICAL 30 B,
// relocheck OK). The same source text is this RVA's only body in both targets (moved out of Render/RfxEmitter.cpp,
// where it was the hand port ParticleEmitter_SetColour). x87 TU (mashedmod/x87_tus.txt). Compiled as C++ with MATCH_PORT.
//
// Per-target binding (precedent Match/MatchS06_004282a0.cpp). The body reads the null-pointer default DAT_00613290 and
// writes DAT_00692554:
//   source name     .asi            exe
//   DAT_00692554    [0x00692554]    [0x00692554] (the exe's image-pad owns 0x006924d8 .. 0x006925a8, exe_main.cpp B17)
//   DAT_00613290    [0x00613290]    [0x00613290], filled by the port-state seed (PORT-STATE-02; before it, the exe bound
//                                   a private copy of the file-backed 0xffffffff, g_rfxDefault00613290)
// That is exactly what the replaced hand port read in each target (`param_1 ? param_1[0] : *k00613290`).
// The body has no callees.
#include "../Core/HookSystem.h"

#include <cstdint>

#define MATCH_PORT 1
#define FUN_004769f0 ParticleEmitter_SetColour
#pragma comment(linker, "/EXPORT:ParticleEmitter_SetColour=_ParticleEmitter_SetColour")

// PORT-STATE-02 (L-101): DAT_00613290 binds to its original address in both targets; in the exe the port-state seed
// (Compat/PortStateSeed.cpp, default ON) puts the file-backed 0xffffffff there. Before, the exe bound
// g_rfxDefault00613290 (Render/RfxEmitter.cpp, removed).
#define MATCH_CELL_DAT_00613290 (0x00613290u)
#define MATCH_CELL_DAT_00692554 (0x00692554u)

// This TU's preamble: the source's own preamble is skipped (MATCH_PREAMBLE), and MGLOBAL binds through the cells above.
// The source uses no other preamble name (only unsigned int).
#define MATCH_PREAMBLE 1
#define MGLOBAL(T, n, a) static T &n = *(T *)(MATCH_CELL_##n)
extern "C" {
#include "../../../../re/match/s12/004769f0.c"
}
RH_ScopedInstall(ParticleEmitter_SetColour, 0x004769f0);  // installs in the .asi, no-op in the exe
