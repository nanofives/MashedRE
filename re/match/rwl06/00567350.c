// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL06 0x00567350: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
// MATCH: 0x00567350 FUN_00567350
void FUN_00567350(uint param_1,int *param_2,uint *param_3,uint *param_4,int param_5,int param_6,
                 int param_7)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  
  iVar1 = (int)(param_5 + (param_5 >> 0x1f & 3U)) >> 2;
  uVar8 = iVar1 * 8 + 0xfU & 0xfffffff0;
  uVar3 = param_7 * 4 + 0xfU & 0xfffffff0;
  iVar6 = iVar1 * 0x10;
  uVar9 = param_6 * 4 + 0xfU & 0xfffffff0;
  uVar7 = iVar1 * 8 + 0xfU & 0xfffffff0;
  uVar4 = *(int *)(param_1 + 8) + 0xfU & 0xfffffff0;
  *(uint *)(param_1 + 8) = uVar4;
  iVar10 = iVar1 * 0xc0;
  *param_4 = uVar4;
  iVar2 = iVar1 * 0x50 + (param_5 / 3) * 0x80 + uVar8 * 2 + uVar3 + uVar9 * 4 + uVar7 * 2 + iVar10;
  *(uint *)(param_1 + 8) = iVar2 + 0xf + *(int *)(param_1 + 8) & 0xfffffff0;
  param_1 = 0;
  iVar5 = 0;
  if (iVar10 <= iVar2) {
    param_1 = *param_4;
    iVar5 = iVar10;
  }
  *param_3 = param_1;
  if (iVar2 < (int)(iVar5 + uVar7)) {
    uVar4 = 0;
  }
  else {
    uVar4 = *param_4 + iVar5;
    iVar5 = iVar5 + uVar7;
  }
  param_3[7] = uVar4;
  uVar4 = 0;
  if ((int)(uVar7 + iVar5) <= iVar2) {
    uVar4 = *param_4 + iVar5;
    iVar5 = uVar7 + iVar5;
  }
  param_3[10] = uVar4;
  uVar4 = 0;
  if ((int)(iVar5 + uVar9) <= iVar2) {
    uVar4 = *param_4 + iVar5;
    iVar5 = iVar5 + uVar9;
  }
  param_3[0xd] = uVar4;
  uVar4 = 0;
  if ((int)(iVar5 + uVar9) <= iVar2) {
    uVar4 = *param_4 + iVar5;
    iVar5 = iVar5 + uVar9;
  }
  param_3[4] = uVar4;
  uVar4 = 0;
  if ((int)(iVar5 + uVar9) <= iVar2) {
    uVar4 = *param_4 + iVar5;
    iVar5 = iVar5 + uVar9;
  }
  param_3[0x10] = uVar4;
  uVar4 = 0;
  if ((int)(uVar9 + iVar5) <= iVar2) {
    uVar4 = *param_4 + iVar5;
    iVar5 = uVar9 + iVar5;
  }
  param_3[1] = uVar4;
  iVar10 = uVar3 + iVar5;
  uVar3 = 0;
  if (iVar10 <= iVar2) {
    uVar3 = *param_4 + iVar5;
    iVar5 = iVar10;
  }
  param_3[0x13] = uVar3;
  uVar3 = 0;
  if ((int)(iVar5 + uVar8) <= iVar2) {
    uVar3 = *param_4 + iVar5;
    iVar5 = iVar5 + uVar8;
  }
  param_3[0x1c] = uVar3;
  uVar3 = 0;
  if ((int)(iVar5 + uVar8) <= iVar2) {
    uVar3 = *param_4 + iVar5;
    iVar5 = iVar5 + uVar8;
  }
  param_3[0x19] = uVar3;
  uVar8 = 0;
  if (iVar5 + iVar6 <= iVar2) {
    uVar8 = *param_4 + iVar5;
    iVar5 = iVar5 + iVar6;
  }
  param_3[0x22] = uVar8;
  uVar8 = 0;
  if (iVar5 + iVar6 <= iVar2) {
    uVar8 = *param_4 + iVar5;
    iVar5 = iVar5 + iVar6;
  }
  param_3[0x25] = uVar8;
  iVar10 = 0;
  if (iVar5 + iVar6 <= iVar2) {
    iVar10 = *param_4 + iVar5;
    iVar5 = iVar5 + iVar6;
  }
  *param_2 = iVar10;
  iVar10 = 0;
  if (iVar6 + iVar5 <= iVar2) {
    iVar10 = *param_4 + iVar5;
  }
  param_2[3] = iVar10;
  uVar8 = iVar1 * 2 + 0xfU & 0xfffffff0;
  param_3[9] = uVar8;
  param_3[0xc] = uVar8;
  uVar8 = param_6 + 0xfU & 0xfffffff0;
  param_3[0xf] = uVar8;
  param_3[6] = uVar8;
  param_3[0x12] = uVar8;
  param_3[3] = uVar8;
  param_3[0x15] = param_7 + 0xfU & 0xfffffff0;
  uVar8 = iVar1 + 0xfU & 0xfffffff0;
  param_3[0x1e] = uVar8;
  param_3[0x1b] = uVar8;
  uVar8 = param_5 + 0xfU & 0xfffffff0;
  param_3[0x24] = uVar8;
  param_3[0x27] = uVar8;
  param_2[2] = uVar8;
  param_2[5] = uVar8;
  return;
}
