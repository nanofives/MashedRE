// IMG-5-TEARDOWN-BODIES (L-153 (a), run 46): exe-only PROVISIONAL port body of rva 0x00455400 = re/match/s29/00455400.c.
// NOT a byte match: match.py 94.0% (131/133 ins, XP DDK /O2 /GS /Ob1); equiv.py NOT-PROVEN call-memory at the call 0x4e6ab0
// (0x00455581 region). Listed in re/match/PROVISIONAL.tsv under L-126: knob path only, never C4 by this status (L-128).
// Residual (verify/portorigtop_20261109/RESULT_POWERUPINIT-B2.md section 2): the two induction-variable inits / decrements
// are scheduled in the other order, and the original reloads the counter and recomputes i*0x6c (imul at 0x00455585) for
// the stores to missile +0x48 / +0x58 / +0x20 where this source reuses the induction register. Store targets and values
// are the same addresses per pass (0x00688400 / 0x00688410 / 0x006883d8 + i*0x6c, value 0).
// Body: the type-table init of entry 2 (+0x14 at 0x005f9a2c). Five launcher records (0x006885d4 + i*0x2c) and five missile
// records (0x006883b8 + i*0x6c), i = 4..0: launcher clone of "MissileLauncher.dff" at +0x14, part-code-1 atom at +0x1c;
// missile clone of "missile.dff" at +0x54, 0x00489940 particle object at +0x60 (stored at 0x00455517 in the original),
// part-code-1 atom at +0x5c scaled {3, 3, 9} (0x004c13e0). Then both source clumps are destroyed.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 2 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp). Its records are read by the matched teardown 0x004555d0
// (verify/portstate_20261114/RESULT_IMG-5-TEARDOWN-FAULT.md).
// Callees bind by name, no raw-address call:
//   0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp), 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp),
//   0x004c5c80 = MashedRw_TexDictionarySetCurrent_004c5c80, 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0,
//   0x004b5190 = MashedRw_AtomicPartCode_004b5190, 0x004c13e0 = MashedRw_FrameScale_004c13e0,
//   0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp, existing RW_BIND entries, no new librw binding),
//   0x004b3fc0 = Orig_004b3fc0 (Match/PortTop_004b3fc0.cpp), 0x00489940 = Orig_00489940 (Match/PortTop_00489940.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00455400 Orig_00455400
#define FUN_0040bb30 Search40bb30
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_004b3fc0 Orig_004b3fc0
#define FUN_004b5190 MashedRw_AtomicPartCode_004b5190
#define FUN_00489940 Orig_00489940
#define FUN_004c13e0 MashedRw_FrameScale_004c13e0
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s29/00455400.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
