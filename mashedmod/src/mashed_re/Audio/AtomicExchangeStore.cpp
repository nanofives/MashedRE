// Mashed RE - Audio: atomic 32-bit store wrapper (single C2->C3 promotion).
//
// FUN_005aefa0 is a thin wrapper around the kernel32 InterlockedExchange import
// (IAT slot 0x005cc164). It atomically stores param_2 into *param_1 and returns
// param_1 (the previous value the API leaves in EAX is discarded). cdecl, 2 args.
// Verbatim transcription of the Ghidra decompiler ([C2 2026-06-01], re-read
// read-only from Mashed_pool5 on 2026-06-25).
//
// Disassembly (9 instrs, body 005aefa0..005aefb3):
//   005aefa0 MOV  EAX,[ESP+8]        ; param_2 (2nd cdecl arg)
//   005aefa4 PUSH ESI
//   005aefa5 MOV  ESI,[ESP+8]        ; param_1 (1st cdecl arg, post-push)
//   005aefa9 PUSH EAX                ; arg2 = param_2
//   005aefaa PUSH ESI                ; arg1 = param_1
//   005aefab CALL [0x005cc164]       ; InterlockedExchange (kernel32 IAT)
//   005aefb1 MOV  EAX,ESI            ; return param_1
//   005aefb3 POP  ESI
//   005aefb4 RET                     ; cdecl (caller cleans)
#include "../Core/HookSystem.h"
#include <windows.h>

// 0x005aefa0  FUN_005aefa0  (20 bytes, body 005aefa0..005aefb3)
// Decompiler ([C2 2026-06-01]):
//   LONG * FUN_005aefa0(LONG *param_1, LONG param_2) {
//     InterlockedExchange(param_1, param_2);
//     return param_1;
//   }
// The MSVC InterlockedExchange intrinsic (inline XCHG) and the kernel32 import
// produce the identical observable effect: *param_1 <- param_2 (atomic), and the
// wrapper returns param_1.
extern "C" __declspec(dllexport) LONG* __cdecl AudioAtomicExchangeStore(LONG* param_1, LONG param_2) {
    InterlockedExchange((volatile LONG*)param_1, param_2);
    return param_1;
}

RH_ScopedInstall(AudioAtomicExchangeStore, 0x005aefa0);
