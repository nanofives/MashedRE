// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005bf4d0 : 70.5% (regs 81.8, ops 93). By hand: stdcall 1 arg, QueryInterface out pointer is &param_1 (arg slot overwritten), FUN_005bf470(iface,&loc) cdecl 2 args, loc released at the end. Remaining diff: the original pushes esi in the prologue (before the first vtbl call) and reloads loc into esi; ours pushes esi only before the 0x005bf470 call and keeps loc in eax. Tried a copy variable for loc, hr reuse. Residual class (b), not found. */

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

MGLOBAL(unsigned int, DAT_005d0b1c, 0x005d0b1c);
typedef int (__stdcall *QIFN)(int *, void *, void *);
typedef int (__stdcall *RFN)(int *);
void FUN_005bcd80(int p0);
int FUN_005bf470(int *p0, int **p1);
// MATCH: 0x005bf4d0 FUN_005bf4d0
int __stdcall FUN_005bf4d0(int *param_1)
{
  int iVar1;
  int iVar2;
  int *loc;
  int *q;

  iVar1 = ((QIFN)(*(int **)param_1)[0])(param_1, &DAT_005d0b1c, &param_1);
  if (iVar1 < 0) {
    return iVar1;
  }
  iVar2 = FUN_005bf470(param_1, &loc);
  ((RFN)(*(int **)param_1)[2])(param_1);
  if (iVar2 < 0) {
    return iVar2;
  }
  q = loc;
  if (q[0xcf] != 0) {
    FUN_005bcd80((int)(q + 0xc9));
  }
  ((RFN)(*(int **)q)[2])(q);
  return 0;
}
