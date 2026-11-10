// PORT-OT-HUD (L-111 (a) step 1): exe-only port body of rva 0x004770c0 = the matched source re/match/s12/004770c0.c
// (MATCH-S12, BYTE-IDENTICAL 801 B): zero the 0x40-byte record, record[1] = RpPTankAtomicCreate(cap, flags, 0), the
// matrix template at 0x00692558..0x00692590, the optional setters for flag bits 0x80..0x1000, the material texture,
// the PTank block +0x04 / +0x40 / +0xa4 / +0xa8 / +0xac, a new frame, record[2] = cap, the per-flag arrays (0x00474db0)
// at record[4..10], record[11] = record[12] = 0.
// Its only caller in the exe is the matched 0x00413b80 (Match/PortTop_00413b80.cpp), itself called only by the matched
// 0x0040bbb0 on the MASHED_ORIG_TOP path, with flags 0x37 and cap 0x14. The exe's other body of this RVA is the hand
// Render/RfxEmitter.cpp Rfx::Create on a port Emitter struct (default path), so this TU is the ORIGTOP-ONLY class.
// Callees bind by name to the exe symbols, no raw-address call. Forwarders carry the source's prototypes:
//  - 0x004b6520 Orig_004b6520 (Match/PortTop_004b6520.cpp), 0x004e7e30 Orig_004e7e30, 0x004e8090 Orig_004e8090 (matched).
//  - 0x00534b60 RpPTankAtomicCreate, 0x004c0b30 RwFrameCreate: LibRw/RwApiBind.cpp (the PTank atomic is the RW-layout
//    MashedRwPTankAtomic record, its block at +0x70 = [0x007dc57c], so the writes through +[0x007dc57c] land in it).
//  - 0x00474db0 Orig_00474db0 (Match/PortTop_00474db0.cpp): RwGlobals +0x108 (librw malloc) + 0x004b6520.
//  - 0x00476ae0 / 0x00476b30 / 0x00476b90 / 0x00476be0: the matched exe bodies Match/MatchS12_*.cpp; 0x00476c10 /
//    0x00476c60: Render/RfxEmitter.cpp (both targets). Flags 0x37 call none of the six.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" void Orig_004b6520(unsigned int dst, unsigned int size);
extern "C" int Orig_004e7e30(int p0, int p1);
extern "C" int* Orig_004e8090(int* p0, int p1);
extern "C" int MashedRw_FrameCreate_004c0b30(void);
extern "C" int MashedRw_PTankAtomicCreate_00534b60(int maxPCount, unsigned dataFlags, int platFlags);
extern "C" unsigned int Orig_00474db0(int p0, int p1);
extern "C" void Batch476ae0(void* holder, void* p2);
extern "C" void Set476b30(void* holder, unsigned int* p2);
extern "C" void Batch476b90(void* holder, void* p2);
extern "C" void Batch476be0(void* holder, unsigned int p2);
extern "C" void Batch476c10(unsigned int* p1, unsigned int* p2);
extern "C" void Batch476c60(unsigned int* p1, unsigned int* p2);

#define FUN_004770c0 Orig_004770c0
#define FUN_00474db0 PortTop_004770c0_Call00474db0
#define FUN_00476ae0 PortTop_004770c0_Call00476ae0
#define FUN_00476b30 PortTop_004770c0_Call00476b30
#define FUN_00476b90 PortTop_004770c0_Call00476b90
#define FUN_00476be0 PortTop_004770c0_Call00476be0
#define FUN_00476c10 PortTop_004770c0_Call00476c10
#define FUN_00476c60 PortTop_004770c0_Call00476c60
#define FUN_004b6520 PortTop_004770c0_Call004b6520
#define FUN_004c0b30 PortTop_004770c0_Call004c0b30
#define FUN_004e7e30 PortTop_004770c0_Call004e7e30
#define FUN_004e8090 PortTop_004770c0_Call004e8090
#define FUN_00534b60 PortTop_004770c0_Call00534b60
extern "C" {
static inline unsigned long PortTop_004770c0_Addr(const void* p) { return reinterpret_cast<unsigned long>(p); }
unsigned int PortTop_004770c0_Call00474db0(unsigned int p0, int p1) { return Orig_00474db0(static_cast<int>(p0), p1); }
void PortTop_004770c0_Call00476ae0(unsigned int* p0, int p1) {
    Batch476ae0(p0, reinterpret_cast<void*>(static_cast<unsigned long>(p1)));
}
void PortTop_004770c0_Call00476b30(unsigned int* p0, int p1) {
    Set476b30(p0, reinterpret_cast<unsigned int*>(static_cast<unsigned long>(p1)));
}
void PortTop_004770c0_Call00476b90(unsigned int* p0, int p1) {
    Batch476b90(p0, reinterpret_cast<void*>(static_cast<unsigned long>(p1)));
}
void PortTop_004770c0_Call00476be0(unsigned int* p0, int p1) { Batch476be0(p0, static_cast<unsigned int>(p1)); }
void PortTop_004770c0_Call00476c10(unsigned int* p0, int p1) {
    Batch476c10(p0, reinterpret_cast<unsigned int*>(static_cast<unsigned long>(p1)));
}
void PortTop_004770c0_Call00476c60(unsigned int* p0, int p1) {
    Batch476c60(p0, reinterpret_cast<unsigned int*>(static_cast<unsigned long>(p1)));
}
void PortTop_004770c0_Call004b6520(void* p0, int p1) {
    Orig_004b6520(static_cast<unsigned int>(PortTop_004770c0_Addr(p0)), static_cast<unsigned int>(p1));
}
int PortTop_004770c0_Call004c0b30(void) { return MashedRw_FrameCreate_004c0b30(); }
int PortTop_004770c0_Call004e7e30(int p0, int p1) { return Orig_004e7e30(p0, p1); }
int* PortTop_004770c0_Call004e8090(int p0, unsigned int p1) {
    return Orig_004e8090(reinterpret_cast<int*>(static_cast<unsigned long>(static_cast<unsigned int>(p0))),
                         static_cast<int>(p1));
}
unsigned int PortTop_004770c0_Call00534b60(unsigned int p0, unsigned int p1, int p2) {
    return static_cast<unsigned int>(MashedRw_PTankAtomicCreate_00534b60(static_cast<int>(p0), p1, p2));
}
#include "../../../../re/match/s12/004770c0.c"
}
#endif  // MASHED_STANDALONE
