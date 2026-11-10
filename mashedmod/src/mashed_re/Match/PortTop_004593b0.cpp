// PORT-OT-PICKUPPOOL (L-111 (a) step 1): exe-only port body of rva 0x004593b0 = the matched source re/match/s09/004593b0.c
// (MATCH-S09 raw g2s): for the four 0x58-byte records from 0x0068ba30 to 0x0068bb90, RpClumpDestroy(rec[7]) and
// 0x004c5770(rec[0]), then 0x004768c0(&DAT_0068b9b8), then RwTexDictionaryDestroy(DAT_0068bb58) when non-zero.
// Its only caller in the exe is the matched 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40 on the
// MASHED_ORIG_TOP path), through the RWMOD selector PortTop_0045b930_Call004593b0: this body runs only with
// MASHED_ORIG_TOP_RWMOD=1, because 0x004c5770 calls RwGlobals +0x11c, null without it (RESULT_PORT-OT-RWG-MODULES.md).
// The records, 0x0068b9b8 and 0x0068bb58 are built only by 0x00459290 (re/match/s09/00459290.c), whose only caller is
// 0x0045bae0, a stand-in on this path. No exe code writes 0x0068b9b8..0x0068bb90, and the range is BSS in MASHED.exe,
// so at this call every word is zero.
// Callees bind by name, no raw-address call:
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp, null skipped).
//  - 0x004c5770 = Orig_004c5770 (Match/PortTop_004c5770.cpp, matched). With RWMOD the +0x11c slot returns 0 for 0.
//  - 0x004768c0 = PortTop_PKP_Call004768c0 (Match/PortTop_00458880.cpp): the matched Orig_004768c0 when the record's
//    +0x04 word is non-zero, else a logged stand-in (the matched body reads *(*(p + 4) + 4) with no null check).
//  - 0x004c5930 = MashedRw_TexDictionaryDestroy_004c5930 (LibRw/RwApiBind.cpp).
// No other body of this RVA exists in either target.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" {
int MashedRw_ClumpDestroy_004e6e00(int clump);
int MashedRw_TexDictionaryDestroy_004c5930(int dict);
unsigned int PortTop_004593b0_Call004e6e00(int p0) { return static_cast<unsigned int>(MashedRw_ClumpDestroy_004e6e00(p0)); }
unsigned int PortTop_004593b0_Call004c5930(int p0) { return static_cast<unsigned int>(MashedRw_TexDictionaryDestroy_004c5930(p0)); }
}  // extern "C"
#define FUN_004593b0 Orig_004593b0
#define FUN_004e6e00 PortTop_004593b0_Call004e6e00   // RW: RpClumpDestroy -> rw::Clump::destroy
#define FUN_004c5770 Orig_004c5770                   // matched, Match/PortTop_004c5770.cpp
#define FUN_004768c0 PortTop_PKP_Call004768c0        // record selector, Match/PortTop_00458880.cpp
#define FUN_004c5930 PortTop_004593b0_Call004c5930   // RW: RwTexDictionaryDestroy -> rw::TexDictionary::destroy
extern "C" {
#include "../../../../re/match/s09/004593b0.c"
}
#endif  // MASHED_STANDALONE
