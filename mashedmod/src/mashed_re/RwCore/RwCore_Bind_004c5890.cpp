// RWC-BIND-4C5890 (L-129 (b), L-124 (C)): the top-down binding of RwTexDictionaryCreate 0x004c5890. EXE ONLY.
//
// Not a body of 0x004c5890. The body is the byte-matched source re/match/s19/004c5890.c (100%, relocheck OK), compiled
// for the RW core path as RwCore_004c5890 (RwCore/RwCore_004c5890.cpp, generated; a DDK object in the object build).
// Every caller on the top-down path binds FUN_004c5890 to RwCoreBind_TexDictionaryCreate_004c5890 (this file): the RW
// core's own caller 0x004c5f60 (texture module constructor, `DAT_007d4080 = FUN_004c5890()`) through
// verify/rwccorerun_20261111/gen_rwcore_tus.py CALL_VIA, and the game caller 0x00555360 (Rt2d open) when its body lands
// (NEARMATCH-555360 restack: `#define FUN_004c5890 RwCoreBind_TexDictionaryCreate_004c5890`).
//
// MASHED_RWC_CORE=1: calls the matched body. It takes the 0x18-byte record from the texture module's texdict freelist
// (RwGlobals [0x007d3ff8] + module offset [0x007d4054] + 0xc) through the RwGlobals +0x118 allocator, so it needs the
// module constructor 0x004c5f60 to have run (RwEngineStart's plugin registry init 0x004d8000). The self-test below checks
// the record against the source text after each call and logs one line.
// MASHED_RWC_CORE off: returns 0, the matched body's own result when the allocation fails. L-129 (a) dropped the librw
// binding (rw::TexDictionary::create) and L-124 adds none, so the librw path has no dictionary create.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#include <cstdint>

extern "C" {
bool RwCore_Enabled(void);
void RwCore_Log(const char* fmt, ...);
void* RwCore_004c5890(void);

namespace {
inline std::uint32_t Rd32(std::uint32_t va) { return *reinterpret_cast<const std::uint32_t*>(static_cast<std::uintptr_t>(va)); }
inline std::uint8_t Rd8(std::uint32_t va) { return *reinterpret_cast<const std::uint8_t*>(static_cast<std::uintptr_t>(va)); }
unsigned s_calls = 0, s_fails = 0;
}

// Self-test of one 0x004c5890 call against re/match/s19/004c5890.c. Returns the number of failed checks.
static int RwCoreBind_CheckTexDict(std::uint32_t d, std::uint32_t g, std::uint32_t off, std::uint32_t oldHead) {
    int bad = 0;
    const std::uint32_t list = g + off;   // TG.texDictList (TGL(texDictList), RwGlobals + [0x007d4054] + 0)
    // rwObjectInitialize(d, 6, 0): type 6, subType 0, flags 0, privateFlags 0, parent 0
    bad += Rd8(d + 0) != 6;
    bad += Rd8(d + 1) != 0;
    bad += Rd8(d + 2) != 0;
    bad += Rd8(d + 3) != 0;
    bad += Rd32(d + 4) != 0;
    // rwLinkListInitialize(&d->texturesInDict): next = prev = &texturesInDict (d + 8)
    bad += Rd32(d + 8) != d + 8;
    bad += Rd32(d + 0xc) != d + 8;
    // rwLinkListAddLLLink(&TGL(texDictList), &d->lInInstance): lInInstance (d + 0x10) at the head
    bad += Rd32(d + 0x10) != oldHead;
    bad += Rd32(d + 0x14) != list;
    bad += Rd32(list) != d + 0x10;
    bad += Rd32(oldHead + 4) != d + 0x10;
    return bad;
}

// RwTexDictionaryCreate binding entry (not a body; the body is RwCore_004c5890).
void* RwCoreBind_TexDictionaryCreate_004c5890(void) {
    if (!RwCore_Enabled()) {
        RwCore_Log("RwTexDictionaryCreate 0x004c5890: MASHED_RWC_CORE off, no RW core: returns 0 (no librw binding, L-129 a)");
        return nullptr;
    }
    const std::uint32_t g = Rd32(0x007d3ff8u);
    const std::uint32_t off = g ? Rd32(0x007d4054u) : 0;
    const std::uint32_t freelist = g ? Rd32(g + off + 0xc) : 0;
    const std::uint32_t oldHead = g ? Rd32(g + off) : 0;
    void* const d = RwCore_004c5890();
    ++s_calls;
    const std::uint32_t dv = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(d));
    if (!dv) {
        ++s_fails;
        RwCore_Log("RwTexDictionaryCreate 0x004c5890 call %u on the RW core (RwCore_004c5890): NULL; RwGlobals %08X, module "
                   "offset 0x%X, texdict freelist %08X", s_calls, g, off, freelist);
        return d;
    }
    const int bad = RwCoreBind_CheckTexDict(dv, g, off, oldHead);
    if (bad) ++s_fails;
    RwCore_Log("RwTexDictionaryCreate 0x004c5890 call %u on the RW core (RwCore_004c5890) = %08X; RwGlobals %08X, module "
               "offset 0x%X, texdict freelist %08X, list head %08X -> %08X; self-test %s (%d of 11 checks failed, %u failing "
               "calls so far)", s_calls, dv, g, off, freelist, oldHead, Rd32(g + off), bad ? "FAIL" : "PASS", bad, s_fails);
    return d;
}

} // extern "C"
#endif // MASHED_STANDALONE
