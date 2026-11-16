// RVA-LINT: ORIGTOP-ONLY
// [RWC-FRAME-MOVIE-BACKDROP] exe only: the movie module's per-frame leaves for the RW core frame route (MASHED_ORIG_TOP +
// MASHED_RWC_CORE + MASHED_RWC_FRAME). Called by the movie block of the 0x0043dfd0 stand-in
// (Match/PortTop_StandIns05.cpp, PortTop_RwcMovieBlock_0043dfd0). Off that route nothing calls them.
// Each body is its re/match source, compiled as C++ with MATCH_PORT, globals at their original addresses:
//   0x00494480  re/match/s15/00494480.c  100% byte-identical, relocheck OK (re/analysis/match_ledger.tsv)
//   0x004942b0  re/match/s15/004942b0.c  100% byte-identical, relocheck OK
//   0x00493fc0  re/match/s15/00493fc0.c  100% byte-identical, relocheck OK
//   0x00494f30  re/match/s16/00494f30.c  100% byte-identical, relocheck OK
//   0x004938e0  re/match/s15/004938e0.c  80.0%, ADMITTED (re/match/ADMITTED.tsv: EQUIV, L-126; not C4)
// The .asi keeps its own copies (Match/MatchS15_00494480.cpp, MatchS15_00493fc0.cpp, MatchS16_00494f30.cpp, which
// forward to the original's addresses); these exe names carry the PortTop_Movie_ prefix so the two never meet.
//
// Argument flow of 0x00494480 (disassembly of MASHED.exe.unpatched):
//   0x00494485 mov eax,[0x6147d8]; 0x0049448a mov ecx,[eax*4+0x771a10]; 0x00494491 push ecx; 0x00494492 call 0x493fc0
//   0x00494497 push eax; 0x00494498 call 0x4938e0; 0x0049449d add esp,8
// so 0x004938e0 receives (raster = 0x00493fc0's return = [0x00771a18], texture = [0x00771a10 + 4 * [0x006147d8]]): the
// texture is the dword 0x00493fc0 was passed, still on the stack. The source's converter form calls 0x004938e0 with one
// argument and 0x00493fc0 with one argument. In the port the two callees are bound to adapters that pass the same
// values explicitly (PortTop_Movie_004938e0_From00494480 reads the same two globals 0x00494485 / 0x0049448a read).
#ifdef MASHED_STANDALONE
extern "C" {
unsigned int PortTop_Movie_00493fc0(void);
void PortTop_Movie_004938e0(unsigned int *param_1, unsigned int param_2);
unsigned int PortTop_Movie_00493fc0_Arg(unsigned int ignored);
void PortTop_Movie_004938e0_From00494480(unsigned int raster);
}

#define MATCH_PORT 1

// 0x00493fc0: return DAT_00771a18
#define FUN_00493fc0 PortTop_Movie_00493fc0
extern "C" {
#include "../../../../re/match/s15/00493fc0.c"
}
#undef FUN_00493fc0

// 0x004938e0: walk raster+0 to the root, store param_2 at root + DAT_00911ae4
#define FUN_004938e0 PortTop_Movie_004938e0
extern "C" {
#include "../../../../re/match/s15/004938e0.c"
}
#undef FUN_004938e0

// 0x004942b0: IMediaEvent::GetEvent on 0x00771a28; EC_COMPLETE (1): put_CurrentPosition(0) on 0x00771a2c and, when
// DAT_00771a08 == 0, DAT_00771a04 = 0; FreeEventParams
#define FUN_004942b0 PortTop_Movie_004942b0
extern "C" {
#include "../../../../re/match/s15/004942b0.c"
}
#undef FUN_004942b0

// 0x00494f30: DAT_00771a50 = DAT_00771a54 = 0x00493fc0()
#define FUN_00494f30 PortTop_Movie_00494f30
#define FUN_00493fc0 PortTop_Movie_00493fc0
extern "C" {
#include "../../../../re/match/s16/00494f30.c"
}
#undef FUN_00493fc0
#undef FUN_00494f30

// 0x00494480: 0x004942b0, then 0x004938e0(0x00493fc0(...), ...), return 1
#define FUN_00494480 PortTop_Movie_00494480
#define FUN_004942b0 PortTop_Movie_004942b0
#define FUN_00493fc0 PortTop_Movie_00493fc0_Arg
#define FUN_004938e0 PortTop_Movie_004938e0_From00494480
extern "C" {
#include "../../../../re/match/s15/00494480.c"
}
#undef FUN_004938e0
#undef FUN_00493fc0
#undef FUN_004942b0
#undef FUN_00494480

extern "C" {
// 0x00493fc0 reads no argument (0x00493fc0 mov eax,[0x771a18]; ret): the caller's pushed dword is ignored.
unsigned int PortTop_Movie_00493fc0_Arg(unsigned int) { return PortTop_Movie_00493fc0(); }
// The second argument 0x00494480 leaves on the stack for 0x004938e0: [0x00771a10 + 4 * [0x006147d8]] (0x00494485, 0x0049448a).
void PortTop_Movie_004938e0_From00494480(unsigned int raster) {
    const int idx = *reinterpret_cast<const volatile int*>(static_cast<unsigned long>(0x006147d8u));
    const unsigned int tex = *reinterpret_cast<const volatile unsigned int*>(static_cast<unsigned long>(0x00771a10u + 4u * static_cast<unsigned>(idx)));
    PortTop_Movie_004938e0(reinterpret_cast<unsigned int*>(static_cast<unsigned long>(raster)), tex);
}
}
#endif  // MASHED_STANDALONE
