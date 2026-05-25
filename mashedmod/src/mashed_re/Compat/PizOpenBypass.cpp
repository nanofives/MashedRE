// Mashed RE - Selective bypass of the piz-file open wrapper.
// MINIMAL VERSION: returns early without logging, no CRT dependencies.
//
// Skips Mashed's piz reader (FUN_004b6940) when called with the powerups
// piz path — that reader is known-broken on this Win11+RTX5070Ti environment
// and crashes inside FUN_004b6940 on the second piz open during boot.
//
// We hook FUN_00495280 (the public wrapper). Its disasm:
//   void FUN_00495280(char* path) {
//       char buf[64];
//       FUN_00402b70(buf, path);                       // path normalize
//       FUN_00496400("Opening piz file %s\n", buf);    // log
//       int ok = FUN_004b6570(buf);                    // ← inner open (crashes for powerups)
//       FUN_004963e0(ok ? "\tOK\n" : "\tFAILED\n");
//   }
//
// Our hook:
//   - For paths containing "powerups" (case-insensitive ASCII): skip
//     entirely, just return.
//   - For everything else: call FUN_00402b70 then FUN_004b6570 then the
//     log printf as the original would. We call by absolute address so we
//     don't need to take the trampoline route.
//
// No CRT — handwritten substring check, no std::*, no headers.
#include "../Core/HookSystem.h"

namespace {

inline char AsciiLower(char c) {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
}

// Case-insensitive substring search. Returns true if `needle` occurs in
// `haystack`. Tiny no-CRT implementation.
bool ContainsLowerI(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;
    for (const char* h = haystack; *h; ++h) {
        const char* hh = h;
        const char* nn = needle;
        while (*hh && *nn && AsciiLower(*hh) == AsciiLower(*nn)) { ++hh; ++nn; }
        if (!*nn) return true;
    }
    return false;
}

using PathNormalizeFn = void (__cdecl*)(char* out, const char* in);
using PizOpenInnerFn  = int  (__cdecl*)(const char* path);
using LogFn           = void (__cdecl*)(const char* fmt, ...);

constexpr unsigned k_PathNormalize = 0x00402b70u;
constexpr unsigned k_PizOpenInner  = 0x004b6570u;
constexpr unsigned k_LogPrintf     = 0x00496400u;
constexpr unsigned k_LogPuts       = 0x004963e0u;

} // namespace

extern "C" __declspec(dllexport) void __cdecl PizOpen_SelectiveBypass(const char* path) {
    auto Normalize = reinterpret_cast<PathNormalizeFn>(k_PathNormalize);
    auto PizOpen   = reinterpret_cast<PizOpenInnerFn>(k_PizOpenInner);
    auto LogF      = reinterpret_cast<LogFn>(k_LogPrintf);
    auto LogP      = reinterpret_cast<LogFn>(k_LogPuts);

    char buf[64];
    Normalize(buf, path);
    LogF("Opening piz file %s\n", buf);

    if (ContainsLowerI(buf, "powerups")) {
        LogP("\t[compat] BYPASSED (known-broken)\n");
        return;
    }

    int ok = PizOpen(buf);
    LogP(ok ? "\tOK\n" : "\tFAILED\n");
}

// MASS-DISABLED 2026-05-24 phase-a2-no-registry-deferred: RH_ScopedInstall(PizOpen_SelectiveBypass, 0x00495280);
