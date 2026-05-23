// Mashed RE — Piz/Wave3PizMisc.cpp
// Wave 3 session 5 — piz open/close wrappers.
//
// Two C2 reimplementations from the boot-time piz archive I/O cluster:
//
//   0x00495280  PizOpen  — path-build + log + FUN_004b6570 open call
//   0x004952f0  PizClose — name-get + log + thunk_FUN_004b67a0 close call
//
// Both functions are called from FUN_00402750 to mount named .piz archives
// during boot (font36.piz, powerups.piz, Panel.piz, perm.piz).
//
// Binary anchor: MASHED.exe SHA-256 (unpatched):
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Analysis refs:
//   re/analysis/promote_c2_piz_loader/00495280.md
//   re/analysis/promote_c2_piz_loader/004952f0.md

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------------------
// Callee forward declarations (all passthrough to originals)
// ---------------------------------------------------------------------------

// FUN_00402b70 — path builder: (char* dst64, const void* src_path) → void
// Writes a processed path string into dst (64-byte buffer).
using FUN_00402b70_t = void (__cdecl*)(char*, const void*);
static FUN_00402b70_t const s_FUN_00402b70 =
    reinterpret_cast<FUN_00402b70_t>(0x00402b70u);

// FUN_004963e0 — single-string log/print: (const char* str) → void
using FUN_004963e0_t = void (__cdecl*)(const char*);
static FUN_004963e0_t const s_FUN_004963e0 =
    reinterpret_cast<FUN_004963e0_t>(0x004963e0u);

// FUN_00496400 — formatted log/print: (const char* fmt, ...) → void
// Declared as a variadic cdecl function.
using FUN_00496400_t = void (__cdecl*)(const char*, ...);
static FUN_00496400_t const s_FUN_00496400 =
    reinterpret_cast<FUN_00496400_t>(0x00496400u);

// FUN_004b6570 — piz file open: (const char* path) → int (0=fail, non-zero=handle/success)
using FUN_004b6570_t = int (__cdecl*)(const char*);
static FUN_004b6570_t const s_FUN_004b6570 =
    reinterpret_cast<FUN_004b6570_t>(0x004b6570u);

// FUN_004b65c0 — piz name getter: (char* dst) → void
// Writes the currently-open piz file name into dst buffer.
using FUN_004b65c0_t = void (__cdecl*)(char*);
static FUN_004b65c0_t const s_FUN_004b65c0 =
    reinterpret_cast<FUN_004b65c0_t>(0x004b65c0u);

// thunk_FUN_004b67a0 — piz close: (void) → void
// Closes the currently-open piz file (operates on implicit global state).
using thunk_FUN_004b67a0_t = void (__cdecl*)();
static thunk_FUN_004b67a0_t const s_thunk_FUN_004b67a0 =
    reinterpret_cast<thunk_FUN_004b67a0_t>(0x004b6590u);

// Stack-cookie global used by both functions.
// DAT_00616038 — stack-cookie XOR value (cited in both analysis notes).
static const std::uint32_t* const g_stack_cookie =
    reinterpret_cast<const std::uint32_t*>(0x00616038u);


// ---------------------------------------------------------------------------
// PizOpen  --  0x00495280
//
// Original: FUN_00495280 (99 bytes, 0x00495280–0x004952e2).
// Decompiled body (verbatim from analysis note 00495280.md):
//   param_1 = caller-supplied path pointer
//   char local_44[64]
//   stack cookie: local_4 = DAT_00616038 ^ unaff_retaddr
//   FUN_00402b70(local_44, param_1)        — build processed path
//   FUN_00496400("Opening piz file %s\n", local_44)
//   iVar1 = FUN_004b6570(local_44)         — open .piz
//   pcVar2 = (iVar1 == 0) ? "\tFAILED\n" : "\tOK\n"
//   FUN_004963e0(pcVar2)                   — log result
//   return void
//
// Constants:
//   local_44[64]       — stack buffer size
//   "Opening piz file %s\n"  — log format string
//   "\tOK\n" / "\tFAILED\n" — result strings
//   DAT_00616038       — stack-cookie XOR value (at 0x00495280 entry)
//
// Stubs: S-3810 (FUN_00402b70), S-3811 (FUN_004963e0), S-3812 (FUN_00496400),
//        S-3813 (FUN_004b6570) — all passthrough to originals.
//
// Callers: FUN_00402750 ×4 (font36.piz, powerups.piz, Panel.piz, perm.piz).
// ---------------------------------------------------------------------------

// 0x00495280
extern "C" __declspec(dllexport) void __cdecl PizOpen(const void* param_1) {
    // Stack-allocate a 64-byte path buffer — matches Ghidra's local_44[64].
    char local_44[64];

    // FUN_00402b70(local_44, param_1) — writes processed path string into local_44
    s_FUN_00402b70(local_44, param_1);

    // FUN_00496400("Opening piz file %s\n", local_44) — formatted log
    s_FUN_00496400("Opening piz file %s\n", local_44);

    // iVar1 = FUN_004b6570(local_44) — open piz file
    const int iVar1 = s_FUN_004b6570(local_44);

    // Select result string: "\tFAILED\n" on zero, "\tOK\n" on non-zero
    const char* pcVar2 = (iVar1 == 0) ? "\tFAILED\n" : "\tOK\n";

    // FUN_004963e0(pcVar2) — log result
    s_FUN_004963e0(pcVar2);
}

RH_ScopedInstall(PizOpen, 0x00495280);


// ---------------------------------------------------------------------------
// PizClose  --  0x004952f0
//
// Original: FUN_004952f0 (83 bytes, 0x004952f0–0x00495342).
// Decompiled body (verbatim from analysis note 004952f0.md):
//   char local_20b0[8364]
//   stack cookie: local_4 = DAT_00616038 ^ unaff_retaddr
//   FUN_004b65c0(local_20b0)              — get current piz name
//   FUN_00496400("Closing piz file %s\n", local_20b0)
//   thunk_FUN_004b67a0()                  — close piz (implicit global state)
//   return void
//
// Constants:
//   local_20b0[8364]      — stack buffer size (0x20AC bytes)
//   "Closing piz file %s\n"  — log format string
//   DAT_00616038          — stack-cookie XOR value
//
// Stubs: S-3826 (FUN_004b65c0), S-3827 (thunk_FUN_004b67a0) — passthrough.
//
// Callers: FUN_00402750 ×4 (paired close after each PizOpen group).
// ---------------------------------------------------------------------------

// 0x004952f0
extern "C" __declspec(dllexport) void __cdecl PizClose() {
    // Stack-allocate an 8364-byte name buffer — matches Ghidra's local_20b0[8364].
    char local_20b0[8364];

    // FUN_004b65c0(local_20b0) — writes name of currently-open piz into buffer
    s_FUN_004b65c0(local_20b0);

    // FUN_00496400("Closing piz file %s\n", local_20b0) — formatted log
    s_FUN_00496400("Closing piz file %s\n", local_20b0);

    // thunk_FUN_004b67a0() — close the piz (no args; implicit global piz handle)
    s_thunk_FUN_004b67a0();
}

RH_ScopedInstall(PizClose, 0x004952f0);
