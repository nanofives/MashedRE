// ATOMIC-VIEW (L-111 step 1): exe-only port body of rva 0x0040bae0 = the matched source re/match/s02/0040bae0.c
// (BYTE-IDENTICAL 44 B, relocheck OK): 0x0053fb70(atomic); when atomic +0x18 (the geometry) is non-zero,
// 0x004e89a0(geometry, 0x0040baa0, data); returns the atomic. On this path the atomic is the RW-layout view
// (MashedRwAtomicView), whose +0x18 is its geometry view.
// Callees bind by name, no raw-address call:
//  - 0x0053fb70 RpMatFXAtomicEnableEffects = MashedRw_MatFXAtomicEnableEffects_0053fb70 (resolves the view to its librw atomic).
//  - 0x004e89a0 RpGeometryForAllMaterials = MashedRw_GeometryForAllMaterials_004e89a0 (resolves the geometry view to its
//    librw geometry, PORT-OT-TRIPOOL), handing out librw rw::Material pointers (texture at +0x00, as RpMaterial).
//  - 0x0040baa0 = Orig_0040baa0 (Match/PortTop_0040baa0.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040bae0 Orig_0040bae0
#define FUN_0053fb70 MashedRw_MatFXAtomicEnableEffects_0053fb70
#define FUN_004e89a0 MashedRw_GeometryForAllMaterials_004e89a0
#define FUN_0040baa0 Orig_0040baa0
extern "C" {
#include "../../../../re/match/s02/0040bae0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
