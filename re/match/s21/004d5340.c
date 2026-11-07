// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004d5340 (h3): 100% (regs 100%). Raw 45.9% (local sized 0x10, not 0x34). A 0x34-byte local struct for the
   slot-0x6c out argument, and the param_6 value as byte 3 << 8 | byte 0 of the dword at +0x20 (shift forms gave 56.8%). */
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
typedef struct Loc5340 { int f[13]; } Loc5340;
typedef int (*Fn6c)(Loc5340 *, int, int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004d5340 FUN_004d5340
int FUN_004d5340(int param_1, int param_2, int *param_3, int *param_4, int *param_5, unsigned int *param_6)
{
  Loc5340 l;
  if ((*(Fn6c *)(DAT_007d3ff8 + 0x6c))(&l, param_1, param_2) == 0) {
    return 0;
  }
  *param_6 = (((unsigned char *)&l.f[8])[3] << 8) | ((unsigned char *)&l.f[8])[0];
  *param_3 = l.f[3];
  *param_4 = l.f[4];
  *param_5 = l.f[5];
  return param_1;
}
