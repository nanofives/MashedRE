// FLAGS: /O2 /Ob1 /G5
/* MATCH-S28 0x005c8910: 37.8% (regs 70.3%). Source structure matches the original control flow; register allocation differs (orig: eax=0 compared against both args, esi base). Tried pointer-typed base, zero-variable compares, permuter (140 compiles; its best swapped call order, rejected as unsafe). Class: b. */
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
void FUN_005b92e0(int p0, int p1);
void FUN_005b93a0(int p0, int p1);
// MATCH: 0x005c8910 FUN_005c8910
void FUN_005c8910(int param_1,int param_2,int param_3)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = *(int *)(param_1 + 0x20);
  uVar2 = 0;
  if (param_3 == 0) {
    if (param_2 != 0) {
      FUN_005b93a0(iVar1 + 0x34,*(undefined4 *)(iVar1 + 0x20c));
      FUN_005b92e0(iVar1 + 0x34,param_2);
      return;
    }
    uVar2 = *(undefined4 *)(iVar1 + 0xbc);
  }
  else {
    *(undefined4 *)(iVar1 + 0x210) = 0;
  }
  *(undefined4 *)(iVar1 + 0x20c) = uVar2;
  FUN_005b92e0(iVar1 + 0x34,param_2);
  return;
}
