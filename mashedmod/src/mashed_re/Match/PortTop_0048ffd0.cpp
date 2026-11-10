// PORT-OT-TRIPOOL (L-111 step 1): exe-only port body of rva 0x0048ffd0 = the matched source re/match/s15/0048ffd0.c:
// for each of the 30 slots built by 0x0048ff20 (Match/PortTop_0048ff20.cpp), RpMaterialSetTexture(slot +0x0c, 0)
// (0x004e8090) and RpClumpDestroy(slot +0x04) (0x004e6e00). Caller: the matched AppDestroy body 0x0040bd00
// (Match/PortTop_0040bd00.cpp), through PortTop_TriPool_Call0048ffd0 below, which runs the matched body only when all 30
// slots hold a clump and a material (the body reads both without a null check) and logs either way.
// Callees: 0x004e8090 = Orig_004e8090 (the matched body, Match/PortTop_004e8090.cpp; a librw material has its texture at
// +0x00), 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (rw::Clump::destroy, which frees the atomic and geometry the
// views of LibRw/RwApiBind.cpp point to; the views are not read after this).
// The source's 0x004e8090 prototype takes int *, its call passes the slot word: under MATCH_PORT the source declares the
// port prototype, and the forwarder below casts.
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdint>
#define MATCH_PORT 1
#define FUN_0048ffd0 Orig_0048ffd0
#define FUN_004e8090 PortTop_TriPool_Call004e8090
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s15/0048ffd0.c"
int* Orig_004e8090(int* p0, int p1);
int* PortTop_TriPool_Call004e8090(undefined4 p0, int p1) {
    return Orig_004e8090(reinterpret_cast<int*>(static_cast<std::uintptr_t>(p0)), p1);
}
int PortTop_TriPool_SlotsBuilt(void);
void PortTop_TriPool_Call0048ffd0(void) {
    const int built = PortTop_TriPool_SlotsBuilt();
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, built ? "[ORIG_TOP] rva 0x0048ffd0: matched body (30 slots built) (PORT-OT-TRIPOOL)\n"
                              : "[ORIG_TOP] rva 0x0048ffd0: not run, a slot has no clump or material (PORT-OT-TRIPOOL)\n");
        std::fclose(f);
    }
    if (built) Orig_0048ffd0();
}
}  // extern "C"
#endif  // MASHED_STANDALONE
