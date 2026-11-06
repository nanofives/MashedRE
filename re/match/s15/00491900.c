// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x00491900 (h5, by hand): 100% (regs 100%), 164 B, byte-identical. Sibling of 0x00491a70. Not in re/console/pc_funcs.csv: scored with the decomp.json size through scratch/h5/m.py (match.py wrapper). */
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
int FUN_004b6fc0(void *L);
double FUN_004b7090(void *L, int idx);
void FUN_00490e50(const char *msg, int code);
void FUN_00490fb0(float a, float b, float c, float d, float e, float f);
// MATCH: 0x00491900 FUN_00491900
int FUN_00491900(void *L)
{
  int n;
  float a, b, c, d, e, f;
  n = FUN_004b6fc0(L);
  a = (float)FUN_004b7090(L, n - 5);
  b = (float)FUN_004b7090(L, n - 4);
  c = (float)FUN_004b7090(L, n - 3);
  d = (float)FUN_004b7090(L, n - 2);
  e = (float)FUN_004b7090(L, n - 1);
  f = (float)FUN_004b7090(L, n);
  if (n != 6) {
    FUN_00490e50("Usage : RainSetPosition(xmin, xmax, ymin, ymax, zmin, zmax)\n", -1);
  }
  FUN_00490fb0(a, b, c, d, e, f);
  return 0;
}
