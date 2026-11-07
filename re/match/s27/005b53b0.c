// FLAGS: /O2 /GS /Ob1
/* MATCH-S27 h1: 17.2% (regs 29.2%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_005ce9f0, 0x005ce9f0);
MGLOBAL(undefined1, DAT_005dae8c, 0x005dae8c);
MGLOBAL(undefined4, DAT_00634438, 0x00634438);
MGLOBAL(undefined4, DAT_0063443c, 0x0063443c);
#ifdef __cplusplus
void FUN_004522d0(...);
#else
void FUN_004522d0(void);
#endif
#ifdef __cplusplus
undefined4 FUN_005aea00(...);
#else
undefined4 FUN_005aea00(void);
#endif
MGLOBAL(int, _DAT_005ce9f0, 0x005ce9f0);
MGLOBAL(int, _DAT_005dae8c, 0x005dae8c);
#ifdef __cplusplus
static int ROUND(double x) { return (int)(x + (x < 0 ? -0.5 : 0.5)); }
static int ABS(int x) { return x < 0 ? -x : x; }
#endif
// MATCH: 0x005b53b0 FUN_005b53b0
/* [C2 2026-06-01] EA-XA ADPCM block encoder: encodes one channel of 16-bit PCM into 8-byte ADPCM
   blocks (14 samples each: 1? */

int FUN_005b53b0(int param_1,uint param_2,uint *param_3,char param_4,char param_5)

{
  float fVar4;
  short *psVar1;
  float fVar2;
  short sVar3;
  float fVar5;
  uint uVar6;
  int iVar7;
  uint uVar8;
  int iVar10;
  uint uVar9;
  byte bVar11;
  uint uVar12;
  int iVar13;
  uint uVar14;
  float *pfVar15;
  byte *pbVar17;
  uint uVar16;
  short *psVar18;
  uint uVar19;
  float fStack_c8;
  uint uStack_c4;
  float fStack_b8;
  float fStack_b4;
  float fStack_bc;
  short *psStack_b0;
  float fStack_a4;
  uint *local_90;
  undefined4 uStack_8c;
  float fStack_78;
  float fStack_ac;
  uint uStack_88;
  int iStack_84;
  int iStack_80;
  float fStack_7c;
  float afStack_74 [15];
  float afStack_38 [14];
  
  psVar1 = (short *)(param_1 + param_5 * 2);
  uVar19 = (uint)param_4;
  uVar6 = (param_2 >> 1) / uVar19;
  local_90 = param_3;
  uVar12 = (uVar6 + 0xd) / 0xe;
  iVar7 = FUN_005aea00(uVar12,0x10806);
  uVar8 = uVar6;
  psVar18 = psVar1 + uVar19 * 2;
  uVar6 = uVar6 - 2;
  afStack_74[0] = (float)(uVar19 * 2);
  fStack_bc = (float)(int)*psVar1 * _DAT_005ce9f0;
  fStack_a4 = (float)(int)psVar1[uVar19] * _DAT_005ce9f0;
  uStack_c4 = 0;
  if (uVar12 != 0) {
    do {
      uVar16 = 0xe;
      if (uVar8 < 0xe) {
        uVar16 = uVar8;
      }
      uVar8 = uVar8 - uVar16;
      uVar9 = 0;
      pfVar15 = afStack_74;
      for (iVar13 = 0xe; pfVar15 = pfVar15 + 1, iVar13 != 0; iVar13 = iVar13 + -1) {
        *pfVar15 = 0.0;
      }
      if (uVar16 != 0) {
        do {
          sVar3 = *psVar18;
          psVar18 = psVar18 + uVar19;
          afStack_74[uVar9] = (float)(int)sVar3 * _DAT_005ce9f0;
          uVar9 = uVar9 + 1;
        } while (uVar9 < uVar16);
      }
      uVar16 = 0xffffffff;
      fStack_b4 = 3.4028235e+38;
      uVar9 = 0;
      pfVar15 = (float *)&DAT_0063443c;
      do {
        uVar14 = 0;
        fStack_b8 = 0.0;
        fStack_ac = fStack_bc;
        fVar2 = fStack_a4;
        do {
          fVar2 = afStack_74[uVar14 + 1];
          fVar5 = ABS(afStack_74[uVar14 + 1] - (fStack_ac * *pfVar15 + fVar4 * pfVar15[-1]));
          fVar4 = fVar2;
          if (fStack_b8 <= fVar5) {
            fStack_b8 = fVar5;
          }
          uVar14 = uVar14 + 1;
          fStack_ac = fVar4;
        } while (uVar14 < 0xe);
        if (fStack_b8 < fStack_b4 != (fStack_b8 == fStack_b4)) {
          fStack_b4 = fStack_b8;
          uVar16 = uVar9;
        }
        uVar9 = uVar9 + 1;
        pfVar15 = pfVar15 + 2;
      } while (uVar9 < 8);
      *(char *)(uStack_c4 + iVar7) = (char)uVar16;
      uStack_c4 = uStack_c4 + 1;
      fStack_bc = fVar4;
      fStack_a4 = fVar2;
    } while (uStack_c4 < uVar12);
  }
  fStack_bc = 0.0;
  fStack_c8 = 0.0;
  fStack_ac = (float)(int)*psVar1 * _DAT_005ce9f0;
  uStack_c4 = 0;
  psStack_b0 = psVar1 + uVar19 + uVar19;
  fVar2 = (float)(int)psVar1[uVar19] * _DAT_005ce9f0;
  if (uVar12 != 0) {
    do {
      uVar8 = 0xe;
      if (uVar6 < 0xe) {
        uVar8 = uVar6;
      }
      uVar6 = uVar6 - uVar8;
      uVar19 = 0;
      pfVar15 = afStack_74;
      for (iVar13 = 0xe; pfVar15 = pfVar15 + 1, iVar13 != 0; iVar13 = iVar13 + -1) {
        *pfVar15 = 0.0;
      }
      if (uVar8 != 0) {
        do {
          sVar3 = *psStack_b0;
          psStack_b0 = (short *)((int)psStack_b0 + (int)afStack_74[0]);
          uVar19 = uVar19 + 1;
          afStack_74[uVar19] = (float)(int)sVar3 * _DAT_005ce9f0;
        } while (uVar19 < uVar8);
      }
      fStack_b4 = 0.0;
      bVar11 = *(byte *)(uStack_c4 + iVar7);
      uVar19 = 0;
      do {
        fVar4 = fVar2;
        fVar2 = *(float *)((int)afStack_74 + uVar19 + 4);
        fVar5 = (*(float *)((int)afStack_74 + uVar19 + 4) -
                (fStack_ac * (float)(&DAT_0063443c)[(uint)bVar11 * 2] +
                fVar4 * (float)(&DAT_00634438)[(uint)bVar11 * 2])) * _DAT_005dae8c;
        *(float *)((int)afStack_38 + uVar19) = fVar5;
        fVar5 = ABS(fVar5);
        if (fVar5 > fStack_b4) {
          fStack_b4 = fVar5;
        }
        uVar19 = uVar19 + 4;
        fStack_ac = fVar4;
      } while (uVar19 < 0x38);
      fStack_7c = fVar2;
      iStack_84 = iVar13;
      iVar13 = (int)ROUND(fStack_b4);
      if (iVar13 < 0x8000) {
        if (iVar13 < -0x8000) {
          iVar13 = 0;
        }
      }
      else {
        iVar13 = 0x7fff;
      }
      uVar19 = 0x4000;
      iVar10 = 0xc;
      do {
        if ((uVar19 & (uVar19 >> 3) + iVar13) != 0) {
          if (iVar10 < 0) {
            iVar10 = 0;
          }
          else if (0xc < iVar10) {
            iVar10 = 0xc;
          }
          goto LAB_005b5716;
        }
        iVar10 = iVar10 + -1;
        uVar19 = uVar19 >> 1;
      } while (-1 < iVar10);
      iVar10 = 0;
LAB_005b5716:
      uStack_88 = 0;
      bVar11 = 0xc - (byte)iVar10;
      uStack_8c = (uint)(byte)(*(char *)(uStack_c4 + iVar7) << 4 | (byte)iVar10);
      uVar19 = 0;
      pbVar17 = (byte *)((int)&uStack_8c + 1);
      if (uVar8 != 0) {
        fStack_78 = (float)(1 << (bVar11 & 0x1f));
        fVar2 = fStack_bc;
        do {
          fStack_bc = fVar2;
          fVar2 = afStack_38[uVar19] -
                  (fStack_c8 * (float)(&DAT_0063443c)[(uint)*(byte *)(uStack_c4 + iVar7) * 2] +
                  fStack_bc * (float)(&DAT_00634438)[(uint)*(byte *)(uStack_c4 + iVar7) * 2]);
          iStack_80 = (int)ROUND(fStack_78 * fVar2);
          uVar16 = iStack_80 + 0x800U & 0xfffff000;
          if ((int)uVar16 < 0x8000) {
            if ((int)uVar16 < -0x8000) {
              uVar16 = 0xffff8000;
            }
          }
          else {
            uVar16 = 0x7fff;
          }
          if (0 == (uVar19 & 1)) {
            *pbVar17 = (byte)(uVar16 >> 8) & 0xf0;
          }
          else {
            *pbVar17 = *pbVar17 | (byte)((int)uVar16 >> 0xc) & 0xf;
            pbVar17 = pbVar17 + 1;
          }
          uVar19 = uVar19 + 1;
          fVar2 = (float)((int)uVar16 >> (bVar11 & 0x1f)) - fVar2;
          fStack_c8 = fStack_bc;
        } while (uVar19 < uVar8);
        fStack_bc = fVar2;
      }
      *local_90 = uStack_8c;
      local_90[1] = uStack_88;
      local_90 = local_90 + 2;
      uStack_c4 = uStack_c4 + 1;
      fVar2 = fStack_7c;
    } while (uStack_c4 < uVar12);
  }
  FUN_004522d0(iVar7);
  return uVar12 * 8;
}
