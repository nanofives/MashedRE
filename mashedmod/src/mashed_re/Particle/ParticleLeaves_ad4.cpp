// Mashed RE — C2->C3 leaf batch (c3-batch-ad session 4, HARVEST).
//
// SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//                 (preserved in original\MASHED.exe.unpatched)
//
// Three viable pure-leaf functions harvested from the 0x0049xxxx
// DirectSound/COM-threading module. The other 10 candidates in this session's
// slate (0049c6b0/0049eca0/0049ecc0/0049ef10/0049ef60/0049f030/0049f1b0/
// 0049f1e0/0049f2e0/0049f420) were SKIPPED as needs-live-state: they require a
// real COM interface / vtable / CRITICAL_SECTION that is not constructible at
// diff-attach time (CoCreateInstance factory, QueryInterface, EnterCriticalSection,
// unconditional vtable dispatch). See re/analysis/particle_promote_ad{1,5,6}/.
//
//   0x0049c690  ComField14Get   — uint __fastcall(ptr): return *(ECX+0x14)
//   0x0049f180  ComRefRelease14 — LONG __stdcall(int*): InterlockedDecrement(+0x14) Release
//   0x0049f2b0  ComRefRelease10 — LONG __stdcall(int*): InterlockedDecrement(+0x10) Release
//
// Every reimpl matches the original's calling convention (verified from the
// disassembly: c690 RET with ECX arg -> __fastcall; f180/f2b0 RET 0x4 with one
// stack arg -> __stdcall) so the inline-JMP install is ABI-correct, and is
// bit-identity verified via re/frida/run_diff_warm.py against the original.
#include "../Core/HookSystem.h"
#include <cstdint>
#include <intrin.h>   // _InterlockedDecrement (atomic dec, returns NEW value)

// ─────────────────────────────────────────────────────────────────────────────
// 0x0049c690  FUN_0049c690  ComField14Get
// __fastcall 1-arg getter. ECX = object pointer; returns the 32-bit field at
// +0x14.
//
// Disasm (anchored binary, 0x0049c690..0x0049c693):
//   8b4114    MOV EAX,dword ptr [ECX + 0x14]   ; eax = *(this + 0x14)
//   c3        RET                               ; no immediate -> reg-only args
//
// Decompiler: `undefined4 __fastcall FUN_0049c690(int param_1)
//              { return *(undefined4 *)(param_1 + 0x14); }`
//
// Exported __fastcall, so MSVC decorates the symbol as @ComField14Get@4; the
// linker pragma re-exports the undecorated name the Frida harness resolves
// (same technique as ComReleaseThunk @ 0x004a1790).
// ref: re/analysis/particle_promote_ad1/0049c690.md
// ─────────────────────────────────────────────────────────────────────────────
#pragma comment(linker, "/export:ComField14Get=@ComField14Get@4")

extern "C" __declspec(dllexport)
std::uint32_t __fastcall ComField14Get(const std::uint8_t* thisptr) {
    return *reinterpret_cast<const std::uint32_t*>(thisptr + 0x14);
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x0049f180  FUN_0049f180  ComRefRelease14
// __stdcall COM-style Release. Atomically decrements the refcount dword at byte
// offset +0x14 (param_1 + 5 in int* arithmetic). When the new count reaches 0
// (and the object pointer is non-null) it makes a __thiscall to vtable slot 7
// (offset +0x1c) with a single argument 1 (the canonical delete-this/finalise
// call). Returns the post-decrement count.
//
// Disasm (anchored binary, 0x0049f180..0x0049f1a8):
//   56          PUSH ESI
//   8b742408    MOV  ESI,[ESP+0x8]              ; param_1 (one stack arg)
//   57          PUSH EDI
//   8d4614      LEA  EAX,[ESI+0x14]             ; &refcount (byte +0x14)
//   50          PUSH EAX
//   ff1584c05c00 CALL [0x005cc084]              ; InterlockedDecrement(&refcount)
//   8bf8        MOV  EDI,EAX                     ; EDI = new count
//   85ff        TEST EDI,EDI
//   750d        JNZ  0x0049f1a3                  ; count != 0 -> skip
//   85f6        TEST ESI,ESI
//   7409        JZ   0x0049f1a3                  ; param_1 == 0 -> skip
//   8b16        MOV  EDX,[ESI]                   ; vtable = *param_1
//   6a01        PUSH 0x1                          ; arg = 1
//   8bce        MOV  ECX,ESI                      ; this = param_1 (__thiscall)
//   ff521c      CALL [EDX+0x1c]                  ; vtable[7](this=param_1, 1)
//   8bc7        MOV  EAX,EDI                      ; return new count
//   5f          POP  EDI
//   5e          POP  ESI
//   c20400      RET  0x4                          ; callee cleans 4 -> __stdcall
//
// Decompiler: `LONG FUN_0049f180(int *param_1) { LVar1 = InterlockedDecrement(
//   param_1 + 5); if ((LVar1==0) && (param_1!=0)) (**(code**)(*param_1+0x1c))(1);
//   return LVar1; }`
// ref: re/analysis/particle_promote_ad5/0049f180.md
// ─────────────────────────────────────────────────────────────────────────────
typedef void(__thiscall* ComFinaliseFn)(void* thisptr, int arg);

extern "C" __declspec(dllexport)
long __stdcall ComRefRelease14(std::uint8_t* param_1) {
    long count = _InterlockedDecrement(
        reinterpret_cast<long volatile*>(param_1 + 0x14));   // InterlockedDecrement(&*(p+0x14))
    if (count == 0 && param_1 != nullptr) {
        void** vtbl = *reinterpret_cast<void***>(param_1);   // vtable = *param_1
        reinterpret_cast<ComFinaliseFn>(vtbl[7])(param_1, 1);// [vtable+0x1c] = slot 7, this=param_1, arg 1
    }
    return count;
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x0049f2b0  FUN_0049f2b0  ComRefRelease10
// Byte-for-byte identical to ComRefRelease14 except the refcount dword sits at
// byte offset +0x10 (param_1 + 4 in int* arithmetic). Same __stdcall ABI, same
// __thiscall vtable-slot-7 / arg-1 finalise on reaching 0.
//
// Disasm (anchored binary, 0x0049f2b0..0x0049f2d8): identical to 0x0049f180
// with `8d4610  LEA EAX,[ESI+0x10]` (vs +0x14). RET 0x4 -> __stdcall.
//
// Decompiler: `LONG FUN_0049f2b0(int *param_1) { LVar1 = InterlockedDecrement(
//   param_1 + 4); if ((LVar1==0) && (param_1!=0)) (**(code**)(*param_1+0x1c))(1);
//   return LVar1; }`
// ref: re/analysis/particle_promote_ad5/0049f2b0.md
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport)
long __stdcall ComRefRelease10(std::uint8_t* param_1) {
    long count = _InterlockedDecrement(
        reinterpret_cast<long volatile*>(param_1 + 0x10));   // InterlockedDecrement(&*(p+0x10))
    if (count == 0 && param_1 != nullptr) {
        void** vtbl = *reinterpret_cast<void***>(param_1);   // vtable = *param_1
        reinterpret_cast<ComFinaliseFn>(vtbl[7])(param_1, 1);// [vtable+0x1c] = slot 7, this=param_1, arg 1
    }
    return count;
}

// ─── Hook registration ───────────────────────────────────────────────────────
// All three are bit-identity verified leaves (run_diff_warm.py, c3-batch-ad-s4).
RH_ScopedInstall(ComField14Get,   0x0049c690);
RH_ScopedInstall(ComRefRelease14, 0x0049f180);
RH_ScopedInstall(ComRefRelease10, 0x0049f2b0);
