// FLAGS: /O2 /Ob1 /G5
/* MATCH-S25 h3 0x005a9c50: by hand. Best 96.7% (regs 96.7%, 59/60 ins). Residual (b): original keeps one 'or ecx, eax' (eax = 0) before the [esi+0x44] word store, i.e. an unfolded OR with the zero just stored to [esi+0x40]; ours folds it away. Wrote the +0x40 update as read-or-store (shl 0x1c, matches shl instead of *0x10000000), tried a temp computed before the 0x44 store: no change. */
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
MGLOBAL(char, DAT_00617ff9, 0x00617ff9);
void FUN_005aa3d0(int p0, int p1);
void FUN_005ae0b0(int p0);
// MATCH: 0x005a9c50 FUN_005a9c50
void FUN_005a9c50(int param_1)
{
  undefined4 *puVar1;
  undefined4 *puVar2;
  int iVar3;
  if ((*(byte *)(param_1 + 0xc) & 4) != 0) {
    FUN_005aa3d0(param_1,1);
    FUN_005aa3d0(param_1,2);
  }
  FUN_005ae0b0(param_1);
  if ((*(byte *)(param_1 + 0xc) & 4) != 0) {
    puVar2 = *(undefined4 **)(param_1 + 0x4c);
    puVar1 = puVar2 + 1;
    *puVar2 = 0;
    puVar2[2] = (undefined4)puVar1;
    puVar2[4] = 0;
    *puVar1 = (undefined4)puVar1;
    puVar2[5] = 0;
    *(undefined2 *)(puVar2 + 3) = 0;
    *(undefined2 *)((int)puVar2 + 0xe) = 0;
  }
  if ((*(byte *)(param_1 + 0xc) & 8) != 0) {
    iVar3 = *(int *)(param_1 + 0x50);
    *(int *)iVar3 = iVar3;
    *(int *)(iVar3 + 4) = iVar3;
  }
  *(undefined4 *)(param_1 + 0x48) = 0;
  *(undefined4 *)(param_1 + 0x40) = 0;
  iVar3 = (int)DAT_00617ff9;
  *(undefined2 *)(param_1 + 0x46) = 8;
  *(undefined2 *)(param_1 + 0x44) = 0;
  *(uint *)(param_1 + 0x40) = *(uint *)(param_1 + 0x40) | (uint)(iVar3 - 1) << 0x1c;
  iVar3 = param_1 + 0x10;
  *(int *)(param_1 + 0x14) = iVar3;
  *(undefined4 *)(param_1 + 0x54) = 0;
  *(int *)iVar3 = iVar3;
  iVar3 = param_1 + 0x20;
  *(undefined4 *)(param_1 + 0x1c) = 0;
  *(undefined4 *)(param_1 + 0x18) = 0;
  *(int *)(param_1 + 0x24) = iVar3;
  *(undefined4 *)(param_1 + 0x28) = 0;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(undefined4 *)(param_1 + 0x30) = 0;
  *(undefined4 *)(param_1 + 0x34) = 0;
  *(undefined4 *)(param_1 + 0x38) = 0;
  *(undefined4 *)(param_1 + 0x3c) = 0;
  *(int *)iVar3 = iVar3;
}
