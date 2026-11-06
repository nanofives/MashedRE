// FLAGS: /O2 /GS /Ob1
/* MATCH-S10 0x0045e040 (h3): 100% (regs 100%), byte-identical. Raw converter 76.0%. The release loop is
   if ((int)u > 0) { if (cur != prev) call(cur); } else call(cur); with an unsigned counter (jb) and a signed
   first-entry test (jle); the two calls are tail-merged by the compiler. FUN_005c9220 is the thunk to 0x005abcf0
   (called as the thunk's address). Residual: none. */
/* ---- MATCH-S10 preamble (same text in every re/match/s10 source; not part of any function body) ----
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
MGLOBAL(int, DAT_0068f62c, 0x0068f62c);
MGLOBAL(int, DAT_0068fc84, 0x0068fc84);
MGLOBAL(int, DAT_0088e674, 0x0088e674);
MGLOBAL(int, DAT_0068f640, 0x0068f640);
MGLOBAL(unsigned int, DAT_00690478, 0x00690478);
MGLOBAL(int, DAT_0068f4cc, 0x0068f4cc);
MGLOBAL(int, DAT_0068f4d0, 0x0068f4d0);
MGLOBAL(int, DAT_0068f618, 0x0068f618);
void FUN_0045da60(void);
int FUN_00546320(int p0);
void FUN_005a6710(int p0, int p1);
void FUN_005a79a0(int p0);
void FUN_005c9220(int p0);
// MATCH: 0x0045e040 FUN_0045e040
void FUN_0045e040(void)
{
  int i;
  unsigned int u;

  if (DAT_0068f62c != 0) {
    FUN_0045da60();
    for (i = DAT_0068fc84; i < DAT_0088e674; i++) {
      FUN_005a6710((&DAT_0068f640)[i * 2],0);
    }
    for (u = 0; u < DAT_00690478; u++) {
      if ((int)u > 0) {
        if ((&DAT_0068f4d0)[u] != (&DAT_0068f4cc)[u]) {
          FUN_005c9220((&DAT_0068f4d0)[u]);
        }
      }
      else {
        FUN_005c9220((&DAT_0068f4d0)[u]);
      }
    }
    FUN_005a79a0(DAT_0068f62c);
    DAT_0068f62c = 0;
  }
  if (DAT_0068f618 != 0) {
    FUN_00546320(DAT_0068f618);
    DAT_0068f618 = 0;
  }
}
