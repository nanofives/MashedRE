// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005bdef0: 100% (regs 100%). if/else-if chain rewritten as switch (1,2); case-2 cdecl 2-arg fn-ptr, case-1 stdcall. Class: none. */
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
typedef int (__stdcall *vfn1)(int *);
typedef int (__cdecl *cfn2)(int *, int);
// MATCH: 0x005bdef0 FUN_005bdef0
void FUN_005bdef0(int *param_1)
{
  int *puVar1;

  switch (param_1[4]) {
  case 1:
    ((vfn1)*(int *)(*(int *)param_1[3] + 8))((int *)param_1[3]);
    param_1[3] = 0;
    break;
  case 2:
    puVar1 = *(int **)(param_1[3] + 0x10);
    ((cfn2)*puVar1)((int *)param_1[3], puVar1[1]);
    param_1[3] = 0;
    break;
  }
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[4] = 0;
}
