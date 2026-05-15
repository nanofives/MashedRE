// Mashed RE - RWS audio pool allocator, destructor, endian-swap field packer,
// and WAVEFORMATEX-like format copy.
//
// All four functions are RWS leaf helpers in the audio subsystem. They are
// tested for C3 via Frida A/B diff against the live MASHED.exe process.
//
// Globals used at runtime:
//   DAT_007ddab0 (0x007ddab0) — secondary pool pointer (used by AudioPoolDestroy)
//   DAT_009146c0 (0x009146c0) — node-list pool header (used by AudioPoolBlockAlloc)
//
// Each function cites its RVA inline and is registered via RH_ScopedInstall.

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Trampoline typedefs for unhooked callees.
// ---------------------------------------------------------------------------

// 0x005aea00 — vtable-based raw alloc trampoline (C2).
typedef void* (__cdecl *RawAllocFn)(int, int);
static RawAllocFn const RawAlloc = reinterpret_cast<RawAllocFn>(0x005aea00u);

// 0x004522d0 — vtable-based raw free trampoline (C1).
typedef void (__cdecl *RawFreeFn)(void*);
static RawFreeFn const RawFree = reinterpret_cast<RawFreeFn>(0x004522d0u);

// 0x005ae920 — bitmap pool free (C2); call signature: void(pool_hdr*, void*)
typedef void (__cdecl *PoolFreeFn)(int*, int*);
static PoolFreeFn const PoolFree = reinterpret_cast<PoolFreeFn>(0x005ae920u);

// Global secondary pool pointer.
// 0x007ddab0 — DAT_007ddab0: pointer to secondary pool header used as the
//              default pool for pool-header allocation/return.
static int** const g_SecondaryPool = reinterpret_cast<int**>(0x007ddab0u);

// ---------------------------------------------------------------------------
// 0x005ae800  FUN_005ae800 — bitmap pool block allocator
//
// Scans the circular block list rooted at param_1[4] for a free bit in each
// block's bitmap (param_1[2] bytes at block+8).  If a free bit is found it is
// set and the aligned address for that slot is computed and returned.  If no
// free bit exists in any existing block a new block is allocated via RawAlloc
// (0x005aea00), its bitmap zeroed, bit-0 set, the block inserted into the
// circular list, and the first slot address returned.
//
// Address formula (from Ghidra 0x005ae800..0x005ae8ff):
//   slot_index = byte_idx * 8 + bit_pos
//   base       = (block + param_1[2] + param_1[3] + 7) & ~(param_1[3] - 1)
//   return     = base + slot_index * param_1[0]
// ---------------------------------------------------------------------------
// 0x005ae800
extern "C" __declspec(dllexport) unsigned int __cdecl AudioPoolBlockAlloc(int* param_1, int param_2)
{
    const int  elem_size   = param_1[0];   // offset 0x00: element size in bytes
    const int  bitmap_len  = param_1[2];   // offset 0x08: bitmap byte count
    const int  align       = param_1[3];   // offset 0x0c: alignment requirement
    int*       sentinel    = param_1 + 4;  // offset 0x10: circular list sentinel

    // Scan existing blocks.
    int* block = reinterpret_cast<int*>(sentinel[0]);  // head = *(sentinel)
    while (block != sentinel) {
        // Bitmap bytes start at block+8 (i.e., block[2]).
        uint8_t* bitmap = reinterpret_cast<uint8_t*>(block + 2);
        for (int byte_idx = 0; byte_idx < bitmap_len; ++byte_idx) {
            uint8_t b = bitmap[byte_idx];
            if (b == 0xffu) continue;  // byte full
            // Find first clear bit (MSB-first: 0x80 >> i).
            for (int bit_pos = 0; bit_pos < 8; ++bit_pos) {
                uint8_t mask = static_cast<uint8_t>(0x80u >> bit_pos);
                if ((b & mask) == 0u) {
                    bitmap[byte_idx] = static_cast<uint8_t>(b | mask);
                    const int slot_idx = byte_idx * 8 + bit_pos;
                    // Base address: (block + bitmap_len + align + 7) & ~(align-1)
                    const uintptr_t raw_base =
                        reinterpret_cast<uintptr_t>(block) +
                        static_cast<uintptr_t>(bitmap_len) +
                        static_cast<uintptr_t>(align) + 7u;
                    const uintptr_t base = raw_base & ~static_cast<uintptr_t>(align - 1);
                    return static_cast<unsigned int>(base + static_cast<uintptr_t>(slot_idx * elem_size));
                }
            }
        }
        block = reinterpret_cast<int*>(block[0]);  // next in circular list
    }

    // No free slot — allocate a new block.
    const int block_size = elem_size * param_1[1] + align + 8 + bitmap_len;
    // param_1[1] = capacity (elements per block), at offset 0x04.
    // Full formula: param_1[1]*param_1[0] + param_1[3] + 8 + param_1[2]
    int* new_block = static_cast<int*>(RawAlloc(block_size, param_2));
    if (!new_block) return 0u;

    // Zero bitmap at new_block+8 (new_block[2..]).
    uint8_t* new_bitmap = reinterpret_cast<uint8_t*>(new_block + 2);
    for (int i = 0; i < bitmap_len; ++i) new_bitmap[i] = 0u;

    // Mark bit-0 used (MSB of first bitmap byte = 0x80).
    new_bitmap[0] = 0x80u;

    // Insert into circular list before sentinel (tail-insert).
    int* prev = reinterpret_cast<int*>(sentinel[1]);  // *(sentinel+4) = tail
    new_block[0] = reinterpret_cast<int>(sentinel);   // new->next = sentinel
    new_block[1] = reinterpret_cast<int>(prev);       // new->prev = tail
    prev[0]      = reinterpret_cast<int>(new_block);  // tail->next = new
    sentinel[1]  = reinterpret_cast<int>(new_block);  // sentinel->prev = new

    // Return slot 0 address.
    const uintptr_t raw_base =
        reinterpret_cast<uintptr_t>(new_block) +
        static_cast<uintptr_t>(bitmap_len) +
        static_cast<uintptr_t>(align) + 7u;
    const uintptr_t base = raw_base & ~static_cast<uintptr_t>(align - 1);
    return static_cast<unsigned int>(base);
}

RH_ScopedInstall(AudioPoolBlockAlloc, 0x005ae800);

// ---------------------------------------------------------------------------
// 0x005ae780  FUN_005ae780 — bitmap pool destructor
//
// Drains the circular block list at param_1+0x10: for each block, unlinks it
// and calls RawFree (0x004522d0). After all blocks are freed, frees the pool
// header itself: if DAT_007ddab0 secondary pool is available, returns the
// header there via PoolFree (0x005ae920); otherwise RawFree the header.
// If bit-0 of *(param_1+0x18) is set, the header is externally owned — skip
// the header free.
//
// Decompilation: Ghidra 0x005ae780..0x005ae7ff
// ---------------------------------------------------------------------------
// 0x005ae780
extern "C" __declspec(dllexport) void __cdecl AudioPoolDestroy(int param_1)
{
    // Block list head is at *(param_1 + 0x10), i.e. offset 4 DWORDs into header.
    int* sentinel = reinterpret_cast<int*>(param_1 + 0x10);
    int* head = reinterpret_cast<int*>(sentinel[0]);  // *(param_1+0x10)

    while (head != sentinel) {
        // Unlink from circular list.
        int* next_node = reinterpret_cast<int*>(head[0]);
        int* prev_node = reinterpret_cast<int*>(head[1]);
        *reinterpret_cast<int*>(head[1]) = head[0];  // *(prev->next) = next
        *reinterpret_cast<int*>(head[0] + 4) = head[1];  // *(next->prev) = prev
        // Free the block.
        RawFree(head);
        // Re-read head (may have changed if RawFree reshuffled, but in this
        // allocator the sentinel is stable).
        head = reinterpret_cast<int*>(sentinel[0]);
    }

    // Check ownership flag: bit-0 of *(param_1 + 0x18).
    const uint8_t flags = *reinterpret_cast<const uint8_t*>(param_1 + 0x18);
    if ((flags & 1u) == 0u) {
        // Header is internally owned — free it.
        int* secondary = *g_SecondaryPool;
        int* hdr = reinterpret_cast<int*>(param_1);
        if (secondary != nullptr && secondary != hdr) {
            // Return to secondary pool.
            PoolFree(secondary, hdr);
        } else {
            RawFree(hdr);
        }
    }
}

RH_ScopedInstall(AudioPoolDestroy, 0x005ae780);

// ---------------------------------------------------------------------------
// 0x005aeca0  FUN_005aeca0 — endian-swap field packer
//
// Writes 1, 2, or 4 bytes from *param_2 into the output buffer at **param_1,
// performing big→little endian byte reversal for 2-byte (bswap16) and 4-byte
// (bswap32) fields.  Advances *param_1 by the field size after each write.
//
// param_3 == 1: copy 1 byte verbatim.
// param_3 == 2: write 2 bytes in big-endian order (bytes[0]=low, bytes[1]=high).
// param_3 == 4: write 4 bytes bswap32.
//
// Decompilation: Ghidra 0x005aeca0..0x005aecff
// ---------------------------------------------------------------------------
// 0x005aeca0
extern "C" __declspec(dllexport) void __cdecl AudioFieldEndianPack(int* param_1, unsigned int* param_2, int param_3)
{
    if (param_3 == 1) {
        // 1-byte: copy low byte verbatim.
        *reinterpret_cast<uint8_t*>(*param_1) = static_cast<uint8_t>(*param_2);
        *param_1 += 1;
    } else if (param_3 == 2) {
        // 2-byte: big-endian swap.
        // Ghidra: *(uint16*)*param_1 = CONCAT11((char)*param_2, *(byte*)((int)param_2+1))
        // CONCAT11(hi, lo) = (hi << 8) | lo stored as uint16 little-endian →
        // byte[0] = lo = param_2[1], byte[1] = hi = param_2[0].
        const uint8_t b0 = static_cast<uint8_t>(*param_2);           // param_2[0] = hi
        const uint8_t b1 = *reinterpret_cast<const uint8_t*>(
                               reinterpret_cast<const char*>(param_2) + 1); // param_2[1] = lo
        *reinterpret_cast<uint8_t*>(*param_1)     = b1;  // lo byte first (little-endian store)
        *reinterpret_cast<uint8_t*>(*param_1 + 1) = b0;  // hi byte second
        *param_1 += 2;
    } else if (param_3 == 4) {
        // 4-byte: bswap32.
        // Ghidra formula: (v<<0x10 | v&0xff00 | v>>0x10&0xff)<<8 | *(byte*)(param_2+3)
        const unsigned int v = *param_2;
        const unsigned int swapped =
            (((v << 0x10u) | (v & 0xff00u) | ((v >> 0x10u) & 0xffu)) << 8u)
            | static_cast<uint8_t>(*reinterpret_cast<const uint8_t*>(
                reinterpret_cast<const char*>(param_2) + 3));
        *reinterpret_cast<unsigned int*>(*param_1) = swapped;
        *param_1 += 4;
    }
    // param_3 values other than 1/2/4: no-op (no else branch in Ghidra decomp).
}

RH_ScopedInstall(AudioFieldEndianPack, 0x005aeca0);

// ---------------------------------------------------------------------------
// 0x005ae0c0  FUN_005ae0c0 — WAVEFORMATEX-like 16-byte format copy
//
// Copies a 16-byte audio format descriptor from param_1 to param_2.
// param_3 controls endian swapping:
//   param_3 == NULL: direct 4-DWORD copy (param_2[0..3] = param_1[0..3]).
//   param_3 != NULL: byte-swap copy through a 4-DWORD stack buffer:
//     AudioFieldEndianPack(&ptr, param_1+0, 4)   — first DWORD
//     AudioFieldEndianPack(&ptr, param_1+1, 2)   — bytes 4-5 (nSamplesPerSec lo)
//     AudioFieldEndianPack(&ptr, (int)param_1+6, 2) — bytes 6-7
//     direct copy: local_buf[2..3] = param_1[2..3]
//     param_2[0..3] = local_buf[0..3]
//
// Returns param_1.
//
// Constants cited from Ghidra at 0x005ae0e3 (4), 0x005ae0ee (2), 0x005ae0fa (2).
// ---------------------------------------------------------------------------
// 0x005ae0c0
extern "C" __declspec(dllexport) unsigned int* __cdecl AudioWaveFmtCopy(
        unsigned int* param_1, unsigned int* param_2, unsigned int* param_3)
{
    if (param_3 == nullptr) {
        // No-swap path: direct 4-DWORD copy.
        param_2[0] = param_1[0];
        param_2[1] = param_1[1];
        param_2[2] = param_1[2];
        param_2[3] = param_1[3];
    } else {
        // Swap path: build swapped 4-DWORD local buffer.
        unsigned int local_buf[4];
        int ptr = reinterpret_cast<int>(local_buf);

        // Byte-swap first 4-byte field (offsets 0-3).
        AudioFieldEndianPack(&ptr, param_1 + 0, 4);
        // Byte-swap 2-byte field at offset 4-5.
        AudioFieldEndianPack(&ptr, param_1 + 1, 2);
        // Byte-swap 2-byte field at offset 6-7.
        AudioFieldEndianPack(&ptr, reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(param_1) + 6), 2);
        // Remaining 8 bytes (offsets 8-15): direct copy.
        local_buf[2] = param_1[2];
        local_buf[3] = param_1[3];

        // Write to destination.
        param_2[0] = local_buf[0];
        param_2[1] = local_buf[1];
        param_2[2] = local_buf[2];
        param_2[3] = local_buf[3];
    }
    return param_1;
}

RH_ScopedInstall(AudioWaveFmtCopy, 0x005ae0c0);
