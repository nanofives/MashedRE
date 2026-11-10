// PORT-OT-TRIPOOL (L-111 step 1): exe-only port body of rva 0x0048ff20 = the matched source re/match/s15/0048ff20.c
// (BYTE-IDENTICAL 170 B, relocheck OK): "Tri.dff" read into a zeroed 0x400-byte buffer (0x004b6520, 0x004b65a0, then
// 0x004b3b70 with the size 0x004b65b0 returns), then for each of the 30 0x50-byte slots at 0x0086a4a0: +0x04 = a clump
// parsed from the buffer (0x004b3e40), +0x00 = 0, RpClumpForAllAtomics(clump, 0x0048fed0, slot) (+0x08 = atomic),
// 0x0048fef0(+0x08, slot) (+0x0c = material). Caller: the matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp), at boot.
// Readers of what it builds on this path: the matched 0x0048ffd0 (AppDestroy, Match/PortTop_0048ffd0.cpp) and
// 0x00490000 (0x0040cfd0, Match/PortTop_00490000.cpp). The slots are the original's BSS at its address (the image pad,
// zero-filled). No other exe code on this path reads or writes 0x0086a4a0..0x0086ae1f: Match/MatchS15_004904d0.cpp
// compiles 0x004904d0 (zeroes slot +0x00) with no exe caller, and Util/L2_00490490.cpp is .asi-only. One owner: this
// body creates the 30 clumps and 0x0048ffd0 destroys them.
// Callees bind by name, no raw-address call:
//  - 0x004b6520 / 0x004b65a0 / 0x004b65b0 / 0x004b3b70 / 0x004b3e40 = Orig_<rva> (POWERUPINIT-A's TUs; 0x004b3b70's
//    RwStreamRead 0x004cbd30 is PortTop_RwStreamRead_004cbd30 there).
//  - 0x004e66d0 = MashedRw_ClumpForAllAtomics_004e66d0, 0x0048fed0 = Orig_0048fed0.
//  - 0x0048fef0 = PortTop_TriPool_Call0048fef0 below: the slot's librw atomic as an RW-layout view
//    (MashedRw_AtomicView_0048fef0), then Orig_0048fef0. RW 3.x reads the geometry at atomic +0x18, librw has it at +0x14.
// The source's "Tri.dff" global is a copy under MATCH_PORT (the exe does not load the original .rdata).
// Tri.dff is in Frontend's piz mount (the 0x004b68f0 binding answers from the mount stack); the log line below gives
// the read length and the slot counts.
#ifdef MASHED_STANDALONE
#include <cstdio>
#define MATCH_PORT 1
#define FUN_0048ff20 Orig_0048ff20
#define FUN_004b6520 Orig_004b6520
#define FUN_004b65a0 Orig_004b65a0
#define FUN_004b65b0 Orig_004b65b0
#define FUN_004b3b70 Orig_004b3b70
#define FUN_004b3e40 Orig_004b3e40
#define FUN_0048fed0 Orig_0048fed0
#define FUN_004e66d0 MashedRw_ClumpForAllAtomics_004e66d0
#define FUN_0048fef0 PortTop_TriPool_Call0048fef0
extern "C" {
#include "../../../../re/match/s15/0048ff20.c"
int Orig_0048fef0(int param_1, void* param_2);
int MashedRw_AtomicView_0048fef0(int atomic, int clump);
// The call site at 0x0048ff9d: the slot's atomic (+0x08, a librw atomic) as an RW-layout view, then the matched body.
int PortTop_TriPool_Call0048fef0(int param_1, void* param_2) {
    const int view = MashedRw_AtomicView_0048fef0(param_1, static_cast<Slot_48ff20*>(param_2)->clump);
    if (!view) return param_1;   // no atomic: the matched body would read *(0 + 0x18)
    Orig_0048fef0(view, param_2);
    return param_1;
}
// The call from 0x0040bbb0: the matched body, then one log line with what the 30 slots hold.
void PortTop_TriPool_Call0048ff20(void) {
    Orig_0048ff20();
    int clumps = 0, atomics = 0, materials = 0;
    for (int i = 0; i < 30; ++i) {
        clumps += DAT_0086a4a0[i].clump != 0;
        atomics += DAT_0086a4a0[i].atomic != 0;
        materials += DAT_0086a4a0[i].rest[0] != 0;
    }
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x0048ff20: matched body, 30 slots at 0x0086a4a0: (clump, atomic, material) set "
                        "(%d, %d, %d), slot 0 = %08X / %08X / %08X (PORT-OT-TRIPOOL)\n", clumps, atomics, materials,
                     DAT_0086a4a0[0].clump, DAT_0086a4a0[0].atomic, DAT_0086a4a0[0].rest[0]);
        std::fclose(f);
    }
}
// For 0x0048ffd0 / 0x00490000: 1 when every slot has its clump (+0x04) and material (+0x0c), the words both read
// without a null check.
int PortTop_TriPool_SlotsBuilt(void) {
    for (int i = 0; i < 30; ++i)
        if (!DAT_0086a4a0[i].clump || !DAT_0086a4a0[i].rest[0]) return 0;
    return 1;
}
}  // extern "C"
#endif  // MASHED_STANDALONE
