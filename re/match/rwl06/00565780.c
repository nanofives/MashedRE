// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL06 0x00565780: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cc33c, 0x005cc33c);
void FUN_005659f0(int p0, int p1, int p2, float p3, float p4);
// MATCH: 0x00565780 FUN_00565780
/* [C1 2026-05-19] **Jacobi eigenvalue iteration for symmetric 3x3 matrix.** param_1 is the in/out
   symmetric matrix (gets diagonalized in
 */

int FUN_00565780(float *param_1,undefined4 *param_2,float param_3,float param_4)

{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float *pfVar9;
  float *pfVar10;
  int iVar11;
  int iVar12;
  int iVar13;
  float *pfVar14;
  float *pfVar15;
  float local_2c;
  float *local_24;
  float *local_20;
  
  param_2[8] = 0x3f800000;
  param_2[4] = 0x3f800000;
  *param_2 = 0x3f800000;
  param_2[7] = 0;
  param_2[6] = 0;
  param_2[3] = 0;
  param_2[5] = 0;
  param_2[2] = 0;
  param_2[1] = 0;
  fVar1 = param_1[7] * param_1[7] + param_1[6] * param_1[6] + param_1[3] * param_1[3];
  for (iVar11 = (int)param_3; (param_4 * param_4 < fVar1 && (iVar11 != 0)); iVar11 = iVar11 + -1) {
    iVar13 = 1;
    pfVar9 = param_1;
    local_20 = param_1;
    do {
      local_20 = local_20 + 4;
      pfVar15 = pfVar9 + 3;
      iVar12 = 0;
      if (0 < iVar13) {
        pfVar10 = pfVar15;
        pfVar14 = param_1 + 2;
        local_24 = param_1;
        do {
          param_3 = 1.0;
          local_2c = 0.0;
          if (*pfVar10 != DAT_005d757c) {
            fVar1 = (DAT_005cc32c / *pfVar10) * (*local_24 - *local_20);
            fVar2 = SQRT(fVar1 * fVar1 + DAT_005cc320);
            if (fVar1 <= DAT_005d757c) {
              local_2c = DAT_005cc33c / (fVar2 - fVar1);
            }
            else {
              local_2c = DAT_005cc320 / (fVar2 + fVar1);
            }
            param_3 = DAT_005cc320 / SQRT(local_2c * local_2c + DAT_005cc320);
            local_2c = local_2c * param_3;
          }
          FUN_005659f0(param_2,iVar13,iVar12,param_3,local_2c);
          FUN_005659f0(param_1,iVar13,iVar12,param_3,local_2c);
          fVar1 = pfVar14[-2];
          iVar12 = iVar12 + 1;
          fVar2 = pfVar14[-1];
          fVar3 = *pfVar15;
          fVar4 = pfVar14[-2];
          fVar5 = pfVar14[-1];
          fVar6 = pfVar9[4];
          fVar7 = pfVar9[5];
          fVar8 = *pfVar14;
          pfVar9[5] = param_3 * pfVar9[5] - local_2c * *pfVar14;
          *pfVar15 = param_3 * *pfVar15 - local_2c * fVar1;
          pfVar9[4] = param_3 * pfVar9[4] - local_2c * fVar2;
          pfVar14[-2] = param_3 * fVar4 + local_2c * fVar3;
          pfVar14[-1] = local_2c * fVar6 + param_3 * fVar5;
          *pfVar14 = param_3 * fVar8 + local_2c * fVar7;
          pfVar10 = pfVar10 + 1;
          local_24 = local_24 + 4;
          pfVar14 = pfVar14 + 3;
        } while (iVar12 < iVar13);
      }
      iVar13 = iVar13 + 1;
      pfVar9 = pfVar15;
    } while (iVar13 < 3);
    fVar1 = param_1[7] * param_1[7] + param_1[6] * param_1[6] + param_1[3] * param_1[3];
  }
  return iVar11;
}
