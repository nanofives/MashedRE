// FLAGS: /O2 /Ob1 /G5
/* RESULT: pending */
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
typedef void (__cdecl *PFN_FREE)(int);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
void FUN_0055bad0(int p0);
void FUN_00564190(int p0);
// MATCH: 0x005668e0 FUN_005668e0
undefined4 FUN_005668e0(int param_1)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  
  iVar1 = *(int *)(param_1 + 0x40);
  uVar3 = *(uint *)(iVar1 + 0x70);
  *(uint *)(iVar1 + 0x70) = uVar3 - 1;
  if (uVar3 <= 1) {
    uVar3 = 0;
    if (*(uint *)(iVar1 + 0x10) > 0) {
      iVar2 = 0;
      do {
        FUN_0055bad0(*(int *)(iVar1 + 0xc) + iVar2);
        uVar3 = uVar3 + 1;
        iVar2 = iVar2 + 0x60;
      } while (uVar3 < *(uint *)(iVar1 + 0x10));
    }
    FUN_00564190(*(int *)(iVar1 + 0xc));
    if (*(int *)(iVar1 + 0x14) != 0) {
      (**(PFN_FREE *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar1 + 0x14));
      (**(PFN_FREE *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar1 + 0x18));
      (**(PFN_FREE *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar1 + 0x24));
      (**(PFN_FREE *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar1 + 0x20));
      (**(PFN_FREE *)(DAT_007d3ff8 + 0x10c))(*(int *)(iVar1 + 0x28));
    }
    (**(PFN_FREE *)(DAT_007d3ff8 + 0x10c))(iVar1);
    *(int *)(param_1 + 0x40) = 0;
  }
  return 1;
}
