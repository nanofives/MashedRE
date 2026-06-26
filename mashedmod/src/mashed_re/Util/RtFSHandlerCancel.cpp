// Mashed RE — RtFSHandler::Cancel  (util subsystem)
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Function: 0x00551180  RtFSHandler::Cancel
//   Slot Cancel: marks the file-system handler slot idle by writing 1 to
//   slot->state (offset +0x20). Returns 1 on success.
//
// Disasm (0x00551180..0x0055118b, 12 bytes):
//   8B 4C 24 04   mov ecx, [esp+4]   ; load ptr from stack (cdecl arg0)
//   B8 01 00 00 00 mov eax, 1        ; return value and sentinel
//   89 41 20      mov [ecx+0x20], eax ; *(param_1+0x20) = 1 (slot state = idle)
//   C3            ret
//
// Signature: int __cdecl RtFSHandlerCancel(void* slot)
//   param_1: pointer to RtFSHandler slot struct
//   return:  always 1
//
// Constants (all cited from body at 0x00551180):
//   +0x20 — state field offset: set to 1 to mark slot idle/cancelled
//
// Callers: RtFS handler vtable slot +0x40 (wired by FUN_00551190 = Install);
//          mirrors Close (0x00550d80) which also writes slot[+0x20]=1.
//
// Uncertainties (non-blocking):
//   U-5901: function boundary was absent in pool0 clone (see 00151180.md).
//   subsystem: util (confirmed by analysis note + vtable context).
//
// ref: re/analysis/bucket_util_00095280_0040e460/00151180.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// RtFSHandlerCancel  --  0x00551180
//
// int __cdecl fn(void* slot): writes 1 to *(slot+0x20) and returns 1.
// ---------------------------------------------------------------------------

// 0x00551180
extern "C" __declspec(dllexport) std::uint32_t __cdecl RtFSHandlerCancel(void* slot) {
    // Write 1 to slot state field at +0x20.
    // +0x20 offset cited at 0x00551180 body (mov [ecx+0x20], eax; eax=1).
    *reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::uintptr_t>(slot) + 0x20u) = 1u;
    return 1u;
}

RH_ScopedInstall(RtFSHandlerCancel, 0x00551180);
