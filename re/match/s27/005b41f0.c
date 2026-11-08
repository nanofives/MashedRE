// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b41f0: 13.7% (regs 34.0%), raw Ghidra shape (C++ fixes: ROUND macro for C++ only, pointer casts). The float to int truncations are ROUND calls here; the matched sibling 0x005b4150 shows the original inlines fnstcw/fldcw/fistp, so the same inline conversion (not _ftol) is the likely next step. Not iterated. Class: b.
/* MATCH-S27 0x005b41f0: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs27_20261107/results.tsv. */
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
typedef unsigned char uchar;
typedef unsigned char byte;
#ifdef __cplusplus
#define ROUND(x) ((int)(x))
#endif
MGLOBAL(undefined1, DAT_005cc320, 0x005cc320);
int FUN_005b4060(int * p0, uint * p1);
uint FUN_005b4150(int p0, int p1, int p2);
MGLOBAL(int, _DAT_005cc320, 0x005cc320);
// MATCH: 0x005b41f0 FUN_005b41f0
/* [C2 2026-06-01] Linear-interpolation sample-rate converter (resampler) PCM transform ? a
   converter-node function ([[U-6909]], reached? */

uint FUN_005b41f0(uint *param_1,int param_2,int param_3,uint *param_4,undefined4 *param_5,
                 int *param_6)

{
  uint uVar1;
  float fVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  byte bVar10;
  undefined4 *puVar11;
  uint uVar12;
  byte *pbVar13;
  ushort *puVar14;
  uint local_90;
  float local_8c;
  float local_88;
  int local_50;
  int local_4c;
  float local_48;
  int local_44;
  uint local_40;
  int local_3c;
  uint local_38;
  undefined4 uStack_34;
  float afStack_30 [6];
  float afStack_18 [6];
  
  if (param_6 == (int *)0x0) {
    param_6 = &local_50;
  }
  iVar4 = FUN_005b4150((int)param_1,param_3,(int)param_4);
  puVar11 = (undefined4 *)(param_2 + param_3);
  *param_6 = iVar4;
  uVar5 = FUN_005b4060((int *)param_1,param_4);
  if (param_5 != (undefined4 *)0x0) {
    param_5 = (undefined4 *)((int)param_5 + *param_6);
  }
  bVar10 = (byte)param_4[6] & 1;
  if ((byte)param_4[3] == 8) {
    iVar4 = 2 - (uint)(bVar10 != 0);
  }
  else {
    iVar4 = 4 - (uint)(bVar10 != 0);
  }
  uVar6 = (uVar5 - *param_6) / (uint)(byte)((byte)param_4[3] >> 3);
  uVar9 = *param_4;
  local_90 = (uint)*(byte *)((int)param_4 + 0xd);
  uVar3 = local_90;
  local_8c = 0.0;
  uVar1 = *param_1;
  if (1 < *(byte *)((int)param_4 + 0xd)) {
    uVar6 = uVar6 & ~(local_90 - 1);
  }
  if (param_5 != (undefined4 *)0x0) {
    if (uVar1 != uVar9) {
      uVar7 = 0;
      if (local_90 != 0) {
        do {
          switch(iVar4) {
          case 1:
            local_8c = (float)*(byte *)(uVar7 + (int)puVar11);
            break;
          case 2:
            local_8c = (float)(int)*(char *)(uVar7 + (int)puVar11);
            break;
          case 3:
            local_8c = (float)*(ushort *)((int)puVar11 + uVar7 * 2);
            break;
          case 4:
            local_8c = (float)(int)*(short *)((int)puVar11 + uVar7 * 2);
            break;
          default:
            return 0;
          }
          afStack_30[uVar7] = local_8c;
          uVar7 = uVar7 + 1;
        } while (uVar7 < local_90);
      }
      uVar12 = 0;
      uVar7 = 0;
      local_90 = 0;
      local_88 = 0.0;
      if (uVar6 != 0) {
        do {
          local_40 = (uint)ROUND(local_88);
          uStack_34 = 0;
          local_38 = local_40;
          uVar8 = 0;
          if (uVar3 != 0) {
            local_48 = _DAT_005cc320 - (local_88 - (float)local_40);
            pbVar13 = (byte *)(uVar3 * uVar7 + (int)puVar11);
            puVar14 = (ushort *)((int)puVar11 + uVar3 * uVar7 * 2);
            do {
              switch(iVar4 + -1) {
              case 0:
                local_8c = (float)*pbVar13;
                break;
              case 1:
                local_8c = (float)(int)(char)*pbVar13;
                break;
              case 2:
                local_8c = (float)*puVar14;
                break;
              case 3:
                local_8c = (float)(int)(short)*puVar14;
              }
              afStack_18[uVar8] = local_8c;
              fVar2 = (local_88 - (float)local_40) * local_8c + local_48 * afStack_30[uVar8];
              switch(iVar4 + -1) {
              case 0:
                local_44 = (int)ROUND(fVar2);
                (*(uchar *)((char *)&local_44 + 0)) = (undefined1)(int)ROUND(fVar2);
                *(undefined1 *)(uVar12 + (int)param_5) = (undefined1)local_44;
                break;
              case 1:
                local_3c = (int)ROUND(fVar2);
                (*(uchar *)((char *)&local_3c + 0)) = (undefined1)(int)ROUND(fVar2);
                *(undefined1 *)(uVar12 + (int)param_5) = (undefined1)local_3c;
                break;
              case 2:
                local_4c = (int)ROUND(fVar2);
                (*(ushort *)((char *)&local_4c + 0)) = (undefined2)(int)ROUND(fVar2);
                (*(ushort *)((char *)&local_50 + 0)) = (undefined2)local_4c;
                goto LAB_005b453a;
              case 3:
                local_50 = (int)ROUND(fVar2);
                (*(ushort *)((char *)&local_50 + 0)) = (undefined2)(int)ROUND(fVar2);
LAB_005b453a:
                *(undefined2 *)((int)param_5 + uVar12 * 2) = (undefined2)local_50;
              }
              uVar12 = uVar12 + 1;
              afStack_30[uVar8] = local_8c;
              uVar8 = uVar8 + 1;
              puVar14 = puVar14 + 1;
              pbVar13 = pbVar13 + 1;
              uVar7 = local_90;
            } while (uVar8 < uVar3);
          }
          for (local_88 = local_88 + (float)uVar1 / (float)(int)uVar9; _DAT_005cc320 < local_88;
              local_88 = local_88 - _DAT_005cc320) {
            uVar7 = uVar7 + 1;
            local_90 = uVar7;
          }
        } while (uVar12 < uVar6);
      }
      return uVar5;
    }
    for (uVar9 = uVar5 >> 2; uVar9 != 0; uVar9 = uVar9 - 1) {
      *param_5 = *puVar11;
      puVar11 = puVar11 + 1;
      param_5 = param_5 + 1;
    }
    for (uVar9 = uVar5 & 3; uVar9 != 0; uVar9 = uVar9 - 1) {
      *(undefined1 *)param_5 = *(undefined1 *)puVar11;
      puVar11 = (undefined4 *)((int)puVar11 + 1);
      param_5 = (undefined4 *)((int)param_5 + 1);
    }
  }
  return uVar5 - *param_6;
}
