// Mashed RE - Input/DInput_j5 cluster (C2->C3 promotion, session c3-batch-j-s5).
//
// Binary anchor: MASHED.exe size 2,846,720
//   SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// Hooks in this file (1):
//   0x004971b0  ControllerConfigLoad_j5   per-slot controller config loader
//                                          (EAX-implicit slot index)
//
// Analysis note (per CLAUDE.md NO-GUESSING rule):
//   re/analysis/promote_c2_dinput_init/004971b0.md  (C2 plate)
//
// Calling-convention notes:
//   0x004971b0 receives the slot index implicitly via register EAX (Ghidra
//   in_EAX). The function calls FUN_00497190 (filename formatter) which
//   reads the same EAX value via FUN_004a2b60's varargs stack-frame trick.
//   We implement as __declspec(naked) so EAX is preserved across the prologue
//   and the call to 0x00497190 sees the same EAX the caller passed.
//
//   The harness extension arg_type='eax_implicit_int' (commit 656273b) builds
//   an RWX trampoline that seeds EAX with each test integer and JMPs to the
//   target. Frida diff thus exercises the real EAX-implicit ABI.
//
// Uncertainties (catalogued, non-blocking for C3):
//   U-2587  _fread size 0x200 vs slot stride 0x80 — buffer block layout
//   U-2588  in_EAX register convention (resolved by caller decomp)

#include "../Core/HookSystem.h"

#include <cstdint>
#include <cstdio>


// ============================================================================
// Game function pointers (cited at exact RVA)
// ============================================================================

// 0x00497190  FUN_00497190  filename formatter -> char*
//   Returns pointer to DAT_007730e4 with "contcfgN.bin" formatted in-place.
//   Reads slot index N from EAX (in_EAX) - we leave EAX intact across the
//   call site, see naked entry below.
//   Cited at: 0x004971bb (CALL).
using FilenameFormatFn_t = char* (__cdecl*)();
static FilenameFormatFn_t const g_FormatContcfgName =
    reinterpret_cast<FilenameFormatFn_t>(0x00497190u);

// 0x00496400  ConfigLogDebug  (variadic log writer) — already C3 in
//                              Save/SettingsConfig.cpp. We call its export
//                              directly to avoid double-hooking.
extern "C" __declspec(dllexport) void __cdecl ConfigLogDebug(const char* fmt, ...);

// 0x004a4541 / 0x004a49cf / 0x004a4368 — the three game-CRT calls, made through their game VAs
// exactly like `call 0x4a4541` (0x004971cd), `call 0x4a49cf` (0x004971ed, _fread) and
// `call 0x4a4368` (0x004971f3, _fclose); all cdecl, popped by `add esp,0x10` (0x004971d4) and
// `add esp,0x1c` (0x00497203). The FILE* belongs to the game's CRT stream table, so it must be
// read and closed by the game's CRT. (Until 2026-10-12 this body called the FsopenSafe export
// and the DLL's own fread/fclose: one CRT end to end, but not the original's. FsopenSafe now
// calls the game __fsopen, so mixing CRTs here would hand a game FILE* to the DLL's fread.)
// Under the boot patch fix_fopen, 0x004a4541 is the JMP into the cave at 0x0050f6c4, which is
// what the original's call reaches too.
using GameFsopenSafeFn = FILE* (__cdecl*)(char* filename, char* mode);
using GameFreadFn      = std::size_t (__cdecl*)(void* dst, std::size_t size, std::size_t count, FILE* fp);
using GameFcloseFn     = int (__cdecl*)(FILE* fp);
static const GameFsopenSafeFn s_GameFsopenSafe = reinterpret_cast<GameFsopenSafeFn>(0x004a4541u);
static const GameFreadFn      s_GameFread      = reinterpret_cast<GameFreadFn>(0x004a49cfu);
static const GameFcloseFn     s_GameFclose     = reinterpret_cast<GameFcloseFn>(0x004a4368u);


// ============================================================================
// Constants (cited at exact RVA in re/analysis/promote_c2_dinput_init/004971b0.md)
// ============================================================================

// 0x005cf010 — "rb\0" mode string (confirmed by memory_read this session).
static constexpr std::uintptr_t kModeRb_Addr = 0x005cf010u;

// 0x007e95c0 — controller config buffer base.
static constexpr std::uintptr_t kCtrlCfgBase = 0x007e95c0u;

// Per-slot byte stride. RAW DISASSEMBLY at 0x004971e7 shows `SHL ESI, 9` =
// slot << 9 = slot * 0x200. The C2 analysis note 004971b0.md cites 0x80;
// that was inferred from the C1 plate's Ghidra decomp which split the
// arithmetic differently. The instruction-level evidence is unambiguous:
// stride is 0x200 (matches read size).  U-2587 is RESOLVED to hypothesis (b)
// adjusted: slot stride 0x200 == _fread size — no overlap, no sub-slot.
static constexpr std::uint32_t kCtrlCfgStride = 0x200u;

// 0x200 — _fread size argument (bytes per call).
static constexpr std::uint32_t kCtrlCfgReadSize = 0x200u;

// 1 — _fread element-size argument.
static constexpr std::uint32_t kFreadElemSize = 1u;


// MATCH-S16: the body of 0x004971b0 (ControllerConfigLoad_j5, with its __stdcall core
// ControllerConfigLoad_Body) moved to Match/MatchS16_004971b0.cpp, which compiles the byte-matched
// source re/match/s16/004971b0.c behind a naked EAX-to-stack entry ControllerConfigLoad_j5.
