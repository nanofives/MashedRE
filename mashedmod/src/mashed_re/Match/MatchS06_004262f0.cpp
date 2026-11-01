// MATCH-S06: port body of 0x004262f0 = the byte-matched source re/match/s06/004262f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// 0x004262f0 had no port body: this is its first body (new body, top-down rule L-72). No install; caller 0x00426e10.
// 0x00425b70 / 0x00425c00 bind to their port bodies (Frontend/BatchAA_s4.cpp, Frontend/MenuLeaves_af4.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_00425b70 SlotFieldSetter
#define FUN_00425bf0 MatchS06_004262f0_Call00425bf0
#define FUN_00425c00 CopterClumpRegister
#define FUN_004952f0 MatchS06_004262f0_Call004952f0
#pragma comment(linker, "/EXPORT:FUN_004262f0=_FUN_004262f0")
extern "C" {
#include "../../../../re/match/s06/004262f0.c"
}

// Callees with no port body: each forwarder makes the call at the original address (S04 rule).
extern "C" {
void MatchS06_004262f0_Call00425bf0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00425bf0u)();
}
void MatchS06_004262f0_Call004952f0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004952f0u)();
}
}
