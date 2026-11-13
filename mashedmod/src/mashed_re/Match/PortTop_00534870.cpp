// RO-EXE-REPLAY-KNOB (replay oracle 7): exe-only port body of rva 0x00534870 = the byte-matched source
// re/match/rwl03/00534870.c (h3 100%, regs 100%). RenderWare's ring RNG: struct {base, rp, wp, end} at
// *(0x007d3ff8) + *(0x007dc578); *rp += *wp, returns *rp >> 1, both cursors advance and wrap at end.
// In the original the struct lives in the RW RNG plugin block of RwGlobals, which the exe's RwGlobals record
// (LibRw/RwApiBind.cpp) does not carry. The replay dispatcher PortTop_Replay_00534870 (Match/PortTop_Replay.cpp) gives
// it a ring of its own: for the duration of one call it sets the plugin offset 0x007dc578 so that the sum names the
// dispatcher's private struct, then restores the word. The body runs unchanged (the DDK object in the object build).
// No exe caller binds 0x00534870 yet (game-side wrappers 0x00472650 / 0x00472690 / 0x004b44f0 / 0x004b4510 have no
// exe body on the ORIG_TOP path), so the dispatcher is called by the replay pins and self-test only.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00534870 PortTop_Bound_00534870
extern "C" {
#include "../../../../re/match/rwl03/00534870.c"
}
#endif // MASHED_STANDALONE
