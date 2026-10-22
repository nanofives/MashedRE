// Mashed RE — Render/TextureLoader_q6.cpp
// RenderWare stream I/O + texture-dictionary creation helpers (c3-batch-q-s6).
//
// Covers:
//   (RwStreamRead / RwStreamSkip moved to Render/RwStreamCore.cpp, one body shared with the exe, lane dcstream)
//   0x004c5890  RwTexDictionaryCreate  — allocate+init RwTexDictionary; link into global TXD list
//
// Deferred (>200B and complex body, or harness-limited):
//   0x005abfa0  FUN_005abfa0  — per-wave data loader: reads 0x803+0x804 RWS chunks; complex multi-phase
//   0x005ac210  FUN_005ac210  — wave_node factory: 16 callees, flag-dispatch bits 0-5; complex multi-phase
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched; patched copy SHA differs — expected)
//
// Analysis refs:
//   re/analysis/texture_loader_d3/0x004c5890.md
//
// Verification strategy:
//   All three functions access live RwStream / RwFreeList objects at runtime.
//   At quiescent main-menu (null/zero context) each crashes deterministically
//   the same way as the original — crash_equal_ok=True GREEN per established
//   pattern (see audio_rws_chunk_header_read, vfs_stream_get_type precedents).

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// 0x004c5890  FUN_004c5890  RwTexDictionaryCreate   (small body, ~60 bytes est.)
//
// RwTexDictionary *RwTexDictionaryCreate(void)
//   No parameters.
//   Returns newly allocated and linked RwTexDictionary on success, NULL on failure.
//
// Body:
//   1. Allocate object via vtable+0x118 call with type-hint 0x30016.
//      vtable base at DAT_007d3ff8; size/slot from *(DAT_007d4054 + 0x0c + DAT_007d3ff8).
//   2. On success: set object type = 6 (rwTEXDICTIONARY) at *result.
//      Zero out next 4 fields (result[1..4]).
//   3. Link into global TXD doubly-linked list (head at DAT_007d4054 + DAT_007d3ff8):
//      result[4] = fwd link (result+0x10); result[5] = bwd link (result+0x14).
//   4. Init circular sentinel for internal texture list at result+0x08/+0x0c.
//   5. Notify event DAT_00618150 via FUN_004d8000(&DAT_00618150, result).
//   6. Return result.
//
// Facts cited from re/analysis/texture_loader_d3/0x004c5890.md:
//   Alloc type-hint 0x30016, via vtable+0x118:  0x004c5897
//   *result = 6 (rwTEXDICTIONARY):              0x004c58a5
//   result+0x10/+0x14 = RwLLLink in global TXD: 0x004c58b4..0x004c58c9
//   result+0x08/+0x0c = RwLinkList sentinel:     0x004c58cc..0x004c58d3
//   Event DAT_00618150 via FUN_004d8000:         0x004c58d8
//
// External callees (called via original addresses — not yet C3):
//   *(DAT_007d3ff8 + 0x118) — RW engine allocator (vtable slot)
//   FUN_004d8000            — RW event notify
// ---------------------------------------------------------------------------

// External callee: FUN_004d8000 — RW event notification.
// void __cdecl FUN_004d8000(void *event_obj, void *result)
using EventNotifyFn_t = void (__cdecl*)(void*, void*);
static EventNotifyFn_t const s_FUN_004d8000 = reinterpret_cast<EventNotifyFn_t>(0x004d8000u);

// 0x004c5890
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
RwTexDictionaryCreate()
{
    // DAT_007d3ff8 = RW globals base (vtable / engine state block ptr)
    // DAT_007d4054 = TXD list root offset into the globals block
    static constexpr std::uintptr_t kRwGlobals  = 0x007d3ff8u;
    static constexpr std::uintptr_t kTxdRootOfs = 0x007d4054u;

    std::uint32_t rw_base = *reinterpret_cast<std::uint32_t*>(kRwGlobals);
    std::uint32_t txd_root_off = *reinterpret_cast<std::uint32_t*>(kTxdRootOfs);

    // Allocator slot: vtable+0x118. Alloc size comes from *(txd_root_off + 0x0c + rw_base).
    // Type-hint 0x30016, cited 0x004c5897.
    using AllocFn_t = std::uint32_t* (__cdecl*)(std::uint32_t, std::uint32_t);
    AllocFn_t alloc_fn = *reinterpret_cast<AllocFn_t*>(rw_base + 0x118u);
    std::uint32_t alloc_size = *reinterpret_cast<std::uint32_t*>(txd_root_off + 0x0cu + rw_base);
    std::uint32_t* puVar3 = alloc_fn(alloc_size, 0x30016u);
    if (puVar3 == nullptr) return nullptr;

    // Set object fields: type=6 (rwTEXDICTIONARY), cited 0x004c58a5
    puVar3[0] = 6u;      // *result = 6
    puVar3[1] = 0u;
    puVar3[2] = 0u;
    puVar3[3] = 0u;
    puVar3[4] = 0u;      // *(undefined4*)(result + 4) = 0

    // Link into global TXD doubly-linked list, cited 0x004c58b4..0x004c58c9.
    // List head pointer lives at (txd_root_off + rw_base).
    std::uint32_t list_head_addr = txd_root_off + rw_base;   // address of list head ptr
    std::uint32_t* list_head_ptr = reinterpret_cast<std::uint32_t*>(list_head_addr);

    // result+0x10 = RwLLLink fwd (index 4 in u32 units)
    std::uint32_t* puVar1 = puVar3 + 4;  // result+0x10

    // fwd = old list head; bwd = list head address (sentinel)
    *puVar1 = *list_head_ptr;                    // result[4] = old_head
    puVar3[5] = list_head_addr;                  // result[5] = &list_head (+0x14)

    // Fix old_head's bwd pointer to point to result+0x10
    std::uint32_t old_head_val = *list_head_ptr;
    *reinterpret_cast<std::uint32_t*>(old_head_val + 4u) =
        reinterpret_cast<std::uint32_t>(puVar1);

    // Update list head to point to result+0x10
    *list_head_ptr = reinterpret_cast<std::uint32_t>(puVar1);

    // Init circular sentinel for internal texture list at result+0x08/+0x0c,
    // cited 0x004c58cc..0x004c58d3.
    std::uint32_t* puVar2 = puVar3 + 2;   // result+0x08
    puVar3[3] = reinterpret_cast<std::uint32_t>(puVar2);  // result[3] = puVar2 (+0x0c)
    *puVar2   = reinterpret_cast<std::uint32_t>(puVar2);  // *puVar2   = puVar2 (self-circular)

    // Event notify: FUN_004d8000(&DAT_00618150, puVar3), cited 0x004c58d8
    static constexpr std::uintptr_t kEventObj = 0x00618150u;
    s_FUN_004d8000(reinterpret_cast<void*>(kEventObj), puVar3);

    return puVar3;
}

RH_ScopedInstall(RwTexDictionaryCreate, 0x004c5890);  // re-enabled 2026-05-24 phase-a2 GREEN (allocator_nonnull both non-null)
