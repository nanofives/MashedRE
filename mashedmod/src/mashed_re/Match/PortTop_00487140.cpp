// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x00487140 = the matched source
// re/match/s14/00487140.c (BYTE-IDENTICAL): 0x004768c0(&DAT_00703068). The matched 0x0040bd00 (Match/PortTop_0040bd00.cpp)
// calls it on the MASHED_ORIG_TOP path (AppDestroy 0x00402a40). The record is the one the matched 0x00487e00
// (Match/PortTop_00487e00.cpp) builds from 0x0040bbb0 on the same path. 0x004768c0 has no null check on record +4, so the
// two are swapped together.
// Callee binds by name, no raw-address call: 0x004768c0 -> Orig_004768c0 (Match/PortTop_004768c0.cpp). The source declares
// it with a pointer parameter under __cplusplus (same stack bytes).
// The other body of this RVA is the .asi-only Match/MatchS14_00487140.cpp (raw-address callees), so this TU is the ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00487140 Orig_00487140
#define FUN_004768c0 Orig_004768c0                              // matched, Match/PortTop_004768c0.cpp
extern "C" {
#include "../../../../re/match/s14/00487140.c"
}
#endif  // MASHED_STANDALONE
