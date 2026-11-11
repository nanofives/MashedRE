// HUDINIT-A (L-111 step 1, chain 1 of 0x00427ca0 FontText_HudInit: the Rt2d open 0x00552b60): exe-only BINDING of
// rva 0x004cc820 RwFreeListCreate (cdecl (entrySize, entriesPerBlock, alignment, blocksToPreallocate, header, hint)).
// Not the matched source: re/match/rwl01/004cc820.c is a best 74.5% (regs 78.4%) and does not run. L0 ruling 2026-11-10:
// this counts as a binding (like a librw binding), not a hand transcription. It binds onto the exe's freelist, the one RwGlobals +0x118 / +0x11c implement
// under MASHED_ORIG_TOP_RWMOD=1 (LibRw/RwApiBind.cpp RwGlobalsFreeListAlloc / RwGlobalsFreeListFree): one allocation per
// entry, entry size from header +0x0 and alignment from +0xc, no block pool.
// It writes the header the way the original does, cited against re/match/rwl01/004cc820.c (the decompiled body):
//  - alignment 0 -> 4;
//  - header 0 -> a 0x24-byte header from RwGlobals +0x108 (original: from the freelist of freelists DAT_007d45fc through
//    +0x118 when that is non-zero, else +0x108), +0x18 = 2; a caller's header -> +0x18 = 3;
//  - +0x0 = (entrySize + alignment - 1) & ~(alignment - 1), +0x4 = entriesPerBlock, +0x8 = (entriesPerBlock + 7) >> 3,
//    +0xc = alignment, +0x10 / +0x14 = the empty block list (self-linked).
// Two declared deviations, both with no reader on the exe's path:
//  - no blocks are preallocated (the original allocates blocksToPreallocate blocks when DAT_006182b0 != 0). The binding's
//    +0x118 never reads a block.
//  - the header is not linked into the global freelist list at DAT_007d45cc (+0x1c / +0x20 stay 0). In the exe that list
//    head is 0: 0x007d45cc is BSS (past .data's 0x4d000 raw bytes from 0x005ea000) and no exe code writes it, while the
//    original's RwEngineInit makes it a self-linked list. The original links through *(head + 4), a null read here.
// Every caller on this path passes its own static header: 0x00557250 (0x007dc864, 0x007dc840), 0x005540d0 (0x007dc77c),
// 0x00556d20 (0x007dc7e8), 0x00555360 (0x007dc7a0, 0x007dc7c4), 0x005571e0 (0x007dc81c), all through the thunk 0x004cc9e0.
#ifdef MASHED_STANDALONE
#include <cstdint>
#include <cstdio>

namespace {
void FlLog(const std::uint32_t* h, std::uint32_t entrySize, std::uint32_t align) {
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x004cc820 RwFreeListCreate(0x%X, 0x%X, align %u): header %p = {0x%X, 0x%X, 0x%X, 0x%X, ..., "
                        "flags %u} (HUDINIT-A)\n", entrySize, h ? h[1] : 0u, align, static_cast<const void*>(h), h ? h[0] : 0u,
                     h ? h[1] : 0u, h ? h[2] : 0u, h ? h[3] : 0u, h ? h[6] : 0u);
        std::fclose(f);
    }
}
}  // namespace

extern "C" unsigned int* PortTop_RwFreeListCreate_004cc820(int entrySize, unsigned int entriesPerBlock, unsigned int alignment,
                                                           int blocksToPreallocate, unsigned int* header, unsigned int hint) {
    (void)blocksToPreallocate;
    if (alignment == 0) alignment = 4;
    if (header == nullptr) {
        using MallocFn = void* (__cdecl*)(unsigned int, unsigned int);
        const std::uintptr_t rwg = *reinterpret_cast<const std::uintptr_t*>(static_cast<std::uintptr_t>(0x007d3ff8u));
        if (!rwg) return nullptr;
        const MallocFn m = *reinterpret_cast<const MallocFn*>(rwg + 0x108u);
        header = m ? static_cast<unsigned int*>(m(0x24u, hint & 0xff0000u)) : nullptr;
        if (header == nullptr) return nullptr;
        header[6] = 2;
    } else {
        header[6] = 3;
    }
    const unsigned int size = (static_cast<unsigned int>(entrySize) - 1u + alignment) & ~(alignment - 1u);
    unsigned int* const list = header + 4;
    header[0] = size;
    header[1] = entriesPerBlock;
    header[2] = (entriesPerBlock + 7u) >> 3;
    header[3] = alignment;
    list[0] = static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(list));
    list[1] = static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(list));
    header[7] = 0;
    header[8] = 0;
    FlLog(header, static_cast<unsigned int>(entrySize), alignment);
    return header;
}
#endif  // MASHED_STANDALONE
