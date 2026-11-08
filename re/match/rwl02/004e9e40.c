// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h1 0x004e9e40: best 34.4% (regs 58.1%), residual class b. structure only (980 B), raw converter form with C++ casts; 34.4. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
MGLOBAL(undefined4, DAT_006189e4, 0x006189e4);
MGLOBAL(int, DAT_007d727c, 0x007d727c);
MGLOBAL(undefined4, DAT_007d7270, 0x007d7270);
uint FUN_004cbd30(uint p0, void * p1, uint p2);
undefined4 FUN_004cc5e0(uint p0, int p1, void * p2, void * p3);
undefined4 FUN_004cc790(undefined4 p0, undefined4 p1, undefined4 p2);
undefined4 FUN_004d7ff0(undefined4 p0, ...);
int FUN_004d8000(void * p0, void * p1);
undefined4 * FUN_004d8480(void * p0);
int FUN_004e1b60(void * p0, int p1, void * p2);
uint FUN_004e1c90(void * p0, int p1, void * p2, undefined4 p3);
void RwFrameDestroy();
void RwFrameDestroyHierarchy(int * p0);
// MATCH: 0x004e9e40 FUN_004e9e40
undefined4 * FUN_004e9e40(undefined4 param_1,int *param_2,int param_3,uint param_4)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int *piVar6;
  undefined4 *puVar7;
  uint local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined2 local_2c [2];
  int local_28;
  int local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  iVar4 = FUN_004cc5e0(param_1,1,0,&local_38);
  if (iVar4 == 0) {
    return (undefined4 *)0x0;
  }
  if ((local_38 < 0x35000) || (0x37002 < local_38)) {
    local_34 = 2;
    local_30 = FUN_004d7ff0(0x80000004);
    FUN_004d8480(&local_34);
    return (undefined4 *)0x0;
  }
  iVar4 = FUN_004cbd30(param_1,local_2c,0x2c);
  if (iVar4 != 0x2c) {
    return (undefined4 *)0x0;
  }
  puVar1 = (undefined4 *)*param_2;
  *param_2 = (int)puVar1 + DAT_006189e4;
  if (puVar1 == (undefined4 *)0x0) {
    local_34 = 2;
    local_30 = FUN_004d7ff0(0x80000013,DAT_006189e4);
    FUN_004d8480(&local_34);
    return (undefined4 *)0x0;
  }
  *puVar1 = 0xffffffff;
  puVar1[0x1b] = local_20;
  puVar1[0x1c] = local_1c;
  puVar1[0x1d] = local_18;
  puVar1[0x18] = local_14;
  puVar1[0x19] = local_10;
  puVar1[0x1a] = local_c;
  *(undefined2 *)(puVar1 + 0x20) = local_2c[0];
  *(undefined2 *)(puVar1 + 0x21) = (undefined2)local_28;
  *(undefined2 *)((int)puVar1 + 0x82) = (undefined2)local_24;
  puVar1[1] = 0;
  puVar1[2] = 0;
  puVar1[0xc] = 0;
  puVar1[3] = 0;
  puVar7 = puVar1 + 4;
  for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar7 = 0;
    puVar7 = puVar7 + 1;
  }
  puVar7 = puVar1 + 0xe;
  puVar1[0xf] = (undefined4)puVar7;
  puVar1[0x1f] = 0;
  *puVar7 = (undefined4)puVar7;
  puVar7 = puVar1 + 0x10;
  puVar1[0xd] = 0;
  puVar1[0x1e] = 0;
  *puVar7 = (undefined4)puVar7;
  puVar1[0x11] = (undefined4)puVar7;
  if ((*(uint *)(param_3 + 8) & 0x1000000) == 0) {
    if (local_24 != 0) {
      iVar4 = *param_2;
      *param_2 = iVar4 + local_24 * 0xc;
      puVar1[2] = iVar4;
      if (iVar4 == 0) {
LAB_004ea0ee:
        local_34 = 2;
        local_30 = FUN_004d7ff0(0x80000013);
      }
      else {
        iVar4 = FUN_004cc790(param_1,iVar4,local_24 * 0xc);
        if (iVar4 == 0) goto LAB_004e9ffd;
        if ((param_4 & 0x10) == 0) {
LAB_004ea030:
          if ((param_4 & 8) != 0) {
            iVar4 = *param_2;
            *param_2 = iVar4 + local_24 * 4;
            puVar1[0xc] = iVar4;
            if (iVar4 == 0) goto LAB_004ea0ee;
            iVar4 = FUN_004cbd30(param_1,(void *)iVar4,local_24 * 4);
            if (iVar4 == 0) goto LAB_004e9ffd;
          }
          if (0 < *(int *)(param_3 + 0x20)) {
            param_4 = 0;
            iVar4 = (uint)*(ushort *)((int)puVar1 + 0x82) * 8;
            if (0 < *(int *)(param_3 + 0x20)) {
              piVar6 = (int *)(puVar1 + 4);
              do {
                iVar5 = *param_2;
                *param_2 = iVar5 + iVar4;
                *piVar6 = iVar5;
                if (iVar5 == 0) goto LAB_004e9fda;
                iVar5 = FUN_004cc790(param_1,iVar5,iVar4);
                if (iVar5 == 0) goto LAB_004e9ffd;
                param_4 = param_4 + 1;
                piVar6 = piVar6 + 1;
              } while ((int)param_4 < *(int *)(param_3 + 0x20));
            }
          }
          goto LAB_004ea0ca;
        }
        iVar5 = *param_2;
        iVar4 = local_24 * 4;
        *param_2 = iVar5 + iVar4;
        puVar1[3] = iVar5;
        if (iVar5 != 0) {
          iVar5 = FUN_004cbd30(param_1,(void *)iVar5,iVar4);
          if (iVar5 != iVar4) goto LAB_004e9ffd;
          goto LAB_004ea030;
        }
LAB_004e9fda:
        local_34 = 2;
        local_30 = FUN_004d7ff0(0x80000013);
      }
      FUN_004d8480(&local_34);
      goto LAB_004e9ffd;
    }
LAB_004ea0ca:
    if (local_28 != 0) {
      iVar4 = *param_2;
      *param_2 = iVar4 + local_28 * 8;
      puVar1[1] = iVar4;
      if (iVar4 == 0) goto LAB_004ea0ee;
      iVar4 = FUN_004cc790(param_1,iVar4,local_28 * 8);
      if (iVar4 == 0) goto LAB_004e9ffd;
      if ((local_38 < 0x36002) && (iVar4 = 0, 0 < local_28)) {
        do {
          uVar2 = *(undefined4 *)(puVar1[1] + 4 + iVar4 * 8);
          puVar7 = (undefined4 *)(puVar1[1] + iVar4 * 8);
          iVar4 = iVar4 + 1;
          uVar3 = *puVar7;
          (*(ushort *)((char *)&local_34 + 2)) = (undefined2)((uint)uVar3 >> 0x10);
          *(undefined2 *)puVar7 = (*(ushort *)((char *)&local_34 + 2));
          *(short *)(puVar1[1] + -6 + iVar4 * 8) = (short)uVar2;
          (*(ushort *)((char *)&local_30 + 2)) = (undefined2)((uint)uVar2 >> 0x10);
          *(undefined2 *)(puVar1[1] + -4 + iVar4 * 8) = (*(ushort *)((char *)&local_30 + 2));
          *(short *)(puVar1[1] + -2 + iVar4 * 8) = (short)uVar3;
          local_34 = uVar3;
          local_30 = uVar2;
        } while (iVar4 < local_28);
      }
    }
  }
  FUN_004d8000(&DAT_006189e4,puVar1);
  DAT_007d727c = 0;
  DAT_007d7270 = 0;
  iVar4 = FUN_004e1b60(&DAT_006189e4,param_1,puVar1);
  if (iVar4 != 0) {
    if (DAT_007d727c != 0) {
      FUN_004e1c90(&DAT_006189e4,DAT_007d727c,puVar1,DAT_007d7270);
    }
    return puVar1;
  }
LAB_004e9ffd:
  if ((*(byte *)(param_3 + 3) & 1) != 0) {
    RwFrameDestroy();
    return (undefined4 *)0x0;
  }
  RwFrameDestroyHierarchy((int *)puVar1);
  return (undefined4 *)0x0;
}
