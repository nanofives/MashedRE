// Mashed RE - x87 sine-of-global-product leaves 0x00431b20 / 0x00431b50 / 0x00431b60.
//
// MATCH-S07: all three bodies moved to Match/MatchS07_00431b20.cpp, MatchS07_00431b50.cpp and MatchS07_00431b60.cpp,
// which compile the byte-matched sources re/match/s07/00431b20.c / 00431b50.c / 00431b60.c (exports
// SinGlobalProduct431b20 / 431b50 / 431b60 kept). Original shape, e.g. 0x00431b50: FLD dword [0x007f0f04] /
// FMUL qword [0x005cd8f0] / FSIN / RET, float10 in ST0. The wrappers keep the bare FSIN (MSVC's CRT sin is not
// FSIN) through a small inline-asm helper. Operand semantics unknown (U-1618).
#include "../Core/HookSystem.h"
