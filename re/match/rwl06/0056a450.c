// FLAGS: /O2 /GS /Ob1
/* MATCH-RWL06 0x0056a450: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined1;
typedef unsigned int undefined4;
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x0056a450 FUN_0056a450
/* [C1 2026-05-19] Signature: `void FUN_0056a450(undefined4 *param_1, int param_2, undefined4
   param_3, int param_4)`. */

void FUN_0056a450(undefined4 *param_1,int param_2,undefined4 param_3,int param_4)

{
  float *pfVar1;
  float *pfVar2;
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
  float fVar20;
  float fVar21;
  float fVar22;
  float fVar23;
  float fVar24;
  float fVar25;
  float fVar26;
  float fVar27;
  float fVar28;
  float fVar29;
  float fVar30;
  float fVar31;
  float fVar32;
  float fVar33;
  float fVar34;
  float fVar35;
  float fVar36;
  float fVar37;
  float fVar38;
  float fVar39;
  undefined4 *puVar40;
  float *pfVar41;
  float *pfVar42;
  int iVar43;
  int in_stack_00000038;
  int *in_stack_00000050;
  int in_stack_0000008c;
  int in_stack_00000098;
  int in_stack_000000a4;
  float *in_stack_000000b0;
  int in_stack_000000bc;
  float *in_stack_000000c8;
  float in_stack_000000d4;
  int local_150;
  int local_14c;
  
  in_stack_000000d4 = _DAT_005cc320 / in_stack_000000d4;
  puVar40 = param_1;
  for (iVar43 = param_2; iVar43 != 0; iVar43 = iVar43 + -1) {
    *puVar40 = 0;
    puVar40[1] = 0;
    puVar40[2] = 0;
    puVar40[3] = 0;
    puVar40 = puVar40 + 4;
  }
  local_14c = 0;
  if (in_stack_0000008c != 0) {
    in_stack_000000bc = in_stack_000000bc + 0x20;
    local_150 = in_stack_0000008c;
    do {
      iVar43 = 0;
      if (*in_stack_00000050 != 0) {
        fVar3 = *in_stack_000000c8;
        fVar4 = in_stack_000000c8[1];
        fVar5 = in_stack_000000c8[2];
        fVar6 = in_stack_000000c8[4];
        fVar7 = in_stack_000000c8[5];
        fVar8 = in_stack_000000c8[6];
        fVar9 = *(float *)(in_stack_000000bc + -0x20);
        fVar10 = *(float *)(in_stack_000000bc + -0x1c);
        fVar11 = *(float *)(in_stack_000000bc + -0x18);
        fVar12 = *(float *)(in_stack_000000bc + -0x14);
        fVar13 = *(float *)(in_stack_000000bc + -0xc);
        fVar14 = *(float *)(in_stack_000000bc + -8);
        fVar15 = *(float *)(in_stack_000000bc + 8);
        pfVar41 = (float *)(local_14c * 0x60 + 0x20 + param_4);
        do {
          fVar16 = pfVar41[-8];
          fVar17 = pfVar41[-7];
          fVar18 = pfVar41[-6];
          fVar19 = pfVar41[-5];
          fVar20 = pfVar41[-4];
          fVar21 = pfVar41[-3];
          fVar22 = pfVar41[-2];
          fVar23 = pfVar41[-1];
          fVar24 = *pfVar41;
          fVar25 = pfVar41[1];
          fVar26 = pfVar41[2];
          fVar27 = pfVar41[3];
          fVar28 = pfVar41[4];
          fVar29 = pfVar41[5];
          fVar30 = pfVar41[6];
          fVar31 = pfVar41[7];
          fVar32 = pfVar41[8];
          fVar33 = pfVar41[9];
          fVar34 = pfVar41[10];
          fVar35 = pfVar41[0xb];
          fVar36 = pfVar41[0xc];
          fVar37 = pfVar41[0xd];
          fVar38 = pfVar41[0xe];
          fVar39 = pfVar41[0xf];
          pfVar42 = (float *)(param_1 + *(int *)(in_stack_00000038 + iVar43 * 4) * 4);
          *pfVar42 = fVar16 * fVar3 + fVar20 * fVar4 + fVar24 * fVar5 + fVar28 * fVar6 +
                     fVar32 * fVar7 + fVar36 * fVar8 + *pfVar42;
          pfVar42[1] = fVar17 * fVar3 + fVar21 * fVar4 + fVar25 * fVar5 + fVar29 * fVar6 +
                       fVar33 * fVar7 + fVar37 * fVar8 + pfVar42[1];
          pfVar42[2] = fVar18 * fVar3 + fVar22 * fVar4 + fVar26 * fVar5 + fVar30 * fVar6 +
                       fVar34 * fVar7 + fVar38 * fVar8 + pfVar42[2];
          pfVar42[3] = fVar19 * fVar3 + fVar23 * fVar4 + fVar27 * fVar5 + fVar31 * fVar6 +
                       fVar35 * fVar7 + fVar39 * fVar8 + pfVar42[3];
          pfVar41[-8] = fVar16 * fVar12;
          pfVar41[-7] = fVar17 * fVar12;
          pfVar41[-6] = fVar18 * fVar12;
          pfVar41[-5] = fVar19 * fVar12;
          pfVar41[-4] = fVar20 * fVar12;
          pfVar41[-3] = fVar21 * fVar12;
          pfVar41[-2] = fVar22 * fVar12;
          pfVar41[-1] = fVar23 * fVar12;
          *pfVar41 = fVar24 * fVar12;
          pfVar41[1] = fVar25 * fVar12;
          pfVar41[2] = fVar26 * fVar12;
          pfVar41[3] = fVar27 * fVar12;
          pfVar41[4] = fVar28 * fVar9 + fVar32 * fVar10 + fVar36 * fVar11;
          pfVar41[5] = fVar29 * fVar9 + fVar33 * fVar10 + fVar37 * fVar11;
          pfVar41[6] = fVar30 * fVar9 + fVar34 * fVar10 + fVar38 * fVar11;
          pfVar41[7] = fVar31 * fVar9 + fVar35 * fVar10 + fVar39 * fVar11;
          pfVar41[8] = fVar28 * fVar10 + fVar32 * fVar13 + fVar36 * fVar14;
          pfVar41[9] = fVar29 * fVar10 + fVar33 * fVar13 + fVar37 * fVar14;
          pfVar41[10] = fVar30 * fVar10 + fVar34 * fVar13 + fVar38 * fVar14;
          pfVar41[0xb] = fVar31 * fVar10 + fVar35 * fVar13 + fVar39 * fVar14;
          local_14c = local_14c + 1;
          pfVar41[0xc] = fVar28 * fVar11 + fVar32 * fVar14 + fVar36 * fVar15;
          pfVar41[0xd] = fVar29 * fVar11 + fVar33 * fVar14 + fVar37 * fVar15;
          pfVar41[0xe] = fVar30 * fVar11 + fVar34 * fVar14 + fVar38 * fVar15;
          pfVar41[0xf] = fVar31 * fVar11 + fVar35 * fVar14 + fVar39 * fVar15;
          pfVar41 = pfVar41 + 0x18;
          iVar43 = iVar43 + 1;
        } while (iVar43 != *in_stack_00000050);
      }
      iVar43 = *in_stack_00000050;
      in_stack_000000c8 = in_stack_000000c8 + 8;
      in_stack_000000bc = in_stack_000000bc + 0x30;
      in_stack_00000050 = in_stack_00000050 + 1;
      in_stack_00000038 = in_stack_00000038 + iVar43 * 4;
      local_150 = local_150 + -1;
    } while (local_150 != 0);
  }
  if (param_2 != 0) {
    in_stack_00000098 = in_stack_00000098 - (int)in_stack_000000b0;
    in_stack_000000a4 = in_stack_000000a4 - (int)in_stack_000000b0;
    iVar43 = (int)param_1 - (int)in_stack_000000b0;
    local_150 = param_2;
    do {
      pfVar41 = (float *)(in_stack_00000098 + (int)in_stack_000000b0);
      fVar3 = pfVar41[1];
      fVar4 = pfVar41[2];
      fVar5 = pfVar41[3];
      fVar6 = in_stack_000000b0[1];
      fVar7 = in_stack_000000b0[2];
      fVar8 = in_stack_000000b0[3];
      pfVar42 = (float *)(in_stack_000000a4 + (int)in_stack_000000b0);
      fVar9 = pfVar42[1];
      fVar10 = pfVar42[2];
      fVar11 = pfVar42[3];
      pfVar1 = (float *)(iVar43 + (int)in_stack_000000b0);
      fVar12 = pfVar1[1];
      fVar13 = pfVar1[2];
      fVar14 = pfVar1[3];
      pfVar2 = (float *)(iVar43 + (int)in_stack_000000b0);
      *pfVar2 = *pfVar1 - (*pfVar42 - *in_stack_000000b0 * *pfVar41 * in_stack_000000d4) *
                          in_stack_000000d4;
      pfVar2[1] = fVar12 - (fVar9 - fVar6 * fVar3 * in_stack_000000d4) * in_stack_000000d4;
      pfVar2[2] = fVar13 - (fVar10 - fVar7 * fVar4 * in_stack_000000d4) * in_stack_000000d4;
      pfVar2[3] = fVar14 - (fVar11 - fVar8 * fVar5 * in_stack_000000d4) * in_stack_000000d4;
      in_stack_000000b0 = in_stack_000000b0 + 4;
      local_150 = local_150 + -1;
    } while (local_150 != 0);
  }
  return;
}
