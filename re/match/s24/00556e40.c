// FLAGS: /O2 /Ob1 /G5
/* MATCH-S24 h3 0x00556e40: 100% (regs 100%). if (--refcount <= 0) {...; return 1;} return 0. The early-return form (> 0 return 0) inverted the branch (80.8%). */
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
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00912a00, 0x00912a00);
typedef void (__cdecl *fn11c_t)(int, int);
void __cdecl FUN_004c5a60(int);
// MATCH: 0x00556e40 FUN_00556e40
int FUN_00556e40(int param_1)
{
  if (--*(int *)(param_1 + 0x74) <= 0) {
    if (*(int *)(param_1 + 0x68) != 0) FUN_004c5a60(*(int *)(param_1 + 0x68));
    (*(fn11c_t *)(DAT_007d3ff8 + 0x11c))(DAT_00912a00, param_1);
    return 1;
  }
  return 0;
}
