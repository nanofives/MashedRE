// POWERUPINIT-C (L-111 step 1): exe-only port body of rva 0x004b5240 = the matched source re/match/s18/004b5240.c
// (BYTE-IDENTICAL): sets bit 0x04 of the byte at atomic +0x02 when the flag is non-zero, else clears it. Callers on the
// MASHED_ORIG_TOP path: the matched inits 0x004583a0, 0x0045b830, 0x00454e70, 0x00452f70 (Match/PortTop_<rva>.cpp),
// on the librw atomics 0x004b3fc0 collects. librw rw::Atomic starts with rw::Object {type, subType, flags,
// privateFlags, parent} (deps/librw/src/rwobjects.h), so +0x02 is rw::Object::flags, and 0x04 is rw::Atomic::RENDER,
// the same bit as RW's rpATOMICRENDER.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004b5240 Orig_004b5240
extern "C" {
#include "../../../../re/match/s18/004b5240.c"
}
#endif  // MASHED_STANDALONE
