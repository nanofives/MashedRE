// FLAGS: /O2 /Ob1 /G5
/* MATCH-S27 h1: 91.9% (regs 91.9%, permuter best (150 s). Residual not classified beyond register/schedule differences. Class: b. */
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
typedef unsigned short undefined2;
typedef unsigned int undefined4;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;
MGLOBAL(undefined1, DAT_005e6424, 0x005e6424);
MGLOBAL(undefined1, DAT_005e6464, 0x005e6464);
MGLOBAL(undefined1, DAT_005e6474, 0x005e6474);
MGLOBAL(undefined1, DAT_007ddfec, 0x007ddfec);
uint FUN_005ac650(uint * p0);
int FUN_005adf30(byte * p0, byte * p1);
int FUN_005ae3a0(int p0, undefined4 p1);
void FUN_005bcab0(short * p0, undefined4 * p1);
// MATCH: 0x005bc920 FUN_005bc920
undefined4 FUN_005bc920(undefined4 param_1,int *param_2)

{
  uint *puVar1;
  uint uVar2;
  undefined2 uVar3;
  int iVar4;
  undefined2 *puVar5;
  
  iVar4 = FUN_005adf30((byte *)param_2[1],&DAT_005e6424);
  if (iVar4 == 0) {
    puVar5 = (undefined2 *)FUN_005ae3a0((int)&DAT_007ddfec,0x12);
    if (puVar5 != (undefined2 *)0x0) {
      puVar5[8] = 0;
      *(uint *)(puVar5 + 4) =
           (uint)(*(byte *)(param_2 + 3) >> 3) * (uint)*(byte *)((int)param_2 + 0xd) * *param_2;
      puVar5[6] = (ushort)(*(byte *)(param_2 + 3) >> 3) * (ushort)*(byte *)((int)param_2 + 0xd);
      puVar5[1] = (ushort)*(byte *)((int)param_2 + 0xd);
      *(int *)(puVar5 + 2) = *param_2;
      puVar5[7] = (ushort)*(byte *)(param_2 + 3);
      *puVar5 = 1;
LAB_005bca84:
      FUN_005bcab0((short *)puVar5,(undefined4 *)param_1);
      return 1;
    }
  }
  else {
    iVar4 = FUN_005adf30((byte *)param_2[1],&DAT_005e6464);
    if (iVar4 == 0) {
      puVar5 = (undefined2 *)FUN_005ae3a0((int)&DAT_007ddfec,0x12);
      if (puVar5 != (undefined2 *)0x0) {
        puVar5[8] = 0;
        *(undefined4 *)(puVar5 + 4) = 0;
        puVar5[6] = 0;
        puVar5[1] = (ushort)*(byte *)((int)param_2 + 0xd);
        *(int *)(puVar5 + 2) = *param_2;
        puVar5[7] = (ushort)*(byte *)(param_2 + 3);
        goto LAB_005bca84;
      }
    }
    else {
      iVar4 = FUN_005adf30((byte *)param_2[1],&DAT_005e6474);
      if (iVar4 == 0) {
        puVar1 = (uint *)param_2[4];
        puVar5 = (undefined2 *)FUN_005ae3a0((int)&DAT_007ddfec,0x1e);
        if (puVar5 != (undefined2 *)0x0) {
          uVar3 = FUN_005ac650((uint *)param_2);
          puVar5[8] = 0xc;
          *puVar5 = 0x55;
          puVar5[1] = (ushort)*(byte *)((int)param_2 + 0xd);
          *(int *)(puVar5 + 2) = *param_2;
          uVar2 = *puVar1;
          *(uint *)(puVar5 + 4) = uVar2 >> 3;
          puVar5[6] = uVar3;
          puVar5[7] = (ushort)*(byte *)(param_2 + 3);
          puVar5[9] = 1;
          *(undefined4 *)(puVar5 + 10) = 0;
          puVar5[0xc] = uVar3;
          puVar5[0xd] = 1;
          puVar5[0xe] = 0;
          goto LAB_005bca84;
        }
      }
    }
  }
  return 0;
}
