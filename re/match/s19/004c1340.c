// FLAGS: /O2 /GS /Ob1 /G5
/* MATCH-S19 0x004c1340 (h1): 100% (regs 100%), byte-identical, relocheck OK. RwFrame matrix op on frame+0x10 then RwFrameUpdateObjects inline; raw body with the flags byte held in a uint, callee typed void* for C++. FLAGS add /G5 (game flags: 82.5% raw). */
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
void *FUN_004c51a0(void *p0, void *p1, int p2);
// MATCH: 0x004c1340 FUN_004c1340
int FUN_004c1340(int param_1,undefined4 param_2,undefined4 param_3)

{
  uint bVar1;
  
  FUN_004c51a0((void *)(param_1 + 0x10),(void *)param_2,param_3);
  bVar1 = *(byte *)(*(int *)(param_1 + 0xa0) + 3);
  if ((bVar1 & 3) == 0) {
    *(undefined4 *)(*(int *)(param_1 + 0xa0) + 8) = *(undefined4 *)(DAT_007d3ff8 + 0xbc);
    *(int *)(*(int *)(param_1 + 0xa0) + 0xc) = DAT_007d3ff8 + 0xbc;
    *(int *)(*(int *)(DAT_007d3ff8 + 0xbc) + 4) = *(int *)(param_1 + 0xa0) + 8;
    *(int *)(DAT_007d3ff8 + 0xbc) = *(int *)(param_1 + 0xa0) + 8;
  }
  *(byte *)(*(int *)(param_1 + 0xa0) + 3) = (byte)(bVar1 | 3);
  *(byte *)(param_1 + 3) = *(byte *)(param_1 + 3) | 0xc;
  return param_1;
}
