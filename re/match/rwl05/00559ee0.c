// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWL05 0x00559ee0: hand-matched (h1). 43.9% (regs 65.3%, ops 77.6%), 84/98 ins. Slot release: bit index kept as unsigned local loaded via xor/mov (matches), vtable +0x14 call typed. Original keeps the 1<<n mask in ebp and rebuilds it for the second test; ours spills/CSEs differently (register pressure). Class (b). */
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
typedef int (__cdecl *fnv)(int *);
void FUN_00565260(int p0, unsigned int p1);
// MATCH: 0x00559ee0 FUN_00559ee0
int * FUN_00559ee0(int *param_1,int *param_2)
{
  unsigned int *puVar1;
  unsigned int uVar2;
  int iVar3;
  unsigned int uVar4;
  unsigned int uVar5;
  int iVar6;

  uVar2 = *(unsigned short *)(param_2 + 8);
  uVar5 = 1 << (uVar2 & 0x1f);
  iVar6 = (uVar2 >> 5) * 4;
  if ((((*(unsigned int *)(iVar6 + param_1[0x18]) & uVar5) != 0) && (iVar3 = param_2[9], iVar3 != 0)) &&
     ((*(unsigned int *)(*(int *)(iVar3 + 0x60) + (uVar2 >> 5) * 4) & (1 << (uVar2 & 0x1f))) != 0)) {
    (*(fnv *)(*param_2 + 0x14))(param_2);
    puVar1 = (unsigned int *)(*(int *)(iVar3 + 0x60) + (unsigned int)(*(unsigned short *)(param_2 + 8) >> 5) * 4);
    *puVar1 = *puVar1 & ~(1 << ((unsigned int)*(unsigned short *)(param_2 + 8) & 0x1f));
    *(int *)(iVar3 + 0x4c) = *(int *)(iVar3 + 0x4c) - 1;
  }
  uVar4 = ~uVar5;
  *(unsigned int *)(param_1[0x16] + iVar6) = *(unsigned int *)(param_1[0x16] + iVar6) & uVar4;
  *(unsigned int *)(param_1[0x17] + iVar6) = *(unsigned int *)(param_1[0x17] + iVar6) & uVar4;
  *(unsigned int *)(param_1[0x19] + iVar6) = *(unsigned int *)(param_1[0x19] + iVar6) & uVar4;
  if ((*(unsigned int *)(iVar6 + param_1[0x1a]) & uVar5) != 0) {
    FUN_00565260(*param_1,uVar2);
    *(unsigned int *)(iVar6 + param_1[0x1a]) = *(unsigned int *)(iVar6 + param_1[0x1a]) & uVar4;
  }
  *(int *)(param_1[0x14] + (unsigned int)uVar2 * 4) = 0;
  *(unsigned short *)(param_2 + 8) = 0xffff;
  param_2[9] = 0;
  return param_1;
}
