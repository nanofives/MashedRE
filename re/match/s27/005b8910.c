// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 0x005b8910: RO-AUDIOINIT-45D460-REACH-5, see below. */
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
/* RO-AUDIOINIT-45D460-REACH-5 (2026-11-27), from RO-AUDIOINIT-45D460-REACH-4's draft (2026-11-22): new source from the Ghidra decompilation of 0x005b8910 (715 B, pc_funcs.csv;
   no hooks.csv row or re/match source before this row). 0x005b8680 stores its address at +0x28
   (re/match/s27/005b8680.c line 52); reached from 0x005aa560 (PortTop_Bound_005aa560+0x18a). */
int FUN_005c6bb0(undefined4 param_1, undefined4 param_2, undefined4 param_3, undefined4 param_4);
void FUN_005b9460(int param_1, int param_2);
int FUN_005aeb90(float param_1);
void FUN_005b8c40(int param_1);

// MATCH: 0x005b8910 FUN_005b8910
int FUN_005b8910(int param_1, int param_2, uint *param_3, undefined4 param_4, undefined4 param_5)
{
  int iVar2;
  int iVar3;
  undefined4 auStack_30[12];

  *(undefined4 *)(param_1 + 0xdc) = 0;
  *(uint *)(param_1 + 0x28) = *param_3;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0xe4) = 0;
  *(int *)(param_1 + 0x94) = param_2 + 0x28;
  if ((*(byte *)(param_2 + 0x78) & 8) != 0) {
    iVar2 = FUN_005c6bb0(*(undefined4 *)(param_2 + 0x11c), 4, param_4, param_5);
    *(int *)(param_1 + 0x11c) = iVar2;
    if (iVar2 == 0) {
      return 0;
    }
  }
  *(undefined4 *)(param_1 + 0x88) = 0;
  *(undefined4 *)(param_1 + 0x8c) = 0;
  *(undefined4 *)(param_1 + 0x90) = 0;
  *(undefined4 *)(param_1 + 0x84) = 0;
  FUN_005b9460(param_1, 0);
  *(undefined4 *)(param_1 + 0x2c) = 0x473b8000;
  *(undefined4 *)(param_1 + 0x30) = 0x3f800000;
  *(undefined4 *)(param_1 + 0x34) = 0;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 0xe0;
  *(undefined4 *)(param_1 + 0x9c) = 0x40;
  *(uint *)(param_1 + 0xd8) = *param_3 >> 1 & 1;
  *(undefined4 *)(param_1 + 0xb8) = 0x168;
  *(undefined4 *)(param_1 + 0xbc) = 0x168;
  *(undefined4 *)(param_1 + 0xa0) = 0;
  *(undefined4 *)(param_1 + 0xa4) = 0;
  *(undefined4 *)(param_1 + 0xa8) = 0;
  *(undefined4 *)(param_1 + 0xac) = 0;
  *(undefined4 *)(param_1 + 0xb0) = 0;
  *(undefined4 *)(param_1 + 0xb4) = 0;
  *(undefined4 *)(param_1 + 0xc0) = 0;
  *(undefined4 *)(param_1 + 0xc4) = 0;
  *(undefined4 *)(param_1 + 200) = 0x3f800000;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
  *(int *)(param_1 + 0xcc) = FUN_005aeb90(1.0f);
  *(undefined4 *)(param_1 + 0x98) = 0x3f800000;
  *(undefined4 *)(param_1 + 0xd0) = 0x461c3c00;
  *(undefined4 *)(param_1 + 0xd4) = 0x461c4000;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
  if ((*(byte *)(*(int *)(param_1 + 0x94) + 0x50) & 7) != 0) {
    auStack_30[0] = 0;
    auStack_30[1] = 0;
    auStack_30[2] = 0x41200000;
    auStack_30[3] = 0x3fbeb852;
    auStack_30[4] = 0x3f547ae1;
    auStack_30[5] = 0xfffff5d6;
    auStack_30[6] = 0x3be56042;
    auStack_30[7] = 200;
    auStack_30[8] = 0x3c343958;
    auStack_30[9] = 0x42c80000;
    auStack_30[10] = 0x42c80000;
    auStack_30[11] = 0x459c4000;
    for (iVar3 = 0; iVar3 < 12; iVar3++) {
      ((undefined4 *)(param_1 + 0x40))[iVar3] = auStack_30[iVar3];
    }
    *(undefined4 *)(param_1 + 0x70) = 0;
    *(undefined4 *)(param_1 + 0x74) = 0;
    *(undefined4 *)(param_1 + 0x78) = 0x3e800000;
    *(undefined4 *)(param_1 + 0x7c) = 0;
    *(undefined4 *)(param_1 + 0x80) = 0x3e800000;
    *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 0x10;
  }
  *(undefined4 *)(param_1 + 0x108) = 0x3f800000;
  *(undefined4 *)(param_1 + 0x110) = 0x3f800000;
  *(undefined4 *)(param_1 + 0xe8) = 0;
  *(undefined4 *)(param_1 + 0xec) = 0;
  *(undefined4 *)(param_1 + 0xf0) = 0;
  *(undefined4 *)(param_1 + 0xf4) = 0;
  *(undefined4 *)(param_1 + 0xf8) = 0;
  *(undefined4 *)(param_1 + 0xfc) = 0;
  *(undefined4 *)(param_1 + 0x100) = 0;
  *(undefined4 *)(param_1 + 0x104) = 0;
  *(undefined4 *)(param_1 + 0x10c) = 0;
  *(undefined4 *)(param_1 + 0x114) = 0;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 8;
  *(undefined4 *)(param_1 + 0x118) = 0;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 8;
  if ((*param_3 & 2) != 0) {
    iVar2 = *(int *)(param_1 + 0x94);
    if ((*(byte *)(iVar2 + 0x50) & 0x40) != 0) {
      *(int **)(param_1 + 0x124) = (int *)(iVar2 + 0x6c);
      *(undefined4 *)(param_1 + 0x120) = *(undefined4 *)(iVar2 + 0x6c);
      *(int *)(*(int *)(iVar2 + 0x6c) + 4) = param_1 + 0x120;
      *(int *)(*(int *)(param_1 + 0x94) + 0x6c) = param_1 + 0x120;
      FUN_005b8c40(param_1);
      return param_1;
    }
    *(undefined4 *)(param_1 + 0x124) = 0;
    *(undefined4 *)(param_1 + 0x120) = 0;
  }
  FUN_005b8c40(param_1);
  return param_1;
}
