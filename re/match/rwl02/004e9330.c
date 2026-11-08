// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e9330: raw converter body (Ghidra structure, gotos kept), comments stripped, prototypes loosened for C++ (void-pointer or int params), F1/F2 typedefs for the engine vtable calls. Score 51.5% structure only (class b: source shape not found, size above budget). */
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
typedef int (*F1)(int);
typedef int (*F2)(int, int);
typedef unsigned char undefined1;
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(undefined4, DAT_006186d0, 0x006186d0);
uint FUN_004cbd30(uint p0, void * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, undefined4 * p2, uint * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004d7ff0(undefined4 p0);
int FUN_004d8060(void * p0, undefined4 p1);
void FUN_004d8480(void * p0);
undefined4 FUN_004d8bd0(int p0);
int FUN_004e1b60(void * p0, int p1, int p2);
int FUN_004e8750(int p0, int p1);
int FUN_004e89e0(int p0, ushort p1);
int FUN_004e8a10(int p0);
int FUN_004e8c70(int p0, int p1, uint p2);
undefined4 FUN_004e9850(int p0);
int FUN_004f3b60(int p0);
int FUN_004f3e90(undefined4 p0, int p1);
// MATCH: 0x004e9330 FUN_004e9330
int FUN_004e9330(uint param_1)

{
  undefined2 uVar1;
  uint uVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined2 *puVar8;
  int iVar9;
  int local_30;
  undefined4 local_2c;
  uint local_28;
  undefined4 local_24;
  undefined4 local_20;
  int local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int local_8;
  int local_4;
  
  uVar2 = param_1;
  iVar4 = FUN_004cc5e0(param_1,1,0,&param_1);
  if (iVar4 == 0) {
    return 0;
  }
  if ((param_1 < 0x35000) || (0x37002 < param_1)) {
    local_30 = 2;
    local_2c = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&local_30);
    return 0;
  }
  iVar4 = FUN_004cbd30(uVar2,&local_28,0x10);
  if (iVar4 != 0x10) {
    return 0;
  }
  iVar4 = FUN_004e8c70(local_20,local_24,local_28);
  if (iVar4 == 0) {
    return 0;
  }
  if ((local_1c < 2) || (iVar5 = FUN_004e8750(iVar4,local_1c + -1), -1 < iVar5)) {
    if (((*(uint *)(iVar4 + 8) & 0x1000000) == 0) && (*(int *)(iVar4 + 0x14) != 0)) {
      if (((local_28 & 8) != 0) &&
         (iVar5 = *(int *)(iVar4 + 0x14) * 4,
         iVar6 = FUN_004cbd30(uVar2,(void *)*(undefined4 *)(iVar4 + 0x30),iVar5), iVar6 != iVar5))
      goto LAB_004e93c9;
      if (0 < *(int *)(iVar4 + 0x1c)) {
        iVar5 = 0;
        local_30 = *(int *)(iVar4 + 0x14) << 3;
        if (0 < *(int *)(iVar4 + 0x1c)) {
          puVar7 = (undefined4 *)(iVar4 + 0x34);
          do {
            iVar6 = FUN_004cc790(uVar2,*puVar7,local_30);
            if (iVar6 == 0) goto LAB_004e959f;
            iVar5 = iVar5 + 1;
            puVar7 = puVar7 + 1;
          } while (iVar5 < *(int *)(iVar4 + 0x1c));
        }
      }
      local_30 = *(int *)(iVar4 + 0x10);
      if (local_30 != 0) {
        puVar8 = *(undefined2 **)(iVar4 + 0x2c);
        iVar5 = local_30 * 8;
        iVar6 = FUN_004cbd30(uVar2,(void *)puVar8,iVar5);
        if (iVar6 != iVar5) {
LAB_004e959f:
          sVar3 = *(short *)(iVar4 + 0xe);
          if (sVar3 == 1 || sVar3 + -1 < 0) {
            if (*(int *)(iVar4 + 0x58) != 0) {
              FUN_004d8bd0(*(int *)(iVar4 + 0x58));
            }
            *(undefined2 *)(iVar4 + 0xe) = *(undefined2 *)(iVar4 + 0xe);
            FUN_004e89e0(iVar4,0xfff);
            FUN_004d8060(&DAT_006186d0,iVar4);
            if (*(int *)(iVar4 + 0x5c) != 0) {
              (*(F1 *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar4 + 0x5c));
              *(undefined4 *)(iVar4 + 0x5c) = 0;
            }
            FUN_004f3b60(iVar4 + 0x20);
            *(short *)(iVar4 + 0xe) = *(short *)(iVar4 + 0xe) + -1;
            (*(F1 *)(DAT_007d3ff8 + 0x10c))(iVar4);
            return 0;
          }
          goto LAB_004e97a4;
        }
        for (; local_30 != 0; local_30 = local_30 + -1) {
          uVar1 = *puVar8;
          *puVar8 = puVar8[1];
          puVar8[1] = uVar1;
          uVar1 = puVar8[2];
          puVar8[2] = puVar8[3];
          puVar8[3] = uVar1;
          puVar8 = puVar8 + 4;
        }
      }
    }
    iVar5 = 0;
    local_30 = 0;
    if (0 < *(int *)(iVar4 + 0x18)) {
      do {
        iVar9 = *(int *)(iVar4 + 0x5c) + iVar5;
        iVar6 = FUN_004cbd30(uVar2,&local_18,0x18);
        if (iVar6 != 0x18) goto LAB_004e959f;
        *(undefined4 *)(iVar9 + 4) = local_18;
        *(undefined4 *)(iVar9 + 8) = local_14;
        *(undefined4 *)(iVar9 + 0xc) = local_10;
        *(undefined4 *)(iVar9 + 0x10) = local_c;
        if (local_8 == 0) {
LAB_004e9640:
          if ((local_4 != 0) &&
             (iVar6 = FUN_004cc790(uVar2,*(undefined4 *)(iVar9 + 0x18),*(int *)(iVar4 + 0x14) * 0xc)
             , iVar6 == 0)) goto LAB_004e96bf;
        }
        else {
          if (local_4 == 0) {
            iVar6 = FUN_004cc790(uVar2,*(undefined4 *)(iVar9 + 0x14),*(int *)(iVar4 + 0x14) * 0xc);
            if (iVar6 != 0) goto LAB_004e9640;
            goto LAB_004e959f;
          }
          iVar6 = FUN_004cc790(uVar2,*(undefined4 *)(iVar9 + 0x14),*(int *)(iVar4 + 0x14) * 0x18);
          if (iVar6 == 0) goto LAB_004e959f;
        }
        local_30 = local_30 + 1;
        iVar5 = iVar5 + 0x1c;
      } while (local_30 < *(int *)(iVar4 + 0x18));
    }
    iVar5 = FUN_004cc5e0(uVar2,8,0,&param_1);
    if (iVar5 == 0) {
      return 0;
    }
    if ((param_1 < 0x35000) || (0x37002 < param_1)) {
      sVar3 = *(short *)(iVar4 + 0xe);
      if (sVar3 == 1 || sVar3 + -1 < 0) {
        if (*(int *)(iVar4 + 0x58) != 0) {
          FUN_004d8bd0(*(int *)(iVar4 + 0x58));
        }
        *(short *)(iVar4 + 0xe) = *(short *)(iVar4 + 0xe) + -1;
        FUN_004e9850(iVar4);
      }
      else {
        *(short *)(iVar4 + 0xe) = sVar3 + -1;
      }
      local_30 = 2;
      local_2c = FUN_004d7ff0(0x80000004);
      FUN_004d8480(&local_30);
      return 0;
    }
    iVar5 = FUN_004f3e90(uVar2,iVar4 + 0x20);
    if (iVar5 == 0) {
LAB_004e96bf:
      sVar3 = *(short *)(iVar4 + 0xe);
    }
    else {
      iVar5 = FUN_004e1b60(&DAT_006186d0,uVar2,iVar4);
      if (iVar5 != 0) {
        iVar5 = FUN_004e8a10(iVar4);
        if (iVar5 != 0) {
          return iVar4;
        }
        sVar3 = *(short *)(iVar4 + 0xe);
        if (sVar3 == 1 || sVar3 + -1 < 0) {
          if (*(int *)(iVar4 + 0x58) != 0) {
            FUN_004d8bd0(*(int *)(iVar4 + 0x58));
          }
          *(undefined2 *)(iVar4 + 0xe) = *(undefined2 *)(iVar4 + 0xe);
          FUN_004e89e0(iVar4,0xfff);
          FUN_004d8060(&DAT_006186d0,iVar4);
          if (*(int *)(iVar4 + 0x5c) != 0) {
            (*(F1 *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar4 + 0x5c));
            *(undefined4 *)(iVar4 + 0x5c) = 0;
          }
          FUN_004f3b60(iVar4 + 0x20);
          *(short *)(iVar4 + 0xe) = *(short *)(iVar4 + 0xe) + -1;
          (*(F1 *)(DAT_007d3ff8 + 0x10c))(iVar4);
          return 0;
        }
        goto LAB_004e97a4;
      }
      sVar3 = *(short *)(iVar4 + 0xe);
    }
    if (sVar3 + -1 < 1) {
      if (*(int *)(iVar4 + 0x58) != 0) {
        FUN_004d8bd0(*(int *)(iVar4 + 0x58));
      }
      *(short *)(iVar4 + 0xe) = *(short *)(iVar4 + 0xe) + -1;
      FUN_004e9850(iVar4);
      return 0;
    }
  }
  else {
LAB_004e93c9:
    sVar3 = *(short *)(iVar4 + 0xe);
    if (sVar3 == 1 || sVar3 + -1 < 0) {
      if (*(int *)(iVar4 + 0x58) != 0) {
        FUN_004d8bd0(*(int *)(iVar4 + 0x58));
      }
      *(undefined2 *)(iVar4 + 0xe) = *(undefined2 *)(iVar4 + 0xe);
      FUN_004e89e0(iVar4,0xfff);
      FUN_004d8060(&DAT_006186d0,iVar4);
      if (*(int *)(iVar4 + 0x5c) != 0) {
        (*(F1 *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar4 + 0x5c));
        *(undefined4 *)(iVar4 + 0x5c) = 0;
      }
      FUN_004f3b60(iVar4 + 0x20);
      *(short *)(iVar4 + 0xe) = *(short *)(iVar4 + 0xe) + -1;
      (*(F1 *)(DAT_007d3ff8 + 0x10c))(iVar4);
      return 0;
    }
  }
LAB_004e97a4:
  *(short *)(iVar4 + 0xe) = sVar3 + -1;
  return 0;
}
