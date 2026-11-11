// PARTICLE-3-SWAP: exe-only port body of rva 0x0048cf70 = the matched source re/match/s15/0048cf70.c (BYTE-IDENTICAL,
// 28 B): calls 0x004768c0 on each of the 17 0x40-byte records 0x00722470..0x007228b0 (teardown of what 0x0048cee0
// built). Caller on the MASHED_ORIG_TOP path: the matched 0x0040bd00 (AppDestroy chain, Match/PortTop_0040bd00.cpp),
// through PortTop_PS3_Call0048cf70 below, which logs once.
// Records: built only by 0x0048cee0 (Match/PortTop_0048cee0.cpp, called from the 0x0048e820 near-match body that
// 0x0040bbb0 runs at init); this TU only reads them. 0x004768c0 reads record +0x4 (the object 0x004770c0 created) and
// frees +0x10..+0x28 through RwGlobals +0x10c when non-zero.
// Callee binds by name: 0x004768c0 = Orig_004768c0 (Match/PortTop_004768c0.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdint>
#define MATCH_PORT 1
#define FUN_0048cf70 Orig_0048cf70
#define FUN_004768c0 Orig_004768c0
extern "C" {
#include "../../../../re/match/s15/0048cf70.c"
}  // extern "C"

extern "C" void PortTop_PS3_Call0048cf70(void) {
    const std::uint32_t* rec = reinterpret_cast<const std::uint32_t*>(0x00722470u);
    int nz = 0;
    for (int i = 0; i < 17; ++i) nz += rec[i * 16 + 1] != 0u;
    Orig_0048cf70();
    if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
        std::fprintf(fp, "[ORIG_TOP] rva 0x0048cf70: matched body ran, 0x004768c0 on 17 records (%d with +0x4 non-zero "
                         "before the call) (PARTICLE-3-SWAP)\n", nz);
        std::fclose(fp);
    }
}
#endif  // MASHED_STANDALONE
