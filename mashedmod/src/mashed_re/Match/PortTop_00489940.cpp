// IMG-5-TEARDOWN-BODIES (L-153 (a), run 46): exe-only port body of rva 0x00489940 = the matched source re/match/s14/00489940.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, re/tools/match/match.py --bytes BYTE-IDENTICAL 127 B, relocheck OK 6/0, C4 by route
// match since 2026-11-07). Body: a 0x2c-byte object from 0x00474db0(1, 0x2c), three arrays 0x00474db0(n, 0xc / 0xc / 4) at
// +0x0 / +0x4 / +0x8, +0xc = +0x10 = n, +0x28 = 0x004892c0(tex, n, p4, p3), +0x14 = *colour, +0x18 / +0x1c = the two
// float bit patterns, +0x24 = 0, then 0x004893a0(+0x28, 5, 6). Returns the object.
// Caller on the MASHED_ORIG_TOP path: the provisional body of 0x00455400 (Match/PortTop_00455400.cpp, type-table entry 2),
// which stores the return at 0x006883b8 + i*0x6c + 0x60 (0x00455517 in the original). The matched teardown 0x004555d0
// passes that word to 0x00489c60.
// Callees bind by name, no raw-address call:
//   0x00474db0 = Orig_00474db0 (Match/PortTop_00474db0.cpp), 0x004892c0 = Orig_004892c0 (Match/PortTop_004892c0.cpp),
//   0x004893a0 = Orig_004893a0 (Match/PortTop_004893a0.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00489940 Orig_00489940
#define FUN_00474db0 Orig_00474db0
#define FUN_004892c0 Orig_004892c0
#define FUN_004893a0 Orig_004893a0
extern "C" {
#include "../../../../re/match/s14/00489940.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
