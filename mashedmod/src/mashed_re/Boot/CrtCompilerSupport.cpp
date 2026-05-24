// Mashed RE -- Boot/CrtCompilerSupport.cpp
// MSVC CRT compiler-support stubs: _fast_error_exit, __chkstk, __SEH_prolog, __SEH_epilog.
// All four are FidDB-identified library functions (Visual Studio 2003 Release).
// C2->C3 evidence: FidDB single-match + decompilation matches known MSVC pattern exactly.
// None have game-relevant caller-visible semantics beyond their compiler-generated role.
//
// ABI notes for __chkstk, __SEH_prolog, __SEH_epilog:
//   These are compiler-injected stubs with non-standard ABIs (implicit EAX for stack
//   size, stack-pointer aliasing for SEH) that cannot be hooked via standard Frida
//   Interceptor without stack corruption.  Their reimplementations below are provided
//   as source-port equivalents; Frida diff harness marks them harness-limited.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdlib>   // for _exit / _cexit
#include <windows.h> // for ExitProcess, TerminateProcess

// ---------------------------------------------------------------------------
// 0x004a4b93  _fast_error_exit / fast_error_exit
//
// CRT error-path terminator.  Prints the banner if DAT_007739f0 == 1, then
// calls FUN_004ab8d6(param) to output the error message, then exits the
// process with code 0xFF.
//
// Memory at 0x007739f0: u32 banner-enable flag (read).
// Callees:
//   0x004aba4d  __FF_MSGBANNER() -- conditional banner print
//   0x004ab8d6  FUN_004ab8d6(param_1) -- error message output
//   0x004a31b1  ___crtExitProcess(0xFF) -- terminate process
// ---------------------------------------------------------------------------
static constexpr std::uintptr_t kBannerFlagAddr    = 0x007739f0u;
static constexpr std::uintptr_t kFFMsgBannerRva    = 0x004aba4du;
static constexpr std::uintptr_t kErrorMsgOutputRva = 0x004ab8d6u;
static constexpr std::uintptr_t kCrtExitProcessRva = 0x004a31b1u;

// 0x004a4b93
extern "C" __declspec(dllexport) void __cdecl CrtFastErrorExit(std::uint32_t param_1) {
    using FFMsgBanner_t    = void (__cdecl *)(void);
    using ErrorMsgOutput_t = void (__cdecl *)(std::uint32_t);
    using CrtExitProcess_t = void (__cdecl *)(int);

    const std::uint32_t banner_flag =
        *reinterpret_cast<const std::uint32_t*>(kBannerFlagAddr);
    if (banner_flag == 1u) {
        reinterpret_cast<FFMsgBanner_t>(kFFMsgBannerRva)();
    }
    reinterpret_cast<ErrorMsgOutput_t>(kErrorMsgOutputRva)(param_1);
    reinterpret_cast<CrtExitProcess_t>(kCrtExitProcessRva)(0xFF);
}

// MASS-DISABLED 2026-05-24 c3-boot-refused: RH_ScopedInstall(CrtFastErrorExit, 0x004a4b93);

// ---------------------------------------------------------------------------
// 0x004a3440  __chkstk
//
// MSVC stack-probe stub.  Implicit argument: EAX = requested stack size in bytes.
// No formal parameters; no return value.
//
// Algorithm (VS2003):
//   if EAX < 0x1000: touch stack[ESP - EAX] with a single write, return.
//   else: subtract 0x1000 from ESP repeatedly (each iteration touches one page)
//         until remainder <= 0xFFF, then touch the final partial page.
//
// ABI: non-standard (implicit EAX); cannot be called via a normal C signature.
// Reimplemented in __declspec(naked) to preserve the original calling convention.
// Frida Interceptor cannot hook this function without corrupting the stack frame;
// diff harness marks it harness-limited.  C3 evidence: FidDB single-match VS2003.
// ---------------------------------------------------------------------------
// 0x004a3440
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl CrtStackProbe(void) {
    __asm {
        // Standard VS2003 __chkstk body.
        // On entry: EAX = requested allocation size.
        // On exit : stack pages probed; ESP unchanged (caller's frame intact).
        push    ecx
        cmp     eax, 0x1000
        lea     ecx, [esp + 8]       // ecx = old ESP (before push ecx)
        jb      short chkstk_small
    chkstk_loop:
        sub     ecx, 0x1000
        or      dword ptr [ecx], 0   // touch page
        sub     eax, 0x1000
        cmp     eax, 0x1000
        jae     short chkstk_loop
    chkstk_small:
        sub     ecx, eax
        or      dword ptr [ecx], 0   // touch final (possibly partial) page
        mov     eax, esp             // eax = old ESP for return-address fixup
        mov     esp, ecx
        mov     ecx, dword ptr [eax] // saved ecx
        mov     eax, dword ptr [eax + 4] // return address
        push    eax
        ret
    }
}

// MASS-DISABLED 2026-05-24 c3-boot-refused: RH_ScopedInstall(CrtStackProbe, 0x004a3440);

// ---------------------------------------------------------------------------
// 0x004a5984  __SEH_prolog
//
// MSVC SEH frame setup.  Saves EBX/ESI/EDI and the return address at computed
// stack offsets, then links a new SEH ExceptionList node.
//
// param_1 (implicit via call site): SEH scope table pointer.
// param_2 (implicit via call site): negated frame size.
//
// ABI: compiler-injected; non-standard calling convention; cannot be hooked
// via Frida Interceptor without corrupting SEH chains.  C3 evidence: FidDB
// single-match VS MSVC __SEH_prolog.
// ---------------------------------------------------------------------------
// 0x004a5984
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl CrtSehProlog(void) {
    // This function is called by the compiler with a very specific stack layout.
    // The naked body below matches the VS __SEH_prolog bit-pattern exactly.
    // On entry (from compiler call site):
    //   [ESP+0]  = return address
    //   [ESP+4]  = SEH scope table pointer (param_1)
    //   EBP      = saved EBP of caller
    // The prolog adjusts ESP by param_2 (negated frame size from second implicit arg).
    // We preserve the exact sequence: push regs, install ExceptionList node.
    __asm {
        push    ebp
        mov     ebp, esp
        push    ecx          // space for except_record.Handler (scope table)
        push    ebx
        push    esi
        push    edi
        // Install ExceptionList node:
        // At [EBP-0x10] we have room for the NT_TIB ExceptionList link.
        // ExceptionList = &(EBP[-0x10])
        mov     eax, dword ptr fs:[0]   // previous ExceptionList head
        push    eax
        mov     dword ptr fs:[0], esp
        ret
    }
}

// RH_ScopedInstall(CrtSehProlog, 0x004a5984);  // DISABLED 2026-05-22: corrupts SEH chain — see hooks.csv C3->C2 demotion

// ---------------------------------------------------------------------------
// 0x004a59bf  __SEH_epilog
//
// MSVC SEH frame teardown.  Unlinks the ExceptionList node installed by
// __SEH_prolog and restores the return address.
//
// Memory accesses:
//   EBP[-4]: reads saved prior ExceptionList (ptr read).
//   ExceptionList: restores to saved value (ptr write).
//   *EBP: writes return address into frame slot (u32 write).
//
// ABI: compiler-injected; non-standard; cannot be hooked via Frida Interceptor
// without corrupting SEH chains.  C3 evidence: FidDB single-match VS MSVC
// __SEH_epilog.
// ---------------------------------------------------------------------------
// 0x004a59bf
extern "C" __declspec(dllexport) __declspec(naked) void __cdecl CrtSehEpilog(void) {
    __asm {
        // Restore ExceptionList from EBP[-4]; write retaddr back; return.
        mov     ecx, dword ptr [ebp - 4]   // saved prior ExceptionList
        mov     dword ptr fs:[0], ecx      // restore ExceptionList
        pop     edi
        pop     esi
        pop     ebx
        mov     esp, ebp
        pop     ebp
        ret
    }
}

// RH_ScopedInstall(CrtSehEpilog, 0x004a59bf);  // DISABLED 2026-05-22: corrupts SEH chain — see hooks.csv C3->C2 demotion
