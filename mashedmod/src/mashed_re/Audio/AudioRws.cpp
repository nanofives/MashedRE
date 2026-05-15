// Mashed RE — Audio RWS doubly-linked list and pool-allocator free.
// Four leaf functions that form the lowest-level audio node management layer.
//
// Node pool: DAT_009146c0 (raw address 0x009146c0) — bitmap-tracked fixed-size
// block allocator.  Each node is 12 bytes: [0]=fwd, [1]=back/prev, [2]=payload.
//
// Promotion order:
//   AudioPoolFree         0x005ae920  (pool alloc free; callee of the others)
//   AudioListInsertHead   0x005addd0  (insert at list head; allocs from pool)
//   AudioListRemoveByValue 0x005ade10 (search + remove; returns node to pool)
//   AudioListDrain        0x005ade90  (drain all nodes; returns each to pool)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Pool header layout (uint32 words):
//   [0]  element size in bytes
//   [1]  elements per block
//   [2]  bitmap dword count (uVar2; bytes per block bitmap)
//   [4]  block list head (circular, sentinel = header+4)
//   [6]  flags: bit 1 = compact (free empty blocks to heap via FUN_004522d0)
//
// Block layout (starting at block pointer):
//   [0]  next block pointer
//   [1]  prev block pointer
//   +8   bitmap  (ceil(elements/8) bytes)
//   +8+bitmap  element array
// ---------------------------------------------------------------------------

// 0x005ae920  FUN_005ae920  (~0xd3 bytes)
// void FUN_005ae920(uint *param_1, uint param_2)
//   param_1 = pool header pointer
//   param_2 = address of item being freed (as uint)
// Walks block list to find the block containing param_2, clears the bitmap bit,
// optionally releases empty blocks to heap when pool-compaction flag is set.
//
// Ghidra address of key steps:
//   0x005ae920: function prologue / stack frame setup
//   0x005ae930: block list traversal start (param_1[4] block head)
//   0x005ae95c: range check: block_start <= param_2 < block_start + N*elem_sz
//   0x005ae96c: compute item index = (param_2 - block_start) / elem_sz
//   0x005ae97a: clear bit: block[8 + (idx>>3)], mask = 0x80 >> (idx & 7) (MSB-first)
//   0x005ae9a0: compaction flag check (param_1[6] bit 1)
//   0x005ae9b0: count remaining set bits; re-link or FUN_004522d0(block)
extern "C" __declspec(dllexport) void __cdecl AudioPoolFree(
        uint32_t* param_1, uint32_t param_2)
{
    // pool header fields
    const uint32_t elem_size   = param_1[0];
    const uint32_t elems_per_blk = param_1[1];
    const uint32_t bitmap_bytes  = param_1[2];  // bytes in bitmap per block
    const uint32_t* sentinel     = param_1 + 4; // sentinel for circular list

    // vtable dealloc trampoline at 0x004522d0 — used when releasing empty blocks
    typedef void (__cdecl *FreeFn)(void*);
    const auto rawFree = reinterpret_cast<FreeFn>(0x004522d0u);

    // Walk block list
    uint32_t* block = reinterpret_cast<uint32_t*>(param_1[4]);
    while (block != sentinel) {
        // block_start: block pointer + 2 words (next/prev) + bitmap_bytes,
        // aligned to elem_size.  From Ghidra: block_start = block + uVar2 + 8.
        const uint32_t block_addr   = reinterpret_cast<uint32_t>(block);
        // uVar2 in the original is param_1[2] (bitmap byte count).
        const uint32_t block_start  = block_addr + 8u + bitmap_bytes;
        const uint32_t range_end    = block_start + elems_per_blk * elem_size;

        if (param_2 >= block_start && param_2 < range_end) {
            // Found the block.
            const uint32_t item_index = (param_2 - block_start) / elem_size;

            // Clear allocation bit: MSB-first bitmap at block+8.
            uint8_t* bitmap = reinterpret_cast<uint8_t*>(block_addr + 8u);
            bitmap[item_index >> 3] &= ~(0x80u >> (item_index & 7u));

            // Unlink block from current list position.
            uint32_t* prev = reinterpret_cast<uint32_t*>(block[1]);
            uint32_t* next = reinterpret_cast<uint32_t*>(block[0]);
            prev[0] = reinterpret_cast<uint32_t>(next);
            next[1] = reinterpret_cast<uint32_t>(prev);

            if (param_1[6] & 2u) {
                // Compaction flag set: count remaining set bits.
                uint32_t bits_set = 0;
                for (uint32_t b = 0; b < bitmap_bytes; ++b) {
                    uint8_t byte = bitmap[b];
                    while (byte) {
                        bits_set += (byte & 1u);
                        byte >>= 1;
                    }
                }
                if (bits_set > 0) {
                    // Still allocated items — re-link at free-list head.
                    uint32_t* head_next = reinterpret_cast<uint32_t*>(param_1[4]);
                    block[0] = reinterpret_cast<uint32_t>(head_next);
                    block[1] = reinterpret_cast<uint32_t>(sentinel);
                    head_next[1] = reinterpret_cast<uint32_t>(block);
                    param_1[4] = reinterpret_cast<uint32_t>(block);
                } else {
                    // Block fully empty — release to heap.
                    rawFree(block);
                }
            } else {
                // No compaction — re-link block at free-list head unconditionally.
                uint32_t* head_next = reinterpret_cast<uint32_t*>(param_1[4]);
                block[0] = reinterpret_cast<uint32_t>(head_next);
                block[1] = reinterpret_cast<uint32_t>(sentinel);
                head_next[1] = reinterpret_cast<uint32_t>(block);
                param_1[4] = reinterpret_cast<uint32_t>(block);
            }
            return;
        }

        block = reinterpret_cast<uint32_t*>(block[0]);
    }
    // Item not found in any block — no-op (matches original behavior).
}

RH_ScopedInstall(AudioPoolFree, 0x005ae920);

// ---------------------------------------------------------------------------

// 0x005addd0  FUN_005addd0  (~60 bytes)
// void FUN_005addd0(int *param_1, int param_2)
//   param_1 = pointer to list head variable
//   param_2 = payload value to store in new node
// Allocates one node from the pool at DAT_009146c0 via FUN_005ae800,
// then inserts it at the head of the doubly-linked list.
//
// Ghidra key addresses:
//   0x005addd0: call FUN_005ae800(&DAT_009146c0, 0x30804) → piVar2
//   0x005adde3: NULL check; return if alloc failed
//   0x005adde8: piVar2[2] = param_2    (payload at +0x08)
//   0x005addec: iVar1 = *param_1       (old head)
//   0x005addf1: piVar2[1] = param_1   (back-link to head variable)
//   0x005addf4: *piVar2 = iVar1        (fwd ptr = old head)
//   0x005addf7: *(old_head+4) = piVar2 (old head's back-link → new node)
//   0x005addfc: *param_1 = piVar2      (list head = new node)
extern "C" __declspec(dllexport) void __cdecl AudioListInsertHead(
        int* param_1, int param_2)
{
    // DAT_009146c0 = global node pool base
    static constexpr uint32_t kNodePool = 0x009146c0u;
    static constexpr uint32_t kAllocTag = 0x00030804u;

    typedef int* (__cdecl *AllocFn)(uint32_t*, uint32_t);
    const auto poolAlloc = reinterpret_cast<AllocFn>(0x005ae800u);

    int* piVar2 = poolAlloc(reinterpret_cast<uint32_t*>(kNodePool), kAllocTag);
    if (!piVar2) return;

    const int iVar1  = *param_1;     // old head
    piVar2[2]        = param_2;      // node[+0x08] = payload
    piVar2[1]        = reinterpret_cast<int>(param_1); // node[+0x04] = back-link to head-ptr
    piVar2[0]        = iVar1;        // node[+0x00] = fwd ptr = old head
    *reinterpret_cast<int**>(iVar1 + 4) = piVar2; // old_head[+0x04] = new node
    *param_1         = reinterpret_cast<int>(piVar2);  // head = new node
}

RH_ScopedInstall(AudioListInsertHead, 0x005addd0);

// ---------------------------------------------------------------------------

// 0x005ade10  FUN_005ade10  (65 bytes)
// int* FUN_005ade10(list_sentinel *param_1, int param_2)
//   param_1 = doubly-linked list sentinel/head node
//   param_2 = data value to search for and remove
//   Returns param_1 on success (found + removed), NULL if not found.
// Traversal starts from param_1[1] (first real node after sentinel).
// Unlinks found node and returns it to the pool at DAT_009146c0 via AudioPoolFree.
//
// Ghidra key addresses:
//   0x005ade10: piVar2 = param_1[1]     (first real node)
//   0x005ade1d: iVar1 = piVar2[2]       (data field)
//   0x005ade22: if iVar1 == param_2: found path
//   0x005ade34: *(piVar2[0]+4) = piVar2[1]  (prev next → current next)
//   0x005ade3a: *(piVar2[1]) = *piVar2       (next prev → current prev)
//   0x005ade40: FUN_005ae920(&DAT_009146c0, piVar2)
//   0x005ade49: return param_1
//   0x005ade4f: if piVar2 == param_1: return NULL
//   0x005ade55: piVar2 = piVar2[1]   (advance: next node)
extern "C" __declspec(dllexport) int* __cdecl AudioListRemoveByValue(
        int* param_1, int param_2)
{
    static constexpr uint32_t kNodePool = 0x009146c0u;

    int* piVar2 = reinterpret_cast<int*>(param_1[1]); // first real node
    while (true) {
        const int iVar1 = piVar2[2];             // data field — read first (0x005ade1d)
        if (iVar1 == param_2) {                  // found check before sentinel (0x005ade22)
            // Unlink from doubly-linked list.
            // prev node's next = current node's next
            *reinterpret_cast<int**>(piVar2[0] + 4) =
                reinterpret_cast<int*>(piVar2[1]);
            // next node's prev = current node's prev
            *reinterpret_cast<int**>(piVar2[1]) =
                reinterpret_cast<int*>(static_cast<uintptr_t>(static_cast<unsigned>(piVar2[0])));
            // Return node to free pool.
            AudioPoolFree(reinterpret_cast<uint32_t*>(kNodePool),
                          reinterpret_cast<uint32_t>(piVar2));
            return param_1;
        }
        if (piVar2 == param_1) return nullptr;   // reached sentinel: not found (0x005ade4f)
        piVar2 = reinterpret_cast<int*>(piVar2[1]); // advance (0x005ade55)
    }
}

RH_ScopedInstall(AudioListRemoveByValue, 0x005ade10);

// ---------------------------------------------------------------------------

// 0x005ade90  FUN_005ade90  (49 bytes)
// void FUN_005ade90(int *param_1)
//   param_1 = list sentinel (head node)
// Drains the doubly-linked list by unlinking and returning all nodes to
// the free pool at DAT_009146c0.  After the call the sentinel points to itself.
//
// Ghidra key addresses:
//   0x005ade90: loop entry: while param_1[1] != param_1
//   0x005ade97: piVar1 = (int*)param_1[1]         (current node)
//   0x005adea0: *(int*)(piVar1[0] + 4) = piVar1[1] (prev node's next)
//   0x005adea9: *(int*)piVar1[1] = piVar1[0]       (next node's prev)
//   0x005adeae: FUN_005ae920(&DAT_009146c0, piVar1)
//   0x005adebb: (constant) pool address 0x009146c0
extern "C" __declspec(dllexport) void __cdecl AudioListDrain(int* param_1)
{
    static constexpr uint32_t kNodePool = 0x009146c0u;

    while (param_1[1] != reinterpret_cast<int>(param_1)) {
        int* piVar1 = reinterpret_cast<int*>(param_1[1]); // current node

        // Unlink: prev node's next = current node's next
        *reinterpret_cast<int**>(piVar1[0] + 4) =
            reinterpret_cast<int*>(piVar1[1]);
        // next node's prev = current node's prev
        *reinterpret_cast<int**>(piVar1[1]) =
            reinterpret_cast<int*>(static_cast<uintptr_t>(static_cast<unsigned>(piVar1[0])));

        // Return node to free pool.
        AudioPoolFree(reinterpret_cast<uint32_t*>(kNodePool),
                      reinterpret_cast<uint32_t>(piVar1));
    }
}

RH_ScopedInstall(AudioListDrain, 0x005ade90);
