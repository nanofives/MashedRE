// ============================================================================
//  L2_0045dfc0.cpp -- Lane 2 GENERATED verbatim ports (re/tools/decomp2port.py, 2026-09-10)
//  Every function below is a mechanical transcription of Ghidra's decompilation of
//  MASHED.exe (anchor in CLAUDE.md). Nothing here is understood or verified by
//  generation: a row moves to C3 only through the shadow A/B (return-value functions
//  carry a ShadowAB::Run wrapper) plus the caller/callee gate in re-classify.
//  Globals are read through their absolute addresses (.asi target only; NOT in
//  exe_sources.rsp). Unported callees are raw-RVA thunks.
// ============================================================================
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"
#include <cstdint>
#include <cstring>

namespace {
inline float f32bits(unsigned int u) { float f; std::memcpy(&f, &u, 4); return f; }
}
typedef unsigned char  undefined;
typedef unsigned char  undefined1;
typedef unsigned short undefined2;
typedef unsigned int   undefined4;
typedef unsigned char  byte;
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef long double    float10;
typedef void           code;

// MATCH-S10: the body of 0x0045dfc0 (L2_FUN_0045dfc0, its _impl and the ShadowAB::RunTracked wrapper) moved to
// Match/MatchS10_0045dfc0.cpp, which compiles the byte-matched source re/match/s10/0045dfc0.c.
