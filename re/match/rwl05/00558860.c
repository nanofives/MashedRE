// FLAGS: /O2 /Ob1 /G5
/* NOTE: the raw `-NAN` operand at pfVar14[2] had no address; replaced by 0.0f, a placeholder (not matched). */
/* MATCH-RWL05 0x00558860: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
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
typedef unsigned char undefined;
typedef unsigned char undefined1;
typedef unsigned int undefined4;
typedef unsigned int uint;
MGLOBAL(int *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
MGLOBAL(undefined1, DAT_005cc32c, 0x005cc32c);
MGLOBAL(char, DAT_00623f88, 0x00623f88);
undefined * FUN_00558180(undefined4 * p0, int * p1);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
MGLOBAL(int, _DAT_005cc32c, 0x005cc32c);

// MATCH: 0x00558860 FUN_00558860
/* [C1 2026-05-19] Calls `FUN_00558180(&local_1c, local_5c)` (cited 0x0055886f) — fetches font
   metrics into stack vars. */

int FUN_00558860(int param_1,uint param_2,int param_3,int param_4,int param_5,int param_6,
                int param_7,short param_8,int param_9)

{
  float fVar1;
  int iVar2;
  int iVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  short sVar12;
  uint uVar13;
  float *pfVar14;
  uint uVar15;
  int iVar16;
  int iVar17;
  int iVar18;
  float *pfVar19;
  short *psVar20;
  undefined1 local_5c [20];
  uint local_48;
  int local_3c;
  int local_38;
  int local_34;
  int local_30;
  int local_24;
  int local_20;
  int local_1c;
  int local_18;
  int local_14;
  int local_10;
  uint local_8;
  
  FUN_00558180(&local_1c,local_5c);
  fVar7 = _DAT_005cc320 / (float)*(int *)(param_3 + 0x10);
  iVar2 = *(int *)(*DAT_007d3ff8 + 0x60);
  iVar3 = *(int *)(iVar2 + 0x10);
  iVar2 = *(int *)(iVar2 + 0xc);
  fVar9 = _DAT_005cc320 / (float)*(int *)(param_3 + 0xc);
  fVar10 = fVar9 * _DAT_005cc32c;
  fVar8 = fVar7 * _DAT_005cc32c;
  fVar1 = (float)DAT_007d3ff8[6];
  fVar11 = _DAT_005cc320 / *(float *)(*DAT_007d3ff8 + 0x80);
  if (param_9 == 0) {
    DAT_00623f88 = '\0';
  }
  iVar18 = 0;
  param_9 = 0;
  local_48 = 0;
  if (param_2 != 0) {
    psVar20 = (short *)(param_7 + 4);
    pfVar19 = (float *)(param_6 + 0x18);
    do {
      if (*(char *)(local_48 + param_1) != DAT_00623f88) {
        local_34 = local_1c + param_4;
        local_38 = param_5;
        local_30 = local_18 + param_5;
        if ((((param_5 < iVar3) && (0 < local_30)) && (param_4 < iVar2)) && (0 < local_34)) {
          uVar15 = (int)*(char *)(local_48 + param_1) - 0x20;
          uVar13 = uVar15 / local_8;
          iVar18 = (uVar15 - uVar13 * local_8) * local_14;
          iVar17 = local_10 * uVar13;
          local_24 = local_1c + iVar18;
          local_20 = local_18 + iVar17;
          if (param_5 < 0) {
            iVar17 = iVar17 - param_5;
            local_38 = 0;
          }
          if (iVar3 + -1 < local_30) {
            local_20 = (iVar17 - local_38) + -1 + iVar3;
            local_30 = iVar3 + -1;
          }
          local_3c = param_4;
          if (param_4 < 0) {
            iVar18 = iVar18 - param_4;
            local_3c = 0;
          }
          if (iVar2 + -1 < local_34) {
            local_24 = (iVar18 - local_3c) + -1 + iVar2;
            local_34 = iVar2 + -1;
          }
          pfVar14 = pfVar19 + -4;
          iVar16 = 4;
          fVar4 = (float)iVar18 * fVar9 + fVar10;
          pfVar19[-1] = fVar4;
          fVar5 = (float)iVar17 * fVar7 + fVar8;
          *pfVar19 = fVar5;
          pfVar19[6] = fVar4;
          fVar4 = (float)local_20 * fVar7 + fVar8;
          pfVar19[7] = fVar4;
          fVar6 = (float)local_24 * fVar9 + fVar10;
          pfVar19[0xd] = fVar6;
          pfVar19[0xe] = fVar4;
          pfVar19[0x14] = fVar6;
          pfVar19[0x15] = fVar5;
          pfVar19[-6] = (float)local_3c;
          pfVar19[-5] = (float)local_38;
          pfVar19[1] = (float)local_3c;
          pfVar19[2] = (float)local_30;
          pfVar19[8] = (float)local_34;
          pfVar19[9] = (float)local_30;
          pfVar19[0xf] = (float)local_34;
          pfVar19[0x10] = (float)local_38;
          do {
            pfVar14[2] = 0.0f;
            pfVar14[1] = fVar11;
            *pfVar14 = fVar1;
            pfVar14 = pfVar14 + 7;
            iVar16 = iVar16 + -1;
          } while (iVar16 != 0);
          pfVar19 = pfVar19 + 0x1c;
          sVar12 = param_8 + (short)param_9 * 4;
          psVar20[-2] = sVar12;
          psVar20[1] = sVar12;
          psVar20[-1] = sVar12 + 1;
          iVar18 = param_9 + 1;
          *psVar20 = sVar12 + 2;
          psVar20[2] = sVar12 + 2;
          psVar20[3] = sVar12 + 3;
          psVar20 = psVar20 + 6;
          param_9 = iVar18;
        }
      }
      param_4 = param_4 + local_1c;
      local_48 = local_48 + 1;
    } while (local_48 < param_2);
  }
  return iVar18;
}
