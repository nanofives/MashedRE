// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004039c0 = the matched source re/match/s01/raw/004039c0.c
// (raw g2s text, match.py 100%, 23 B BYTE-IDENTICAL, relocheck OK): DAT_00636c00 = FUN_0042a5d0("Bomb.dff", 0, 0).
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0, whose last call is the tail jump to it
// (Match/PortTop_0045bae0.cpp).
//  - 0x0042a5d0 = FUN_0042a5d0, the matched exe body (Match/MatchS06_0042a5d0.cpp: the path search 0x0042a530, then
//    0x004b3bf0 -> Orig_004b3bf0 in the exe since POWERUPINIT-A). It takes one argument. The source pushes three, and
//    cdecl leaves the extra two unread, so the function-like name below drops them.
//  - DAT_00636c00 is the original address. The source's `extern` declaration is C-only (#ifndef MATCH_PORT), and the
//    preamble is C-only except undefined4 (#ifndef __cplusplus), so the DDK text match.py scores is unchanged.
//    Exe readers of 0x00636c00: none (Render/PerModeRender.cpp names it only as the end of a table range).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004039c0 Orig_004039c0
#define FUN_0042a5d0(name, ...) FUN_0042a5d0(name)
#define DAT_00636c00 (*(undefined4*)0x00636c00u)
extern "C" {
#include "../../../../re/match/s01/raw/004039c0.c"
}
#endif  // MASHED_STANDALONE
