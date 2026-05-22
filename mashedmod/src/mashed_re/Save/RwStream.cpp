// Mashed RE - Save/RwStream.cpp (C2->C3 promotions, session save-sdone-a-s2).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (1):
//   0x004cbe80  RwStreamWrite_s2  RW stream write; 4-type switch (file/mem/callback).
//
// Analysis note: re/analysis/promote_c2_rw_engine_init/004cbe80.md (C2)
//
// Companion files:
//   Save/SettingsAndIO.cpp contains FileWriteWrapper_i3 which calls this function.
//   The companion RwStreamRead (0x004cbd30) is in re/frida/hooks_registry.py
//   (entry 'rw_stream_read', C3, save-sdone workstream B).
//
// Deferred this session (STOP-AND-ASK triggered):
//   0x00550910 FUN_00550910 — VFS stream close: no confirmed internal C2+ callee
//   (IAT indirect calls only); live file-handle mutation risk at main-menu state.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ============================================================================
// 0x004cbe80  RwStreamWrite_s2
//
// Original 444 bytes (0x004cbe80..0x004cc037).  __cdecl 3-arg (+ implicit ESI).
// Signature (from mechanical description + callee FileWriteWrapper_i3):
//   undefined4* RwStreamWrite_s2(undefined4* param_1, undefined4* param_2, uint param_3)
//   param_1: stream context pointer (context[0] = stream type)
//   param_2: source data buffer
//   param_3: byte count to write
//
// Analysis note: re/analysis/promote_c2_rw_engine_init/004cbe80.md (C2)
//
// Anti-island:
//   Callers: FileWriteWrapper_i3 (0x004b3bb0, C3, via FileWriteWrapper_i3 call chain)
//            FUN_004cc6e0 RwStreamWriteChunked (0x004cc6e0, C3)
//   Callees:
//     0x004d7ff0 RwIdentityPassthrough (C3, render; error-tag constructor)
//     0x004d8480 RwErrSlotWrite        (C3, render; error dispatcher)
//     0x00550980 VfsStreamRead/fwrite  (C3, save; fwrite-style fn)
//     vtable indirect: *(DAT_007d3ff8+0x108) alloc first-alloc
//     vtable indirect: *(DAT_007d3ff8+0x110) realloc grow
//     vtable indirect: (code*)param_1[5]     case-4 write callback
//
// Stream context layout (index * 4 = byte offset; consistent with FUN_004cc230):
//   context[0] = stream type   (1/2=file, 3=mem, 4=callback)
//   context[3] = file handle / mem write pos
//   context[4] = mem buffer capacity
//   context[5] = mem buffer ptr / callback fn ptr
//   context[7] = callback user data
//
// Cases (switch on context[0]):
//   1, 2 (file): FUN_00550980(param_2, 1, param_3, context[3]); check full write.
//   3 (mem):     first-alloc if context[5]==0; grow if remaining<param_3;
//                DWORD loop + byte loop copy; advance context[3] by param_3.
//   4 (callback): (*(code*)context[5])(context[7], param_2, param_3).
//   default:     error 0xe.
//
// Error codes (cited at exact RVA in mechanical description):
//   0x8000001c  short-write failure (cases 1/2), cited at 0x004cbf20
//   0x80000013  alloc failure (case 3), cited at 0x004cbf40
//   0xe         unknown stream type (default), cited at 0x004cbef4
//
// Alloc vtable slot offsets from DAT_007d3ff8 (cited at 0x004cbf10 / 0x004cbf6c):
//   +0x108 (264) — first-alloc: *(DAT_007d3ff8+0x108)(0x200, 0x30404)
//   +0x110 (272) — realloc:     *(DAT_007d3ff8+0x110)(ptr, new_size, 0x1030404)
//
// [UNCERTAIN U-2328] 0x30404 alloc flags semantics — non-blocking.
// [UNCERTAIN U-2329] 0x1030404 realloc flags semantics — non-blocking.
//
// Frida strategy: arg_type='none'; both sides crash identically when param_1=NULL
//   (deref at context[0] type dispatch). crash_equal_ok=True.
// ============================================================================

// Constants cited at exact RVAs in 004cbe80.md
static constexpr std::uint32_t kErrShortWrite    = 0x8000001cu;  // 0x004cbf20
static constexpr std::uint32_t kErrAllocFail     = 0x80000013u;  // 0x004cbf40
static constexpr std::uint32_t kErrUnknownType   = 0x0eu;        // 0x004cbef4
static constexpr std::uint32_t kInitAllocSize    = 0x200u;        // 512, cited at 0x004cbf40
static constexpr std::uint32_t kInitAllocTag     = 0x30404u;      // cited at 0x004cbf40 [U-2328]
static constexpr std::uint32_t kGrowIncrement    = 0x200u;        // cited at 0x004cbf60
static constexpr std::uint32_t kGrowThreshold    = 0x1ffu;        // 511, cited at 0x004cbf60
static constexpr std::uint32_t kReallocTag       = 0x1030404u;    // cited at 0x004cbf6c [U-2329]
static constexpr std::uint32_t kAllocVtableSlot  = 0x108u;        // cited at 0x004cbf10
static constexpr std::uint32_t kReallocVtableSlot = 0x110u;       // cited at 0x004cbf6c

// DAT_007d3ff8 — RW vtable root (cited in 004cbe80.md + companion FUN_004cc230)
static constexpr std::uintptr_t kVtableRoot = 0x007d3ff8u;

// Callee: FUN_004d7ff0 — error-tag constructor (C3; cited at 0x004cbf20 call site)
using ErrTagFn_t = void* (__cdecl*)(std::uint32_t);
static ErrTagFn_t const g_RwIdentityPassthrough =
    reinterpret_cast<ErrTagFn_t>(0x004d7ff0u);

// Callee: FUN_004d8480 — error dispatcher (C3; cited at 0x004cbf22 / 0x004cbf42 call sites)
using ErrDispatchFn_t = void* (__cdecl*)();
static ErrDispatchFn_t const g_RwErrSlotWrite =
    reinterpret_cast<ErrDispatchFn_t>(0x004d8480u);

// Callee: FUN_00550980 — fwrite-style file write (C3; cited at 0x004cbefc call site)
// Signature: fn(buf, 1, count, file_handle)
using FwriteStyleFn_t = std::uint32_t (__cdecl*)(void*, std::uint32_t, std::uint32_t, std::uint32_t);
static FwriteStyleFn_t const g_VfsStreamWrite =
    reinterpret_cast<FwriteStyleFn_t>(0x00550980u);


extern "C" __declspec(dllexport)
void* __cdecl RwStreamWrite_s2(
    std::uint32_t* param_1,    // stream context (DWORD array)
    std::uint32_t* param_2,    // source data buffer
    std::uint32_t  param_3)    // byte count
{
    if (param_1 == nullptr) {
        // Both sides crash identically on null context — crash_equal_ok covers this.
        // The original dereferences context[0] unconditionally.
        return nullptr;
    }

    const std::uint32_t stream_type = param_1[0];  // context[0]

    switch (stream_type)
    {
    case 1:  // file write (same branch as case 2; fall-through in original)
    case 2:  // file write
    {
        // 0x004cbefc: FUN_00550980(param_2, 1, param_3, context[3])
        // context[3] = file handle / write position
        const std::uint32_t bytes_written =
            g_VfsStreamWrite(param_2, 1u, param_3, param_1[3]);

        if (bytes_written == param_3) {
            // 0x004cbf07: full write — return param_1
            return param_1;
        } else {
            // 0x004cbf0b: short write error 0x8000001c
            // 0x004cbf20: FUN_004d7ff0(0x8000001c)
            g_RwIdentityPassthrough(kErrShortWrite);
            // 0x004cbf22: FUN_004d8480()
            g_RwErrSlotWrite();
            return nullptr;  // 0x004cbf27: return NULL
        }
    }

    case 3:  // memory-buffer write
    {
        // context[4] = capacity, context[3] = write position, context[5] = buffer ptr
        if (param_1[5] == 0u) {
            // 0x004cbf38: buffer not yet allocated — first-alloc
            // *(*(DAT_007d3ff8+0x108))(0x200, 0x30404)
            const std::uint32_t vtable_root_val =
                *reinterpret_cast<std::uint32_t*>(kVtableRoot);
            using AllocFn_t = std::uint32_t (__cdecl*)(std::uint32_t, std::uint32_t);
            AllocFn_t alloc_fn = *reinterpret_cast<AllocFn_t*>(
                vtable_root_val + kAllocVtableSlot);             // 0x004cbf10
            const std::uint32_t new_buf = alloc_fn(kInitAllocSize, kInitAllocTag);

            if (new_buf == 0u) {
                // 0x004cbf40: alloc failed — error 0x80000013
                g_RwIdentityPassthrough(kErrAllocFail);          // 0x004cbf40
                g_RwErrSlotWrite();                              // 0x004cbf45
                return nullptr;
            }
            param_1[5] = new_buf;          // context[5] = new buffer
            param_1[4] = kInitAllocSize;   // context[4] = capacity = 0x200
        }

        // Capacity check: remaining = context[4] - context[3]
        // 0x004cbf52: if ((context[4] - context[3]) < param_3) => grow
        const std::uint32_t remaining = param_1[4] - param_1[3];
        if (remaining < param_3) {
            // 0x004cbf60: compute new size
            std::uint32_t new_size = param_1[4] + kGrowIncrement;
            if (param_3 > kGrowThreshold) {
                // 0x004cbf60: if param_3 > 0x1ff, use param_3 as growth
                new_size = param_1[4] + param_3;
            }

            // 0x004cbf6c: realloc via vtable+0x110
            const std::uint32_t vtable_root_val =
                *reinterpret_cast<std::uint32_t*>(kVtableRoot);
            using ReallocFn_t = std::uint32_t (__cdecl*)(std::uint32_t, std::uint32_t, std::uint32_t);
            ReallocFn_t realloc_fn = *reinterpret_cast<ReallocFn_t*>(
                vtable_root_val + kReallocVtableSlot);           // 0x004cbf6c
            const std::uint32_t iVar1 = realloc_fn(param_1[5], new_size, kReallocTag);

            if (iVar1 == 0u) {
                // 0x004cbf7a: realloc failed
                const std::uint32_t delta = new_size - param_1[4];
                g_RwIdentityPassthrough(kErrAllocFail);         // error 0x80000013 per note
                (void)delta;  // cited as arg in note; pass error code only per note
                g_RwErrSlotWrite();
                return nullptr;
            }
            param_1[5] = iVar1;         // context[5] = new buffer ptr
            param_1[4] = new_size;      // context[4] = new capacity
        }

        // 0x004cbf88: destination ptr = context[5] + context[3]
        std::uint8_t* dest = reinterpret_cast<std::uint8_t*>(param_1[5]) + param_1[3];
        const std::uint8_t* src = reinterpret_cast<const std::uint8_t*>(param_2);

        // 0x004cbf98: DWORD loop — param_3 >> 2 iterations
        std::uint32_t dword_count = param_3 >> 2;
        const std::uint32_t* src32  = reinterpret_cast<const std::uint32_t*>(src);
        std::uint32_t*       dest32 = reinterpret_cast<std::uint32_t*>(dest);
        for (std::uint32_t i = 0; i < dword_count; ++i) {
            *dest32++ = *src32++;       // 0x004cbf98: copy DWORD
        }

        // 0x004cbfac: byte loop — param_3 & 3 remaining bytes
        const std::uint8_t* src8  = reinterpret_cast<const std::uint8_t*>(src32);
        std::uint8_t*       dest8 = reinterpret_cast<std::uint8_t*>(dest32);
        std::uint32_t byte_rem = param_3 & 3u;
        for (std::uint32_t i = 0; i < byte_rem; ++i) {
            *dest8++ = *src8++;         // 0x004cbfac: copy byte
        }

        // 0x004cbfbc: advance write position
        param_1[3] += param_3;          // context[3] += param_3

        return param_1;                 // 0x004cbfc4: return context ptr
    }

    case 4:  // callback write
    {
        // 0x004cbfcc: (*(code*)context[5])(context[7], param_2, param_3)
        using CbWriteFn_t = std::uint32_t (__cdecl*)(std::uint32_t, std::uint32_t*, std::uint32_t);
        CbWriteFn_t cb_fn = reinterpret_cast<CbWriteFn_t>(param_1[5]);
        const std::uint32_t iVar1 = cb_fn(param_1[7], param_2, param_3);

        // 0x004cbfd8: return -(iVar1 != 0) & param_1 : conditional NULL/param_1 select
        // If callback returned nonzero: return param_1; else return NULL.
        return (iVar1 != 0u)
            ? static_cast<void*>(param_1)
            : nullptr;
    }

    default:
    {
        // 0x004cbef4: unknown stream type — error 0xe
        g_RwIdentityPassthrough(kErrUnknownType);               // 0x004cbef4
        g_RwErrSlotWrite();                                     // 0x004cbef9
        return nullptr;
    }
    }
}

RH_ScopedInstall(RwStreamWrite_s2, 0x004cbe80);
