// FLAGS: /O2 /GS /Ob1
/* 0x0040e370 FUN_0040e370 (34 B): returns DAT_005f2770[13 + i] != 0 for i < 4, else 0. Best: 100% exact
   (regs 100, ops 100). Raw Ghidra 71.4%; the fix was an int return (Ghidra bool gives xor al,al / mov al,cl).
   DAT_005f2770 is a pointer read as int[13 + i]. Residual: none. */
/* ---- MATCH-S02 preamble (same text in every re/match/s02 source; not part of any function body) ----
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
MGLOBAL(int *, PTR_PTR_005f2770, 0x005f2770);

// MATCH: 0x0040e370 FUN_0040e370
int FUN_0040e370(int param_1)
{
  if (param_1 >= 4) {
    return 0;
  }
  return PTR_PTR_005f2770[param_1 + 0xd] != 0;
}
