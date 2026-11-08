// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x0055f520: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl05_20261107/results.tsv. */
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
typedef unsigned int uint;
uint FUN_005672c0(int p0);
int FUN_00567630(int p0);
int FUN_005687a0(int p0);
int FUN_005687d0(int p0);
int FUN_00568c40(int p0);
// MATCH: 0x0055f520 FUN_0055f520
uint FUN_0055f520(uint *param_1)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  uVar5 = 0;
  if ((*param_1 & 1) != 0) {
    uVar1 = FUN_00568c40(param_1);
    param_1[0x18] = uVar1;
    if (uVar1 != 0) {
      uVar5 = uVar1;
    }
  }
  uVar1 = FUN_005687d0(param_1);
  param_1[0x14] = uVar1;
  uVar2 = FUN_005687a0(param_1);
  iVar3 = param_1[0xf] + 1;
  param_1[0x19] = uVar2;
  uVar4 = (param_1[2] * 2 + 0xf & 0xfffffff0) + (iVar3 * 4 + 0xfU & 0xfffffff0) * 2 + iVar3 * 0x30 +
          (iVar3 * 0xc + 0xfU & 0xfffffff0);
  uVar1 = *param_1;
  param_1[0x17] = uVar4;
  if ((uVar1 & 8) == 0) {
    if ((uVar1 & 2) == 0) {
      if (((uVar1 & 4) != 0) && (uVar5 < uVar4)) {
        uVar5 = uVar4;
      }
    }
    else {
      if ((uVar1 & 4) != 0) {
        uVar2 = uVar2 + uVar4;
      }
      if (uVar5 < uVar2) {
        uVar5 = uVar2;
      }
    }
  }
  else {
    uVar6 = param_1[0x14];
    if ((uVar1 & 2) != 0) {
      uVar6 = uVar6 + uVar2;
    }
    if ((uVar1 & 4) != 0) {
      uVar6 = uVar6 + uVar4;
    }
    if (uVar5 < uVar6) {
      uVar5 = uVar6;
    }
  }
  uVar1 = FUN_00567630(param_1);
  param_1[0x11] = uVar1;
  uVar2 = FUN_005672c0(param_1);
  uVar1 = param_1[8];
  param_1[0x12] = uVar2;
  uVar6 = uVar1 * 0x40;
  uVar4 = (uVar1 * 4 + 0xf & 0xfffffff0) + uVar1 * 0x10;
  uVar1 = *param_1;
  param_1[0x15] = uVar4;
  param_1[0x16] = uVar6;
  if ((uVar1 & 0x10) == 0) {
    if ((uVar1 & 0x20) == 0) {
      if (((uVar1 & 0x40) != 0) && (uVar5 < uVar6)) {
        return uVar6;
      }
    }
    else {
      if ((uVar1 & 0x40) != 0) {
        uVar4 = uVar4 + uVar6;
      }
      if (uVar5 < uVar4) {
        return uVar4;
      }
    }
  }
  else {
    uVar2 = param_1[0x11] + uVar2;
    if ((uVar1 & 0x20) != 0) {
      uVar2 = uVar2 + uVar4;
    }
    if ((uVar1 & 0x40) != 0) {
      uVar2 = uVar2 + uVar6;
    }
    if (uVar5 < uVar2) {
      return uVar2;
    }
  }
  return uVar5;
}
