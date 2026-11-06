// FLAGS: /O2 /GS /Ob1
/* MATCH-S15 h6 0x00492440: 100% (regs 100%). Hand: struct pointer param, compound += on the fields and divide the fields re-read (Ghidra's temporaries gave 63.5%). Residual: none. */
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
typedef struct {
  int pad[8];
  int f20;
  int f24;
  int f28;
  int f2c;
  int f30;
  int f34;
  int f38;
  int f3c;
  int f40;
} Stats492440;
// MATCH: 0x00492440 FUN_00492440
void FUN_00492440(Stats492440 *p)
{
  int a;
  int b;
  a = p->f20;
  b = p->f24;
  p->f2c += a;
  p->f30 += b;
  if (p->f28 >= 0x3c) {
    p->f34 = p->f2c / 0x3c;
    p->f2c = 0;
    p->f38 = p->f30 / 0x3c;
    p->f30 = 0;
    p->f28 = 0;
  }
  p->f28++;
  if (a > p->f3c) {
    p->f3c = a;
  }
  if (b > p->f40) {
    p->f40 = b;
  }
}
