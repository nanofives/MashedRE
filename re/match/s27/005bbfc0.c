// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bbfc0: 94.1% (regs 94.1%). Hand: COM calls via __stdcall vfn types, param_1 is address-taken (&param_1 is the out arg, original reuses the param slot). Residual (a): original schedules lea edx,[esp] before mov [esp],0; pp=&out temp, store order, permuter 112 compiles did not move it. */
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
typedef int (__stdcall *vfn2)(void *, void *, void *);
typedef int (__stdcall *vfn4)(void *, void *, int, void *);
typedef int (__stdcall *vfn1)(void *);
MGLOBAL(undefined1, DAT_005d09bc, 0x005d09bc);
MGLOBAL(undefined1, DAT_005e7100, 0x005e7100);
// MATCH: 0x005bbfc0 FUN_005bbfc0
undefined4 FUN_005bbfc0(int ***param_1)
{
  int *out;
  int **piVar2;
  int iVar1;

  out = 0;
  piVar2 = *param_1;
  iVar1 = (*(vfn2 *)*piVar2)(piVar2, &DAT_005d09bc, &out);
  if (iVar1 >= 0) {
    iVar1 = (*(vfn4 *)(*out + 0x14))(out, &DAT_005e7100, 0, &param_1);
    if ((iVar1 >= 0) && ((int)param_1 == 3)) {
      return 1;
    }
    (*(vfn1 *)(*out + 8))(out);
  }
  return 0;
}
