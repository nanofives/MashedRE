// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dabd0 (h4): 13.4% (regs 14.1%, ops 70.4%), raw converter body kept (globals retyped float, ROUND defined as plain truncation so the file builds as C++). Every float->int conversion (the two fixed-point step sizes and the four channel bytes per pixel) is the inline truncation sequence: store to a float temp, fnstcw, or ah,0xc, fldcw, fistp, fldcw restore. 9176 emits call __ftol for a C (int) cast, so this is an inline-asm RW conversion macro. That is the same finding as h2 on sibling 0x004d9a60. No C form without __asm gives these bytes. Class (c). */
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
typedef unsigned int uint;
/* ROUND: Ghidra's name for the inline fistp truncation (see header); C truncation here */
#define ROUND(x) (x)
void FUN_004daff0(uint * p0, uint p1, int p2, int p3, float * p4);
MGLOBAL(float, DAT_005cea64, 0x005cea64);
MGLOBAL(float, DAT_005d8d88, 0x005d8d88);
MGLOBAL(float, DAT_005cc320, 0x005cc320);
MGLOBAL(float, DAT_005cd04c, 0x005cd04c);
MGLOBAL(float, DAT_005cc32c, 0x005cc32c);
// MATCH: 0x004dabd0 FUN_004dabd0
/* [C1 2026-05-19] 32-bit-RGBA image resampler from descriptor `param_2` to descriptor `param_1`
   using bilinear-area weighted resampling. */

uint * FUN_004dabd0(uint *param_1,uint *param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  int iVar10;
  uint uVar11;
  undefined1 *puVar12;
  int iVar13;
  int iVar14;
  uint local_48;
  int local_3c;
  undefined1 local_30;
  undefined1 local_2c;
  undefined1 local_28;
  undefined1 local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  float local_8;
  float local_4;
  
  uVar4 = param_1[1];
  uVar11 = param_2[1];
  uVar5 = param_2[2];
  uVar6 = param_1[2];
  *param_1 = *param_1 | *param_2 & 2;
  iVar2 = (int)ROUND(((float)(int)uVar11 / (float)(int)uVar4) * DAT_005cea64);
  iVar3 = (int)ROUND(((float)(int)uVar5 / (float)(int)uVar6) * DAT_005cea64);
  uVar11 = 0;
  local_3c = 0;
  if (0 < (int)uVar6) {
    do {
      iVar14 = 0;
      if (0 < (int)uVar4) {
        iVar1 = iVar3 + -1 + uVar11;
        iVar10 = iVar1 >> 0x10;
        fVar7 = (float)(int)(iVar1 - uVar11) * DAT_005d8d88;
        puVar12 = (undefined1 *)(param_1[4] * local_3c + param_1[5] + 2);
        local_48 = uVar4;
        do {
          iVar1 = iVar2 + -1 + iVar14;
          if ((int)uVar11 >> 0x10 == iVar10) {
            FUN_004daff0(param_2,iVar14,iVar1,uVar11,&local_20);
            local_20 = fVar7 * local_20;
            local_1c = local_1c * fVar7;
            local_18 = local_18 * fVar7;
            local_14 = local_14 * fVar7;
          }
          else {
            iVar13 = (uVar11 & 0xffff0000) + 0x10000;
            FUN_004daff0(param_2,iVar14,iVar1,uVar11,&local_20);
            fVar8 = (float)(int)(iVar13 - uVar11) * DAT_005d8d88;
            local_20 = local_20 * fVar8;
            local_1c = local_1c * fVar8;
            local_18 = local_18 * fVar8;
            local_14 = local_14 * fVar8;
            for (; iVar13 >> 0x10 != iVar10; iVar13 = iVar13 + 0x10000) {
              FUN_004daff0(param_2,iVar14,iVar1,iVar13,&local_10);
              local_20 = local_10 + local_20;
              local_1c = local_1c + local_c;
              local_18 = local_18 + local_8;
              local_14 = local_14 + local_4;
            }
            FUN_004daff0(param_2,iVar14,iVar1,iVar13,&local_10);
            fVar8 = (float)(int)((iVar3 + -1 + uVar11) - iVar13) * DAT_005d8d88;
            local_10 = local_10 * fVar8;
            local_c = local_c * fVar8;
            local_8 = local_8 * fVar8;
            local_4 = local_4 * fVar8;
            local_20 = local_10 + local_20;
            local_1c = local_1c + local_c;
            local_18 = local_18 + local_8;
            local_14 = local_14 + local_4;
          }
          fVar8 = DAT_005cc320 / fVar7;
          local_20 = fVar8 * local_20;
          local_1c = local_1c * fVar8;
          local_18 = local_18 * fVar8;
          local_14 = local_14 * fVar8;
          fVar8 = local_1c * DAT_005cd04c;
          local_30 = (undefined1)(int)ROUND(local_20 * DAT_005cd04c + DAT_005cc32c);
          puVar12[-2] = local_30;
          fVar9 = local_18 * DAT_005cd04c;
          local_2c = (undefined1)(int)ROUND(fVar8 + DAT_005cc32c);
          puVar12[-1] = local_2c;
          fVar8 = local_14 * DAT_005cd04c;
          local_28 = (undefined1)(int)ROUND(fVar9 + DAT_005cc32c);
          *puVar12 = local_28;
          local_24 = (undefined1)(int)ROUND(fVar8 + DAT_005cc32c);
          puVar12[1] = local_24;
          iVar14 = iVar14 + iVar2;
          puVar12 = puVar12 + 4;
          local_48 = local_48 - 1;
        } while (local_48 != 0);
      }
      uVar11 = uVar11 + iVar3;
      local_3c = local_3c + 1;
    } while (local_3c < (int)uVar6);
  }
  return param_1;
}
