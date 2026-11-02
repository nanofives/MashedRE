// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00478110 (h4, by hand): 100%. Zeroed local array c[3] in reverse order, combined if, i++ before the 0x40 stride in the for increment */
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
int FUN_004cc160(int stream, int p1);
int FUN_004cc230(int type, int mode, int name);
int FUN_004cc790(int stream, void *buf, int len);
int FUN_004cc5e0(int stream, int type, unsigned int *len, unsigned int *ver);
int FUN_004cf2b0(int stream, int dst);

// MATCH: 0x00478110 FUN_00478110
unsigned int FUN_00478110(int param_1, int param_2)
{
  int s;
  unsigned int i;
  unsigned int c[3];

  c[2] = 0;
  c[1] = 0;
  c[0] = 0;
  if (param_1 != 0 && (s = FUN_004cc230(2, 1, param_1)) != 0) {
    FUN_004cc790(s, c, 4);
    for (i = 0; i < c[0]; i++, param_2 += 0x40) {
      FUN_004cc5e0(s, 0xd, &c[2], &c[1]);
      FUN_004cf2b0(s, param_2);
    }
    FUN_004cc160(s, 0);
    return c[0];
  }
  return 0;
}
