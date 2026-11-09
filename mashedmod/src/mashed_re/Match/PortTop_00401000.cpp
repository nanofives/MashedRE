// PORT-TOP-04J (family 1, cup world / lights): exe-only port body of rva 0x00401000 = the matched source
// re/match/s01/00401000.c (match.py BYTE-IDENTICAL 482 B). Replaces the 04G stand-in of Match/PortTop_0040bbb0.cpp.
// Reached only on the MASHED_ORIG_TOP path, from the matched 0x0040bbb0. It builds the cup world:
//   DAT_00636568 = 0x004671a0(0); bbox (-100..100)^3; DAT_00636560 = RpWorldCreate(&bbox);
//   DAT_00636564 = 0x004b3bf0("cup.dff"); 0x004b5320(clump, 0x40, 1); DAT_00636574 = 0;
//   DAT_00636570 = RpLightCreate(2) + colour (0.75, 0.75, 0.75, 1);
//   DAT_0063656c = RpLightCreate(0x82) + a new frame, cone 0.7853982, flags (+0x02) = 1, colour 1, radius 100;
//   RpWorldAddLight x2, then 0x004e4450(world, clump).
// Every callee binds by name, no raw-address call:
//   - matched exe bodies: 0x004671a0 (Match/MatchS11_004671a0.cpp), 0x004c1800 / 0x004c1840, 0x004b3bf0 (PORT-TOP-04H),
//     0x004b5320, 0x004e4900, 0x004e4b90, 0x004e48e0 (Match/PortTop_<rva>.cpp).
//   - RW bindings (LibRw/RwApiBind.cpp): RpWorldCreate 0x004e5850, RpLightCreate 0x004e4dd0 (the RW-layout light
//     record), RwFrameCreate 0x004c0b30, _rwObjectHasFrameSetFrame 0x004c0740, RpWorldAddLight 0x004e4810,
//     0x004e4450 (hooks.csv name RwFrameAddChild; called with world, clump).
//   - sprintf 0x004a2b60 (MSVC CRT, libid): the exe CRT's vsprintf.
//   - 0x004b4080 / 0x0053fbb0 run only when DAT_00636ad0 != 0: named stand-ins below, which log the call.
#ifdef MASHED_STANDALONE
#include <cstdarg>
#include <cstdio>

extern "C" {
// rva 0x004a2b60 (sprintf, MSVC CRT libcmt sprintf.obj by libid): the exe CRT with the same arguments.
int PortTop_Sprintf04J_004a2b60(char* buf, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const int n = std::vsprintf(buf, fmt, ap);
    va_end(ap);
    return n;
}
namespace {
void Log04J(const char* fmt, int v) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fputs("[ORIG_TOP] ", f);
        std::fprintf(f, fmt, v);
        std::fputc('\n', f);
        std::fclose(f);
    }
}
}  // namespace
// rva 0x004b4080 (matched s18, callees 0x004b4050 / 0x004e89a0 not bound) and rva 0x0053fbb0 (C2, no matched source):
// the matched 0x00401000 calls them only when DAT_00636ad0 != 0. Stand-ins: log, return 0.
int PortTop_StandIn04J_004b4080(int clump) {
    Log04J("rva 0x004b4080(clump 0x%08X): stand-in, not run (DAT_00636ad0 != 0 branch of 0x00401000) (PORT-TOP-04J)", clump);
    return 0;
}
int PortTop_StandIn04J_0053fbb0(int a, int b) {
    (void)b;
    Log04J("rva 0x0053fbb0(0x%08X, 0): stand-in, not run (DAT_00636ad0 != 0 branch of 0x00401000) (PORT-TOP-04J)", a);
    return 0;
}
}  // extern "C"

#define MATCH_PORT 1
#define FUN_00401000 Orig_00401000
#define FUN_004671a0 BackBuffRasterOpen                       // matched, Match/MatchS11_004671a0.cpp
#define FUN_004c1800 Orig_004c1800                            // matched, Match/PortTop_004c1800.cpp (RwBBoxInitialize)
#define FUN_004c1840 Orig_004c1840                            // matched, Match/PortTop_004c1840.cpp (RwBBoxAddPoint)
#define RpWorldCreate MashedRw_WorldCreate_004e5850           // RW: RpWorldCreate -> rw::World::create
#define FUN_004a2b60 PortTop_Sprintf04J_004a2b60              // CRT sprintf
#define FUN_004b3bf0 Orig_004b3bf0                            // matched, Match/PortTop_004b3bf0.cpp (DFF read)
#define FUN_004b5320 Orig_004b5320                            // matched, Match/PortTop_004b5320.cpp
#define FUN_004b4080 PortTop_StandIn04J_004b4080              // stand-in (DAT_00636ad0 != 0 only)
#define FUN_0053fbb0 PortTop_StandIn04J_0053fbb0              // stand-in (DAT_00636ad0 != 0 only)
#define RpLightCreate MashedRw_LightCreate_004e4dd0           // RW: RpLightCreate -> the RW-layout light record
#define FUN_004e4900 Orig_004e4900                            // matched, Match/PortTop_004e4900.cpp (RpLightSetColor)
#define FUN_004c0b30 MashedRw_FrameCreate_004c0b30            // RW: RwFrameCreate
#define FUN_004c0740 MashedRw_ObjectHasFrameSetFrame_004c0740 // RW: _rwObjectHasFrameSetFrame (light record case)
#define RpLightSetConeAngle Orig_004e4b90                     // matched, Match/PortTop_004e4b90.cpp
#define FUN_004e48e0 Orig_004e48e0                            // matched, Match/PortTop_004e48e0.cpp (RpLightSetRadius)
#define RpWorldAddLight MashedRw_WorldAddLight_004e4810       // RW: RpWorldAddLight -> rw::World::addLight
#define RwFrameAddChild MashedRw_WorldAddClump_004e4450       // rva 0x004e4450 (world, clump) -> rw::World::addClump
extern "C" {
#include "../../../../re/match/s01/00401000.c"
}
#endif  // MASHED_STANDALONE
