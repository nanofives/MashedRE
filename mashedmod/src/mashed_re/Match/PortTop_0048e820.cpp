// Exe-only port body of rva 0x0048e820 = the matched source re/match/s15/0048e820.c (EQUIV-FU-48E820, 2026-11-12:
// 100%, 664 B BYTE-IDENTICAL, relocheck OK 48/48; verify/equivcheck_20261112/RESULT_EQUIV-FU-48E820.md). Before that
// it ran as the 85.5% near-match admitted by L-118 (ESI <-> EDI swap) through an index bridge (PARTICLE-3-SWAP).
// For each emitter i = 16..0: 0x004b6480 clears 0x100 bits at DAT_00722250 + i*0x20, every one of the 256 0x40-byte
// particles of emitter i at 0x007228b0 gets x/y/z / +0x24 / +0x2c = 0 and the per-type +0x34, the per-type parameters
// at 0x00722140 + i*0x10 are written, then 0x0048cee0 builds record i of the 17 records at 0x00722470.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp), through
// PortTop_PS3_Call0048e820 below, which logs the record check once.
//
// The emitter index: the original keeps i in EDI across `call 0x0048cee0` (0x0048cee0 reads it as its argument, Ghidra
// unaff_EDI; re/match/s14/0048cee0.c). The source now calls FUN_0048cee0(i) (s15 source, end of the do loop); under
// MATCH_PORT it declares FUN_0048cee0(int i) instead of the static copy the DDK compile uses for the EDI convention.
// Callees bind by name: 0x004b6480 = Orig_004b6480 (Match/PortTop_004b6480.cpp), 0x0048cee0 =
// PortTop_PS3_Cee0_0048cee0 (Match/PortTop_0048cee0.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdint>
#define MATCH_PORT 1
#define FUN_0048e820 Orig_0048e820
#define FUN_004b6480 PortTop_PS3_BitClear_004b6480
#define FUN_0048cee0 PortTop_PS3_Cee0_0048cee0
// Object build only (MASHED_DDKOBJ): the DDK compiles the source as C without MATCH_PORT, so its DDK object carries the
// static 0x0048cee0 copy (140 B, itself BYTE-IDENTICAL to 0x0048cee0) and these three callees as undefined names.
// ddkobj renames each to what this wrapper #defines it to (same bindings as Match/PortTop_0048cee0.cpp). Unused here.
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_00476cb0 Batch476cb0
extern "C" {
#include "../../../../re/match/s15/0048e820.c"

void Orig_004b6480(unsigned char* param_1, int param_2);
}  // extern "C"

// Type adapter for the bit-clear callee: the s15 source declares (void *, unsigned int), Orig_004b6480 takes (unsigned char *, int).
extern "C" void PortTop_PS3_BitClear_004b6480(void* p0, unsigned int p1) {
    Orig_004b6480(static_cast<unsigned char*>(p0), static_cast<int>(p1));
}

// The init caller's call of this RVA (bound in Match/PortTop_0040bbb0.cpp). It pushes 0x20 (re/match/s02/0040bbb0.c:104),
// which the body does not read. After the body: count the 17 records at 0x00722470 whose +0x4 (the object 0x004770c0
// creates, read by 0x004768c0 at teardown) is non-zero.
extern "C" void PortTop_PS3_Call0048e820(int p0) {
    (void)p0;
    Orig_0048e820();
    static int s_once = 0;
    if (s_once++) return;
    const std::uint32_t* rec = reinterpret_cast<const std::uint32_t*>(0x00722470u);
    int nz = 0;
    for (int i = 0; i < 17; ++i) nz += rec[i * 16 + 1] != 0u;
    if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
        std::fprintf(fp, "[ORIG_TOP] rva 0x0048e820: matched body ran (EQUIV-FU-48E820); 0x0048cee0 built %d of 17 "
                         "records at 0x00722470 with +0x4 non-zero (rec[0] +0x4 = %08X, rec[16] +0x4 = %08X)\n",
                     nz, static_cast<unsigned>(rec[1]), static_cast<unsigned>(rec[16 * 16 + 1]));
        std::fclose(fp);
    }
}
#endif  // MASHED_STANDALONE
