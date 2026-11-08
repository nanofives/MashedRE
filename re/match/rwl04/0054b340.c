// FLAGS: /O2 /Ob1 /G5
/* h2 note: 20.8% raw form. cxx FAIL: code() call arity, float* vs undefined4*. class (b). */
/* MATCH-RWL04 0x0054b340: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchrwl04_20261107/results.tsv. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef long double float10;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(float, DAT_005d757c, 0x005d757c);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
float10 FUN_004c3ac0(float * p0);
int FUN_0054b630(int p0, int p1, undefined2 p2);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x0054b340 FUN_0054b340
/* [C1 2026-05-19] 5-param `(patch*, angle_tol_f, cos_thresh_f, vbuf, nbuf)`. */

int * FUN_0054b340(int *param_1,float param_2,float param_3,float *param_4,float *param_5)

{
  uint uVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  undefined4 uVar7;
  uint uVar8;
  int iVar9;
  float *pfVar10;
  ushort *puVar11;
  float *pfVar12;
  uint uVar13;
  float *pfVar14;
  uint uVar15;
  int *piVar16;
  undefined4 *puVar17;
  float10 fVar18;
  uint local_18;
  float fStack_c;
  float fStack_8;
  float fStack_4;
  
  pfVar14 = param_4;
  if (param_4 == (float *)0x0) {
    pfVar14 = (float *)param_1[1];
  }
  if (param_5 == (float *)0x0) {
    param_5 = (float *)param_1[1];
  }
  uVar13 = param_1[0xc];
  param_4 = (float *)(**(code **)(DAT_007d3ff8 + 0x108))(uVar13 * 0xc,0x30123);
  puVar17 = param_4;
  for (uVar8 = uVar13 * 3 & 0x3fffffff; uVar8 != 0; uVar8 = uVar8 - 1) {
    *puVar17 = 0;
    puVar17 = puVar17 + 1;
  }
  for (iVar9 = 0; iVar9 != 0; iVar9 = iVar9 + -1) {
    *(undefined1 *)puVar17 = 0;
    puVar17 = (undefined4 *)((int)puVar17 + 1);
  }
  if (uVar13 != 0) {
    pfVar10 = (float *)(*param_1 + 4);
    uVar8 = 0;
    do {
      uVar1 = uVar8 + 1;
      if (uVar1 < uVar13) {
        fVar3 = -param_2;
        pfVar12 = pfVar10 + 4;
        uVar15 = uVar1;
        do {
          fVar4 = pfVar10[-1] - pfVar12[-2];
          fVar5 = *pfVar10 - pfVar12[-1];
          fVar6 = pfVar10[1] - *pfVar12;
          if ((((fVar3 <= fVar4) && (fVar4 < param_2 != (fVar4 == param_2))) && (fVar3 <= fVar5)) &&
             (((fVar5 < param_2 != (fVar5 == param_2) && (fVar3 <= fVar6)) &&
              (fVar6 < param_2 != (fVar6 == param_2))))) {
            uVar7 = FUN_0054b630(param_4,uVar8,uVar15);
            param_4 = (float *)FUN_0054b630(uVar7,uVar15,uVar8);
          }
          uVar15 = uVar15 + 1;
          pfVar12 = pfVar12 + 3;
        } while (uVar15 < uVar13);
      }
      pfVar10 = pfVar10 + 3;
      uVar8 = uVar1;
    } while (uVar1 < uVar13);
    if (uVar13 != 0) {
      pfVar10 = pfVar14 + 1;
      pfVar12 = param_5;
      local_18 = uVar13;
      do {
        param_2 = *(float *)((int)pfVar12 + ((int)param_4 - (int)param_5));
        if (param_2 == 0.0) {
          if (param_5 != pfVar14) {
            *pfVar12 = pfVar10[-1];
            *(float *)((int)pfVar10 + ((int)param_5 - (int)pfVar14)) = *pfVar10;
            pfVar12[2] = pfVar10[1];
          }
        }
        else {
          fStack_c = pfVar10[-1];
          fStack_8 = *pfVar10;
          fStack_4 = pfVar10[1];
          puVar11 = *(ushort **)((int)pfVar10 + (int)((int)param_4 + (4 - (int)pfVar14)));
          do {
            pfVar2 = pfVar14 + (uint)*puVar11 * 3;
            if (param_3 <=
                pfVar2[2] * pfVar10[1] +
                *pfVar2 * pfVar10[-1] + pfVar14[(uint)*puVar11 * 3 + 1] * *pfVar10) {
              fStack_c = fStack_c + *pfVar2;
              fStack_8 = fStack_8 + pfVar14[(uint)*puVar11 * 3 + 1];
              fStack_4 = fStack_4 + pfVar14[(uint)*puVar11 * 3 + 2];
            }
            puVar11 = puVar11 + 1;
            param_2 = (float)((int)param_2 + -1);
          } while (param_2 != 0.0);
          fVar18 = (float10)FUN_004c3ac0(&fStack_c);
          if ((float10)DAT_005d757c < fVar18) {
            fVar18 = (float10)_DAT_005cc320 / fVar18;
            *pfVar12 = (float)((float10)fStack_c * fVar18);
            *(float *)((int)pfVar10 + ((int)param_5 - (int)pfVar14)) =
                 (float)((float10)fStack_8 * fVar18);
            pfVar12[2] = (float)(fVar18 * (float10)fStack_4);
          }
        }
        pfVar10 = pfVar10 + 3;
        pfVar12 = pfVar12 + 3;
        local_18 = local_18 - 1;
      } while (local_18 != 0);
      if (uVar13 != 0) {
        piVar16 = (int *)(param_4 + 2);
        do {
          if (*piVar16 != 0) {
            (**(code **)(DAT_007d3ff8 + 0x10c))(*piVar16);
          }
          *piVar16 = 0;
          piVar16[-2] = 0;
          piVar16[-1] = 0;
          piVar16 = piVar16 + 3;
          uVar13 = uVar13 - 1;
        } while (uVar13 != 0);
      }
    }
  }
  (**(code **)(DAT_007d3ff8 + 0x10c))(param_4);
  return param_1;
}
