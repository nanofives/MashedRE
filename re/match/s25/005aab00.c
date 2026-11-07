// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005aab00: by hand. Best 77.8% (regs 77.8%, ops 100.0%, 45/45 ins). Residual (b): original reloads param_1 from [esp+0x10] in the two arms that set piVar4 = param_1, ours copies esi (mov eax, esi). piVar4 assigned in every branch (instead of Ghidra's up-front init) fixed the block layout; permuter (170 compiles) no better than the Ghidra form. */
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
typedef int (*S25_vfn)(int *);
// MATCH: 0x005aab00 FUN_005aab00
int * FUN_005aab00(int *param_1,int param_2)
{
  int *piVar1;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  piVar5 = param_1;
  if (param_2 == 0) {
    if (*(S25_vfn *)(*param_1 + 0x2c) != (S25_vfn)0) {
      piVar4 = (int *)(**(S25_vfn *)(*param_1 + 0x2c))(param_1);
    }
    else {
      piVar4 = param_1;
    }
    if (piVar4 == (int *)0x0) {
      piVar5 = (int *)0x0;
    }
  }
  else {
    piVar4 = param_1;
  }
  piVar3 = (int *)param_1[3];
  while (piVar3 != param_1 + 3) {
    piVar1 = piVar3 + -5;
    piVar3 = (int *)*piVar3;
    if (*(S25_vfn *)(*piVar1 + 0x2c) != (S25_vfn)0) {
      piVar4 = (int *)(**(S25_vfn *)(*piVar1 + 0x2c))(piVar1);
    }
    if (piVar4 == (int *)0x0) {
      piVar5 = (int *)0x0;
    }
  }
  return piVar5;
}
