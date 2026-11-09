// PORT-TOP-04F (perm family): exe-only port body of rva 0x00425bc0 = the matched source re/match/s05/00425bc0.c
// (match.py BYTE-IDENTICAL 46 B, relocheck OK 5/5). Called by the matched 0x00402750 with perm.piz mounted: it reads
// "ToastArt/common/perm/Copters/Copters.txd" through 0x004b3d80 into DAT_00644150, sets the current texture dictionary
// to 0 (RwTexDictionarySetCurrent binding) and zeroes the 0x98 dwords at DAT_00899260.
//
// The source declares 0x004b3d80's first parameter undefined4 and passes a string literal, which C accepts and C++ does
// not. The source text stays as matched: a C++ overload taking const char* is declared first, so the call resolves to
// it, and it forwards the pointer to the one C-linkage body (Match/PortTop_004b3d80.cpp).
#ifdef MASHED_STANDALONE
#include <cstdint>
extern "C" unsigned int Orig_004b3d80(unsigned int p0, unsigned int p1, unsigned int p2);
static inline unsigned int Orig_004b3d80(const char* p0, int p1, int p2) {
    return Orig_004b3d80(static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(p0)), static_cast<unsigned int>(p1),
                         static_cast<unsigned int>(p2));
}
#define MATCH_PORT 1
#define FUN_00425bc0 Orig_00425bc0
#define FUN_004b3d80 Orig_004b3d80
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
extern "C" {
#include "../../../../re/match/s05/00425bc0.c"
}
#endif  // MASHED_STANDALONE
