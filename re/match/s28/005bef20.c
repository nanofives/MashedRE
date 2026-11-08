// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bef20 : 100.0% (regs 100). By hand from raw g2s, stdcall callee ret 8, vtable call typed __stdcall. Residual: none. */

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

short * FUN_005c2f0a(short * p0, short * p1);
// MATCH: 0x005bef20 FUN_005bef20
int __stdcall FUN_005bef20(int param_1,int param_2)
{
  int *piVar1;
  FUN_005c2f0a((short *)param_2,(short *)(param_1 + 0x18));
  piVar1 = *(int **)(param_1 + 0x118);
  *(int **)(param_2 + 0x100) = piVar1;
  if (piVar1 != (int *)0)
    (**(int (__stdcall **)(int *))(*piVar1 + 4))(piVar1);
  return 0;
}
