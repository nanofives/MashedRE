// PORT-TOP-04H (family 3, perm clumps): exe-only port body of rva 0x00401630 = the matched source
// re/match/s01/raw/00401630.c (match.py BYTE-IDENTICAL 87 B, relocheck OK): sprintf(buf, "map_%d.dff", id), then
// 0x004b3bf0(buf, 0, 0), whose result it returns in eax (the source now says `return`: same bytes, PORT-TOP-04H).
// Caller on the MASHED_ORIG_TOP path: 0x004025f0 (Match/PortTop_004025f0.cpp). Callees: sprintf 0x004a2b60 -> the CRT
// sprintf, 0x004b3bf0 -> the matched body Match/PortTop_004b3bf0.cpp.
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdio>
#define MATCH_PORT 1
#define FUN_00401630 Orig_00401630
#define FUN_004a2b60 PortTop_Sprintf04H_004a2b60
#define FUN_004b3bf0 Orig_004b3bf0
extern "C" {
// rva 0x004a2b60 (CRT sprintf: a string FILE with _cnt 0x7fffffff, _output, then the NUL): the exe CRT's vsprintf.
int PortTop_Sprintf04H_004a2b60(char* buf, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsprintf(buf, fmt, ap);
    va_end(ap);
    return n;
}
#include "../../../../re/match/s01/raw/00401630.c"
}
#endif  // MASHED_STANDALONE
