// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit): exe-only port body of rva 0x00552b60 FontSys_InitSeq
// (the Rt2d open) = the matched source re/match/s24/00552b60.c (100%, 45 B BYTE-IDENTICAL, relocheck OK).
// DAT_00912bdc = 0, then 0x00557250, 0x005540d0, 0x00556d20, 0x00555360, 0x005571e0, 0x00552c10 (results not read) and a tail
// jmp to 0x00552a60 at 0x00552b88. The jmp leaves the caller's argument in place, so 0x00552a60 gets the camera 0x00552b60
// was called with (0x00427ca0 passes 0, re/match/s06/00427ca0.c). The source declares neither argument; the entry
// PortTop_FontSysInit_00552b60(camera) below keeps it and the 0x00552a60 forwarder passes it on.
//
// Callees: exe bodies of this step (Match/PortTop_<rva>.cpp), no raw-address call; each forwarder records the result.
// 0x00555360 runs its unedited 94.4% source as a near-match body (L-115 / L-117 = (a), NEARMATCH-555360; byte table
// verify/portorigtop_20261109/RESULT_L115-HUDINIT-PROOFS.md), Match/PortTop_00555360.cpp.
//
// Owner of the Rt2d globals in the exe: this chain. It is the only writer of 0x00912500..0x0091320c (0x00912a00 brush
// list, 0x00912a04, 0x00912a08 font list, 0x00912a0c path, 0x00912a14, 0x00912a1c / 0x00912a20 dictionary, 0x00912a24 /
// 0x00912a28, 0x00912a2c, 0x00912a34 / 0x00912a38, 0x00912a44 matrix, 0x00912a84 matrix stack, 0x00912b04 depth,
// 0x00912bd8.., 0x00912bdc, 0x00912c0c, 0x00913100) and of the static freelist headers 0x007dc77c..0x007dc887. In the exe
// the hand code only reads them: Frontend/BatchAA_s1.cpp 0x00552840 (the stack at 0x00912a84 / depth 0x00912b04, and
// writes 0x00912bd8 / 0x00912bec as 0x00552750 does), Frontend/BatchAA_s6.cpp 0x00556cd0 (reads 0x00912a20); the one
// other writer, Frontend/BatchAA_s3.cpp SetDat00912a20, is 0x00556cc0's own body, which 0x00427ca0 calls after this.
//
// Call site: the 0x00427ca0 call in the matched 0x00402750 binds to the selector PortTop_00402750_Call00427ca0 below.
// MASHED_ORIG_TOP_RWMOD=1: the matched 0x00427ca0 body (HUDINIT-B), whose first call is this Rt2d open with camera 0.
// Without RWMOD: the stand-in PortTop_StandIn04E_00427ca0 alone (RwGlobals +0x118 and the +0xf4 string slot are 0 then,
// and 0x005540d0 calls +0x118 unconditionally).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdint>
#include <cstdio>

extern "C" {
int MashedRw_RwModKnob(void);
int PortTop_StandIn04E_00427ca0();
void FUN_00427ca0(void);              // matched, Match/MatchS06_00427ca0.cpp (exe callees bound since HUDINIT-B)
unsigned int Orig_00557250(void);
unsigned int Orig_005540d0(void);
unsigned int Orig_00556d20(void);
int Orig_00555360(void);            // near-match (L-115 / L-117), Match/PortTop_00555360.cpp
unsigned int Orig_005571e0(void);
int Orig_00552c10(void);
int Orig_00552a60(int camera);
}

namespace {
int s_camera;          // 0x00552b60's argument, handed to 0x00552a60 (the tail jmp at 0x00552b88)
int s_ret[7];          // results of the seven callees, in call order
int s_ran;

void HiaLog(const char* fmt, ...);
}  // namespace

extern "C" {
unsigned int PortTop_00552b60_Call00557250(void) { return static_cast<unsigned int>(s_ret[0] = static_cast<int>(Orig_00557250())); }
unsigned int PortTop_00552b60_Call005540d0(void) { return static_cast<unsigned int>(s_ret[1] = static_cast<int>(Orig_005540d0())); }
unsigned int PortTop_00552b60_Call00556d20(void) { return static_cast<unsigned int>(s_ret[2] = static_cast<int>(Orig_00556d20())); }
unsigned int PortTop_00552b60_Call00555360(void) { return static_cast<unsigned int>(s_ret[3] = Orig_00555360()); }
unsigned int PortTop_00552b60_Call005571e0(void) { return static_cast<unsigned int>(s_ret[4] = static_cast<int>(Orig_005571e0())); }
unsigned int PortTop_00552b60_Call00552c10(void) { return static_cast<unsigned int>(s_ret[5] = Orig_00552c10()); }
unsigned int PortTop_00552b60_Call00552a60(void) { return static_cast<unsigned int>(s_ret[6] = Orig_00552a60(s_camera)); }
}  // extern "C"

#define MATCH_PORT 1
#define FUN_00552b60 Orig_00552b60
#define FUN_00557250 PortTop_00552b60_Call00557250   // Match/PortTop_00557250.cpp (two freelists)
#define FUN_005540d0 PortTop_00552b60_Call005540d0   // Match/PortTop_005540d0.cpp (canvas list + 0x00553f40)
#define FUN_00556d20 PortTop_00552b60_Call00556d20   // Match/PortTop_00556d20.cpp (0x2400 buffer, brush list)
#define FUN_00555360 PortTop_00552b60_Call00555360   // near-match Orig_00555360 (font lists, path, dictionary)
#define FUN_005571e0 PortTop_00552b60_Call005571e0   // Match/PortTop_005571e0.cpp (one freelist)
#define FUN_00552c10 PortTop_00552b60_Call00552c10   // Match/PortTop_00552c10.cpp (matrix stack, render state)
#define FUN_00552a60 PortTop_00552b60_Call00552a60   // Match/PortTop_00552a60.cpp, with the camera (tail jmp 0x00552b88)
extern "C" {
#include "../../../../re/match/s24/00552b60.c"
}

namespace {
void HiaLog(const char* fmt, ...) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fputs("[ORIG_TOP] ", f);
        va_list ap;
        va_start(ap, fmt);
        std::vfprintf(f, fmt, ap);
        va_end(ap);
        std::fputs(" (HUDINIT-A)\n", f);
        std::fclose(f);
    }
}
std::uint32_t D(std::uint32_t a) { return *reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(a)); }
}  // namespace

extern "C" {
// 0x00552b60 with its argument (the camera). The one entry into this chain in the exe.
void PortTop_FontSysInit_00552b60(int camera) {
    s_camera = camera;
    Orig_00552b60();
    s_ran = 1;
    const char* const path = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(D(0x00912a0cu)));
    HiaLog("rva 0x00552b60 Rt2d open(camera 0x%08X) ran: 0x00557250=%d 0x005540d0=%d 0x00556d20=%d 0x00555360=%d "
           "0x005571e0=%d 0x00552c10=%d 0x00552a60=%d", static_cast<unsigned>(camera), s_ret[0], s_ret[1], s_ret[2], s_ret[3],
           s_ret[4], s_ret[5], s_ret[6]);
    HiaLog("  freelists 0x00912a00=%08X 0x00912a08=%08X 0x00912a14=%08X 0x00912a24=%08X 0x00912a2c=%08X 0x00912a34=%08X "
           "0x00912a38=%08X; 0x00912a04=%08X 0x00912a28=%08X", D(0x00912a00u), D(0x00912a08u), D(0x00912a14u), D(0x00912a24u),
           D(0x00912a2cu), D(0x00912a34u), D(0x00912a38u), D(0x00912a04u), D(0x00912a28u));
    HiaLog("  path 0x00912a0c=%08X \"%s\" (cap 0x00912a10=%u), dictionary 0x00912a1c=%08X 0x00912a20=%08X, reader 0x00913100=%08X",
           D(0x00912a0cu), path ? path : "", D(0x00912a10u), D(0x00912a1cu), D(0x00912a20u), D(0x00913100u));
    HiaLog("  matrix stack 0x00912a84[0]=%08X depth 0x00912b04=%u, 0x00912a44 flags %08X, 0x00912c0c=%08X, 0x00912bdc=%u, "
           "index 0x00912700[6..11]=%04X %04X %04X %04X %04X %04X", D(0x00912a84u), D(0x00912b04u), D(0x00912a50u),
           D(0x00912c0cu), D(0x00912bdcu), D(0x00912700u + 12) & 0xffffu, D(0x00912700u + 14) & 0xffffu,
           D(0x00912700u + 16) & 0xffffu, D(0x00912700u + 18) & 0xffffu, D(0x00912700u + 20) & 0xffffu,
           D(0x00912700u + 22) & 0xffffu);
}

// The 0x00427ca0 call of the matched 0x00402750 (Match/PortTop_00402750.cpp). [HUDINIT-B] RWMOD on: the matched body
// of that RVA (Match/MatchS06_00427ca0.cpp, re/match/s06/00427ca0.c), whose first call is this Rt2d open with camera 0,
// then the brushes and handles 0x0067d838..0x0067d848. RWMOD off: the stand-in (the body allocates through RwGlobals
// +0x118, which is 0 without RWMOD).
int PortTop_00402750_Call00427ca0() {
    const bool on = MashedRw_RwModKnob() != 0;
    if (!on) {
        HiaLog("rva 0x00427ca0: RWMOD off: stand-in only (the body needs RwGlobals +0x118)");
        return PortTop_StandIn04E_00427ca0();
    }
    HiaLog("rva 0x00427ca0: RWMOD on: matched body (HUDINIT-B)");
    FUN_00427ca0();
    const std::uint32_t* const h = reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(0x0067d838u));
    HiaLog("rva 0x00427ca0 ran: font 0x0067d838=%08X brushes 0x0067d83c=%08X 0x0067d840=%08X 0x0067d844=%08X 0x0067d848=%08X, "
           "dictionary 0x00912a20=%08X", h[0], h[1], h[2], h[3], h[4], D(0x00912a20u));
    return 0;
}
}  // extern "C"
#endif  // MASHED_STANDALONE
