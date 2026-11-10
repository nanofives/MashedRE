// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x004576f0 = the matched source re/match/s29/004576f0.c
// (POWERUPINIT-B, BYTE-IDENTICAL). "oil" / "oilShine" particle batches 0x0068a2c0 / 0x0068a210, four
// oilslicker.dff clones in the 16-byte records at 0x0068a250 (+0xc), each through 0x0040cda0 and the static 0x00456d00.
// Caller on the MASHED_ORIG_TOP path: the matched 0x0045bae0 through type-table entry 7 (+0x14), stored by
// PortTop_PWI_Call0045bae0 (Match/PortTop_0045bae0.cpp).
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp)
//  - 0x004770c0 = Orig_004770c0 (Match/PortTop_004770c0.cpp)
//  - 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp)
//  - 0x0042a5d0 = FUN_0042a5d0 (Match/MatchS06_0042a5d0.cpp, the exe body)
//  - 0x004e6ab0 = MashedRw_ClumpClone_004e6ab0 (LibRw/RwApiBind.cpp)
//  - 0x0040cda0 = PortTop_PWI_Call0040cda0 (forwarder below) -> Orig_0040cda0 (Match/PortTop_0040cda0.cpp, ATOMIC-VIEW)
//  - 0x00474d60 = Orig_00474d60 (Match/PortTop_00474d60.cpp)
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp)
// 0x0040cda0 (re/match/s02/0040cda0.c, BYTE-IDENTICAL) runs since ATOMIC-VIEW: its callback chain 0x0040bb10 ->
// 0x0040bae0 -> 0x0040baa0 is matched and reaches librw through the RW-layout atomic view. 0x004672d0 reads the camera
// record DAT_006905b0 (+0x04 = frame) when DAT_0067ecb0 != 3, else *(0x0042f510() + 4). The forwarder runs the matched
// body only when the record it will read is built (the 0x004768c0 precedent), else it logs and stands in. The static
// 0x00456d00 (record in EAX) is in the source, and calls 0x00474d60.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstring>
#define MATCH_PORT 1
#define FUN_004576f0 Orig_004576f0
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_00476cb0 Batch476cb0
#define FUN_004e6ab0 MashedRw_ClumpClone_004e6ab0
#define FUN_0040cda0 PortTop_PWI_Call0040cda0
#define FUN_00474d60 Orig_00474d60
#define FUN_004e6e00 MashedRw_ClumpDestroy_004e6e00
extern "C" {
#include "../../../../re/match/s29/004576f0.c"
void Orig_0040cda0(void *clump, unsigned int coef, int texture);
int MenuAlphaGet(void);
// Call site of rva 0x0040cda0 (reason in the header). The s29 source passes the coefficient as a float (push 0x3ecccccd),
// the s02 source takes it as a dword: the same 4 bytes.
void PortTop_PWI_Call0040cda0(int clump, float f, int tex) {
    const unsigned cam = *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x006905b0u));
    const unsigned frame = cam ? *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(cam + 4u)) : 0u;
    const int mode = MenuAlphaGet();
    const unsigned envoff = *reinterpret_cast<const volatile unsigned*>(static_cast<unsigned long>(0x00636ad0u));
    const bool run = (mode != 3) && cam != 0 && frame != 0;
    static int s_n = 0;
    if (s_n++ < 4) {
        if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
            std::fprintf(fp, "[ORIG_TOP] rva 0x0040cda0(clump 0x%08X, %g, %d): %s; DAT_0067ecb0 = %d, DAT_006905b0 = %08X (frame %08X), "
                             "DAT_00636ad0 = %u (ATOMIC-VIEW)\n", static_cast<unsigned>(clump), f, tex,
                         run ? "matched body" : "stand-in, not run: the record 0x004672d0 reads is not built", mode, cam, frame, envoff);
            std::fclose(fp);
        }
    }
    if (!run) return;
    unsigned bits;
    std::memcpy(&bits, &f, 4);
    Orig_0040cda0(reinterpret_cast<void *>(static_cast<unsigned long>(static_cast<unsigned>(clump))), bits, tex);
}
}  // extern "C"
#endif  // MASHED_STANDALONE
