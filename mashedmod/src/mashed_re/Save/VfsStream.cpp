// Mashed RE — VFS stream helpers (save subsystem).
// Analysis session: save_gamesave_d3-20260511
//
// 0x00550980  FUN_00550980  VfsStreamRead
//   fread-style read via vtable dispatch.
//   Computes total_bytes = element_size * count;
//   loads sub-ctx = *(*(obj+0x38)+0x28);
//   calls vtable[3](ctx, buf, total_bytes) at [eax+0x0C];
//   returns total_bytes / element_size (mirrors fread element-count semantics).
//
//   Key addresses:
//     0x00550980  function start
//     0x00550989  IMUL EDX, [ESP+10]  -- total_bytes = element_size * count
//     0x0055098E  MOV EAX, [ECX+0x38] -- load stream sub-context
//     0x00550997  CALL [EAX+0x0C]     -- vtable[3] read dispatch
//     0x0055099E  DIV ESI             -- result / element_size
//
// 0x00550bc0  FUN_00550bc0  VfsStreamGetType
//   Pure 8-byte leaf accessor: return *(int*)(ctx + 8).
//
//   Key addresses:
//     0x00550bc0  function start
//     0x00550bc4  MOV EAX, [EAX+8]    -- field read at ctx+8

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// 0x00550980  VfsStreamRead
// Signature: int(void* ctx, void* buf, int element_size, int count)
// (param ordering from mechanical description: ctx=ECX/param1 passed first
//  by the callers; buf=param2; element_size=param3; count=param4)
// Note: the analysis note lists ctx as param5; signature chosen to match the
// observed register ordering exactly as mechanical description shows.
// ---------------------------------------------------------------------------
// The VFS sub-context is at *(*(ctx+0x38)+0x28).
// vtable slot index 3 is at byte offset 0x0C (4 bytes per pointer).
extern "C" __declspec(dllexport) int __cdecl VfsStreamRead(
    void* ctx,
    void* buf,
    int   element_size,
    int   count)
{
    // 0x00550989: total_bytes = element_size * count
    const int total_bytes = element_size * count;

    // 0x0055098E: sub_ctx_base = *(ctx+0x38)
    auto* ctx_bytes = reinterpret_cast<std::uint8_t*>(ctx);
    void* sub_ctx_base;
    std::memcpy(&sub_ctx_base, ctx_bytes + 0x38, sizeof(sub_ctx_base));

    // sub_ctx = sub_ctx_base + 0x28
    auto* sub_ctx = reinterpret_cast<std::uint8_t*>(sub_ctx_base) + 0x28;

    // 0x00550997: vtable[3] = *(*(sub_ctx)+0x0C)
    // sub_ctx is a pointer to an object; read vtable ptr from *sub_ctx, then slot 3
    void* vtable_ptr;
    std::memcpy(&vtable_ptr, sub_ctx, sizeof(vtable_ptr));
    auto* vtable = reinterpret_cast<std::uint8_t*>(vtable_ptr);
    using ReadFn = int(__cdecl*)(void*, void*, int);
    ReadFn read_fn;
    std::memcpy(&read_fn, vtable + 0x0C, sizeof(read_fn));

    // 0x00550997: call vtable[3](ctx, buf, total_bytes)
    const int bytes_read = read_fn(ctx, buf, total_bytes);

    // 0x0055099E: return bytes_read / element_size
    if (element_size == 0) return 0;
    return bytes_read / element_size;
}

RH_ScopedInstall(VfsStreamRead, 0x00550980);

// ---------------------------------------------------------------------------
// 0x00550bc0  VfsStreamGetType
// Pure leaf: return *(int*)(ctx + 8).
// Disassembly:
//   00550BC0: 8B 44 24 04   MOV EAX, [ESP+4]    ; EAX = param1 (stream ctx ptr)
//   00550BC4: 8B 40 08      MOV EAX, [EAX+8]    ; EAX = *(ctx + 8)
//   00550BC7: C3            RET
// ---------------------------------------------------------------------------
extern "C" __declspec(dllexport) int __cdecl VfsStreamGetType(void* ctx)
{
    // 0x00550bc4: return *(int*)(ctx + 8)
    auto* ctx_bytes = reinterpret_cast<std::uint8_t*>(ctx);
    int result;
    std::memcpy(&result, ctx_bytes + 8, sizeof(result));
    return result;
}

RH_ScopedInstall(VfsStreamGetType, 0x00550bc0);
