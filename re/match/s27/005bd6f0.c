// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005bd6f0: 30.7% (regs 39.4%), raw converter body kept. Residual (b): 221 vs 254 instr; no structural pass (budget).  C++ fixes: KR (variadic under C++) prototypes, int-typed pointer params, CARRY4 defined for C++ only. */
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
#ifdef __cplusplus
#define KR ...
#else
#define KR
#endif
typedef unsigned int undefined4;
typedef unsigned __int64 undefined8;
typedef unsigned int uint;
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
typedef int code(KR);
#ifndef __cplusplus
#define true 1
#endif
#ifdef __cplusplus
#define CARRY4(a,b) ((uint)(((uint)(a) + (uint)(b)) < (uint)(a)))
#endif
#define CONCAT44(a,b) ((ulonglong)(((ulonglong)(uint)(a)<<32)|(uint)(b)))
void FUN_005b0ec0(int p0);
undefined4 FUN_005b1080(int p0);
uint FUN_005bcda0(int p0);
void FUN_005bde50(int p0);
undefined4 FUN_005bde70(KR);
void FUN_005bdef0(int p0);
void FUN_005bdf40(int p0, int p1);
undefined4 FUN_005be540(KR);
undefined4 FUN_005be5f0(int p0, int p1);
undefined8 __stdcall __alldiv(KR);
undefined8 __stdcall __aulldiv(KR);
// MATCH: 0x005bd6f0 FUN_005bd6f0
/* WARNING: Removing unreachable block (ram,0x005bd880) */
/* [C2 2026-06-02] Streaming per-tick handler for the RwaDSRenderer (installed at filter `+0x30` by
   FUN_005bce80; called via pointer, no? */

int FUN_005bd6f0(int param_1)

{
  int iVar1;
  longlong lVar2;
  undefined4 uVar3;
  int iVar4;
  undefined8 uVar5;
  undefined4 local_60;
  undefined4 local_5c;
  undefined8 local_28;
  undefined8 local_20;
  uint local_18 [3];
  uint local_c;
  int local_8;
  
  iVar1 = *(int *)(param_1 + 0x20);
  iVar4 = *(int *)(iVar1 + 0x20);
  if (iVar4 == 1) {
    if (*(int *)(iVar1 + 0x28) == 8) {
      FUN_005b0ec0(param_1);
    }
  }
  else if ((1 < iVar4) && (iVar4 < 4)) {
    do {
      iVar4 = *(int *)(iVar1 + 0x780);
      if (*(int *)(iVar1 + 0x780) == 0) {
        local_8 = FUN_005b1080(iVar1 + 0x10);
        FUN_005bde70(iVar1 + 0x774,local_8,2,*(undefined4 *)(iVar1 + 0x6fc));
        iVar4 = local_8;
      }
      local_8 = iVar4;
      iVar4 = *(int *)(iVar1 + 0x76c);
      if (iVar4 == 0) {
        uVar3 = FUN_005be540(*(undefined4 *)(iVar1 + 0x3b4),1,*(undefined4 *)(iVar1 + 0x6fc));
        FUN_005bde70(iVar1 + 0x760,uVar3);
        iVar4 = *(int *)(iVar1 + 0x76c);
        if (iVar4 != 0) goto LAB_005bd790;
      }
      else {
LAB_005bd790:
        if (*(int *)(iVar1 + 0x764) == *(int *)(iVar1 + 0x768)) {
          iVar4 = FUN_005be5f0(*(undefined4 *)(iVar1 + 0x3b4),iVar4);
          if (iVar4 != 0) {
            FUN_005bde50(iVar1 + 0x760);
          }
        }
        else if (*(int *)(iVar1 + 0x780) != 0) {
          if (*(int *)(iVar1 + 0x764) == 0) {
            iVar4 = __aulldiv((ulonglong)*(uint *)(iVar1 + 0x778) *
                              (ulonglong)*(uint *)(iVar1 + 0x758),*(undefined4 *)(iVar1 + 0x6fc),0);
            local_18[0] = iVar4 + *(int *)(*(int *)(*(int *)(iVar1 + 0x780) + 0xc) + 8);
            local_18[2] = *(uint *)(iVar1 + 0x75c) + local_18[0];
            local_c = (uint)CARRY4(*(uint *)(iVar1 + 0x75c),local_18[0]);
            local_18[1] = 0;
            uVar5 = __alldiv((ulonglong)local_18[0] * 10000000,*(undefined4 *)(iVar1 + 0x710),0);
            if (local_c == 0) {
              lVar2 = (ulonglong)local_18[2] * 10000000;
            }
            else {
              lVar2 = CONCAT44((int)((ulonglong)local_18[2] * 10000000 >> 0x20) + local_c * 10000000
                               ,(int)((ulonglong)local_18[2] * 10000000));
            }
            local_5c = (undefined4)((ulonglong)lVar2 >> 0x20);
            local_60 = (undefined4)lVar2;
            local_28 = uVar5;
            uVar5 = __alldiv(local_60,local_5c,*(undefined4 *)(iVar1 + 0x710),0);
            local_20 = uVar5;
            iVar4 = (**(code **)(**(int **)(iVar1 + 0x76c) + 0x48))
                              (*(int **)(iVar1 + 0x76c),local_18,local_18 + 2);
            if (-1 < iVar4) {
              (**(code **)(**(int **)(iVar1 + 0x76c) + 0x18))
                        (*(int **)(iVar1 + 0x76c),&local_28,&local_20);
            }
          }
          FUN_005bdf40(iVar1 + 0x760,iVar1 + 0x774);
          if ((*(int *)(iVar1 + 0x780) != 0) && (*(int *)(iVar1 + 0x778) == *(int *)(iVar1 + 0x77c))
             ) {
            FUN_005bdef0(iVar1 + 0x774);
          }
        }
      }
      if (((*(int *)(iVar1 + 0x20) == 3) && (*(int *)(iVar1 + 0x28) == 4)) ||
         ((*(int *)(iVar1 + 0x20) == 2 && (*(int *)(iVar1 + 0x28) != 6)))) {
        iVar4 = FUN_005bcda0(iVar1 + 0x6dc);
        if (iVar4 != 0) {
          FUN_005b0ec0(param_1);
        }
      }
      if (local_8 == 0) {
        return param_1;
      }
      if (*(int *)(iVar1 + 0x76c) == 0) {
        return param_1;
      }
    } while( true );
  }
  return param_1;
}
