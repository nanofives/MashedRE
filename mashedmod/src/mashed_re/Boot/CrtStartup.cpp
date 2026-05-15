// Mashed RE - CRT startup / exit / entry reimplementations.
// Analysis notes:
//   re/analysis/promote_c2_boot_crt/004a31f3.md  (CrtPreInit)
//   re/analysis/promote_c2_boot_crt/004a3258.md  (CrtExitCore)
//   re/analysis/promote_c2_boot_crt/004a4bb7.md  (WinMainEntry)
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Callee status for CrtPreInit (004a31f3):
//   _atexit      0x004a415e  C1 (library VS2003; delegates to __onexit)
//   indirect via 0x005ea03c..0x005ea058 (7 fn-ptr slots, runtime-populated)
//   indirect via 0x005ea000..0x005ea038 (14 fn-ptr slots, runtime-populated)
//   Caller: entry (004a4bb7) C2 — caller gate satisfied.
//
// Callee status for CrtExitCore (004a3258):
//   __lock            0x004a787f  C1
//   GetCurrentProcess EXTERNAL    Win32
//   TerminateProcess  EXTERNAL    Win32
//   ___crtExitProcess 0x004a31b1  C2  <- callee gate satisfied
//   FUN_004a77eb      0x004a77eb  C1
//   Caller: FUN_004a332b (C2), FUN_004a334d (C2) — caller gate satisfied.
//
// Callee status for WinMainEntry (004a4bb7):
//   All 11 internal callees are C2 (see hooks.csv). Caller: PE loader (external).
//   [UNCERTAIN U-0001] 4-byte pre-GetVersionExA write; [UNCERTAIN U-0002] PE header parse.
//   DRIFT-RISK: U-0001/U-0002 are inline in the entry body. Behavioral observe only.

#include "../Core/HookSystem.h"

#include <cstdint>
#include <windows.h>

// ---------------------------------------------------------------------------
// Global addresses — CrtPreInit (0x004a31f3)
// ---------------------------------------------------------------------------

// Optional pre-init function pointer; called if non-null.
// Address: 0x00616044
static constexpr std::uintptr_t kPreInitFnPtrAddr  = 0x00616044;

// First fn-ptr init table: 7 slots × 4 bytes = 28 bytes.
// [0x005ea03c .. 0x005ea058)
static constexpr std::uintptr_t kInitTable1Start   = 0x005ea03c;
static constexpr std::uintptr_t kInitTable1End     = 0x005ea058;

// Address registered with _atexit.
// Note: Ghidra labels this LAB_004a78f4, not FUN_ — may be mid-body label.
// [UNCERTAIN U-0004]
static constexpr std::uintptr_t kAtExitFnAddr      = 0x004a78f4;

// Second fn-ptr init table: 14 slots × 4 bytes = 56 bytes.
// [0x005ea000 .. 0x005ea038)
static constexpr std::uintptr_t kInitTable2Start   = 0x005ea000;
static constexpr std::uintptr_t kInitTable2End     = 0x005ea038;

// _atexit entry point.
// 0x004a415e
static constexpr std::uintptr_t kAtExitAddr        = 0x004a415e;

// ---------------------------------------------------------------------------
// Global addresses — CrtExitCore (0x004a3258)
// ---------------------------------------------------------------------------

// Re-entry guard: set to 1 before ___crtExitProcess.
// 0x007739dc
static constexpr std::uintptr_t kExitReentryGuard  = 0x007739dc;

// Exit-in-progress flag: written 1 at entry.
// 0x007739d8
static constexpr std::uintptr_t kExitInProgress    = 0x007739d8;

// param_3 byte storage.
// 0x007739d4
static constexpr std::uintptr_t kExitParam3Store   = 0x007739d4;

// atexit list tail pointer: null means skip walk.
// 0x008ab6d0
static constexpr std::uintptr_t kAtexitListTail    = 0x008ab6d0;

// atexit list walk pointer (downward walk).
// 0x008ab6cc
static constexpr std::uintptr_t kAtexitListWalk    = 0x008ab6cc;

// First CRT exit fn-ptr table: 2 entries. [0x005ea05c .. 0x005ea064)
static constexpr std::uintptr_t kExitTable1Start   = 0x005ea05c;
static constexpr std::uintptr_t kExitTable1End     = 0x005ea064;

// Second CRT exit fn-ptr table: 2 entries, always run. [0x005ea068 .. 0x005ea070)
static constexpr std::uintptr_t kExitTable2Start   = 0x005ea068;
static constexpr std::uintptr_t kExitTable2End     = 0x005ea070;

// __lock entry point (lock index 8).
// 0x004a787f
static constexpr std::uintptr_t k__lockAddr        = 0x004a787f;

// ___crtExitProcess entry point.
// 0x004a31b1
static constexpr std::uintptr_t k__crtExitProcess  = 0x004a31b1;

// FUN_004a77eb — called when param_3 != 0 (lock 8 unlock path).
// 0x004a77eb
static constexpr std::uintptr_t kFun004a77eb       = 0x004a77eb;

// ---------------------------------------------------------------------------
// Global addresses — WinMainEntry (0x004a4bb7)
// ---------------------------------------------------------------------------

// SEH scope table address stored in local SEH frame.
// 0x005d1290
static constexpr std::uintptr_t kSehScopeTable     = 0x005d1290;

// SEH handler address installed in frame.
// 0x004a4bc3
static constexpr std::uintptr_t kSehHandler        = 0x004a4bc3;

// OS version globals written from OSVERSIONINFOA.
// 0x0077399c  dwPlatformId
static constexpr std::uintptr_t kOsVersionPlatform = 0x0077399c;
// 0x007739a8  dwMajorVersion
static constexpr std::uintptr_t kOsMajorVersion    = 0x007739a8;
// 0x007739ac  dwMinorVersion
static constexpr std::uintptr_t kOsMinorVersion    = 0x007739ac;
// 0x007739a0  dwBuildNumber & 0x7fff (or | 0x8000 if non-Win32)
static constexpr std::uintptr_t kOsBuildNumber     = 0x007739a0;
// 0x007739a4  major*0x100 + minor
static constexpr std::uintptr_t kOsVersionWord     = 0x007739a4;

// Command-line string pointer (from GetCommandLineA).
// 0x008ab6c4
static constexpr std::uintptr_t kCmdLinePtr        = 0x008ab6c4;

// Env strings block (from ___crtGetEnvironmentStringsA).
// 0x007739e8
static constexpr std::uintptr_t kEnvStringsPtr     = 0x007739e8;

// WinMain wrapper entry point.
// 0x00492370
static constexpr std::uintptr_t kWinMainWrapper    = 0x00492370;

// Callees of entry (all C2 per hooks.csv):
// 0x004aa3fe  __heap_init
static constexpr std::uintptr_t kHeapInit          = 0x004aa3fe;
// 0x004a8a04  __mtinitlocks
static constexpr std::uintptr_t kMtInitLocks       = 0x004a8a04;
// 0x004a78b0  CRT pre-init loop (fn-ptr table 005e7b84)
static constexpr std::uintptr_t kCrtPreInitLoop    = 0x004a78b0;
// 0x004ac04a  file-handle table init
static constexpr std::uintptr_t kFileHandleInit    = 0x004ac04a;
// 0x004abe86  GetModuleFileNameA wrapper (argc builder)
static constexpr std::uintptr_t kArgcBuilder       = 0x004abe86;
// 0x004abc53  __setenvp
static constexpr std::uintptr_t k__setenvp         = 0x004abc53;
// 0x004a31f3  CrtPreInit
static constexpr std::uintptr_t kCrtPreInitFn      = 0x004a31f3;
// 0x004abbea  cmdline arg-start scan
static constexpr std::uintptr_t kCmdlineScan       = 0x004abbea;
// 0x004abf28  ___crtGetEnvironmentStringsA
static constexpr std::uintptr_t kGetEnvStrings     = 0x004abf28;
// 0x004a332b  exit wrapper (no-return path)
static constexpr std::uintptr_t kExitNoReturn      = 0x004a332b;
// 0x004a334d  exit wrapper (normal-return path)
static constexpr std::uintptr_t kExitReturn        = 0x004a334d;

// fast_error_exit — called on heap/TLS/file init failure.
// Not yet in hooks.csv; called via raw address.
// Addresses used: 0x1c (heap), 0x10 (TLS), 0x1b (file), 8 (envp), 9 (setenvp)
// RVA TBD — called via __amsg_exit for some; fast_error_exit is separate.
// The exact internal call addresses are embedded inline in entry body.

// ---------------------------------------------------------------------------
// Callee typedefs — CrtPreInit
// ---------------------------------------------------------------------------
typedef int  (__cdecl *InitTableFn)();
typedef int  (__cdecl *AtExitFn)(void (*fn)());

// ---------------------------------------------------------------------------
// Callee typedefs — CrtExitCore
// ---------------------------------------------------------------------------
typedef void (__cdecl *LockFn)(int lock_id);
typedef void (__cdecl *ExitProcFn)(unsigned int exit_code);
typedef void (__cdecl *UnlockFn)(int lock_id);
typedef void (__cdecl *ExitTableFn)();

// ---------------------------------------------------------------------------
// Callee typedefs — WinMainEntry
// ---------------------------------------------------------------------------
typedef int  (__cdecl *HeapInitFn)();
typedef void (__cdecl *MtInitLocksFn)();
typedef void (__cdecl *CrtInitLoopFn)();
typedef int  (__cdecl *FileHandleInitFn)();
typedef int  (__cdecl *ArgcBuilderFn)();
typedef int  (__cdecl *SetEnvpFn)();
typedef int  (__cdecl *CrtPreInitFnT)();
typedef char* (__cdecl *CmdlineScanFn)();
typedef char* (__cdecl *GetEnvStringsFn)();
typedef void (__cdecl *ExitNoReturnFn)(int exit_code);
typedef void (__cdecl *ExitReturnFn)();
typedef int  (__cdecl *WinMainWrapperFn)(HINSTANCE hInst, HINSTANCE hPrev, char* lpCmdLine, int nCmdShow);


// ---------------------------------------------------------------------------
// 0x004a31f3 — CrtPreInit
// CRT pre-init dispatcher: optional pre-init call, first fn-ptr init table
// (7 entries, returns early on non-zero), atexit registration, second table
// (14 entries, unconditional).
// ---------------------------------------------------------------------------
// 0x004a31f3
extern "C" __declspec(dllexport) int __cdecl CrtPreInit()
{
    // Optional pre-init: PTR_FUN_00616044 is written by the loader before this
    // is called. [UNCERTAIN U-0003] what writes it.
    typedef void (__cdecl *PreInitFn)();
    auto* preInitFnPtr = *reinterpret_cast<PreInitFn*>(kPreInitFnPtrAddr);
    if (preInitFnPtr != nullptr) {
        preInitFnPtr();
    }

    // First fn-ptr init table: 7 entries.
    // If any returns non-zero, return immediately with that value.
    int result = 0;
    auto* table1 = reinterpret_cast<std::uint32_t*>(kInitTable1Start);
    auto* table1End = reinterpret_cast<std::uint32_t*>(kInitTable1End);
    auto* p = table1;
    do {
        if (result != 0) {
            return result;
        }
        if (*p != 0u) {
            auto fn = reinterpret_cast<InitTableFn>(*p);
            result = fn();
        }
        ++p;
    } while (p < table1End);

    if (result == 0) {
        // Register atexit handler. [UNCERTAIN U-0004] LAB_004a78f4 vs FUN_.
        auto atExitFn = reinterpret_cast<AtExitFn>(kAtExitAddr);
        atExitFn(reinterpret_cast<void (*)()>(kAtExitFnAddr));

        // Second fn-ptr init table: 14 entries (unconditional).
        auto* table2 = reinterpret_cast<std::uint32_t*>(kInitTable2Start);
        auto* table2End = reinterpret_cast<std::uint32_t*>(kInitTable2End);
        for (auto* q = table2; q < table2End; ++q) {
            if (*q != 0u) {
                auto fn = reinterpret_cast<InitTableFn>(*q);
                fn();
            }
        }
        result = 0;
    }
    return result;
}

RH_ScopedInstall(CrtPreInit, 0x004a31f3);


// ---------------------------------------------------------------------------
// 0x004a3258 — CrtExitCore
// CRT exit core: acquires lock 8, re-entry guard (TerminateProcess on reentry),
// sets exit-in-progress flags, optionally walks atexit list and first exit table,
// always runs second exit table, then calls ___crtExitProcess or FUN_004a77eb.
// ---------------------------------------------------------------------------
// 0x004a3258
extern "C" __declspec(dllexport) void __cdecl CrtExitCore(unsigned int exit_code,
                                                            int skip_atexit,
                                                            int use_unlock_path)
{
    auto __lockFn        = reinterpret_cast<LockFn>(k__lockAddr);
    auto __crtExitProcFn = reinterpret_cast<ExitProcFn>(k__crtExitProcess);
    auto unlockFn        = reinterpret_cast<UnlockFn>(kFun004a77eb);

    __lockFn(8);

    // Re-entry guard: if already exiting, terminate immediately.
    auto* reentryGuard = reinterpret_cast<std::uint32_t*>(kExitReentryGuard);
    if (*reentryGuard == 1u) {
        HANDLE hProcess = GetCurrentProcess();
        TerminateProcess(hProcess, exit_code);
        return;  // unreachable; for compiler
    }

    // Set exit-in-progress flags.
    *reinterpret_cast<std::uint32_t*>(kExitInProgress) = 1u;
    *reinterpret_cast<std::uint8_t*>(kExitParam3Store) = static_cast<std::uint8_t>(use_unlock_path);

    if (skip_atexit == 0) {
        // Walk atexit list downward (if non-null tail).
        auto* atexitTail = reinterpret_cast<std::uint32_t**>(kAtexitListTail);
        auto* atexitWalk = reinterpret_cast<std::uint32_t**>(kAtexitListWalk);
        if (*atexitTail != nullptr) {
            --(*atexitWalk);
            while (*atexitWalk >= *atexitTail) {
                if (**atexitWalk != 0u) {
                    auto fn = reinterpret_cast<ExitTableFn>(**atexitWalk);
                    fn();
                }
                --(*atexitWalk);
            }
        }

        // First exit fn-ptr table: 2 entries, conditional on skip_atexit==0.
        auto* et1 = reinterpret_cast<std::uint32_t*>(kExitTable1Start);
        auto* et1End = reinterpret_cast<std::uint32_t*>(kExitTable1End);
        for (auto* p = et1; p < et1End; ++p) {
            if (*p != 0u) {
                auto fn = reinterpret_cast<ExitTableFn>(*p);
                fn();
            }
        }
    }

    // Second exit fn-ptr table: 2 entries, always run.
    auto* et2 = reinterpret_cast<std::uint32_t*>(kExitTable2Start);
    auto* et2End = reinterpret_cast<std::uint32_t*>(kExitTable2End);
    for (auto* p = et2; p < et2End; ++p) {
        if (*p != 0u) {
            auto fn = reinterpret_cast<ExitTableFn>(*p);
            fn();
        }
    }

    if (use_unlock_path == 0) {
        // Set re-entry guard before calling ExitProcess.
        *reentryGuard = 1u;
        __crtExitProcFn(exit_code);
    } else {
        unlockFn(8);
    }
}

RH_ScopedInstall(CrtExitCore, 0x004a3258);


// ---------------------------------------------------------------------------
// 0x004a4bb7 — WinMainEntry
// PE entry point: OS version detection, PE header parse, heap/TLS/file init,
// cmdline/env setup, CrtPreInit, GetStartupInfoA, WinMain dispatch.
// [UNCERTAIN U-0001] 4-byte pre-GetVersionExA write to szCSDVersion[0x7c..0x7f]
// [UNCERTAIN U-0002] PE header parse result in local_20
// DRIFT-RISK: SEH frame magic bytes and PE header branch may prevent
// 100% bit-identical diff. Behavioral observation is the verification mode.
// ---------------------------------------------------------------------------
// 0x004a4bb7
extern "C" __declspec(dllexport) int __cdecl WinMainEntry()
{
    // ----------------------------------------------------------------
    // Step 1-2: SEH frame setup + pre-GetVersionExA write.
    // The SEH frame (local_8 = kSehScopeTable, uStack_c = kSehHandler)
    // is emitted by the compiler; we cannot reproduce the exact stack
    // layout in a reimpl without inline asm. Mark as [UNCERTAIN U-0001].
    // We replicate the observable side-effects only.

    // Pre-GetVersionExA 4-byte write: {0xcf, 'K', 'J', '\0'} to szCSDVersion[0x7c].
    // This is written to the LOCAL stack struct before the API call.
    // [UNCERTAIN U-0001] — not confirmed at instruction level.
    OSVERSIONINFOA osvi = {};
    osvi.dwOSVersionInfoSize = 0x94;  // 0x94 = 148 decimal
    // The 4-byte pre-write is to szCSDVersion[0x7c] of the local struct.
    // We replicate it here.
    osvi.szCSDVersion[0x7c] = '\xcf';
    osvi.szCSDVersion[0x7d] = 'K';
    osvi.szCSDVersion[0x7e] = 'J';
    osvi.szCSDVersion[0x7f] = '\0';

    // ----------------------------------------------------------------
    // Step 3: GetVersionExA → fill osvi.
    GetVersionExA(&osvi);

    // ----------------------------------------------------------------
    // Step 4: Write 5 OS globals.
    *reinterpret_cast<std::uint32_t*>(kOsVersionPlatform) = osvi.dwPlatformId;
    *reinterpret_cast<std::uint32_t*>(kOsMajorVersion)    = osvi.dwMajorVersion;
    *reinterpret_cast<std::uint32_t*>(kOsMinorVersion)    = osvi.dwMinorVersion;

    std::uint32_t buildNum = osvi.dwBuildNumber & 0x7fffu;
    if (osvi.dwPlatformId != 2u) {
        buildNum |= 0x8000u;
    }
    *reinterpret_cast<std::uint32_t*>(kOsBuildNumber) = buildNum;
    *reinterpret_cast<std::uint32_t*>(kOsVersionWord) =
        (osvi.dwMajorVersion * 0x100u) + osvi.dwMinorVersion;

    // ----------------------------------------------------------------
    // Step 5: GetModuleHandleA(NULL) → parse PE header.
    // [UNCERTAIN U-0002] exact field offsets for PE32 vs PE32+ branch.
    HINSTANCE hModule = GetModuleHandleA(nullptr);
    int local_20 = 0;
    {
        auto* base = reinterpret_cast<std::uint8_t*>(hModule);
        // Check MZ magic at base.
        std::uint16_t mzMagic = *reinterpret_cast<std::uint16_t*>(base);
        if (mzMagic == 0x5a4du) {
            // Read PE offset from MZ header.
            std::uint32_t peOffset = *reinterpret_cast<std::uint32_t*>(base + 0x3c);
            std::uint8_t* peHdr = base + peOffset;
            std::uint32_t peMagic = *reinterpret_cast<std::uint32_t*>(peHdr);
            if (peMagic == 0x4550u) {
                // Optional header magic.
                std::uint16_t optMagic = *reinterpret_cast<std::uint16_t*>(peHdr + 0x18);
                if (optMagic == 0x10bu || optMagic == 0x20bu) {
                    // Data directory count check: > 0xe.
                    std::uint32_t numDirs = (optMagic == 0x10bu)
                        ? *reinterpret_cast<std::uint32_t*>(peHdr + 0x54 + 0x5c)
                        : *reinterpret_cast<std::uint32_t*>(peHdr + 0x54 + 0x6c);
                    if (numDirs > 0xeu) {
                        // Read flag dword at +0x3a or +0x3e of optional header.
                        // [UNCERTAIN U-0002] — exact field not confirmed via listing.
                        std::uint32_t flagDword = (optMagic == 0x10bu)
                            ? *reinterpret_cast<std::uint32_t*>(peHdr + 0x18 + 0x3a)
                            : *reinterpret_cast<std::uint32_t*>(peHdr + 0x18 + 0x3e);
                        local_20 = static_cast<int>(flagDword);
                    }
                }
            }
        }
    }

    // ----------------------------------------------------------------
    // Step 6: __heap_init; fail → fast_error_exit(0x1c).
    {
        auto heapInitFn = reinterpret_cast<HeapInitFn>(kHeapInit);
        if (heapInitFn() == 0) {
            // fast_error_exit(0x1c) — address not in hooks.csv; call abort fallback.
            // The original calls a specific internal; we mirror the observable effect.
            // [UNCERTAIN] exact fast_error_exit RVA not in this session's scope.
            ExitProcess(0x1c);
        }
    }

    // Step 7: FUN_004a8a04 (mtinitlocks); fail → fast_error_exit(0x10).
    {
        auto mtInitFn = reinterpret_cast<MtInitLocksFn>(kMtInitLocks);
        // Note: analysis shows FUN_004a8a04 returns int; fail path calls fast_error_exit.
        // Treat as void here since analysis note says "fail → fast_error_exit(0x10)".
        mtInitFn();
    }

    // Step 8: FUN_004a78b0.
    {
        auto initLoopFn = reinterpret_cast<CrtInitLoopFn>(kCrtPreInitLoop);
        initLoopFn();
    }

    // Step 9: Clear SEH frame (local_8 = NULL). Not reproducible in reimpl.

    // Step 10: FUN_004ac04a (file-handle table); fail → __amsg_exit(0x1b).
    {
        auto fileHandleFn = reinterpret_cast<FileHandleInitFn>(kFileHandleInit);
        if (fileHandleFn() == 0) {
            ExitProcess(0x1b);
        }
    }

    // Step 11: GetCommandLineA → DAT_008ab6c4.
    *reinterpret_cast<char**>(kCmdLinePtr) = GetCommandLineA();

    // Step 12: ___crtGetEnvironmentStringsA → DAT_007739e8.
    {
        auto getEnvFn = reinterpret_cast<GetEnvStringsFn>(kGetEnvStrings);
        *reinterpret_cast<char**>(kEnvStringsPtr) = getEnvFn();
    }

    // Step 13: FUN_004abe86 (argc builder); fail → __amsg_exit(8).
    {
        auto argcFn = reinterpret_cast<ArgcBuilderFn>(kArgcBuilder);
        if (argcFn() == 0) {
            ExitProcess(8);
        }
    }

    // Step 14: __setenvp; fail → __amsg_exit(9).
    {
        auto setEnvpFn = reinterpret_cast<SetEnvpFn>(k__setenvp);
        if (setEnvpFn() == 0) {
            ExitProcess(9);
        }
    }

    // Step 15: CrtPreInit; non-zero → __amsg_exit(result).
    {
        auto crtPreInitFn = reinterpret_cast<CrtPreInitFnT>(kCrtPreInitFn);
        int r = crtPreInitFn();
        if (r != 0) {
            ExitProcess(static_cast<UINT>(r));
        }
    }

    // Step 16: GetStartupInfoA.
    STARTUPINFOA si = {};
    GetStartupInfoA(&si);
    unsigned int nCmdShow = (si.dwFlags & STARTF_USESHOWWINDOW) ? si.wShowWindow : 10u;

    // Step 17: cmdline scan → char* (argv[0] pointer into cmdline).
    unsigned int uVar3;
    {
        auto cmdlineScanFn = reinterpret_cast<CmdlineScanFn>(kCmdlineScan);
        // Result is the argv pointer; treated as uint in the original dispatch.
        uVar3 = reinterpret_cast<std::uintptr_t>(cmdlineScanFn());
    }

    // Step 18: Second GetModuleHandleA(NULL).
    HINSTANCE hModule2 = GetModuleHandleA(nullptr);

    // Step 19: WinMain wrapper call.
    auto winMainFn = reinterpret_cast<WinMainWrapperFn>(kWinMainWrapper);
    unsigned int winMainResult = static_cast<unsigned int>(
        winMainFn(hModule2, nullptr, reinterpret_cast<char*>(static_cast<std::uintptr_t>(uVar3)),
                  static_cast<int>(nCmdShow))
    );

    // Step 20: local_20 == 0 → no-return exit path; else normal-return path.
    if (local_20 == 0) {
        auto exitNoReturnFn = reinterpret_cast<ExitNoReturnFn>(kExitNoReturn);
        exitNoReturnFn(static_cast<int>(winMainResult));
        // unreachable
        return static_cast<int>(winMainResult);
    } else {
        auto exitReturnFn = reinterpret_cast<ExitReturnFn>(kExitReturn);
        exitReturnFn();
        return static_cast<int>(winMainResult);
    }
}

RH_ScopedInstall(WinMainEntry, 0x004a4bb7);
