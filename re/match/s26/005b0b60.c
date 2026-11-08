// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005b0b60 (37 B). Best 100.0% (regs 100.0%), exact. list walk, s=(p-0x4c) computed before key compare, flags /O2 /Ob1 /G5. */
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
typedef struct S { char pad[0x8]; int key; } S;
// MATCH: 0x005b0b60 FUN_005b0b60
S * FUN_005b0b60(int *param_1,int param_2)
{
  int *p;
  for (p = (int *)*param_1; p != param_1; p = (int *)*p) {
    S *s = (S *)((char *)p - 0x4c);
    int k = *(int *)((char *)p - 0x44);
    if (k == param_2) return s;
  }
  return 0;
}
