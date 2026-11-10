// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x00458360 = the matched source re/match/s09/00458360.c
// (MATCH-S09, BYTE-IDENTICAL 51 B): 0x00458020 over the four 0x18-byte dropper records at 0x0068b0d0, then 0x00457ba0
// over the 32 0x6c-byte mine records at 0x0068a310. Both are statics of the same source (BYTE-IDENTICAL): 0x00457ba0
// writes the record's identity matrix and zeroes its state, then detaches +0x58 (the mine clone) from its world;
// 0x00458020 releases the two entry pointers through 0x00457ba0, zeroes +0x08..+0x10, 0x004b6520(h, 8), detaches +0x14
// (the dropper clone). Caller on the MASHED_ORIG_TOP path: the matched init 0x004583a0 (Match/PortTop_004583a0.cpp),
// right after it filled both arrays, so every record these read was written by that init.
// Callees bind by name, no raw-address call:
//  - 0x004e4800 = MashedRw_ClumpGetWorld_004e4800, 0x004e45b0 = MashedRw_WorldRemoveClump_004e45b0 (LibRw/RwApiBind.cpp).
//    The clones are in no world, so 0x004e45b0 is not reached.
//  - 0x004b6520 = Orig_004b6520 (Match/PortTop_004b6520.cpp).
// The same source holds 0x00458080 (static) and 0x004582f0, which nothing in the exe calls. The linker still needs
// their callees, so those bind to logging traps PortTop_PWC_NotRun_<rva> below (and 0x004c0ed0 to its binding).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
#define FUN_00458360 Orig_00458360
#define FUN_004582f0 PortTop_PWC_NotRun_004582f0
#define FUN_004e4800 MashedRw_ClumpGetWorld_004e4800
#define FUN_004e45b0 MashedRw_WorldRemoveClump_004e45b0
#define FUN_004b6520 Orig_004b6520
#define FUN_004c0ed0 MashedRw_FrameGetLTM_004c0ed0
#define FUN_004769a0 PortTop_PWC_NotRun_004769a0
#define FUN_004769d0 PortTop_PWC_NotRun_004769d0
#define FUN_004769f0 PortTop_PWC_NotRun_004769f0
#define FUN_00476d00 PortTop_PWC_NotRun_00476d00
#define FUN_00476df0 PortTop_PWC_NotRun_00476df0
#define FUN_00484cf0 PortTop_PWC_NotRun_00484cf0
#define FUN_00457e10 PortTop_PWC_NotRun_00457e10
#define FUN_00476880 PortTop_PWC_NotRun_00476880
#define FUN_00475ab0 PortTop_PWC_NotRun_00475ab0
#define FUN_00477760 PortTop_PWC_NotRun_00477760
#define FUN_00453eb0 PortTop_PWC_NotRun_00453eb0
#define FUN_004c1480 PortTop_PWC_NotRun_004c1480
extern "C" {
#include "../../../../re/match/s09/00458360.c"
static void PortTop_PWC_Trap(const char* rva) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva %s: reached from the uncalled 0x00458080 / 0x004582f0, no exe path calls them "
                        "(POWERUPINIT-C)\n", rva);
        std::fclose(f);
    }
}
void PortTop_PWC_NotRun_004769a0(V3 *p) { (void)p; PortTop_PWC_Trap("0x004769a0"); }
void PortTop_PWC_NotRun_004769d0(V2 *s) { (void)s; PortTop_PWC_Trap("0x004769d0"); }
void PortTop_PWC_NotRun_004769f0(float *c) { (void)c; PortTop_PWC_Trap("0x004769f0"); }
void PortTop_PWC_NotRun_00476d00(int *l) { (void)l; PortTop_PWC_Trap("0x00476d00"); }
void PortTop_PWC_NotRun_00476df0(int *l) { (void)l; PortTop_PWC_Trap("0x00476df0"); }
void PortTop_PWC_NotRun_00484cf0(Query *q) { (void)q; PortTop_PWC_Trap("0x00484cf0"); }
void PortTop_PWC_NotRun_00457e10(void) { PortTop_PWC_Trap("0x00457e10"); }
void PortTop_PWC_NotRun_00476880(Sphere *s, float *v, float a, float b) { (void)s; (void)v; (void)a; (void)b; PortTop_PWC_Trap("0x00476880"); }
void PortTop_PWC_NotRun_00475ab0(int a, PoolJ *e, int c) { (void)a; (void)e; (void)c; PortTop_PWC_Trap("0x00475ab0"); }
void PortTop_PWC_NotRun_00477760(int a, PoolJ *e, RGBA *col, float b, float c) { (void)a; (void)e; (void)col; (void)b; (void)c; PortTop_PWC_Trap("0x00477760"); }
void PortTop_PWC_NotRun_00453eb0(V4 *p, float a, float b, float c, int d) { (void)p; (void)a; (void)b; (void)c; (void)d; PortTop_PWC_Trap("0x00453eb0"); }
void * PortTop_PWC_NotRun_004c1480(void *frame, Mat *m, int op) { (void)frame; (void)m; (void)op; PortTop_PWC_Trap("0x004c1480"); return 0; }
}  // extern "C"
#endif  // MASHED_STANDALONE
