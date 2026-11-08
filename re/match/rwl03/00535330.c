// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL03 0x00535330: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl03_20261108/results.tsv. */
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
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x00535330 FUN_00535330
undefined4 FUN_00535330(int *param_1,uint param_2)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  bool bVar5;
  bool bVar6;
  bool bVar7;
  bool bVar8;
  bool bVar9;
  bool bVar10;
  bool bVar11;
  bool bVar12;
  bool bVar13;
  
  bVar1 = (byte)param_2;
  bVar5 = (bVar1 & 1) == 1;
  bVar6 = (bVar1 & 2) == 2;
  bVar7 = (bVar1 & 4) == 4;
  bVar8 = (bVar1 & 8) == 8;
  bVar9 = (bVar1 & 0x10) == 0x10;
  bVar10 = (bVar1 & 0x20) == 0x20;
  bVar11 = (bVar1 & 0x40) == 0x40;
  bVar12 = (bVar1 & 0x80) == 0x80;
  bVar13 = (param_2 & 0x100) == 0x100;
  uVar4 = param_1[0x28];
  iVar2 = 0;
  param_1[0x28] = uVar4 & 0xfffffe00;
  param_1[0x26] = 0;
  param_1[0xf] = 0;
  if (bVar5) {
    param_1[0x26] = 1;
    param_1[0x28] = uVar4 & 0xfffffe00 | 1;
    iVar2 = *param_1 * 0xc;
  }
  if (bVar8) {
    param_1[0x26] = param_1[0x26] + 1;
    param_1[0x28] = param_1[0x28] | 8;
    iVar2 = iVar2 + *param_1 * 0x40;
  }
  if (bVar9) {
    param_1[0x26] = param_1[0x26] + 1;
    param_1[0x28] = param_1[0x28] | 0x10;
    iVar2 = iVar2 + *param_1 * 0xc;
  }
  if (bVar7) {
    param_1[0x26] = param_1[0x26] + 1;
    param_1[0x28] = param_1[0x28] | 4;
    iVar2 = iVar2 + *param_1 * 8;
  }
  if (bVar6) {
    param_1[0x26] = param_1[0x26] + 1;
    param_1[0x28] = param_1[0x28] | 2;
    iVar2 = iVar2 + *param_1 * 4;
  }
  if (bVar11) {
    param_1[0x26] = param_1[0x26] + 1;
    param_1[0x28] = param_1[0x28] | 0x40;
    iVar2 = iVar2 + *param_1 * 0x10;
  }
  if (bVar10) {
    param_1[0x26] = param_1[0x26] + 1;
    param_1[0x28] = param_1[0x28] | 0x20;
    iVar2 = iVar2 + *param_1 * 4;
  }
  if (bVar12) {
    param_1[0x26] = param_1[0x26] + 1;
    param_1[0x28] = param_1[0x28] | 0x80;
    iVar2 = iVar2 + *param_1 * 0x10;
  }
  if (bVar13) {
    param_1[0x28] = param_1[0x28] | 0x100;
    param_1[0x26] = param_1[0x26] + 1;
    iVar2 = iVar2 + *param_1 * 0x20;
  }
  iVar2 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar2 + 0x10,0x3012f);
  if (iVar2 != 0) {
    iVar3 = 0;
    uVar4 = iVar2 + 0xfU & 0xfffffff0;
    if (bVar5) {
      param_1[0x13] = uVar4;
      param_1[0x14] = 0xc;
      iVar3 = *param_1 * 0xc;
    }
    if (bVar8) {
      param_1[0x16] = 0x40;
      param_1[0x15] = uVar4 + iVar3;
      iVar3 = iVar3 + *param_1 * 0x40;
    }
    if (bVar9) {
      param_1[0x18] = 0xc;
      param_1[0x17] = uVar4 + iVar3;
      iVar3 = iVar3 + *param_1 * 0xc;
    }
    if (bVar7) {
      param_1[0x1a] = 8;
      param_1[0x19] = uVar4 + iVar3;
      iVar3 = iVar3 + *param_1 * 8;
    }
    if (bVar6) {
      param_1[0x1c] = 4;
      param_1[0x1b] = uVar4 + iVar3;
      iVar3 = iVar3 + *param_1 * 4;
    }
    if (bVar11) {
      param_1[0x1e] = 0x10;
      param_1[0x1d] = uVar4 + iVar3;
      iVar3 = iVar3 + *param_1 * 0x10;
    }
    if (bVar10) {
      param_1[0x20] = 4;
      param_1[0x1f] = uVar4 + iVar3;
      iVar3 = iVar3 + *param_1 * 4;
    }
    if (bVar12) {
      param_1[0x22] = 0x10;
      param_1[0x21] = uVar4 + iVar3;
      iVar3 = iVar3 + *param_1 * 0x10;
    }
    if (bVar13) {
      param_1[0x24] = 0x20;
      param_1[0x23] = uVar4 + iVar3;
    }
    param_1[3] = iVar2;
    param_1[0x27] = 0;
    return 1;
  }
  return 0;
}
