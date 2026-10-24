// Mashed RE — Audio/AudioPool.cpp
// The bitmap pool family (0x005ae650 / 0x005ae800 / 0x005ae780 / 0x005ae920) and the node-list family on the
// 0x009146c0 pool (0x005addd0 / 0x005ade10 / 0x005ade90), split out of Audio/AudioRws.cpp (D4-AUDIO-RWS slice 2,
// 2026-10-24) so they link into the exe. In both rsp files. Calls inside the family go by name.
// Bytes: verify/audiorws_20261024/disasm_rws2.txt. PREREG: verify/audiorws_20261024/PREREG_rws2.md.
//
// Exe arm (MASHED_STANDALONE): raw alloc / free are the CRT heap (the declared deviation of Audio/AudioMemory.cpp's
// AudioRawAlloc / AudioRawFree), and the three globals are private zero storage. All three are BSS until the audio
// open (0x004627f0 -> 0x005a5f00 -> init table 0x00632ba8; verify/audiolink_20261023/POOL_CONSTRUCTOR.md), so zero
// is the original's pre-open state. Nothing in the exe calls the family yet. .asi arm: the original's addresses.
#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstdlib>

namespace {
// Raw allocator: the RwGlobals slots reached through 0x005aea00 (alloc, cdecl (size, tag)) and 0x004522d0
// (free, cdecl (ptr)).
#ifdef MASHED_STANDALONE
std::uint32_t* s_audioHdrPool_007ddab0 = nullptr;      // stands in for DAT_007ddab0 (header pool pointer)
std::uint32_t  s_audioPoolList_007dda80[2] = {};       // stands in for DAT_007dda80 / 0x007dda84 (list head)
std::uint32_t  s_audioNodePool_009146c0[9] = {};       // stands in for DAT_009146c0 (node pool header, 0x24 bytes)
std::uint32_t** HdrPoolSlot()  { return &s_audioHdrPool_007ddab0; }
std::uint32_t*  PoolListHead() { return s_audioPoolList_007dda80; }
std::uint32_t*  NodePool()     { return s_audioNodePool_009146c0; }
std::uint32_t* PoolRawAlloc(std::uint32_t size, std::uint32_t) { return static_cast<std::uint32_t*>(std::malloc(size)); }
void           PoolRawFree(void* raw)                         { std::free(raw); }
#else
std::uint32_t** HdrPoolSlot()  { return reinterpret_cast<std::uint32_t**>(0x007ddab0u); }
std::uint32_t*  PoolListHead() { return reinterpret_cast<std::uint32_t*>(0x007dda80u); }
std::uint32_t*  NodePool()     { return reinterpret_cast<std::uint32_t*>(0x009146c0u); }
std::uint32_t* PoolRawAlloc(std::uint32_t size, std::uint32_t tag) {
    typedef std::uint32_t* (__cdecl *AllocFn)(std::uint32_t, std::uint32_t);
    return reinterpret_cast<AllocFn>(0x005aea00u)(size, tag);
}
void PoolRawFree(void* raw) {
    typedef void (__cdecl *FreeFn)(void*);
    reinterpret_cast<FreeFn>(0x004522d0u)(raw);
}
#endif
}  // namespace

// ---------------------------------------------------------------------------
// 0x005ae800  FUN_005ae800 — bitmap pool block allocator
// uint __cdecl FUN_005ae800(int *h, int flags)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn), disasm 0x005ae800..0x005ae912.
// Header h: [0] elem, [1] cap, [2] nbytes (bitmap bytes), [3] align, [4]/[5] block-list
// sentinel. Block: [0] next, [1] prev, bitmap bytes at +8.
//
// Per block the scan carries a COUNTDOWN seeded from h[1] (0x005ae828/0x005ae82f, kept
// in the param slot [esp+0x18]):
//   byte == 0xff            -> countdown -= 8, next byte        (0x005ae839, 0x005ae90d)
//   countdown == 0          -> next byte (not next block)       (0x005ae848 test/jbe = ==0)
//   bit set                 -> bit++, countdown--, up to 8 bits (0x005ae85b..0x005ae864)
//   bit clear (0x80 >> bit) -> set it, addr = ((blk + align + nbytes + 7) & ~(align-1))
//                              + (byte*8 + bit) * elem          (0x005ae868..0x005ae888)
//                              addr == 0 keeps scanning bytes   (0x005ae88c)
// The countdown is unsigned and only tested for zero, so a 0xff byte taken at
// countdown < 8 wraps it and later bytes are scanned again (faithful).
// No slot: RawAlloc(cap*elem + align + nbytes + 8, flags) (0x005ae8ac..0x005ae8c2); NULL
// -> 0 (0x005ae8d0). Bitmap zeroed (rep stos, 0x005ae8da/0x005ae8e1), block HEAD-inserted:
// new[1] = sentinel, new[0] = old head, old_head[1] = new, sentinel[0] = new
// (0x005ae8e3..0x005ae8f3); bitmap byte 0 = 0x80 (0x005ae8f5); returns the slot-0 address
// (new + align + nbytes + 7) & ~(align-1) (0x005ae8f9..0x005ae905).
// The old port scanned all 8 bits of every byte regardless of h[1] and tail-inserted.
// ---------------------------------------------------------------------------
// 0x005ae800
extern "C" __declspec(dllexport) unsigned int __cdecl AudioPoolBlockAlloc(int* param_1, int param_2)
{
    std::uint32_t* const h = reinterpret_cast<std::uint32_t*>(param_1);
    std::uint32_t* const sentinel = h + 4;                       // 0x005ae811
    const std::uint32_t nbytes = h[2];                           // 0x005ae80e

    std::uint32_t* blk = reinterpret_cast<std::uint32_t*>(h[4]);  // 0x005ae80b
    while (blk != sentinel) {                                    // 0x005ae818 / 0x005ae8a0
        std::uint32_t countdown = h[1];                          // 0x005ae828
        std::uint8_t* const bitmap = reinterpret_cast<std::uint8_t*>(blk) + 8;
        for (std::uint32_t byte_idx = 0; byte_idx < nbytes; ++byte_idx) {   // 0x005ae82d / 0x005ae893
            std::uint8_t bits = bitmap[byte_idx];                // 0x005ae835
            if (bits == 0xffu) {                                 // 0x005ae839
                countdown -= 8u;                                 // 0x005ae90d
                continue;
            }
            for (std::uint32_t bit = 0; bit < 8u; ++bit) {       // 0x005ae842 / 0x005ae85d
                if (countdown == 0u) break;                      // 0x005ae848
                const std::uint8_t mask = static_cast<std::uint8_t>(0x80u >> bit);   // 0x005ae851
                if ((bits & mask) == 0u) {                       // 0x005ae853
                    bits = static_cast<std::uint8_t>(bits | mask);                   // 0x005ae868
                    bitmap[byte_idx] = bits;                     // 0x005ae86d
                    const std::uint32_t idx = bit + byte_idx * 8u;                   // 0x005ae86a
                    const std::uint32_t align = h[3];            // 0x005ae875
                    const std::uint32_t addr =
                        ((reinterpret_cast<std::uint32_t>(blk) + align + nbytes + 7u) & ~(align - 1u))
                        + idx * h[0];                            // 0x005ae871..0x005ae888
                    if (addr != 0u) return addr;                 // 0x005ae88c
                    break;                                       // addr 0 -> next byte (0x005ae88e)
                }
                --countdown;                                     // 0x005ae85c
            }
        }
        blk = reinterpret_cast<std::uint32_t*>(blk[0]);          // 0x005ae89b
    }

    // 0x005ae8ac: no free slot, allocate a new block.
    const std::uint32_t size = h[1] * h[0] + h[3] + nbytes + 8u;  // 0x005ae8ac..0x005ae8bd
    std::uint32_t* const nb = PoolRawAlloc(size, static_cast<std::uint32_t>(param_2));   // 0x005ae8c2
    if (nb == nullptr) return 0u;                                // 0x005ae8d0

    std::uint8_t* const nbm = reinterpret_cast<std::uint8_t*>(nb) + 8;
    for (std::uint32_t i = 0; i < nbytes; ++i) nbm[i] = 0u;     // 0x005ae8da / 0x005ae8e1

    nb[1] = reinterpret_cast<std::uint32_t>(sentinel);           // 0x005ae8e5
    nb[0] = sentinel[0];                                         // 0x005ae8e3 / 0x005ae8e8
    reinterpret_cast<std::uint32_t*>(sentinel[0])[1] = reinterpret_cast<std::uint32_t>(nb);   // 0x005ae8ea / 0x005ae8f0
    sentinel[0] = reinterpret_cast<std::uint32_t>(nb);           // 0x005ae8f3
    nbm[0] = 0x80u;                                              // 0x005ae8f5

    const std::uint32_t align = h[3];                            // 0x005ae8f9
    return (reinterpret_cast<std::uint32_t>(nb) + align + nbytes + 7u) & ~(align - 1u);   // 0x005ae8fc..0x005ae905
}

RH_ScopedInstall(AudioPoolBlockAlloc, 0x005ae800);  // re-enabled 2026-05-24 c3-audio-b

// 0x005ae920  FUN_005ae920  (0xd3 bytes, 0x005ae920..0x005ae9f2)
// void __cdecl FUN_005ae920(uint *h, uint item)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn).
//   0x005ae93d  base = blk + nbytes + 8   (h[2] at 0x005ae92b)
//   0x005ae941  item <  base            -> next block (unsigned, jb)
//   0x005ae945  item <= base + cap*elem -> found      (unsigned, jbe: end INCLUSIVE)
//   0x005ae95d  idx = (item - base) / elem (unsigned div)
//   0x005ae966..0x005ae98b  bitmap[idx >> 3] &= ~(0x80 >> (idx & 7))
//   0x005ae98f..0x005ae99b  unlink blk
//   0x005ae99e  head = sentinel[0] (after the unlink)
//   0x005ae9a0  list empty            -> relink (no compaction test)
//   0x005ae9a4  (byte h[6] & 2) == 0  -> relink
//   0x005ae9aa..0x005ae9bd  sum of the bitmap BYTE VALUES (not a popcount); nbytes 0 skips
//   0x005ae9c1  sum != 0 -> relink, else 0x005ae9c4 RwFree(blk) via 0x004522d0
//   relink (0x005ae9d1 / 0x005ae9e2): blk[0] = head, blk[1] = sentinel,
//          sentinel[0][1] = blk, sentinel[0] = blk
// The old port used an exclusive end and tested compaction even when the block was the
// list's only member.
extern "C" __declspec(dllexport) void __cdecl AudioPoolFree(
        std::uint32_t* param_1, std::uint32_t param_2)
{
    std::uint32_t* const sentinel = param_1 + 4;                  // 0x005ae92e
    const std::uint32_t nbytes = param_1[2];                      // 0x005ae92b

    std::uint32_t* blk = reinterpret_cast<std::uint32_t*>(param_1[4]);   // 0x005ae928
    for (;;) {
        if (blk == sentinel) return;                              // 0x005ae931 / 0x005ae954
        const std::uint32_t base = nbytes + reinterpret_cast<std::uint32_t>(blk) + 8u;   // 0x005ae93d
        if (param_2 >= base && param_2 <= param_1[1] * param_1[0] + base)               // 0x005ae941 / 0x005ae94e
            break;
        blk = reinterpret_cast<std::uint32_t*>(blk[0]);           // 0x005ae952
    }

    const std::uint32_t base = nbytes + reinterpret_cast<std::uint32_t>(blk) + 8u;
    const std::uint32_t idx = (param_2 - base) / param_1[0];      // 0x005ae95d..0x005ae961
    std::uint8_t* const bitmap = reinterpret_cast<std::uint8_t*>(blk) + 8;
    bitmap[idx >> 3] = static_cast<std::uint8_t>(
        bitmap[idx >> 3] & static_cast<std::uint8_t>(~(0x80u >> (idx & 7u))));          // 0x005ae966..0x005ae98b

    reinterpret_cast<std::uint32_t*>(blk[1])[0] = blk[0];         // 0x005ae98f..0x005ae994
    reinterpret_cast<std::uint32_t*>(blk[0])[1] = blk[1];         // 0x005ae996..0x005ae99b

    const std::uint32_t head = sentinel[0];                       // 0x005ae99e
    if (head != reinterpret_cast<std::uint32_t>(sentinel) &&      // 0x005ae9a0
        (*reinterpret_cast<const std::uint8_t*>(param_1 + 6) & 2u) != 0u) {   // 0x005ae9a4
        std::uint32_t sum = 0;                                    // 0x005ae9aa
        for (std::uint32_t i = 0; i < nbytes; ++i) sum += bitmap[i];          // 0x005ae9b2..0x005ae9bd
        if (sum == 0u) {                                          // 0x005ae9bf
            PoolRawFree(blk);                                     // 0x005ae9c4
            return;
        }
    }

    blk[0] = head;                                                // 0x005ae9d1 / 0x005ae9e2
    blk[1] = reinterpret_cast<std::uint32_t>(sentinel);           // 0x005ae9d3 / 0x005ae9e4
    reinterpret_cast<std::uint32_t*>(sentinel[0])[1] = reinterpret_cast<std::uint32_t>(blk);   // 0x005ae9d6 / 0x005ae9e7
    sentinel[0] = reinterpret_cast<std::uint32_t>(blk);           // 0x005ae9db / 0x005ae9ec
}

RH_ScopedInstall(AudioPoolFree, 0x005ae920);  // re-enabled 2026-05-24 c3-audio-b

// ---------------------------------------------------------------------------
// 0x005ae780  FUN_005ae780 — bitmap pool destructor  (0x005ae780..0x005ae7d9)
//
// Drains the circular block list at param_1+0x10: each block is unlinked
// (0x005ae790..0x005ae79d, both links re-read after the first store) and freed through
// 0x004522d0 (0x005ae7a0); the head is re-read from the sentinel (0x005ae7a5).
// Byte +0x18 bit 0 set -> return (0x005ae7ae / 0x005ae7b2). Otherwise DAT_007ddab0
// (0x005ae7b4) == param_1 or == 0 -> free the header through 0x004522d0 (0x005ae7cf),
// else 0x005ae920(DAT_007ddab0, param_1) (0x005ae7c3).
// EAX differs by path (free result, or param_1+0x10 on the bit-0 path), so void.
// ---------------------------------------------------------------------------
// 0x005ae780
extern "C" __declspec(dllexport) void __cdecl AudioPoolDestroy(int param_1)
{
    int* sentinel = reinterpret_cast<int*>(param_1 + 0x10);      // 0x005ae789
    int* head = reinterpret_cast<int*>(sentinel[0]);              // 0x005ae786

    while (head != sentinel) {                                    // 0x005ae78c / 0x005ae7aa
        *reinterpret_cast<int*>(head[1]) = head[0];               // 0x005ae790..0x005ae796
        *reinterpret_cast<int*>(head[0] + 4) = head[1];           // 0x005ae798..0x005ae79d
        PoolRawFree(head);                                        // 0x005ae7a0
        head = reinterpret_cast<int*>(sentinel[0]);               // 0x005ae7a5
    }

    const std::uint8_t flags = *reinterpret_cast<const std::uint8_t*>(param_1 + 0x18);   // 0x005ae7ae
    if ((flags & 1u) == 0u) {
        std::uint32_t* const secondary = *HdrPoolSlot();          // 0x005ae7b4
        std::uint32_t* const hdr = reinterpret_cast<std::uint32_t*>(param_1);
        if (secondary != nullptr && secondary != hdr) {           // 0x005ae7b9 / 0x005ae7bd
            AudioPoolFree(secondary, static_cast<std::uint32_t>(param_1));   // 0x005ae7c3
        } else {
            PoolRawFree(hdr);                                     // 0x005ae7cf
        }
    }
}

RH_ScopedInstall(AudioPoolDestroy, 0x005ae780);  // re-enabled 2026-05-24 c3-audio-b

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ae650  AudioPoolConstruct
// uint *FUN_005ae650(int elem, uint count, uint align, int prealloc, uint *hdr, int flags)
//
// Re-ported from the bytes 2026-10-12 (negctl re-earn), disasm 0x005ae650..0x005ae773.
//   0x005ae654  align == 0 -> 1
//   0x005ae661  hdr != NULL -> hdr[6] = 3 (0x005ae6a2)
//               hdr == NULL -> DAT_007ddab0 != 0 ? FUN_005ae800(DAT_007ddab0, 0x30800)
//                                                (0x005ae669..0x005ae678)
//                                              : FUN_005aea00(0x24, flags) (0x005ae67f..0x005ae686)
//               NULL -> return 0 (0x005ae694); else hdr[6] = 2 (0x005ae699)
//   0x005ae6a9..0x005ae6d8  hdr[0] = (elem + align - 1) & ~(align - 1), hdr[1] = count,
//               hdr[2] = (count + 7) >> 3, hdr[3] = align, hdr[4] = hdr[5] = &hdr[4]
//   0x005ae6db  prealloc counted down as an unsigned dword until 0 (dec/jne 0x005ae736):
//     size = hdr[0]*count + nbytes + align + 8 (0x005ae6dd..0x005ae6e2)
//     blk = FUN_005aea00(size, flags) (0x005ae6f4); NULL -> FUN_005ae780(hdr), return 0
//       (0x005ae764)
//     blk[1] = 0, blk[0] = 0 (0x005ae702/0x005ae709), then HEAD insert: blk[1] = &hdr[4],
//     blk[0] = hdr[4], hdr[4][1] = blk, hdr[4] = blk (0x005ae70f..0x005ae722);
//     bitmap zeroed, nbytes bytes at blk+8 (0x005ae729/0x005ae730)
//   0x005ae73d..0x005ae75b  global pool list, DAT_007dda80 is a POINTER variable (head):
//     hdr[8] = 0x007dda80, hdr[7] = DAT_007dda80, DAT_007dda80[1] = &hdr[7] (re-read of the
//     global at 0x005ae750), DAT_007dda80 = &hdr[7]
//   returns hdr (0x005ae760)
// The old port always used FUN_005aea00 for the header, skipped the prealloc loop, and
// treated 0x007dda80 as a list node (tail insert through 0x007dda84).
// Exe arm: the head is private zero storage, and 0x005ae5b0 (which self-links it before the first construct) is
// unported, so a call before that port would store through NULL at 0x005ae758. No exe code calls this.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t * __cdecl AudioPoolConstruct(
        int param_1, std::uint32_t param_2, std::uint32_t param_3,
        int param_4, std::uint32_t *param_5, std::uint32_t param_6)
{
    std::uint32_t align = param_3;
    if (align == 0u) align = 1u;                                   // 0x005ae658 / 0x005ae65c

    std::uint32_t *hdr = param_5;                                  // 0x005ae661
    if (hdr == nullptr) {
        std::uint32_t *const hdrPool = *HdrPoolSlot();             // 0x005ae669    U-1736
        if (hdrPool != nullptr)
            hdr = reinterpret_cast<std::uint32_t *>(
                AudioPoolBlockAlloc(reinterpret_cast<int *>(hdrPool), 0x30800));   // 0x005ae678
        else
            hdr = PoolRawAlloc(0x24u, param_6);                    // 0x005ae686
        if (hdr == nullptr) return nullptr;                        // 0x005ae690
        hdr[6] = 2u;                                               // 0x005ae699
    } else {
        hdr[6] = 3u;                                               // 0x005ae6a2
    }

    const std::uint32_t elem   = (static_cast<std::uint32_t>(param_1) + align - 1u) & ~(align - 1u);   // 0x005ae6ad..0x005ae6ba
    const std::uint32_t nbytes = (param_2 + 7u) >> 3;             // 0x005ae6c0 / 0x005ae6c6
    std::uint32_t *const sentinel = hdr + 4;                       // 0x005ae6c3

    hdr[0] = elem;                                                 // 0x005ae6cb
    hdr[1] = param_2;                                              // 0x005ae6cd
    hdr[3] = align;                                                // 0x005ae6d0
    hdr[2] = nbytes;                                               // 0x005ae6d3
    sentinel[0] = reinterpret_cast<std::uint32_t>(sentinel);       // 0x005ae6d6
    hdr[5] = reinterpret_cast<std::uint32_t>(sentinel);            // 0x005ae6d8

    std::uint32_t remaining = static_cast<std::uint32_t>(param_4);
    if (remaining != 0u) {                                         // 0x005ae6c9 / 0x005ae6db
        const std::uint32_t size = elem * param_2 + nbytes + align + 8u;   // 0x005ae6dd..0x005ae6e2
        do {
            std::uint32_t *const blk = PoolRawAlloc(size, param_6);   // 0x005ae6f4
            if (blk == nullptr) {                                  // 0x005ae700
                AudioPoolDestroy(static_cast<int>(reinterpret_cast<std::uintptr_t>(hdr)));   // 0x005ae765
                return nullptr;                                    // 0x005ae76d
            }
            blk[1] = 0u;                                           // 0x005ae702
            blk[0] = 0u;                                           // 0x005ae709
            blk[1] = reinterpret_cast<std::uint32_t>(sentinel);    // 0x005ae711
            blk[0] = sentinel[0];                                  // 0x005ae70f / 0x005ae714
            reinterpret_cast<std::uint32_t *>(sentinel[0])[1] = reinterpret_cast<std::uint32_t>(blk);   // 0x005ae716 / 0x005ae71d
            sentinel[0] = reinterpret_cast<std::uint32_t>(blk);    // 0x005ae722
            std::uint8_t *const bm = reinterpret_cast<std::uint8_t *>(blk) + 8;
            for (std::uint32_t i = 0; i < nbytes; ++i) bm[i] = 0u; // 0x005ae729 / 0x005ae730
        } while (--remaining != 0u);                               // 0x005ae736 / 0x005ae73b
    }

    std::uint32_t *volatile *const gHead = reinterpret_cast<std::uint32_t *volatile *>(PoolListHead());
    hdr[8] = reinterpret_cast<std::uint32_t>(PoolListHead());     // 0x005ae746
    hdr[7] = reinterpret_cast<std::uint32_t>(*gHead);              // 0x005ae73d / 0x005ae74e
    (*gHead)[1] = reinterpret_cast<std::uint32_t>(hdr + 7);        // 0x005ae750 / 0x005ae758
    *gHead = hdr + 7;                                              // 0x005ae75b

    return hdr;                                                    // 0x005ae760
}

RH_ScopedInstall(AudioPoolConstruct, 0x005ae650);  // re-enabled 2026-05-24 c3-audio-b

// ---------------------------------------------------------------------------
// 0x005addd0  FUN_005addd0  (49 bytes, 0x005addd0..0x005ade00)
//   param_1 = pointer to list head variable, param_2 = payload
//   0x005addd0  node = FUN_005ae800(&DAT_009146c0, 0x30804); NULL -> ret with EAX 0 (0x005adde6)
//   0x005addeb  node[2] = param_2
//   0x005addf2  edx = *param_1 ; node[1] = param_1 ; node[0] = edx
//   0x005addf9  edx = *param_1 RE-READ ; edx[1] = node
//   0x005addfe  *param_1 = node ; ret with EAX = node
// Returns the node (was void; static evidence, the harness does not read it).
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int* __cdecl AudioListInsertHead(
        int* param_1, int param_2)
{
    int* const node = reinterpret_cast<int*>(
        AudioPoolBlockAlloc(reinterpret_cast<int*>(NodePool()), 0x30804));   // 0x005addd0..0x005addda
    if (node == nullptr) return nullptr;                           // 0x005adde2 / 0x005adde6

    volatile int* const head = param_1;
    node[2] = param_2;                                             // 0x005addeb
    const int oldHead = head[0];                                   // 0x005addf2
    node[1] = reinterpret_cast<int>(param_1);                      // 0x005addf4
    node[0] = oldHead;                                             // 0x005addf7
    reinterpret_cast<int*>(head[0])[1] = reinterpret_cast<int>(node);   // 0x005addf9 / 0x005addfb
    head[0] = reinterpret_cast<int>(node);                         // 0x005addfe
    return node;
}

RH_ScopedInstall(AudioListInsertHead, 0x005addd0);  // re-enabled 2026-05-24 c3-audio-a

// ---------------------------------------------------------------------------
// 0x005ade10  FUN_005ade10  (66 bytes, 0x005ade10..0x005ade51)
//   param_1 = list sentinel, param_2 = data value to remove. Returns param_1 when found, NULL if not.
//   0x005ade19  node = param_1[1]
//   0x005ade1c  node[2] == param_2 -> found        (data compared BEFORE the sentinel test)
//   0x005ade21  node == param_1 -> return 0 (0x005ade4e)
//   0x005ade25  node = node[1], loop
//   found: node[0][1] = node[1] (0x005ade38), node[1][0] = node[0] (both re-read, 0x005ade3b..0x005ade40),
//          0x005ae920(&DAT_009146c0, node) (0x005ade42), return param_1 (0x005ade4a)
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int* __cdecl AudioListRemoveByValue(
        int* param_1, int param_2)
{
    int* node = reinterpret_cast<int*>(param_1[1]);                // 0x005ade19
    while (true) {
        if (node[2] == param_2) {                                  // 0x005ade1c / 0x005ade28
            *reinterpret_cast<int*>(node[0] + 4) = node[1];        // 0x005ade2d..0x005ade38
            *reinterpret_cast<int*>(node[1]) = node[0];            // 0x005ade3b..0x005ade40
            AudioPoolFree(NodePool(), reinterpret_cast<std::uint32_t>(node));   // 0x005ade42
            return param_1;                                        // 0x005ade4a
        }
        if (node == param_1) return nullptr;                       // 0x005ade21 / 0x005ade4e
        node = reinterpret_cast<int*>(node[1]);                    // 0x005ade25
    }
}

RH_ScopedInstall(AudioListRemoveByValue, 0x005ade10);  // re-enabled 2026-05-24 c3-audio-a

// ---------------------------------------------------------------------------
// 0x005ade90  FUN_005ade90  (50 bytes, 0x005ade90..0x005adec1)
//   param_1 = list sentinel. While param_1[1] != param_1 (0x005ade95 / 0x005adeb6, re-read each pass):
//     node = param_1[1]; node[0][1] = node[1]; node[1][0] = node[0] (re-read); 0x005ae920(&DAT_009146c0, node)
//   Exit leaves EAX = param_1[1] == param_1 (0x005adeb6..0x005adec1), so the port returns it (was void;
//   static evidence, the harness does not read it).
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int* __cdecl AudioListDrain(int* param_1)
{
    while (param_1[1] != reinterpret_cast<int>(param_1)) {         // 0x005ade95 / 0x005adebc
        int* const node = reinterpret_cast<int*>(param_1[1]);
        *reinterpret_cast<int*>(node[0] + 4) = node[1];            // 0x005ade9c..0x005adea7
        *reinterpret_cast<int*>(node[1]) = node[0];                // 0x005adeaa..0x005adeaf
        AudioPoolFree(NodePool(), reinterpret_cast<std::uint32_t>(node));   // 0x005adeb1
    }
    return reinterpret_cast<int*>(param_1[1]);                     // 0x005adeb6
}

RH_ScopedInstall(AudioListDrain, 0x005ade90);  // re-enabled 2026-05-24 c3-audio-a
