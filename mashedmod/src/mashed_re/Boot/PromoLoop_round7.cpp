// Mashed RE — promote-round round 7 (L2 cheap re-earns, demoted-needs-reimpl).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Included in this file (subsystem per hooks.csv noted per function):
//   0x00499730  BootPtr773818Get — boot; 6B constant-pointer getter
//   0x00495120  TimerQpfStore    — util; QPF -> two globals, returns 1
//
// Bodies byte-verified in original\MASHED.exe.unpatched 2026-06-12
// (file offset = RVA - 0x400000); cites in the per-function headers.
//
// NOT included (round-7 triage deferrals — see PROMOTION_LOOP_LEDGER):
//   0x004c9eb0 — analysis-note prose ambiguous on inner-loop variable roles
//                (uVar1/uVar3 assignment ordering); verbatim reimpl needs a
//                Ghidra decomp transcript first (NO-GUESSING)
//   0x00493900 — cmdline tokenizer; live cmdline has no -vs/-cs/-l tokens ->
//                degenerate; needs arbitrary-buffer seeding per vector
//   004926c0/00493480 — QPC time-varying outputs; synthetic bit-diff cannot
//                apply; needs a behavioral/tolerance evidence lane
//
// Analysis:
//   re/analysis/skeleton_prep_boot_winmain_b/00499730.md
//   re/analysis/skeleton_prep_boot_winmain_a/00495120.md

#include "../Core/HookSystem.h"

#include <cstdint>

// ---------------------------------------------------------------------------
// BootPtr773818Get  --  0x00499730   (subsystem: boot)
//
// Original: FUN_00499730 (6 bytes, 0x00499730..0x00499735)
// Bytes: B8 18 38 77 00 / C3   (mov eax,0x00773818; ret)
// Signature: undefined4 FUN_00499730(void)
//
// POINTER-RETURN: returns the constant ORIGINAL-image VA 0x00773818. The
// constant return IS this leaf's entire behavior (no loads, no branches).
//
// Constants (cited from function body at 0x00499730):
//   0x00773818 — returned buffer address (purpose tracked as U-8947)
//
// Caller: FUN_00493900 (boot cmd-line parser, C2).
//
// Uncertainties (non-blocking):
//   U-8947: purpose of DAT_00773818 (data-semantic; renumbered from U-3012).
// ---------------------------------------------------------------------------

// 0x00499730
extern "C" __declspec(dllexport) std::uint32_t __cdecl BootPtr773818Get(void) {
    // 0x00773818 cited at 0x00499730 body (mov eax, imm32).
    return 0x00773818u;
}

RH_ScopedInstall(BootPtr773818Get, 0x00499730);

// ---------------------------------------------------------------------------
// TimerQpfStore  --  0x00495120   (subsystem: util)
//
// Original: FUN_00495120 (42 bytes, 0x00495120..0x00495149)
// Bytes: 83 EC 08 / 8D 44 24 00 / 50 / FF 15 30 C0 5C 00 /
//        8B 4C 24 00 / 8B 54 24 04 / 89 0D 70 1E 77 00 / 89 15 74 1E 77 00 /
//        B8 01 00 00 00 / 83 C4 08 / C3
//   (sub esp,8; lea eax,[esp]; push eax;
//    call [0x005cc030]   <- IAT slot: QueryPerformanceFrequency;
//    store LowPart -> [0x00771e70], HighPart -> [0x00771e74];
//    mov eax,1; add esp,8; ret)
// Signature: undefined4 FUN_00495120(void) — returns 1 unconditionally;
//   QueryPerformanceFrequency's return value is ignored (per the original).
//
// QPF is a machine constant -> deterministic across A/B calls.
//
// Constants (cited from function body at 0x00495120):
//   0x005cc030 — IAT slot for QueryPerformanceFrequency
//   0x00771e70 — global: QPF LowPart store target
//   0x00771e74 — global: QPF HighPart store target
//   1          — unconditional return value
//
// Caller: FUN_00493540 / thunk_LaunchLangGate (C3). Callee is a Win32 import
// (not a stub). No uncertainties.
// ---------------------------------------------------------------------------

// 0x00495120
extern "C" __declspec(dllexport) std::uint32_t __cdecl TimerQpfStore(void) {
    LARGE_INTEGER freq;
    // Original calls QueryPerformanceFrequency via IAT 0x005cc030 and ignores
    // its return (cited at 0x00495120 body).
    QueryPerformanceFrequency(&freq);
    // 0x00771e70 / 0x00771e74 cited at 0x00495120 body.
    *reinterpret_cast<std::uint32_t*>(0x00771e70u) =
        static_cast<std::uint32_t>(freq.LowPart);
    *reinterpret_cast<std::uint32_t*>(0x00771e74u) =
        static_cast<std::uint32_t>(freq.HighPart);
    return 1u;
}

RH_ScopedInstall(TimerQpfStore, 0x00495120);
