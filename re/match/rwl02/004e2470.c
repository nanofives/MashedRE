// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e2470: hand-written + permuter (store of piVar2[2] moved up). 93.6%, residual (b): param_3/param_4 load order into ecx/edx. */
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
// MATCH: 0x004e2470 FUN_004e2470
void FUN_004e2470(unsigned int *param_1, int param_2, int param_3, int param_4, int param_5)
{
  unsigned int uVar1;
  int *piVar2;
  unsigned int uVar3;

  uVar1 = *param_1;
  if (uVar1 != 0) {
    piVar2 = (int *)param_1[3];
    for (uVar3 = 0; uVar3 < uVar1; uVar3++) {
      if (*piVar2 == param_2) {
        if (param_3 != 1) {
          return;
        }
        piVar2[1] = 1;
        piVar2[8] = param_5;
        return;
      }
      piVar2 = piVar2 + 9;
    }
  }
  piVar2 = (int *)(param_1[3] + uVar1 * 0x24);
  piVar2[2] = param_4;
  *param_1 = uVar1 + 1;
  *piVar2 = param_2;
  piVar2[4] = 0;
  piVar2[5] = (int)param_1;
  piVar2[3] = 0;
  piVar2[6] = 0;
  piVar2[7] = -1;
  piVar2[1] = param_3;
  piVar2[8] = param_5;
}


