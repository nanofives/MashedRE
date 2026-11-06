// FLAGS: /O2 /GS /Ob1
/* MATCH-S13 0x00482930 (h9, by hand): 100% (regs 100%). Replay constructor: 0x19c-byte record zeroed by a
   0x67-dword loop, frame count (int)((float)(int)(len*60)/res + 2). 60.0 @0x005cc728 and 2.0 @0x005cc574 must be
   literals (a global float reorders the fld/fmul). FUN_00482900 takes (float, int). Residual: none. */
/* ---- MATCH-S13 preamble (same text in every re/match/s13 source; not part of any function body) ----
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
typedef void *(*AllocFn)(int size, int flags);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_00482860(int *p0);
int FUN_00482900(float p0, int p1);
// MATCH: 0x00482930 FUN_00482930
int *FUN_00482930(float len, int res, int p3)
{
  int *r;
  int i;
  int n;

  r = (int *)(*(AllocFn *)(DAT_007d3ff8 + 0x108))(0x19c, 0);
  for (i = 0; i < 0x67; i++) {
    r[i] = 0;
  }
  ((float *)r)[3] = len;
  r[4] = res;
  r[1] = FUN_00482900(len, res);
  r[2] = p3;
  n = (int)((float)(int)(len * 60.0f) / res + 2.0f);
  r[0x5b] = n - 2;
  r[5] = (int)(*(AllocFn *)(DAT_007d3ff8 + 0x108))(n * 0x24, 0);
  r[0] = 0x10b;
  FUN_00482860(r);
  return r;
}
