// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00558d30: hand-matched (h1). 100%. Search loop as while+break (goto removed), callback loop reads *p++ into a local before the call. */
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
typedef int (__cdecl *fn1)(int *);
typedef void (__cdecl *fn1v)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
int FUN_0055deb0(int p0);
int FUN_0055dec0(int p0);
void FUN_005c4d20(int p0, int p1);
int FUN_005c4d50(int p0);
int FUN_005c4df0(int p0, int p1);
int FUN_005c4e10(int p0);
// MATCH: 0x00558d30 FUN_00558d30
int FUN_00558d30(int *param_1)
{
  int iVar1;
  int *piVar2;
  int *piVar3;
  int *puVar5;
  int *puVar6;

  if (param_1 == *(int **)(*param_1 + 0x14)) {
    *(int *)(*param_1 + 0x14) = 0;
  }
  iVar1 = *param_1;
  piVar2 = (int *)FUN_0055dec0(*(int *)(iVar1 + 0x10));
  piVar3 = (int *)FUN_005c4e10(*(int *)(iVar1 + 0x10));
  while (piVar2 != piVar3) {
    if (param_1 == (int *)*piVar2) {
      piVar3 = (int *)FUN_005c4df0(*(int *)(iVar1 + 0x10), FUN_0055deb0(*(int *)(iVar1 + 0x10)) - 1);
      *piVar2 = *piVar3;
      FUN_005c4d20(*(int *)(iVar1 + 0x10),1);
      break;
    }
    piVar2 = piVar2 + 1;
  }
  puVar5 = (int *)FUN_005c4e10(param_1[1]);
  puVar6 = (int *)FUN_0055dec0(param_1[1]);
  while (puVar6 != puVar5) {
    int t = *puVar6++;
    (*(fn1v *)(*param_1 + 0x1c))(t);
  }
  FUN_005c4d50(param_1[1]);
  (*(fn1v *)(DAT_007d3ff8 + 0x10c))((int)param_1);
  return 1;
}
