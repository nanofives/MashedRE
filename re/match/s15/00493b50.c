// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493b50 (h8): 100% (regs 100%). /GS frame from the OSVERSIONINFOA char array; returns the int of
   the tail call through PTR_FUN_006147dc (eax survives the cookie check jmp). Locally declared OSVERSIONINFOA
   layout (no #include). Picks FUN_00493b40 (re/match/s15/00493b40.c) or FUN_00493ac0. Residual: none. */
/* ---- MATCH-S15 preamble (same text in every re/match/s15 source; not part of any function body) ----
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
typedef struct _OSVERSIONINFOA_ {
  unsigned long dwOSVersionInfoSize;
  unsigned long dwMajorVersion;
  unsigned long dwMinorVersion;
  unsigned long dwBuildNumber;
  unsigned long dwPlatformId;
  char szCSDVersion[128];
} OSVERSIONINFOA_;
typedef int (*code)(void);
__declspec(dllimport) int __stdcall GetVersionExA(OSVERSIONINFOA_ *lpVersionInformation);
__declspec(dllimport) long __stdcall InterlockedExchange(long *Target, long Value);
MGLOBAL(code, PTR_FUN_006147dc, 0x006147dc);
int FUN_00493b40(void);
int FUN_00493ac0(void);
// MATCH: 0x00493b50 FUN_00493b50
int FUN_00493b50(void)
{
  code Value;
  OSVERSIONINFOA_ local_98;

  local_98.dwOSVersionInfoSize = 0x94;
  GetVersionExA(&local_98);
  if ((local_98.dwPlatformId != 2) || (Value = FUN_00493b40, local_98.dwMajorVersion < 5)) {
    Value = FUN_00493ac0;
  }
  InterlockedExchange((long *)&PTR_FUN_006147dc, (long)Value);
  return PTR_FUN_006147dc();
}
