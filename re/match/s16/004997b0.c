// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x004997b0 (h6): find/load/size/lock a resource of the module FUN_00499720 returns.
   By hand: 100% exact (regs 100%), relocheck OK. Residual: none. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
#ifndef MATCH_PORT /* port: the Win32 types and imports below come from <windows.h> (Core/HookSystem.h) */
typedef void *HMODULE;
typedef void *HRSRC;
typedef void *HGLOBAL;
typedef unsigned long DWORD;
#endif
HMODULE FUN_00499720(void);
#ifndef MATCH_PORT /* port: the Win32 types and imports below come from <windows.h> (Core/HookSystem.h) */
__declspec(dllimport) HRSRC __stdcall FindResourceA(HMODULE hModule, const char *lpName, const char *lpType);
__declspec(dllimport) HGLOBAL __stdcall LoadResource(HMODULE hModule, HRSRC hResInfo);
__declspec(dllimport) void *__stdcall LockResource(HGLOBAL hResData);
__declspec(dllimport) DWORD __stdcall SizeofResource(HMODULE hModule, HRSRC hResInfo);
#endif
// MATCH: 0x004997b0 FUN_004997b0
int FUN_004997b0(unsigned short param_1, const char *param_2, void **param_3, DWORD *param_4)
{
  HMODULE hModule;
  HRSRC hResInfo;
  HGLOBAL hResData;

  hModule = FUN_00499720();
  hResInfo = FindResourceA(hModule, (const char *)(unsigned int)param_1, param_2);
  if (hResInfo == 0) {
    return 0;
  }
  hResData = LoadResource(hModule, hResInfo);
  if (hResData == 0) {
    return 0;
  }
  if (param_4 != 0) {
    *param_4 = SizeofResource(hModule, hResInfo);
  }
  if (param_3 != 0) {
    *param_3 = LockResource(hResData);
  }
  return 1;
}
