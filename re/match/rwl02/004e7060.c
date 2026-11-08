// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL02 h3 0x004e7060: raw converter body (Ghidra structure, gotos kept), comments stripped, prototypes loosened for C++ (void-pointer or int params), F1/F2 typedefs for the engine vtable calls. Score 39.5% structure only (class b: source shape not found, size above budget). */
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
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned char byte;
MGLOBAL(int, DAT_007d71fc, 0x007d71fc);
MGLOBAL(undefined4, DAT_007d71d4, 0x007d71d4);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_007d7200, 0x007d7200);
MGLOBAL(undefined4, DAT_00618664, 0x00618664);
void FUN_004c0740(int p0, int p1);
void FUN_004c0790(int p0);
void FUN_004c0e50(int p0);
uint FUN_004cbd30(uint p0, void * p1, uint p2);
undefined4 FUN_004cc5e0(undefined4 p0, int p1, void * p2, uint * p3);
undefined4 FUN_004d7ff0(undefined4 p0);
int FUN_004d8060(void * p0, undefined4 p1);
void FUN_004d8480(void * p0);
undefined4 FUN_004d8bd0(int p0);
int FUN_004e1b60(void * p0, int p1, int p2);
uint FUN_004e1c90(void * p0, int p1, undefined4 p2, undefined4 p3);
undefined4 FUN_004e4440(int p0);
void FUN_004e8e90(int p0);
undefined4 FUN_004e8ea0(int p0);
int FUN_004e9330(uint p0);
int RpAtomicCreate(void);
// MATCH: 0x004e7060 FUN_004e7060
int FUN_004e7060(undefined4 param_1,int *param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  uint local_20;
  int local_1c [3];
  int local_10;
  int local_c;
  undefined4 local_8;
  undefined4 local_4;
  
  iVar2 = FUN_004cc5e0(param_1,1,local_1c,&local_20);
  if (iVar2 == 0) {
LAB_004e72fe:
    local_1c[1] = 2;
    local_1c[2] = FUN_004d7ff0(0x8000001a);
    FUN_004d8480(local_1c + 1);
    return 0;
  }
  if ((local_20 < 0x35000) || (0x37002 < local_20)) {
    uVar5 = 0x80000004;
  }
  else {
    local_10 = 0;
    local_c = 0;
    local_8 = 0;
    local_4 = 0;
    iVar2 = FUN_004cbd30(param_1,&local_10,local_1c[0]);
    if (local_1c[0] == iVar2) {
      iVar2 = RpAtomicCreate();
      if (iVar2 == 0) {
        return 0;
      }
      *(undefined1 *)(iVar2 + 2) = (undefined1)local_8;
      if (param_2[1] != 0) {
        FUN_004c0740(iVar2,*(undefined4 *)(*param_2 + local_10 * 4));
        *(byte *)(iVar2 + 3) = *(byte *)(iVar2 + 3) | 1;
      }
      if (param_3[1] == 0) {
        iVar4 = FUN_004cc5e0(param_1,0xf,0,&local_20);
        if (iVar4 == 0) {
LAB_004e71de:
          FUN_004d8060(&DAT_00618664,iVar2);
          if (*(int *)(iVar2 + 0x14) != 0) {
            FUN_004d8bd0(*(int *)(iVar2 + 0x14));
          }
          if (*(int *)(iVar2 + 0x18) != 0) {
            FUN_004e8ea0(*(int *)(iVar2 + 0x18));
            iVar4 = *(int *)(iVar2 + 4);
            *(undefined4 *)(iVar2 + 0x18) = 0;
            if ((iVar4 != 0) && (iVar3 = FUN_004e4440(iVar2), iVar3 != 0)) {
              FUN_004c0e50(iVar4);
            }
          }
          FUN_004c0790(iVar2);
          (*(F2 *)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7200 + DAT_007d3ff8),iVar2);
          local_1c[1] = 2;
          local_1c[2] = FUN_004d7ff0(0x8000001a);
          FUN_004d8480(local_1c + 1);
          return 0;
        }
        if ((local_20 < 0x35000) || (0x37002 < local_20)) {
          FUN_004d8060(&DAT_00618664,iVar2);
          if (*(int *)(iVar2 + 0x14) != 0) {
            FUN_004d8bd0(*(int *)(iVar2 + 0x14));
          }
          if (*(int *)(iVar2 + 0x18) != 0) {
            FUN_004e8ea0(*(int *)(iVar2 + 0x18));
            iVar4 = *(int *)(iVar2 + 4);
            *(undefined4 *)(iVar2 + 0x18) = 0;
            if ((iVar4 != 0) && (iVar3 = FUN_004e4440(iVar2), iVar3 != 0)) {
              FUN_004c0e50(iVar4);
            }
          }
          FUN_004c0790(iVar2);
          (*(F2 *)(DAT_007d3ff8 + 0x11c))(*(undefined4 *)(DAT_007d7200 + DAT_007d3ff8),iVar2);
          local_1c[1] = 2;
          local_1c[2] = FUN_004d7ff0(0x80000004);
          FUN_004d8480(local_1c + 1);
          return 0;
        }
        iVar4 = FUN_004e9330(param_1);
        if (iVar4 == 0) goto LAB_004e71de;
        if (iVar4 != *(int *)(iVar2 + 0x18)) {
          FUN_004e8e90(iVar4);
          if (*(int *)(iVar2 + 0x18) != 0) {
            FUN_004e8ea0(*(int *)(iVar2 + 0x18));
          }
          *(int *)(iVar2 + 0x18) = iVar4;
          iVar3 = *(int *)(iVar4 + 0x5c);
          iVar1 = *(int *)(iVar2 + 4);
          *(undefined4 *)(iVar2 + 0x1c) = *(undefined4 *)(iVar3 + 4);
          *(undefined4 *)(iVar2 + 0x20) = *(undefined4 *)(iVar3 + 8);
          *(undefined4 *)(iVar2 + 0x24) = *(undefined4 *)(iVar3 + 0xc);
          *(undefined4 *)(iVar2 + 0x28) = *(undefined4 *)(iVar3 + 0x10);
          if ((iVar1 != 0) && (iVar3 = FUN_004e4440(iVar2), iVar3 != 0)) {
            FUN_004c0e50(iVar1);
          }
        }
        FUN_004e8ea0(iVar4);
      }
      else {
        iVar4 = *(int *)(*param_3 + local_c * 4);
        if (iVar4 != *(int *)(iVar2 + 0x18)) {
          if (iVar4 != 0) {
            FUN_004e8e90(iVar4);
          }
          if (*(int *)(iVar2 + 0x18) != 0) {
            FUN_004e8ea0(*(int *)(iVar2 + 0x18));
          }
          *(int *)(iVar2 + 0x18) = iVar4;
          if (iVar4 != 0) {
            iVar4 = *(int *)(iVar4 + 0x5c);
            *(undefined4 *)(iVar2 + 0x1c) = *(undefined4 *)(iVar4 + 4);
            *(undefined4 *)(iVar2 + 0x20) = *(undefined4 *)(iVar4 + 8);
            *(undefined4 *)(iVar2 + 0x24) = *(undefined4 *)(iVar4 + 0xc);
            *(undefined4 *)(iVar2 + 0x28) = *(undefined4 *)(iVar4 + 0x10);
          }
          iVar4 = *(int *)(iVar2 + 4);
          if ((iVar4 != 0) && (iVar3 = FUN_004e4440(iVar2), iVar3 != 0)) {
            FUN_004c0e50(iVar4);
          }
        }
      }
      DAT_007d71fc = 0;
      DAT_007d71d4 = 0;
      iVar4 = FUN_004e1b60(&DAT_00618664,param_1,iVar2);
      if (iVar4 != 0) {
        if (DAT_007d71fc != 0) {
          FUN_004e1c90(&DAT_00618664,DAT_007d71fc,iVar2,DAT_007d71d4);
        }
        return iVar2;
      }
      goto LAB_004e72fe;
    }
    uVar5 = 0x8000001a;
  }
  local_1c[1] = 2;
  local_1c[2] = FUN_004d7ff0(uVar5);
  FUN_004d8480(local_1c + 1);
  return 0;
}
