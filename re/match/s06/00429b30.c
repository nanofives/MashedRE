// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00429b30 (h10, by hand): 100% exact. Raw converter 68.4%: callee arity/return types.
   Same idiom as sibling 0x00429b70 (FUN_0042f6a0(void) int, FUN_0042b8c0 short). Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
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
MGLOBAL(int, DAT_008991b0, 0x008991b0);
MGLOBAL(float, DAT_008991b4, 0x008991b4);
MGLOBAL(float, DAT_005cd6c8, 0x005cd6c8);
short FUN_0042b8c0(void);
int FUN_0042f6a0(void);
// MATCH: 0x00429b30 FUN_00429b30
void FUN_00429b30(void)
{
  short s;
  if (FUN_0042f6a0() != 2) {
    if (DAT_008991b0 == 0) {
      s = FUN_0042b8c0();
      DAT_008991b0 = 0x3f;
      DAT_008991b4 = -(DAT_005cd6c8 / (float)s);
    }
  }
}
