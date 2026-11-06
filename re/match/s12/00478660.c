// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00478660: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs12_20261102/results.tsv. */
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
typedef long double float10;
MGLOBAL(float, DAT_005d757c, 0x005d757c);
void FUN_004783f0(int p0);
void __fastcall FUN_004785e0();
void FUN_004987b0(undefined4 p0);
void FUN_004b4550(float * p0, float * p1, int p2);
undefined4 FUN_004b46b0(float * p0, float * p1);
undefined4 FUN_004b5030(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
float10 FUN_004c39b0(float * p0, float * p1);
undefined4 FUN_004c3d90(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
void FUN_004c45f0(int p0);
float * FUN_004c4dc0(float * p0, float * p1);
// MATCH: 0x00478660 FUN_00478660
void FUN_00478660(int param_1,int param_2)

{
  int iVar1;
  undefined4 *puVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined1 uStack_178;
  undefined1 uStack_177;
  undefined1 uStack_176;
  undefined1 uStack_175;
  undefined4 auStack_174 [4];
  undefined4 uStack_164;
  undefined4 uStack_160;
  undefined4 uStack_15c;
  undefined4 uStack_158;
  undefined4 uStack_154;
  undefined4 uStack_150;
  undefined4 uStack_14c;
  undefined4 uStack_148;
  undefined4 uStack_144;
  undefined4 uStack_140;
  undefined4 uStack_13c;
  float fStack_138;
  float fStack_134;
  float fStack_12c;
  float fStack_128;
  float fStack_120;
  float fStack_11c;
  float fStack_114;
  float fStack_110;
  float fStack_108;
  float fStack_104;
  float fStack_100;
  float fStack_f8;
  float fStack_f4;
  float fStack_f0;
  float fStack_e8;
  float fStack_e4;
  float fStack_e0;
  undefined4 uStack_d8;
  undefined4 uStack_d4;
  undefined4 uStack_d0;
  int local_c8;
  undefined4 auStack_c4 [9];
  undefined1 auStack_a0 [64];
  undefined4 auStack_60 [12];
  undefined4 auStack_30 [12];
  
  iVar3 = *(int *)(param_2 + 0x14);
  *(int *)(param_1 + 0x16240) = iVar3;
  local_c8 = iVar3;
  FUN_004987b0("Number of AI polygons is %d\n",iVar3);
  iVar4 = 0;
  if (0 < iVar3) {
    puVar5 = (undefined4 *)(param_1 + 0xc1d8);
    do {
      uStack_177 = (undefined1)((uint)iVar4 >> 8);
      uStack_176 = 0;
      uStack_175 = 0xff;
      uStack_178 = (undefined1)iVar4;
      FUN_004b5030(param_2,&uStack_178,auStack_60,2);
      puVar2 = auStack_60;
      puVar6 = auStack_174;
      for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar6 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar6 = puVar6 + 1;
      }
      puVar2 = auStack_30;
      puVar6 = auStack_c4;
      for (iVar3 = 9; iVar3 != 0; iVar3 = iVar3 + -1) {
        *puVar6 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar6 = puVar6 + 1;
      }
      iVar3 = 0;
      puVar2 = auStack_c4;
      do {
        uStack_144 = *puVar2;
        uStack_140 = puVar2[1];
        uStack_13c = puVar2[2];
        iVar1 = FUN_004b46b0(&uStack_144,auStack_174);
        if (((iVar1 == 0) && (iVar1 = FUN_004b46b0(&uStack_144,auStack_174 + 3), iVar1 == 0)) &&
           (iVar1 = FUN_004b46b0(&uStack_144,&uStack_15c), iVar1 == 0)) {
          uStack_150 = uStack_144;
          uStack_14c = uStack_140;
          uStack_148 = uStack_13c;
          break;
        }
        iVar3 = iVar3 + 1;
        puVar2 = puVar2 + 3;
      } while (iVar3 < 3);
      FUN_004785e0(1);
      FUN_004b4550(puVar5 + -3,auStack_174,4);
      uStack_d4 = puVar5[-2];
      uStack_d0 = puVar5[-1];
      uStack_d8 = puVar5[-3];
      fStack_e4 = (float)puVar5[-5];
      fStack_e0 = (float)puVar5[-4];
      fStack_108 = fStack_e0 - fStack_e4 * DAT_005d757c;
      fStack_e8 = (float)puVar5[-6];
      fStack_104 = fStack_e8 * DAT_005d757c - fStack_e0 * DAT_005d757c;
      fStack_100 = fStack_e4 * DAT_005d757c - fStack_e8;
      FUN_004c39b0(&fStack_108,&fStack_108);
      fStack_f8 = fStack_100 * fStack_e4 - fStack_104 * fStack_e0;
      fStack_f4 = fStack_108 * fStack_e0 - fStack_100 * fStack_e8;
      fStack_f0 = fStack_104 * fStack_e8 - fStack_108 * fStack_e4;
      FUN_004c39b0(&fStack_f8,&fStack_f8);
      FUN_004c45f0(&fStack_108);
      FUN_004c4dc0(auStack_a0,&fStack_108);
      FUN_004c3d90(&fStack_138,auStack_174,4,auStack_a0);
      if (DAT_005d757c <= fStack_138) {
LAB_0047897c:
        if (DAT_005d757c < fStack_138) {
          if (fStack_134 <= DAT_005d757c) {
            if ((DAT_005d757c < fStack_138) && (fStack_134 < DAT_005d757c)) {
              puVar2 = puVar5 + 9;
              goto LAB_004789dc;
            }
          }
          else {
            puVar5[6] = auStack_174[0];
            puVar5[7] = auStack_174[1];
            puVar5[8] = auStack_174[2];
          }
        }
      }
      else {
        puVar2 = puVar5;
        if (DAT_005d757c <= fStack_134) {
          if ((DAT_005d757c <= fStack_138) || (fStack_134 <= DAT_005d757c)) goto LAB_0047897c;
          puVar5[3] = auStack_174[0];
          puVar5[4] = auStack_174[1];
          puVar5[5] = auStack_174[2];
        }
        else {
LAB_004789dc:
          *puVar2 = auStack_174[0];
          puVar2[1] = auStack_174[1];
          puVar2[2] = auStack_174[2];
        }
      }
      if (DAT_005d757c <= fStack_12c) {
LAB_00478a54:
        if (DAT_005d757c < fStack_12c) {
          if (fStack_128 <= DAT_005d757c) {
            if ((DAT_005d757c < fStack_12c) && (fStack_128 < DAT_005d757c)) {
              puVar2 = puVar5 + 9;
              goto LAB_00478ab4;
            }
          }
          else {
            puVar5[6] = auStack_174[3];
            puVar5[7] = uStack_164;
            puVar5[8] = uStack_160;
          }
        }
      }
      else {
        puVar2 = puVar5;
        if (DAT_005d757c <= fStack_128) {
          if ((DAT_005d757c <= fStack_12c) || (fStack_128 <= DAT_005d757c)) goto LAB_00478a54;
          puVar5[3] = auStack_174[3];
          puVar5[4] = uStack_164;
          puVar5[5] = uStack_160;
        }
        else {
LAB_00478ab4:
          *puVar2 = auStack_174[3];
          puVar2[1] = uStack_164;
          puVar2[2] = uStack_160;
        }
      }
      if (DAT_005d757c <= fStack_120) {
LAB_00478b2c:
        if (DAT_005d757c < fStack_120) {
          if (fStack_11c <= DAT_005d757c) {
            if ((DAT_005d757c < fStack_120) && (fStack_11c < DAT_005d757c)) {
              puVar2 = puVar5 + 9;
              goto LAB_00478b8c;
            }
          }
          else {
            puVar5[6] = uStack_15c;
            puVar5[7] = uStack_158;
            puVar5[8] = uStack_154;
          }
        }
      }
      else {
        puVar2 = puVar5;
        if (DAT_005d757c <= fStack_11c) {
          if ((DAT_005d757c <= fStack_120) || (fStack_11c <= DAT_005d757c)) goto LAB_00478b2c;
          puVar5[3] = uStack_15c;
          puVar5[4] = uStack_158;
          puVar5[5] = uStack_154;
        }
        else {
LAB_00478b8c:
          *puVar2 = uStack_15c;
          puVar2[1] = uStack_158;
          puVar2[2] = uStack_154;
        }
      }
      if (DAT_005d757c <= fStack_114) {
LAB_00478c04:
        if (DAT_005d757c < fStack_114) {
          if (fStack_110 <= DAT_005d757c) {
            if ((DAT_005d757c < fStack_114) && (fStack_110 < DAT_005d757c)) {
              puVar2 = puVar5 + 9;
              goto LAB_00478c64;
            }
          }
          else {
            puVar5[6] = uStack_150;
            puVar5[7] = uStack_14c;
            puVar5[8] = uStack_148;
          }
        }
      }
      else {
        puVar2 = puVar5;
        if (DAT_005d757c <= fStack_110) {
          if ((DAT_005d757c <= fStack_114) || (fStack_110 <= DAT_005d757c)) goto LAB_00478c04;
          puVar5[3] = uStack_150;
          puVar5[4] = uStack_14c;
          puVar5[5] = uStack_148;
        }
        else {
LAB_00478c64:
          *puVar2 = uStack_150;
          puVar2[1] = uStack_14c;
          puVar2[2] = uStack_148;
        }
      }
      iVar4 = iVar4 + 1;
      puVar5 = puVar5 + 0x13;
    } while (iVar4 < local_c8);
  }
  FUN_004783f0(param_1);
  return;
}
