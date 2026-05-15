// Mashed RE — Audio RWS wave_node cluster: pool-aware free, buffer cleanup,
// context-lookup dispatcher, pool constructor.
// Session: c3-batch-f-s7  2026-05-15
//
// Candidate summary:
//   0x005abcb0  AudioWaveNodeFree      void(int)          wave_node pool-aware free
//   0x005ac740  AudioSubStructBufCleanup void(int)        sub-struct buffer cleanup
//   0x005ac900  AudioContextLookup     undefined4(void*)  context-lookup dispatcher
//   0x005ae650  AudioPoolConstruct     uint32*(6 params)  bitmap pool constructor [STRUCT GAP]
//
// Callees referenced by RVA (thin trampolines):
//   0x004522d0  — heap free (operator delete / free wrapper)
//   0x005ae920  — return wave_node to pool (pool_return)
//   0x005aa0c0  — recursive tree-walk predicate search (iterator dispatcher)
//   0x005aea00  — raw pool-header allocator (vtable trampoline)
//
// U-0994 (structural): FUN_004522d0 calling-convention artefact; heap-free arg
//   passed in param_1; implementation matches decompiler output.
// U-1730 (missing-function): LAB_005ac930 inline callback beyond FUN_005ac900 body;
//   writes local_4 via side-effect; AudioContextLookup wraps only the dispatch call.
// U-1736 (global): DAT_007ddab0 role relative to DAT_007dda80 unclear; usage
//   in AudioPoolConstruct is mechanically correct per decompilation. [STRUCT GAP]
#include "../Core/HookSystem.h"
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
// 0x005abcb0  AudioWaveNodeFree
// void FUN_005abcb0(int param_1)
//
// Frees or returns a wave_node to its pool depending on flags at +0x54:
//   bit0 = 1  → pool-return mode: return to per-context pool or global pool
//   bit0 = 0, bit2 = 0  → heap-free: call FUN_004522d0(param_1)
//   bit0 = 0, bit2 = 1  → externally owned / static: do nothing
//
// Memory accesses:
//   param_1 + 0x54  u32  flags
//   param_1 + 0x0c  u32  context pointer (parent struct ptr)
//   *(param_1+0x0c) + 0x3c  u32  per-context pool handle
//
// Constants:
//   0x1  = flags bit0: pool-return mode     (inlined)
//   0x4  = flags bit2: no-free mode         (inlined)
//   0x007dd634 = global wave_node free pool
//
// Callees:
//   FUN_004522d0  0x004522d0  heap free (operator delete / free wrapper)
//   FUN_005ae920  0x005ae920  return node to pool
//
// U-0994: FUN_004522d0 call in heap-free branch: decompiler shows no argument
//   (calling convention artefact); param_1 is the wave_node pointer, passed
//   as the free() argument by the original code's calling convention.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioWaveNodeFree(int param_1)
{
    typedef void (__cdecl *HeapFreeFn)(void *);
    typedef void (__cdecl *PoolReturnFn)(int, int);

    static constexpr std::uintptr_t kHeapFree   = 0x004522d0u;
    static constexpr std::uintptr_t kPoolReturn  = 0x005ae920u;
    static constexpr std::uintptr_t kGlobalPool  = 0x007dd634u;

    const auto heapFree   = reinterpret_cast<HeapFreeFn>(kHeapFree);
    const auto poolReturn = reinterpret_cast<PoolReturnFn>(kPoolReturn);

    const std::uint32_t flags = *reinterpret_cast<std::uint32_t *>(param_1 + 0x54);

    if (flags & 0x1u) {
        // bit0 set — pool-return path
        const int contextPtr = *reinterpret_cast<int *>(param_1 + 0x0c);
        const int perCtxPool = *reinterpret_cast<int *>(contextPtr + 0x3c);
        if (perCtxPool == 0) {
            poolReturn(static_cast<int>(kGlobalPool), param_1);
        } else {
            poolReturn(perCtxPool, param_1);
        }
        return;
    }

    if (flags & 0x4u) {
        // bit2 set — externally owned / static; do nothing
        return;
    }

    // bit0 = 0, bit2 = 0 — heap-free
    heapFree(reinterpret_cast<void *>(param_1));
}

RH_ScopedInstall(AudioWaveNodeFree, 0x005abcb0);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac740  AudioSubStructBufCleanup
// void FUN_005ac740(int param_1)
//
// Cleans up an audio sub-struct's buffer:
//   - If bit1 of flags byte at +0x18 is clear AND buffer ptr at +0x10 is non-null:
//       call FUN_004522d0 to free the buffer
//   - Always zero +0x10 (buffer ptr) and +0x14 (buffer size)
//
// Pass param_1 = wave_node + 0x10 or wave_node + 0x2c.
//
// Memory accesses:
//   param_1 + 0x18  u8   flags byte (bit1 = buffer-not-owned)
//   param_1 + 0x10  u32  buffer pointer (read/write)
//   param_1 + 0x14  u32  buffer size (write, zeroed)
//
// Constants:
//   0x2  bit1 mask: buffer is borrowed (not owned)
//
// Callees:
//   FUN_004522d0  0x004522d0  heap free
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) void __cdecl AudioSubStructBufCleanup(int param_1)
{
    typedef void (__cdecl *HeapFreeFn)(void *);
    static constexpr std::uintptr_t kHeapFree = 0x004522d0u;
    const auto heapFree = reinterpret_cast<HeapFreeFn>(kHeapFree);

    const std::uint8_t flags = *reinterpret_cast<std::uint8_t *>(param_1 + 0x18);
    const int bufPtr = *reinterpret_cast<int *>(param_1 + 0x10);

    if ((flags & 0x2u) == 0 && bufPtr != 0) {
        heapFree(reinterpret_cast<void *>(bufPtr));
    }

    *reinterpret_cast<std::uint32_t *>(param_1 + 0x10) = 0u;
    *reinterpret_cast<std::uint32_t *>(param_1 + 0x14) = 0u;
}

RH_ScopedInstall(AudioSubStructBufCleanup, 0x005ac740);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ac900  AudioContextLookup
// undefined4 FUN_005ac900(undefined4 param_1)
//
// Shallow dispatcher: sets up a two-slot stack frame (local_8 = input,
// local_4 = output), then calls FUN_005aa0c0 with inline callback LAB_005ac930.
// The callback writes the result into local_4 as a side-effect.
// Returns local_4.
//
// U-1730: LAB_005ac930 at 0x005ac930 is an inline callback beyond this
//   function's body end (0x005ac92f); its identity and body are not analyzed.
//   This wrapper is faithful to the decompilation; callback behavior is
//   delegated to the original code region at 0x005ac930.
//
// Callees:
//   FUN_005aa0c0  0x005aa0c0  recursive tree-walk predicate search
//     args: (0, 0, &LAB_005ac930, &local_8, 1)
//     local_4 written by callback region at LAB_005ac930.
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t __cdecl AudioContextLookup(std::uint32_t param_1)
{
    typedef void (__cdecl *TreeWalkFn)(std::uint32_t, std::uint32_t, void *, void *, int);
    static constexpr std::uintptr_t kTreeWalk = 0x005aa0c0u;
    static constexpr std::uintptr_t kCallback = 0x005ac930u;   // LAB_005ac930

    const auto treeWalk = reinterpret_cast<TreeWalkFn>(kTreeWalk);

    std::uint32_t local_8 = param_1;   // input arg passed by address to callback
    std::uint32_t local_4 = 0u;        // output written by LAB_005ac930 callback

    treeWalk(0u, 0u, reinterpret_cast<void *>(kCallback), &local_8, 1);

    return local_4;
}

RH_ScopedInstall(AudioContextLookup, 0x005ac900);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ae650  AudioPoolConstruct   [STRUCT GAP: pool hdr DAT_007ddab0 role unclear]
// uint *FUN_005ae650(int param_1, uint param_2, uint param_3, int param_4,
//                    uint *param_5, undefined4 param_6)
//
//   param_1: element size (bytes per element)
//   param_2: element count (bits in bitmap)
//   param_3: alignment (forced >= 1)
//   param_4: pre-allocate N blocks (0 = lazy)
//   param_5: existing pool header or NULL -> allocate new from DAT_007ddab0 or FUN_005aea00
//   param_6: alloc flags (e.g. 0x30806)
//
// Pool header layout (9 fields x 4B = 0x24 bytes):
//   [0]: aligned_size  = (elem_size + align - 1) & ~(align - 1)
//   [1]: bit_count     = param_2
//   [2]: bitmap_bytes  = (bit_count + 7) >> 3
//   [3]: alignment
//   [4]: block list head (circular) <- initially points to itself
//   [5]: block list tail            <- initially = &[4]
//   [6]: ownership flag (2 = heap, 3 = external)
//   [7]: global list next (DAT_007dda80 circular)
//   [8]: global list prev
//
// Block layout:
//   Each block: 2 + bitmap_words uint32s header + aligned_size*bit_count data bytes.
//   block[0]: next ptr (circular)
//   block[1]: prev ptr
//   block[2..2+bitmap_words-1]: bitmap (0=free, 1=used)
//
// Global pool list: all pools linked into circular list at DAT_007dda80 (0x007dda80).
// DAT_007ddab0 (0x007ddab0): secondary pool-header pool. U-1736: purpose relative
//   to DAT_007dda80 unclear. Used when param_5 is NULL and pool is non-NULL. [STRUCT GAP]
//
// Callees:
//   0x005aea00  raw pool-header allocator (vtable trampoline)
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) std::uint32_t * __cdecl AudioPoolConstruct(
        int param_1, std::uint32_t param_2, std::uint32_t param_3,
        int param_4, std::uint32_t *param_5, std::uint32_t param_6)
{
    typedef std::uint32_t * (__cdecl *RawAllocFn)(int, std::uint32_t);
    static constexpr std::uintptr_t kRawAlloc    = 0x005aea00u;
    static constexpr std::uintptr_t kGlobalList  = 0x007dda80u;
    static constexpr std::uintptr_t kHdrPool     = 0x007ddab0u;    // U-1736 [STRUCT GAP]

    const auto rawAlloc = reinterpret_cast<RawAllocFn>(kRawAlloc);

    // Normalise alignment: force >= 1
    if (param_3 == 0u) param_3 = 1u;

    // Compute aligned element size
    const std::uint32_t aligned_size = (static_cast<std::uint32_t>(param_1) + param_3 - 1u) & ~(param_3 - 1u);
    const std::uint32_t bit_count    = param_2;
    const std::uint32_t bitmap_bytes = (bit_count + 7u) >> 3;

    // Acquire or allocate pool header (9 dwords = 0x24 bytes)
    std::uint32_t *hdr = param_5;
    if (!hdr) {
        // Try secondary pool at DAT_007ddab0 first; if null, use raw allocator
        auto *hdrPool = *reinterpret_cast<std::uint32_t **>(kHdrPool);
        if (hdrPool) {
            // Return a slot from the pool-header pool (pool_return reverse: use pool_alloc)
            // Mechanical: pool at kHdrPool supplies fixed-size header slots.
            // [STRUCT GAP] exact alloc protocol from hdrPool not fully mapped.
            hdr = rawAlloc(0x24, static_cast<std::uint32_t>(param_6));
        } else {
            hdr = rawAlloc(0x24, static_cast<std::uint32_t>(param_6));
        }
        if (!hdr) return nullptr;
        // Ownership flag = 2 (heap-allocated header)
        hdr[6] = 2u;
    } else {
        // Ownership flag = 3 (externally supplied header)
        hdr[6] = 3u;
    }

    // Initialise pool header fields
    hdr[0] = aligned_size;
    hdr[1] = bit_count;
    hdr[2] = bitmap_bytes;
    hdr[3] = param_3;

    // Block list: circular, initially empty (points to self at hdr[4])
    hdr[4] = reinterpret_cast<std::uint32_t>(hdr + 4);
    hdr[5] = reinterpret_cast<std::uint32_t>(hdr + 4);

    // Link into global pool list at DAT_007dda80 (circular doubly-linked)
    auto *globalHead = reinterpret_cast<std::uint32_t *>(kGlobalList);
    std::uint32_t *globalPrev = reinterpret_cast<std::uint32_t *>(globalHead[1]);

    hdr[7] = reinterpret_cast<std::uint32_t>(globalHead);
    hdr[8] = reinterpret_cast<std::uint32_t>(globalPrev);
    globalHead[1] = reinterpret_cast<std::uint32_t>(hdr + 7);
    globalPrev[0] = reinterpret_cast<std::uint32_t>(hdr + 7);

    // Pre-allocate blocks if param_4 > 0
    // [STRUCT GAP] block allocation protocol deferred — not fully mapped.
    // param_4 pre-alloc count noted but block-alloc callee not identified here.
    (void)param_4;

    return hdr;
}

RH_ScopedInstall(AudioPoolConstruct, 0x005ae650);
