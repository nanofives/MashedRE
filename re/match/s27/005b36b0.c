// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1, PORT-TOP-AUDIO-5AA3D0-STOPS: BYTE-IDENTICAL 29 ins. Rewritten from the listing: the list end is param_1+1 (lea edi,[eax+4] 0x005b36c7; cmp ecx,edi 0x005b36d5) and the walk returns 0 when the next node equals it; the old source (53.1%) tested param_1 == node+1, a different condition (equiv branch-condition at 0x005b36d3). */
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
// MATCH: 0x005b36b0 FUN_005b36b0
int * FUN_005b36b0(int *param_1,int param_2)
{
  int *piVar1;
  int *piVar2;
  int iVar3;

  if ((param_2 <= *param_1) && (*param_1 != 0)) {
    piVar1 = (int *)param_1[1];
    iVar3 = 0;
    piVar2 = piVar1 + -8;
    if (0 < param_2) {
      do {
        piVar1 = (int *)*piVar1;
        if (piVar1 == param_1 + 1) {
          return (int *)0x0;
        }
        iVar3 = iVar3 + 1;
        piVar2 = piVar1 + -8;
      } while (iVar3 < param_2);
    }
    return piVar2;
  }
  return (int *)0x0;
}
