// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00558140: 100% (regs 100%). Result variable initialised to 0 and set to param_1 inside if (p != 0) gives the hoisted xor edx,edx / mov eax,edx. Early-return forms gave 64.7-70.6%. */
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
MGLOBAL(int, DAT_00913274, 0x00913274);
// MATCH: 0x00558140 FUN_00558140
int FUN_00558140(int param_1)
{
  int *p;
  int r = 0;
  p = *(int **)(DAT_00913274 + param_1);
  if (p != 0) {
    r = param_1;
    if (*p != 0) {
      if (p[1] != 0) *(int *)(param_1 + 0x48) = p[1];
      p[1] = 0;
      *p = 0;
    }
  }
  return r;
}
