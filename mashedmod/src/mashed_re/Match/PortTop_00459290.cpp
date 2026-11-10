// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x00459290 = the matched source re/match/s09/00459290.c
// (MATCH-S09, 100%): the record DAT_0068b9b8 (0x004770c0 with 0x807, 0x19 and the "Headlight" sprite, then 0x00476cb0
// 5 / 6 and 0x00476c10 colour {0, 0, 1, 1}), DAT_0068bb58 = 0x004b3d80("TargetHUD.txd"), 0x004c5c80(0), then for the
// four 0x58-byte slots at 0x0068b9f8: an identity matrix from 0x004c57a0 at +0x38, a "lazer.dff" clump from 0x004b3bf0
// at +0x54 (0x00474d60 flag 0 on it), +0x00 = index, +0x08 = -1, +0x10 = 0.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 (Match/PortTop_0045bae0.cpp). The record 0x0068b9b8 is the
// one the matched teardown 0x004593b0 passes to 0x004768c0 (Match/PortTop_004593b0.cpp), and the slots are the ones it
// frees.
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp, both targets).
//  - 0x004770c0 = Orig_004770c0 (Match/PortTop_004770c0.cpp, exe-only matched copy, original record layout).
//  - 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp, matched). 0x00476c10 = Batch476c10 (Render/RfxEmitter.cpp,
//    the original record layout, as Match/PortTop_004770c0.cpp binds it). Both through C++ forwarders: the source
//    passes uint* / float*, the exe symbols take void* / uint32*.
//  - 0x004b3d80 = Orig_004b3d80, 0x004b3bf0 = Orig_004b3bf0 (Match/PortTop_004b3d80.cpp, PortTop_004b3bf0.cpp).
//  - 0x004c57a0 = MashedRw_MatrixCreate_004c57a0 (LibRw/RwBridge.cpp, rw::Matrix::create, identity 0x20003).
//  - 0x004c5c80 = MashedRw_TexDictionarySetCurrent_004c5c80 (LibRw/RwApiBind.cpp).
//  - 0x00474d60 = Orig_00474d60 (Match/PortTop_00474d60.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void Batch476cb0(void* holder, unsigned int p2, unsigned int p3);
extern "C" void Batch476c10(unsigned int* p1, unsigned int* p2);
#define FUN_00459290 Orig_00459290
#define FUN_0040bb30 Search40bb30
#define FUN_00474d60 Orig_00474d60
#define FUN_00476c10 PortTop_00459290_Call00476c10
#define FUN_00476cb0 PortTop_00459290_Call00476cb0
#define FUN_004770c0 Orig_004770c0
#define FUN_004b3bf0 Orig_004b3bf0
#define FUN_004b3d80 Orig_004b3d80
#define FUN_004c57a0 MashedRw_MatrixCreate_004c57a0
#define FUN_004c5c80 MashedRw_TexDictionarySetCurrent_004c5c80
extern "C" {
#include "../../../../re/match/s09/00459290.c"
// Call-throughs (not bodies), with the source's own parameter types.
void PortTop_00459290_Call00476cb0(unsigned int* p, int a, int b) {
    Batch476cb0(p, static_cast<unsigned int>(a), static_cast<unsigned int>(b));
}
void PortTop_00459290_Call00476c10(unsigned int* p, float* col) {
    Batch476c10(p, reinterpret_cast<unsigned int*>(col));
}
}  // extern "C"
#endif  // MASHED_STANDALONE
