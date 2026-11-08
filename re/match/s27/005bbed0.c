// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bbed0: 80.5% (regs 80.5%). Hand: COM QueryInterface/Release via __stdcall vfn types. Residual (a): original keeps the first 'return 0' (xor eax,eax; pop esi; ret) as its own epilogue and does not tail-merge it with the second; the compiler merges all three. Tried /Ox /Ob2 /Gy /GS /O1 /G6, permuter 97 compiles. */
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef int (__stdcall *vfn1)(void *);
typedef int (__stdcall *vfn2)(void *, void *, void *);
MGLOBAL(undefined1, DAT_005d09bc, 0x005d09bc);
MGLOBAL(undefined1, DAT_005d09cc, 0x005d09cc);
// MATCH: 0x005bbed0 FUN_005bbed0
undefined4 FUN_005bbed0(int *param_1, int param_2, int param_3)
{
  int *piVar1;
  int iVar2;

  if (param_2 != 0) {
    piVar1 = (int *)*param_1;
    iVar2 = (*(vfn2 *)*piVar1)(piVar1, &DAT_005d09cc, param_1 + 1);
    if (iVar2 < 0) {
      return 0;
    }
  }
  if (param_3 != 0) {
    piVar1 = (int *)*param_1;
    iVar2 = (*(vfn2 *)*piVar1)(piVar1, &DAT_005d09bc, param_1 + 2);
    if (iVar2 < 0) {
      piVar1 = (int *)param_1[1];
      if (piVar1 != (int *)0x0) {
        (*(vfn1 *)(*piVar1 + 8))(piVar1);
      }
      return 0;
    }
  }
  return 1;
}
