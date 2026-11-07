// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005ae3a0 (h3): 84.8% exact, regs 84.8%. Residual class (b) original returns the last FUN_005ae4c0 result through a shared mov eax,edi epilogue; return-in-branch form scored lower (56.5%). */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
undefined4 FUN_005ae300(int p0);
uint FUN_005ae4c0(undefined4 *p0, undefined4 p1, int p2);
// MATCH: 0x005ae3a0 FUN_005ae3a0
int FUN_005ae3a0(int param_1,undefined4 param_2)
{
  undefined4 *p;
  undefined4 *q;
  int r = 0;
  p = *(undefined4 **)(param_1 + 0xc);
  while (p != (undefined4 *)(param_1 + 0xc)) {
    r = FUN_005ae4c0(p, param_2, *(int *)(param_1 + 8));
    p = (undefined4 *)*p;
    if (r != 0) return r;
  }
  q = (undefined4 *)FUN_005ae300(param_1);
  if (q != 0) {
    r = FUN_005ae4c0(q, param_2, *(int *)(param_1 + 8));
  }
  return r;
}
