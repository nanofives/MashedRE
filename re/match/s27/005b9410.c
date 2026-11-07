// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b9410: 100%. Compound assignments (|= and &=) on the field give the memory-direct or/and and the hoisted mask 8; the plain a = a | b form scored 38.5%. Class: none.
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
typedef unsigned int uint;
typedef unsigned char byte;
// MATCH: 0x005b9410 FUN_005b9410
void FUN_005b9410(int param_1,int param_2)
{
  if ((*(byte *)(*(int *)(param_1 + 0x94) + 0x50) & 8) != 0) {
    if (param_2 != 0) {
      *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) |= 8;
    } else {
      *(uint *)(*(int *)(param_1 + 0x11c) + 0xcc) &= 0xfffffff7;
    }
  } else {
    if (param_2 != 0) {
      *(uint *)(param_1 + 0x28) |= 0x800;
    } else {
      *(uint *)(param_1 + 0x28) &= 0xfffff7ff;
    }
  }
}
