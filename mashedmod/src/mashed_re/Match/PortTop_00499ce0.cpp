// PORT-TOP-04E: exe-only port body of rva 0x00499ce0 = the matched source re/match/s16/00499ce0.c (identity matrix at
// 0x00773928; match.py 100%, relocheck OK). Called by the matched 0x00402750 (Match/PortTop_00402750.cpp) on the
// MASHED_ORIG_TOP path. No other exe code reads 0x00773928..0x00773967.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00499ce0 Orig_00499ce0
extern "C" {
#include "../../../../re/match/s16/00499ce0.c"
}
#endif  // MASHED_STANDALONE
