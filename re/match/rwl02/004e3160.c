// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e3160: raw converter body (Ghidra structure, gotos kept), comments stripped, prototypes loosened for C++ (void-pointer or int params), F1/F2 typedefs for the engine vtable calls. Score 9.9% structure only (class b: source shape not found, size above budget). */
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
typedef unsigned char byte;
// MATCH: 0x004e3160 FUN_004e3160
void FUN_004e3160(undefined4 *param_1,uint param_2,uint param_3,int param_4,uint param_5,
                 uint param_6)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  uint uVar3;
  byte bVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  uint *puVar7;
  uint uVar8;
  undefined4 *puVar9;
  uint uVar10;
  undefined4 *puVar11;
  bool bVar12;
  undefined4 *local_190;
  undefined4 **local_188;
  undefined4 *local_180;
  undefined4 *local_17c;
  int local_178;
  undefined4 *local_174 [93];
  
  if (((param_1 != (undefined4 *)0x0) && (param_4 + 4U <= param_3)) && (param_5 < param_6)) {
    if (5 < param_2) {
      if (param_6 == 0) {
        bVar4 = 0xff;
      }
      else {
        bVar4 = 0;
        while (param_6 = param_6 >> 1, param_6 != 0) {
          bVar4 = bVar4 + 1;
        }
      }
      local_180 = param_1;
      local_17c = (undefined4 *)((param_2 - 1) * param_3 + (int)param_1);
      local_188 = local_174;
      local_178 = 1 << (bVar4 & 0x1f);
      do {
        local_190 = local_188[-1];
        puVar11 = local_188[-3];
        puVar6 = local_188[-2];
        local_188 = local_188 + -3;
        puVar9 = puVar11;
        puVar5 = puVar6;
        do {
          do {
            uVar8 = *(uint *)((int)puVar9 + param_4);
            while ((uVar8 & (uint)local_190) == 0) {
              puVar9 = (undefined4 *)((int)puVar9 + param_3);
              if (puVar6 < puVar9) goto LAB_004e3267;
              uVar8 = *(uint *)((int)puVar9 + param_4);
            }
            uVar8 = *(uint *)((int)puVar6 + param_4);
            while ((uVar8 & (uint)local_190) != 0) {
              puVar6 = (undefined4 *)((int)puVar6 - param_3);
              if (puVar6 < puVar9) goto LAB_004e3267;
              uVar8 = *(uint *)((int)puVar6 + param_4);
            }
            if (3 < param_3) {
              uVar8 = param_3 >> 2;
              puVar2 = puVar9;
              do {
                uVar1 = *(undefined4 *)(((int)puVar6 - (int)puVar9) + (int)puVar2);
                *(undefined4 *)(((int)puVar6 - (int)puVar9) + -4 + (int)(puVar2 + 1)) = *puVar2;
                *puVar2 = uVar1;
                uVar8 = uVar8 - 1;
                puVar2 = puVar2 + 1;
              } while (uVar8 != 0);
            }
            puVar9 = (undefined4 *)((int)puVar9 + param_3);
            puVar6 = (undefined4 *)((int)puVar6 - param_3);
          } while (puVar9 <= puVar6);
LAB_004e3267:
          local_190 = (undefined4 *)((uint)local_190 >> 1);
          if (local_190 == (undefined4 *)0x0) break;
          if ((undefined4 *)(param_3 * 5 + (int)((int)puVar6 + param_3)) <= puVar5) {
            *local_188 = (undefined4 *)((int)puVar6 + param_3);
            local_188[1] = puVar5;
            local_188[2] = local_190;
            local_188 = local_188 + 3;
          }
          puVar6 = (undefined4 *)((int)puVar9 - param_3);
          puVar9 = puVar11;
          puVar5 = puVar6;
        } while ((undefined4 *)(param_3 * 5 + (int)puVar11) <= puVar6);
      } while (local_188 != &local_180);
    }
    if (1 < param_2) {
      local_190 = (undefined4 *)(param_2 - 1);
      uVar8 = 4;
      if ((uint)local_190 < 4) {
        uVar8 = (uint)local_190;
      }
      uVar10 = *(uint *)((int)param_1 + param_4 + uVar8 * param_3);
      puVar7 = (uint *)((int)param_1 + param_4 + (uVar8 - 1) * param_3);
      uVar3 = uVar8 - 1;
      do {
        if (*puVar7 < uVar10) {
          uVar10 = *puVar7;
          uVar8 = uVar3;
        }
        puVar7 = (uint *)((int)puVar7 - param_3);
        bVar12 = uVar3 != 0;
        uVar3 = uVar3 - 1;
      } while (bVar12);
      if ((uVar8 != 0) && (3 < param_3)) {
        uVar10 = param_3 >> 2;
        puVar11 = param_1;
        do {
          uVar1 = *(undefined4 *)(uVar8 * param_3 + (int)puVar11);
          *(undefined4 *)(uVar8 * param_3 + (int)puVar11) = *puVar11;
          *puVar11 = uVar1;
          puVar11 = puVar11 + 1;
          uVar10 = uVar10 - 1;
        } while (uVar10 != 0);
      }
      if (local_190 != (undefined4 *)0x0) {
        puVar7 = (uint *)((int)param_1 + param_4 + param_3);
        do {
          uVar8 = *puVar7;
          uVar10 = *(uint *)((int)param_1 + param_4);
          puVar11 = param_1;
          while (uVar8 < uVar10) {
            if (3 < param_3) {
              uVar10 = param_3 >> 2;
              puVar6 = puVar11;
              do {
                uVar1 = *(undefined4 *)((int)puVar6 + param_3);
                *(undefined4 *)((int)puVar6 + param_3) = *puVar6;
                *puVar6 = uVar1;
                puVar6 = puVar6 + 1;
                uVar10 = uVar10 - 1;
              } while (uVar10 != 0);
            }
            puVar11 = (undefined4 *)((int)puVar11 - param_3);
            uVar10 = *(uint *)((int)puVar11 + param_4);
          }
          puVar7 = (uint *)((int)puVar7 + param_3);
          param_1 = (undefined4 *)((int)param_1 + param_3);
          local_190 = (undefined4 *)((int)local_190 - 1);
        } while (local_190 != (undefined4 *)0x0);
      }
    }
  }
  return;
}
