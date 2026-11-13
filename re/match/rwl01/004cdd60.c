// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-RWT-S2 0x004cdd60: 100% under rw9044 (also 100% under ddk). Was 79.2% (MATCH-RWL01). Lever: the flag is a boolean expression (d==4||d==8) and both the palette size and the +0x18 store are ternaries on it; an if/else gives 90.9% (then-block inline), zero-init + if gives 85.7%. */
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
#ifdef __cplusplus
typedef int code(...);
#else
typedef int code();
#endif
#ifndef __cplusplus
#define true 1
#endif
#ifndef __cplusplus
#define false 0
#endif
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
undefined4 FUN_004d7ff0(undefined4 p0, undefined4 p1);
undefined4 * FUN_004d8480(undefined4 * p0);

// MATCH: 0x004cdd60 FUN_004cdd60
uint * FUN_004cdd60(uint *param_1)
{
  int bVar1;
  int iVar3;
  uint uVar2;
  int iVar4;
  undefined4 loc[2];
  bVar1 = (param_1[3] == 4) || (param_1[3] == 8);
  iVar3 = bVar1 ? (1 << param_1[3]) << 2 : 0;
  uVar2 = ((int)(param_1[3] + 7) >> 3) * param_1[1] + 3 & 0xfffffffc;
  iVar4 = param_1[2] * uVar2;
  param_1[4] = uVar2;
  uVar2 = (**(code **)(DAT_007d3ff8 + 0x108))(iVar4 + iVar3,0x30018);
  param_1[5] = uVar2;
  if (uVar2 == 0) {
    loc[0] = 1;
    loc[1] = FUN_004d7ff0(0x80000013,iVar4 + iVar3);
    FUN_004d8480(loc);
    return (uint *)0x0;
  }
  param_1[6] = bVar1 ? uVar2 + iVar4 : 0;
  *param_1 = *param_1 | 1;
  return param_1;
}
