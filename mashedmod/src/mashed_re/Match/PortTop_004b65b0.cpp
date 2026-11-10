// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004b65b0 = the matched source re/match/s18/004b65b0.c
// (return FUN_004b68f0(name)). Callers on the MASHED_ORIG_TOP path: the matched builders 0x004587a0 ("IconCube.dff")
// and 0x00411f30 ("lazer.dff"), both from 0x0045bae0, which pass the result to 0x004b3b70 as the read size.
// 0x004b68f0 (re/match/s18/004b68f0.c) returns the +0x78 word of the first entry with that name in the directory the
// original's piz reader copies to 0x0090dac0 (count 0x008ad9a8, 0x004b6a4a..0x004b6ab0), and that word is the entry
// length (Piz/PizReader.h: entry +0x78 length). The exe's piz reader keeps its directories in Piz::Archive and never
// fills 0x0090dac0, so 0x004b68f0 binds to the piz file system: PortTop_PizFs_EntryLength_004b68f0
// (Match/PortTop_00402750_Pieces.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b65b0 Orig_004b65b0
#define FUN_004b68f0 PortTop_PizFs_EntryLength_004b68f0
extern "C" {
#include "../../../../re/match/s18/004b65b0.c"
}
#endif  // MASHED_STANDALONE
