// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only port body of
// rva 0x00552c10 = the matched source re/match/s24/00552c10.c (100%, 254 B BYTE-IDENTICAL, relocheck OK).
// Font render state: the matrix stack at 0x00912a84 (depth 0x00912b04 = 0, [0] = RwMatrixCreate(), [1..31] = 0), 0x00552750
// on [0], the 0x00912a44 identity matrix with flags | 0x20003, the 0x00912500 ramp 0..255 and the scalars 0x00912bd8..0x0091320c.
// Returns 1.
// Reached only from the Rt2d open 0x00552b60 (Match/PortTop_00552b60.cpp), which runs only on the MASHED_ORIG_TOP path
// with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x118 freelist alloc and the +0xf4 string slot are bound only then).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00552c10 Orig_00552c10
#define FUN_004c57a0 MashedRw_MatrixCreate_004c57a0   // RW: RwMatrixCreate -> rw::Matrix::create (LibRw/RwBridge.cpp, PORT-TOP-01)
#define FUN_00552750 Orig_00552750   // matched, Match/PortTop_00552750.cpp
extern "C" {
#include "../../../../re/match/s24/00552c10.c"
}
#endif  // MASHED_STANDALONE
