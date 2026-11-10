// PORT-OT-HUD-2 (L-111 (a) step 1): exe-only port body of rva 0x00556e40 = the matched source re/match/s24/00556e40.c
// (MATCH-S24 h3, 100%, regs 100%): if (--*(param_1 + 0x74) <= 0) { RwTextureDestroy 0x004c5a60 on +0x68 when non-zero;
// RwGlobals +0x11c (freelist free)(DAT_00912a00, param_1); return 1; } return 0. There is no null check on param_1: the
// first read is *(param_1 + 0x74).
// Its only caller in the exe is the matched 0x00427620 (Match/PortTop_00427620.cpp, AppDestroy 0x00402a40 on the
// MASHED_ORIG_TOP path), four times, and that caller runs this body only with MASHED_ORIG_TOP_RWMOD=1 (RwGlobals +0x11c is
// null without it, RESULT_PORT-OT-RWG-MODULES.md) and a non-zero handle (the init 0x00427ca0 that fills the four handles
// is still a stand-in on that path). Callees bind by name, no raw-address call: 0x004c5a60 =
// MashedRw_TextureDestroy_004c5a60 (LibRw/RwApiBind.cpp), RwGlobals +0x11c through DAT_007d3ff8 (MashedRwGlobals flfree).
// No other body of this RVA exists in either target.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" int MashedRw_TextureDestroy_004c5a60(int tex);
#define FUN_00556e40 Orig_00556e40
#define FUN_004c5a60 PortTop_00556e40_Call004c5a60
extern "C" {
void __cdecl PortTop_00556e40_Call004c5a60(int p0) { MashedRw_TextureDestroy_004c5a60(p0); }
#include "../../../../re/match/s24/00556e40.c"
}
#endif  // MASHED_STANDALONE
