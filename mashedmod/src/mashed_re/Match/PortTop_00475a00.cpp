// PORT-OT-RENDER-2 (L-111 step 1, L-115 near-match body): exe-only port body of rva 0x00475a00 = re/match/s12/00475a00.c
// (MATCH-S12, 93.1%: NOT a byte match, port axis only, C2 stays C2). L-115 lets a near-match body run on MASHED_ORIG_TOP when
// the only residual is the SIB base/index order. Here that is all of it: match.py --bytes gives 91 B, 2 differing unmasked
// bytes, both SIB bytes. 0x00475a41: original 89 04 17 `mov [edi+edx], eax`, DDK 89 04 3a `mov [edx+edi], eax`. 0x00475a49:
// original 89 34 07 `mov [edi+eax], esi`, DDK 89 34 38 `mov [eax+edi], esi`. Scale 1, no displacement, neither base is
// EBP/ESP, so the effective address, the segment (DS) and the stored value are the same. relocheck OK 9/9.
// Job: record DAT_00691608 + DAT_0069160c * 0x40 = 0x004770c0(rec, 0x80c, param_2, param_1), DAT_00691604[n] =
// 0x00474db0(param_2, 0x50), DAT_00691610[n] = param_2, DAT_0069160c + 1. The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp)
// calls it twice on the MASHED_ORIG_TOP path, after 0x004759b0(2): ("scorch" texture, 0x18) and ("wfall" texture, 0x10).
// Its teardown 0x00475d90 (Match/PortTop_00475d90.cpp, from the matched 0x0040bd00) frees these two records, so the two
// swap together.
// Callees bind by name, no raw-address call (forwarders carry the source's prototypes):
//  - 0x004770c0 -> Orig_004770c0 (Match/PortTop_004770c0.cpp). Flag 0x800 calls 0x00476c10 (Batch476c10,
//    Render/RfxEmitter.cpp), as for 0x00476390's 0x817 / 0x807 records.
//  - 0x00474db0 -> Orig_00474db0 (Match/PortTop_00474db0.cpp): RwGlobals +0x108 (librw malloc) + 0x004b6520.
// Shared state: no other exe TU reads or writes 0x00691600..0x00691617 (the four arrays of Match/PortTop_004759b0.cpp).
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00475a00 Orig_00475a00
#define FUN_004770c0 PortTop_00475a00_Call004770c0
#define FUN_00474db0 PortTop_00475a00_Call00474db0
extern "C" {
void Orig_004770c0(unsigned int* p0, unsigned int p1, unsigned int p2, unsigned int p3);
unsigned int Orig_00474db0(int p0, int p1);
#include "../../../../re/match/s12/00475a00.c"
void PortTop_00475a00_Call004770c0(int p0, uint p1, uint p2, undefined4 p3) {
    Orig_004770c0(reinterpret_cast<unsigned int*>(static_cast<unsigned long>(static_cast<unsigned int>(p0))), p1, p2, p3);
}
undefined4 PortTop_00475a00_Call00474db0(int p0, int p1) { return Orig_00474db0(p0, p1); }
}
#endif  // MASHED_STANDALONE
