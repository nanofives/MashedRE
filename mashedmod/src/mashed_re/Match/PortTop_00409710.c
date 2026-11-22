// BIND-GATES-ALLKNOBS (2026-11-21): exe-only port body of rva 0x00409710 = re/match/s01/raw/00409710.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP, from the bound caller named in
// re/tools/bind/state/gates/profile.json (re/tools/bind/porttop_bind.py --profile gates). Callees bind to exe bodies by
// name; a callee with no matched or admitted body, or outside the gate roots, is a named stop (Match/PortTop_GatesStops.cpp).
// C unit (BIND-CUNCLEAN-AS-C, re/tools/bind/porttop_bind.py): as C++ (MATCH_PORT, VS2022) the source fails
// with C2632 (state/gates/cpp_unclean.txt); cl compiles a .c as C, same exe symbol, C-linkage callees.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00409710 PortTop_Bound_00409710
#define FUN_004095a0 PortTop_GTStop_004095a0   // named stop
#define FUN_00495280 PortTop_PizFs_Mount_00495280   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004952f0 PortTop_PizFs_Unmount_004952f0   // exe body (Match/PortTop_00402750_Pieces.cpp)
#define FUN_004cbd30 RwCore_004cbd30   // exe body (RwCore/RwCore_004cbd30.cpp)
#define FUN_004cc160 RwCore_004cc160   // exe body (RwCore/RwCore_004cc160.cpp)
#define FUN_004cc230 RwCore_004cc230   // exe body (RwCore/RwCore_004cc230.cpp)
#define FUN_004cc5e0 RwCore_004cc5e0   // exe body (RwCore/RwCore_004cc5e0.cpp)
#include "../../../../re/match/s01/raw/00409710.c"
#endif // MASHED_STANDALONE
