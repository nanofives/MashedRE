// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005af510: 100% from the raw converter body with one spelling change: (*param_3 & 0x80) != 0 instead of (char)(byte)*param_3 < 0 (jns). Callee prototypes retyped for C++ (uint* flags arg). cxx checked. */
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
typedef unsigned int uint;
typedef unsigned char byte;
typedef long LONG;
void FUN_005af260(undefined4 p0);
void FUN_005af600(int p0, undefined4 p1, uint * p2);
void FUN_005af690(undefined4 p0, int p1, uint * p2);
void FUN_005af740(int p0, int p1, int p2);
void FUN_005af7a0(int p0, uint * p1);
void FUN_005af7d0(int p0, undefined4 p1, uint * p2);
void FUN_005af8f0(int p0, undefined4 p1, uint * p2);
// MATCH: 0x005af510 FUN_005af510
undefined4 FUN_005af510(int param_1,undefined4 param_2,uint *param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  if (((*param_3 & 4) != 0) && (*(int *)(param_1 + 4) == 0)) {
    FUN_005af260(param_1);
    *(undefined4 *)(param_1 + 8) = 2;
    *param_3 = *param_3 & 0xfffffffb;
  }
  iVar1 = *(int *)(param_1 + 8);
  if (iVar1 < 5) {
    FUN_005af8f0(param_1,param_2,param_3);
    iVar1 = *(int *)(param_1 + 8);
    if (iVar1 < 5) {
      return 0;
    }
    uVar2 = 1;
  }
  if (iVar1 != 6) {
    if ((*param_3 & 8) != 0) {
      FUN_005af600(param_1,param_2,param_3);
      *param_3 = *param_3 & 0xfffffff7;
    }
    if ((*param_3 & 0x20) != 0) {
      FUN_005af7d0(param_1,param_2,param_3);
      *param_3 = *param_3 & 0xffffffdf;
    }
    if ((*param_3 & 0x10) != 0) {
      FUN_005af690(param_1,param_2,param_3 + 6);
      *param_3 = *param_3 & 0xffffffef;
    }
    if ((*param_3 & 0x40) != 0) {
      FUN_005af7a0(param_1,param_3);
      *param_3 = *param_3 & 0xffffffbf;
    }
    if ((*param_3 & 0x80) != 0) {
      if (*(uint *)(param_1 + 0x38) != param_3[1]) {
        FUN_005af740(param_1,param_2,param_3[1]);
        *(uint *)(param_1 + 0x38) = param_3[1];
      }
      *param_3 = *param_3 & 0xffffff7f;
    }
  }
  return uVar2;
}
