// FLAGS: /O2 /GS /Ob1
/* MATCH-S07 0x00430b60 (h9): 100% (regs 100%). Residual: none. Raw g2s 37.5% (char count seeded with a compare, which
   9176 lowers to setne). Hand form: int counter from 0, four `if (g != -1) n++;` (the first increment becomes
   mov eax,1). The four dwords sit 0x10 apart (0x007f1a14..0x007f1a44); kept as separate MGLOBALs. */
/* ---- MATCH-S07 preamble (same text in every re/match/s07 source; not part of any function body) ----
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
MGLOBAL(int, DAT_007f1a14, 0x007f1a14);
MGLOBAL(int, DAT_007f1a24, 0x007f1a24);
MGLOBAL(int, DAT_007f1a34, 0x007f1a34);
MGLOBAL(int, DAT_007f1a44, 0x007f1a44);
// MATCH: 0x00430b60 FUN_00430b60
int FUN_00430b60(void)
{
  int n = 0;
  if (DAT_007f1a14 != -1) n++;
  if (DAT_007f1a24 != -1) n++;
  if (DAT_007f1a34 != -1) n++;
  if (DAT_007f1a44 != -1) n++;
  return n;
}
