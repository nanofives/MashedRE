// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL04 0x005449f0 h1: 100%; memset intrinsic, n = param_2*0x14 shared. */
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
typedef int (*code)(int,int);
typedef int (*code1)(int);
typedef struct { char pad[0x108]; code fn108; code1 fn10c; } RwGlob;
MGLOBAL(int, DAT_007dc670, 0x007dc670);
MGLOBAL(RwGlob *, DAT_007d3ff8, 0x007d3ff8);
void *memset(void *, int, unsigned int);
#pragma intrinsic(memset)
// MATCH: 0x005449f0 FUN_005449f0
// MATCH: 0x005449f0 FUN_005449f0
int FUN_005449f0(int param_1,int param_2)
{
  int *piVar1;
  int p;
  int n;
  piVar1 = (int *)(DAT_007dc670 + param_1);
  if (piVar1[2] != 0) {
    DAT_007d3ff8->fn10c(piVar1[2]);
    *piVar1 = 0;
    piVar1[2] = 0;
  }
  n = param_2 * 0x14;
  p = DAT_007d3ff8->fn108(n, 0x30105);
  piVar1[2] = p;
  if (p != 0) {
    *piVar1 = param_2;
    memset((void *)p, 0, n);
    return param_1;
  }
  return 0;
}
