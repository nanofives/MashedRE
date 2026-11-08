// FLAGS: /O2 /Ob1 /G5
// MATCH-S27 0x005b6710: 87.5% (regs 87.5%). Residual: byte bVar1 zero-extended via a stack slot (mov [esp+8],cl) where the original does mov eax,ecx / and eax,0xff. Permuter (9 variants) and if/else spelling did not move it. Class: a.
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
typedef unsigned int uint;
typedef unsigned char byte;
undefined4 FUN_005b6760(uint * p0, short * p1, int p2);
undefined4 FUN_005b6910(uint * p0, short * p1, int p2);
// MATCH: 0x005b6710 FUN_005b6710
void FUN_005b6710(int param_1,uint *param_2,short *param_3)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = *(byte *)(param_1 + 0xd);
  uVar2 = *(uint *)(param_1 + 8) / ((uint)bVar1 * 0x24);
  if (bVar1 == 1) {
    FUN_005b6760(param_2,param_3,uVar2);
    return;
  }
  if (bVar1 == 2) {
    FUN_005b6910(param_2,param_3,uVar2);
  }
  return;
}
