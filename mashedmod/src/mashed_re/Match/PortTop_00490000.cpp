// PORT-OT-TRIPOOL (L-111 step 1): exe-only port body of rva 0x00490000 = the matched source re/match/s15/00490000.c:
// RpMaterialSetTexture(slot +0x0c, 0) (0x004e8090) for the 30 slots 0x0048ff20 builds (Match/PortTop_0048ff20.cpp),
// p = 0x0086a4ac step 0x50 to 0x0086ae0c. Caller: the matched 0x0040cfd0 (Match/PortTop_0040cfd0.cpp), through
// PortTop_TriPool_Call00490000 below, which runs the matched body only when all 30 slots hold a material (the body
// reads *(slot +0x0c) without a null check) and logs either way. 0x0040cfd0 runs in no check of this row
// (RESULT_PORT-OT-VEHICLE.md), so this body is compiled and bound, not run.
// Callee: 0x004e8090 = Orig_004e8090 through the int * forwarder of Match/PortTop_0048ffd0.cpp.
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
#define FUN_00490000 Orig_00490000
#define FUN_004e8090 PortTop_TriPool_Call004e8090
extern "C" {
#include "../../../../re/match/s15/00490000.c"
int PortTop_TriPool_SlotsBuilt(void);
void PortTop_TriPool_Call00490000(void) {
    const int built = PortTop_TriPool_SlotsBuilt();
    static int s_n = 0;
    if (s_n++ < 4) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, built ? "[ORIG_TOP] rva 0x00490000: matched body (30 slots built) (PORT-OT-TRIPOOL)\n"
                                  : "[ORIG_TOP] rva 0x00490000: not run, a slot has no clump or material (PORT-OT-TRIPOOL)\n");
            std::fclose(f);
        }
    }
    if (built) Orig_00490000();
}
}  // extern "C"
#endif  // MASHED_STANDALONE
