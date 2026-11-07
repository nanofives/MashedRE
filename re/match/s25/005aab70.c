// FLAGS: /O2 /Ob1 /G5
/* best 75.6% (regs 75.6%, ops 91.1%): residual = register allocation. Original keeps the fn pointer of the first block in eax
   (forcing a reload of param_2 from [esp+0x10] on the no-fn path); ours keeps param_2 in eax and uses ecx. Result var is param_2 in
   the original. Tried param_2 as result var, fn/result sharing a variable, permuter (116 compiles). Class (b). */
/* MATCH-S25 0x005aab70: hand-matched (h1). */
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

typedef int * (__cdecl *vfn)(int *);
// MATCH: 0x005aab70 FUN_005aab70
int * FUN_005aab70(int *param_1,int param_2)
{
  int *piVar1;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  vfn f;
  piVar4 = (int *)param_2;
  piVar5 = param_1;
  if (param_2 == 0) {
    piVar4 = *(int **)(*param_1 + 0x30);
    if (piVar4 != 0) {
      piVar4 = ((vfn)piVar4)(param_1);
    } else {
      piVar4 = (int *)param_2;
    }
    if (piVar4 == 0) {
      piVar5 = 0;
    }
  }
  piVar3 = (int *)param_1[3];
  while (piVar3 != param_1 + 3) {
    piVar1 = piVar3 + -5;
    piVar3 = (int *)*piVar3;
    f = *(vfn *)(*piVar1 + 0x30);
    if (f != 0) {
      piVar4 = f(piVar1);
    }
    if (piVar4 == 0) {
      piVar5 = 0;
    }
  }
  return piVar5;
}
