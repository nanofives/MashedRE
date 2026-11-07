// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 h1 0x005be540 : 73.2% (regs 73, ops 76). By hand: cdecl 1 arg, vtbl[6] on param_1+0x15c with out pointer &param_1 (arg slot overwritten), QueryInterface for the 0x005d0b1c IID, vtbl[3], vtbl[7](obj,&v,0,0,4), each interface released on every path, returns the last out value. The original keeps every failure return as its own xor/add/ret epilogue; ours cross-jumps the identical failure tails into one. Flag sets O1, Oy-, GS, Ox, Gy, Oa did not change that. Residual class (b), shape not found. */

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
typedef int (__stdcall *F2)(int *, void *);
typedef int (__stdcall *F3)(int *, void *, void *);
typedef int (__stdcall *F5)(int *, int, void *, int, int);
typedef int (__stdcall *RFN)(int *);
// MATCH: 0x005be540 FUN_005be540
int FUN_005be540(int *param_1)
{
  int iVar1;
  int *obj;
  int loc[3];

  obj = (int *)((char *)param_1 + 0x15c);
  iVar1 = ((F2)(*(int **)obj)[6])(obj, &param_1);
  if (iVar1 < 0) {
    return 0;
  }
  iVar1 = ((F3)(*(int **)param_1)[0])(param_1, &DAT_005d0b1c, &loc[0]);
  if (iVar1 < 0) {
    ((RFN)(*(int **)param_1)[2])(param_1);
    return 0;
  }
  ((RFN)(*(int **)param_1)[2])(param_1);
  iVar1 = ((F2)(*(int **)loc[0])[3])((int *)loc[0], &loc[1]);
  if (iVar1 < 0) {
    ((RFN)(*(int **)loc[0])[2])((int *)loc[0]);
    return 0;
  }
  ((RFN)(*(int **)loc[0])[2])((int *)loc[0]);
  iVar1 = ((F5)(*(int **)loc[1])[7])((int *)loc[1], 0, &loc[2], 0, 4);
  if (iVar1 < 0) {
    ((RFN)(*(int **)loc[1])[2])((int *)loc[1]);
    return 0;
  }
  ((RFN)(*(int **)loc[1])[2])((int *)loc[1]);
  return loc[2];
}
