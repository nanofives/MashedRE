// FLAGS: /O2 /GS /Ob1
/* h2 note: 18.7% raw form, flags /O2 /GS /Ob1. cxx FAIL: FUN_005c4d30 and FUN_005538a0 arg types. class (b). */
/* MATCH-RWL04 0x00553620: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned int undefined4;
typedef long double float10;
MGLOBAL(undefined1, DAT_00912be8, 0x00912be8);
MGLOBAL(undefined4, DAT_00912bf0, 0x00912bf0);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
float10 FUN_004c3b30(float p0);
float10 FUN_004c3b90(float p0);
float10 FUN_00552890(void);
int FUN_005533d0(int p0);
void FUN_005538a0(undefined4 * p0, float * p1, int p2);
undefined4 FUN_0055deb0(int p0);
int FUN_005c4bb0(int * p0, undefined4 p1);
undefined4 FUN_005c4d30(undefined4 * p0);
MGLOBAL(int, _DAT_00912be8, 0x00912be8);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x00553620 FUN_00553620
/* [C1 2026-05-19] 2-param `(dst_path, src_path)`. Copy src into dst with curve-segment
   tessellation. */

undefined4 * FUN_00553620(undefined4 *param_1,undefined4 *param_2)

{
  float fVar1;
  float fVar2;
  float fVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  int *piVar8;
  int *piVar9;
  float *pfVar10;
  float10 fVar11;
  float10 fVar12;
  int *local_54;
  int *local_50;
  int local_4c;
  int *local_48;
  int local_28;
  int local_24;
  float local_20;
  float local_1c;
  int local_18;
  int local_14;
  int local_10;
  int local_c;
  
  fVar11 = (float10)FUN_00552890();
  _DAT_00912be8 = (float)fVar11;
  if (param_2[1] == -1) {
    piVar4 = (int *)FUN_005c4d30(*param_2);
  }
  else {
    piVar4 = (int *)*param_2;
  }
  iVar5 = param_2[1];
  if (iVar5 == -1) {
    iVar5 = FUN_0055deb0(*param_2);
  }
  local_4c = 0;
  if (0 < iVar5) {
    pfVar10 = (float *)(piVar4 + 1);
    local_50 = piVar4 + 0xd;
    local_54 = piVar4 + 7;
    local_48 = piVar4 + -5;
    do {
      if (*piVar4 == 2) {
        local_28 = *local_48;
        local_24 = local_48[1];
        local_20 = *pfVar10;
        local_1c = pfVar10[1];
        local_18 = *local_54;
        local_14 = local_54[1];
        local_10 = *local_50;
        local_c = local_50[1];
        FUN_005538a0(param_1,&local_28,DAT_00912bf0);
        local_4c = local_4c + 2;
        local_48 = local_48 + 0xc;
        local_50 = local_50 + 0xc;
        local_54 = local_54 + 0xc;
        piVar4 = piVar4 + 0xc;
        pfVar10 = pfVar10 + 0xc;
      }
      else if (*piVar4 == 1) {
        iVar6 = FUN_0055deb0(*param_1);
        puVar7 = (undefined4 *)FUN_005c4bb0(*param_1,0x30190);
        *puVar7 = 1;
        puVar7[1] = *pfVar10;
        puVar7[2] = piVar4[2];
        fVar1 = (float)piVar4[2] - (float)puVar7[-4];
        fVar2 = (float)puVar7[-5] - *pfVar10;
        fVar3 = fVar1 * fVar1 + fVar2 * fVar2;
        fVar11 = (float10)FUN_004c3b30(fVar3);
        fVar12 = (float10)FUN_004c3b90(fVar3);
        puVar7[3] = (float)((float10)fVar1 * fVar12);
        puVar7[4] = (float)(fVar12 * (float10)fVar2);
        puVar7[5] = (float)fVar11 + (float)puVar7[-1];
        if (iVar6 < 2) {
          puVar7[-3] = puVar7[3];
          puVar7[-2] = puVar7[4];
        }
        else {
          fVar1 = (float)puVar7[-3] + (float)((float10)fVar1 * fVar12);
          fVar3 = (float)puVar7[-2] + (float)(fVar12 * (float10)fVar2);
          fVar2 = _DAT_005cc320 / ((float)puVar7[-2] * fVar3 + (float)puVar7[-3] * fVar1);
          puVar7[-3] = fVar2 * fVar1;
          puVar7[-2] = fVar2 * fVar3;
        }
      }
      else {
        piVar8 = (int *)FUN_005c4bb0(*param_1,0x301a1);
        piVar9 = piVar4;
        for (iVar6 = 6; iVar6 != 0; iVar6 = iVar6 + -1) {
          *piVar8 = *piVar9;
          piVar9 = piVar9 + 1;
          piVar8 = piVar8 + 1;
        }
      }
      local_54 = local_54 + 6;
      piVar4 = piVar4 + 6;
      local_48 = local_48 + 6;
      local_50 = local_50 + 6;
      pfVar10 = pfVar10 + 6;
      local_4c = local_4c + 1;
    } while (local_4c < iVar5);
  }
  if (param_2[2] == 0) {
    return param_1;
  }
  FUN_005533d0(param_1);
  return param_1;
}
