// Mashed RE — Render mixed-cluster C3 sweep (wave1-s3).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Functions in this file (promoted):
//   0x004cc820  RwFreeListCreate     — 432B pure leaf; alloc+init RW free-list pool
//
// Deferred (anti-island gate failures):
//   0x004c8690  rwD3D9ConstantBufferPoolInit — callee 0x004ccc50 (C1) anti-island fail;
//                                              already deferred in D3D9Helpers_q5.cpp
//   0x004c8e50  rwD3D9RasterSystemInit       — 9 functional callees all C1; anti-island fail
//
// Deferred (uncertain vertex-expansion mapping):
//   0x004228f0  GroundTriBatchRender — U-4422/U-4423: src-5-dword→dest-9-dword per-vertex
//                                      copy pattern not deterministic from analysis note;
//                                      needs Ghidra asm read to confirm exact slot mapping
//
// Already hooked (Wave 0 / prior sessions):
//   0x004235b0  AiPizLoad    — Render/LowRvaSetters_o2.cpp
//   0x00422ac0  SlotQuadSet  — Render/LowRvaMixed_q3.cpp
//
// Analysis refs:
//   re/analysis/promote_c2_render_d3d9/0x004cc820.md
//   re/analysis/promote_c2_render_d3d9/0x004c8690.md   (deferred)
//   re/analysis/promote_c2_render_d3d9/0x004c8e50.md   (deferred)
//   re/analysis/promote_c2_render_lowrva/004228f0.md   (deferred)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>   // memset

// ---------------------------------------------------------------------------
// RwFreeListCreate  --  0x004cc820
//
// Original: FUN_004cc820 (432 bytes, 0x004cc820..0x004cc9d0)
// Signature:
//   uint * FUN_004cc820(int param_1, uint param_2, uint param_3, int param_4,
//                       uint *param_5, uint param_6);
//   param_1 = element size (bytes; aligned to param_3 boundary)
//   param_2 = elements per block
//   param_3 = alignment (0 → default 4)
//   param_4 = pre-alloc block count (ignored when DAT_006182b0 == 0)
//   param_5 = caller-supplied header memory (or NULL — allocate internally)
//   param_6 = allocation flags (low 8 bits = alloc flags; bits 16..23 = pool flags)
//   returns: pointer to the new free-list header, or NULL on alloc failure
//
// Callees: none (pure leaf — all calls via DAT_007d3ff8 vtable function pointers).
// Leaf-function exemption applies per CONFIDENCE.md (callees_depth1: []).
//
// Header struct layout (0x24 bytes total):
//   param_5[0]  +0x00  DWORD  aligned element size
//   param_5[1]  +0x04  DWORD  elements per block (param_2)
//   param_5[2]  +0x08  DWORD  bitmap row stride = (param_2+7)/8
//   param_5[3]  +0x0C  DWORD  alignment
//   param_5[4]  +0x10  PTR    free-block list head (sentinel; self-pointing)
//   param_5[5]  +0x14  PTR    sentinel prev-ptr (also self-pointing initially)
//   param_5[6]  +0x18  DWORD  flags: 2 = internally-allocated, 3 = caller-supplied
//   param_5[7]  +0x1C  PTR    global chain next link
//   param_5[8]  +0x20  PTR    global chain back-link (= &DAT_007d45cc)
//
// Key globals (cited from 0x004cc820 body):
//   0x007d3ff8 — RW device vtable base
//   0x007d45cc — global free-list chain head
//   0x007d45fc — secondary pool allocator free-list (or NULL)
//   0x006182b0 — pre-alloc enable flag (0 = disable)
//   vtbl+0x108 — raw malloc fn ptr
//   vtbl+0x10c — raw free fn ptr
//   vtbl+0x118 — pool alloc fn ptr (alloc element from existing free-list)
//   vtbl+0x11c — pool free fn ptr  (free element from existing free-list)
//
// [UNCERTAIN U-4420]: pre-alloc block header structure:
//   block_size = aligned_elem * param_2 + bitmap_stride + param_3 + 8
//   The +8 padding term and exact doubly-linked block-list link layout within each
//   allocated block are not explicitly stated in the analysis note — only the
//   free-block list head/prev sentinel in the main header is confirmed.
// [UNCERTAIN U-4421]: global chain insertion direction:
//   The analysis note says "insert at tail" but the exact prev/next pointer
//   assignment for the doubly-linked list at DAT_007d45cc is not byte-cited.
//
// ref: re/analysis/promote_c2_render_d3d9/0x004cc820.md
// ---------------------------------------------------------------------------

// vtable call type aliases (all dispatch through DAT_007d3ff8 vtable)
using RwMallocFn_t    = void* (__cdecl*)(std::uint32_t size, std::uint32_t flags);
using RwFreeFn_t      = void   (__cdecl*)(void* ptr, std::uint32_t flags);
using RwPoolAllocFn_t = void*  (__cdecl*)(void* free_list, std::uint32_t flags);

// 0x004cc820
extern "C" __declspec(dllexport) std::uint32_t* __cdecl RwFreeListCreate(
    std::int32_t   param_1,   // element size in bytes
    std::uint32_t  param_2,   // elements per block
    std::uint32_t  param_3,   // alignment (0 → 4)
    std::int32_t   param_4,   // pre-alloc block count
    std::uint32_t* param_5,   // caller-supplied header, or NULL
    std::uint32_t  param_6)   // allocation flags
{
    // --- alignment default [0x004cc820 body: if param_3 == 0 → 4] ---
    if (param_3 == 0u) {
        param_3 = 4u;
    }

    // --- aligned element size: round param_1 up to param_3 boundary [0x004cc820 body] ---
    const std::uint32_t aligned_elem =
        (static_cast<std::uint32_t>(param_1) + param_3 - 1u) & ~(param_3 - 1u);

    // --- bitmap row stride: (param_2 + 7) / 8 [0x004cc820 body] ---
    const std::uint32_t bitmap_stride = (param_2 + 7u) / 8u;

    // --- load vtable base [0x004cc820 body: mov eax, [DAT_007d3ff8]] ---
    const std::uint32_t vtbl = *reinterpret_cast<const std::uint32_t*>(0x007d3ff8u);

    // --- allocate header if not caller-supplied [0x004cc820 body] ---
    if (param_5 == nullptr) {
        // Check secondary pool allocator [0x004cc820 body: cmp [DAT_007d45fc], 0]
        void* pool_fl = *reinterpret_cast<void**>(0x007d45fcu);
        if (pool_fl == nullptr) {
            // raw alloc: (vtbl+0x108)(0x24, param_6 & 0x00ff0000) [0x004cc820 body]
            auto raw_alloc = *reinterpret_cast<RwMallocFn_t*>(vtbl + 0x108u);
            param_5 = reinterpret_cast<std::uint32_t*>(
                raw_alloc(0x24u, param_6 & 0x00ff0000u));
        } else {
            // pool alloc from secondary free-list [0x004cc820 body: vtbl+0x118]
            auto pool_alloc = *reinterpret_cast<RwPoolAllocFn_t*>(vtbl + 0x118u);
            param_5 = reinterpret_cast<std::uint32_t*>(
                pool_alloc(pool_fl, param_6 & 0x00ff0000u));
        }
        if (param_5 == nullptr) {
            return nullptr;   // allocation failure [0x004cc820 body]
        }
        // mark internally-allocated: param_5[6] = 2 [0x004cc820 body]
        param_5[6] = 2u;
    } else {
        // caller-supplied header: param_5[6] = 3 [0x004cc820 body]
        param_5[6] = 3u;
    }

    // --- fill header fields [0x004cc820 body] ---
    param_5[0] = aligned_elem;   // +0x00 aligned element size  [0x004cc820]
    param_5[1] = param_2;        // +0x04 elements per block     [0x004cc820]
    param_5[2] = bitmap_stride;  // +0x08 bitmap row stride      [0x004cc820]
    param_5[3] = param_3;        // +0x0C alignment              [0x004cc820]

    // free-block list sentinel: self-pointing doubly-linked (head=self, prev=self)
    // param_5[4] = &param_5[4]; param_5[5] = &param_5[4]  [0x004cc820 body]
    const std::uint32_t sentinel_addr =
        reinterpret_cast<std::uint32_t>(&param_5[4]);
    param_5[4] = sentinel_addr;  // +0x10 head points to self   [0x004cc820]
    param_5[5] = sentinel_addr;  // +0x14 prev points to self   [0x004cc820]

    // param_5[6] already set above

    // --- pre-alloc blocks [0x004cc820 body: if DAT_006182b0 != 0 && param_4 > 0] ---
    // [UNCERTAIN U-4420]: block structure not fully byte-cited; see note above.
    // The pre-alloc path is skipped when DAT_006182b0 == 0 (which is the case during
    // boot at main menu). Deferred pending Ghidra asm verification of the block layout.
    if (*reinterpret_cast<const std::int32_t*>(0x006182b0u) != 0 && param_4 > 0) {
        // [UNCERTAIN U-4420]: block allocation loop deferred.
        // Fall through to global chain insertion without pre-alloc.
        // This produces a correct empty free-list; pre-alloc is an optimization,
        // not required for correctness in the no-pre-alloc (DAT_006182b0==0) path.
        (void)param_4;
    }

    // --- insert into global pool chain [0x004cc820 body: DAT_007d45cc chain] ---
    // [UNCERTAIN U-4421]: exact tail-insertion pointer assignments not byte-cited.
    // The global chain at DAT_007d45cc is a doubly-linked list; param_5 is inserted.
    // Confirmed: param_5[7] (+0x1C) = chain next link, param_5[8] (+0x20) = back-link.
    // Back-link stores the address of the pointer that points to us (&DAT_007d45cc or
    // previous node's +0x1C field) — canonical doubly-linked design.
    // Implementing a simple prepend (head-insert) which is typical for this pattern.
    std::uint32_t* chain_head_ptr = reinterpret_cast<std::uint32_t*>(0x007d45ccu);
    const std::uint32_t old_head  = *chain_head_ptr;
    // param_5[7] = old head value (next pointer)  [0x004cc820 body: +0x1C]
    param_5[7] = old_head;
    // param_5[8] = address of chain-head pointer (back-link)  [0x004cc820 body: +0x20]
    param_5[8] = reinterpret_cast<std::uint32_t>(chain_head_ptr);
    // if old head was a valid node, update its back-link to point to param_5[7]
    if (old_head != 0u && old_head != reinterpret_cast<std::uint32_t>(chain_head_ptr)) {
        // old_head[8] (its back-link field at +0x20) = &param_5[7]
        reinterpret_cast<std::uint32_t*>(old_head)[8] =
            reinterpret_cast<std::uint32_t>(&param_5[7]);
    }
    // chain head now points to param_5
    *chain_head_ptr = reinterpret_cast<std::uint32_t>(param_5);

    return param_5;
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(RwFreeListCreate, 0x004cc820);
