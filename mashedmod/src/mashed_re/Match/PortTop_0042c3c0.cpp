// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-08 (L-96 top-down lane): exe-only port body of rva 0x0042c3c0 = the matched source re/match/s07/0042c3c0.c
// (match.py 100%), called by 0x0042c960 (Match/PortTop_0042c960.cpp). Its CRT callee sprintf 0x004a2b60 is an adapter
// to the VS2022 CRT, as in Match/PortTop_00498860.cpp. The .rdata it reads (DAT_005cc950, DAT_005cc320, DAT_005cd788,
// s__2f_005cd78c) is in the port-state seed (Compat/PortStateSeed.inc).
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdio>
#include "../Core/HookSystem.h"

extern "C" {
// CRT sprintf 0x004a2b60.
int PortTop_Crt08_004a2b60(char* out, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsprintf(out, fmt, ap);
    va_end(ap);
    return n;
}
}

#define MATCH_PORT 1
#define FUN_0042c3c0 Orig_0042c3c0
#define FUN_00427780 PortTop08_Fwd00427780   // matched exe body FontText_StringTableLookup (Match/MatchS06_00427780.cpp)
#define FUN_004a2b60 PortTop_Crt08_004a2b60  // CRT sprintf adapter (above)
extern "C" const unsigned char* __cdecl FontText_StringTableLookup(int index);
extern "C" short* PortTop08_Fwd00427780(int p0) {
    return reinterpret_cast<short*>(const_cast<unsigned char*>(FontText_StringTableLookup(p0)));
}
extern "C" {
#include "../../../../re/match/s07/0042c3c0.c"
}
#endif // MASHED_STANDALONE
