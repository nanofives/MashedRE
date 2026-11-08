// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 h1 0x005ad730 (55 B). Best 100.0% (regs 100.0%), exact. table {a,key}x5 at 0x6336f8, tail-calls FUN_005ae920(a,param_1). */
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
typedef struct Ent { unsigned int a; int key; } Ent;
MGLOBALA(Ent, DAT_006336f8, 5, 0x006336f8);
int FUN_005a61b0(int p0);
void FUN_005ae920(unsigned int p0, int p1);
// MATCH: 0x005ad730 FUN_005ad730
void FUN_005ad730(int param_1,int param_2)
{
  int k = FUN_005a61b0(param_2);
  unsigned int i = 0;
  do {
    if (DAT_006336f8[i].key == k) {
      FUN_005ae920(DAT_006336f8[i].a, param_1);
      return;
    }
    i++;
  } while (i < 5);
}
