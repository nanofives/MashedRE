// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005be630 : 100.0% (regs 100). By hand: cdecl 3 args, the raw 'unaff_retaddr' were args 2 and 3; vtbl[4] call is stdcall (obj, local[4]); test form 'iVar2 >= 0' gives test/jl. Residual: none. */

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

// MATCH: 0x005be630 FUN_005be630
int FUN_005be630(int *param_1, int *param_2, int *param_3)
{
  int *piVar1;
  int iVar2;
  int local_10[4];

  piVar1 = (int *)param_1[0xa4];
  if (piVar1 != (int *)0) {
    iVar2 = (**(int (__stdcall **)(int *, int *))(*piVar1 + 0x10))(piVar1, local_10);
    if (iVar2 >= 0) {
      *param_2 = local_10[1];
      *param_3 = local_10[0];
      return 1;
    }
  }
  return 0;
}

