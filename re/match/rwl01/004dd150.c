// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL01 0x004dd150: 100% (regs 100%). D3D texture/cube size sum written from the listing with real COM slot prototypes (levels 0x34, desc 0x44, lock 0x4c, unlock 0x50, cube variants take the face argument). The flag byte is read as (byte << 8) so the compiler uses ch; cube branch first, Height < 4 form. */
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
typedef int code();
MGLOBAL(int, DAT_00911ae4, 0x00911ae4);
typedef struct LR9 { int Pitch; void *pBits; } LR9;
typedef struct SD9 { uint Format, Type, Usage, Pool, MSType, MSQuality, Width, Height; } SD9;
typedef int (__stdcall *PFN_COUNT)(int *);
typedef int (__stdcall *PFN_DESC)(int *, uint, SD9 *);
typedef int (__stdcall *PFN_LOCK)(int *, uint, LR9 *, uint, uint);
typedef int (__stdcall *PFN_UNLOCK)(int *, uint);
typedef int (__stdcall *PFN_LOCKC)(int *, uint, uint, LR9 *, uint, uint);
typedef int (__stdcall *PFN_UNLOCKC)(int *, uint, uint);
// MATCH: 0x004dd150 FUN_004dd150
undefined4 FUN_004dd150(int *param_1,int *param_2)
{
  int *puVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  LR9 lr;
  SD9 sd;
  iVar4 = *param_2;
  if (iVar4 == 0) {
    *param_1 = 0x54;
    return 1;
  }
  uVar6 = (uint)*(byte *)(iVar4 + 0x23) << 8;
  param_2 = (int *)0x64;
  if ((uVar6 & 0x4000) != 0) {
    param_2 = (int *)0xe4;
  }
  else if ((uVar6 & 0x2000) != 0) {
    param_2 = (int *)0x464;
  }
  puVar1 = (int *)(DAT_00911ae4 + iVar4);
  if ((*(byte *)(DAT_00911ae4 + 9 + iVar4) & 0xf) != 0) {
    piVar2 = (int *)*puVar1;
    iVar4 = 0;
    if ((*(byte *)((int)puVar1 + 10) & 0xf) != 0) {
      iVar5 = 1;
    }
    else {
      iVar5 = (*(PFN_COUNT *)(*piVar2 + 0x34))(piVar2);
      if (iVar5 == 0) goto LAB_cube_done;
    }
    do {
      iVar5 = iVar5 + -1;
      (*(PFN_DESC *)(*piVar2 + 0x44))(piVar2,iVar5,&sd);
      (*(PFN_LOCKC *)(*piVar2 + 0x4c))(piVar2,0,iVar5,&lr,0,0x8010);
      (*(PFN_UNLOCKC *)(*piVar2 + 0x50))(piVar2,0,iVar5);
      if ((*(byte *)((int)puVar1 + 10) & 0xf0) != 0) {
        if (sd.Height < 4) {
          iVar3 = lr.Pitch;
        }
        else {
          iVar3 = (sd.Height >> 2) * lr.Pitch;
        }
      }
      else {
        iVar3 = lr.Pitch * sd.Height;
      }
      iVar4 = iVar4 + 4 + iVar3;
    } while (iVar5 != 0);
LAB_cube_done:
    param_2 = (int *)((int)param_2 + iVar4 * 6);
  }
  else {
    if ((*(byte *)((int)puVar1 + 10) & 0xf) != 0) {
      iVar4 = 1;
    }
    else {
      iVar4 = (*(PFN_COUNT *)(*(int *)*puVar1 + 0x34))((int *)*puVar1);
      if (iVar4 == 0) goto LAB_004dd2bd;
    }
    do {
      iVar4 = iVar4 + -1;
      (*(PFN_DESC *)(*(int *)*puVar1 + 0x44))((int *)*puVar1,iVar4,&sd);
      (*(PFN_LOCK *)(*(int *)*puVar1 + 0x4c))((int *)*puVar1,iVar4,&lr,0,0x8010);
      (*(PFN_UNLOCK *)(*(int *)*puVar1 + 0x50))((int *)*puVar1,iVar4);
      if ((*(byte *)((int)puVar1 + 10) & 0xf0) != 0) {
        if (sd.Height < 4) {
          iVar5 = lr.Pitch;
        }
        else {
          iVar5 = (sd.Height >> 2) * lr.Pitch;
        }
      }
      else {
        iVar5 = lr.Pitch * sd.Height;
      }
      param_2 = (int *)((int)param_2 + 4 + iVar5);
    } while (iVar4 != 0);
  }
LAB_004dd2bd:
  *param_1 = (int)param_2;
  return 1;
}
