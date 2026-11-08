// FLAGS: /O2 /Ob1 /G5
/* MATCH-S26 0x005aea10 (h3): 100.0% exact (regs 100.0%). FUN_005aea00 is uint(uint,uint) cdecl; unsigned >0 gives jbe; (x+4)/4*4 gives shr/shl. */
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
uint FUN_005aea00(uint a, uint b);
// MATCH: 0x005aea10 FUN_005aea10
uint FUN_005aea10(int param_1,undefined4 param_2)

{
  uint iVar1;
  uint uVar2;
  
  iVar1 = FUN_005aea00(param_1 + 4,param_2);
  if (iVar1 > 0) {
    uVar2 = (iVar1 + 4U) / 4 * 4;
    *(uint *)(uVar2 - 4) = iVar1;
    return uVar2;
  }
  return 0;
}
