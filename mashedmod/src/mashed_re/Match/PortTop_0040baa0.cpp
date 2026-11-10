// ATOMIC-VIEW (L-111 step 1): exe-only port body of rva 0x0040baa0 = the matched source re/match/s02/0040baa0.c
// (BYTE-IDENTICAL 63 B, relocheck OK; the `#ifdef __cplusplus` prototypes added by this row leave the DDK text
// unchanged): when the material's texture (+0x00) is non-zero, 0x0053fbb0(material, 2) and 0x0053fd30(material,
// data[1] texture, data[2] frame, 1, data[0] coefficient), else 0x0053fbb0(material, 0); returns the material. The
// material is a librw rw::Material (texture at +0x00, static_assert in LibRw/RwApiBind.cpp).
// Callees bind by name: 0x0053fbb0 RpMatFXMaterialSetEffects = MashedRw_MatFXMaterialSetEffects_0053fbb0, 0x0053fd30
// RpMatFXMaterialSetupEnvMap = MashedRw_MatFXMaterialSetupEnvMap_0053fd30 (LibRw/RwApiBind.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0040baa0 Orig_0040baa0
#define FUN_0053fbb0 MashedRw_MatFXMaterialSetEffects_0053fbb0
#define FUN_0053fd30 MashedRw_MatFXMaterialSetupEnvMap_0053fd30
extern "C" {
#include "../../../../re/match/s02/0040baa0.c"
}  // extern "C"
#endif  // MASHED_STANDALONE
