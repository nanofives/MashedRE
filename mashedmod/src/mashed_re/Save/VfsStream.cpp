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
// ABI confirmed via disassembly (Ghidra pool10 2026-05-22):
//   00550980: PUSH ESI
//   00550981: MOV ESI, [ESP+0xC]        ; ESI = element_size (param_2)
//   00550985: MOV EDX, ESI
//   00550987: MOV ECX, [ESP+0x14]       ; ECX = ctx (param_4, NOT thiscall)
//   0055098B: IMUL EDX, [ESP+0x10]      ; EDX = element_size * count (total_bytes)
//   00550990: MOV EAX, [ECX+0x38]       ; EAX = *(ctx+0x38)
//   00550993: PUSH EDX                  ; push total_bytes
//   00550994: MOV EDX, [ESP+0xC]        ; EDX = buf (param_1, re-read from stack)
//   00550998: ADD EAX, 0x28             ; EAX = *(ctx+0x38) + 0x28
//   0055099B: PUSH EDX                  ; push buf
//   0055099C: PUSH ECX                  ; push ctx
//   0055099D: CALL [EAX+0xC]            ; call *(*(ctx+0x38)+0x28+0x0C)(ctx, buf, total_bytes)
//   005509A0: ADD ESP, 0xC              ; cdecl caller cleanup (3 args)
//   005509A3: XOR EDX, EDX
//   005509A5: DIV ESI                   ; EAX = EAX / element_size
//   005509A7: POP ESI
//   005509A8: RET
//
// Stack args at call site (cdecl, caller pushes right-to-left):
//   param_1 = buf         [ESP+4] before PUSH ESI / [ESP+8] inside
//   param_2 = element_size [ESP+8] / [ESP+C] inside
//   param_3 = count        [ESP+C] / [ESP+10] inside
//   param_4 = ctx          [ESP+10] / [ESP+14] inside
//
// Dispatch: fn = *(void**)(*(char**)(ctx+0x38) + 0x28 + 0x0C)
//           fn(ctx, buf, total_bytes)  — 3 cdecl stack args
extern "C" __declspec(dllexport) int __cdecl VfsStreamRead(
    void* buf,
    int   element_size,
    int   count,
    void* ctx)
{
    // 0x0055098B: total_bytes = element_size * count
    const int total_bytes = element_size * count;

    // 0x00550990: base = *(ctx+0x38)
    // 0x00550998: fn_slot = base + 0x28 + 0x0C  (single pointer read, not vtable double-deref)
    auto* ctx_bytes = reinterpret_cast<std::uint8_t*>(ctx);
    void* base;
    std::memcpy(&base, ctx_bytes + 0x38, sizeof(base));
    auto* fn_slot = reinterpret_cast<std::uint8_t*>(base) + 0x28 + 0x0C;

    // 0x0055099D: CALL [EAX+0xC] — read function pointer directly from fn_slot
    using ReadFn = int(__cdecl*)(void*, void*, int);
    ReadFn read_fn;
    std::memcpy(&read_fn, fn_slot, sizeof(read_fn));

    // 0x0055099C/9B/93: call fn(ctx, buf, total_bytes)
    const int bytes_read = read_fn(ctx, buf, total_bytes);

    // 0x005509A5: return bytes_read / element_size
    if (element_size == 0) return 0;
    return bytes_read / element_size;
}

RH_ScopedInstall(VfsStreamRead, 0x00550980);  // re-enabled 2026-05-24 c3-safe

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

RH_ScopedInstall(VfsStreamGetType, 0x00550bc0);  // re-enabled 2026-05-24 batch-save-f
