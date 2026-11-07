// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 NOTE: permuter best 57.1% (regs 69.0%, ops 90.5%, 294/293 ins), raw converter start. NOT port-ready: the VS2022 C++ check FAILS (array/pointer argument types for FUN_005acaa0/005aca80/005aec00/005ae0c0, `code` typedef with several call arities through DAT_007d3ff8 + 0xf4/0xcc, FUN_005aa0c0 callback argument). Residual (b): ops 90.5% but register allocation differs over a 290-instruction body; budget spent. */
/* MATCH-S25 0x005ac210: raw g2s.py output (re/tools/match/g2s.py on decomp_pc.py --port --callees), body
   unchanged; globals moved to MGLOBAL. Score: see verify/matchs25_20261107/results.tsv. */
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
typedef unsigned char byte;
typedef int code();
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_00914700, 0x00914700);
MGLOBAL(undefined1, DAT_005e6194, 0x005e6194);
MGLOBAL(undefined1, DAT_005e6e60, 0x005e6e60);
MGLOBAL(undefined1, FUN_005aaaa0, 0x005aaaa0);
MGLOBAL(undefined1, FUN_005ac770, 0x005ac770);
undefined4 FUN_005aa060(undefined4 p0, undefined4 * p1, undefined4 * p2);
undefined4 * FUN_005aa0c0(undefined4 * p0, int * p1, undefined * p2, undefined4 p3, int p4);
void FUN_005aaac0(undefined * p0, undefined4 p1);
int FUN_005aba20(int p0, undefined4 p1, undefined4 * p2, undefined4 * p3, uint p4, int p5);
uint FUN_005ac7b0(uint p0, undefined4 p1, undefined4 p2);
undefined4 FUN_005ac900(undefined4 p0);
void FUN_005ac980(undefined4 * p0, undefined4 * p1, int p2);
int FUN_005aca80(int p0);
undefined4 * FUN_005acaa0(undefined4 * p0, undefined4 * p1, int p2, int p3);
undefined4 FUN_005acd10(int p0, undefined4 p1);
int FUN_005adf30(byte * p0, byte * p1);
undefined4 * FUN_005ae0c0(undefined4 * p0, undefined4 * p1, undefined4 * p2);
undefined4 FUN_005aea00(void);
void FUN_005aec00(int p0, uint p1);
// MATCH: 0x005ac210 FUN_005ac210

int FUN_005ac210(uint *param_1,undefined4 *param_2,int param_3,int *param_4,uint param_5)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  uint *puVar9;
  uint *puVar10;
  uint uVar11;
  uint *local_54;
  uint *local_50;
  int local_4c;
  int iStack_48;
  uint local_44;
  int local_40;
  undefined4 *local_3c;
  int local_38 [7];
  undefined1 local_1c [28];
  
  uVar2 = param_5;
  puVar9 = param_1 + 1;
  param_1 = (uint *)*param_1;
  local_50 = (uint *)0x0;
  uVar11 = param_5 >> 4 & 1;
  local_54 = (uint *)0x0;
  local_4c = 0;
  if (uVar11 != 0) {
    FUN_005aec00(&param_1,4);
  }
  FUN_005acaa0(local_1c,puVar9,0,uVar11);
  iVar3 = FUN_005aca80(local_1c);
  iVar3 = (int)puVar9 + iVar3;
  FUN_005acaa0(local_38,iVar3,0,uVar11);
  iVar4 = FUN_005aca80(local_38);
  puVar9 = (uint *)(iVar3 + iVar4);
  if (!((uVar2 & 1) == 0)) {
    local_44 = uVar2 >> 1 & 1;
  } else {
    local_44 = *puVar9;
    if (uVar11 != 0) {
      FUN_005aec00(puVar9,4);
    }
  }
  puVar10 = puVar9 + 1;
  if (((uint)param_1 & 1) != 0) {
    if ((uVar2 & 8) == 0) {
      FUN_005ae0c0(puVar10,puVar10,uVar11);
      local_50 = puVar10;
    }
    puVar10 = puVar9 + 5;
  }
  if (((uint)param_1 & 2) != 0) {
    if ((uVar2 & 4) == 0) {
      local_54 = puVar10;
    }
    iVar3 = (**(code **)(DAT_007d3ff8 + 0xf4))(puVar10);
    puVar10 = (uint *)((int)puVar10 + (iVar3 + DAT_00914700 & ~(DAT_00914700 - 1U)));
  }
  iVar3 = param_3;
  puVar5 = param_2;
  if (((uint)param_1 & 4) != 0) {
    if (param_2 == (undefined4 *)0x0) {
      if (!(param_3 == 0)) {
        local_40 = param_3;
        local_3c = param_2;
        FUN_005aa0c0(0,0,FUN_005ac770,&local_40,1);
        puVar5 = local_3c;
        if (local_3c == (undefined4 *)0x0) {
          return 0;
        }
      } else {
        FUN_005ae0c0(puVar10,puVar10,uVar11);
        puVar5 = (undefined4 *)FUN_005ac900(puVar10);
        if (puVar5 == (undefined4 *)0x0) {
          return 0;
        }
        iVar4 = FUN_005adf30(*puVar5,&DAT_005e6194);
        if (iVar4 == 0) {
          puVar5 = (undefined4 *)FUN_005ac900(&DAT_005e6e60);
          iVar4 = FUN_005acd10(puVar5,local_38);
          if (iVar4 == 0) {
            if (puVar5[10] == 0) {
              uVar6 = 0;
            }
            else {
              uVar6 = *(undefined4 *)(puVar5[10] + puVar5[0xb] * 4);
            }
            FUN_005ac980(uVar6,local_38,0);
          }
        }
      }
    }
    puVar10 = puVar10 + 4;
  }
  if (param_4 == (int *)0x0) {
    param_4 = local_38;
  }
  if ((*param_4 == 0) || ((param_5 & 0x20) != 0)) {
    *param_4 = local_38[0];
  }
  if (!(((uint)param_1 & 8) == 0)) {
    if (iVar3 != 0) goto LAB_005ac442;
    if (puVar5 == (undefined4 *)0x0) {
      FUN_005ae0c0(puVar10,puVar10,uVar11);
      iVar4 = FUN_005aa060(0,puVar10,0);
      if (iVar4 == 0) {
        return 0;
      }
    }
    else {
      iVar4 = puVar5[0xc];
    }
  } else {
    iVar4 = local_4c;
    if (iVar3 != 0) goto LAB_005ac442;
  }
  local_4c = 0;
  iStack_48 = iVar4;
  FUN_005aaac0(FUN_005aaaa0,&local_4c);
  param_3 = local_4c;
LAB_005ac442:
  iVar4 = iVar3 + 0x5c;
  iVar3 = puVar5[0xd];
  if (local_54 == (uint *)0x0) {
    iVar7 = 0;
  }
  else {
    iVar7 = (**(code **)(DAT_007d3ff8 + 0xf4))(local_54);
    iVar7 = iVar7 + 1;
  }
  iVar7 = (-(uint)(local_50 != (uint *)0x0) & 0x10) + iVar4 + iVar7;
  iVar8 = FUN_005aea00(iVar7,0x30806);
  if (iVar8 != 0) {
    if (local_50 != (uint *)0x0) {
      iVar1 = iVar4 + iVar8;
      *(uint *)(iVar4 + iVar8) = *local_50;
      *(uint *)(iVar1 + 4) = local_50[1];
      *(uint *)(iVar1 + 0xc) = local_50[3];
      local_50 = (uint *)(iVar4 + iVar8);
      iVar4 = iVar3 + 0x6c;
      *(uint *)(iVar1 + 8) = local_50[2];
    }
    if (local_54 != (uint *)0x0) {
      (**(code **)(DAT_007d3ff8 + 0xcc))((uint *)(iVar4 + iVar8),local_54);
      local_54 = (uint *)(iVar4 + iVar8);
    }
    iVar3 = FUN_005aba20(puVar5,param_3,local_1c,param_4,local_44,iVar8);
    if ((iVar3 != 0) && (iVar7 != 0)) {
      *(uint *)(iVar3 + 0x54) = *(uint *)(iVar3 + 0x54) & 0xfffffffb;
      iVar4 = FUN_005ac7b0(iVar3,local_50,local_54);
      if (iVar4 != 0) {
        return iVar3;
      }
    }
  }
  return 0;
}
