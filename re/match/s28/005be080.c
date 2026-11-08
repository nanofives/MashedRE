// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005be080 : 100.0% (regs 100). By hand: cdecl 1 arg, switch on param_1[4] (cases 1,2 give the dec/je/dec/jne chain), QueryInterface out pointer is &param_1 (the arg slot is overwritten with the interface), vtbl[0x13] stdcall(obj,0x30,buf). Residual: none. */

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

MGLOBAL(unsigned int, DAT_005d0b3c, 0x005d0b3c);
typedef int (__stdcall *QIFN)(int *, void *, void *);
typedef int (__stdcall *VFN)(int *, int, void *);
typedef int (__stdcall *RFN)(int *);
// MATCH: 0x005be080 FUN_005be080
int FUN_005be080(int *param_1)
{
  int iVar1;
  int uVar3;
  int buf[12];

  uVar3 = 0;
  switch (param_1[4]) {
  case 1:
    iVar1 = ((QIFN)(*(int **)param_1[3])[0])((int *)param_1[3], &DAT_005d0b3c, &param_1);
    if (iVar1 >= 0) {
      iVar1 = ((VFN)(*(int **)param_1)[0x13])(param_1, 0x30, buf);
      if ((iVar1 >= 0) && ((buf[2] & 0x200) != 0)) {
        uVar3 = 1;
      }
      ((RFN)(*(int **)param_1)[2])(param_1);
    }
    break;
  case 2:
    if ((*(unsigned char *)param_1[3] & 8) != 0) {
      return 1;
    }
    break;
  }
  return uVar3;
}
