// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005be720 : 100.0% (regs 100). By hand: cdecl 2 args, 268-byte local, local[0] is re-read from memory at every use (do not cache it in a variable) - that was the whole difference. Residual: none. */

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

MGLOBAL(unsigned int, DAT_005d0bac, 0x005d0bac);
typedef int (__stdcall *QIFN)(int *, void *, void *);
typedef int (__stdcall *GFN)(int *, int *);
typedef int (__stdcall *RFN)(int *);
// MATCH: 0x005be720 FUN_005be720
int FUN_005be720(int *param_1, int *param_2)
{
  int iVar1;
  int local[67];

  iVar1 = ((QIFN)(*(int **)param_1)[0])(param_1, &DAT_005d0bac, local);
  if (iVar1 >= 0) {
    iVar1 = ((GFN)(*(int **)local[0])[8])((int *)local[0], &local[1]);
    if (iVar1 < 0) {
      ((RFN)(*(int **)local[0])[2])((int *)local[0]);
      return iVar1;
    }
    ((RFN)(*(int **)local[0])[2])((int *)local[0]);
    *param_2 = local[1];
    iVar1 = 0;
  }
  return iVar1;
}
