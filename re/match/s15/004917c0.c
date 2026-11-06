// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 0x004917c0 (h5, by hand): 100% (regs 100%), 155 B, byte-identical. int locals for the __ftol results (unsigned char locals spill bytes). Sibling of 0x00491860. Not in re/console/pc_funcs.csv: scored with the decomp.json size through scratch/h5/m.py (match.py wrapper). */
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
MGLOBAL(int, DAT_00771534, 0x00771534);
int FUN_004b6fc0(void *L);
double FUN_004b7090(void *L, int idx);
void FUN_00490e50(const char *msg, int code);
void FUN_00490f50(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
// MATCH: 0x004917c0 FUN_004917c0
int FUN_004917c0(void *L)
{
  int n;
  int r, g, b, a;
  n = FUN_004b6fc0(L);
  r = (int)FUN_004b7090(L, n - 3);
  g = (int)FUN_004b7090(L, n - 2);
  b = (int)FUN_004b7090(L, n - 1);
  a = (int)FUN_004b7090(L, n);
  if (n != 4) {
    FUN_00490e50("Usage : RainSetHeadColour(red,green,blue,alpha)\n", -1);
  }
  if (DAT_00771534 == 0) {
    FUN_00490e50("Line Rain has not been enabled\n", -1);
  }
  FUN_00490f50(r, g, b, a);
  return 0;
}
