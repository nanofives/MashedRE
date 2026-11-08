// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00569e90: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
// MATCH: 0x00569e90 FUN_00569e90
void FUN_00569e90(int param_1,undefined4 param_2,undefined4 param_3,int param_4)

{
  float fVar1;
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
  int iVar19;
  int iVar20;
  float *pfVar21;
  int iVar22;
  float *pfVar23;
  uint uVar24;
  int iVar25;
  int in_stack_00000020;
  int in_stack_0000002c;
  int in_stack_00000050;
  int in_stack_00000074;
  int in_stack_00000080;
  float in_stack_00000098;
  uint in_stack_0000009c;
  float local_40;
  float local_3c;
  float local_38;
  float local_30;
  float local_2c;
  float local_28;
  float local_20;
  float local_1c;
  float local_18;
  float local_10;
  float local_c;
  float local_8;
  
  iVar20 = ((int)(in_stack_0000009c + ((int)in_stack_0000009c >> 0x1f & 3U)) >> 2) * 8;
  iVar25 = *(int *)(iVar20 + in_stack_00000074);
  iVar19 = *(int *)(iVar20 + 4 + in_stack_00000074);
  if (iVar25 != -1) {
    iVar22 = *(int *)(iVar20 + in_stack_00000080) * 0x60 + param_4;
    uVar24 = in_stack_0000009c & 0x80000003;
    if ((int)uVar24 < 0) {
      uVar24 = (uVar24 - 1 | 0xfffffffc) + 1;
    }
    local_40 = in_stack_00000098 * *(float *)(iVar22 + uVar24 * 4);
    local_3c = in_stack_00000098 * *(float *)(iVar22 + 0x10 + uVar24 * 4);
    local_38 = in_stack_00000098 * *(float *)(iVar22 + 0x20 + uVar24 * 4);
    local_30 = in_stack_00000098 * *(float *)(iVar22 + 0x30 + uVar24 * 4);
    local_2c = in_stack_00000098 * *(float *)(iVar22 + 0x40 + uVar24 * 4);
    local_28 = in_stack_00000098 * *(float *)(iVar22 + 0x50 + uVar24 * 4);
  }
  if (iVar19 != -1) {
    iVar20 = *(int *)(iVar20 + 4 + in_stack_00000080) * 0x60 + param_4;
    in_stack_0000009c = in_stack_0000009c & 0x80000003;
    if ((int)in_stack_0000009c < 0) {
      in_stack_0000009c = (in_stack_0000009c - 1 | 0xfffffffc) + 1;
    }
    local_20 = in_stack_00000098 * *(float *)(iVar20 + in_stack_0000009c * 4);
    local_1c = in_stack_00000098 * *(float *)(iVar20 + 0x10 + in_stack_0000009c * 4);
    local_18 = in_stack_00000098 * *(float *)(iVar20 + 0x20 + in_stack_0000009c * 4);
    local_10 = in_stack_00000098 * *(float *)(iVar20 + 0x30 + in_stack_0000009c * 4);
    local_c = in_stack_00000098 * *(float *)(iVar20 + 0x40 + in_stack_0000009c * 4);
    local_8 = in_stack_00000098 * *(float *)(iVar20 + 0x50 + in_stack_0000009c * 4);
  }
  if ((iVar25 != -1) && (iVar20 = 0, *(int *)(in_stack_00000050 + iVar25 * 4) != 0)) {
    do {
      iVar22 = *(int *)(in_stack_00000020 + iVar25 * 4) + iVar20;
      pfVar21 = (float *)(iVar22 * 0x60 + param_4);
      fVar1 = pfVar21[1];
      pfVar23 = (float *)(*(int *)(in_stack_0000002c + iVar22 * 4) * 0x10 + param_1);
      fVar2 = pfVar21[5];
      fVar3 = pfVar21[9];
      fVar4 = pfVar21[0xd];
      fVar5 = pfVar21[0x11];
      fVar6 = pfVar21[0x15];
      fVar7 = pfVar21[2];
      fVar8 = pfVar21[6];
      fVar9 = pfVar21[10];
      fVar10 = pfVar21[0xe];
      fVar11 = pfVar21[0x12];
      fVar12 = pfVar21[0x16];
      iVar20 = iVar20 + 1;
      fVar13 = *pfVar21;
      fVar14 = pfVar21[4];
      fVar15 = pfVar21[8];
      fVar16 = pfVar21[0xc];
      fVar17 = pfVar21[0x10];
      fVar18 = pfVar21[0x14];
      pfVar23[3] = local_28 * pfVar21[0x17] +
                   local_2c * pfVar21[0x13] +
                   local_30 * pfVar21[0xf] +
                   local_38 * pfVar21[0xb] +
                   local_3c * pfVar21[7] + local_40 * pfVar21[3] + pfVar23[3];
      *pfVar23 = local_28 * fVar18 +
                 local_2c * fVar17 +
                 local_30 * fVar16 +
                 local_38 * fVar15 + local_3c * fVar14 + local_40 * fVar13 + *pfVar23;
      pfVar23[1] = local_28 * fVar6 +
                   local_2c * fVar5 +
                   local_30 * fVar4 +
                   local_38 * fVar3 + local_3c * fVar2 + local_40 * fVar1 + pfVar23[1];
      pfVar23[2] = local_28 * fVar12 +
                   local_2c * fVar11 +
                   local_30 * fVar10 +
                   local_38 * fVar9 + local_3c * fVar8 + local_40 * fVar7 + pfVar23[2];
    } while (iVar20 != *(int *)(in_stack_00000050 + iVar25 * 4));
  }
  if ((iVar19 != -1) && (iVar25 = 0, *(int *)(in_stack_00000050 + iVar19 * 4) != 0)) {
    do {
      iVar20 = *(int *)(in_stack_00000020 + iVar19 * 4) + iVar25;
      pfVar21 = (float *)(iVar20 * 0x60 + param_4);
      fVar1 = pfVar21[1];
      pfVar23 = (float *)(*(int *)(in_stack_0000002c + iVar20 * 4) * 0x10 + param_1);
      fVar2 = pfVar21[5];
      fVar3 = pfVar21[9];
      fVar4 = pfVar21[0xd];
      fVar5 = pfVar21[0x11];
      fVar6 = pfVar21[0x15];
      fVar7 = pfVar21[2];
      fVar8 = pfVar21[6];
      fVar9 = pfVar21[10];
      fVar10 = pfVar21[0xe];
      fVar11 = pfVar21[0x12];
      fVar12 = pfVar21[0x16];
      iVar25 = iVar25 + 1;
      fVar13 = *pfVar21;
      fVar14 = pfVar21[4];
      fVar15 = pfVar21[8];
      fVar16 = pfVar21[0xc];
      fVar17 = pfVar21[0x10];
      fVar18 = pfVar21[0x14];
      pfVar23[3] = local_8 * pfVar21[0x17] +
                   local_c * pfVar21[0x13] +
                   local_10 * pfVar21[0xf] +
                   local_18 * pfVar21[0xb] +
                   local_1c * pfVar21[7] + local_20 * pfVar21[3] + pfVar23[3];
      *pfVar23 = local_8 * fVar18 +
                 local_c * fVar17 +
                 local_10 * fVar16 +
                 local_18 * fVar15 + local_1c * fVar14 + local_20 * fVar13 + *pfVar23;
      pfVar23[1] = local_8 * fVar6 +
                   local_c * fVar5 +
                   local_10 * fVar4 +
                   local_18 * fVar3 + local_1c * fVar2 + local_20 * fVar1 + pfVar23[1];
      pfVar23[2] = local_8 * fVar12 +
                   local_c * fVar11 +
                   local_10 * fVar10 +
                   local_18 * fVar9 + local_1c * fVar8 + local_20 * fVar7 + pfVar23[2];
    } while (iVar25 != *(int *)(in_stack_00000050 + iVar19 * 4));
  }
  return;
}
