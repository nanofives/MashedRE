// PORT-TOP-04H (family 3, perm clumps): exe-only port body of rva 0x004025f0 = the matched source re/match/s01/00401690.c
// (0x004025f0 match.py BYTE-IDENTICAL 211 B, relocheck OK 6/6). For each of the 13 slots at 0x00636578 (stride 0x68):
// id from the local table, clump = 0x00401630(id) ("map_%d.dff"), the bounding box of the clump's first atomic
// (0x00401690), four clumps from 0x00402240 into +0x40 and six from 0x00402590 into +0x50. Reached only on the
// MASHED_ORIG_TOP path, from the matched 0x00402750 (Match/PortTop_00402750.cpp). Callees bind by name, no raw call:
//  - 0x00401630: the matched body Match/PortTop_00401630.cpp (-> 0x004b3bf0 -> RpClumpStreamRead on librw).
//  - 0x00401690: the static in the same source. It is the C2 reconstruction of that file (match.py 60.2%: the only
//    residual is the unroll factor of the vertex loop, x8 under 9176 where the original has x4), not a matched body.
//  - 0x004b3f90 (first atomic): LibRw/RwApiBind.cpp, an RW-layout view of the librw atomic (the fields 0x00401690 reads).
//  - 0x004c3ac0 Vec3Magnitude: the exe body Math/Vec3.cpp (Math::LutLength, the RW sqrt LUT).
//  - 0x00402240 / 0x00402590: stand-ins (below), return 0.
// Evidence: verify/porttop_20261108/RESULT_PORT-TOP-04H.md.
#ifdef MASHED_STANDALONE
#include <cstdio>

extern "C" {
static void StandInLog04H(const char* rva, const char* job, int* once) {
    if (*once) return;
    *once = 1;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva %s: stand-in, not run (%s) (PORT-TOP-04H)\n", rva, job);
        std::fclose(f);
    }
}
// Stand-ins for the two matched clump builders (re/match/s01/raw/00402240.c / 00402590.c). Each builds a clump with one
// atomic from a geometry builder (FUN_00401fe0 / FUN_004022a0) that writes RW-layout geometry and material fields
// (+0x2c / +0x30 / +0x34 / +0x5c, material +0x04 / +0x0c..+0x14) and calls RpGeometryCreate, RpMaterialCreate and four
// more RW geometry functions with no librw binding, plus FUN_004c1040 (no source). Each logs its first call and returns
// 0, so slots +0x40 / +0x50 stay 0 (the value they had while 0x004025f0 was a stand-in); 0x004015a0's RpClumpDestroy
// binding skips 0.

// rva 0x00402240 (stand-in)
int PortTop_StandIn04H_00402240(void) {
    static int once;
    StandInLog04H("0x00402240", "1-triangle clump; 0x00401fe0 RW geometry writes, RpGeometryCreate tree unbound", &once);
    return 0;
}
// rva 0x00402590 (stand-in)
int PortTop_StandIn04H_00402590(void) {
    static int once;
    StandInLog04H("0x00402590", "2-triangle clump; 0x004022a0 RW geometry writes, RpGeometryCreate tree unbound", &once);
    return 0;
}
}  // extern "C"

#define MATCH_PORT 1
#define FUN_004025f0 Orig_004025f0
#define FUN_00401630 Orig_00401630
#define FUN_004b3f90 MashedRw_ClumpFirstAtomic_004b3f90
#define FUN_004c3ac0 Vec3Magnitude
#define FUN_00402240 PortTop_StandIn04H_00402240
#define FUN_00402590 PortTop_StandIn04H_00402590
extern "C" {
#include "../../../../re/match/s01/00401690.c"
}
#endif  // MASHED_STANDALONE
