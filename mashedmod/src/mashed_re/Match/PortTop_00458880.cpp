// PORT-OT-PICKUPPOOL (L-111 (a) step 1): exe-only port body of rva 0x00458880 = the matched source re/match/s09/00458880.c
// (MATCH-S09, 100%): RpClumpDestroy(DAT_0068b198[i].clump) for the 25 0x50-byte records at 0x0068b198, then
// 0x004768c0(&DAT_0068b968), then RwTexDictionaryDestroy(DAT_0068b9ac). Its only caller in the exe is the matched
// 0x0045b930 (Match/PortTop_0045b930.cpp, AppDestroy 0x00402a40 on the MASHED_ORIG_TOP path), through the call-site
// forwarder PortTop_0045b930_Call00458880, which first hands back the 25 +0x00 words the exe's hand pickup code
// (Gameplay/PickupTick.cpp BindAll) wrote into the pool (PickupsOrig_ReleasePool).
// All three things this body frees are built only by 0x004587a0 (re/match/s09/004587a0.c), whose only caller is
// 0x0045bae0, a stand-in on this path (PortTop_StandIn04E_0045bae0). No exe code writes 0x0068b968 or 0x0068b9ac, and
// all three addresses are BSS in MASHED.exe (.data raw size 0x4d000 from 0x005ea000), so at this call they are zero.
// Callees bind by name, no raw-address call:
//  - 0x004e6e00 = MashedRw_ClumpDestroy_004e6e00 (LibRw/RwApiBind.cpp, rw::Clump::destroy, null skipped).
//  - 0x004c5930 = MashedRw_TexDictionaryDestroy_004c5930 (LibRw/RwApiBind.cpp, rw::TexDictionary::destroy, null skipped).
//  - 0x004768c0 = the record selector below: the matched Orig_004768c0 (Match/PortTop_004768c0.cpp) when the record's
//    +0x04 word is non-zero, else a stand-in that logs and does nothing. The matched body reads *(*(p + 4) + 4) with no
//    null check (re/match/s12/004768c0.c), and the record is only built by 0x004770c0 from 0x004587a0
//    (0x004587c5..0x004587d2). Same shape as the 0x00556e40 handle selector (Match/PortTop_00427620.cpp). The same
//    selector serves 0x004593b0 (Match/PortTop_004593b0.cpp, record 0x0068b9b8 built by 0x00459290).
// No other body of this RVA exists in either target.
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
extern "C" {
int MashedRw_ClumpDestroy_004e6e00(int clump);
int MashedRw_TexDictionaryDestroy_004c5930(int dict);
void Orig_004768c0(int p0);                                   // matched, Match/PortTop_004768c0.cpp
// Record selector (call-through, not a body): the matched Orig_004768c0 when the record is built, else logged and skipped.
void PortTop_PKP_Call004768c0(void* rec) {
    const unsigned int r = static_cast<unsigned int>(reinterpret_cast<unsigned long>(rec));
    const unsigned int w4 = *reinterpret_cast<const unsigned int*>(static_cast<unsigned long>(r + 4));
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x004768c0(0x%08X): %s (PORT-OT-PICKUPPOOL)\n", r,
                     w4 ? "matched body (record +0x04 set)" : "stand-in, not run (record +0x04 is 0, builder not run)");
        std::fclose(f);
    }
    if (w4) Orig_004768c0(static_cast<int>(r));
}
int PortTop_00458880_Call004e6e00(unsigned int p0) { return MashedRw_ClumpDestroy_004e6e00(static_cast<int>(p0)); }
void PortTop_00458880_Call004c5930(unsigned int p0) { MashedRw_TexDictionaryDestroy_004c5930(static_cast<int>(p0)); }
}  // extern "C"
#define FUN_00458880 Orig_00458880
#define FUN_004e6e00 PortTop_00458880_Call004e6e00   // RW: RpClumpDestroy -> rw::Clump::destroy
#define FUN_004768c0 PortTop_PKP_Call004768c0        // record selector (above)
#define FUN_004c5930 PortTop_00458880_Call004c5930   // RW: RwTexDictionaryDestroy -> rw::TexDictionary::destroy
extern "C" {
#include "../../../../re/match/s09/00458880.c"
}
#endif  // MASHED_STANDALONE
