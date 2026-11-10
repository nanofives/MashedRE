// PORT-OT-OTHER (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x00485e10 = the matched source
// re/match/s14/00485e10.c (BYTE-IDENTICAL, relocheck OK): for the four records at 0x006fde10 (stride 0x23c, pool pointer at
// +4), 0x004768c0(pool + 0x10) and RwGlobals +0x10c (librw free) on *pool (m_items). The matched 0x0040bd00
// (Match/PortTop_0040bd00.cpp) calls it on the MASHED_ORIG_TOP path (AppDestroy 0x00402a40). The pools are the ones the
// matched 0x00485d90 (Match/PortTop_00485d90.cpp) takes from 0x0040bbb0 on the same path. 0x004768c0 has no null check on
// record +4, so the two are swapped together. OT_MEASURE.tsv: RwGlobals +0x10c OK.
// Source edit: the code typedef is now int code(int), and DAT_006fde14 an int, under __cplusplus only (the 00483a40.c
// pattern), C text unchanged (match.py --bytes BYTE-IDENTICAL).
// Callee binds by name, no raw-address call: 0x004768c0 -> Orig_004768c0 (Match/PortTop_004768c0.cpp). The RwGlobals slot
// is read through DAT_007d3ff8, the exe's fake RW device table.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00485e10 Orig_00485e10
#define FUN_004768c0 Orig_004768c0                              // matched, Match/PortTop_004768c0.cpp
extern "C" {
#include "../../../../re/match/s14/00485e10.c"
}
#endif  // MASHED_STANDALONE
