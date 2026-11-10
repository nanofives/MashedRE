// PORT-OT-BOOT (L-111 (a) step 1): exe-only port body of rva 0x004026d0 = the matched source re/match/s01/004026d0.c
// (MATCH-S01, BYTE-IDENTICAL). The matched AppInitialiseOnBootup 0x00402750 (Match/PortTop_00402750.cpp) calls it with
// n = 4 on the MASHED_ORIG_TOP path. MatchS01.cpp compiles the same source as BootQueueFlush for both targets, but that
// copy's callees are absolute original addresses (0x004671a0, 0x004c1bb0, 0x004c1a00, 0x004c19f0, 0x004c1be0), which
// the exe does not map (Compat/StandaloneRvaThunks installs only five thunks, exe_main.cpp). This copy binds the same
// five callees by name to their exe bodies (verify/porttop_20261108/BINDINGS.tsv exe_symbol). All C linkage; the
// macros only convert the source's pointer arguments to the bindings' parameter types.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MASHED_RE_PORT 1
extern "C" {
unsigned BackBuffRasterOpen(int i);                                     // matched, Match/MatchS11_004671a0.cpp
int MashedRw_CameraClear_004c1bb0(int camera, const unsigned char* colour, int mode);  // RW: RwCameraClear
int IntroSplashVtableSlot6(int camera);                                 // matched 0x004c1a00 RwCameraBeginUpdate
int RwVtableSlot07Call(int camera);                                     // matched 0x004c19f0 RwCameraEndUpdate
void* Orig_004c1be0(void* camera, int a, int b);                        // matched, Match/PortTop_004c1be0.cpp
#define FUN_004026d0 Orig_004026d0
#define FUN_004671a0(i) ((void*)BackBuffRasterOpen(i))
#define FUN_004c1bb0(p, c, n) MashedRw_CameraClear_004c1bb0((int)(p), (const unsigned char*)(c), (n))
#define FUN_004c1a00(p) IntroSplashVtableSlot6((int)(p))
#define FUN_004c19f0(p) RwVtableSlot07Call((int)(p))
#define FUN_004c1be0(p, a, b) Orig_004c1be0((p), (a), (b))
#include "../../../../re/match/s01/004026d0.c"
}
#endif  // MASHED_STANDALONE
