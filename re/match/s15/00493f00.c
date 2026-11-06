// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00493f00 (h8): 100% (regs 100%). Variadic (fmt, ...): FUN_004a3b84 (the CRT _vsnprintf-shaped
   buf/count/fmt/args callee) gets &param_1 + 4, the va_start value, spelled out since there is no #include.
   char[1024] buffer gives the /GS frame; buf[1023] = 0 then MessageBoxA(0, buf, "MASHED" @0x005cfabc, 0x10).
   Residual: none. */
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
__declspec(dllimport) int __stdcall MessageBoxA(void *hWnd, const char *lpText, const char *lpCaption, unsigned int uType);
int FUN_004a3b84(char *buf, unsigned int count, const char *fmt, char *args);
MGLOBALA(char, s_MASHED_005cfabc, 7, 0x005cfabc);
// MATCH: 0x00493f00 FUN_00493f00
void FUN_00493f00(const char *param_1, ...)
{
  char local_404[1024];

  FUN_004a3b84(local_404, 0x3ff, param_1, (char *)&param_1 + sizeof(param_1));
  local_404[1023] = 0;
  MessageBoxA(0, local_404, s_MASHED_005cfabc, 0x10);
}
