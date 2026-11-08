// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 0x005aaa00: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char undefined;
typedef unsigned int undefined4;
typedef unsigned char byte;
typedef int code(int *, undefined4, undefined4);
void FUN_005a9f60(int * p0, undefined4 p1, undefined4 p2);
void FUN_005aacf0(int * p0, int p1);
// MATCH: 0x005aaa00 FUN_005aaa00
void FUN_005aaa00(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int *piVar1;
  int *piVar2;
  
  if (*(code **)(*param_1 + 0x34) != (code *)0x0) {
    (**(code **)(*param_1 + 0x34))(param_1,param_2,param_3);
  }
  if (param_1[5] != 0) {
    *(int *)param_1[6] = param_1[5];
    *(int *)(param_1[5] + 4) = param_1[6];
    param_1[6] = 0;
    param_1[5] = 0;
  }
  piVar2 = (int *)param_1[3];
  while (piVar2 != param_1 + 3) {
    piVar1 = (int *)*piVar2;
    *(int **)piVar2[1] = piVar1;
    *(int *)(*piVar2 + 4) = piVar2[1];
    piVar2 = piVar1;
  }
  if ((*(byte *)(param_1 + 7) & 1) != 0) {
    FUN_005aacf0(param_1,1);
    FUN_005aacf0(param_1,2);
  }
  *(int *)param_1[2] = param_1[1];
  *(int *)(param_1[1] + 4) = param_1[2];
  FUN_005a9f60(param_1,param_2,param_3);
  return;
}
