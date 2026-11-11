// PARTICLE-3-SWAP: exe-only port body of rva 0x0048e820 = the UNEDITED near-match source re/match/s15/0048e820.c
// (85.5%, 124/124 ins), admitted per function on the port axis by L-118 (= a) under L-115 / L-117; never C4. Byte table:
// verify/portorigtop_20261109/RESULT_PORT-OT-PARTICLE-3.md (18/18 differing bytes = the callee-saved ESI <-> EDI swap,
// relocheck OK 48/48).
// For each emitter i = 16..0: 0x004b6480 clears 0x100 bits at DAT_00722250 + i*0x20, every one of the 256 0x40-byte
// particles of emitter i at 0x007228b0 gets x/y/z / +0x24 / +0x2c = 0 and the per-type +0x34, the per-type parameters
// at 0x00722140 + i*0x10 are written, then 0x0048cee0 builds record i of the 17 records at 0x00722470.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp), through
// PortTop_PS3_Call0048e820 below, which logs the record check once.
//
// The emitter index: the original keeps i in EDI across `call 0x0048cee0` (0x0048cee0 reads it as its argument, Ghidra
// unaff_EDI; re/match/s14/0048cee0.c). The unedited source calls FUN_0048cee0() with no argument, and VS2022 carries
// nothing in EDI. Each iteration calls 0x004b6480(&DAT_00722250[i * 8], 0x100) first (s15 source line 58), so the
// bridge PortTop_PS3_BitClear_004b6480 records i = (p0 - 0x00722250) / 0x20 and runs Orig_004b6480, and
// PortTop_PS3_Call0048cee0 passes that i to 0x0048cee0. s_emitter is the one owner of that value; nothing else reads it.
// Callees bind by name: 0x004b6480 = Orig_004b6480 (Match/PortTop_004b6480.cpp) through the bridge, 0x0048cee0 =
// PortTop_PS3_Cee0_0048cee0 (Match/PortTop_0048cee0.cpp) through the bridge.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdint>
#define MATCH_PORT 1
#define FUN_0048e820 Orig_0048e820
#define FUN_004b6480 PortTop_PS3_BitClear_004b6480
#define FUN_0048cee0 PortTop_PS3_Call0048cee0
extern "C" {
#include "../../../../re/match/s15/0048e820.c"

void Orig_004b6480(unsigned char* param_1, int param_2);
void PortTop_PS3_Cee0_0048cee0(int i);
}  // extern "C"

static int s_emitter = -1;

extern "C" void PortTop_PS3_BitClear_004b6480(void* p0, unsigned int p1) {
    s_emitter = static_cast<int>((reinterpret_cast<std::uintptr_t>(p0) - 0x00722250u) / 0x20u);
    Orig_004b6480(static_cast<unsigned char*>(p0), static_cast<int>(p1));
}

extern "C" void PortTop_PS3_Call0048cee0(void) { PortTop_PS3_Cee0_0048cee0(s_emitter); }

// The init caller's call of this RVA (bound in Match/PortTop_0040bbb0.cpp). It pushes 0x20 (re/match/s02/0040bbb0.c:104),
// which the body does not read. After the body: count the 17 records at 0x00722470 whose +0x4 (the object 0x004770c0
// creates, read by 0x004768c0 at teardown) is non-zero, and the emitter index the last bridge call carried.
extern "C" void PortTop_PS3_Call0048e820(int p0) {
    (void)p0;
    Orig_0048e820();
    static int s_once = 0;
    if (s_once++) return;
    const std::uint32_t* rec = reinterpret_cast<const std::uint32_t*>(0x00722470u);
    int nz = 0;
    for (int i = 0; i < 17; ++i) nz += rec[i * 16 + 1] != 0u;
    if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
        std::fprintf(fp, "[ORIG_TOP] rva 0x0048e820: near-match body ran (L-115/L-117/L-118); 0x0048cee0 built %d of 17 "
                         "records at 0x00722470 with +0x4 non-zero (rec[0] +0x4 = %08X, rec[16] +0x4 = %08X), last "
                         "emitter %d (PARTICLE-3-SWAP)\n",
                     nz, static_cast<unsigned>(rec[1]), static_cast<unsigned>(rec[16 * 16 + 1]), s_emitter);
        std::fclose(fp);
    }
}
#endif  // MASHED_STANDALONE
