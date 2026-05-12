# Audio Allocator Structs

Derived from Session 40 (2026-05-12) — analysis of the bitmap pool family at 0x005ae650–0x005ae920.

---

## Vtable-dispatch alloc/dealloc trampolines

All audio allocations are routed through two global vtable-dispatch trampolines:

| RVA | Vtable offset | Role |
|---|---|---|
| 0x005aea00 | `*(DAT_007d3ff8) + 0x108` | Allocator trampoline. JMP thunk — passes args from caller directly to target. |
| 0x004522d0 | `*(DAT_007d3ff8) + 0x10c` | Deallocator trampoline. Same JMP pattern. |
| 0x005aea50 | `*(DAT_007d3ff8) + 0x114` | Secondary allocator trampoline (different pool class). |

`DAT_007d3ff8` (0x007d3ff8) is runtime-initialized (0x0 in static PE). The actual function pointers at these slots are only determinable at runtime. [UNCERTAIN U-0125] [UNCERTAIN U-1735]

---

## AudioAlignedAlloc (0x005aea10)

Thin wrapper over the vtable allocator trampoline that adds a 4-byte hidden header for alignment tracking.

```c
// 0x005aea10
uint AudioAlignedAlloc(int size, undefined4 type_tag)
{
    void* raw = FUN_005aea00(size + 4, type_tag);    // alloc size+4 bytes
    if (!raw) return 0;
    uint aligned = (raw + 4) & 0xfffffffc;           // 4-byte align
    *(uint*)(aligned - 4) = raw;                     // store raw base before aligned ptr
    return aligned;
}
```

- Returns 4-byte-aligned pointer into the allocation.
- Raw base stored at `*(aligned_ptr - 4)` for free via `FUN_004522d0`.

---

## AudioBitmapPool (pool header, 0x24 bytes = 9 × uint32)

Constructed by `FUN_005ae650` (0x005ae650). Managed by `FUN_005ae800` (alloc) and `FUN_005ae920` (free).

```
Offset  Field           Notes
+0x00   aligned_size    (elem_size + align - 1) & ~(align - 1)
+0x04   bit_count       elements per block (param_2 to constructor)
+0x08   bitmap_bytes    (bit_count + 7) >> 3
+0x0c   alignment       alignment value (≥ 1)
+0x10   block_list_head circular linked list of AllocBlock; sentinel = &header[4]
+0x14   block_list_tail  = &header[4] when empty
+0x18   ownership_flags bit0 = header externally owned (skip free); bit1 = compact (release empty blocks)
+0x1c   global_list_next linked into DAT_007dda80 global pool list
+0x20   global_list_prev
```

Global pool list head: `DAT_007dda80` (0x007dda80).
Secondary pool-header pool: `DAT_007ddab0` (0x007ddab0) — purpose relative to main pool unclear. [UNCERTAIN U-1736]

---

## AudioAllocBlock (block header, variable size)

Each block: `2 × uint32` circular-list links + `bitmap_bytes` bytes bitmap + `aligned_size × bit_count` bytes data.

```
Offset  Field     Notes
+0x00   next      circular list next ptr
+0x04   prev      circular list prev ptr
+0x08   bitmap[]  ceil(bit_count/8) bytes; bit=1 means slot occupied; MSB-first within each byte
+0x08+bitmap_bytes  data[]  element array; stride = aligned_size
```

Allocation address formula:
`(block_start + (byte_idx * 8 + bit_pos) * aligned_size)` where `block_start = &block[0x08 + bitmap_bytes]` aligned to `header.alignment`.

---

## AudioSubStructA (at audio_obj + 0x24, 3 × uint32)

Managed by `FUN_005ae010` (init), `FUN_005ae080` (conditional pool-return), `FUN_005ae030` (combined teardown).

```
Offset  Field        Notes
+0x00   data_ptr     pointer to pool-allocated block; 0 = empty
+0x04   (unknown)    not accessed by sub-struct A functions
+0x08   flags        bit0 = pool-owned (set by alloc; cleared by FUN_005ae080 on return)
```

Pool for sub-struct A data: `DAT_007dda28` (0x007dda28).

---

## AudioSubStructB (at audio_obj + 0x34, at least 3 × uint32)

Managed by `FUN_005ae050` (conditional heap-free), `FUN_005ae030` (combined teardown).

```
Offset  Field        Notes
+0x00   (unknown)    not accessed by sub-struct B functions
+0x04   data_ptr     pointer to heap-allocated buffer; 0 = empty
+0x08   flags        bit1 = heap-owned (set by alloc; cleared by FUN_005ae050 on free)
```

Heap free via `FUN_004522d0` (vtable deallocator trampoline).
