// FLAGS: /O2 /Ob1 /G5
/* best 66.1% (regs 77.7%, ops 90.2%): permuter picked (ushort)>=1. Residual: original saves ebx,esi,edi,ebp with param_2 in ebx and a
   "test bp,bp; jbe" ushort-gt-0 loop entry; ours allocates ebx/ebp/esi/edi differently (112 vs 108 insns). Struct typing of the 0x18-byte
   entries not tried. Class (b). */
/* MATCH-S25 0x005aa200: hand-matched (h1). */
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

int *FUN_005adbe0(int *p0);
undefined4 FUN_005aea00(uint p0, undefined4 p1);
void FUN_004d7ff0(void);
// MATCH: 0x005aa200 FUN_005aa200
int FUN_005aa200(int param_1,int param_2,ushort param_3,int param_4,ushort param_5)
{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  ushort uVar4;
  if ((param_2 == 0) && (param_4 == 0)) {
    *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) & 0xfffffffe;
    uVar2 = FUN_005aea00(((uint)param_5 + (uint)param_3) * 0x18,0x30809);
    *(undefined4 *)(*(int *)(param_1 + 0x4c) + 0x10) = uVar2;
    iVar1 = *(int *)(*(int *)(param_1 + 0x4c) + 0x10);
    if (iVar1 == 0) {
      return 0;
    }
    *(uint *)(*(int *)(param_1 + 0x4c) + 0x14) = iVar1 + (uint)param_3 * 0x18;
    return param_1;
  }
  uVar4 = 0;
  if ((ushort)param_3 >= 1) {
    do {
      iVar1 = param_2 + (uint)uVar4 * 0x18;
      iVar3 = (int)FUN_005adbe0((int *)(iVar1 + 0xc));
      if (iVar3 == 0) {
        return 0;
      }
      if (*(int *)(iVar1 + 0x14) == 0) {
        *(void **)(iVar1 + 0x14) = (void *)FUN_004d7ff0;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < param_3);
  }
  *(int *)(*(int *)(param_1 + 0x4c) + 0x10) = param_2;
  uVar4 = 0;
  *(ushort *)(*(int *)(param_1 + 0x4c) + 0xc) = param_3;
  if (0 < (ushort)param_5) {
    do {
      iVar1 = param_4 + (uint)uVar4 * 0x18;
      iVar3 = (int)FUN_005adbe0((int *)(iVar1 + 0xc));
      if (iVar3 == 0) {
        return 0;
      }
      if (*(int *)(iVar1 + 0x14) == 0) {
        *(void **)(iVar1 + 0x14) = (void *)FUN_004d7ff0;
      }
      uVar4 = uVar4 + 1;
    } while (uVar4 < param_5);
  }
  *(uint *)(param_1 + 0xc) = *(uint *)(param_1 + 0xc) | 1;
  *(int *)(*(int *)(param_1 + 0x4c) + 0x14) = param_4;
  *(ushort *)(*(int *)(param_1 + 0x4c) + 0xe) = param_5;
  return param_1;
}
