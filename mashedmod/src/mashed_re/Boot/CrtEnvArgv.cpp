// Mashed RE - CRT environment/argv init stubs.
// Two functions from the CRT startup sequence. Both are Ghidra FidDB single-
// matches to Visual Studio 2003 Release library functions.
//
// All globals below are in MASHED.exe's .data / .bss sections; addresses
// are cited at the first instruction that references them (per NO-GUESSING).
//
// Internal CRT callees (_malloc, _free, _strlen, FUN_004ac560 / strcpy-wrapper,
// ___initmbctable) are called via RVA function pointers so the original MASHED
// heap and locale state are always used — our reimpl shares the same heap.
#include "../Core/HookSystem.h"
#include <cstdint>
#include <windows.h>

// ─── Shared CRT callee trampolines ───────────────────────────────────────────
// Called via RVA so they use MASHED.exe's heap state, not ours.

// 0x004a45fb  _malloc  (Visual Studio 2003 Release)
static void* mashed_malloc(std::size_t size) {
    typedef void* (__cdecl *MallocFn)(std::size_t);
    return reinterpret_cast<MallocFn>(0x004a45fbu)(size);
}

// 0x004a460d  _free  (Visual Studio 2003 Release)
static void mashed_free(void* ptr) {
    typedef void (__cdecl *FreeFn)(void*);
    reinterpret_cast<FreeFn>(0x004a460du)(ptr);
}

// 0x004a9410  _strlen  (Visual Studio 2003 Release)
static std::size_t mashed_strlen(const char* s) {
    typedef std::size_t (__cdecl *StrlenFn)(const char*);
    return reinterpret_cast<StrlenFn>(0x004a9410u)(s);
}

// 0x004ac560  FUN_004ac560 / strcpy-wrapper  (7-byte stub; tail-call into FUN_004ac570)
// Signature: void __cdecl strcpy_wrapper(char* dst, const char* src)
static void mashed_strcpy(char* dst, const char* src) {
    typedef void (__cdecl *StrcpyFn)(char*, const char*);
    reinterpret_cast<StrcpyFn>(0x004ac560u)(dst, src);
}

// 0x004af2b6  ___initmbctable  (Visual Studio 2003 Release)
// Guards itself on DAT_008ab6d4; idempotent after first call.
static void mashed_initmbctable(void) {
    typedef void (__cdecl *InitMbcFn)(void);
    reinterpret_cast<InitMbcFn>(0x004af2b6u)();
}

// 0x004aaff0  _memcpy  (Visual Studio 2003 Release)
static void* mashed_memcpy(void* dst, const void* src, std::size_t n) {
    typedef void* (__cdecl *MemcpyFn)(void*, const void*, std::size_t);
    return reinterpret_cast<MemcpyFn>(0x004aaff0u)(dst, src, n);
}

// ─── Globals in MASHED.exe ────────────────────────────────────────────────────
// 0x004abc53 body:  DAT_008ab6d4  — initmbctable once-init guard
// 0x004abc53 body:  DAT_007739e8  — env strings pointer (set by ___crtGetEnvironmentStringsA)
// 0x004abc53 body:  DAT_007739bc  — receives malloc'd pointer array
// 0x004abc53 body:  DAT_008ab6c8  — set to 1 on successful completion
// 0x004abf28 body:  DAT_00773d48  — mode flag (0=unknown, 1=wide, 2=ANSI)

static std::uintptr_t* const g_initmbctable_guard =
    reinterpret_cast<std::uintptr_t*>(0x008ab6d4u);
static char** const g_env_strings_ptr =
    reinterpret_cast<char**>(0x007739e8u);
static char*** const g_env_argv_array =
    reinterpret_cast<char***>(0x007739bcu);
static std::uint32_t* const g_env_init_done =
    reinterpret_cast<std::uint32_t*>(0x008ab6c8u);
static std::uint32_t* const g_env_str_mode =
    reinterpret_cast<std::uint32_t*>(0x00773d48u);

// ─── 0x004abc53  __setenvp  ────────────────────────────────────────────────────
// Reads DAT_007739e8 (env string block set by ___crtGetEnvironmentStringsA),
// builds a NULL-terminated array of `char*` pointers skipping '='-prefixed entries,
// stores the array in DAT_007739bc, sets DAT_008ab6c8=1 on success.
// Returns 0 on success, -1 on failure.
//
// Ghidra FidDB: Library Function - Single Match: __setenvp,
//               Library: Visual Studio 2003 Release
extern "C" __declspec(dllexport) int __cdecl CrtSetEnvp(void) {
    // If DAT_008ab6d4 == 0: call ___initmbctable() to init the MBCS table.
    // 0x004abc53: CMP dword ptr [0x008ab6d4], 0x0
    if (*g_initmbctable_guard == 0u) {
        mashed_initmbctable();
    }

    // pcVar4 = DAT_007739e8.
    char* pcVar4 = *g_env_strings_ptr;

    // If pcVar4 == NULL: return -1.
    if (pcVar4 == nullptr) {
        return -1;
    }

    // First pass: count entries not starting with '='.
    int iVar5 = 0;
    {
        char* p = pcVar4;
        while (*p != '\0') {
            const std::size_t sVar2 = mashed_strlen(p);
            if (*p != '=') {
                ++iVar5;
            }
            p += sVar2 + 1u;
        }
    }

    // Allocate pointer array: iVar5 * 4 + 4 bytes.
    // Stores to DAT_007739bc.
    char** puVar1 = static_cast<char**>(mashed_malloc(
        static_cast<std::size_t>(iVar5) * sizeof(char*) + sizeof(char*)));
    *g_env_argv_array = puVar1;

    // Reset pcVar4 for second pass.
    pcVar4 = *g_env_strings_ptr;

    if (puVar1 == nullptr) {
        return -1;
    }

    // Second pass: fill array.
    for (;;) {
        if (*pcVar4 == '\0') {
            // End of env block: free the env strings buffer, NULL-terminate array.
            mashed_free(*g_env_strings_ptr);
            *g_env_strings_ptr = nullptr;
            *puVar1 = nullptr;
            *g_env_init_done = 1u;
            return 0;
        }

        const std::size_t sVar2 = mashed_strlen(pcVar4);

        if (*pcVar4 != '=') {
            void* pvVar3 = mashed_malloc(sVar2 + 1u);
            *puVar1 = static_cast<char*>(pvVar3);
            if (pvVar3 == nullptr) {
                mashed_free(*g_env_argv_array);
                *g_env_argv_array = nullptr;
                return -1;
            }
            mashed_strcpy(static_cast<char*>(pvVar3), pcVar4);
            ++puVar1;
        }

        pcVar4 += sVar2 + 1u;
    }
}

RH_ScopedInstall(CrtSetEnvp, 0x004abc53);

// ─── 0x004abf28  ___crtGetEnvironmentStringsA  ─────────────────────────────────
// Returns a malloc'd ANSI copy of the process environment block.
// Mode flag DAT_00773d48: 0=unknown, 1=wide, 2=ANSI.
// Wide path: GetEnvironmentStringsW + WideCharToMultiByte + FreeEnvironmentStringsW.
// ANSI path: GetEnvironmentStrings + memcpy + FreeEnvironmentStringsA.
//
// Ghidra FidDB: Library Function - Single Match: ___crtGetEnvironmentStringsA,
//               Library: Visual Studio 2003 Release
extern "C" __declspec(dllexport) void* __cdecl CrtGetEnvStrings(void) {
    LPWCH lpWideCharStr = nullptr;

    // Check DAT_00773d48 for cached mode.
    // 0x004abf28: CMP dword ptr [0x00773d48], 0x0
    if (*g_env_str_mode == 0u) {
        lpWideCharStr = ::GetEnvironmentStringsW();
        if (lpWideCharStr != nullptr) {
            *g_env_str_mode = 1u;
            goto LAB_004abf76;
        }
        // lpWideCharStr == NULL: check GetLastError.
        DWORD DVar3 = ::GetLastError();
        if (DVar3 == 0x78u) {  // ERROR_CALL_NOT_IMPLEMENTED (decimal 120)
            *g_env_str_mode = 2u;
        }
        // If neither condition: mode stays 0 (neither 1 nor 2).
    }

    if (*g_env_str_mode != 1u) {
        // ANSI path: mode == 2 or mode == 0 (fall-through).
        if (*g_env_str_mode != 2u && *g_env_str_mode != 0u) {
            // Unknown mode — cannot proceed.
            return nullptr;
        }
        // GetEnvironmentStrings (ANSI).
        char* _Src = static_cast<char*>(::GetEnvironmentStrings());
        if (_Src == nullptr) {
            return nullptr;
        }
        // Walk double-null-terminated ANSI block to find end.
        char* pcVar7 = _Src;
        while (*pcVar7 != '\0') {
            while (*pcVar7 != '\0') { ++pcVar7; }
            ++pcVar7;
        }
        ++pcVar7;  // include trailing '\0'
        // Allocate and copy.
        std::size_t sz = static_cast<std::size_t>(pcVar7 - _Src);
        char* _Dst = static_cast<char*>(mashed_malloc(sz));
        if (_Dst != nullptr) {
            mashed_memcpy(_Dst, _Src, sz);
        }
        ::FreeEnvironmentStringsA(_Src);
        return _Dst;
    }

LAB_004abf76:
    // Wide-char path: mode == 1.
    if (lpWideCharStr == nullptr) {
        // Re-fetch (re-entry with mode already == 1).
        lpWideCharStr = ::GetEnvironmentStringsW();
        if (lpWideCharStr == nullptr) {
            return nullptr;
        }
    }

    // Walk double-null-terminated wide block to find end.
    LPWCH pEnd = lpWideCharStr;
    while (*pEnd != L'\0') {
        while (*pEnd != L'\0') { ++pEnd; }
        ++pEnd;
    }
    ++pEnd;  // include trailing L'\0'

    // Character count: ((end - start) >> 1) + 1 (divides by sizeof(wchar_t)=2, then +1).
    // 0x004abfb9: MOV ECX, EDX; SUB ECX, EAX; SAR ECX, 0x1; INC ECX
    int iVar6 = static_cast<int>(
        (reinterpret_cast<uintptr_t>(pEnd) - reinterpret_cast<uintptr_t>(lpWideCharStr)) >> 1
    ) + 1;

    // Dry run: compute required byte count.
    int _Size = ::WideCharToMultiByte(
        0, 0, lpWideCharStr, iVar6, nullptr, 0, nullptr, nullptr);

    char* lpMultiByteStr = nullptr;
    void* local_8 = nullptr;

    if (_Size != 0) {
        lpMultiByteStr = static_cast<char*>(mashed_malloc(static_cast<std::size_t>(_Size)));
        if (lpMultiByteStr != nullptr) {
            iVar6 = ::WideCharToMultiByte(
                0, 0, lpWideCharStr, iVar6, lpMultiByteStr, _Size, nullptr, nullptr);
            if (iVar6 == 0) {
                mashed_free(lpMultiByteStr);
                local_8 = nullptr;
            } else {
                local_8 = lpMultiByteStr;
            }
        }
    }

    ::FreeEnvironmentStringsW(lpWideCharStr);
    return local_8;
}

RH_ScopedInstall(CrtGetEnvStrings, 0x004abf28);
