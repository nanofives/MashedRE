// Mashed RE - Boot/Boot_j5 cluster (C2->C3 promotions, session c3-batch-j-s5).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (3 — 1 boot RVA refused, see report):
//   0x004a31b1  CrtExitProcess_j5    CRT process terminator (CLR-aware exit)
//   0x004a332b  CrtExitNoReturn_j5   no-return exit wrapper to CrtExitCore
//   0x004a334d  CrtExitNormal_j5     normal-return exit wrapper to CrtExitCore
//
// Analysis notes (per CLAUDE.md NO-GUESSING rule):
//   re/analysis/promote_c2_boot_crt/004a31b1.md
//   re/analysis/promote_c2_boot_crt/004a332b.md
//   re/analysis/promote_c2_boot_crt/004a334d.md
//
// All three are leaf-ish CRT exit helpers. 0x004a31b1 has only Win32 externals
// (GetModuleHandleA / GetProcAddress / ExitProcess). 0x004a332b and 0x004a334d
// are trivial one-call wrappers to CrtExitCore (0x004a3258 — already C3).
//
// At quiescent main-menu state none of these fire; both sides return identical
// values when called synthetically (or AV identically with crash_equal_ok).
//
// Refusal record (see PROMOTION_QUEUE.md row for batch-j-s5):
//   - 0x004a4bb7 entry — PE entry-point. We cannot hook the OS loader's
//     execution of this function (it has already run by the time DllMain
//     fires). 17 internal callees including several C0/C1 (FUN_004a8a04,
//     FUN_004a78b0, FUN_004ac04a, FUN_004abe86, FUN_00492370 — WinMain,
//     etc.). Refuse on both feasibility and callee-gate.

#include "../Core/HookSystem.h"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


// ============================================================================
// Game function pointers (cited at exact RVA)
// ============================================================================

// 0x004a3258  CrtExitCore  (C3 — see Boot/CrtStartup.cpp).
// Signature: void __cdecl CrtExitCore(int exit_code, int param_2, int mode_flag)
// Called by 0x004a332b with (param_1, 0, 0) and by 0x004a334d with (0, 0, 1).
using CrtExitCoreFn_t = void (__cdecl*)(int, int, int);
static CrtExitCoreFn_t const g_CrtExitCore =
    reinterpret_cast<CrtExitCoreFn_t>(0x004a3258u);


// ============================================================================
// 0x004a31b1  CrtExitProcess_j5
//
// FidDB-matched: ___crtExitProcess (VS2003 Release).
// Body cited at 0x004a31b1.md:
//   - GetModuleHandleA("mscoree.dll")     -> hModule
//   - if hModule != NULL:
//       GetProcAddress(hModule, "CorExitProcess") -> pFn
//       if pFn != NULL: (*pFn)(exit_code)   ; CLR-aware exit if CLR loaded
//   - ExitProcess(exit_code)               ; always reached; non-returning
//
// String literals cited at 0x004a31b1 body:
//   "mscoree.dll"        — arg to GetModuleHandleA
//   "CorExitProcess"     — arg to GetProcAddress
//
// No globals read or written. All state via parameters and Win32 calls.
// ============================================================================
extern "C" __declspec(dllexport)
void __cdecl CrtExitProcess_j5(int exit_code)
{
    HMODULE hModule = GetModuleHandleA("mscoree.dll");                   // 0x004a31b1 body
    if (hModule != nullptr) {
        FARPROC pFn = GetProcAddress(hModule, "CorExitProcess");         // 0x004a31b1 body
        if (pFn != nullptr) {
            // Indirect call -> CorExitProcess(exit_code)
            reinterpret_cast<void(__cdecl*)(int)>(pFn)(exit_code);
        }
    }
    ExitProcess(static_cast<UINT>(exit_code));                            // 0x004a31b1 body
}

RH_ScopedInstall(CrtExitProcess_j5, 0x004a31b1);  // re-enabled 2026-05-24 c3-boot-b


// ============================================================================
// 0x004a332b  CrtExitNoReturn_j5
//
// Trivial wrapper. Body cited at 0x004a332b.md:
//   void FUN_004a332b(undefined4 param_1) {
//     FUN_004a3258(param_1, 0, 0);
//     return;
//   }
//
// Caller (entry at 0x004a4bb7) annotates this call /* Subroutine does not
// return */. We mirror the original ABI (the body of CrtExitCore decides
// whether to actually return; the no-return annotation is purely informational
// for the decomp).
//
// Constants cited at 0x004a332b body:
//   0 (second arg to CrtExitCore)
//   0 (third arg — mode flag = 0 selects the ExitProcess path inside core)
// ============================================================================
extern "C" __declspec(dllexport)
void __cdecl CrtExitNoReturn_j5(int exit_code)
{
    g_CrtExitCore(exit_code, 0, 0);                                       // 0x004a332b body
}

RH_ScopedInstall(CrtExitNoReturn_j5, 0x004a332b);  // re-enabled 2026-05-24 c3-boot-b


// ============================================================================
// 0x004a334d  CrtExitNormal_j5
//
// Trivial wrapper. Body cited at 0x004a334d.md:
//   void FUN_004a334d(void) {
//     FUN_004a3258(0, 0, 1);
//     return;
//   }
//
// Constants cited at 0x004a334d body:
//   0 (exit code)
//   0 (param_2)
//   1 (mode flag — selects the FUN_004a77eb(8) path inside CrtExitCore)
// ============================================================================
extern "C" __declspec(dllexport)
void __cdecl CrtExitNormal_j5()
{
    g_CrtExitCore(0, 0, 1);                                               // 0x004a334d body
}

RH_ScopedInstall(CrtExitNormal_j5, 0x004a334d);  // re-enabled 2026-05-24 c3-boot-b
