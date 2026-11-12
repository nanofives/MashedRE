// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWC-NEAR (2026-11-12): 100%. The list search walks a pointer to the +0x9c link field (link = &cur->next, cur = *link) instead of the node pointer, so the load after `mov edx,eax` stays on edx as in the original (0x004c1076). */
/* MATCH-RWL01 0x004c1040: best 99.2% (regs 100%, ops 100%), class (a). Holds the static callee 0x004c0ad0 (100%, same helper as in 004c1210.c) so ecx/edx stay live across the call; uint flag byte for the xor/mov al pattern. Only the base register of one [+0x9c] load in the list-search loop differs (3 spellings tried). */
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
typedef unsigned int undefined4;
typedef unsigned char byte;
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
// MATCH: 0x004c0ad0 _FUN_004c0ad0
static void FUN_004c0ad0(int p0, int p1)
{
  int iVar1;
  iVar1 = *(int *)(p0 + 0x98);
  *(int *)(p0 + 0xa0) = p1;
  for (; iVar1 != 0; iVar1 = *(int *)(iVar1 + 0x9c)) {
    FUN_004c0ad0(iVar1,p1);
  }
}

// MATCH: 0x004c1040 FUN_004c1040

int FUN_004c1040(int param_1,int param_2)

{
  uint bVar1;
  int iVar2;
  int iVar3;
  int uVar4;
  
  iVar2 = *(int *)(param_2 + 4);
  if (iVar2 != 0) {
    iVar3 = *(int *)(iVar2 + 0x98);
    if (iVar3 == param_2) {
      *(undefined4 *)(iVar2 + 0x98) = *(undefined4 *)(param_2 + 0x9c);
    }
    else {
      {
        int *link = (int *)(iVar3 + 0x9c);
        while (*link != param_2) {
          iVar3 = *link;
          link = (int *)(iVar3 + 0x9c);
        }
      }
      *(undefined4 *)(iVar3 + 0x9c) = *(undefined4 *)(param_2 + 0x9c);
    }
    iVar2 = *(int *)(param_2 + 0x98);
    *(undefined4 *)(param_2 + 4) = 0;
    *(undefined4 *)(param_2 + 0x9c) = 0;
    *(int *)(param_2 + 0xa0) = param_2;
    for (; iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x9c)) {
      FUN_004c0ad0(iVar2,param_2);
    }
    bVar1 = (uint)*(byte *)(*(int *)(param_2 + 0xa0) + 3);
    if ((bVar1 & 3) == 0) {
      *(undefined4 *)(*(int *)(param_2 + 0xa0) + 8) = *(undefined4 *)(DAT_007d3ff8 + 0xbc);
      *(int *)(*(int *)(param_2 + 0xa0) + 0xc) = DAT_007d3ff8 + 0xbc;
      *(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(param_2 + 0xa0) + 8;
      *(int *)(DAT_007d3ff8 + 0xbc) = *(int *)(param_2 + 0xa0) + 8;
    }
    *(byte *)(*(int *)(param_2 + 0xa0) + 3) = (byte)(bVar1 | 3);
    *(byte *)(param_2 + 3) = *(byte *)(param_2 + 3) | 0xc;
  }
  *(undefined4 *)(param_2 + 0x9c) = *(undefined4 *)(param_1 + 0x98);
  *(int *)(param_1 + 0x98) = param_2;
  iVar2 = *(int *)(param_2 + 0x98);
  *(int *)(param_2 + 4) = param_1;
  uVar4 = *(undefined4 *)(param_1 + 0xa0);
  *(undefined4 *)(param_2 + 0xa0) = uVar4;
  for (; iVar2 != 0; iVar2 = *(int *)(iVar2 + 0x9c)) {
    FUN_004c0ad0(iVar2,uVar4);
  }
  if ((*(byte *)(param_2 + 3) & 3) != 0) {
    **(undefined4 **)(param_2 + 0xc) = *(undefined4 *)(param_2 + 8);
    *(undefined4 *)(*(int *)(param_2 + 8) + 4) = *(undefined4 *)(param_2 + 0xc);
    *(byte *)(param_2 + 3) = *(byte *)(param_2 + 3) & 0xfc;
  }
  bVar1 = (uint)*(byte *)(*(int *)(param_2 + 0xa0) + 3);
  if ((bVar1 & 3) == 0) {
    *(undefined4 *)(*(int *)(param_2 + 0xa0) + 8) = *(undefined4 *)(DAT_007d3ff8 + 0xbc);
    *(int *)(*(int *)(param_2 + 0xa0) + 0xc) = DAT_007d3ff8 + 0xbc;
    *(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(param_2 + 0xa0) + 8;
    *(int *)(DAT_007d3ff8 + 0xbc) = *(int *)(param_2 + 0xa0) + 8;
  }
  *(byte *)(*(int *)(param_2 + 0xa0) + 3) = (byte)(bVar1 | 3);
  *(byte *)(param_2 + 3) = *(byte *)(param_2 + 3) | 0xc;
  return param_1;
}
