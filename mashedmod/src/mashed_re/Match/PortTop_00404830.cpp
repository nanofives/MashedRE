// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x00404830 = the matched source re/match/s01/00404830.c
// (MATCH-S01 a3, 100%): the two car material tables DAT_00639cc8 (int[40]) and DAT_00636c08 (int[40][13][6]) at their
// original addresses. The matched AppInitialiseOnBootup 0x00402750 (Match/PortTop_00402750.cpp) calls it on the
// MASHED_ORIG_TOP path. The hand body of this RVA (Vehicle/CarMaterialTables.cpp + CarMaterialTables_dirt.inc) stays
// on the default path until the ORIG_TOP flip; the exe's readers use that TU's own storage (Tables()), not these
// addresses, so this copy changes no value the default path reads.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00404830 Orig_00404830
#define DAT_00639cc8 (*(int(*)[40])0x00639cc8u)
#define DAT_00636c08 (*(int(*)[40][13][6])0x00636c08u)
extern "C" {
#include "../../../../re/match/s01/00404830.c"
}
#endif  // MASHED_STANDALONE
