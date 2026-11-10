// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x004c1840 (RwBBoxAddPoint: widens inf / sup by *vertex) = the matched source
// re/match/s19/004c1840.c (match.py BYTE-IDENTICAL). Caller on the MASHED_ORIG_TOP path: 0x00401000 (Match/PortTop_00401000.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004c1840 Orig_004c1840
extern "C" {
#include "../../../../re/match/s19/004c1840.c"
}
#endif  // MASHED_STANDALONE
