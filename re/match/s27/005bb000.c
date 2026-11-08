// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 42.7% (regs 48.8%, permuter best (240 s). Not classified beyond structure/register differences (large function, budget spent). Class: b. */
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
typedef unsigned short ushort;
typedef unsigned char byte;
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
typedef void *HANDLE;
typedef int BOOL;
typedef unsigned long DWORD;
typedef void *LPVOID;
typedef long LONG;
typedef struct { unsigned char b[16]; } IID;
MGLOBAL(undefined1, DAT_005d098c, 0x005d098c);
MGLOBAL(undefined1, DAT_005e6424, 0x005e6424);
MGLOBAL(undefined1, FUN_005bb380, 0x005bb380);
__declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE p0);
#ifdef __cplusplus
void FUN_004522d0(...);
#else
void FUN_004522d0(void);
#endif
#ifdef __cplusplus
undefined4 FUN_005aea50(...);
#else
undefined4 FUN_005aea50(void);
#endif
uint FUN_005aeea0(undefined4 * p0, LONG p1, LONG p2);
void FUN_005aef00(undefined4 * p0, undefined4 p1, undefined4 p2, undefined4 p3);
undefined4 * FUN_005aef30(undefined4 * p0, undefined4 p1);
undefined4 FUN_005bbdb0(int * p0, int * p1, int p2, undefined4 p3, uint p4, undefined4 * p5);
void FUN_005bbf30(int * p0);
LPVOID * FUN_005bc470(IID * p0, undefined4 p1, LPVOID * p2);
void FUN_005bc640(int * p0);
__declspec(dllimport) DWORD __stdcall WaitForSingleObject(HANDLE p0, DWORD p1);
// MATCH: 0x005bb000 FUN_005bb000
void FUN_005bb000(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  byte bVar3;
  undefined4 *puVar4;
  uint uVar5;
  int iVar6;
  undefined4 uVar7;
  uint uVar8;
  int iVar9;
  undefined4 *puVar10;
  uint uStack_28;
  uint uStack_24;
  undefined4 *puStack_20;
  undefined4 uStack_1c;
  undefined *local_18;
  uint local_14;
  undefined1 local_10;
  undefined1 uStack_f;
  undefined4 local_c;
  undefined4 local_8;
  undefined1 local_4;
  undefined1 local_3;
  
  puVar4 = param_1;
  puVar1 = param_1 + 10;
  if (param_2 == (undefined4 *)0x0) {
    puVar1 = param_1 + 0x27;
    if (param_1[0x27] != 0) {
      param_1[0x1e] = param_1[0x1e] | 0x200;
      WaitForSingleObject((HANDLE)param_1[0x3d],0xffffffff);
      puVar4[0x1e] = puVar4[0x1e] & 0xfffffdff;
      if ((*(byte *)(puVar4 + 0x1e) & 8) != 0) {
        FUN_005bc640((int *)(puVar4 + 0x2a));
      }
      FUN_005bbf30((int *)puVar1);
      iVar6 = puVar4[0x43];
      *puVar1 = 0;
      puVar4[0x28] = 0;
      puVar4[0x29] = 0;
      if (iVar6 != 0) {
        FUN_004522d0(iVar6);
        puVar4[0x43] = 0;
      }
      CloseHandle((HANDLE)puVar4[0x3d]);
    }
    puVar4[0x38] = 0;
    puVar4[0x39] = 0;
    puVar4[0x3a] = 0;
    return;
  }
  if ((*(byte *)(param_1 + 0x1e) & 8) != 0) {
    uVar8 = param_2[1];
    uVar5 = ((uint)(*(ushort *)((int)param_1 + 0x2a) >> 3) * param_1[0xb] * 0x140) / 2000;
    if (uVar8 < uVar5) {
      iVar9 = ((uVar5 + uVar8) / uVar8) * uVar8;
      iVar6 = FUN_005aea50(1,iVar9,0x3080c);
      puVar4[0x43] = iVar6;
      if (iVar6 == 0) {
        return;
      }
      puVar4[0x46] = iVar9;
      puVar4[0x44] = 0;
      puVar4[0x45] = 0;
    }
  }
  local_10 = *(undefined1 *)((int)puVar4 + 0x2a);
  local_14 = puVar4[0x3b];
  uStack_1c = puVar4[0xb];
  puVar4[0x3b] = param_2[1] * 2;
  uVar7 = 0x88;
  puVar4[0x3c] = 0;
  local_4 = 0;
  local_3 = 0;
  local_c = 0;
  local_8 = 0;
  local_18 = &DAT_005e6424;
  uStack_f = 2;
  if ((puVar4[0x1e] & 0x400) != 0) {
    uVar7 = 0x8088;
  }
  puVar2 = puVar4 + 0x27;
  iVar6 = FUN_005bbdb0((int *)puVar4[0x1f],(int *)&uStack_1c,0,0,uVar7,puVar2);
  if (iVar6 == 0) goto LAB_005bb2d3;
  if ((*(byte *)(puVar4 + 0x1e) & 8) == 0) {
LAB_005bb1c6:
    iVar6 = (**(code **)(*(int *)*puVar2 + 0x2c))
                      ((int *)*puVar2,0,puVar4[0x3b],&puStack_20,&uStack_24,&param_1,&uStack_28,0);
    if (iVar6 < 0) {
LAB_005bb1f0:
      bVar3 = *(byte *)(puVar4 + 0x1e);
    }
    else {
      if ((puStack_20 != (undefined4 *)0x0) && (uStack_24 != 0)) {
        puVar10 = puStack_20;
        for (uVar8 = uStack_24 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
          puVar10 = puVar10 + 1;
          *puVar10 = 0;
        }
        for (uVar8 = uStack_24 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *(undefined1 *)puVar10 = 0;
          puVar10 = (undefined4 *)((int)puVar10 + 1);
        }
      }
      if ((param_1 != (undefined4 *)0x0) && (uStack_28 != 0)) {
        puVar10 = param_1;
        for (uVar8 = uStack_28 >> 2; uVar8 != 0; uVar8 = uVar8 - 1) {
          *puVar10 = 0;
          puVar10 = puVar10 + 1;
        }
        for (uVar8 = uStack_28 & 3; uVar8 != 0; uVar8 = uVar8 - 1) {
          *(undefined1 *)puVar10 = 0;
          puVar10 = (undefined4 *)((int)puVar10 + 1);
        }
      }
      iVar6 = (**(code **)(*(int *)*puVar2 + 0x4c))
                        ((int *)*puVar2,puStack_20,uStack_24,param_1,uStack_28);
      if (iVar6 < 0) goto LAB_005bb1f0;
      iVar6 = (**(code **)(*(int *)*puVar2 + 0x30))((int *)*puVar2,0,0,1);
      if (-1 < iVar6) {
        puVar4[0x38] = *param_2;
        iVar6 = FUN_005aeea0(puVar4 + 0x3d,0,1);
        puVar4[0x39] = param_2[1];
        puVar4[0x3a] = param_2[2];
        if (iVar6 != 0) {
          FUN_005aef00(puVar4 + 0x3e,FUN_005bb380,0xf,0x1000);
          iVar6 = (int)(FUN_005aef30(puVar4 + 0x3e,(undefined4)puVar1));
          if (iVar6 != 0) {
            return;
          }
          CloseHandle((HANDLE)puVar4[0x3d]);
          (**(code **)(*(int *)*puVar2 + 0x48))((int *)*puVar2);
          if ((*(byte *)(puVar4 + 0x1e) & 8) != 0) {
            FUN_005bc640((int *)(puVar4 + 0x2a));
          }
          FUN_005bbf30((int *)puVar2);
          if (puVar4[0x43] == 0) {
            return;
          }
          FUN_004522d0(puVar4[0x43]);
          puVar4[0x43] = 0;
          return;
        }
        (**(code **)(*(int *)*puVar2 + 0x48))((int *)*puVar2);
      }
      bVar3 = *(byte *)(puVar4 + 0x1e);
    }
    if ((bVar3 & 8) != 0) {
      FUN_005bc640((int *)(puVar4 + 0x2a));
    }
  }
  else {
    local_14 = local_14 >> 1;
    iVar6 = (int)(FUN_005bc470((IID *)&DAT_005d098c,(undefined4)&uStack_1c,(LPVOID *)(puVar4 + 0x2a)));
    if (iVar6 != 0) goto LAB_005bb1c6;
  }
  FUN_005bbf30((int *)puVar2);
LAB_005bb2d3:
  if (puVar4[0x43] == 0) {
    return;
  }
  FUN_004522d0(puVar4[0x43]);
  puVar4[0x43] = 0;
  return;
}
