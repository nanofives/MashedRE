// POWERUPINIT-A (L-111 step 1): exe-only port body of rva 0x004781b0 = the matched source re/match/s12/004781b0.c
// (MATCH-S12, 100%): RwImageSetPath(param_2) when param_2 != 0, RwStreamOpen(2, 1, name), RwStreamFindChunk 0x10,
// RpClumpStreamRead, RwStreamClose; returns the clump or 0. Caller on the MASHED_ORIG_TOP path: the matched builder
// 0x00411f30 ("LaserTower.dff", 0 -> DAT_0063bc40, Match/PortTop_00411f30.cpp) from 0x0045bae0. Callees bind by name, no
// raw-address call: the stream bindings of Match/PortTop_RwStream04F.cpp, RpClumpStreamRead 0x004e7420 and
// RwImageSetPath 0x004ce2d0 in LibRw/RwApiBind.cpp.
// The source is g2s text: it passes undefined4 values where its prototypes take pointers (C conversions C++ refuses), so
// those three callee names bind to C++ overloads taking the integers, which forward to the exe symbols. The source
// text's own prototypes stay declared and unused (the 0x00413b80 pattern, Match/PortTop_00413b80.cpp).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void* PortTop_RwStreamOpen_004cc230(int type, int access, const void* data);
extern "C" int PortTop_RwStreamFindChunk_004cc5e0(void* stream, int type, int* lenOut, unsigned* verOut);
extern "C" int PortTop_RwStreamClose_004cc160(void* stream, void* memOut);
extern "C" int MashedRw_ImageSetPath_004ce2d0(const char* path);
static inline int PortTop_004781b0_Call004cc230(unsigned int type, int access, unsigned int name) {
    return static_cast<int>(reinterpret_cast<unsigned long>(
        PortTop_RwStreamOpen_004cc230(static_cast<int>(type), access, reinterpret_cast<const void*>(static_cast<unsigned long>(name)))));
}
static inline int PortTop_004781b0_Call004cc5e0(int stream, int type, int lenOut, int verOut) {
    return PortTop_RwStreamFindChunk_004cc5e0(reinterpret_cast<void*>(static_cast<unsigned long>(stream)), type,
                                              reinterpret_cast<int*>(static_cast<unsigned long>(lenOut)),
                                              reinterpret_cast<unsigned*>(static_cast<unsigned long>(verOut)));
}
static inline int PortTop_004781b0_Call004cc160(int stream, int memOut) {
    return PortTop_RwStreamClose_004cc160(reinterpret_cast<void*>(static_cast<unsigned long>(stream)),
                                          reinterpret_cast<void*>(static_cast<unsigned long>(memOut)));
}
static inline int PortTop_004781b0_Call004ce2d0(int path) {
    return MashedRw_ImageSetPath_004ce2d0(reinterpret_cast<const char*>(static_cast<unsigned long>(path)));
}
#define FUN_004781b0 Orig_004781b0
#define FUN_004cc230 PortTop_004781b0_Call004cc230
#define FUN_004cc5e0 PortTop_004781b0_Call004cc5e0
#define FUN_004cc160 PortTop_004781b0_Call004cc160
#define FUN_004ce2d0 PortTop_004781b0_Call004ce2d0
#define RpClumpStreamRead MashedRw_ClumpStreamRead_004e7420
extern "C" {
#include "../../../../re/match/s12/004781b0.c"
}
#endif  // MASHED_STANDALONE
