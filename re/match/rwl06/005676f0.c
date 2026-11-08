// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x005676f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl06_20261108/results.tsv. */
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
// MATCH: 0x005676f0 FUN_005676f0
void FUN_005676f0(int param_1,int *param_2,int *param_3,uint *param_4,int param_5,int param_6,
                 int param_7,int param_8)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  
  uVar3 = param_7 * 4 + 0xfU & 0xfffffff0;
  uVar6 = param_5 * 4 + 0xfU & 0xfffffff0;
  uVar9 = (param_5 / 3) * 4 + 0xfU & 0xfffffff0;
  uVar10 = param_7 * 8 + 0xfU & 0xfffffff0;
  iVar7 = (int)(param_5 + (param_5 >> 0x1f & 3U)) >> 2;
  iVar8 = iVar7 * 0x10;
  uVar4 = param_8 * 4 + 0xfU & 0xfffffff0;
  iVar7 = param_6 * 0xa0 + uVar4 * 6 + iVar7 * 0x20 + uVar3 * 2 + uVar10 + uVar9 + uVar6 * 0xe +
          param_5 * 0x40 + param_7 * 0x40;
  uVar1 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar1;
  *param_4 = uVar1;
  uVar1 = 0;
  *(uint *)(param_1 + 8) = *(int *)(param_1 + 8) + 0xf + iVar7 & 0xfffffff0;
  uVar2 = 0;
  if ((int)uVar6 <= iVar7) {
    uVar1 = *param_4;
    uVar2 = uVar6;
  }
  param_2[6] = uVar1;
  if (iVar7 < (int)(uVar2 + uVar6)) {
    iVar5 = 0;
  }
  else {
    iVar5 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_2[9] = iVar5;
  uVar1 = param_6 * 0x20 + uVar2;
  if (iVar7 < (int)uVar1) {
    iVar5 = 0;
  }
  else {
    iVar5 = *param_4 + uVar2;
    uVar2 = uVar1;
  }
  param_2[0xc] = iVar5;
  uVar1 = param_7 * 0x40 + uVar2;
  iVar5 = 0;
  if ((int)uVar1 <= iVar7) {
    iVar5 = *param_4 + uVar2;
    uVar2 = uVar1;
  }
  param_2[0x12] = iVar5;
  iVar5 = 0;
  if ((int)(uVar3 + uVar2) <= iVar7) {
    iVar5 = *param_4 + uVar2;
    uVar2 = uVar3 + uVar2;
  }
  param_2[0xf] = iVar5;
  uVar1 = uVar2 + iVar8;
  iVar5 = 0;
  if ((int)uVar1 <= iVar7) {
    iVar5 = *param_4 + uVar2;
    uVar2 = uVar1;
  }
  param_2[3] = iVar5;
  uVar1 = iVar8 + uVar2;
  iVar8 = 0;
  if ((int)uVar1 <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar1;
  }
  *param_2 = iVar8;
  iVar8 = 0;
  if ((int)(uVar4 + uVar2) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar4 + uVar2;
  }
  param_2[0x15] = iVar8;
  iVar8 = 0;
  if ((int)(uVar4 + uVar2) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar4 + uVar2;
  }
  param_2[0x18] = iVar8;
  param_2[0xe] = param_6;
  uVar1 = param_5 + 0xfU & 0xfffffff0;
  param_2[0x14] = param_7;
  param_2[0xb] = uVar1;
  param_2[8] = uVar1;
  param_2[5] = uVar1;
  param_2[2] = uVar1;
  param_2[0x11] = param_7;
  param_2[0x17] = param_8;
  param_2[0x1a] = param_8;
  param_2[10] = 0;
  param_2[7] = 0;
  param_2[0xd] = 0;
  param_2[0x13] = 0;
  param_2[0x10] = 0;
  param_2[4] = 0;
  param_2[1] = 0;
  param_2[0x16] = 0;
  param_2[0x19] = 0;
  uVar1 = param_6 * 0x30 + uVar2;
  iVar8 = 0;
  if ((int)uVar1 <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar1;
  }
  uVar1 = param_6 * 0x20 + uVar2;
  param_3[0x46] = iVar8;
  iVar8 = 0;
  if ((int)uVar1 <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar1;
  }
  param_3[0x43] = iVar8;
  uVar1 = param_5 * 0x40 + uVar2;
  iVar8 = 0;
  if ((int)uVar1 <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar1;
  }
  *param_3 = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[4] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[7] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[10] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0xd] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0x10] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0x13] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0x16] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0x22] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0x19] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0x1c] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar6) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar6;
  }
  param_3[0x1f] = iVar8;
  iVar8 = 0;
  if ((int)(uVar6 + uVar2) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar6 + uVar2;
  }
  param_3[0x25] = iVar8;
  uVar9 = uVar9 + uVar2;
  iVar8 = 0;
  if ((int)uVar9 <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar9;
  }
  param_3[0x28] = iVar8;
  uVar10 = uVar10 + uVar2;
  iVar8 = 0;
  if ((int)uVar10 <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar10;
  }
  param_3[0x2b] = iVar8;
  iVar8 = 0;
  if ((int)(uVar3 + uVar2) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar3 + uVar2;
  }
  param_3[0x2e] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar4) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar4;
  }
  param_3[0x31] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar4) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar4;
  }
  param_3[0x34] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar4) <= iVar7) {
    iVar8 = *param_4 + uVar2;
    uVar2 = uVar2 + uVar4;
  }
  param_3[0x37] = iVar8;
  iVar8 = 0;
  if ((int)(uVar2 + uVar4) <= iVar7) {
    iVar8 = *param_4 + uVar2;
  }
  param_3[0x48] = param_6;
  param_3[0x45] = param_6;
  param_3[0x3a] = iVar8;
  param_3[3] = param_5;
  param_3[6] = param_5;
  param_3[9] = param_5;
  param_3[0xc] = param_5;
  param_3[0xf] = param_5;
  param_3[0x12] = param_5;
  param_3[0x15] = param_5;
  param_3[0x18] = param_5;
  param_3[0x24] = param_5;
  param_3[0x1b] = param_5;
  param_3[0x1e] = param_5;
  param_3[0x21] = param_5;
  param_3[0x27] = param_5;
  param_3[0x2d] = param_7;
  param_3[0x30] = param_7;
  param_3[0x2a] = param_5 / 3;
  param_3[0x33] = param_8;
  param_3[0x36] = param_8;
  param_3[0x39] = param_8;
  param_3[0x3c] = param_8;
  return;
}
