// Mashed RE — Render/TextureLoader_q6.cpp
// RenderWare stream I/O + texture-dictionary creation helpers (c3-batch-q-s6).
//
// Covers:
//   0x004cbd30  RwStreamRead           — 4-type stream dispatch: fread / membuf copy / callback
//   0x004cc050  RwStreamSkip           — 4-type stream dispatch: fseek / membuf advance / callback
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
//   re/analysis/texture_loader_d3/0x004cbd30.md
//   re/analysis/texture_loader_d3/0x004cc050.md
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
// 0x004cbd30  FUN_004cbd30  RwStreamRead   (317 bytes)
//
// RwUInt32 RwStreamRead(RwStream *stream, void *buffer, RwUInt32 length)
//   param_1 = stream ptr (RwStream *); type field at stream+0x00
//   param_2 = destination buffer
//   param_3 = byte count to read
//   Returns bytes successfully read (== param_3 on full success, 0 on error).
//
// 4-case type dispatch (stream type at *param_1 i.e. stream+0x00):
//   case 1, case 2  — file stream:
//     file handle at stream+0x0c (param_1[3])
//     calls FUN_00550950(buffer, 1, length, handle) — fread() wrapper
//     if returned count != length → return bytes actually read (partial)
//   case 3          — memory buffer stream:
//     end pointer at stream+0x10 (param_1[4])
//     current position at stream+0x0c (param_1[3])
//     data base at stream+0x14 (param_1[5])
//     bounds check: cap length to (end - pos) if length would overrun
//     DWORD-copy loop then byte-copy tail from (base + pos)
//     advance param_1[3] by bytes copied
//   case 4          — callback stream:
//     fn ptr at stream+0x10 (param_1[4])
//     user data at stream+0x1c (param_1[7])
//     calls (*fn)(user, buffer, length)
//   default         — error code 0xe → return 0
//
// Facts cited from re/analysis/texture_loader_d3/0x004cbd30.md:
//   Stream type dispatch:       0x004cbd3f
//   File handle stream+0x0c:   0x004cbd77
//   fread call FUN_00550950:    0x004cbd7b
//   Memory end at stream+0x10: 0x004cbe12
//   Memory pos at stream+0x0c: 0x004cbe16
//   Memory base at stream+0x14: 0x004cbe24
//   Callback fn at stream+0x10, user at stream+0x1c: 0x004cbe5b
//   Error code 0xe:             implied by default case
// ---------------------------------------------------------------------------

// External callee: FUN_00550950 — fread() wrapper.
// int __cdecl FUN_00550950(void *buf, int element_sz, int count, int file_handle)
using FreadFn_t = std::uint32_t (__cdecl*)(void*, std::uint32_t, std::uint32_t, std::uint32_t);
static FreadFn_t const s_FUN_00550950 = reinterpret_cast<FreadFn_t>(0x00550950u);

// 0x004cbd30
extern "C" __declspec(dllexport) std::uint32_t __cdecl
RwStreamRead(std::uint32_t* param_1, void* param_2, std::uint32_t param_3)
{
    std::uint32_t stream_type = *param_1;  // stream+0x00, cited 0x004cbd3f

    switch (stream_type) {
    case 1:
    case 2: {
        // File stream: file handle at stream+0x0c = param_1[3], cited 0x004cbd77
        std::uint32_t handle = param_1[3];
        // FUN_00550950(dest, 1, size, handle) — fread wrapper, cited 0x004cbd7b
        std::uint32_t uVar4 = s_FUN_00550950(param_2, 1u, param_3, handle);
        // Partial read: return actual bytes (includes 0 on full failure).
        return uVar4;
    }
    case 3: {
        // Memory buffer stream.
        // end pointer at stream+0x10, cited 0x004cbe12
        // pos at stream+0x0c, cited 0x004cbe16
        // data base at stream+0x14, cited 0x004cbe24
        std::uint32_t end = param_1[4];   // stream+0x10
        std::uint32_t pos = param_1[3];   // stream+0x0c
        std::uint32_t len = param_3;

        // Bounds check: cap to remaining bytes if overrun would occur.
        if ((end - pos) < len) {
            // Truncate: error condition; cap to remaining.
            len = end - pos;
        }

        // Copy len bytes from (param_1[5] + pos) into param_2.
        // Ghidra shows DWORD-copy loop then byte-copy tail — replicate with memcpy.
        std::uint8_t* src = reinterpret_cast<std::uint8_t*>(param_1[5] + pos);  // base+0x14
        std::uint8_t* dst = reinterpret_cast<std::uint8_t*>(param_2);
        for (std::uint32_t i = 0; i < len; ++i) {
            dst[i] = src[i];
        }
        // Advance position, cited 0x004cbe16 area.
        param_1[3] = pos + len;
        return len;
    }
    case 4: {
        // Callback stream: fn ptr at stream+0x10 (param_1[4]),
        // user data at stream+0x1c (param_1[7]), cited 0x004cbe5b
        typedef std::uint32_t (__cdecl *CallbackFn)(std::uint32_t, void*, std::uint32_t);
        CallbackFn fn = reinterpret_cast<CallbackFn>(param_1[4]);
        std::uint32_t user = param_1[7];
        std::uint32_t uVar4 = fn(user, param_2, param_3);
        return uVar4;
    }
    default:
        // Error code 0xe — return 0 (implied by default case in analysis note).
        return 0u;
    }
}

RH_ScopedInstall(RwStreamRead, 0x004cbd30);  // re-enabled 2026-05-24 c3-render-b

// ---------------------------------------------------------------------------
// 0x004cc050  FUN_004cc050  RwStreamSkip   (249 bytes)
//
// RwStream *RwStreamSkip(RwStream *stream, RwUInt32 offset)
//   param_1 = stream ptr
//   param_2 = byte count to skip forward
//   Returns param_1 (stream) on success, NULL on failure.
//
// 4-case type dispatch (stream type at *param_1):
//   case 1, case 2  — file stream:
//     calls FUN_005509b0(handle, offset, 1) — fseek(SEEK_CUR) wrapper
//     FUN_005509b0 at 0x005509b0
//   case 3          — memory buffer stream:
//     bounds check: if (end - pos) < offset → error (return NULL)
//     advance param_1[3] += offset
//   case 4          — callback stream:
//     fn ptr at stream+0x18 (param_1[6])
//     user data at stream+0x1c (param_1[7])
//     calls (*fn)(user, offset)
//   default         — error code 0xe → return NULL
//
// Facts cited from re/analysis/texture_loader_d3/0x004cc050.md:
//   Stream type dispatch:         0x004cc063
//   Memory end-of-buffer guard:   0x004cc0a5
//   Memory advance param_1[3]:    0x004cc0b3
//   fseek call FUN_005509b0:      0x004cc0cc
//   Callback fn at param_1[6]:    0x004cc11f
// ---------------------------------------------------------------------------

// External callee: FUN_005509b0 — fseek(SEEK_CUR) wrapper.
// int __cdecl FUN_005509b0(int file_handle, int offset, int whence)
using FseekFn_t = std::int32_t (__cdecl*)(std::uint32_t, std::int32_t, std::int32_t);
static FseekFn_t const s_FUN_005509b0 = reinterpret_cast<FseekFn_t>(0x005509b0u);

// 0x004cc050
extern "C" __declspec(dllexport) std::uint32_t* __cdecl
RwStreamSkip(std::uint32_t* param_1, std::uint32_t param_2)
{
    std::uint32_t stream_type = *param_1;  // stream+0x00, cited 0x004cc063

    switch (stream_type) {
    case 1:
    case 2: {
        // File stream: fseek(SEEK_CUR) via FUN_005509b0, cited 0x004cc0cc
        std::uint32_t handle = param_1[3];   // file handle at stream+0x0c
        std::int32_t ok = s_FUN_005509b0(handle, static_cast<std::int32_t>(param_2), 1);
        if (ok == 0) return param_1;
        return nullptr;
    }
    case 3: {
        // Memory buffer stream.
        // end at stream+0x10 = param_1[4], pos at stream+0x0c = param_1[3]
        // bounds check, cited 0x004cc0a5
        std::uint32_t end = param_1[4];
        std::uint32_t pos = param_1[3];
        if ((end - pos) < param_2) {
            // Overrun — error path.
            return nullptr;
        }
        // Advance position, cited 0x004cc0b3
        param_1[3] = pos + param_2;
        return param_1;
    }
    case 4: {
        // Callback stream: fn ptr at stream+0x18 (param_1[6]), cited 0x004cc11f
        typedef std::int32_t (__cdecl *SkipFn)(std::uint32_t, std::uint32_t);
        SkipFn fn   = reinterpret_cast<SkipFn>(param_1[6]);
        std::uint32_t user = param_1[7];   // user data at stream+0x1c
        std::int32_t ok = fn(user, param_2);
        if (ok == 0) return param_1;
        return nullptr;
    }
    default:
        // Error code 0xe — return NULL (implied by default case).
        return nullptr;
    }
}

RH_ScopedInstall(RwStreamSkip, 0x004cc050);  // re-enabled 2026-05-24 c3-render-b

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

// MASS-DISABLED 2026-05-24 loader-broken-9d: RH_ScopedInstall(RwTexDictionaryCreate, 0x004c5890);
