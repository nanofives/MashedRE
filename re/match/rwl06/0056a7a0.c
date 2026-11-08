// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x0056a7a0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef unsigned int uint;
// MATCH: 0x0056a7a0 FUN_0056a7a0
void FUN_0056a7a0(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  int iVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  float fVar15;
  float fVar16;
  float fVar17;
  float fVar18;
  float fVar19;
  float *pfVar20;
  uint uVar21;
  int iVar22;
  float *pfVar23;
  int iVar24;
  int iVar25;
  int in_stack_00000020;
  int in_stack_0000002c;
  int in_stack_00000050;
  int in_stack_00000074;
  int in_stack_00000080;
  float in_stack_00000098;
  uint in_stack_0000009c;
  float local_50;
  float fStack_4c;
  float fStack_48;
  float local_40;
  float fStack_3c;
  float fStack_38;
  float local_30;
  float fStack_2c;
  float fStack_28;
  float local_20;
  float fStack_1c;
  float fStack_18;
  
  iVar24 = (int)in_stack_0000009c >> 2;
  uVar21 = in_stack_0000009c & 3;
  iVar25 = *(int *)(in_stack_00000074 + iVar24 * 8);
  iVar1 = *(int *)(in_stack_00000074 + 4 + iVar24 * 8);
  if (iVar25 != -1) {
    iVar24 = *(int *)(in_stack_00000080 + iVar24 * 8) * 0x60 + param_4;
    local_50 = in_stack_00000098 * *(float *)(iVar24 + uVar21 * 4);
    fStack_4c = in_stack_00000098 * *(float *)(iVar24 + 0x10 + uVar21 * 4);
    fStack_48 = in_stack_00000098 * *(float *)(iVar24 + 0x20 + uVar21 * 4);
    local_40 = in_stack_00000098 * *(float *)(iVar24 + 0x30 + uVar21 * 4);
    fStack_3c = in_stack_00000098 * *(float *)(iVar24 + 0x40 + uVar21 * 4);
    fStack_38 = in_stack_00000098 * *(float *)(iVar24 + 0x50 + uVar21 * 4);
  }
  if (iVar1 != -1) {
    iVar24 = *(int *)(in_stack_00000080 + 4 +
                     ((int)(in_stack_0000009c + ((int)in_stack_0000009c >> 0x1f & 3U)) >> 2) * 8) *
             0x60 + param_4;
    local_30 = in_stack_00000098 * *(float *)(iVar24 + uVar21 * 4);
    fStack_2c = in_stack_00000098 * *(float *)(iVar24 + 0x10 + uVar21 * 4);
    fStack_28 = in_stack_00000098 * *(float *)(iVar24 + 0x20 + uVar21 * 4);
    local_20 = in_stack_00000098 * *(float *)(iVar24 + 0x30 + uVar21 * 4);
    fStack_1c = in_stack_00000098 * *(float *)(iVar24 + 0x40 + uVar21 * 4);
    fStack_18 = in_stack_00000098 * *(float *)(iVar24 + 0x50 + uVar21 * 4);
  }
  if ((iVar25 != -1) && (iVar24 = 0, *(int *)(in_stack_00000050 + iVar25 * 4) != 0)) {
    do {
      iVar22 = *(int *)(in_stack_00000020 + iVar25 * 4) + iVar24;
      pfVar20 = (float *)(iVar22 * 0x60 + param_4);
      fVar2 = pfVar20[0x15];
      fVar3 = pfVar20[0x16];
      fVar4 = pfVar20[0x17];
      fVar5 = pfVar20[0x11];
      fVar6 = pfVar20[0x12];
      fVar7 = pfVar20[0x13];
      pfVar23 = (float *)(*(int *)(in_stack_0000002c + iVar22 * 4) * 0x10 + param_1);
      fVar8 = pfVar20[0xd];
      fVar9 = pfVar20[0xe];
      fVar10 = pfVar20[0xf];
      iVar24 = iVar24 + 1;
      fVar11 = pfVar20[9];
      fVar12 = pfVar20[10];
      fVar13 = pfVar20[0xb];
      fVar14 = pfVar20[5];
      fVar15 = pfVar20[6];
      fVar16 = pfVar20[7];
      fVar17 = pfVar20[1];
      fVar18 = pfVar20[2];
      fVar19 = pfVar20[3];
      *pfVar23 = *pfVar23 + *pfVar20 * local_50 + pfVar20[4] * fStack_4c + pfVar20[8] * fStack_48 +
                 pfVar20[0xc] * local_40 + pfVar20[0x10] * fStack_3c + pfVar20[0x14] * fStack_38;
      pfVar23[1] = pfVar23[1] + fVar17 * local_50 + fVar14 * fStack_4c + fVar11 * fStack_48 +
                   fVar8 * local_40 + fVar5 * fStack_3c + fVar2 * fStack_38;
      pfVar23[2] = pfVar23[2] + fVar18 * local_50 + fVar15 * fStack_4c + fVar12 * fStack_48 +
                   fVar9 * local_40 + fVar6 * fStack_3c + fVar3 * fStack_38;
      pfVar23[3] = pfVar23[3] + fVar19 * local_50 + fVar16 * fStack_4c + fVar13 * fStack_48 +
                   fVar10 * local_40 + fVar7 * fStack_3c + fVar4 * fStack_38;
    } while (iVar24 != *(int *)(in_stack_00000050 + iVar25 * 4));
  }
  if ((iVar1 != -1) && (iVar25 = 0, *(int *)(in_stack_00000050 + iVar1 * 4) != 0)) {
    do {
      iVar24 = *(int *)(in_stack_00000020 + iVar1 * 4) + iVar25;
      pfVar20 = (float *)(iVar24 * 0x60 + param_4);
      fVar2 = pfVar20[0x15];
      fVar3 = pfVar20[0x16];
      fVar4 = pfVar20[0x17];
      fVar5 = pfVar20[0x11];
      fVar6 = pfVar20[0x12];
      fVar7 = pfVar20[0x13];
      pfVar23 = (float *)(*(int *)(in_stack_0000002c + iVar24 * 4) * 0x10 + param_1);
      fVar8 = pfVar20[0xd];
      fVar9 = pfVar20[0xe];
      fVar10 = pfVar20[0xf];
      iVar25 = iVar25 + 1;
      fVar11 = pfVar20[9];
      fVar12 = pfVar20[10];
      fVar13 = pfVar20[0xb];
      fVar14 = pfVar20[5];
      fVar15 = pfVar20[6];
      fVar16 = pfVar20[7];
      fVar17 = pfVar20[1];
      fVar18 = pfVar20[2];
      fVar19 = pfVar20[3];
      *pfVar23 = *pfVar23 + *pfVar20 * local_30 + pfVar20[4] * fStack_2c + pfVar20[8] * fStack_28 +
                 pfVar20[0xc] * local_20 + pfVar20[0x10] * fStack_1c + pfVar20[0x14] * fStack_18;
      pfVar23[1] = pfVar23[1] + fVar17 * local_30 + fVar14 * fStack_2c + fVar11 * fStack_28 +
                   fVar8 * local_20 + fVar5 * fStack_1c + fVar2 * fStack_18;
      pfVar23[2] = pfVar23[2] + fVar18 * local_30 + fVar15 * fStack_2c + fVar12 * fStack_28 +
                   fVar9 * local_20 + fVar6 * fStack_1c + fVar3 * fStack_18;
      pfVar23[3] = pfVar23[3] + fVar19 * local_30 + fVar16 * fStack_2c + fVar13 * fStack_28 +
                   fVar10 * local_20 + fVar7 * fStack_1c + fVar4 * fStack_18;
    } while (iVar25 != *(int *)(in_stack_00000050 + iVar1 * 4));
  }
  return;
}
