// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004587a0 = the matched source re/match/s09/004587a0.c
// (MATCH-S09, 100%): the record DAT_0068b968 (0x004770c0 with 0x817, 0x19 and the "puglow" sprite), DAT_0068b9ac =
// 0x004b3d80("PowerUpIcons.txd"), 0x004c5c80(0), then "IconCube.dff" read into a zeroed 0x800-byte buffer
// (0x004b65a0, then 0x004b3b70 with the size 0x004b65b0 returns) and 25 clumps parsed from it (0x004b3e40) into the
// 0x50-byte pool entries at 0x0068b198 (+0x00 clump, +0x08 = 1.0, +0x0c = 0, +0x10 = 1.0, +0x14 = 0).
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0, through PortTop_PWI_Call004587a0
// (Match/PortTop_0045bae0.cpp), which then hands the pool's ownership rule to the hand pickup code
// (Gameplay/PickupTick.cpp PickupsOrig_SetPoolBuilt). The record 0x0068b968 is the one the matched teardown 0x00458880
// passes to 0x004768c0, and the 25 clumps and DAT_0068b9ac are what it destroys (Match/PortTop_00458880.cpp).
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30, 0x004770c0 = Orig_004770c0, 0x004b3d80 = Orig_004b3d80, 0x004c5c80 =
//    MashedRw_TexDictionarySetCurrent_004c5c80 (as in Match/PortTop_00459290.cpp).
//  - 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp).
//  - 0x004b65a0 = Orig_004b65a0, 0x004b65b0 = Orig_004b65b0, 0x004b3b70 = Orig_004b3b70, 0x004b3e40 = Orig_004b3e40
//    (this row's Match/PortTop_<rva>.cpp TUs).
// IconCube.dff is 0x434 bytes in Powerups.piz (piz_extract.py list), inside the 0x800-byte buffer.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004587a0 Orig_004587a0
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_004b3b70 Orig_004b3b70
#define FUN_004b3d80 Orig_004b3d80
#define FUN_004b3e40 Orig_004b3e40
#define FUN_004b6520 Orig_004b6520
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
#define FUN_004b65a0 Orig_004b65a0
#define FUN_004b65b0 Orig_004b65b0
extern "C" {
#include "../../../../re/match/s09/004587a0.c"
}
#endif  // MASHED_STANDALONE
