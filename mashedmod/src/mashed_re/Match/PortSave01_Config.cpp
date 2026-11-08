// Mashed RE — PORT-SAVE-01 (2026-11-08): port bodies of the video-config cluster compiled from the byte-matched
// sources in re/match/s16/ (MATCH-S16: XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, C, match.py 100%,
// relocheck OK).
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
//   0x00496400 ConfigLogDebug     re/match/s16/00496400.c
//   0x00498910 ConfigFilenameGet  re/match/s16/00498910.c (one file holds all three functions, the same text as
//   0x00498950 ConfigLoad                                  re/match/s16/00498950.c and re/match/s16/004989b0.c)
//   0x004989b0 ConfigSave
//
// Replaced bodies: the hand ports in Save/SettingsConfig.cpp (ConfigLogDebugCore / ConfigFilenameGetCore /
// ConfigLoadCore / ConfigSaveCore). 0x004963e0 ConfigLogError is compiled in Save/SettingsConfig.cpp: its definition
// takes (char*) and 00498910.c declares it (const char*, ...), which one C++ TU cannot hold (the original's C TUs
// were separate too).
//
// Binding (Save/SettingsConfig.cpp supplies every name):
//   * defined functions FUN_<rva> -> the export names of the replaced bodies (RH_ScopedInstall unchanged);
//   * callees fputs, FUN_004a42c5 (vsprintf), FUN_004a4541 (fopen), FUN_004a49cf (fread), FUN_004a4b22 (fwrite),
//     FUN_004a4368 (fclose) -> SettingsCfg* functions that call the host functions the replaced bodies called (game
//     CRT thunks in the .asi, the exe's CRT in the exe). No absolute-address call is made from these bodies;
//   * FUN_004963e0 -> ConfigLogError (by name);
//   * globals (the preamble's MGLOBAL / MGLOBALA) -> SettingsCfgGlobal(address): the original address in the .asi, the
//     replaced bodies' private storage and string copies in the exe (its .rdata mapping is zero-filled).
// x87 (x87_tus.txt): matched code is listed x87 like every Match/ wrapper (these bodies hold no float operation).
// Evidence: verify/portsave_20261108/RESULT_PORT-SAVE-01.md.

#include "../Core/HookSystem.h"

#include <cstdio>

extern "C" void* SettingsCfgGlobal(unsigned int a);   // Save/SettingsConfig.cpp

#define MATCH_PREAMBLE
#define MGLOBAL(T, n, a) static T &n = *(T *)SettingsCfgGlobal(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])SettingsCfgGlobal(a)

extern "C" {

// ---- defined functions -> port export names ---------------------------------------------------------------
#define FUN_00496400 ConfigLogDebug
#define FUN_00498910 ConfigFilenameGet
#define FUN_00498950 ConfigLoad
#define FUN_004989b0 ConfigSave
__declspec(dllexport) void __cdecl ConfigLogDebug(const char* fmt, ...);
__declspec(dllexport) char* __cdecl ConfigFilenameGet(void);
__declspec(dllexport) int __cdecl ConfigLoad(void);
__declspec(dllexport) int __cdecl ConfigSave(void);

// ---- callees, by name ---------------------------------------------------------------------------------------
#define FUN_004963e0 ConfigLogError        // Save/SettingsConfig.cpp, re/match/s16/004963e0.c
#define fputs SettingsCfgFputs
#define FUN_004a42c5 SettingsCfgVsprintf
#define FUN_004a4541 SettingsCfgFopen
#define FUN_004a49cf SettingsCfgFread
#define FUN_004a4b22 SettingsCfgFwrite
#define FUN_004a4368 SettingsCfgFclose

// The recorded flags are /Ob1 (only functions marked inline are inlined). VS2022 /O2 would inline FUN_00498910 into
// FUN_00498950 / FUN_004989b0, where the original CALLs it (0x00498951, 0x004989b1).
#pragma auto_inline(off)
#include "../../../../re/match/s16/00496400.c"
// 00498910.c declares FILE as an opaque `struct FILE_` (the CRT's is `struct _iobuf`): renamed for this file only.
// Its FILE* arguments only pass through to the SettingsCfg* functions (C linkage, so the pointee type is not part of
// the symbol).
#define FILE FILE_s16_00498910
#include "../../../../re/match/s16/00498910.c"
#undef FILE
#pragma auto_inline(on)

#undef fputs
}  // extern "C"
