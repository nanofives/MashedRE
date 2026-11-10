// PORT-OT-HUD-2 (L-111 (a) step 1): exe-only port body of rva 0x00555830 = the matched source re/match/s24/00555830.c
// (MATCH-S24 h2, 100%, regs 100%, byte-identical). For a non-zero param_1: each non-zero obj of the 0x20-byte entries
// FUN_005c4d30(param_1[+0x134]) returns (count param_1[+0x130]) goes to FUN_00553e80 and is zeroed, then
// FUN_005c4d50(param_1[+0x134]); +0x134 = 0; RwGlobals +0x10c (free) on +0x12c (300) when non-zero, +0x12c = 0; RwTextureDestroy
// 0x004c5a60 on each non-zero of the four words at +0x14, each zeroed; then RwGlobals +0x11c (freelist free)
// (DAT_00912a08, param_1), returns 1. param_1 == 0 returns 0 and calls nothing.
// Its only caller in the exe is the matched 0x00427620 (Match/PortTop_00427620.cpp, AppDestroy 0x00402a40 on the
// MASHED_ORIG_TOP path), and that caller runs this body only with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x11c is null
// without it, RESULT_PORT-OT-RWG-MODULES.md). Callees bind by name, no raw-address call:
//  - 0x005c4d30 = Orig_005c4d30 (Match/PortTop_005c4d30.cpp), 0x005c4d50 = Orig_005c4d50 (Match/PortTop_005c4d50.cpp).
//  - 0x004c5a60 = MashedRw_TextureDestroy_004c5a60 (LibRw/RwApiBind.cpp, rw::Texture::destroy).
//  - 0x00553e80: no matched source (C4 by hooks.csv, no re/match file) and no exe body, so a named stand-in below that
//    logs its first call and returns 0. It runs only for a non-zero entry obj, which needs a non-zero param_1.
//  - RwGlobals +0x10c / +0x11c through DAT_007d3ff8 (LibRw/RwApiBind.cpp MashedRwGlobals rwfree / flfree).
// No other body of this RVA exists in either target.
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
extern "C" {
int MashedRw_TextureDestroy_004c5a60(int tex);
unsigned int Orig_005c4d30(unsigned int* p0);                // matched, Match/PortTop_005c4d30.cpp
unsigned int Orig_005c4d50(int* p0);                         // matched, Match/PortTop_005c4d50.cpp
int PortTop_StandInOTH2_00553e80(int p0) {
    static int once;
    if (!once) {
        once = 1;
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] rva 0x00553e80(0x%08X): stand-in, not run (no matched source) (PORT-OT-HUD-2)\n",
                         static_cast<unsigned int>(p0));
            std::fclose(f);
        }
    }
    return 0;
}
}  // extern "C"
#define FUN_00555830 Orig_00555830
#define FUN_004c5a60 MashedRw_TextureDestroy_004c5a60   // RW: RwTextureDestroy -> rw::Texture::destroy
#define FUN_00553e80 PortTop_StandInOTH2_00553e80
#define FUN_005c4d30 PortTop_00555830_Call005c4d30
#define FUN_005c4d50 PortTop_00555830_Call005c4d50
extern "C" {
#include "../../../../re/match/s24/00555830.c"
// Defined after the source text so they can use its Entry20 type and match the prototypes it declares.
Entry20* PortTop_00555830_Call005c4d30(int p0) {
    return reinterpret_cast<Entry20*>(static_cast<unsigned long>(Orig_005c4d30(reinterpret_cast<unsigned int*>(
        static_cast<unsigned long>(static_cast<unsigned int>(p0))))));
}
int PortTop_00555830_Call005c4d50(int p0) {
    return static_cast<int>(Orig_005c4d50(reinterpret_cast<int*>(static_cast<unsigned long>(static_cast<unsigned int>(p0)))));
}
}  // extern "C"
#endif  // MASHED_STANDALONE
