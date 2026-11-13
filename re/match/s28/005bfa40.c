// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005bfa40: 82.2% (regs 82.2%). bool flag must be int (edi, not bl): 64.4->82.2; cdecl 4-arg fn-ptr call. Residual: orig keeps the two return 0x80004005 epilogues separate, ours tail-merges them; permuter (99 compiles) found nothing. Class: b. */
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
#ifndef __cplusplus
#define true 1
#define false 0
#endif
typedef int (__cdecl *cfn4)(int, int *, int, int *);
typedef int (__stdcall *vfn1)(int *);
int *FUN_005c0e00(int p0, int p1);
// MATCH: 0x005bfa40 FUN_005bfa40
int __stdcall FUN_005bfa40(int param_1, int *param_2, int param_3, int *param_4)
{
  int bVar1;
  int *piVar2;
  int iVar3;

  bVar1 = false;
  piVar2 = param_2;
  if (param_2 == (int *)0) {
    piVar2 = FUN_005c0e00(0, 0);
    bVar1 = true;
    if (piVar2 == (int *)0) {
      return 0x80004005;
    }
  }
  iVar3 = ((cfn4)*(int *)(param_1 + 0x144))(param_1 + 4, piVar2, param_3, (int *)&param_2);
  if (iVar3 == 0) {
    if (bVar1) {
      ((vfn1)*(int *)(*piVar2 + 8))(piVar2);
    }
    return 0x80004005;
  }
  *param_4 = (int)piVar2;
  return 0;
}


