// MATCH-S02 / MATCH-L68-SWAP: port body of 0x00411350 = the byte-matched source re/match/s02/00411350.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (replaces
// Save/ReplayTimeFormat.cpp, whose body computed (double)param_1 * k; the original computes the product as a
// float local, (float)param_1 * k, and the two differ on 1,787,798 of 1,800,000 inputs, MATCH-S02 RESULT).
// Compiled here as C++ with MATCH_PORT, x87 (mashedmod/x87_tus.txt).
//
// Carried over from the retired TU (raw-listing plate re/analysis/replay_record/0x00411350_RAW.md):
// there is no x87 INPUT argument (Ghidra's extraout_ST0 is the FLD ST0 copy left over the __ftol call);
// param_1 is unsigned (TEST/JGE/FADD 2^32 from the float at 0x005cc94c); the scale is a multiply by the float
// at 0x005cc948 (0x39aec33e), a hair below 1/3000, so 180000 ticks give secs 59 mins 0; the `> 59` compare is
// signed and *param_3 is re-read every iteration.
// Callers: the original's 0x00411530 (ReplayGetTimeAtIdx calls 0x00411350 by address). No exe callers:
// match evidence identical + swapped, C4 only when a caller lands (L-68 ruling, 2026-10-31).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00411350 ReplayTimeFormat
#pragma comment(linker, "/EXPORT:ReplayTimeFormat=_ReplayTimeFormat")
extern "C" {
#include "../../../../re/match/s02/00411350.c"
}
RH_ScopedInstall(ReplayTimeFormat, 0x00411350);
