// FLAGS: /O2 /Ob1 /G5
/* MATCH-S23 0x0052de60: 100% (regs 100%). Binds a row-converter object to a source image (no library source):
   copies the format descriptor fields (+4..+0x18) into the object, runs the converter (+0x44, cdecl 5 args) once
   per row, fills the per-row pointer pairs at +0x4c (stride +0x24), end pointer at +8. Raw g2s body (96.2%); the
   permuter moved `iVar1 = param_1[0xb]` and the second `iVar4 = 0` (262 compiles). C++ casts added (conv_fn,
   (undefined4)param_2, (int *)). Residual: none. */
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
typedef unsigned int undefined4;
typedef int (*conv_fn)(int, int, int, int, int);
// MATCH: 0x0052de60 FUN_0052de60
undefined4 FUN_0052de60(undefined4 *param_1,int *param_2)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  
  iVar4 = 0;
  *param_1 = (undefined4)param_2;
  param_1[1] = 0;
  iVar3 = *param_2;
  param_1[9] = *(undefined4 *)(iVar3 + 4);
  param_1[10] = *(undefined4 *)(iVar3 + 8);
  param_1[0xf] = *(undefined4 *)(iVar3 + 0xc);
  param_1[0x10] = *(undefined4 *)(iVar3 + 0x10);
  param_1[0x11] = *(undefined4 *)(iVar3 + 0x14);
  param_1[0x12] = *(undefined4 *)(iVar3 + 0x18);
  iVar1 = param_1[0xb];
  if (iVar4 < iVar1) {
    do {
      ((conv_fn)param_1[0x11])
                (param_1[9] * iVar4 + 0x4c + (int)param_1,param_1[10] * iVar4 + param_2[4],
                 (iVar1 + iVar4) * param_1[10] + param_2[4],0,param_2[5]);
      iVar1 = param_1[0xb];
      iVar4 = iVar4 + 1;
    } while (iVar4 < iVar1);
  }
  iVar3 = param_2[4];
  iVar1 = param_1[10] * param_1[0xb] + iVar3;
  piVar2 = (int *)(param_1 + 0x13);
  iVar4 = 0;
  if (0 < (int)param_1[0xb]) {
    do {
      *piVar2 = iVar3;
      piVar2[1] = iVar1;
      piVar2 = (int *)((int)piVar2 + param_1[9]);
      iVar3 = iVar3 + param_1[10];
      iVar1 = iVar1 + param_1[10];
      iVar4 = iVar4 + 1;
    } while (iVar4 < (int)param_1[0xb]);
  }
  param_1[2] = param_2[4] + param_1[10] * param_1[0xb] * 2;
  return 1;
}
