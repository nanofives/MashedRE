// FLAGS: /O2 /GS /Ob1
/* MATCH-S16 0x00495530 (h2): 100% (regs 100%). Hand-written: DirectInput8Create called through its linker thunk as FUN_0049b300 (__stdcall, 5 args), logger FUN_004987b0 cdecl 1 arg. */
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
typedef const char *LPCSTR;
#endif
MGLOBAL(undefined1, DAT_005d0a8c, 0x005d0a8c);
MGLOBAL(undefined4, DAT_00771e78, 0x00771e78);
long __stdcall FUN_0049b300(HMODULE hinst, unsigned long ver, void *riid, void *out, void *outer);
void FUN_004987b0(char *p0);
#ifndef MATCH_PORT /* port: the Win32 types and imports below come from <windows.h> (Core/HookSystem.h) */
__declspec(dllimport) HMODULE __stdcall GetModuleHandleA(LPCSTR p0);
#endif
// MATCH: 0x00495530 FUN_00495530
int FUN_00495530(void)
{
  FUN_004987b0("Creating DInput object\n");
  if (FUN_0049b300(GetModuleHandleA(0), 0x800, &DAT_005d0a8c, &DAT_00771e78, 0) < 0) {
    FUN_004987b0("Failed\n");
    return 0;
  }
  return 1;
}
