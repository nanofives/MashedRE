// Mashed RE — timer base-pointer getter.
// 0x00413f90  FUN_00413f90  timer_d3  C2 (promote_c2_util_timer-20260513)
//
// Asm (6 bytes: 5-byte MOV + 1-byte RET + NOP padding):
//   B8 10 2B 5F 00  MOV EAX, 0x005f2b10
//   C3              RET
//
// Returns the address of DAT_005f2b10 as an opaque pointer.
// Callers (0x0043d7c0, 0x0043dfd0) treat the return value as a struct base ptr.
#include "../Core/HookSystem.h"

extern "C" __declspec(dllexport) void* __cdecl TimerGetBasePtr() {
    return reinterpret_cast<void*>(0x005f2b10);
}

RH_ScopedInstall(TimerGetBasePtr, 0x00413f90);
