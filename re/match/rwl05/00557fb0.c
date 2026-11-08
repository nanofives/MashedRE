// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00557fb0: hand-matched (h1). 100%. Result var set after the inner ifs (iVar2 = param_1 at block end). */
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
int FUN_004e68a0(int p0, int p1, int p2);
// MATCH: 0x00557fb0 FUN_00557fb0
int FUN_00557fb0(int param_1,int param_2)
{
  int *piVar1;
  int iVar2 = 0;
  piVar1 = *(int **)(DAT_00913274 + param_1);
  if (piVar1) {
    if (piVar1[5] != param_2) {
      FUN_004e68a0(param_1,piVar1[param_2 + 6],0);
    }
    piVar1[5] = param_2;
    if (*piVar1) {
      if (piVar1[1]) {
        *(int *)(param_1 + 0x48) = piVar1[1];
      }
      piVar1[1] = 0;
      *piVar1 = 0;
    }
    iVar2 = param_1;
  }
  return iVar2;
}
