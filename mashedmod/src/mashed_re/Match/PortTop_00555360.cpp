// NEARMATCH-555360 (L-111 step 1, L-115 / L-117 = (a), 2026-11-10): exe-only NEAR-MATCH body of rva 0x00555360 = the
// unedited source re/match/s24/00555360.c (94.4%, 89/90 instructions; not byte-identical). L-117 admits this function
// under L-115 by name: every one of its 55 differing bytes is accounted for in
// verify/portorigtop_20261109/RESULT_L115-HUDINIT-PROOFS.md (50 by the return-block layout and the shift it causes, 5 by
// the order of two stores to non-overlapping addresses). It counts on the port axis only, never C4.
// Two freelists (0x144-byte entries, header 0x007dc7a0 -> DAT_00912a08; 0xc-byte, header 0x007dc7c4 -> DAT_00912a14), an
// 0x51-byte RwGlobals +0x108 path buffer DAT_00912a0c grown with +0x110 to strlen (+0xf4) of the string at 0x00623ea4 plus 1
// and filled with +0xcc, the 64-quad index table at 0x00912700, DAT_00912a18 = 0, DAT_00912a1c = DAT_00912a20 =
// RwTexDictionaryCreate(), DAT_0091325c = DAT_00913260 = 0, DAT_00913100 = 0x00555910. Returns 1, or 0 when a freelist or the
// buffer is 0.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path with
// MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 and the +0xf4 string slot are bound only then).
// Callees bind by name, no raw-address call:
//  - 0x004cc9e0 = Orig_004cc9e0 (matched thunk, Match/PortTop_004cc9e0.cpp, onto the 0x004cc820 binding)
//  - 0x004c5890 = RwCoreBind_TexDictionaryCreate_004c5890 (RwCore/RwCore_Bind_004c5890.cpp, RWC-BIND-4C5890, L-129 / L-130).
//    With MASHED_RWC_CORE off it returns 0, so 0x00912a1c / 0x00912a20 are stored as 0. This body stores the result and
//    tests it nowhere (re/match/s24/00555360.c, last block), so a 0 result changes no return value of this function.
//  - 0x00555910 is stored at 0x00913100, not called here: the named stand-in below (the font-file reader, chain C). Its
//    source re/match/s24/00555910.c is 100%, but its callees are the font-read chain.
#ifdef MASHED_STANDALONE
#include <cstdio>
extern "C" int PortTop_StandInNM_00555910() {
    static int once;
    if (!once) {
        once = 1;
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x00555910: stand-in, not run (font-file reader, chain C) (NEARMATCH-555360)\n");
            std::fclose(f);
        }
    }
    return 0;
}
#define MATCH_PORT 1
#define FUN_00555360 Orig_00555360
// FUN_004cc9e0: the matched thunk, Match/PortTop_004cc9e0.cpp
#define FUN_004cc9e0 Orig_004cc9e0
// FUN_004c5890: the RW core binding entry (RwCore/RwCore_Bind_004c5890.cpp), no librw binding (L-129 a, L-124)
#define FUN_004c5890 RwCoreBind_TexDictionaryCreate_004c5890
// FUN_00555910: stored at 0x00913100 only, the stand-in above
#define FUN_00555910 PortTop_StandInNM_00555910
extern "C" {
#include "../../../../re/match/s24/00555360.c"
}
#endif  // MASHED_STANDALONE
