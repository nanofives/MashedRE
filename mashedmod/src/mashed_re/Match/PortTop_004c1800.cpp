// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x004c1800 (RwBBoxInitialize: sup = inf = *vertex) = the matched source
// re/match/s19/004c1800.c (match.py BYTE-IDENTICAL). Caller on the MASHED_ORIG_TOP path: 0x00401000 (Match/PortTop_00401000.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004c1800 Orig_004c1800
extern "C" {
#include "../../../../re/match/s19/004c1800.c"
}
#endif  // MASHED_STANDALONE
