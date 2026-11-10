// PORT-OT-TRIPOOL (L-111 step 1): exe-only port body of rva 0x0048fef0 = the matched source re/match/s15/0048fef0.c
// (BYTE-IDENTICAL 35 B): geometry = atomic +0x18 (0x0048fef5), geometry +0x08 |= 0x40 (0x0048fef8),
// RpGeometryForAllMaterials(geometry, 0x0048fee0, data) (0x0048ff07), returns the atomic.
// The atomic it is given is an RW-layout view (LibRw/RwApiBind.cpp MashedRwAtomicView, built by
// MashedRw_AtomicView_0048fef0 at the 0x0048ff20 call site, Match/PortTop_0048ff20.cpp), so +0x18 is the view's
// geometry view, whose +0x08 holds a copy of the librw geometry's flags. The 0x004e89a0 binding writes that copy back
// to the librw geometry before it walks the materials.
// Callees bind by name: 0x0048fee0 = Orig_0048fee0 (Match/PortTop_0048fee0.cpp), 0x004e89a0 =
// MashedRw_GeometryForAllMaterials_004e89a0.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048fef0 Orig_0048fef0
#define FUN_0048fee0 Orig_0048fee0
#define FUN_004e89a0 MashedRw_GeometryForAllMaterials_004e89a0
extern "C" {
#include "../../../../re/match/s15/0048fef0.c"
}
#endif  // MASHED_STANDALONE
