// MATCH-551A50-INCLUDE (2026-11-15): exe-only port body of rva 0x00551a50 = re/match/rwl04/00551a50.c
// (match.py --bytes BYTE-IDENTICAL 121 B with the source's own // TOOLCHAIN rw9044 and // FLAGS /O2 /Ob1 /G5, relocheck OK
// checked=2 unchecked=0). Generated as a RW-band callee of 0x00492e90 (RWC-FRAME-492E90, PROVISIONAL.tsv row until this
// include). Callees bind by name, no raw-address call:
//  - 0x004c5770 = Orig_004c5770 (Match/PortTop_004c5770.cpp, matched).
//  - 0x004c57a0 = MashedRw_MatrixCreate_004c57a0 (LibRw/RwBridge.cpp, existing binding, the same one
//    Match/PortTop_00459290.cpp uses; no new librw binding added).
// No caller in the exe binds to Orig_00551a50 yet: this row only makes the matched source part of the exe build.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" unsigned int __cdecl MashedRw_MatrixCreate_004c57a0(void);
#define FUN_00551a50 Orig_00551a50
#define FUN_004c5770 Orig_004c5770                   // matched, Match/PortTop_004c5770.cpp
#define FUN_004c57a0 MashedRw_MatrixCreate_004c57a0  // RW: RwMatrixCreate shape -> rw::Matrix::create
extern "C" {
#include "../../../../re/match/rwl04/00551a50.c"
}
#endif  // MASHED_STANDALONE
